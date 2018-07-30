//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// DisplayPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDisplayPage dialog

class CDisplayPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CDisplayPage)

// Construction
public:
	CDisplayPage();
	~CDisplayPage();

// Dialog Data
	//{{AFX_DATA(CDisplayPage)
	enum { IDD = IDD_PAGE_DISPLAY };
	CButton	m_AutoExpandOptions;
	int m_OptionStartUpOption;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDisplayPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_Inited;

	// Generated message map functions
	//{{AFX_MSG(CDisplayPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
