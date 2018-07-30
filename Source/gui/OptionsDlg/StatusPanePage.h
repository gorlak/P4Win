//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// StatusPanePage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStatusPanePage dialog

class CStatusPanePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CStatusPanePage)

// Construction
public:
	CStatusPanePage();
	~CStatusPanePage();

// Dialog Data
	//{{AFX_DATA(CStatusPanePage)
	enum { IDD = IDD_PAGE_STATUSPANE };
	CButton	m_ShowStatusTime;
	CButton m_Use24hourClock;
	CButton	m_ShowCommandTrace;
	CButton	m_ShowStatusMsgs;
	CButton	m_ShowTruncTooltip;
	CButton m_UseNotepad;
	int		m_MaxStatusLines;
	CButton m_AllowExceptionReporting;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CStatusPanePage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_Inited;

	// Generated message map functions
	//{{AFX_MSG(CStatusPanePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSettodefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
