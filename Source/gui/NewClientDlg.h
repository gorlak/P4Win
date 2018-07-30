#ifndef __NEWCLIENTDLG__
#define __NEWCLIENTDLG__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// Depot Filter Types
enum _dlgtype
{
	DLG_NEW,		// Dialogbox being used to get name of New object
	DLG_FILTER		// Dialogbox being used to get name of object to filter on
};

#define NEWCLIENT   "Client"
#define NEWUSER     "User"
#define NEWBRANCH   "Branch"
#define NEWLABEL    "Label"
#define NEWFILESPEC "Filespec"


/////////////////////////////////////////////////////////////////////////////
// CNewClientDlg dialog


class CNewClientDlg : public CDialog
{
// Construction
public:
	BOOL SwitchTo();
	void SetNew( const CString & );
	CString GetName();
	CNewClientDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewClientDlg)
	enum { IDD = IDD_CREATENEWCLIENT };
	CString	m_Name;
	BOOL m_RunWizard;
	//}}AFX_DATA

	int	m_Type;
	CString m_Active;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewClientDlg)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewClientDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString m_What;
};


#endif // __NEWCLIENTDLG__
