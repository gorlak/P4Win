// ClientView.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "ClientView.h"
#include "ImageList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClientView

IMPLEMENT_DYNCREATE(CClientView, CP4PaneView)

CClientView::CClientView() 
{
	m_content = &m_clientListCtrl;
}

CClientView::~CClientView() {}

bool CClientView::CreateContent()
{
	if(!m_clientListCtrl.Create( WS_CHILD | WS_VISIBLE, CRect(0, 0, 10, 10), this, 1))
		return false;
	return true;
}

void CClientView::SetToolBarButtons()
{
	m_toolBar.SetButtons(NULL, 3);
	m_toolBar.SetButtonInfo(0, ID_VIEW_UPDATE_RIGHT, TBBS_BUTTON, 
		CP4WinToolBarImageList::TBI_REFRESH);
	m_toolBar.SetButtonInfo(1, ID_FILTERCLIENTS, TBBS_BUTTON, 
		CP4WinToolBarImageList::TBI_SETFILTER);
	m_toolBar.SetButtonInfo(2, ID_CLEARCLIENTFILTER, TBBS_BUTTON, 
		CP4WinToolBarImageList::TBI_CLEARFILTER);

}

void CClientView::LButtonDblClk()
{
	((CClientListCtrl *)GetContentWnd())->OnFilterClients();
}


BEGIN_MESSAGE_MAP(CClientView, CP4PaneView)
END_MESSAGE_MAP()