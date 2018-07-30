//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// ConnectInfoPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConnectInfoPage dialog

class CConnectInfoPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CConnectInfoPage)

// Construction
public:
	CConnectInfoPage();
	~CConnectInfoPage();

// Dialog Data
	//{{AFX_DATA(CConnectInfoPage)
	enum { IDD = IDD_PAGE_CONNECTINFO };
	CButton	m_ReloadOnUncover;
	CButton	m_PollJobs;
	CButton	m_PollIconic;
	CButton	m_AutoPoll;
	int		m_AutoPollTime;
	int     m_ReloadUncoverTime;
	CComboBox m_PortCombo;
	CComboBox m_CharsetCombo;
	CButton	m_ShowConnectSettings;
	CButton	m_ShowConnectPort1st;
	CButton m_LogoutOnExit;
	//}}AFX_DATA

protected:
	CString	m_P4Port;
	CString	m_P4Charset;
	CString m_OrigClient;
	CString m_OrigUser;
	CString m_OrigPort;
	CString m_OrigCharset;
	int m_ErrorCount;
	BOOL m_Inited;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CConnectInfoPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void OnP4autopoll(); 
	void OnP4ReloadOnUncover();

	// Generated message map functions
	//{{AFX_MSG(CConnectInfoPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowConnectSettings();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL EditPort( );
	BOOL EditCharset( );
};
