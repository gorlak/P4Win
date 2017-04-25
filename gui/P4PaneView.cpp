// P4PaneView.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "P4PaneView.h"
#include "P4PaneContent.h"
#include "MainFrm.h"
#include "ImageList.h"
#include <afxpriv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CP4PaneView

const int ID_CAPTION = 222;

IMPLEMENT_DYNCREATE(CP4PaneView, CView)

BEGIN_MESSAGE_MAP(CP4PaneView, CView)
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
	ON_WM_CREATE()
	ON_MESSAGE( WM_FINDPATTERN, OnFindPattern )
	ON_BN_CLICKED( ID_CAPTION, LButtonClk )
	ON_BN_DOUBLECLICKED( ID_CAPTION, LButtonDblClk )
	ON_WM_SYSCOLORCHANGE()
	ON_MESSAGE( WM_ACTIVATEMODELESS, OnActivateModeless )
END_MESSAGE_MAP()

CP4PaneView::CP4PaneView() 
	: m_content(0)
{
}

CP4PaneView::~CP4PaneView() {}

void CP4PaneView::SetBranchWnd(CWnd *wnd) { m_content->SetBranchWnd(wnd); }
void CP4PaneView::SetChangeWnd(CWnd *wnd) { m_content->SetChangeWnd(wnd); }
void CP4PaneView::SetClientWnd(CWnd *wnd) { m_content->SetClientWnd(wnd); }
void CP4PaneView::SetDepotWnd(CWnd *wnd) { m_content->SetDepotWnd(wnd); }
void CP4PaneView::SetJobWnd(CWnd *wnd) { m_content->SetJobWnd(wnd); }
void CP4PaneView::SetLabelWnd(CWnd *wnd) { m_content->SetLabelWnd(wnd); }
void CP4PaneView::SetOldChgWnd(CWnd *wnd) { m_content->SetOldChgWnd(wnd); }
void CP4PaneView::SetUserWnd(CWnd *wnd) { m_content->SetUserWnd(wnd); }

void CP4PaneView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// for some reason, list views end up with client edge style set
	// and we don't want it for any views
	ModifyStyleEx(WS_EX_CLIENTEDGE, 0);

	// want tooltips for caption
	m_reBar.SetBarStyle(CBRS_ALIGN_TOP |CBRS_TOOLTIPS);
	// don't want separator bar between caption and toolbar bands
	m_reBar.ModifyStyle(RBS_BANDBORDERS, 0);

	// put dropdown arrows on dropdown buttons
	m_toolBar.GetToolBarCtrl().SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);
	// set owner to mainframe so tooltips status bar messages will work
	m_toolBar.SetOwner(AfxGetApp()->m_pMainWnd); 
	// make it flat and transparent so it looks good on XP
	m_toolBar.ModifyStyle(0, TBSTYLE_FLAT|TBSTYLE_TRANSPARENT);
	// get rid of extra space around buttons
	m_toolBar.SetBorders();
	// set style to get tooltips working
	m_toolBar.SetBarStyle(CBRS_ALIGN_TOP | CBRS_TOOLTIPS | CBRS_FLYBY);
	// created non-visible so styles could be set before showing, so show now
	m_toolBar.ShowWindow(SW_SHOW);

	// set image list(s) for the toolbar
	m_toolBar.GetToolBarCtrl().SetImageList(
		TheApp()->GetToolBarImageList());
	m_toolBar.GetToolBarCtrl().SetDisabledImageList(
		TheApp()->GetToolBarImageList()->GetDisabled());

	// must set button size before adding buttons
	CSize sizeImage(18, 16);
	CSize sizeButton(18 + 7, 16 + 7);
	m_toolBar.SetSizes(sizeButton, sizeImage);

	// add the buttons to the toolbar
	SetToolBarButtons();

	// make the caption control transparent so the rebar will
	// draw the background under it
	m_captionCtrl.ModifyStyleEx(0, WS_EX_TRANSPARENT);

	// put the bands into the rebar
	// the ' ' text for the caption provides a little space on the left
	m_reBar.AddBar(&m_captionCtrl, _T(" "), NULL, RBBS_NOGRIPPER);
	m_reBar.AddBar(&m_toolBar, NULL, NULL, RBBS_NOGRIPPER|RBBS_FIXEDBMP);

	// determine how tall the bar should be to fit the taller of
	// the caption band or the toolbar band
	CRect r;
	r.SetRectEmpty();
	m_reBar.CalcInsideRect(r, TRUE);
	m_barHeight = max(16 + 7, GetSystemMetrics(SM_CYCAPTION));
	m_barHeight -= r.Height();
	m_contentTop = m_barHeight + GetSystemMetrics(SM_CYEDGE);

	// set up min/max sizes and ideal sizes for pieces of the rebar
	REBARBANDINFO rbbi;
	rbbi.cbSize = sizeof(rbbi);

	// set title band so it will be as wide as possible
	// and with a min size of one, it will never disappear
	// since the caption control will use the standard caption
	// font, it should be as tall as a standard caption.
	rbbi.fMask = RBBIM_CHILDSIZE | RBBIM_IDEALSIZE | RBBIM_SIZE;
	rbbi.cxMinChild = 1;
	rbbi.cyMinChild = GetSystemMetrics(SM_CYCAPTION);
	rbbi.cx = rbbi.cxIdeal = 20000;	// want to be really big!
	m_reBar.GetReBarCtrl().SetBandInfo(0, &rbbi);

	// set max size for toolbar band to it's actual size
	// and min size to same so it won't be crowded out by
	// caption.
	m_toolBar.GetWindowRect(&r);
	rbbi.fMask = RBBIM_CHILDSIZE | RBBIM_IDEALSIZE | RBBIM_SIZE;
	rbbi.cxMinChild = r.Width();
	rbbi.cyMinChild = 16 + 7;
	rbbi.cx = rbbi.cxIdeal = r.Width();
	m_reBar.GetReBarCtrl().SetBandInfo(1, &rbbi);

	// reposition the bars, now that their sizes have been set
	GetClientRect(&r);
	r.bottom = m_barHeight;
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 1,
		reposDefault, NULL, &r);
	// and reposition the content, allowing space for the rebar edge
	GetContentWnd()->MoveWindow(0,m_contentTop,
		r.Width(),r.Height() - m_contentTop,TRUE);

	EnableToolTips();

	if (GET_P4REGPTR( )->SwapButtonPosition())
		SwapButtonPosition();
}

void CP4PaneView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// don't do other sizing until created
	if(!IsWindow(m_toolBar.m_hWnd) || !m_reBar.GetReBarCtrl().GetBandCount())
		return;

	// put toolbar at top, list below it
	CRect r;
	r.SetRect(0, 0, cx, m_barHeight);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 1,
		reposDefault, NULL, &r);
	// RepositionBars won't leave space between the rebar and the content
	// so we need to re-move the content to make space needed for the border
	GetContentWnd()->MoveWindow(0, m_contentTop,	cx, cy - m_contentTop, TRUE);
}

void CP4PaneView::OnDraw(CDC* pDC) 
{
	// just draw an edget between the toolbar and the content
	CRect r;
	GetClientRect(&r);
	r.bottom = r.top + m_contentTop;
	pDC->DrawEdge(&r, EDGE_RAISED, BF_RECT);
}

void CP4PaneView::OnSetFocus(CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);

	if(GetContentWnd() && IsWindow(GetContentWnd()->m_hWnd))
		GetContentWnd()->SetFocus();
}

BOOL CP4PaneView::OnCmdMsg(UINT nID, int nCode, void* pExtra,
	AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if(GetContentWnd()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	return CView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CP4PaneView::OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
	// first, check for special case of caption control
	// and handle it here
    TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;    
    UINT nID =pNMHDR->idFrom;
    if (pTTT->uFlags & TTF_IDISHWND)    
    {
        // idFrom is actually the HWND of the tool
        nID = ::GetDlgCtrlID((HWND)nID);
		if(nID == ID_CAPTION)
		{
			m_captionCtrl.GetWindowText(m_captionTxt);
			pTTT->lpszText = (LPTSTR)(LPCTSTR)m_captionTxt;
            pTTT->hinst = NULL;
			return TRUE;
		}
	}
	// for other cases, just pass it along to mainframe
	CMainFrame *mainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    return mainFrame->OnToolTipText(id, pNMHDR, pResult);
}

int CP4PaneView::GetSelectedItem()
{
	CP4ListCtrl * pList = DYNAMIC_DOWNCAST(CP4ListCtrl, GetContentWnd());
	ASSERT(pList);
	if(pList)
		return pList->GetSelectedItem();
	return 0;
}

void CP4PaneView::OnEditPaste( const CString &Name )
{
	CP4ListCtrl * pList = DYNAMIC_DOWNCAST(CP4ListCtrl, GetContentWnd());
	ASSERT(pList);
	if(pList)
		pList->OnEditPaste(Name);
}

CString CP4PaneView::GetSelectedItemText( )
{
	CP4ListCtrl * pList = DYNAMIC_DOWNCAST(CP4ListCtrl, GetContentWnd());
	ASSERT(pList);
	if(pList)
		return pList->GetSelectedItemText();
	return "";
}

HTREEITEM CP4PaneView::GetSelectedItem(int index)
{
	CMultiSelTreeCtrl * pTree = DYNAMIC_DOWNCAST(CMultiSelTreeCtrl, GetContentWnd());
	ASSERT(pTree);
	if(pTree)
		return pTree->GetSelectedItem(index);
	return 0;
}

CString CP4PaneView::GetItemText(HTREEITEM curr_item)
{
	CMultiSelTreeCtrl * pTree = DYNAMIC_DOWNCAST(CMultiSelTreeCtrl, GetContentWnd());
	ASSERT(pTree);
	if(pTree)
		return pTree->GetItemText(curr_item);
	return "";
}

void CP4PaneView::SetCaption()
{
	m_captionCtrl.SetWindowText(m_content->GetCaption());

	// Getting this updated is a bit complicated since the caption
	// control is transparent (to allow the rebar background to
	// show through).  First, redraw the rebar, then the caption control.
	m_reBar.InvalidateRect(NULL);
	m_reBar.UpdateWindow();
	m_captionCtrl.InvalidateRect(NULL);
	m_captionCtrl.UpdateWindow();
}

int CP4PaneView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(!m_reBar.Create(this))
		return -1;
	if(!m_toolBar.Create(this,WS_CHILD | CBRS_ALIGN_TOP))
		return -1;
	if(!m_captionCtrl.Create(_T(""),WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|BS_NOTIFY,
		CRect(0,0,1,1),this,ID_CAPTION))
		return -1;
	if(!CreateContent())
		return -1;

	LOGFONT lf;
	m_reBar.GetFont()->GetLogFont(&lf);
	m_inactiveFont.CreateFontIndirect(&lf);
	lf.lfWeight = FW_BOLD;
	m_activeFont.CreateFontIndirect(&lf);

	// Register that we accept Exploder files
	try
	{
		m_DropTarget.Register(this);
	}
	catch(...)
	{
		return 0;
	}

	return 0;
}

void CP4PaneView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	if(GetContentWnd() && IsWindow(GetContentWnd()->m_hWnd))
	{
		// force UI update right away, so if activation is caused by
		// click on toolbar dropdown, it can be enabled before the
		// toolbar gets the click message.  Without this, it takes
		// one click to activate and another to drop down.
		m_toolBar.SendMessage(WM_IDLEUPDATECMDUI, (WPARAM)TRUE, 0);

		if(bActivate)
		{
			m_captionCtrl.SetFont(&m_activeFont);
		}
		else
		{
			m_captionCtrl.SetFont(&m_inactiveFont);
		}
		// The size of the caption text will change with the font change
		// so force an update
		SetCaption();
	}

	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

DROPEFFECT CP4PaneView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	MapWindowPoints(GetContentWnd(), &point, 1);
	return m_content->OnDragEnter(pDataObject, dwKeyState, point);
}

void CP4PaneView::OnDragLeave()
{
	m_content->OnDragLeave();
}

DROPEFFECT CP4PaneView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	MapWindowPoints(GetContentWnd(), &point, 1);
	return m_content->OnDragOver(pDataObject, dwKeyState, point);
}

BOOL CP4PaneView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	MapWindowPoints(GetContentWnd(), &point, 1);
	return m_content->OnDrop(pDataObject, dropEffect, point);
}

void CP4PaneView::SwapButtonPosition()
{
	// swap the bands
	m_reBar.GetReBarCtrl().MoveBand(1, 0);
	// for some inexplicable reason, MoveBand doesn't update the bar
	// immediately.  It takes some other action to make it actually update
	// and this seems like the least disruptive thing to do:
	m_reBar.GetReBarCtrl().ShowBand(0, FALSE);
	m_reBar.GetReBarCtrl().ShowBand(0, TRUE);
}

// This is required in order to make the LButtonDblClk() work!
void CP4PaneView::LButtonClk()
{
	SetFocus();
}

void CP4PaneView::LButtonDblClk()
{
	// by default, do nothing
}

LRESULT CP4PaneView::OnFindPattern(WPARAM wParam, LPARAM lParam)
{
	CP4ListCtrl * pList = DYNAMIC_DOWNCAST(CP4ListCtrl, GetContentWnd());
	ASSERT(pList);
	if(pList)
		return pList->OnFindPattern(wParam, lParam);
	return Default();
}

IMPLEMENT_DYNCREATE(CCaptionTextControl, CButton)

BEGIN_MESSAGE_MAP(CCaptionTextControl, CButton)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

HBRUSH CCaptionTextControl::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	// we don't want any background drawn; we're supposed to be transparent
	pDC->SetBkMode(TRANSPARENT);
	return (HBRUSH)GetStockObject(NULL_BRUSH);
}

void CCaptionTextControl::DrawItem(LPDRAWITEMSTRUCT di) 
{
	// Just draw the text.  If it's too long, it will be shortened, since
	// we're using the DT_END_ELLIPSIS style.  This also means the string
	// passed to DrawText may be modified, so we use a temp copy.
	CDC dc;
	dc.Attach(di->hDC);
	const int maxCaption = 256;
	TCHAR txt[maxCaption];
	GetWindowText(txt,maxCaption);
	txt[maxCaption-1] = 0;
	TCHAR mtxt[maxCaption];
	lstrcpy(mtxt,txt);
	dc.DrawText(mtxt, lstrlen(mtxt), &di->rcItem, DT_END_ELLIPSIS | DT_SINGLELINE|DT_VCENTER | DT_MODIFYSTRING);
	m_textTruncated = lstrcmp(mtxt,txt) != 0;
	dc.Detach();
}

int CCaptionTextControl::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	// CWnd implementation won't do us any good, so we have to handle
	// this.
	// watch out!  sometimes get called with null pointer
	if(pTI == 0)
		return -1;

	// don't want to show tooltip if nothing is hidden
	// the tooltip is only there to show truncated text
	if(!m_textTruncated)
		return -1;

	// we want to show a tooltip, so set up for the normal callback
	pTI->hwnd = GetParent()->GetParent()->m_hWnd;	// our parent
	pTI->uId = (UINT_PTR)m_hWnd;		// us
	pTI->uFlags |= TTF_IDISHWND;		// uId is HWND, not ID
	pTI->lpszText = LPSTR_TEXTCALLBACK;

	return 1;
}

DROPEFFECT CP4OleDropTarget::OnDragScroll(CWnd* pWnd, DWORD dwKeyState,
	CPoint point)
{
	// We need special handling of autoscrolling because we want
	// it to behave as though the CP4PaneView's content window is
	// doing the autoscroll.  Without this, it looks like you are
	// dragging over the caption to autoscroll the content.

	// This code is only slightly modified from the base implementation,
	// as needed to get the correct window's client rect for determining
	// the autoscroll inset region.  Also, code for synchronized
	// scrolling of splitters has been stripped out.

	ASSERT_VALID(this);
	ASSERT_VALID(pWnd);

	// if it's not a CP4PaneView, just use the default handler
	CP4PaneView * pView = DYNAMIC_DOWNCAST(CP4PaneView,pWnd);
	if(!pView)
		return COleDropTarget::OnDragScroll(pWnd, dwKeyState, point);

	DROPEFFECT dropEffect = pView->OnDragScroll(dwKeyState, point);

	// DROPEFFECT_SCROLL means do the default
	if (dropEffect != DROPEFFECT_SCROLL)
		return dropEffect;

	// get client rectangle of destination window
	CRect rectClient;
	pView->GetContentWnd()->GetClientRect(&rectClient);
	pView->GetContentWnd()->MapWindowPoints(pView, &rectClient);
	CRect rect = rectClient;

	// hit-test against inset region
	UINT nTimerID = 0xffff;
	rect.InflateRect(-nScrollInset, -nScrollInset);
	if (rectClient.PtInRect(point) && !rect.PtInRect(point))
	{
		// determine which way to scroll along both X & Y axis
		if (point.x < rect.left)
			nTimerID = MAKEWORD(SB_LINEUP, HIBYTE(nTimerID));
		else if (point.x >= rect.right)
			nTimerID = MAKEWORD(SB_LINEDOWN, HIBYTE(nTimerID));
		if (point.y < rect.top)
			nTimerID = MAKEWORD(LOBYTE(nTimerID), SB_LINEUP);
		else if (point.y >= rect.bottom)
			nTimerID = MAKEWORD(LOBYTE(nTimerID), SB_LINEDOWN);
		ASSERT(nTimerID != 0xffff);

		// we don't do synchronized splitter scrolling, so this part is
		// somewhat simplified
		BOOL bEnableScroll = pView->OnScroll(nTimerID, 0, FALSE);
		if (!bEnableScroll)
			nTimerID = 0xffff;
	}

	if (nTimerID == 0xffff)
	{
		if (m_nTimerID != 0xffff)
		{
			// send fake OnDragEnter when transition from scroll->normal
			COleDataObject dataObject;
			dataObject.Attach(m_lpDataObject, FALSE);
			OnDragEnter(pWnd, &dataObject, dwKeyState, point);
			m_nTimerID = 0xffff;
		}
		return DROPEFFECT_NONE;
	}

	// save tick count when timer ID changes
	DWORD dwTick = GetTickCount();
	if (nTimerID != m_nTimerID)
	{
		m_dwLastTick = dwTick;
		m_nScrollDelay = nScrollDelay;
	}

	// scroll if necessary
	if (dwTick - m_dwLastTick > m_nScrollDelay)
	{
		pView->OnScroll(nTimerID, 0, TRUE);
		m_dwLastTick = dwTick;
		m_nScrollDelay = nScrollInterval;
	}
	if (m_nTimerID == 0xffff)
	{
		// send fake OnDragLeave when transitioning from normal->scroll
		OnDragLeave(pWnd);
	}

	m_nTimerID = nTimerID;
	// check for force link
	if ((dwKeyState & (MK_CONTROL|MK_SHIFT)) == (MK_CONTROL|MK_SHIFT))
		dropEffect = DROPEFFECT_SCROLL|DROPEFFECT_LINK;
	// check for force copy
	else if ((dwKeyState & MK_CONTROL) == MK_CONTROL)
		dropEffect = DROPEFFECT_SCROLL|DROPEFFECT_COPY;
	// check for force move
	else if ((dwKeyState & MK_ALT) == MK_ALT ||
		(dwKeyState & MK_SHIFT) == MK_SHIFT)
		dropEffect = DROPEFFECT_SCROLL|DROPEFFECT_MOVE;
	// default -- recommended action is move
	else
		dropEffect = DROPEFFECT_SCROLL|DROPEFFECT_MOVE;
	return dropEffect;
}

void CP4PaneView::OnSysColorChange()
{
	CView::OnSysColorChange();

	m_toolBar.GetToolBarCtrl().SetImageList(
		TheApp()->GetToolBarImageList());
	m_toolBar.GetToolBarCtrl().SetDisabledImageList(
		TheApp()->GetToolBarImageList()->GetDisabled());

	CP4ListCtrl * pList = DYNAMIC_DOWNCAST(CP4ListCtrl, GetContentWnd());
	if(pList)
		pList->SetImageList(TheApp()->GetViewImageList(), LVSIL_SMALL);
	CMultiSelTreeCtrl * pTree = DYNAMIC_DOWNCAST(CMultiSelTreeCtrl, GetContentWnd());
	if(pTree)
		pTree->SetImageList(TheApp()->GetViewImageList(), TVSIL_NORMAL);
}

LRESULT CP4PaneView::OnActivateModeless(WPARAM wParam, LPARAM lParam)
{
	CP4ListCtrl * pList = DYNAMIC_DOWNCAST(CP4ListCtrl, GetContentWnd());
	if(pList)
		pList->SendMessage(WM_ACTIVATEMODELESS, WA_ACTIVE, NULL);
	else
	{
		CMultiSelTreeCtrl * pTree = DYNAMIC_DOWNCAST(CMultiSelTreeCtrl, GetContentWnd());
		if(pTree)
			pTree->SendMessage(WM_ACTIVATEMODELESS, WA_ACTIVE, NULL);
	}
	return 0;
}
