#ifndef __GETPWDDLG__
#define __GETPWDDLG__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



/////////////////////////////////////////////////////////////////////////////
// CGetPwdDlg dialog

class CGetPwdDlg : public CDialog
{
// Construction
public:
	CGetPwdDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGetPwdDlg)
	enum { IDD = IDD_PASSWORD };
	BOOL	m_IsPermanent;
	CString	m_Password;
	//}}AFX_DATA

	CRect m_InitRect;
	CRect m_LastRect;
	BOOL m_InitDialogDone;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetPwdDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	CString GetPassword();
	BOOL IsWriteToRegistry( ){ return m_IsPermanent ; }

// Implementation
protected:

	UINT_PTR m_Timer;

	// Generated message map functions
	//{{AFX_MSG(CGetPwdDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __GETPWDDLG__
