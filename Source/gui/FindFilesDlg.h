#if !defined(AFX_FINDFILESDLG_H__766760D3_D8EC_11D3_8328_009027AF6042__INCLUDED_)
#define AFX_FINDFILESDLG_H__766760D3_D8EC_11D3_8328_009027AF6042__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WinPos.h"

// FindFilesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFindFilesDlg dialog

class CFindFilesDlg : public CDialog
{
// Construction
public:
	CFindFilesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFindFilesDlg)
	enum { IDD = IDD_P4FILES };
	CP4ListBox m_List;
	CComboBox m_EditCombo;
	BOOL m_CloseAfterFind;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindFilesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	int m_ExitCode;
	void Init(CString &filespec,CStringList *filelist,CDepotTreeCtrl *depotView,BOOL bDeselect=FALSE);
	CString GetEditString( ) { return m_Edit; }
	CString GetListString( ) { return m_SelStr; }

protected:
	CWnd* m_pParent;
	CDepotTreeCtrl * m_pDepotView;
	CStringList * m_pList;
	CString m_Edit;
	CString m_SelStr;
	CWinPos m_WinPos;
	CRect m_InitRect;
	CRect m_LastRect;
	BOOL m_Deselect;
	BOOL m_InitDialogDone;
	BOOL m_IsMinimized;

	// Generated message map functions
	//{{AFX_MSG(CFindFilesDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnSetfocusEdit1();
	afx_msg void OnSetfocusList1();
	afx_msg void OnDblclkList();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnDepotFilter();
	afx_msg void OnCopy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	void CheckEditField();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDFILESDLG_H__766760D3_D8EC_11D3_8328_009027AF6042__INCLUDED_)
