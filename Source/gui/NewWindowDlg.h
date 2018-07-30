#if !defined(AFX_NEWWINDOWDLG__H__605126A6_857D_11D3_A376_00105AC64526__INCLUDED_)
#define AFX_NEWWINDOWDLG__H__605126A6_857D_11D3_A376_00105AC64526__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewWindowDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewWindowDlg dialog

class CNewWindowDlg : public CDialog
{
// Construction
public:
	CNewWindowDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewWindowDlg)
	enum { IDD = IDD_NEW_WINDOW };
	CString	m_client;
	CString	m_user;
	CComboBox m_PCUcombo;
	CComboBox m_PortCombo;
	BOOL	m_SetDefClient;
	BOOL	m_SetDefPort;
	BOOL	m_SetDefUser;
	int		m_StartWith;
	//}}AFX_DATA

	CString	m_port;
	BOOL m_bSamePort;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewWindowDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CStringList	m_RmvMRUPcu;
	void LoadPortCombo();
	BOOL EditPort();

	// Generated message map functions
	//{{AFX_MSG(CNewWindowDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangePortclientuser();
	afx_msg void OnEditchangePortclientuser();
	afx_msg void OnSelchangePort();
	afx_msg void OnEditchangePort();
	afx_msg void OnSetfocusOther();
	afx_msg void OnOK();
	afx_msg void OnNewWindow();
	afx_msg void OnSetdefall();
	afx_msg void OnRemove();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnStartwithdefaults();
	afx_msg void OnStartwithmrupcu();
	afx_msg void OnBrowseClients();
	afx_msg void OnBrowseUsers();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	LRESULT OnP4ClientList(WPARAM wParam, LPARAM lParam);
	LRESULT OnBrowseClientsCallBack(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4UserList(WPARAM wParam, LPARAM lParam);
	LRESULT OnBrowseUsersCallBack(WPARAM wParam, LPARAM lParam);
	LRESULT OnDoBrowseClients(WPARAM wParam, LPARAM lParam);
	LRESULT OnDoBrowseUsers(WPARAM wParam, LPARAM lParam);

	void ReloadMRUPcu();
	int LoadClientList(CObList const *clients, CObList *objs, int nbrcols, BOOL bFilter);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWWINDOWDLG__H__605126A6_857D_11D3_A376_00105AC64526__INCLUDED_)
