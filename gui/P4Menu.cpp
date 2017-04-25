// P4Menu.cpp: implementation of the CP4Menu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "p4win.h"
#include "MainFrm.h"
#include "P4Menu.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CP4Menu::CP4Menu()
{

}

CP4Menu::~CP4Menu()
{

}

BOOL CP4Menu::TrackPopupMenu(UINT nFlags, int x, int y, CWnd* pWnd, LPCRECT lpRect /*= 0*/)
{
	MainFrame()->DoNotAutoPoll();
	MainFrame()->m_InPopUpMenu = TRUE;
	BOOL rc = CMenu::TrackPopupMenu(nFlags, x-6, y-6, pWnd, lpRect);
	MainFrame()->m_InPopUpMenu = FALSE;
	MainFrame()->ResumeAutoPoll();
	return rc;
}
