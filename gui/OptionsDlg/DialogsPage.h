#pragma once


// CDialogsPage dialog

class CDialogsPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CDialogsPage)

public:
	CDialogsPage();
	virtual ~CDialogsPage();

// Dialog Data
	//{{AFX_DATA(CDialogsPage)
	enum { IDD = IDD_PAGE_DIALOGS };
	CButton m_ShowDiscard;
	CButton m_ShowNewClient;
	CButton m_ShowSyncClient;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDialogsPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	BOOL m_Inited;

	// Generated message map functions
	//{{AFX_MSG(CDialogsPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
