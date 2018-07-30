//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// DepotPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDepotPage dialog

class CDepotPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CDepotPage)

// Construction
public:
	CDepotPage();
	~CDepotPage();

	int m_ErrorCount;

// Dialog Data
	//{{AFX_DATA(CDepotPage)
	enum { IDD = IDD_PAGE_DEPOT };
	CButton	m_ShowTypes;
	CButton	m_ShowHidden;
	CButton	m_ShowDeleted;
	CButton	m_ShowHighLite;
	CButton	m_ShowClientPath;
	CButton m_Diff2InDialog;
	CButton m_AutoTreeExpand;
	CString	m_ExpandPath;
	int		m_ExpandFlag;
	CString	m_ExtSortMax;
	CString	m_BusyWaitTime;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDepotPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_Inited;

	// Generated message map functions
	//{{AFX_MSG(CDepotPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowclientpath();
	afx_msg void OnFetchRadio();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
