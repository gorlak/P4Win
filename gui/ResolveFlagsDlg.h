#if !defined(AFX_RESOLVEFLAGSDLG_H__9E086F33_5826_11D4_836B_009027AF6042__INCLUDED_)
#define AFX_RESOLVEFLAGSDLG_H__9E086F33_5826_11D4_836B_009027AF6042__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ResolveFlagsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CResolveFlagsDlg dialog

class CResolveFlagsDlg : public CDialog
{
// Construction
public:
	CResolveFlagsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CResolveFlagsDlg)
	enum { IDD = IDD_RESOLVE_FLAGS };
	BOOL	m_ReResolve;
	BOOL	m_TextualMerge;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResolveFlagsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CResolveFlagsDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESOLVEFLAGSDLG_H__9E086F33_5826_11D4_836B_009027AF6042__INCLUDED_)
