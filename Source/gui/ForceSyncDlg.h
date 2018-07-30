#if !defined(AFX_FORCESYNCDLG_H__75519DB6_25CB_11D4_8353_009027AF6042__INCLUDED_)
#define AFX_FORCESYNCDLG_H__75519DB6_25CB_11D4_8353_009027AF6042__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ForceSyncDlg.h : header file
//

#include "WinPos.h"

/////////////////////////////////////////////////////////////////////////////
// CForceSyncDlg dialog

class CForceSyncDlg : public CDialog
{
// Construction
public:
	CForceSyncDlg(CWnd* pParent = NULL);   // standard constructor

	CStringList *m_lpCstrListC;
	CStringList *m_lpCstrListD;
	CStringList m_pChangeList;
	CStringList m_StringList;
	CString m_SelChange;
	int m_SelectedChange;
	int m_Key;
	CWinPos m_WinPos;
	CRect m_InitRect;
	CRect m_LastRect;
	BOOL m_InitDialogDone;

// Dialog Data
	//{{AFX_DATA(CForceSyncDlg)
	enum { IDD = IDD_FORCESYNC };
	CComboBox	m_ChangeCombo;
	CButton	m_OK;
	CButton	m_Delete;
	CP4ListBox	m_List;
	int		m_Action;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CForceSyncDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void OnDiffFile(BOOL bHave);

	// Generated message map functions
	//{{AFX_MSG(CForceSyncDlg)
	afx_msg void OnDeleteSelected();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeList();
	afx_msg void OnOpenFiles();
	afx_msg void OnForceResync();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnDiffHead();
	afx_msg void OnDiffHave();
	afx_msg void OnPositionDepot();
	afx_msg void OnFileRevisionhistory();
	afx_msg void OnEditSelectAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	LRESULT OnP4Diff(WPARAM wParam, LPARAM lParam);
};

void OnRightClickList(UINT flags, CPoint pt);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORCESYNCDLG_H__75519DB6_25CB_11D4_8353_009027AF6042__INCLUDED_)
