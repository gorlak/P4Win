//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// JobView.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "JobView.h"
#include "MainFrm.h"
#include "ImageList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJobView

IMPLEMENT_DYNCREATE(CJobView, CP4PaneView)

BEGIN_MESSAGE_MAP(CJobView, CP4PaneView)
	ON_NOTIFY(TBN_DROPDOWN, AFX_IDW_TOOLBAR, OnDropDown)
	ON_UPDATE_COMMAND_UI(ID_SETFILTER_JOBS, OnUpdateSetFilterJobs)
	ON_UPDATE_COMMAND_UI(ID_CLEARFILTER_JOBS, OnUpdateClearFilterJobs)
END_MESSAGE_MAP()

CJobView::CJobView() 
{
	m_content = &m_jobListCtrl;
}

CJobView::~CJobView() {}

bool CJobView::CreateContent()
{
	if(!m_jobListCtrl.Create( WS_CHILD | WS_VISIBLE, CRect(0, 0, 10, 10), this, 1))
		return false;
	return true;
}

void CJobView::SetToolBarButtons()
{
	m_toolBar.SetButtons(NULL, 4);
	m_toolBar.SetButtonInfo(0, ID_VIEW_UPDATE_RIGHT, TBBS_BUTTON, 
		CP4WinToolBarImageList::TBI_REFRESH);
	m_toolBar.SetButtonInfo(1, ID_SETFILTER_JOBS, TBBS_DROPDOWN, 
		CP4WinToolBarImageList::TBI_SETFILTER);
	m_toolBar.SetButtonInfo(2, ID_CLEARFILTER_JOBS, TBBS_DROPDOWN, 
		CP4WinToolBarImageList::TBI_CLEARFILTER);
	m_toolBar.SetButtonInfo(3, ID_JOB_CONFIGURE, TBBS_BUTTON, 
		CP4WinToolBarImageList::TBI_COLUMNS);
	if (MainFrame()->m_osVer.dwMajorVersion >= 5 && MainFrame()->m_IconsInMenus)
		MainFrame()->SaveToolBarBitmap(CP4WinToolBarImageList::TBI_COLUMNS, ID_JOB_CONFIGURE);
}

void CJobView::OnUpdateSetFilterJobs(CCmdUI* pCmdUI) 
{
	((CJobListCtrl *)GetContentWnd())->OnUpdateSetFilterJobs(pCmdUI);	
}

void CJobView::OnUpdateClearFilterJobs(CCmdUI* pCmdUI) 
{
	((CJobListCtrl *)GetContentWnd())->OnUpdateClearFilterJobs(pCmdUI);	
}

void CJobView::OnDropDown(NMHDR* pNotifyStruct, LRESULT* pResult)
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
	case ID_SETFILTER_JOBS:
		menu.LoadMenu(IDR_SETFILTER_JOB);
		if (!GET_P4REGPTR()->GetEnableSubChgIntegFilter())
			menu.DeleteMenu(ID_JOB_SETFILEFILTERINTEG, MF_BYCOMMAND);
		break;
	case ID_CLEARFILTER_JOBS:
		menu.LoadMenu(IDR_CLEARFILTER_JOB);
		break;
	default:
		MainFrame()->OnDropDown(pNotifyStruct, pResult);
		return;
	}

	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, rect.left, rect.top + 1, AfxGetMainWnd());
	*pResult = TBDDRET_DEFAULT;
}

void CJobView::LButtonDblClk()
{
	((CJobListCtrl *)GetContentWnd())->OnJobFilter3();
}
