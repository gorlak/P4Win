//
// 
// Copyright 1999 Perforce Software.  All rights reserved.
//
// This file is part of Perforce - the FAST SCM System.
//
//


#ifndef __INTEGOPTIONSPAGE__
#define __INTEGOPTIONSPAGE__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsPage.h : header file
//

enum IntegrateType
{
	INTEG_USING_FILE_SPEC,
	INTEG_USING_BRANCH_SPEC,
	INTEG_USING_BRANCH
};

/////////////////////////////////////////////////////////////////////////////
// CIntegOptionsPage dialog

class CIntegOptionsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CIntegOptionsPage)

// Construction
public:
	CIntegOptionsPage();
	~CIntegOptionsPage();

// Dialog Data
	//{{AFX_DATA(CIntegOptionsPage)
	enum { IDD = IDD_PAGE_INTEGOPTIONS };
	BOOL	m_IsNoCopy;
	BOOL	m_IsForceInteg;
	BOOL	m_IsForceDirect;
	BOOL	m_IsDeleteSource;
	BOOL	m_IsMappingReverse;
	CString	m_FileSpecs;
	BOOL	m_IsPermitDelReadd;
	int		m_DelReaddType;
	BOOL	m_IsBaselessMerge;
	BOOL	m_IsIndirectMerge;
	BOOL	m_IsPropagateTypes;
	BOOL	m_IsBaseViaDelReadd;
	BOOL	m_UseNewChglist;
	BOOL	m_UseNewChglist2;
	BOOL	m_IsSyncFirst;
	//}}AFX_DATA

	BOOL m_IsDefOpts;
	BOOL m_Initialized;
	BOOL m_IsBranch;
	int  m_BranchFlag;

public:
	LPCTSTR GetFileSpecs() { return LPCTSTR(m_FileSpecs); }
	void SetIsNoCopy( BOOL IsNoCopy ) { m_IsNoCopy = IsNoCopy; }
	void SetIsSyncFirst( BOOL IsSyncFirst ) { m_IsSyncFirst = IsSyncFirst; }
	void SetIsPermitDelReadd( BOOL IsPermitDelReadd ) { m_IsPermitDelReadd = IsPermitDelReadd; }
	void SetDelReaddType( int delReaddType ) { m_DelReaddType = delReaddType; }
	void SetIsBaselessMerge( BOOL IsBaselessMerge ) { m_IsBaselessMerge = IsBaselessMerge; }
	void SetIsIndirectMerge( BOOL IsIndirectMerge ) { m_IsIndirectMerge = IsIndirectMerge; }
	void SetIsPropagateTypes( BOOL IsPropagateTypes ) { m_IsPropagateTypes = IsPropagateTypes; }
	void SetIsBaseViaDelReadd( BOOL IsBaseViaDelReadd ) { m_IsBaseViaDelReadd = IsBaseViaDelReadd; }
	void SetIsForceInteg( BOOL IsForceInteg ) { m_IsForceInteg = IsForceInteg; }
	void SetIsForceDirect( BOOL IsForceDirect ) { m_IsForceDirect = IsForceDirect; }
	void SetIsDeleteSource( BOOL IsDeleteSource ) { m_IsDeleteSource = IsDeleteSource; }
	void SetIsMappingReverse( BOOL IsMappingReverse ) { m_IsMappingReverse = IsMappingReverse; }
	void SetFileSpecs( CString specs ) { m_FileSpecs = specs; }

	BOOL IsNoCopy() { return m_IsNoCopy; }
	BOOL IsSyncFirst() { return m_IsSyncFirst; }
	BOOL IsPermitDelReadd() { return m_IsPermitDelReadd; }
	int  DelReaddType() { return m_DelReaddType; }
	BOOL IsBaselessMerge() { return m_IsBaselessMerge; }
	BOOL IsIndirectMerge() { return m_IsIndirectMerge; }
	BOOL IsPropagateTypes() { return m_IsPropagateTypes; }
	BOOL IsBaseViaDelReadd() { return m_IsBaseViaDelReadd; }
	BOOL IsForceInteg() { return m_IsForceInteg; }
	BOOL IsForceDirect() { return m_IsForceDirect; }
	BOOL IsDeleteSource() { return m_IsDeleteSource; }
	BOOL IsMappingReverse() { return m_IsMappingReverse; }
	
	void SetIsDefOpts( BOOL isDefOpts ) { m_IsDefOpts= isDefOpts; }
	void SetIsBranch( BOOL isBranch ) { m_IsBranch= isBranch; }
	void SetBranchFlag( int branchFlag ) { m_BranchFlag= branchFlag; }

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CIntegOptionsPage)
	public:
	virtual int DoModal();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_ErrorCount;

	// Generated message map functions
	//{{AFX_MSG(CIntegOptionsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnAllfiles();
	afx_msg void OnFilesubset();
	afx_msg void OnDelReadd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __INTEGOPTIONSPAGE__
