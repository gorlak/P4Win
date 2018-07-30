//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


#include "stdafx.h"
#include "p4win.h"
#include "DepotView.h"
#include "MainFrm.h"
#include "ImageList.h"

/////////////////////////////////////////////////////////////////////////////
// CDepotView

IMPLEMENT_DYNCREATE(CDepotView, CP4PaneView)

BEGIN_MESSAGE_MAP(CDepotView, CP4PaneView)
ON_NOTIFY(TBN_DROPDOWN, AFX_IDW_TOOLBAR, OnDropDown)
ON_UPDATE_COMMAND_UI(ID_VIEW_DEPOT, OnUpdateViewDepot)
END_MESSAGE_MAP()

CDepotView::CDepotView() 
{
	m_content = &m_depotTreeCtrl;
}

CDepotView::~CDepotView() {}

bool CDepotView::CreateContent()
{
	if(!m_depotTreeCtrl.Create( WS_CHILD | WS_VISIBLE, CRect(0, 0, 10, 10), this, 1))
		return false;
	return true;
}

void CDepotView::SetToolBarButtons()
{
	m_toolBar.SetButtons(NULL, 4);
	m_toolBar.SetButtonInfo(0, ID_VIEW_UPDATE_LEFT, TBBS_BUTTON, 
		CP4WinToolBarImageList::TBI_REFRESH);
	m_toolBar.SetButtonInfo(1, ID_VIEW_FILTEREDVIEW, TBBS_BUTTON, 
		CP4WinToolBarImageList::TBI_SETFILTER);
	m_toolBar.SetButtonInfo(2, ID_VIEW_CLEARFILTER, TBBS_BUTTON, 
		CP4WinToolBarImageList::TBI_CLEARFILTER);
	m_toolBar.SetButtonInfo(3, ID_VIEW_DEPOT, TBBS_DROPDOWN|BTNS_WHOLEDROPDOWN, 
		CP4WinToolBarImageList::TBI_VIEWOPTIONS);
	if (MainFrame()->m_osVer.dwMajorVersion >= 5 && MainFrame()->m_IconsInMenus)
	{
		MainFrame()->SaveToolBarBitmap(CP4WinToolBarImageList::TBI_REFRESH, ID_VIEW_UPDATE);	// use ID_VIEW_UPDATE because that's in menu
		MainFrame()->SaveToolBarBitmap(CP4WinToolBarImageList::TBI_SETFILTER, ID_VIEW_FILTEREDVIEW);
		MainFrame()->SaveToolBarBitmap(CP4WinToolBarImageList::TBI_CLEARFILTER, ID_VIEW_CLEARFILTER);
	}

}

void CDepotView::OnUpdateViewDepot(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( !SERVER_BUSY() );	
}

void CDepotView::OnDropDown(NMHDR* pNotifyStruct, LRESULT* pResult)
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
	case ID_VIEW_DEPOT:
		menu.LoadMenu(IDR_VIEW_DEPOT);
		break;
	default:
		MainFrame()->OnDropDown(pNotifyStruct, pResult);
		return;
	}

	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, rect.left, rect.top + 1, AfxGetMainWnd());
	*pResult = TBDDRET_DEFAULT;
}

void CDepotView::LButtonDblClk()
{
#ifdef _DEBUG
	CMenu menu;
	menu.LoadMenu(IDR_VIEW_DEPOT);
	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, 0, 0 + 1, AfxGetMainWnd());
#endif
}
