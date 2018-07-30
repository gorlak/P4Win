//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// DiffAssocDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDiffAssocDlg dialog

class CDiffAssocDlg : public CDialog
{
// Construction
public:
	CDiffAssocDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDiffAssocDlg)
	enum { IDD = IDD_DIFFASSOCIATION };
	CString	m_Application;
	CString	m_Extension;
	//}}AFX_DATA
	BOOL m_EditMode;
	BOOL m_bMerge;

protected:
	CStringList m_ExtensionList;
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiffAssocDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ParseExtensions(CString &error);
	void UpdateRegistry();
	// Generated message map functions
	//{{AFX_MSG(CDiffAssocDlg)
	virtual void OnOK();
	afx_msg void OnBrowse();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

