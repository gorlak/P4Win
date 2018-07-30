#if !defined(AFX_JOBDESCRIBE_H__65462BE5_76FF_11D5_8F31_009027AF6042__INCLUDED_)
#define AFX_JOBDESCRIBE_H__65462BE5_76FF_11D5_8F31_009027AF6042__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// JobDescribe.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CJobDescribe dialog

class CJobDescribe : public CDialog
{
// Construction
public:
	CJobDescribe(CWnd* pParent = NULL);   // standard constructor

	CString GetJobStr();


// Dialog Data
	//{{AFX_DATA(CJobDescribe)
	enum { IDD = IDD_JOB_DESCRIBE };
	CString	m_JobStr;
	BOOL m_IsConvertNbr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJobDescribe)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CJobDescribe)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JOBDESCRIBE_H__65462BE5_76FF_11D5_8F31_009027AF6042__INCLUDED_)
