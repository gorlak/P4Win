//
// Copyright 1997 Perforce Software.  All rights reserved.
//
// This file is part of Perforce - the FAST SCM System.
//
//

// RichEdCtrlEx.cpp : implementation file
//
// Same as CRichEditCtrl, except we notify the parent wnd
// whenever the caret changes lines.

#include "stdafx.h"
#include "RichEdCtrlEx.h"
#include <malloc.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRichEdCtrlEx

CRichEdCtrlEx::CRichEdCtrlEx()
{
    m_TabStop = 4;
    m_pf.cbSize = sizeof(m_pf);
    m_pf.dwMask = PFM_TABSTOPS;
    m_CharWidth = 0;
}

CRichEdCtrlEx::~CRichEdCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CRichEdCtrlEx, CRichEditCtrl)
	//{{AFX_MSG_MAP(CRichEdCtrlEx)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRichEdCtrlEx message handlers

void
CRichEdCtrlEx::SetTabs()
{
	GetParaFormat( m_pf );
	m_pf.cTabCount  = MAX_TAB_STOPS;
    // fill rgxTabs array with absolute tab stop positions, measured in twips
	m_pf.rgxTabs[0] = 3 * m_CharWidth;
	for (int i = 0; ++i < m_pf.cTabCount; )
		m_pf.rgxTabs[i] = m_pf.rgxTabs[i-1] + (m_TabStop * m_CharWidth);
	SetParaFormat( m_pf );
}

void CRichEdCtrlEx::SetFont(CFont* pFont, BOOL bRedraw /*= TRUE*/)
{
	CRichEditCtrl::SetFont(pFont, bRedraw);

    CDC *pDC = GetDC();
    if(pDC)
    {
        // Assume fixed pitch font, so it we can just get extent of any character
        CPoint ext = pDC->GetTextExtent(_T("a"));
        // convert pixels to twips
        m_CharWidth = ext.x * 1440 / pDC->GetDeviceCaps(LOGPIXELSX);
        ReleaseDC(pDC);
    }
    else
        m_CharWidth = 120;

	SetTabs();
}

void CRichEdCtrlEx::SetTabWidth(int width)
{
	m_TabStop = width;
	if (!m_TabStop)
		m_TabStop = 4;
    SetTabs();
}

CString CRichEdCtrlEx::GetSelText()
{
    // copied from CRichEdCtrl::GetSelText() and modified
    // by making sure there is a terminating 0
    // and making it UNICODE compatible

	ASSERT(::IsWindow(m_hWnd));
	CHARRANGE cr;
	cr.cpMin = cr.cpMax = 0;
	::SendMessage(m_hWnd, EM_EXGETSEL, 0, (LPARAM)&cr);
#ifdef UNICODE
	LPTSTR lpsz = (LPTSTR)_alloca((cr.cpMax - cr.cpMin + 1)*sizeof(TCHAR));
#else
	LPSTR lpsz = (char*)_alloca((cr.cpMax - cr.cpMin + 1)*2);
#endif
//    memset(lpsz, 0, (cr.cpMax - cr.cpMin + 1)*sizeof(TCHAR));
	lpsz[0] = NULL;
	::SendMessage(m_hWnd, EM_GETSELTEXT, 0, (LPARAM)lpsz);
	return CString(lpsz);
}