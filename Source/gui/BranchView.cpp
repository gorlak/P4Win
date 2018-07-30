// BranchView.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "BranchView.h"
#include "ImageList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CBranchView

IMPLEMENT_DYNCREATE(CBranchView, CP4PaneView)

CBranchView::CBranchView() 
{
	m_content = &m_branchListCtrl;
}

CBranchView::~CBranchView() {}

bool CBranchView::CreateContent()
{
	if(!m_branchListCtrl.Create( WS_CHILD | WS_VISIBLE, CRect(0, 0, 10, 10), this, 1))
		return false;
	return true;
}

void CBranchView::SetToolBarButtons()
{
	m_toolBar.SetButtons(NULL, 3);
	m_toolBar.SetButtonInfo(0, ID_VIEW_UPDATE_RIGHT, TBBS_BUTTON, 
		CP4WinToolBarImageList::TBI_REFRESH);
	m_toolBar.SetButtonInfo(1, ID_FILTERBRANCHBYOWNER, TBBS_BUTTON, 
		CP4WinToolBarImageList::TBI_SETFILTER);
	m_toolBar.SetButtonInfo(2, ID_CLEARBRANCHOWNERFILTER, TBBS_BUTTON, 
		CP4WinToolBarImageList::TBI_CLEARFILTER);
}

BEGIN_MESSAGE_MAP(CBranchView, CP4PaneView)
END_MESSAGE_MAP()

