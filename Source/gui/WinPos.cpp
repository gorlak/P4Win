/*
 * Copyright 1997, 1999 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */

// WinPos.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "WinPos.h"
#include "RegKeyEx.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static LPCTSTR sRegKey = _T("Software\\Perforce\\P4Win\\Layout\\");
static LPCTSTR sRegValue_WindowPosition = _T("WindowPosition");

/////////////////////////////////////////////////////////////////////////////
// CWinPos


CWinPos::CWinPos(bool noSize)
	: m_noSize(noSize)
{
	m_pWnd = NULL;
	m_MinSize= CSize(0,0);
	m_DefPos = CRect(0,0,0,0);
}

void CWinPos::SetWindow( CWnd *pWnd, LPCTSTR windowName )
{
	m_pWnd= pWnd;
	m_WindowName= windowName;
}

bool CWinPos::RestoreWindowPosition()
{
	ASSERT( m_pWnd != NULL );
	ASSERT( !m_WindowName.IsEmpty() );

	/////////////////
	// Restore window size and context from registry
	CRect rect;
	m_pWnd->GetWindowRect(&rect);

	// If we don't get a registry value,
	// make sure we have enough height 
	// so as to not trigger a vertical scrollbar
	rect.InflateRect( 0, 0, 0, 50 );

	bool rc = ReadRegistry(&rect);

	rect.NormalizeRect();

	if(rect.Width() < m_MinSize.cx)
		rect.right= rect.left + m_MinSize.cx;
	if(rect.Height() <= m_MinSize.cy)
		rect.bottom= rect.top + m_MinSize.cy + GetSystemMetrics(SM_CYHSCROLL) * 2;

	if(rect.Width() < 50 || rect.Height() < 50)
	{
		// Might have a bad reg value, so set a reasonable default
		rect= CRect(GetSystemMetrics(SM_CXSCREEN) /10,
				GetSystemMetrics(SM_CYSCREEN) /10,
				GetSystemMetrics(SM_CXSCREEN) * 9/10,
				GetSystemMetrics(SM_CYSCREEN) * 9/10);
	}
	else
	{
		// If rectangle bigger than screen, shrink/shift as required
		// (some 8-ball may have changed video res since we last ran)
		int reduceX= 0;
		if (MainFrame()->m_NbrDisplays < 2)
			reduceX= max( rect.Width() - GetSystemMetrics(SM_CXSCREEN), 0 );
		int reduceY= max( rect.Height() - GetSystemMetrics(SM_CYSCREEN), 0 );
		rect.DeflateRect(reduceX/2, reduceY/2);
		int shiftX= 0;
		if (MainFrame()->m_NbrDisplays < 2)
			shiftX= max( rect.right - GetSystemMetrics(SM_CXSCREEN), 0);
		int shiftY= max( rect.bottom - GetSystemMetrics(SM_CYSCREEN), 0);
		rect.OffsetRect(-shiftX, -shiftY);
	}

	/////////////////
	// Set the size  of the window
	
	WINDOWPLACEMENT place;
	place.showCmd=SW_NORMAL;
	place.flags=WPF_SETMINPOSITION;
	place.length= sizeof(WINDOWPLACEMENT);
	place.ptMinPosition=CPoint(0,0);
	place.ptMaxPosition=CPoint(-::GetSystemMetrics(SM_CXBORDER), -::GetSystemMetrics(SM_CYBORDER));
	place.rcNormalPosition= rect;
	m_pWnd->SetWindowPlacement(&place);

	return rc;
}


void CWinPos::SaveWindowPosition()
{
	ASSERT( m_pWnd != NULL );
	ASSERT( !m_WindowName.IsEmpty() );

	WINDOWPLACEMENT place;
	if(m_pWnd->GetWindowPlacement(&place))
	{
		WriteRegistry(&place.rcNormalPosition);
	}
}

void CWinPos::WriteRegistry( RECT *rect )
{
    // Save values to registry.  Fail silently if unable to write.
	CString keyName= sRegKey + m_WindowName;
 
    CRegKeyEx regKey;
    if(regKey.Create(HKEY_CURRENT_USER, keyName) == ERROR_SUCCESS)
    {
		CString str;
		if(m_noSize)
			str.Format(_T("%d,%d"), rect->top, rect->left);
		else
			str.Format(_T("%d,%d,%d,%d"), rect->top, rect->left, 
							rect->right- rect->left, rect->bottom-rect->top);

        regKey.SetValueString(str, sRegValue_WindowPosition);
    }
}

bool CWinPos::ReadRegistry( RECT *rect )
							  
{
	CString keyName= sRegKey + m_WindowName;

	// Look in the registry for the window rect, and assign it if it is
	// a valid rect 
    CRegKeyEx regKey;
    if(regKey.Open(HKEY_CURRENT_USER, keyName, KEY_READ) == ERROR_SUCCESS)
    {
        CString result = regKey.QueryValueString(sRegValue_WindowPosition);
        if(!result.IsEmpty())
        {
			int top= GetPositiveNumber(result);
			int left= GetPositiveNumber(result);
			int width,height;
			if(m_noSize)
			{
				width = rect->right - rect->left;
				height = rect->bottom - rect->top;
			}
			else
			{
				width= GetPositiveNumber(result);
				height= GetPositiveNumber(result);
			}
			if(left >=0 && top >= 0 && width > 0 && height > 0)
			{
				width= min( width, GetSystemMetrics(SM_CXSCREEN));
				height= min( height, GetSystemMetrics(SM_CYSCREEN));
				if (MainFrame()->m_NbrDisplays > 1)
					rect->left= left;
				else
					rect->left= min(left, GetSystemMetrics(SM_CXSCREEN)-width);
				rect->top= min(top, GetSystemMetrics(SM_CYSCREEN)-height);
				rect->right= rect->left+width;
				rect->bottom= rect->top+height;
			}
			return true;
        }
    }
	// failed to get something from the registry,
	// so see if there is a default position
	if (m_DefPos.Width() && m_DefPos.Height())
	{
		rect->top   = m_DefPos.top;
		rect->bottom= m_DefPos.bottom;
		rect->left  = m_DefPos.left;
		rect->right = m_DefPos.right;
	}
	return false;
}
