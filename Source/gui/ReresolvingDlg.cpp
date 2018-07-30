// ReresolvingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "ReresolvingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReresolvingDlg dialog


CReresolvingDlg::CReresolvingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReresolvingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CReresolvingDlg)
	//}}AFX_DATA_INIT
}


void CReresolvingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReresolvingDlg)
	DDX_Control(pDX, IDC_LIST, m_ListBox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CReresolvingDlg, CDialog)
	//{{AFX_MSG_MAP(CReresolvingDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReresolvingDlg message handlers

BOOL CReresolvingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	for (POSITION pos= m_List.GetHeadPosition(); pos != NULL; )
	{
		m_ListBox.AddString(m_List.GetNext(pos));
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
