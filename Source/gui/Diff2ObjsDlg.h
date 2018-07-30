#if !defined(AFX_DIFF2OBJSDLG_H__D27203CD_1C9C_4CAE_B451_03F46CB17371__INCLUDED_)
#define AFX_DIFF2OBJSDLG_H__D27203CD_1C9C_4CAE_B451_03F46CB17371__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Diff2ObjsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDiff2ObjsDlg dialog

class CDiff2ObjsDlg : public CDialog
{
// Construction
public:
	CDiff2ObjsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDiff2ObjsDlg)
	enum { IDD = IDD_DIFF2OBJS };
	CString	m_Edit1;
	CString	m_Edit2;
	//}}AFX_DATA

	int m_Type;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiff2ObjsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDiff2ObjsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
//	afx_msg void OnHelp();
//	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void On1Browse();
	afx_msg void On2Browse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	LRESULT On1BrowseCallBack(WPARAM wParam, LPARAM lParam);
	LRESULT On2BrowseCallBack(WPARAM wParam, LPARAM lParam);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIFF2OBJSDLG_H__D27203CD_1C9C_4CAE_B451_03F46CB17371__INCLUDED_)
