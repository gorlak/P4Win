//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// MergeAppPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMergeAppPage dialog

class CMergeAppPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CMergeAppPage)

// Construction
public:
	CMergeAppPage();
	~CMergeAppPage();
	BOOL OK2Cancel();

// Dialog Data
	//{{AFX_DATA(CMergeAppPage)
	enum { IDD = IDD_PAGE_MERGE };
	CButton	m_MergeNSF;
	CButton	m_P4MrgRadio;
	CButton	m_WinMrgRadio;
	CButton	m_OtherMrgRadio;
	CButton	m_MergeClose;
	CButton	m_MergeBrowse;
	CButton	m_MergeConsole;
	int m_P4MrgWhtSp;
	CString	m_MergeUserApp;
	int	m_MrgTabSize;
	CButton m_MergeOptArgChk;
	CString m_MergeOptArgs;
	CButton m_Radio1;
	CP4ListBox m_List;
	CButton	m_Edit;
	CButton	m_Delete;
	CButton	m_Add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMergeAppPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_Inited;
	BOOL m_Closing;

	BOOL ReadMergeAssocList();

	// Generated message map functions
	//{{AFX_MSG(CMergeAppPage)
	afx_msg void OnMrgP4Radio();
	virtual BOOL OnInitDialog();
	afx_msg void OnMergebrowse();
	afx_msg void OnMergeconsole();
	afx_msg void OnMergeOptArgChk();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnEdit();
	afx_msg void OnP4AssAppsSelChange();
	afx_msg void OnKillfocusArgs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

void Browse(CString &filename, LPCTSTR title);

};
