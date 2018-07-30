// P4PaneContent.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "P4PaneContent.h"
#include "P4PaneView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CP4PaneContent

CP4PaneContent::CP4PaneContent()
	: m_depotWnd(0)
	, m_changeWnd(0)
	, m_oldChgWnd(0)
	, m_jobWnd(0)
{}

CP4PaneView * CP4PaneContent::GetView() 
{ 
	return DYNAMIC_DOWNCAST(CP4PaneView, GetWnd()->GetParent()); 
}

void CP4PaneContent::SetBranchWnd(CWnd *wnd) 
{ 
	CP4PaneView * pv = DYNAMIC_DOWNCAST(CP4PaneView, wnd);
	if(pv)
	{
		// target is a CP4PaneView, so we actually want to
		// send messages to its content window
		m_branchWnd = pv->GetContent()->GetWnd()->m_hWnd;
	}
	else
	{
		// assume messages should go to the window given
		m_branchWnd = wnd->m_hWnd;
	}
	ASSERT(m_branchWnd);
}

void CP4PaneContent::SetChangeWnd(CWnd *wnd) 
{ 
	CP4PaneView * pv = DYNAMIC_DOWNCAST(CP4PaneView, wnd);
	if(pv)
	{
		// target is a CP4PaneView, so we actually want to
		// send messages to its content window
		m_changeWnd = pv->GetContent()->GetWnd()->m_hWnd;
	}
	else
	{
		// assume messages should go to the window given
		m_changeWnd = wnd->m_hWnd;
	}
	ASSERT(m_changeWnd);
}

void CP4PaneContent::SetClientWnd(CWnd *wnd) 
{ 
	CP4PaneView * pv = DYNAMIC_DOWNCAST(CP4PaneView, wnd);
	if(pv)
	{
		// target is a CP4PaneView, so we actually want to
		// send messages to its content window
		m_clientWnd = pv->GetContent()->GetWnd()->m_hWnd;
	}
	else
	{
		// assume messages should go to the window given
		m_clientWnd = wnd->m_hWnd;
	}
	ASSERT(m_clientWnd);
}

void CP4PaneContent::SetDepotWnd(CWnd *wnd) 
{ 
	CP4PaneView * pv = DYNAMIC_DOWNCAST(CP4PaneView, wnd);
	if(pv)
	{
		// target is a CP4PaneView, so we actually want to
		// send messages to its content window
		m_depotWnd = pv->GetContent()->GetWnd()->m_hWnd;
	}
	else
	{
		// assume messages should go to the window given
		m_depotWnd = wnd->m_hWnd;
	}
	ASSERT(m_depotWnd);
}

void CP4PaneContent::SetJobWnd(CWnd *wnd) 
{ 
	CP4PaneView * pv = DYNAMIC_DOWNCAST(CP4PaneView, wnd);
	if(pv)
	{
		// target is a CP4PaneView, so we actually want to
		// send messages to its content window
		m_jobWnd = pv->GetContent()->GetWnd()->m_hWnd;
	}
	else
	{
		// assume messages should go to the window given
		m_jobWnd = wnd->m_hWnd;
	}
	ASSERT(m_jobWnd);
}

void CP4PaneContent::SetLabelWnd(CWnd *wnd) 
{ 
	CP4PaneView * pv = DYNAMIC_DOWNCAST(CP4PaneView, wnd);
	if(pv)
	{
		// target is a CP4PaneView, so we actually want to
		// send messages to its content window
		m_labelWnd = pv->GetContent()->GetWnd()->m_hWnd;
	}
	else
	{
		// assume messages should go to the window given
		m_labelWnd = wnd->m_hWnd;
	}
	ASSERT(m_labelWnd);
}

void CP4PaneContent::SetOldChgWnd(CWnd *wnd) 
{ 
	CP4PaneView * pv = DYNAMIC_DOWNCAST(CP4PaneView, wnd);
	if(pv)
	{
		// target is a CP4PaneView, so we actually want to
		// send messages to its content window
		m_oldChgWnd = pv->GetContent()->GetWnd()->m_hWnd;
	}
	else
	{
		// assume messages should go to the window given
		m_oldChgWnd = wnd->m_hWnd;
	}
	ASSERT(m_oldChgWnd);
}

void CP4PaneContent::SetUserWnd(CWnd *wnd) 
{ 
	CP4PaneView * pv = DYNAMIC_DOWNCAST(CP4PaneView, wnd);
	if(pv)
	{
		// target is a CP4PaneView, so we actually want to
		// send messages to its content window
		m_userWnd = pv->GetContent()->GetWnd()->m_hWnd;
	}
	else
	{
		// assume messages should go to the window given
		m_userWnd = wnd->m_hWnd;
	}
	ASSERT(m_userWnd);
}

// By default, just pass Drag/Drop back up to CView
// Subclasses of CP4PaneContent can override these functions to
// implement Drag/Drop

DROPEFFECT CP4PaneContent::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	return (DYNAMIC_DOWNCAST(CView,GetWnd()->GetParent()))->CView::OnDragEnter(pDataObject, dwKeyState, point);
}

void CP4PaneContent::OnDragLeave()
{
	(DYNAMIC_DOWNCAST(CView,GetWnd()->GetParent()))->CView::OnDragLeave();
}

DROPEFFECT CP4PaneContent::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	return (DYNAMIC_DOWNCAST(CView,GetWnd()->GetParent()))->CView::OnDragOver(pDataObject, dwKeyState, point);
}

BOOL CP4PaneContent::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	return (DYNAMIC_DOWNCAST(CView,GetWnd()->GetParent()))->CView::OnDrop(pDataObject, dropEffect, point);
}

