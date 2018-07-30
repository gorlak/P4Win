//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// OldChgView.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "OldChgView.h"
#include "MainFrm.h"
#include "ImageList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COldChgView

IMPLEMENT_DYNCREATE(COldChgView, CP4PaneView)

BEGIN_MESSAGE_MAP(COldChgView, CP4PaneView)
END_MESSAGE_MAP()

COldChgView::COldChgView() 
{
	m_content = &m_oldChgListCtrl;
}

COldChgView::~COldChgView() {}

bool COldChgView::CreateContent()
{
	if(!m_oldChgListCtrl.Create( WS_CHILD | WS_VISIBLE, CRect(0, 0, 10, 10), this, 1))
		return false;
	return true;
}

void COldChgView::SetToolBarButtons()
{
	m_toolBar.SetButtons(NULL, 3);
	m_toolBar.SetButtonInfo(0, ID_VIEW_UPDATE_RIGHT, TBBS_BUTTON, 
		CP4WinToolBarImageList::TBI_REFRESH);
	m_toolBar.SetButtonInfo(1, ID_FILTER_SETVIEW, TBBS_BUTTON, 
		CP4WinToolBarImageList::TBI_SETFILTER);
	m_toolBar.SetButtonInfo(2, ID_FILTER_CLEARVIEW, TBBS_BUTTON, 
		CP4WinToolBarImageList::TBI_CLEARFILTER);
}

void COldChgView::OnInitialUpdate()
{
	CP4PaneView::OnInitialUpdate();

	// this can't be done in COldChgListCtrl::OnCreate because
	// it depends on other parts of the app being initialized first
	m_oldChgListCtrl.PersistentChgFilter( KEY_READ );
}

void COldChgView::LButtonDblClk()
{
	((COldChgListCtrl *)GetContentWnd())->OnFilterSetview();
}
