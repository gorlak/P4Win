#if !defined(AFX_JOBSCONFIGURE_H__7B277A53_076B_11D5_83CC_009027AF6042__INCLUDED_)
#define AFX_JOBSCONFIGURE_H__7B277A53_076B_11D5_83CC_009027AF6042__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// JobsConfigure.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CJobsConfigure dialog

class CJobsConfigure : public CDialog
{
// Construction
public:
	CJobsConfigure(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CJobsConfigure)
	enum { IDD = IDD_JOB_CONFIGURE };
	CP4ListBox	m_ListShow;
	CP4ListBox	m_ListOther;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJobsConfigure)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	CString	m_ColNames;
	CString	m_SpecNames;

protected:
	// Generated message map functions
	//{{AFX_MSG(CJobsConfigure)
	virtual BOOL OnInitDialog();
	afx_msg void OnSetToDefault();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	virtual void OnOK();
	afx_msg void OnSetfocusListOther();
	afx_msg void OnSetfocusListShow();
	afx_msg void OnSelchangeListShow();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnUp();
	afx_msg void OnDown();
	afx_msg void OnSelchangeListOther();
	afx_msg void OnDblclkListOther();
	afx_msg void OnFormInfo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	LRESULT OnP4Describe( WPARAM wParam, LPARAM lParam );
	LRESULT OnP4EndDescribe( WPARAM wParam, LPARAM lParam );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JOBSCONFIGURE_H__7B277A53_076B_11D5_83CC_009027AF6042__INCLUDED_)
