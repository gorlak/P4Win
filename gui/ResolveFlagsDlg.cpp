// ResolveFlagsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "ResolveFlagsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CResolveFlagsDlg dialog


CResolveFlagsDlg::CResolveFlagsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CResolveFlagsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CResolveFlagsDlg)
	m_ReResolve = FALSE;
	m_TextualMerge = FALSE;
	//}}AFX_DATA_INIT
}


void CResolveFlagsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResolveFlagsDlg)
	DDX_Check(pDX, IDC_RE_RESOLVE, m_ReResolve);
	DDX_Check(pDX, IDC_RESOLVE_BINARY, m_TextualMerge);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CResolveFlagsDlg, CDialog)
	//{{AFX_MSG_MAP(CResolveFlagsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResolveFlagsDlg message handlers


BOOL CResolveFlagsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (m_TextualMerge)
		m_TextualMerge = FALSE;
	else
		GetDlgItem(IDC_RESOLVE_BINARY)->EnableWindow(FALSE);

	if (m_ReResolve)
		m_ReResolve = FALSE;
	else
		GetDlgItem(IDC_RE_RESOLVE)->EnableWindow(FALSE);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
