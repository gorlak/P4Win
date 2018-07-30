#ifndef __SETPWDDLG__
#define __SETPWDDLG__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetPwdDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetPwdDlg dialog

class CSetPwdDlg : public CDialog
{
// Construction
public:
	CSetPwdDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetPwdDlg)
	enum { IDD = IDD_SETPWD };
	CString	m_NewPwd;
	CString	m_NewPwd2;
	CString	m_OldPwd;
	BOOL	m_RememberPSW;
	//}}AFX_DATA

	CString m_Caption;
	BOOL	m_bLogin;
	int		m_Key;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetPwdDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetPwdDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __SETPWDDLG__
