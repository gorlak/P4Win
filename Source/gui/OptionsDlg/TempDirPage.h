//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// TempDirPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTempDirPage dialog

class CTempDirPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CTempDirPage)

// Construction
public:
	CTempDirPage();
	~CTempDirPage();

	int m_ErrorCount;

// Dialog Data
	//{{AFX_DATA(CTempDirPage)
	enum { IDD = IDD_PAGE_TEMPFILES };
	CString	m_TempPath;
	CString	m_AltExpl;
	CButton	m_Browse;
	CButton m_BrowseDir;
	int		m_Explorer;
	CButton	m_2Panes;
	int		m_DnDDefault;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTempDirPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_Inited;

	BOOL TestTempDir( LPCTSTR path );
	// Generated message map functions
	//{{AFX_MSG(CTempDirPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowse();
	afx_msg void OnBrowseDir();
	afx_msg void OnSetExplorer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
