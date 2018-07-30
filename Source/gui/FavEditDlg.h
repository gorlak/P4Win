#if !defined(AFX_FAVEDITDLG_H__31BF7FD3_21F8_11D4_8350_009027AF6042__INCLUDED_)
#define AFX_FAVEDITDLG_H__31BF7FD3_21F8_11D4_8350_009027AF6042__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FavEditDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFavEditDlg dialog

class CFavEditDlg : public CDialog
{
// Construction
public:
	CFavEditDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFavEditDlg)
	enum { IDD = IDD_FAVEDITDLG };
	CString m_Port;
	CString m_Client;
	CString m_User;
	CString m_Desc;
	int		m_MenuType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFavEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_Title;
	CString	m_Name;
	BOOL m_IsOK2Cr8SubMenu;
	BOOL m_RadioShow;	// 0=show both, 1=disable 1st, 2=disable 2nd, 3=hide both
	BOOL m_DescFocus;	// TRUE => Set the initial focus to Description field

public:
	CString GetNewMenuName() { return m_Name; }
	void SetNewMenuName(CString name) { m_Name = name; }
	void SetTitle(CString title) { m_Title = title; }
	void SetIsSubMenu(BOOL b) { m_MenuType = b ? 1 : 0; }
	BOOL GetIsSubMenu() { return m_MenuType == 1; }
	void SetCanCr8SubMenu(BOOL b) { m_IsOK2Cr8SubMenu = b; }
	void SetRadioShow(int x) { m_RadioShow = x; }
	void SetFocusDesc(BOOL b) { m_DescFocus = b ? 1 : 0; }
	
protected:

	// Generated message map functions
	//{{AFX_MSG(CFavEditDlg)
	afx_msg void OnCommand();
	afx_msg void OnSubMenu();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FAVEDITDLG_H__31BF7FD3_21F8_11D4_8350_009027AF6042__INCLUDED_)

