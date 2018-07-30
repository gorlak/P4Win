/*
 * Copyright 1997, 1999 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */



#if !defined(AFX_INTEGOPTSHEET_H__D41FD0F3_7B6B_11D3_A36A_00105AC64526__INCLUDED_)
#define AFX_INTEGOPTSHEET_H__D41FD0F3_7B6B_11D3_A36A_00105AC64526__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IntegOptSheet.h : header file
//

#include "OptionsPage.h"
#include "RevRange.h"

/////////////////////////////////////////////////////////////////////////////
// CIntegOptSheet

class CIntegOptSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CIntegOptSheet)

// Construction
public:
	CIntegOptSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CIntegOptSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
	CIntegOptionsPage m_OptsPage;
	CIntegRevRange m_RevRngPage;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIntegOptSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CIntegOptSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CIntegOptSheet)
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INTEGOPTSHEET_H__D41FD0F3_7B6B_11D3_A36A_00105AC64526__INCLUDED_)
