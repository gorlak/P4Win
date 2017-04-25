//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// ReviewList.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "ReviewList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReviewList

CP4CheckListBox::CP4CheckListBox()
{
}

CP4CheckListBox::~CP4CheckListBox()
{
}

BEGIN_MESSAGE_MAP(CP4CheckListBox, CCheckListBox)
	//{{AFX_MSG_MAP(CP4CheckListBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CP4CheckListBox message handlers

void CP4CheckListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CCheckListBox::DrawItem(lpDrawItemStruct);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CReviewList

CReviewList::CReviewList()
{
	m_InitDone = FALSE;
	m_BtnHeight = GetSystemMetrics(SM_CYMENU) + 4;
	m_BtnWidth  = m_BtnHeight*4;
	m_List = new CP4CheckListBox;
	m_SelAll = new CButton;
	m_UnSelAll = new CButton;
	m_nbrItems = m_nbrChked = m_nbrUnCked = m_nbrIndetr = 0;
}

CReviewList::~CReviewList()
{
	delete m_List;
	delete m_SelAll;
	delete m_UnSelAll;
}

BEGIN_MESSAGE_MAP(CReviewList, CWnd)
	//{{AFX_MSG_MAP(CReviewList)
	ON_BN_CLICKED(IDC_SELECT_ALL, OnSelectAll)
	ON_BN_CLICKED(IDC_UNSELECT_ALL, OnUnselectAll)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_CLBN_CHKCHANGE(IDC_P4CHECKLISTBOX, OnChkChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReviewList message handlers

int CReviewList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_List->CreateEx(WS_EX_CLIENTEDGE, _T("ListBox"), _T("P4CheckListBox"), 
				 lpCreateStruct->style | WS_VISIBLE,
                 0, 0, lpCreateStruct->cx, lpCreateStruct->cy - m_BtnHeight,
				 m_hWnd, (HMENU)IDC_P4CHECKLISTBOX);
	m_List->SetFont(GetParent()->GetFont());

	RECT rect;
	rect.left = 2;
	rect.top  = lpCreateStruct->cy - m_BtnHeight;
	rect.right = rect.left + m_BtnWidth;
	rect.bottom = rect.top + m_BtnHeight;
	m_SelAll->Create(LoadStringResource(IDS_SELECT_ALL), 
						WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|WS_DISABLED|WS_TABSTOP, 
						rect, this, IDC_SELECT_ALL);
	m_SelAll->SetFont(GetParent()->GetFont());

	rect.left = lpCreateStruct->cx - m_BtnWidth;
	rect.right = rect.left + m_BtnWidth;
	m_UnSelAll->Create(LoadStringResource(IDS_UNSELECT_ALL), 
						WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|WS_DISABLED|WS_TABSTOP, 
						rect, this, IDC_UNSELECT_ALL);
	m_UnSelAll->SetFont(GetParent()->GetFont());

	SetWindowLong(m_hWnd, GWL_STYLE, WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE);
	SetWindowLong(m_hWnd, GWL_EXSTYLE, WS_EX_CONTROLPARENT | WS_EX_TRANSPARENT);
	GetWindowRect(&m_InitRect);
	m_LastRect = m_InitRect;
	m_InitDone = TRUE;
	return 0;
}

void CReviewList::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);
	if (::IsWindow(m_List->m_hWnd))
		m_List->SetFocus();
}

void CReviewList::OnSelectAll()
{
	GetParent()->SendMessage(WM_COMMAND, IDC_SELECT_ALL, 0);

	m_nbrItems = m_List->GetCount();
	for ( int i = 0; i < m_nbrItems; i++ )
		m_List->SetCheck( i, 1 );

	m_nbrChked = m_nbrItems;
	m_nbrUnCked = m_nbrIndetr = 0;
	m_SelAll->EnableWindow( FALSE );
	m_UnSelAll->EnableWindow( TRUE );

	m_List->SetFocus();
	m_SelAll->SetButtonStyle(WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|WS_DISABLED|WS_TABSTOP);
}

void CReviewList::OnUnselectAll()
{
	GetParent()->SendMessage(WM_COMMAND, IDC_UNSELECT_ALL, 0);

	m_nbrItems = m_List->GetCount();
	for ( int i = 0; i < m_List->GetCount ( ); i++ )
		m_List->SetCheck( i, 0 );

	m_nbrUnCked = m_nbrItems;
	m_nbrChked = m_nbrIndetr = 0;
	m_SelAll->EnableWindow( TRUE );
	m_UnSelAll->EnableWindow( FALSE );

	m_List->SetFocus();
	m_UnSelAll->SetButtonStyle(WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|WS_DISABLED|WS_TABSTOP);
}

void CReviewList::OnSize(UINT nType, int cx, int cy) 
{
	if (!m_InitDone)
		return;

	// Compute the change in width (dx) and height (dy)
	CRect rect;
	GetWindowRect(&rect);
	int dx = rect.Width() - m_LastRect.Width();
	int dy = rect.Height() - m_LastRect.Height();
	if (dy >= -1 && dy <= 1)
		dy = 0;
	if (!dx && !dy)
		return;

	// Save the new size
	m_LastRect = rect;

	// Widen the list box by the change in width (dx) and
	// heighten the list box to the new height (cy)
	m_List->GetWindowRect(&rect);
	m_List->SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								     cy - m_BtnHeight, SWP_NOMOVE | SWP_NOZORDER);

	// Because checklistboxes are possibly an integral height,
	// we have to ask the system where the bottom
	// actually is, and adjust our size accordingly.
	m_List->GetWindowRect(&rect);
	SetWindowPos(NULL, 0, 0, rect.Width(), 
							 rect.Height() + m_BtnHeight, SWP_NOMOVE | SWP_NOZORDER);

	// Slide the Select All button down by the change in height
	ScreenToClient(rect);
	m_SelAll->SetWindowPos(NULL, rect.left+2, rect.bottom, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// Slide the Unselect All button down by the change in height and all the way to the right
	m_UnSelAll->SetWindowPos(NULL, rect.right - m_BtnWidth, 
								   rect.bottom, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

int CReviewList::AddString(LPCTSTR lpszItem)
{
	int i = m_List->AddString(lpszItem);
	if (i != LB_ERR && i != LB_ERRSPACE)
	{
		m_nbrItems++;
		m_nbrUnCked++;
		m_SelAll->EnableWindow( TRUE );
		m_UnSelAll->EnableWindow( m_nbrChked && !m_nbrIndetr );
	}
	return i;
}

void CReviewList::ResetContent( )
{
	m_List->ResetContent( );

	m_nbrItems = m_nbrChked = m_nbrUnCked = m_nbrIndetr = 0;
	m_SelAll->EnableWindow( FALSE );
	m_UnSelAll->EnableWindow( FALSE );
}

void CReviewList::SetCheck(int nItem, BOOL fCheck)
{
	int i = m_List->GetCheck(nItem);
	m_List->SetCheck(nItem, fCheck);
	if (i != fCheck)
	{
		switch(fCheck)
		{
		case 0:
			m_nbrChked--;
			m_nbrUnCked++;
			break;
		case 1:
			m_nbrChked++;
			m_nbrUnCked--;
			break;
		default:
			m_nbrIndetr++;
			break;
		}
		if (m_nbrIndetr)
		{
			m_SelAll->EnableWindow( TRUE );
			m_UnSelAll->EnableWindow( TRUE );
		}
		else
		{
			m_SelAll->EnableWindow( m_nbrChked < m_nbrItems );
			m_UnSelAll->EnableWindow( m_nbrUnCked < m_nbrItems );
		}
	}
}

void CReviewList::OnChkChange()
{
	m_nbrItems = m_List->GetCount();
	m_nbrChked = m_nbrUnCked = m_nbrIndetr = 0;
	for ( int i = 0; i < m_nbrItems; i++ )
	{
		switch(m_List->GetCheck(i))
		{
		case 0:
			m_nbrUnCked++;
			break;
		case 1:
			m_nbrChked++;
			break;
		default:
			m_nbrIndetr++;
			m_SelAll->EnableWindow( TRUE );
			m_UnSelAll->EnableWindow( TRUE );
			return;
		}
	}
	m_SelAll->EnableWindow( m_nbrChked < m_nbrItems );
	m_UnSelAll->EnableWindow( m_nbrUnCked < m_nbrItems );
}
