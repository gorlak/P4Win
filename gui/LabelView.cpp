// LabelView.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "LabelView.h"
#include "MainFrm.h"
#include "ImageList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLabelView

IMPLEMENT_DYNCREATE(CLabelView, CP4PaneView)

BEGIN_MESSAGE_MAP(CLabelView, CP4PaneView)
	ON_NOTIFY(TBN_DROPDOWN, AFX_IDW_TOOLBAR, OnDropDown)
	ON_UPDATE_COMMAND_UI(ID_LABELFILTER_SET, OnUpdateFilterLabels)
	ON_UPDATE_COMMAND_UI(ID_LABELFILTER_CLEAR, OnUpdateClearFilterLabels)
END_MESSAGE_MAP()

CLabelView::CLabelView() 
{
	m_content = &m_labelListCtrl;
}

CLabelView::~CLabelView() {}

bool CLabelView::CreateContent()
{
	if(!m_labelListCtrl.Create( WS_CHILD | WS_VISIBLE, CRect(0, 0, 10, 10), this, 1))
		return false;
	return true;
}

void CLabelView::SetToolBarButtons()
{
	m_toolBar.SetButtons(NULL, 3);
	m_toolBar.SetButtonInfo(0, ID_VIEW_UPDATE_RIGHT, TBBS_BUTTON, 
		CP4WinToolBarImageList::TBI_REFRESH);
	m_toolBar.SetButtonInfo(1, ID_LABELFILTER_SET, TBBS_DROPDOWN, 
		CP4WinToolBarImageList::TBI_SETFILTER);
	m_toolBar.SetButtonInfo(2, ID_LABELFILTER_CLEAR, TBBS_DROPDOWN, 
		CP4WinToolBarImageList::TBI_CLEARFILTER);
}

void CLabelView::OnUpdateFilterLabels(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( !SERVER_BUSY() );	
}

void CLabelView::OnUpdateClearFilterLabels(CCmdUI* pCmdUI) 
{
	((CLabelListCtrl *)GetContentWnd())->OnUpdateClearFilterLabels(pCmdUI);	
}

void CLabelView::OnDropDown(NMHDR* pNotifyStruct, LRESULT* pResult)
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
	case ID_LABELFILTER_SET:
		menu.LoadMenu(IDR_FILTER_LABEL);
		break;
	case ID_LABELFILTER_CLEAR:
		menu.LoadMenu(IDR_CLEARFILTER_LABEL);
		break;
	default:
		MainFrame()->OnDropDown(pNotifyStruct, pResult);
		return;
	}

	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, rect.left, rect.top + 1, AfxGetMainWnd());
	*pResult = TBDDRET_DEFAULT;
}

