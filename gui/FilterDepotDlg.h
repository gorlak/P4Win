#if !defined(AFX_FILTERDEPOTDLG_H__7BD93526_FEBC_11D4_83C8_009027AF6042__INCLUDED_)
#define AFX_FILTERDEPOTDLG_H__7BD93526_FEBC_11D4_83C8_009027AF6042__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FilterDepotDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// FilterDepotDlg dialog

class FilterDepotDlg : public CDialog
{
// Construction
public:
	FilterDepotDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(FilterDepotDlg)
	enum { IDD = IDD_DEPOT_FILTER };
	int		m_FilterType;
	CString	m_FileList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(FilterDepotDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(FilterDepotDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDepotList();
	afx_msg void OnAllOpened();
	afx_msg void OnMyOpened();
	afx_msg void OnClearfilter();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILTERDEPOTDLG_H__7BD93526_FEBC_11D4_83C8_009027AF6042__INCLUDED_)
