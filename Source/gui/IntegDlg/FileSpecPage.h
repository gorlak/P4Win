//
// 
// Copyright 1999 Perforce Software.  All rights reserved.
//
// This file is part of Perforce - the FAST SCM System.
//
//

#ifndef __INTEGFILESPECPAGE__
#define __INTEGFILESPECPAGE__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileSpecPage.h : header file
//

#include "WinPos.h"
#include "P4EditBox.h"

enum ShowMode
{
	BRANCH_FROMSOURCE= 1,
	BRANCH_FROMTARGET,
	FILESPEC
};
	

/////////////////////////////////////////////////////////////////////////////
// CIntegFileSpecPage dialog

class CIntegFileSpecPage : public CDialog
{
// Construction
public:
	CIntegFileSpecPage(CWnd* pParent = NULL);   // standard constructor
	~CIntegFileSpecPage();

protected:
// Dialog Data
	//{{AFX_DATA(CIntegFileSpecPage)
	enum { IDD = IDD_PAGE_INTEGFILESPECS };
	CString	m_SourceSpecs;
	CString	m_TargetSpecs;
	CString	m_TargetSpecsAlt;
	CComboBox	m_ChangeCombo;
	BOOL	m_IsPreview;
	BOOL	m_IsNoCopy;
	BOOL	m_IsPermitDelReadd;
	int		m_DelReaddType;
	BOOL	m_IsBaselessMerge;
	BOOL	m_IsIndirectMerge;
	BOOL	m_IsPropagateTypes;
	BOOL	m_IsBaseViaDelReadd;
	BOOL	m_IsForceInteg;
	BOOL	m_IsForceDirect;
	BOOL	m_IsDeleteSource;
	BOOL	m_IsAutoSync;
	BOOL	m_IsMappingReverse;
	BOOL	m_ReverseInteg;
	CString	m_BranchSpecs;
	//}}AFX_DATA

	CP4EditBox m_targetspecs;

	CWinPos m_WinPos;
	CRect m_InitRect;
	CRect m_LastRect;

	int   m_NewChangeNbr;
	CRect m_RectLabelSource;
	CRect m_RectSource;
	CRect m_RectSourceRadio;
	CRect m_RectLabelTarget;
	CRect m_RectTarget;
	CRect m_RectTargetRadio;
	CRect m_RectBranch;

	BOOL m_IsMinimized;
	BOOL m_LastSelectedTarget;

	CString m_CommonPath;
	CString m_2FilesOnLine;
	CStringList m_SourceSpecList;
	CStringList m_TargetSpecList;
	CStringList *m_pChangeList;
	int m_ChangeNum;

	void UpdateChangesControl();
	void CIntegFileSpecPage::LoadBranchView();
	void PutListInText( CString &text, CStringList *list );
	void PutTextInList( CString &text, CStringList *list );
	void ShowAlternateEditControls( ShowMode mode );
	BOOL OnSetInactive();

	BOOL m_SetBranch;
	BOOL m_Initialized;
	BOOL m_IsBranch;
	BOOL m_IsRename;
	BOOL m_Previewing;
	BOOL m_IsSourceSpecified;
	BOOL m_HaveSwappedAlready;
	BOOL m_IsChgListInteg;
	BOOL m_BiDir;
	int  m_BranchFlag;
	int  m_SaveBranchFlag;
	CString m_BranchName;
	CString m_RevRange;
	CString	m_RevRangeFrom;
	CString	m_RevRangeTo;
	int m_ChangeNbr;
	int	m_RevRangeFromFlag;
	int	m_RevRangeToFlag;
	int	m_RevRangeFromType;
	int	m_RevRangeToType;

	HWND m_DepotWnd;
	
public:
	void SetIsRename( BOOL isRename );
	void SetIsChgListInteg (BOOL isChgListInteg) { m_IsChgListInteg = isChgListInteg; }
	void SetCommonPath( LPCTSTR path ) { m_CommonPath = path; }
	void SetIsBranch( BOOL isBranch ) { m_SetBranch= TRUE; m_IsBranch= isBranch; }
	void SetBranchFlag( int branchFlag ) { m_SaveBranchFlag = m_BranchFlag = branchFlag; }
	void SetBranchMode( BOOL isSourceSpecified );
	void SetBranchName( LPCTSTR branchName ) { m_BranchName = branchName; }
	void SetChangeNbr( int changeList ) { m_ChangeNbr = changeList; }
	void SetSourceSpecs( CStringList *list );
	void SetTargetSpecs( CStringList *list );
	void SetDepotWnd(HWND depotWnd) { ASSERT(IsWindow(depotWnd)); m_DepotWnd=depotWnd; }
	BOOL SetSpecList( CStringList *list );
	LPCTSTR GetReference();
	LPCTSTR GetCommonPath() { return m_CommonPath; }
	LPCTSTR GetBranchName() { return LPCTSTR( m_BranchName ); }
	BOOL IsBranch() { return m_IsBranch; }
	BOOL IsRename() { return IsDeleteSource(); }
	BOOL IsForced() { return IsForceInteg(); }
	BOOL IsForcedDirect() { return IsForceDirect(); }
	BOOL IsReverse() { return IsMappingReverse(); }
	int  GetBranchFlag() { return m_BranchFlag; }
	BOOL GetBiDirFlag() { return m_BiDir; }
	int GetNewChangeNbr() { return m_NewChangeNbr; }
	void ClrNewChangeNbr() { m_NewChangeNbr = 0; }
	CStringList *GetSourceList();
	CStringList *GetTargetList();

	BOOL IsPreview() { return m_IsPreview; }
	void SetChangesList( CStringList *list );
	int GetChangeNum();

	LPCTSTR GetRevRange() { return LPCTSTR(m_RevRange); }

	BOOL IsNoCopy() { return m_IsNoCopy; }
	BOOL IsPermitDelReadd() { return m_IsPermitDelReadd; }
	int  DelReaddType() { return m_DelReaddType; }
	BOOL IsBaselessMerge() { return m_IsBaselessMerge; }
	BOOL IsIndirectMerge() { return m_IsIndirectMerge; }
	BOOL IsPropagateTypes() { return m_IsPropagateTypes; }
	BOOL IsBaseViaDelReadd() { return m_IsBaseViaDelReadd; }
	BOOL IsForceInteg() { return m_IsForceInteg; }
	BOOL IsForceDirect() { return m_IsForceDirect; }
	BOOL IsDeleteSource() { return m_IsDeleteSource; }
	BOOL IsAutoSync() { return m_IsAutoSync; }
	BOOL IsMappingReverse() { return m_IsMappingReverse; }

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CIntegFileSpecPage)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CIntegFileSpecPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdvancedOptions();
	afx_msg void OnSourceradio();
	afx_msg void OnTargetradio();
	afx_msg void OnReverseInteg();
	afx_msg void OnPreview();
	afx_msg void OnWizardBack();
	afx_msg void OnWizardCancel();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDestroy();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	BOOL CheckForAllOK();
	LRESULT OnGotoDlgCtrl(WPARAM wParam, LPARAM lParam);
	LRESULT OnEnableDisableButtons(WPARAM wParam, LPARAM lParam);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __INTEGFILESPECPAGE__
