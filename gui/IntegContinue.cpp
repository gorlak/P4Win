// IntegContinue.cpp: implementation of the CIntegContinue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "p4win.h"
#include "IntegContinue.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIntegContinue::CIntegContinue()
{
}

//CIntegContinue::CIntegContinue(CIntegSheet dlg)
//{
//}

CIntegContinue::CIntegContinue(LPCTSTR reference, LPCTSTR revRange,
					LPCTSTR commonPath,
					BOOL isBranch, BOOL isReverse, BOOL isNoCopy,
					BOOL isForced, BOOL isForcedDirect, BOOL isRename, BOOL isPreview,
					int changeNum, BOOL isPermitDelReadd, int delReaddType,
					BOOL isBaselessMerge,  BOOL isIndirectMerge, 
					BOOL isPropagateTypes, BOOL isBaseViaDelReadd, 
					int branchFlag, BOOL bBiDir, int newChangeNbr)
{
	m_changeNum = changeNum;
	m_isPreview = isPreview;
	m_isRename  = isRename;
	m_isForced  = isForced;
	m_isForcedDirect  = isForcedDirect;
	m_isNoCopy  = isNoCopy;
	m_isReverse = isReverse;
	m_isBranch  = isBranch;
	m_commonPath= commonPath;
	m_revRange  = revRange;
	m_reference = reference;
	m_isPermitDelReadd = isPermitDelReadd;
	m_DelReaddType = delReaddType;
	m_isBaselessMerge = isBaselessMerge;
	m_isIndirectMerge = isIndirectMerge;
	m_isPropagateTypes = isPropagateTypes;
	m_isBaseViaDelReadd = isBaseViaDelReadd;
	m_BranchFlag = branchFlag;
	m_BiDir = bBiDir;
	m_NewChangeNbr = newChangeNbr;
}

CIntegContinue::~CIntegContinue()
{

}
