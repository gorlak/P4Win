//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// FirstTimeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "FirstTimeDlg.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFirstTimeDlg dialog


CFirstTimeDlg::CFirstTimeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFirstTimeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFirstTimeDlg)
	//}}AFX_DATA_INIT
}


void CFirstTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFirstTimeDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFirstTimeDlg, CDialog)
	//{{AFX_MSG_MAP(CFirstTimeDlg)
	ON_BN_CLICKED(IDC_GETADOBE, OnGetAdobe)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

	
/////////////////////////////////////////////////////////////////////////////
// CFirstTimeDlg message handlers

BOOL CFirstTimeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    // Load the Get Abode bitmap
	m_Bitmap.LoadBitmap(MAKEINTRESOURCE(IDB_GETADOBE));
	GetDlgItem(IDC_GETADOBE)->SendMessage(BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)HBITMAP(m_Bitmap));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFirstTimeDlg::OnGetAdobe()
{
	GotoDlgCtrl(GetDlgItem(IDOK));
	ShellExecute( m_hWnd, _T("open"), LoadStringResource(IDS_WWW_ADOBE), NULL, NULL, SW_SHOWNORMAL);
	TheApp()->StatusAdd(LoadStringResource(IDS_WAIT4ADOBE), SV_MSG);
}

void CFirstTimeDlg::OnOK() 
{
	CDialog::OnOK();
}
