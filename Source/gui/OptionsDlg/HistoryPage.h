//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// HistoryPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHistoryPage dialog

class CHistoryPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CHistoryPage)

// Construction
public:
	CHistoryPage();
	~CHistoryPage();

// Dialog Data
	//{{AFX_DATA(CHistoryPage)
	enum { IDD = IDD_PAGE_HISTORY };

	CButton	m_HistCountRadio;
	int		m_HistCount;
	CButton m_EnableRevHistShowIntegs;
	CButton m_UseShortRevHistDesc;
	//}}AFX_DATA

protected:
	int m_ErrorCount;
	BOOL m_Inited;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CHistoryPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void OnFetchRadio(); 

	// Generated message map functions
	//{{AFX_MSG(CHistoryPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
};
