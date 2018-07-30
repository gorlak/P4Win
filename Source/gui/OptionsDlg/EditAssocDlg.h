//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// EditAssocDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditAssocDlg dialog

class CEditAssocDlg : public CDialog
{
// Construction
public:
	CEditAssocDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditAssocDlg)
	enum { IDD = IDD_EDITASSOCIATION };
	CString	m_Application;
	CString	m_Extension;
	//}}AFX_DATA
	BOOL m_EditMode;

protected:
	CStringList m_ExtensionList;
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditAssocDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ParseExtensions(CString &error);
	void UpdateRegistry();
	// Generated message map functions
	//{{AFX_MSG(CEditAssocDlg)
	virtual void OnOK();
	afx_msg void OnBrowse();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
