//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// RevertListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "RevertListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CRevertListDlg dialog


CRevertListDlg::CRevertListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRevertListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRevertListDlg)

	//}}AFX_DATA_INIT
	m_InitRect.SetRect(0,0,0,0);
	m_Initialized = m_InitDialogDone = m_DeletedRows = FALSE;
	m_WinPos.SetWindow( this, _T("RevertListDlg") );
}


void CRevertListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRevertListDlg)
	DDX_Control(pDX, ID_DELETESELECTED, m_Delete);
	DDX_Control(pDX, IDC_STATICRECT, m_StaticRect);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRevertListDlg, CDialog)
	//{{AFX_MSG_MAP(CRevertListDlg)
	ON_BN_CLICKED(ID_DELETESELECTED, OnDeleteselected)
	ON_LBN_SELCHANGE(IDC_THELIST, OnSelchangeList)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CRevertListDlg::Init(CStringList *strList)
{
	ASSERT_KINDOF(CStringList, strList);
	m_pStrList=strList;
	m_Initialized=TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CRevertListDlg message handlers

BOOL CRevertListDlg::OnInitDialog() 
{
	ASSERT(m_Initialized);
	
	CDialog::OnInitDialog();
	
	GetWindowRect(&m_InitRect);
	m_LastRect = m_InitRect;

	// Fill the file list
	CRect rect;
	m_StaticRect.GetWindowRect(&rect);
	ScreenToClient(rect);
	DWORD style = WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | LBS_SORT | 
					LBS_STANDARD | LBS_HASSTRINGS | LBS_EXTENDEDSEL | LBS_NOINTEGRALHEIGHT;

	m_List.CreateEx(WS_EX_CLIENTEDGE, _T("ListBox"), _T(""),style,
                 rect.left, rect.top, rect.Width(), rect.Height(),
				 this->m_hWnd, (HMENU) IDC_THELIST);
	
	m_List.SetFont(GetFont());
	
	for(POSITION pos=m_pStrList->GetHeadPosition(); pos != NULL; )
	{
		int i;
		CString str = m_pStrList->GetNext(pos);
		if (GET_SERVERLEVEL() >= 14)	// 2002.2 or later?
		{
			if ((i = str.ReverseFind(_T('#'))) != -1)
				str = str.Left(i);
		}
		m_List.AddString(str);
	}

	// The Delete button is disabled till an item is selected
	m_Delete.ShowWindow(SW_SHOWNOACTIVATE);
	m_Delete.EnableWindow(FALSE);

	// Restore window position
	m_InitDialogDone = TRUE;
	m_WinPos.RestoreWindowPosition();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CRevertListDlg::OnDeleteselected() 
{
	for(int i=m_List.GetCount()-1; i>=0; i--)
	{
		if(m_List.GetSel(i))
		{
			m_List.DeleteString(i);
			m_DeletedRows=TRUE;
		}
	}
	m_Delete.EnableWindow(FALSE);
}

void CRevertListDlg::OnSelchangeList() 
{
	if(m_List.GetSelCount() > 0)
		m_Delete.EnableWindow(TRUE);
	else
		m_Delete.EnableWindow(FALSE);
}

void CRevertListDlg::OnOK() 
{
	m_WinPos.SaveWindowPosition();
	if(m_DeletedRows)
	{
		CString txt;
		m_pStrList->RemoveAll();

		for(int i=0; i< m_List.GetCount(); i++)
		{
			m_List.GetText(i, txt);
			m_pStrList->AddTail(txt);
		}
	}

	CDialog::OnOK();
}

void CRevertListDlg::OnCancel() 
{
	m_WinPos.SaveWindowPosition();
	CDialog::OnCancel();
}


void CRevertListDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	if (!m_InitDialogDone)
		return;

	// Compute the change in width (dx)
	CRect rect;
	GetWindowRect(&rect);
	int dx = rect.Width() - m_LastRect.Width();
	int dy = rect.Height() - m_LastRect.Height();
	// Save the new size
	m_LastRect = rect;

	// Widen the list box by the change in width (dx) and
	// heighten the list box by the change in height (dy)
	m_List.GetWindowRect(&rect);
	m_List.SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								    rect.bottom - rect.top + dy, SWP_NOMOVE | SWP_NOZORDER);

	// Slide the buttons to the right by the change in width
	// and down by the change in height
	CWnd *pWnd = GetDlgItem(IDOK);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);

	pWnd = GetDlgItem(IDCANCEL);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);

	pWnd = GetDlgItem(ID_DELETESELECTED);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);
}

void CRevertListDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	if (m_InitRect.Height())
	{
		lpMMI->ptMinTrackSize.x= m_InitRect.Width();
		lpMMI->ptMinTrackSize.y= m_InitRect.Height();
	}
}
