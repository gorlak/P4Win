#if !defined(AFX_AUTORESOLVEDLG_H__25CC82E6_3BCE_11D4_835C_009027AF6042__INCLUDED_)
#define AFX_AUTORESOLVEDLG_H__25CC82E6_3BCE_11D4_835C_009027AF6042__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AutoResolveDlg.h : header file
//
#include "DeltaTreeCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CAutoResolveDlg dialog

class CAutoResolveDlg : public CDialog
{
// Construction
public:
	CAutoResolveDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAutoResolveDlg)
	enum { IDD = IDD_AUTORESOLVE };
	int     m_ResolveDefault;
	int		m_ResolveType;
	BOOL	m_ReResolve;
	int		m_AllFiles;
	BOOL	m_TextMerge;
	int		m_ResolveWhtSp;
	//}}AFX_DATA

	BOOL	m_NoSel2Res;
	BOOL	m_Preview;
	BOOL	m_SelResolved;
	BOOL	m_AnyResolved;
	BOOL	m_ResolveFromChgList;
	CDeltaTreeCtrl *m_pDeltaTreeCtrl;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoResolveDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAutoResolveDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnPreview();
	afx_msg void OnAllfiles();
	afx_msg void OnSelectedfiles();
	afx_msg void OnSetDefResolveType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTORESOLVEDLG_H__25CC82E6_3BCE_11D4_835C_009027AF6042__INCLUDED_)
