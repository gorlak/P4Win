#if !defined(AFX_BOOKMARKS_H__DCDB6405_7BA5_11D5_8F36_009027AF6042__INCLUDED_)
#define AFX_BOOKMARKS_H__DCDB6405_7BA5_11D5_8F36_009027AF6042__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// bookmarks.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBookmarks dialog

class CBookmarks : public CDialog
{
// Construction
public:
	CBookmarks(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBookmarks)
	enum { IDD = IDD_BOOKMARKS };
	CP4ListBox	m_MenuItemList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBookmarks)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void LoadMenuItemList();
	int  m_CurSel;
	int  m_NbrSubMenus;
	int  m_1stSubmenu;

	// date from BkMks Registry Key
	CString m_BkMkMenuName[MAX_BOOKMARKS];
	BOOL m_BkMkIsSubMenu[MAX_BOOKMARKS];

	BOOL m_Canceling;

	// Generated message map functions
	//{{AFX_MSG(CBookmarks)
	afx_msg void OnAddSubmenu();
	afx_msg void OnChangeItem();
	afx_msg void OnMoveDown();
	afx_msg void OnMoveUp();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	virtual void OnOK();
	afx_msg void OnSelchangeMenulist();
	afx_msg void OnImport();
	afx_msg void OnExport();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnRemove();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void OnChangeSubmenu();
	void OnChangeBkMkItem();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BOOKMARKS_H__DCDB6405_7BA5_11D5_8F36_009027AF6042__INCLUDED_)
