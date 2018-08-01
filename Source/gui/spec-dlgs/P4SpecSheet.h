#pragma once


// CP4SpecSheet dialog
#include "P4SpecDlg.h"

class CP4SpecSheet : public CDialog
{
	DECLARE_DYNAMIC(CP4SpecSheet)

public:
	CP4SpecSheet(CWnd* pParent = NULL);   // standard constructor
	virtual ~CP4SpecSheet();

	CP4SpecDlg m_P4SpecDlg;
	
// Dialog Data
	//{{AFX_DATA(CP4SpecSheet)
	enum { IDD = IDD_SPECSHEET };
	CButton	m_bHelpHelp;
	CButton	m_bHelp;
	CButton	m_bEditor;
	CButton	m_bOK;
	CButton	m_bAlternate;
	CButton	m_bCancel;
	CButton m_bReopen;
	int m_UnchangedFlag;
	//}}AFX_DATA

protected:
	CWnd *m_pParent;
	CWnd *m_LastFocus;
	int m_ScreenHeight;
	int m_BtnId[5];
	int m_Pad;
	BOOL m_bShowReopen;
	BOOL m_bShowRevertUnchg;

	// data for modeless edit and submit dialogs
 	BOOL m_IsMinimized;
	BOOL m_MainFRmSignaled;

	void ChgFont(int id);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CP4SpecSheet)
	public:
	virtual INT_PTR DoModal();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CP4SpecSheet)
	virtual BOOL OnInitDialog();
	afx_msg void OnEnter();
	afx_msg void On_OK();
	afx_msg void OnAlternate();
	virtual void On_Cancel();
	afx_msg void OnHelpnotes();
	afx_msg void OnEditor();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnReopen();
	afx_msg void OnRadioUnchg();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnClose();
 	afx_msg void OnDestroy();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	LRESULT OnEnableDisable( WPARAM wParam, LPARAM lParam );
	LRESULT OnModifyStyle( WPARAM wParam, LPARAM lParam );
	LRESULT OnP4EndHelpnotes(WPARAM wParam, LPARAM lParam);
	LRESULT OnQuitting(WPARAM wParam, LPARAM lParam);
	LRESULT OnForceFocus(WPARAM wParam, LPARAM lParam);
};
