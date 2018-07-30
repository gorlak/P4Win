#if !defined(AFX_DELETEFIXES_H__CAE3D423_BCE4_11D5_8F53_009027AF6042__INCLUDED_)
#define AFX_DELETEFIXES_H__CAE3D423_BCE4_11D5_8F53_009027AF6042__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DeleteFixes.h : header file
//

#include "WinPos.h"

/////////////////////////////////////////////////////////////////////////////
// CDeleteFixes dialog

class CDeleteFixes : public CDialog
{
// Construction
public:
	CDeleteFixes(CWnd* pParent = NULL);   // standard constructor

protected:
	CWnd* m_pParent;
	HWND m_OldChgWnd;
	CString m_Active;
	CString m_Caption;
	CReviewList *m_List;
	int m_SortFlag;				// Bits: 1=>some files checked; 2=>some file not checked
	CStringList * m_SelectedList;
	CStringList * m_OutputList;
	CString *m_JobStatusValue;
	CString *m_pJobSpec;
	CWinPos m_WinPos;
	CRect m_InitRect;
	CRect m_LastRect;
	BOOL m_InitDialogDone;
	BOOL m_IsMinimized;
	UINT m_ReturnMsg;

// Dialog Data
	//{{AFX_DATA(CDeleteFixes)
	enum { IDD = IDD_DELETEFIXES };
	CComboBox	m_JobStatus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeleteFixes)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDeleteFixes();
	void SetActive(LPCTSTR txt) { m_Active= txt; }
	void SetCaption(LPCTSTR txt) { m_Caption= txt; }
	void SetJobSpec(CString *jobSpec) { m_pJobSpec= jobSpec; }
	void SetSelected(CStringList *plist) { m_SelectedList = plist; }
	void SetOutputList(CStringList *plist) { m_OutputList = plist; }
	void SetOutputStatus(CString *pstr) { m_JobStatusValue = pstr; }
	void SetReturnMsg(UINT msg) { m_ReturnMsg = msg; }

protected:

	void LoadFixes();
	BOOL LoadJobStatusComboBox();

	// Generated message map functions
	//{{AFX_MSG(CDeleteFixes)
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnDescribeJob();
	afx_msg void OnSelchangeList();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnCancel();
	afx_msg void OnSelectAll();
	afx_msg void OnUnselectAll();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELETEFIXES_H__CAE3D423_BCE4_11D5_8F53_009027AF6042__INCLUDED_)
