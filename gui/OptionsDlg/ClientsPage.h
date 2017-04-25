//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// ClientsPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CClientsPage dialog

class CClientsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CClientsPage)

// Construction
public:
	CClientsPage();
	~CClientsPage();

// Dialog Data
	//{{AFX_DATA(CClientsPage)
	enum { IDD = IDD_PAGE_CLIENTS };
	CButton	m_ClearAndReload;
	BOOL	m_UseDepotInClientView;
	BOOL	m_LocalCliTemplateSw;
	BOOL	m_UseClientSpecSubmitOpts;
	//}}AFX_DATA

protected:
	int m_ErrorCount;
	BOOL m_Inited;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CClientsPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CClientsPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
};
