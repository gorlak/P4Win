// OldChgRevRangeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "P4win.h"
#include "OldChgRevRangeDlg.h"
#include "MainFrm.h"
#include "hlp\p4win.hh"

// COldChgRevRangeDlg dialog

IMPLEMENT_DYNAMIC(COldChgRevRangeDlg, CDialog)
COldChgRevRangeDlg::COldChgRevRangeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COldChgRevRangeDlg::IDD, pParent)
	, m_WinPos(false)
{
	m_WinPos.SetWindow( this, _T("OldChgRevRangeDlg") );
}

COldChgRevRangeDlg::~COldChgRevRangeDlg()
{
}

void COldChgRevRangeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Radio(pDX, IDC_FROM_IS1STREV, m_from);
	DDX_Radio(pDX, IDC_TO_ISHAVEREV, m_to);

	DDX_Check(pDX, IDC_F_ISHAVEEXCLUDE, m_fromIsHaveExclusive);
	DDX_Text(pDX, IDC_F_REVNBR, m_fromRev);
	DDX_Text(pDX, IDC_F_SYMBOL, m_fromSymbol);

	DDX_Check(pDX, IDC_T_ISHAVEINCLUDE, m_toIsHaveInclusive);
	DDX_Text(pDX, IDC_T_REVNBR, m_toRev);
	DDX_Text(pDX, IDC_T_SYMBOL, m_toSymbol);

	DDX_Control(pDX, IDC_F_COMBO, m_fromTypeCombo);
	DDX_Control(pDX, IDC_T_COMBO, m_toTypeCombo);
}


BEGIN_MESSAGE_MAP(COldChgRevRangeDlg, CDialog)
	ON_BN_CLICKED(IDC_FROM_IS1STREV, OnFrom)
	ON_BN_CLICKED(IDC_FROM_ISHAVEREV, OnFrom)
	ON_BN_CLICKED(IDC_FROM_ISREVNBR, OnFrom)
	ON_BN_CLICKED(IDC_F_ISSYMBOL, OnFrom)
	ON_CBN_SELCHANGE(IDC_F_COMBO, OnFromComboValueChg)
	ON_BN_CLICKED(IDC_F_BROWSE, OnFromBrowse)
	ON_BN_CLICKED(IDC_TO_ISHEADREV, OnTo)
	ON_BN_CLICKED(IDC_TO_ISHAVEREV, OnTo)
	ON_BN_CLICKED(IDC_TO_ISREVNBR, OnTo)
	ON_BN_CLICKED(IDC_T_ISSYMBOL, OnTo)
	ON_CBN_SELCHANGE(IDC_T_COMBO, OnToComboValueChg)
	ON_BN_CLICKED(IDC_T_BROWSE, OnToBrowse)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_HELPINFO()
	ON_MESSAGE(WM_BROWSECALLBACK3, OnFromBrowseCallBack)
	ON_MESSAGE(WM_BROWSECALLBACK4, OnToBrowseCallBack)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL COldChgRevRangeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// by default, check exclude/include checkboxes
	CheckDlgButton(IDC_F_ISHAVEEXCLUDE,1);
	CheckDlgButton(IDC_T_ISHAVEINCLUDE,1);

	// Disable some rev range controls
	GetDlgItem(IDC_F_ISHAVEEXCLUDE)->EnableWindow(FALSE);
	GetDlgItem(IDC_F_REVNBR)->EnableWindow(FALSE);
	GetDlgItem(IDC_F_SYMBOL)->EnableWindow(FALSE);
	GetDlgItem(IDC_T_ISHAVEINCLUDE)->EnableWindow(FALSE);
	GetDlgItem(IDC_T_REVNBR)->EnableWindow(FALSE);
	GetDlgItem(IDC_T_SYMBOL)->EnableWindow(FALSE);

	GetDlgItem(IDC_F_STATIC)->ShowWindow(SW_HIDE);
	m_fromTypeCombo.AddString(LoadStringResource(IDS_COMBO_CHGNBR));
	m_fromTypeCombo.AddString(LoadStringResource(IDS_COMBO_LABEL));
	m_fromTypeCombo.AddString(LoadStringResource(IDS_COMBO_DATE));
	m_fromTypeCombo.AddString(LoadStringResource(IDS_COMBO_CLIENT));
	m_fromTypeCombo.SetCurSel(m_fromTypeComboIX);

	GetDlgItem(IDC_T_STATIC)->ShowWindow(SW_HIDE);
	m_toTypeCombo.AddString(LoadStringResource(IDS_COMBO_CHGNBR));
	m_toTypeCombo.AddString(LoadStringResource(IDS_COMBO_LABEL));
	m_toTypeCombo.AddString(LoadStringResource(IDS_COMBO_DATE));
	m_toTypeCombo.AddString(LoadStringResource(IDS_COMBO_CLIENT));
	m_toTypeCombo.SetCurSel(m_toTypeComboIX);

	// Get the size from the RC file
	CRect rect;
	GetWindowRect(&rect);
	// Get the position from the Registry
	m_WinPos.RestoreWindowPosition();
	// Force the width & height to be the height in the RC file
	SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(), SWP_NOMOVE|SWP_NOZORDER);

	OnFrom();
	OnTo();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// COldChgRevRangeDlg message handlers
void COldChgRevRangeDlg::OnFrom()
{
	UpdateData();
	GetDlgItem(IDC_F_ISHAVEEXCLUDE)->EnableWindow(
		IsDlgButtonChecked(IDC_FROM_ISHAVEREV));
	GetDlgItem(IDC_F_REVNBR)->EnableWindow(
		IsDlgButtonChecked(IDC_FROM_ISREVNBR));
	GetDlgItem(IDC_F_COMBO)->EnableWindow(
		IsDlgButtonChecked(IDC_F_ISSYMBOL));
	GetDlgItem(IDC_F_SYMBOL)->EnableWindow(
		IsDlgButtonChecked(IDC_F_ISSYMBOL));
	GetDlgItem(IDC_F_BROWSE)->EnableWindow(
		IsDlgButtonChecked(IDC_F_ISSYMBOL));
	GetDlgItem(IDC_F_STATIC)->EnableWindow(
		IsDlgButtonChecked(IDC_F_ISSYMBOL));
	switch(m_from)
	{
	case 2:
		GotoDlgCtrl(GetDlgItem(IDC_F_REVNBR));
		break;
	case 3:
		OnFromComboValueChg();
		GotoDlgCtrl(GetDlgItem(IDC_F_COMBO));
		break;
	default:
		break;
	}
}

void COldChgRevRangeDlg::OnFromComboValueChg() 
{
	switch(m_fromTypeCombo.GetCurSel())
	{
	default:
	case COMBO_CHGNBR:
		GetDlgItem(IDC_F_BROWSE)->EnableWindow( FALSE );
		GetDlgItem(IDC_F_STATIC)->ShowWindow(SW_HIDE);
		break;
	case COMBO_LABEL:
		GetDlgItem(IDC_F_BROWSE)->EnableWindow( TRUE );
		GetDlgItem(IDC_F_STATIC)->ShowWindow(SW_HIDE);
		break;
	case COMBO_CLIENT:
		GetDlgItem(IDC_F_BROWSE)->EnableWindow( TRUE );
		GetDlgItem(IDC_F_STATIC)->ShowWindow(SW_HIDE);
		break;
	case COMBO_DATE:
		GetDlgItem(IDC_F_BROWSE)->EnableWindow( FALSE );
		GetDlgItem(IDC_F_STATIC)->ShowWindow(SW_SHOW);
		break;
	}
}

void COldChgRevRangeDlg::OnFromBrowse()
{
	HWND hWnd;
	switch(m_fromTypeCombo.GetCurSel())
	{
	case COMBO_CLIENT:
		hWnd = MainFrame()->ClientWnd();
		break;
	case COMBO_LABEL:
		hWnd = MainFrame()->LabelWnd();
		break;
	default:
		hWnd = 0;
		break;
	}
	::SendMessage(hWnd, WM_FETCHOBJECTLIST, (WPARAM)(this->m_hWnd), WM_BROWSECALLBACK3);
}

LRESULT COldChgRevRangeDlg::OnFromBrowseCallBack(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
	CString *str = (CString *)lParam;
	m_fromSymbol = *str;
	UpdateData(FALSE);
	return 0;
}

void COldChgRevRangeDlg::OnTo()
{
	UpdateData();
	GetDlgItem(IDC_T_ISHAVEINCLUDE)->EnableWindow(
		IsDlgButtonChecked(IDC_TO_ISHAVEREV));
	GetDlgItem(IDC_T_REVNBR)->EnableWindow(
		IsDlgButtonChecked(IDC_TO_ISREVNBR));
	GetDlgItem(IDC_T_COMBO)->EnableWindow(
		IsDlgButtonChecked(IDC_T_ISSYMBOL));
	GetDlgItem(IDC_T_SYMBOL)->EnableWindow(
		IsDlgButtonChecked(IDC_T_ISSYMBOL));
	GetDlgItem(IDC_T_BROWSE)->EnableWindow(
		IsDlgButtonChecked(IDC_T_ISSYMBOL));
	GetDlgItem(IDC_T_STATIC)->EnableWindow(
		IsDlgButtonChecked(IDC_T_ISSYMBOL));
	switch(m_to)
	{
	case 2:
		GotoDlgCtrl(GetDlgItem(IDC_T_REVNBR));
		break;
	case 3:
		OnToComboValueChg();
		GotoDlgCtrl(GetDlgItem(IDC_T_COMBO));
		break;
	default:
		break;
	}
}

void COldChgRevRangeDlg::OnToComboValueChg() 
{
	switch(m_toTypeCombo.GetCurSel())
	{
	default:
	case COMBO_CHGNBR:
		GetDlgItem(IDC_T_BROWSE)->EnableWindow( FALSE );
		GetDlgItem(IDC_T_STATIC)->ShowWindow(SW_HIDE);
		break;
	case COMBO_LABEL:
		GetDlgItem(IDC_T_BROWSE)->EnableWindow( TRUE );
		GetDlgItem(IDC_T_STATIC)->ShowWindow(SW_HIDE);
		break;
	case COMBO_CLIENT:
		GetDlgItem(IDC_T_BROWSE)->EnableWindow( TRUE );
		GetDlgItem(IDC_T_STATIC)->ShowWindow(SW_HIDE);
		break;
	case COMBO_DATE:
		GetDlgItem(IDC_T_BROWSE)->EnableWindow( FALSE );
		GetDlgItem(IDC_T_STATIC)->ShowWindow(SW_SHOW);
		break;
	}
}

void COldChgRevRangeDlg::OnToBrowse()
{
	HWND hWnd;
	switch(m_toTypeCombo.GetCurSel())
	{
	case COMBO_CLIENT:
		hWnd = MainFrame()->ClientWnd();
		break;
	case COMBO_LABEL:
		hWnd = MainFrame()->LabelWnd();
		break;
	default:
		hWnd = 0;
		break;
	}
	::SendMessage(hWnd, WM_FETCHOBJECTLIST, (WPARAM)(this->m_hWnd), WM_BROWSECALLBACK4);
}

LRESULT COldChgRevRangeDlg::OnToBrowseCallBack(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
	CString *str = (CString *)lParam;
	m_toSymbol = *str;
	UpdateData(FALSE);
	return 0;
}

void COldChgRevRangeDlg::GetFilterRevRange(CString *filter) 
{
	filter->Empty();
	if (m_from == 0 && m_to == 1)
		return;

	switch(m_from)
	{
	case 0:	// 1st rev
	default:
		*filter = _T("#1");
		break;
	case 1:	// have rev
		*filter = (GET_SERVERLEVEL() >= 11 && m_fromIsHaveExclusive)
				? _T("#>have") : _T("#have");
		break;
	case 2:	// rev number
		*filter = _T("#") + m_fromRev;
		break;
	case 3:	// change number
		*filter = _T("@") + m_fromSymbol;
		break;
	}
	switch(m_to)
	{
	case 0:	// have rev
		*filter += (GET_SERVERLEVEL() >= 11 && !m_toIsHaveInclusive) 
				? _T(",#<have") : _T(",#have");
		break;
	case 1:	// head rev
	default:
		*filter += _T(",#head");
		break;
	case 2:	// rev number
		*filter += _T(",#") + m_toRev;
		break;
	case 3:	// change number
		*filter += _T(",@") + m_toSymbol;
		break;
	}
}

void COldChgRevRangeDlg::OnHelp() 
{
	AfxGetApp()->WinHelp(TASK_FILTERING_CHANGELISTS);
}

BOOL COldChgRevRangeDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}

void COldChgRevRangeDlg::OnOK()
{
	m_WinPos.SaveWindowPosition();

	UpdateData();
	CDialog::OnOK();
}

void COldChgRevRangeDlg::OnCancel()
{
	m_WinPos.SaveWindowPosition();

	CDialog::OnCancel();
}
