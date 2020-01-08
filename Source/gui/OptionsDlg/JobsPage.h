//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// JobsPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CJobsPage dialog

class CJobsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CJobsPage)

// Construction
public:
	CJobsPage();
	~CJobsPage();

// Dialog Data
	//{{AFX_DATA(CJobsPage)
	enum { IDD = IDD_PAGE_JOBS };
	CButton	m_PollJobs;
	CButton	m_JobCountRadio;
	CButton m_JobsEnabledCheckbox;
	int		m_JobCount;
	//}}AFX_DATA

protected:
	int m_ErrorCount;
	BOOL m_Inited;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CJobsPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void OnFetchRadio(); 
	void OnJobConfigure();
	void OnEnableCheckbox();

	// Generated message map functions
	//{{AFX_MSG(CJobsPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
