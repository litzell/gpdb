//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal, Inc.
//
//	@filename:
//		CXformImplementDynamicBitmapTableGet
//
//	@doc:
//		Implement DynamicBitmapTableGet
//
//	@owner:
//
//
//	@test:
//
//---------------------------------------------------------------------------

#ifndef GPOPT_CXformImplementDynamicBitmapTableGet_H
#define GPOPT_CXformImplementDynamicBitmapTableGet_H

#include "gpos/base.h"

#include "gpopt/xforms/CXformImplementation.h"

namespace gpopt
{
//---------------------------------------------------------------------------
//	@class:
//		CXformImplementDynamicBitmapTableGet
//
//	@doc:
//		Implement CLogicalDynamicBitmapTableGet as a CPhysicalDynamicBitmapTableScan
//
//---------------------------------------------------------------------------
class CXformImplementDynamicBitmapTableGet : public CXformImplementation
{
private:
	CXformImplementDynamicBitmapTableGet(
		const CXformImplementDynamicBitmapTableGet &) = delete;

public:
	// ctor
	explicit CXformImplementDynamicBitmapTableGet(CMemoryPool *mp);

	// dtor
	virtual ~CXformImplementDynamicBitmapTableGet() = default;

	// identifier
	virtual EXformId
	Exfid() const
	{
		return ExfImplementDynamicBitmapTableGet;
	}

	// xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformImplementDynamicBitmapTableGet";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise
	Exfp(CExpressionHandle &  // exprhdl
	) const
	{
		return CXform::ExfpHigh;
	}

	// actual transform
	virtual void Transform(CXformContext *pxfctxt, CXformResult *pxfres,
						   CExpression *pexpr) const;

};	// class CXformImplementDynamicBitmapTableGet
}  // namespace gpopt

#endif	// !GPOPT_CXformImplementDynamicBitmapTableGet_H

// EOF
