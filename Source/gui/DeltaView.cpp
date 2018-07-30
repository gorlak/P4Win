//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// DeltaView.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "DeltaView.h"
#include "MainFrm.h"
#include "ImageList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeltaView

IMPLEMENT_DYNCREATE(CDeltaView, CP4PaneView)

BEGIN_MESSAGE_MAP(CDeltaView, CP4PaneView)
ON_NOTIFY(TBN_DROPDOWN, AFX_IDW_TOOLBAR, OnDropDown)
ON_UPDATE_COMMAND_UI(ID_VIEW_DELTA, OnUpdateViewDelta)
END_MESSAGE_MAP()

CDeltaView::CDeltaView() 
{
	m_content = &m_deltaTreeCtrl;
}

CDeltaView::~CDeltaView() {}

bool CDeltaView::CreateContent()
{
	if(!m_deltaTreeCtrl.Create( WS_CHILD | WS_VISIBLE, CRect(0, 0, 10, 10), this, 1))
		return false;
	return true;
}

void CDeltaView::SetToolBarButtons()
{
	m_toolBar.SetButtons(NULL, 2);
	m_toolBar.SetButtonInfo(0, ID_VIEW_UPDATE_RIGHT, TBBS_BUTTON, 
		CP4WinToolBarImageList::TBI_REFRESH);
	m_toolBar.SetButtonInfo(1, ID_VIEW_DELTA, TBBS_DROPDOWN|BTNS_WHOLEDROPDOWN, 
		CP4WinToolBarImageList::TBI_VIEWOPTIONS);
	if (MainFrame()->m_osVer.dwMajorVersion >= 5 && MainFrame()->m_IconsInMenus)
	{
		HICON hIcon = TheApp()->GetToolBarImageList()->ExtractIcon(
								CP4WinToolBarImageList::TBI_VIEWOPTIONS);
		if (hIcon)
		{
			MENUITEMINFO mii;
			CWindowDC dc(this);
			CDC memdc;
			memdc.CreateCompatibleDC(&dc);
			CBrush brush;
			brush.CreateSysColorBrush(COLOR_MENU);
			RECT rect = {0, 0, 20, 18};
			m_SortMenuBitmap.CreateCompatibleBitmap(&dc, 20, 18);
			CBitmap *poldbm = memdc.SelectObject(&m_SortMenuBitmap);
			memdc.FillRect(&rect, &brush);
			::DrawIconEx(memdc.m_hDC, 0, 0, hIcon,
				GetSystemMetrics(SM_CXMENUCHECK), // cx
		        GetSystemMetrics(SM_CYMENUCHECK), // cy
				0, brush, DI_NORMAL);         // frame, brush, flags
			memdc.SelectObject(poldbm);
			// Attach the icon to the menu item
			memset(&mii, _T('\0'), sizeof(mii));
			mii.cbSize = sizeof(mii);
			mii.fMask  = MIIM_BITMAP;
			mii.hbmpItem = HBITMAP(m_SortMenuBitmap);
			CMenu* pSubMenu = MainFrame()->GetMenu()->GetSubMenu(4);
			pSubMenu->SetMenuItemInfo(pSubMenu->GetMenuItemCount()-7, &mii, TRUE);
		}
	}
}

void CDeltaView::OnUpdateViewDelta(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( !SERVER_BUSY() );	
}

void CDeltaView::OnDropDown(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	// this function handles the dropdown menus from the toolbar
	NMTOOLBAR* pNMToolBar = (NMTOOLBAR*)pNotifyStruct;
	CRect rect;

	// translate the current toolbar item rectangle into screen coordinates
	// so that we'll know where to pop up the menu
	m_toolBar.GetToolBarCtrl().GetRect(pNMToolBar->iItem, &rect);
	rect.top = rect.bottom;
	::ClientToScreen(pNMToolBar->hdr.hwndFrom, &rect.TopLeft());
	CMenu menu;
	switch(pNMToolBar->iItem)
	{
	case ID_VIEW_DELTA:
		menu.LoadMenu(IDR_VIEW_DELTA);
		break;
	default:
		MainFrame()->OnDropDown(pNotifyStruct, pResult);
		return;
	}

	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, rect.left, rect.top + 1, AfxGetMainWnd());
	*pResult = TBDDRET_DEFAULT;
}

BOOL CDeltaView::OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll) 
{
	// give CDeltaTreeCtrl a crack at it first
	BOOL bScrolled;
	if(GetTreeCtrl().OnScroll(nScrollCode, nPos, bDoScroll, &bScrolled))
		return bScrolled;

	// if not handled there, pass to default view handler
	return CP4PaneView::OnScroll(nScrollCode, nPos, TRUE);
}
