#if !defined(AFX_RERESOLVINGDLG_H__80E59F03_3CC3_11D4_835E_009027AF6042__INCLUDED_)
#define AFX_RERESOLVINGDLG_H__80E59F03_3CC3_11D4_835E_009027AF6042__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReresolvingDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CReresolvingDlg dialog

class CReresolvingDlg : public CDialog
{
// Construction
public:
	CReresolvingDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CReresolvingDlg)
	enum { IDD = IDD_RERESOLVING };
	CP4ListBox	m_ListBox;
	//}}AFX_DATA

	CStringList m_List;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReresolvingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CReresolvingDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RERESOLVINGDLG_H__80E59F03_3CC3_11D4_835E_009027AF6042__INCLUDED_)
