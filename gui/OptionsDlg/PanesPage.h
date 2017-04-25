//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// PanesPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPanesPage dialog

class CPanesPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPanesPage)

// Construction
public:
	CPanesPage();
	~CPanesPage();

// Dialog Data
	//{{AFX_DATA(CPanesPage)
	enum { IDD = IDD_PAGE_PANES };
	CButton	m_UseAllIconColors;
	CButton m_SwapButtonPosition;
	CButton m_EnableSubChgIntegFilter;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPanesPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_Inited;

	// Generated message map functions
	//{{AFX_MSG(CPanesPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
