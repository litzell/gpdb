//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CXformImplementAssert.h
//
//	@doc:
//		Implement Assert
//---------------------------------------------------------------------------
#ifndef GPOPT_CXformImplementAssert_H
#define GPOPT_CXformImplementAssert_H

#include "gpos/base.h"
#include "gpopt/xforms/CXformImplementation.h"

namespace gpopt
{
using namespace gpos;

//---------------------------------------------------------------------------
//	@class:
//		CXformImplementAssert
//
//	@doc:
//		Implement Assert
//
//---------------------------------------------------------------------------
class CXformImplementAssert : public CXformImplementation
{
private:
	CXformImplementAssert(const CXformImplementAssert &) = delete;

public:
	// ctor
	explicit CXformImplementAssert(CMemoryPool *mp);

	// dtor
	virtual ~CXformImplementAssert() = default;

	// ident accessors
	virtual EXformId
	Exfid() const
	{
		return ExfImplementAssert;
	}

	// xform name
	virtual const CHAR *
	SzId() const
	{
		return "CXformImplementAssert";
	}

	// compute xform promise for a given expression handle
	virtual EXformPromise Exfp(CExpressionHandle &exprhdl) const;

	// actual transform
	virtual void Transform(CXformContext *, CXformResult *,
						   CExpression *) const;

};	// class CXformImplementAssert

}  // namespace gpopt

#endif	// !GPOPT_CXformImplementAssert_H

// EOF
