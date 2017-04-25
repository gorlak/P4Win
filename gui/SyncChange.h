#if !defined(AFX_SYNCCHANGE_H__37C94933_BCA5_11D3_830E_009027AF6042__INCLUDED_)
#define AFX_SYNCCHANGE_H__37C94933_BCA5_11D3_830E_009027AF6042__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SyncChange.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSyncChange dialog

class CSyncChange : public CDialog
{
// Construction
public:
	CSyncChange(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSyncChange)
	enum { IDD = IDD_CHANGE_SYNC };
	int		m_SyncDef;
	int		m_SyncType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSyncChange)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HWND m_OldChgWnd;

	// Generated message map functions
	//{{AFX_MSG(CSyncChange)
	afx_msg void OnPreview();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnSync();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYNCCHANGE_H__37C94933_BCA5_11D3_830E_009027AF6042__INCLUDED_)
