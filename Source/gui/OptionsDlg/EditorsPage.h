//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// EditorsPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditorsPage dialog

class CEditorsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CEditorsPage)

// Construction
public:
	CEditorsPage();
	~CEditorsPage();

// Dialog Data
	//{{AFX_DATA(CEditorsPage)
	enum { IDD = IDD_PAGE_EDITORS };
	CButton	m_Browse;
	CP4ListBox	m_List;
	CButton	m_Edit;
	CButton	m_Delete;
	CButton	m_Add;
	CButton	m_EditConsole;
	CButton	m_IgnoreAssoc;
	CString	m_DefaultEditApp;
	CButton m_UseOpenForEdit;
	CString m_UseOpenForEditExts;
	int m_UseTempForView;
	CString m_UseTempForExts;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CEditorsPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_Inited;

	BOOL ReadAssociationList() ;

	// Generated message map functions
	//{{AFX_MSG(CEditorsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnEdit();
	afx_msg void OnP4AssAppsSelChange();
	afx_msg void OnBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void OnUseTempClick();
	void OnEditUseView();
};
