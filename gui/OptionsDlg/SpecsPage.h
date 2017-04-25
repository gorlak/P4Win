/*
 * Copyright 1999 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */



#ifndef __SpecsPage__
#define __SpecsPage__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SpecsPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSpecsPage dialog

class CSpecsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CSpecsPage)

// Construction
public:
	CSpecsPage();
	~CSpecsPage();

// Dialog Data
	//{{AFX_DATA(CSpecsPage)
	enum { IDD = IDD_PAGE_SPECS };
	CComboBox	m_FontCombo;
	CComboBox	m_SizeCombo;
	CComboBox	m_StyleCombo;
	CStatic	m_Sample;
	CString	m_FontFace;
	CString	m_FontSize;
	CString	m_FontStyle;
	CButton	m_PreserveSpecFormat;
	CButton	m_AllowPromptAbove;
	CString m_MinMultiLineSize;
	BOOL    m_DescWrapSw;
	CString m_DescWrap;
	CButton	m_AutoMinEditDlg;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSpecsPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_Inited;
	int m_ErrorCount;

	// Generated message map functions
	//{{AFX_MSG(CSpecsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChange();
	afx_msg void OnSelchangeFont();
	afx_msg void OnDescWrapSw();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CFont m_FontSample;
	int	m_cyPixelsPerInch;
	static int CALLBACK FontEnumProc( ENUMLOGFONTEX *enumLogFontEx, NEWTEXTMETRICEX *newTextMetric,
		int fontType, CSpecsPage *specsPage );
	static int CALLBACK SizeEnumProc( ENUMLOGFONT *enumLogFont, NEWTEXTMETRIC *newTextMetric, 
		int fontType, CSpecsPage *specsPage);

	void UpdateSampleFont();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __SpecsPage__
