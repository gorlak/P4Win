// IntegContinue.h: interface for the CIntegContinue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INTEGCONTINUE_H__28A999E1_6633_11D3_A35B_00105AC64526__INCLUDED_)
#define AFX_INTEGCONTINUE_H__28A999E1_6633_11D3_A35B_00105AC64526__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIntegContinue  
{
public:
	CIntegContinue();
	CIntegContinue( LPCTSTR reference, LPCTSTR revRange, LPCTSTR commonPath, 
					BOOL isBranch, BOOL isReverse, BOOL isNoCopy,
					BOOL isForced, BOOL isForcedDirect, BOOL isRename, BOOL isPreview,
					int changeNum, BOOL isPermitDelReadd, int delReaddType,
					BOOL isBaselessMerge,  BOOL isIndirectMerge, 
					BOOL isPropagateTypes, BOOL isBaseViaDelReadd, 
					int branchFlag, BOOL bBiDir, int newChangeNbr );
	virtual ~CIntegContinue();

	CString m_revRange;
	CString m_reference;
	CString m_commonPath;
	BOOL m_isPreview;
	BOOL m_isRename;
	BOOL m_isForced;
	BOOL m_isForcedDirect;
	BOOL m_isNoCopy;
	BOOL m_isReverse;
	BOOL m_isBranch;
	int  m_changeNum;
	BOOL m_isPermitDelReadd;
	int	 m_DelReaddType;
	BOOL m_isBaselessMerge;
	BOOL m_isIndirectMerge;
	BOOL m_isPropagateTypes;
	BOOL m_isBaseViaDelReadd;
	int  m_BranchFlag;
	BOOL m_BiDir;
	int m_NewChangeNbr;
};

#endif // !defined(AFX_INTEGCONTINUE_H__28A999E1_6633_11D3_A35B_00105AC64526__INCLUDED_)
