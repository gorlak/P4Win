#if !defined(AFX_REMOVEVIEWER_H__CEED12E3_6FD9_11D5_8F2B_009027AF6042__INCLUDED_)
#define AFX_REMOVEVIEWER_H__CEED12E3_6FD9_11D5_8F2B_009027AF6042__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RemoveViewer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRemoveViewer dialog

class CRemoveViewer : public CDialog
{
// Construction
public:
	CRemoveViewer(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRemoveViewer)
	enum { IDD = IDD_RMVVIEWER };
	CP4ListBox	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRemoveViewer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRemoveViewer)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REMOVEVIEWER_H__CEED12E3_6FD9_11D5_8F2B_009027AF6042__INCLUDED_)
