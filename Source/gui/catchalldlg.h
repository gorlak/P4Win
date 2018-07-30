#if !defined(AFX_CATCHALLDLG_H__840357D5_3BB7_11D2_80BB_0000B480D005__INCLUDED_)
#define AFX_CATCHALLDLG_H__840357D5_3BB7_11D2_80BB_0000B480D005__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// catchalldlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CJobFilter dialog

class CJobFilter : public CDialog
{
// Construction
public:
	CJobFilter(CWnd* pParent = NULL);   // standard constructor
	void SetFilterString( const CString &filter ) { m_Filter = filter; }
	CString GetFilterString ( ) { return m_Filter; }

	LONG m_top;
	LONG m_left;
	LONG m_right;
	CRect m_InitRect;

// Dialog Data
	//{{AFX_DATA(CJobFilter)
	enum { IDD = IDD_JOB_FILTER };
	CButton	m_RmvFilter;
	CButton	m_JobViewBtn;
	CButton	m_Help;
	CComboBox m_ChangeCombo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJobFilter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CString	m_Filter;

	LRESULT OnP4Describe( WPARAM wParam, LPARAM lParam );
	LRESULT OnP4EndDescribe( WPARAM wParam, LPARAM lParam );

	// Generated message map functions
	//{{AFX_MSG(CJobFilter)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnHelpJobFilter();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnRmvJobFilter();
	afx_msg void OnJobView();
	afx_msg void OnFormInfo();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnClearfilter();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void LoadFilterComboBox();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CATCHALLDLG_H__840357D5_3BB7_11D2_80BB_0000B480D005__INCLUDED_)
