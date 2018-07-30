// UserView.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "UserView.h"
#include "ImageList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUserView

IMPLEMENT_DYNCREATE(CUserView, CP4PaneView)

CUserView::CUserView() 
{
	m_content = &m_userListCtrl;
}

CUserView::~CUserView() {}

bool CUserView::CreateContent()
{
	if(!m_userListCtrl.Create( WS_CHILD | WS_VISIBLE, CRect(0, 0, 10, 10), this, 1))
		return false;
	return true;
}

void CUserView::SetToolBarButtons()
{
	m_toolBar.SetButtons(NULL, 1);
	m_toolBar.SetButtonInfo(0, ID_VIEW_UPDATE_RIGHT, TBBS_BUTTON, 
		CP4WinToolBarImageList::TBI_REFRESH);
}

BEGIN_MESSAGE_MAP(CUserView, CP4PaneView)
END_MESSAGE_MAP()