//
// Copyright 1997 Perforce Software.  All rights reserved.
//
// This file is part of Perforce - the FAST SCM System.
//
//

// RichEdCtrlEx.h : header file
//

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRichEdCtrlEx window

class CRichEdCtrlEx : public CRichEditCtrl
{
// Construction
public:
	CRichEdCtrlEx();

// Attributes
protected:
    // character width, in twips (only valid for fixed pitch font)
    // this is used for setting tab stops
	long m_CharWidth;   
	PARAFORMAT	m_pf;

public:
    // tab settings will only be valid when used with a fixed pitch font
	int	m_TabStop;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRichEdCtrlEx)
	//}}AFX_VIRTUAL

// Implementation
public:
    void SetTabWidth(int width);
	void SetTabs();
	void SetFont(CFont* pFont, BOOL bRedraw = TRUE);
	virtual ~CRichEdCtrlEx();

    CString GetSelText();
protected:

	// Generated message map functions
protected:
	//{{AFX_MSG(CRichEdCtrlEx)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
