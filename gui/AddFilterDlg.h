#if !defined(AFX_ADDFILTERDLG1_H__4E2D5263_1D3E_11D4_834B_009027AF6042__INCLUDED_)
#define AFX_ADDFILTERDLG1_H__4E2D5263_1D3E_11D4_834B_009027AF6042__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddFilterDlg1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddFilterDlg dialog

class CAddFilterDlg : public CDialog
{
// Construction
public:
	CString m_Extensions;
	CStringList m_Exts;
	CAddFilterDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddFilterDlg)
	enum { IDD = IDD_ADDFILTER };
	CP4ListBox	m_ExtList;
	CString	m_Ext;
	int m_Exclude;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddFilterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void GetExtensions();

	// Generated message map functions
	//{{AFX_MSG(CAddFilterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddToFilter();
	afx_msg void OnSelchangeList();
	afx_msg void OnDelete();
	afx_msg void OnDeleteAll();
	afx_msg void OnRadio();
	afx_msg void OnOK();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDFILTERDLG1_H__4E2D5263_1D3E_11D4_834B_009027AF6042__INCLUDED_)
