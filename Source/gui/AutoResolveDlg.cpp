// AutoResolveDlg.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "AutoResolveDlg.h"
#include "Mainfrm.h"
#include "ReresolvingDlg.h"
#include "hlp\p4win.hh"
#include "MsgBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoResolveDlg dialog


CAutoResolveDlg::CAutoResolveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoResolveDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAutoResolveDlg)
	m_ResolveType = m_ResolveDefault = GET_P4REGPTR()->GetResolveDefault();
	m_ResolveWhtSp = GET_P4REGPTR()->GetResolveWhtSp();
	m_ReResolve = FALSE;
	m_AllFiles = 0;
	m_TextMerge = FALSE;
	m_NoSel2Res = FALSE;
	m_ResolveFromChgList = FALSE;
	//}}AFX_DATA_INIT
	m_Preview = m_SelResolved = m_AnyResolved = FALSE;
}


void CAutoResolveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoResolveDlg)
	DDX_Radio(pDX, IDC_ACCEPTTHEIRS, m_ResolveType);
	DDX_Check(pDX, IDC_RERESOLVE, m_ReResolve);
	DDX_Radio(pDX, IDC_SELECTEDFILES, m_AllFiles);
	DDX_Check(pDX, IDC_TEXTMERGE, m_TextMerge);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAutoResolveDlg, CDialog)
	//{{AFX_MSG_MAP(CAutoResolveDlg)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_PREVIEW, OnPreview)
	ON_BN_CLICKED(IDC_ALLFILES, OnAllfiles)
	ON_BN_CLICKED(IDC_SELECTEDFILES, OnSelectedfiles)
	ON_BN_CLICKED(IDC_DEFAUTORSLV, OnSetDefResolveType)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoResolveDlg message handlers

BOOL CAutoResolveDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (m_ResolveFromChgList)
	{
		CString str = LoadStringResource(IDS_ALL_FILES_IN_SELECTED_CHANGE);
		GetDlgItem(IDC_SELECTEDFILES)->SetWindowText(str);
	}
	OnSelectedfiles();
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAutoResolveDlg::OnHelp() 
{
	AfxGetApp()->WinHelp(TASK_RESOLVING_FILES);
}

BOOL CAutoResolveDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}

void CAutoResolveDlg::OnOK() 
{
	UpdateData(TRUE);
	if (m_ResolveType == -1)
	{
		AfxMessageBox(IDS_MUSTSELECTAUTORESOLVETYPE, MB_ICONERROR);
		m_Preview = FALSE;	// clear this - they may hit OK next time
		return;
	}
	if (!m_Preview)
	{
		if (m_ResolveType == 0)
		{
			if (IDC_BUTTON1 != MsgBox(IDS_AUTORESOLVEACCEPTTHEIRSWARNING,
							MB_DEFBUTTON2 | MB_ICONQUESTION))
				return;
		}
		if (m_ReResolve)
		{
			if (m_AllFiles)
				 m_pDeltaTreeCtrl->AnyMyFilesResolved(TRUE);
			else m_pDeltaTreeCtrl->AnyResolvedFiles(TRUE);
			CStringList * list = m_pDeltaTreeCtrl->GetStringList();
			CReresolvingDlg dlg;
			for (POSITION pos= list->GetHeadPosition(); pos != NULL; )
			{
				dlg.m_List.AddHead(list->GetNext(pos));
			}
			if (dlg.DoModal() != IDOK)
				return;
		}
	}
	CDialog::OnOK();
}

void CAutoResolveDlg::OnPreview() 
{
	m_Preview = TRUE;
	OnOK();
}

void CAutoResolveDlg::OnAllfiles() 
{
	(GetDlgItem(IDC_RERESOLVE))->EnableWindow( m_AnyResolved );
	if (m_NoSel2Res)
	{
		(GetDlgItem(IDC_ACCEPTTHEIRS))->EnableWindow( TRUE );
		(GetDlgItem(IDC_ACCEPTYOURS))->EnableWindow( TRUE );
		(GetDlgItem(IDC_SAFERESOLVE))->EnableWindow( TRUE );
		(GetDlgItem(IDC_AUTORESOLVE))->EnableWindow( TRUE );
		(GetDlgItem(IDC_AUTOWITHMARKERS))->EnableWindow( TRUE );
		(GetDlgItem(IDC_TEXTMERGE))->EnableWindow( TRUE );
		(GetDlgItem(IDOK))->EnableWindow( TRUE );
		(GetDlgItem(IDC_PREVIEW))->EnableWindow( TRUE );
	}
}

void CAutoResolveDlg::OnSelectedfiles() 
{
	if (m_NoSel2Res)
	{
		(GetDlgItem(IDC_ACCEPTTHEIRS))->EnableWindow( FALSE );
		(GetDlgItem(IDC_ACCEPTYOURS))->EnableWindow( FALSE );
		(GetDlgItem(IDC_SAFERESOLVE))->EnableWindow( FALSE );
		(GetDlgItem(IDC_AUTORESOLVE))->EnableWindow( FALSE );
		(GetDlgItem(IDC_AUTOWITHMARKERS))->EnableWindow( FALSE );
		(GetDlgItem(IDC_TEXTMERGE))->EnableWindow( FALSE );
		(GetDlgItem(IDOK))->EnableWindow( FALSE );
		(GetDlgItem(IDC_PREVIEW))->EnableWindow( FALSE );
		(GetDlgItem(IDC_RERESOLVE))->EnableWindow( FALSE );
	}
	else (GetDlgItem(IDC_RERESOLVE))->EnableWindow( m_SelResolved );
}

void CAutoResolveDlg::OnSetDefResolveType() 
{
	UpdateData(TRUE);
	MainFrame()->OnPerforceOptions(TRUE, FALSE, 0, IDS_PAGE_AUTORESOLVE);
	int i = GET_P4REGPTR()->GetResolveDefault();
	if (i != m_ResolveDefault)
	{
		m_ResolveType = m_ResolveDefault = i;
		UpdateData(FALSE);
	}
}
