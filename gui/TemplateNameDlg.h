/*
 * Copyright 1998 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */

// TemplateNameDlg.h : header file
//

#ifndef __SPECFROMTEMPLATE__
#define __SPECFROMTEMPLATE__

/////////////////////////////////////////////////////////////////////////////
// CTemplateNameDlg dialog

class CTemplateNameDlg : public CDialog
{
// Construction
public:
	CTemplateNameDlg(CWnd* pParent = NULL);   // standard constructor

protected:
// Dialog Data
	//{{AFX_DATA(CTemplateNameDlg)
	enum { IDD = IDD_TEMPLATENAME };
	CString	m_NewSpecName;
	CString	m_TemplateSpecName;
	//}}AFX_DATA

    CStringList *m_pVerbotenSpecNames;
	int m_SpecType;

public:
    void SetTemplateName(LPCTSTR name) { m_TemplateSpecName= name; }
	void SetSpecType( int specType ) { m_SpecType= specType; }
    void SetNewSpecName(LPCTSTR name) { m_NewSpecName= name; }
    LPCTSTR GetNewSpecName() { return LPCTSTR(m_NewSpecName); }
    void SetVerbotenSpecNames(CStringList *list) 
        { ASSERT_KINDOF(CStringList,list); m_pVerbotenSpecNames= list; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTemplateNameDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTemplateNameDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __SPECFROMTEMPLATE__
