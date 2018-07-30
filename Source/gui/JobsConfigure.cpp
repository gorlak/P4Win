// JobsConfigure.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "JobsConfigure.h"
#include "JobView.h"
#include "Mainfrm.h"
#include "TokenString.h"
#include "..\SpecDescDlg.h"
#include "hlp\p4win.hh"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJobsConfigure dialog


CJobsConfigure::CJobsConfigure(CWnd* pParent /*=NULL*/)
	: CDialog(CJobsConfigure::IDD, pParent)
{
	//{{AFX_DATA_INIT(CJobsConfigure)
	//}}AFX_DATA_INIT

	m_ColNames = _T("");
}


void CJobsConfigure::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJobsConfigure)
	DDX_Control(pDX, IDC_LIST_SHOW, m_ListShow);
	DDX_Control(pDX, IDC_LIST_OTHER, m_ListOther);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CJobsConfigure, CDialog)
	//{{AFX_MSG_MAP(CJobsConfigure)
	ON_BN_CLICKED(IDIGNORE, OnSetToDefault)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_LBN_SETFOCUS(IDC_LIST_OTHER, OnSetfocusListOther)
	ON_LBN_SETFOCUS(IDC_LIST_SHOW, OnSetfocusListShow)
	ON_LBN_SELCHANGE(IDC_LIST_SHOW, OnSelchangeListShow)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDC_DOWN, OnDown)
	ON_LBN_SELCHANGE(IDC_LIST_OTHER, OnSelchangeListOther)
	ON_LBN_DBLCLK(IDC_LIST_OTHER, OnDblclkListOther)
	ON_BN_CLICKED(ID_FORMINFO, OnFormInfo)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_P4DESCRIBE, OnP4Describe )
    ON_MESSAGE(WM_P4ENDDESCRIBE, OnP4EndDescribe )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJobsConfigure message handlers

BOOL CJobsConfigure::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CTokenString tokstr;
	CString token, str;
	BOOL bInShow;
	int i;

	tokstr.Create(m_ColNames);
	tokstr.PrepareParse( );
	token=tokstr.GetToken();  
	while(!token.IsEmpty())
	{
		m_ListShow.AddString(token);
		token=tokstr.GetToken();
	}
	tokstr.Reset();

	tokstr.Create(m_SpecNames);
	tokstr.PrepareParse( );
	token=tokstr.GetToken();  
	while(!token.IsEmpty())
	{
		for (bInShow = FALSE, i = -1; ++i < m_ListShow.GetCount(); )
		{
			m_ListShow.GetText(i, str);
			if (str == token)
			{
				bInShow = TRUE;
				break;
			}
		}
		if (!bInShow)
			m_ListOther.AddString(token);
		token=tokstr.GetToken();
	}
	m_ListOther.SetCurSel(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CJobsConfigure::OnSetToDefault() 
{
	EndDialog( IDIGNORE );
}

void CJobsConfigure::OnHelp() 
{
	AfxGetApp()->WinHelp(TASK_DISPLAYING_JOBS);
}

BOOL CJobsConfigure::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}

void CJobsConfigure::OnOK() 
{
	UpdateData( );
	m_ColNames.Empty();
	CString str;

	for (int i = -1; ++i < m_ListShow.GetCount(); )
	{
		m_ListShow.GetText(i, str);
		m_ColNames += str + _T(' ');
	}
	CDialog::OnOK();
}

void CJobsConfigure::OnSetfocusListOther() 
{
	m_ListShow.SetCurSel(-1);
	GetDlgItem(IDC_REMOVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_UP)->EnableWindow(FALSE);
	GetDlgItem(IDC_DOWN)->EnableWindow(FALSE);
	OnSelchangeListOther();
}

void CJobsConfigure::OnSelchangeListOther() 
{
	GetDlgItem(IDC_ADD)->EnableWindow(((m_ListShow.GetCount() < MAX_JOBS_COLUMNS)
									&&  m_ListOther.GetCount()
									&& (m_ListOther.GetCurSel() > -1)) ? TRUE : FALSE);
}

void CJobsConfigure::OnSetfocusListShow() 
{
	m_ListOther.SetCurSel(-1);
	OnSelchangeListShow();
}

void CJobsConfigure::OnSelchangeListShow() 
{
	int i = m_ListShow.GetCurSel();
	GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
	GetDlgItem(IDC_REMOVE)->EnableWindow(i > 0 ? TRUE : FALSE);
	GetDlgItem(IDC_UP)->EnableWindow(i > 1 ? TRUE : FALSE);
	GetDlgItem(IDC_DOWN)->EnableWindow(((i > 0) && (i < (m_ListShow.GetCount()-1))) ? TRUE : FALSE);
}

void CJobsConfigure::OnDblclkListOther() 
{
	OnAdd();
}

void CJobsConfigure::OnAdd() 
{
	CString str;
	m_ListOther.GetText(m_ListOther.GetCurSel(), str);
	m_ListOther.DeleteString(m_ListOther.GetCurSel());
	m_ListShow.AddString(str);
	m_ListShow.SetCurSel(m_ListShow.GetCount() - 1);
	m_ListShow.SetFocus();
}

void CJobsConfigure::OnRemove() 
{
	CString str;
	m_ListShow.GetText(m_ListShow.GetCurSel(), str);
	m_ListShow.DeleteString(m_ListShow.GetCurSel());
	m_ListOther.AddString(str);
	m_ListOther.SetCurSel(m_ListOther.GetCount() - 1);
	m_ListOther.SetFocus();
}

void CJobsConfigure::OnUp() 
{
	int i = m_ListShow.GetCurSel();
    // Job column must stay at top of list, so only 3rd and later items can be moved up
	if (i < 2)
		return;
	CString str;
	m_ListShow.GetText(i, str);
	m_ListShow.DeleteString(i);
    i--;
	m_ListShow.InsertString(i, str);
	m_ListShow.SetCurSel(i);
    if(i == 1)
    	GetDlgItem(IDC_UP)->EnableWindow(FALSE);
   	GetDlgItem(IDC_DOWN)->EnableWindow(TRUE);
}

void CJobsConfigure::OnDown() 
{
	int i = m_ListShow.GetCurSel();
    // Job column must stay at top of list, and nothing can move below bottom of list
    if (i < 1 || i == m_ListShow.GetCount() - 1)
		return;
	CString str;
	m_ListShow.GetText(i, str);
	m_ListShow.DeleteString(i);
    i++;
	m_ListShow.InsertString(i, str);
	m_ListShow.SetCurSel(i);
    if(i == m_ListShow.GetCount() - 1)
        GetDlgItem(IDC_DOWN)->EnableWindow(FALSE);
    GetDlgItem(IDC_UP)->EnableWindow(TRUE);
}

void CJobsConfigure::OnFormInfo()
{
	CCmd_Describe *pCmd = new CCmd_Describe;
	pCmd->Init( m_hWnd, RUN_ASYNC );
	if( pCmd->Run( P4JOB_SPEC, _T("0") ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_FETCHING_SPEC) );	
	else
		delete pCmd;
	GotoDlgCtrl(GetDlgItem(IDC_LIST_OTHER));
}

LRESULT CJobsConfigure::OnP4Describe( WPARAM wParam, LPARAM lParam )
{
	CCmd_Describe *pCmd = ( CCmd_Describe * )wParam;

	MainFrame()->ClearStatus();
	if(!pCmd->GetError())
	{
		int i;
		CString desc = MakeCRs( pCmd->GetDescription( ) );
		if ((i = desc.ReverseFind(_T('#'))) != -1)
		{
			if ((i = desc.Find(_T('\n'), i)) != -1)
				desc = desc.Left(i+1);
		}
		int key;
		CSpecDescDlg *dlg = new CSpecDescDlg(this);
		dlg->SetIsModeless(TRUE);
		dlg->SetKey(key = pCmd->HaveServerLock()? pCmd->GetServerKey() : 0);
		dlg->SetDescription( desc, FALSE );
		dlg->SetCaption( LoadStringResource(IDS_P4WIN_SPECIFICATION_NOTES) );
		dlg->SetWinPosName(_T("JobSpecInfo"));
		CRect rect;
		GetWindowRect(&rect);
		rect.top += rect.Height() - 10;
		rect.bottom = GetSystemMetrics(SM_CYFULLSCREEN);
		rect.right  = GetSystemMetrics(SM_CXFULLSCREEN);
		dlg->SetWinPosDefault(rect);
		if (!dlg->Create(IDD_SPECDESC, this))	// display the description dialog box
		{
			dlg->DestroyWindow();	// some error! clean up
			delete dlg;
		}
	}
	delete pCmd;
	GotoDlgCtrl(GetDlgItem(IDC_LIST_OTHER));
	return 0;
}

LRESULT CJobsConfigure::OnP4EndDescribe(WPARAM wParam, LPARAM lParam)
{
	CSpecDescDlg *dlg = (CSpecDescDlg *)lParam;
	dlg->DestroyWindow();
	return TRUE;
}
