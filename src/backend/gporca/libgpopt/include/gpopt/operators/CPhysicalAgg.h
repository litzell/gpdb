//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CPhysicalAgg.h
//
//	@doc:
//		Basic physical aggregate operator
//---------------------------------------------------------------------------
#ifndef GPOS_CPhysicalAgg_H
#define GPOS_CPhysicalAgg_H

#include "gpos/base.h"
#include "gpopt/base/CUtils.h"
#include "gpopt/operators/CLogicalGbAgg.h"
#include "gpopt/operators/CPhysical.h"


namespace gpopt
{
// fwd declaration
class CDistributionSpec;

//---------------------------------------------------------------------------
//	@class:
//		CPhysicalAgg
//
//	@doc:
//		Aggregate operator
//
//---------------------------------------------------------------------------
class CPhysicalAgg : public CPhysical
{
private:
	CPhysicalAgg(const CPhysicalAgg &) = delete;

	// array of grouping columns
	CColRefArray *m_pdrgpcr;

	// aggregate type (local / intermediate / global)
	COperator::EGbAggType m_egbaggtype;

	BOOL m_isAggFromSplitDQA;

	CLogicalGbAgg::EAggStage m_aggStage;

	// compute required distribution of the n-th child of an intermediate aggregate
	CDistributionSpec *PdsRequiredIntermediateAgg(CMemoryPool *mp,
												  ULONG ulOptReq) const;

	// compute required distribution of the n-th child of a global aggregate
	CDistributionSpec *PdsRequiredGlobalAgg(CMemoryPool *mp,
											CExpressionHandle &exprhdl,
											CDistributionSpec *pdsInput,
											ULONG child_index,
											CColRefArray *pdrgpcrGrp,
											CColRefArray *pdrgpcrGrpMinimal,
											ULONG ulOptReq) const;

	// compute a maximal hashed distribution using the given columns,
	// if no such distribution can be created, return a Singleton distribution
	static CDistributionSpec *PdsMaximalHashed(CMemoryPool *mp,
											   CColRefArray *colref_array);

protected:
	// array of minimal grouping columns based on FDs
	CColRefArray *m_pdrgpcrMinimal;

	// could the local / intermediate / global aggregate generate
	// duplicate values for the same group across segments
	BOOL m_fGeneratesDuplicates;

	// array of columns used in distinct qualified aggregates (DQA)
	// used only in the case of intermediate aggregates
	CColRefArray *m_pdrgpcrArgDQA;

	// is agg part of multi-stage aggregation
	BOOL m_fMultiStage;

	// should distribution enforcement be enabled on this agg?
	// By default, global and local aggregate are created with same
	// grouping columns. In such cases, if local derives the same
	// distribution as global then we need no motion in-between, which
	// implies that a single aggregate is enough. Hence, such plans are
	// prohibited. In CXformEagerAgg, however, the local agg is created
	// with different grouping columns but can have the same
	// distribution as the global. We don't need to prohibit such plans,
	// since the global agg is applied with different grouping columns
	// compared to to the local and is still necessary.
	BOOL m_should_enforce_distribution;

	// compute required columns of the n-th child
	CColRefSet *PcrsRequiredAgg(CMemoryPool *mp, CExpressionHandle &exprhdl,
								CColRefSet *pcrsRequired, ULONG child_index,
								CColRefArray *pdrgpcrGrp);

	// compute required distribution of the n-th child
	CDistributionSpec *PdsRequiredAgg(CMemoryPool *mp,
									  CExpressionHandle &exprhdl,
									  CDistributionSpec *pdsInput,
									  ULONG child_index, ULONG ulOptReq,
									  CColRefArray *pdrgpcgGrp,
									  CColRefArray *pdrgpcrGrpMinimal) const;

public:
	// ctor
	CPhysicalAgg(CMemoryPool *mp, CColRefArray *colref_array,
				 CColRefArray *pdrgpcrMinimal,	// FD's on grouping columns
				 COperator::EGbAggType egbaggtype, BOOL fGeneratesDuplicates,
				 CColRefArray *pdrgpcrArgDQA, BOOL fMultiStage,
				 BOOL isAggFromSplitDQA, CLogicalGbAgg::EAggStage aggStage,
				 BOOL should_enforce_distribution);

	// is this agg generated by CXformSplitDQA
	BOOL IsAggFromSplitDQA() const;

	// is this part of Two Stage Scalar DQA
	BOOL IsTwoStageScalarDQA() const;

	// is this part of Three Stage Scalar DQA
	BOOL IsThreeStageScalarDQA() const;

	// dtor
	virtual ~CPhysicalAgg();

	// does this aggregate generate duplicate values for the same group
	virtual BOOL
	FGeneratesDuplicates() const
	{
		return m_fGeneratesDuplicates;
	}

	virtual const CColRefArray *
	PdrgpcrGroupingCols() const
	{
		return m_pdrgpcr;
	}

	// array of columns used in distinct qualified aggregates (DQA)
	virtual const CColRefArray *
	PdrgpcrArgDQA() const
	{
		return m_pdrgpcrArgDQA;
	}

	// aggregate type
	COperator::EGbAggType
	Egbaggtype() const
	{
		return m_egbaggtype;
	}

	// is a global aggregate?
	BOOL
	FGlobal() const
	{
		return (COperator::EgbaggtypeGlobal == m_egbaggtype);
	}

	// is agg part of multi-stage aggregation
	BOOL
	FMultiStage() const
	{
		return m_fMultiStage;
	}

	// match function
	virtual BOOL Matches(COperator *pop) const;

	// hash function
	virtual ULONG HashValue() const;

	// sensitivity to order of inputs
	virtual BOOL
	FInputOrderSensitive() const
	{
		return true;
	}

	//-------------------------------------------------------------------------------------
	// Required Plan Properties
	//-------------------------------------------------------------------------------------

	// compute required output columns of the n-th child
	virtual CColRefSet *PcrsRequired(
		CMemoryPool *mp, CExpressionHandle &exprhdl, CColRefSet *pcrsRequired,
		ULONG child_index, CDrvdPropArray *pdrgpdpCtxt, ULONG ulOptReq);

	// compute required ctes of the n-th child
	virtual CCTEReq *PcteRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
								  CCTEReq *pcter, ULONG child_index,
								  CDrvdPropArray *pdrgpdpCtxt,
								  ULONG ulOptReq) const;

	// compute required distribution of the n-th child
	virtual CDistributionSpec *
	PdsRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
				CDistributionSpec *pdsRequired, ULONG child_index,
				CDrvdPropArray *,  //pdrgpdpCtxt,
				ULONG ulOptReq) const
	{
		return PdsRequiredAgg(mp, exprhdl, pdsRequired, child_index, ulOptReq,
							  m_pdrgpcr, m_pdrgpcrMinimal);
	}

	// compute required rewindability of the n-th child
	virtual CRewindabilitySpec *PrsRequired(CMemoryPool *mp,
											CExpressionHandle &exprhdl,
											CRewindabilitySpec *prsRequired,
											ULONG child_index,
											CDrvdPropArray *pdrgpdpCtxt,
											ULONG ulOptReq) const;

	// check if required columns are included in output columns
	virtual BOOL FProvidesReqdCols(CExpressionHandle &exprhdl,
								   CColRefSet *pcrsRequired,
								   ULONG ulOptReq) const;


	// compute required partition propagation of the n-th child
	virtual CPartitionPropagationSpec *PppsRequired(
		CMemoryPool *mp, CExpressionHandle &exprhdl,
		CPartitionPropagationSpec *pppsRequired, ULONG child_index,
		CDrvdPropArray *pdrgpdpCtxt, ULONG ulOptReq);

	//-------------------------------------------------------------------------------------
	// Derived Plan Properties
	//-------------------------------------------------------------------------------------

	// derive distribution
	virtual CDistributionSpec *PdsDerive(CMemoryPool *mp,
										 CExpressionHandle &exprhdl) const;

	// derive rewindability
	virtual CRewindabilitySpec *PrsDerive(CMemoryPool *mp,
										  CExpressionHandle &exprhdl) const;

	// derive partition index map
	virtual CPartIndexMap *
	PpimDerive(CMemoryPool *,  // mp
			   CExpressionHandle &exprhdl,
			   CDrvdPropCtxt *	//pdpctxt
	) const
	{
		return PpimPassThruOuter(exprhdl);
	}

	// derive partition filter map
	virtual CPartFilterMap *
	PpfmDerive(CMemoryPool *,  // mp
			   CExpressionHandle &exprhdl) const
	{
		return PpfmPassThruOuter(exprhdl);
	}

	//-------------------------------------------------------------------------------------
	// Enforced Properties
	//-------------------------------------------------------------------------------------

	// return distribution property enforcing type for this operator
	virtual CEnfdProp::EPropEnforcingType EpetDistribution(
		CExpressionHandle &exprhdl, const CEnfdDistribution *ped) const;

	// return rewindability property enforcing type for this operator
	virtual CEnfdProp::EPropEnforcingType EpetRewindability(
		CExpressionHandle &,		// exprhdl
		const CEnfdRewindability *	// per
	) const;

	// return true if operator passes through stats obtained from children,
	// this is used when computing stats during costing
	virtual BOOL
	FPassThruStats() const
	{
		return false;
	}

	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------

	// conversion function
	static CPhysicalAgg *
	PopConvert(COperator *pop)
	{
		GPOS_ASSERT(CUtils::FPhysicalAgg(pop));

		return dynamic_cast<CPhysicalAgg *>(pop);
	}

	// debug print
	virtual IOstream &OsPrint(IOstream &os) const;

};	// class CPhysicalAgg

}  // namespace gpopt


#endif	// !GPOS_CPhysicalAgg_H

// EOF
