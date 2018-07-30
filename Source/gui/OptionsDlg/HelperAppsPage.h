//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// HelperAppsPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHelperAppsPage dialog

class CHelperAppsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CHelperAppsPage)

// Construction
public:
	CHelperAppsPage();
	~CHelperAppsPage();
	BOOL OK2Cancel();

// Dialog Data
	//{{AFX_DATA(CHelperAppsPage)
	enum { IDD = IDD_PAGE_DIFF };
	CButton	m_DiffClose;
	CButton	m_DiffBinary;
	CButton m_DiffModal;
	CButton	m_DiffBrowse;
	CButton	m_DiffConsole;
	CButton	m_P4DiffRadio;
	CButton	m_WinDiffRadio;
	CButton	m_OtherDiffRadio;
	int	m_P4DiffWhtSp;
	CString	m_DiffUserApp;
	int	m_TabSize;
	CButton m_DiffOptArgChk;
	CString m_DiffOptArgs;
	CP4ListBox m_List;
	CButton	m_Edit;
	CButton	m_Delete;
	CButton	m_Add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CHelperAppsPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_Inited;
	BOOL m_Closing;

	BOOL ReadDiffAssocList();

	// Generated message map functions
	//{{AFX_MSG(CHelperAppsPage)
	afx_msg void OnDiffP4Radio();
	virtual BOOL OnInitDialog();
	afx_msg void OnDiffbrowse();
	afx_msg void OnDiffconsole();
	afx_msg void OnDiffOptArgChk();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnEdit();
	afx_msg void OnP4AssAppsSelChange();
	afx_msg void OnKillfocusArgs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

void Browse(CString &filename, LPCTSTR title);

};
