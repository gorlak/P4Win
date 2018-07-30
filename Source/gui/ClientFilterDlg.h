#ifndef __CLIENTFILTERDLG__
#define __CLIENTFILTERDLG__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



/////////////////////////////////////////////////////////////////////////////
// CClientFilterDlg dialog

class CClientFilterDlg : public CDialog
{
// Construction
public:
	CClientFilterDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CClientFilterDlg)
	enum { IDD = IDD_CLIENTFILTER };
	CButton m_IsOwner;
	CButton m_IsHost;
	CButton m_IsDesc;
	int     m_NotUser;
	int     m_NotMyHost;
	CString	m_Owner;
	CString	m_Host;
	CString	m_DescContains;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientFilterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:

// Implementation
protected:
	int	m_Flags;

	// Generated message map functions
	//{{AFX_MSG(CClientFilterDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnOwner();
	afx_msg void OnHost();
	afx_msg void OnDesc();
	afx_msg void OnBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	LRESULT OnBrowseUsersCallBack(WPARAM wParam, LPARAM lParam);
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CLIENTFILTERDLG__
