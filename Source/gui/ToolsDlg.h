#if !defined(AFX_TOOLSDLG_H__C4458913_21EF_11D4_8350_009027AF6042__INCLUDED_)
#define AFX_TOOLSDLG_H__C4458913_21EF_11D4_8350_009027AF6042__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CToolsDlg dialog

class CToolsDlg : public CDialog
{
// Construction
public:
	CToolsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CToolsDlg)
	enum { IDD = IDD_CUSTOMIZETOOLS };
	CP4ListBox	m_MenuItemList;
	CString	m_Command;
	BOOL	m_IsConsole;
	CString	m_InitDir;
	CString	m_PromptText;
	CString	m_Args;
	BOOL	m_IsPrompt;
	BOOL	m_IsOutput2Status;
	BOOL	m_IsCloseOnExit;
	BOOL	m_IsShowBrowse;
	BOOL	m_IsRefresh;
	BOOL	m_OnContext;
	CString	m_Name;
	int		m_MenuType;
	//}}AFX_DATA

	BOOL m_bEdit;
	BOOL m_OldbEdit;
	CString m_Caption;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void Clear();
	void GetVariables();
	LRESULT LoadVariables(WPARAM wParam = 0, LPARAM lParam = 0);
	void LoadMenuItemList();
	int  m_CurSel;
	int  m_NbrSubMenus;
	int  m_1stSubmenu;
	void SetCanCr8SubMenu(BOOL b) { m_IsOK2Cr8SubMenu = b; }
	void SetIsSubMenu(BOOL b) { m_MenuType = b ? 1 : 0; }
	BOOL GetIsSubMenu() { return m_MenuType == 1; }

	// date from Tools Registry Key
	CString m_ToolMenuName[MAX_TOOLS];
	CString m_ToolCommand[MAX_TOOLS];
	CString m_ToolArgs[MAX_TOOLS];
	CString m_ToolInitDir[MAX_TOOLS];
	CString m_ToolPromptText[MAX_TOOLS];
	BOOL m_ToolIsConsole[MAX_TOOLS];
	BOOL m_ToolIsPrompt[MAX_TOOLS];
	BOOL m_ToolIsOutput2Status[MAX_TOOLS];
	BOOL m_ToolIsCloseOnExit[MAX_TOOLS];
	BOOL m_ToolIsShowBrowse[MAX_TOOLS];
	BOOL m_ToolIsRefresh[MAX_TOOLS];
	BOOL m_ToolOnContext[MAX_TOOLS];
	BOOL m_ToolIsSubMenu[MAX_TOOLS];

	BOOL m_IsOK2Cr8SubMenu;
	BOOL m_Finished;

	// Generated message map functions
	//{{AFX_MSG(CToolsDlg)
	afx_msg void OnAdd();
	afx_msg void OnBrowse();
	afx_msg void OnBrowseDir();
	afx_msg void OnIsConsole();
	afx_msg void OnIsPrompt();
	afx_msg void OnMoveDown();
	afx_msg void OnMoveUp();
	afx_msg void OnRemove();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	virtual void OnOK();
	afx_msg void OnEdit();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeMenulist();
	afx_msg void OnDblclkMenuList();
	afx_msg void OnKillfocusArgs();
	afx_msg void OnKillfocusInitDir();
	afx_msg void OnMenuTypeClick();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLSDLG_H__C4458913_21EF_11D4_8350_009027AF6042__INCLUDED_)
