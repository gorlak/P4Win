// OldChgFilterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "P4win.h"
#include "OldChgFilterDlg.h"
#include "OldChgRevRangeDlg.h"
#include "MainFrm.h"
#include "hlp\p4win.hh"

// COldChgFilterDlg dialog

IMPLEMENT_DYNAMIC(COldChgFilterDlg, CDialog)
COldChgFilterDlg::COldChgFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COldChgFilterDlg::IDD, pParent)
	, m_WinPos(false)
{
	m_includeIntegrations = m_UseClientSyntax = m_bPending = FALSE;
	m_InitRect = m_LastRect = CRect(0,0,0,0);
	m_WinPos.SetWindow( this, _T("OldChgFilterDlg") );
	m_filter = _T("");

	m_from = 0;
	m_fromIsHaveExclusive = TRUE;
	m_fromRev = _T("");
	m_fromSymbol = _T("");
	m_fromTypeComboIX = 0;
	m_to = 1;
	m_toIsHaveInclusive = TRUE;
	m_toRev = _T("");
	m_toSymbol = _T("");
	m_toTypeComboIX = 0;
}

COldChgFilterDlg::~COldChgFilterDlg()
{
}

void COldChgFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_USE_CLIENT, m_useClient);
	DDX_Text(pDX, IDC_CLIENT, m_client);
	DDV_MaxChars(pDX, m_client, 1024);

	DDX_Check(pDX, IDC_USE_USER, m_useUser);
	DDX_Text(pDX, IDC_USER, m_user);
	DDV_MaxChars(pDX, m_user, 1024);

	DDX_Check(pDX, IDC_INCLUDE_INTEGS, m_includeIntegrations);
	DDX_Check(pDX, IDC_USE_CLIENT_SYNTAX, m_UseClientSyntax);

	DDX_Control(pDX, IDC_FILESPEC, m_fileCombo);
	DDX_Text(pDX, IDC_SELECTED, m_selectedFiles);

	DDX_Radio(pDX, IDC_FILE_ANY, m_filterFiles);
}


BEGIN_MESSAGE_MAP(COldChgFilterDlg, CDialog)
	ON_BN_CLICKED(IDC_USE_CLIENT, OnClient)
	ON_BN_CLICKED(IDC_BROWSE_CLIENTS, OnBrowseClients)
	ON_BN_CLICKED(IDC_USE_USER, OnUser)
	ON_BN_CLICKED(IDC_BROWSE_USERS, OnBrowseUsers)
	ON_BN_CLICKED(IDC_FILE_ANY, OnFile)
	ON_BN_CLICKED(IDC_FILE_FILESPEC, OnFile)
	ON_BN_CLICKED(IDC_FILE_SELECTED, OnFile)
	ON_BN_CLICKED(IDC_SETREVRANGE, OnSetRevRange)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_HELPINFO()
	ON_MESSAGE(WM_BROWSECALLBACK1, OnBrowseClientsCallBack)
	ON_MESSAGE(WM_BROWSECALLBACK2, OnBrowseUsersCallBack)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL COldChgFilterDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (m_bPending || !GET_P4REGPTR()->GetEnableSubChgIntegFilter( ))
		GetDlgItem(IDC_INCLUDE_INTEGS)->ShowWindow(SW_HIDE);

	if (m_bPending)
	{
		GetDlgItem(IDC_SETREVRANGE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_USE_CLIENT)->EnableWindow( FALSE );
		GetDlgItem(IDC_USE_USER)->EnableWindow( FALSE );
		SetWindowText(LoadStringResource(IDS_FILTER_PENDINGCHGS_OTHERCLIENTS));
		if (GET_SERVERLEVEL() < 21)	// earlier than 2006.1?
		{
			GetDlgItem(IDC_FILE_FILESPEC)->EnableWindow( FALSE );
			GetDlgItem(IDC_FILESPEC)->EnableWindow( FALSE );
			GetDlgItem(IDC_FILE_SELECTED)->EnableWindow( FALSE );
			GetDlgItem(IDC_SELECTED)->EnableWindow( FALSE );
			GetDlgItem(IDC_USE_CLIENT_SYNTAX)->EnableWindow( FALSE );
			GetDlgItem(IDC_SETREVRANGE)->EnableWindow( FALSE );
		}
	}

	LoadFilterComboBox();
	OnClient();
	OnUser();
	OnFile();
	
	if (m_selectedFiles.IsEmpty())
		GetDlgItem(IDC_FILE_SELECTED)->EnableWindow( FALSE );

	// Record the initial window size, and then see if there is a registry preference
	GetWindowRect(&m_InitRect);
	m_LastRect = m_InitRect;
	m_WinPos.RestoreWindowPosition();
	// Force the height to be the height in the RC file
	CRect rDlg;
	GetWindowRect(&rDlg);
	SetWindowPos(NULL, 0, 0, rDlg.Width(), m_InitRect.Height(), SWP_NOMOVE|SWP_NOZORDER);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COldChgFilterDlg::LoadFilterComboBox()
{
	// Make sure the combo is empty
	m_fileCombo.ResetContent();
	m_fileCombo.Clear();

	// Load all MRU filters into list box
	for(int i=0; i < MAX_MRUCHG_FILTERS; i++)
	{
		if(GET_P4REGPTR()->GetMRUChgFilter(i).GetLength() > 0)
		{
			CString str = GET_P4REGPTR()->GetMRUChgFilter(i);
			str.TrimRight();
			str.Replace(_T("\t//"), _T(" //"));
			m_fileCombo.AddString(str);
		}
	}

	// Select the first item
	m_fileCombo.SetCurSel(0);
}


// COldChgFilterDlg message handlers

void COldChgFilterDlg::OnClient()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_CLIENT)->EnableWindow(m_useClient);
	GetDlgItem(IDC_BROWSE_CLIENTS)->EnableWindow(m_useClient);
	if (m_useClient)
		GotoDlgCtrl(GetDlgItem(IDC_CLIENT));
}

void COldChgFilterDlg::OnBrowseClients()
{
	::SendMessage(MainFrame()->ClientWnd(), WM_FETCHOBJECTLIST, 
					(WPARAM)(this->m_hWnd), WM_BROWSECALLBACK1);
	GotoDlgCtrl(GetDlgItem(IDC_CLIENT));
}

LRESULT COldChgFilterDlg::OnBrowseClientsCallBack(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
	CString *str = (CString *)lParam;
	m_client = *str;
	UpdateData(FALSE);
	GotoDlgCtrl(GetDlgItem(IDC_CLIENT));
	return 0;
}

void COldChgFilterDlg::OnUser()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_USER)->EnableWindow(m_useUser);
	GetDlgItem(IDC_BROWSE_USERS)->EnableWindow(m_useUser);
	if (m_useUser)
		GotoDlgCtrl(GetDlgItem(IDC_USER));
}

void COldChgFilterDlg::OnBrowseUsers()
{
	::SendMessage(MainFrame()->UserWnd(), WM_FETCHOBJECTLIST, 
					(WPARAM)(this->m_hWnd), WM_BROWSECALLBACK2);
	GotoDlgCtrl(GetDlgItem(IDC_USER));
}

LRESULT COldChgFilterDlg::OnBrowseUsersCallBack(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
	CString *str = (CString *)lParam;
	m_user = *str;
	UpdateData(FALSE);
	GotoDlgCtrl(GetDlgItem(IDC_USER));
	return 0;
}

void COldChgFilterDlg::OnFile()
{
	int fileChoice = GetCheckedRadioButton(IDC_FILE_ANY, IDC_FILE_LASTSELECTED);
	switch(fileChoice)
	{
	case IDC_FILE_ANY:
        GetDlgItem(IDC_FILESPEC)->EnableWindow(FALSE);
        GetDlgItem(IDC_USE_CLIENT_SYNTAX)->EnableWindow(FALSE);
		GetDlgItem(IDC_SETREVRANGE)->EnableWindow(FALSE);
		break;
	case IDC_FILE_FILESPEC:
        GetDlgItem(IDC_FILESPEC)->EnableWindow(TRUE);
        GetDlgItem(IDC_USE_CLIENT_SYNTAX)->EnableWindow(FALSE);
		GetDlgItem(IDC_SETREVRANGE)->EnableWindow(FALSE);
		break;
	case IDC_FILE_SELECTED:
        GetDlgItem(IDC_FILESPEC)->EnableWindow(FALSE);
        GetDlgItem(IDC_USE_CLIENT_SYNTAX)->EnableWindow(TRUE);
		GetDlgItem(IDC_SETREVRANGE)->EnableWindow(TRUE);
		break;
	}
}

void COldChgFilterDlg::OnSetRevRange()
{
	UpdateData();
	COldChgRevRangeDlg dlg;
	dlg.m_from = m_from;
	dlg.m_fromIsHaveExclusive = m_fromIsHaveExclusive;
	dlg.m_fromRev = m_fromRev;
	dlg.m_fromSymbol = m_fromSymbol;
	dlg.m_fromTypeComboIX = m_fromTypeComboIX;
	dlg.m_to = m_to;
	dlg.m_toIsHaveInclusive = m_toIsHaveInclusive;
	dlg.m_toRev = m_toRev;
	dlg.m_toSymbol = m_toSymbol;
	dlg.m_toTypeComboIX = m_toTypeComboIX;

	if(dlg.DoModal() == IDCANCEL)
		return;
	dlg.GetFilterRevRange(&m_filter);

	m_from = dlg.m_from;
	m_fromIsHaveExclusive = dlg.m_fromIsHaveExclusive;
	m_fromRev = dlg.m_fromRev;
	m_fromSymbol = dlg.m_fromSymbol;
	m_fromTypeComboIX = dlg.m_fromTypeComboIX;
	m_to = dlg.m_to;
	m_toIsHaveInclusive = dlg.m_toIsHaveInclusive;
	m_toRev = dlg.m_toRev;
	m_toSymbol = dlg.m_toSymbol;
	m_toTypeComboIX = dlg.m_toTypeComboIX;

	m_selectedFiles.Empty();
	POSITION pos = m_selected.GetHeadPosition();
	for(int i=0; pos != NULL; i++)
		m_selectedFiles += m_selected.GetNext(pos) + m_filter + _T(' ');

	UpdateData(FALSE);
}

void COldChgFilterDlg::OnHelp() 
{
	AfxGetApp()->WinHelp(TASK_FILTERING_CHANGELISTS);
}

BOOL COldChgFilterDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}

void COldChgFilterDlg::OnOK()
{
	m_WinPos.SaveWindowPosition();

	UpdateData();
	if(m_fileCombo.GetCurSel() != CB_ERR)
		m_fileCombo.GetLBText(m_fileCombo.GetCurSel(), m_filespec);
	else
		m_fileCombo.GetWindowText( m_filespec );

	CDialog::OnOK();
}

void COldChgFilterDlg::OnCancel()
{
	m_WinPos.SaveWindowPosition();

	CDialog::OnCancel();
}

void COldChgFilterDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	if (m_InitRect.Height())
	{
		lpMMI->ptMinTrackSize.x= m_InitRect.Width();
		lpMMI->ptMinTrackSize.y= lpMMI->ptMaxTrackSize.y= m_InitRect.Height();
	}
}

void COldChgFilterDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	CWnd * pWnd = GetDlgItem(IDC_FILES);
	// Have we finished initializing?
	if (!pWnd || !IsWindow(pWnd->m_hWnd) || !m_LastRect.Width())
		return;

	// Compute the change in width (dx)
	CRect rect;
	GetWindowRect(&rect);
	int dx = rect.Width() - m_LastRect.Width();
	// Save the new size
	m_LastRect = rect;

	// Widen the group boxes by the change in width (dx)
	pWnd->GetWindowRect(&rect);
	pWnd->SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								   rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);

	pWnd = GetDlgItem(IDC_CRITERIA);
	pWnd->GetWindowRect(&rect);
	pWnd->SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								   rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);

	// Widen the 2 file combo/edit boxes by the change in width
	pWnd = GetDlgItem(IDC_FILESPEC);
	pWnd->GetWindowRect(&rect);
	pWnd->SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								   rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
	pWnd = GetDlgItem(IDC_SELECTED);
	pWnd->GetWindowRect(&rect);
	pWnd->SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								   rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);

	// Slide the buttons to the right by the change in width
	pWnd = GetDlgItem(IDC_SETREVRANGE);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);

	pWnd = GetDlgItem(IDOK);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);

	pWnd = GetDlgItem(IDCANCEL);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);

	pWnd = GetDlgItem(IDHELP);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);
}
