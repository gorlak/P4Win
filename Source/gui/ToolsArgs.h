#if !defined(AFX_TOOLSARGS_H__31BF7FD3_21F8_11D4_8350_009027AF6042__INCLUDED_)
#define AFX_TOOLSARGS_H__31BF7FD3_21F8_11D4_8350_009027AF6042__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolsArgs.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CToolsArgs dialog

class CToolsArgs : public CDialog
{
// Construction
public:
	CToolsArgs(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CToolsArgs)
	enum { IDD = IDD_CUSTOMIZEARGS };
	CButton	m_OkBtn;
	CComboBox	m_ArgsCombo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolsArgs)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_Title;
	CString m_Args;
	CString m_StatusMsg;
	BOOL m_ShowBrowse;
	CString m_InitDir;


public:
	CString GetArgs() { return m_Args; }
	void SetTitle(CString title) { m_Title = title; }
	void SetStatusText(CString statusmsg) { m_StatusMsg = statusmsg; }
	void SetShowBrowse(BOOL showBrowse) { m_ShowBrowse = showBrowse; }
	
protected:

	// Generated message map functions
	//{{AFX_MSG(CToolsArgs)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void GetArgsValue();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLSARGS_H__31BF7FD3_21F8_11D4_8350_009027AF6042__INCLUDED_)
