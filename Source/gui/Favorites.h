#if !defined(AFX_FAVORITES_H__AA2C294B_59DF_49AA_8957_B9D9994BFA8B__INCLUDED_)
#define AFX_FAVORITES_H__AA2C294B_59DF_49AA_8957_B9D9994BFA8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Favorites.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFavorites dialog

class CFavorites : public CDialog
{
// Construction
public:
	CFavorites(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFavorites)
	enum { IDD = IDD_FAVORITES };
	CP4ListBox	m_MenuItemList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFavorites)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void LoadMenuItemList();
	int  m_CurSel;
	int  m_NbrSubMenus;
	int  m_1stSubmenu;

	// date from Favs Registry Key
	CString m_FavMenuName[MAX_FAVORITES];
	CString m_FavMenuPath[MAX_FAVORITES];
	BOOL m_FavIsSubMenu[MAX_FAVORITES];

	BOOL m_Canceling;

	// Generated message map functions
	//{{AFX_MSG(CFavorites)
	afx_msg void OnAdd();
	afx_msg void OnChangeSubmenu();
	afx_msg void OnMoveDown();
	afx_msg void OnMoveUp();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnImport();
	afx_msg void OnExport();
	virtual void OnOK();
	afx_msg void OnSelchangeMenulist();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnRemove();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FAVORITES_H__AA2C294B_59DF_49AA_8957_B9D9994BFA8B__INCLUDED_)
