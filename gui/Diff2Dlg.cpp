// Diff2Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "Diff2Dlg.h"
#include "MainFrm.h"
#include "hlp\p4win.hh"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDiff2Dlg dialog


CDiff2Dlg::CDiff2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDiff2Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDiff2Dlg)
	m_Edit1 = _T("");
	m_Edit2 = _T("");
	m_RevNbr1 = _T("");
	m_RevNbr2 = _T("");
	m_Symbol1 = _T("");
	m_Symbol2 = _T("");
	m_RevRadio1 = 0;
	m_RevRadio2 = 0;
	m_DefaultRadio1 = GET_P4REGPTR()->GetDiff2Default1();
	m_DefaultRadio2 = GET_P4REGPTR()->GetDiff2Default2();
	//}}AFX_DATA_INIT
	m_InitRect = CRect(0,0,0,0);
	m_LastWidth = 0;
	m_IsFolders = FALSE;
}


void CDiff2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDiff2Dlg)
	DDX_Text(pDX, IDC_EDIT1, m_Edit1);
	DDX_Text(pDX, IDC_EDIT2, m_Edit2);
	DDX_Text(pDX, IDC_F_REVNBR, m_RevNbr1);
	DDX_Text(pDX, IDC_T_REVNBR, m_RevNbr2);
	DDX_Text(pDX, IDC_F_SYMBOL, m_Symbol1);
	DDX_Text(pDX, IDC_T_SYMBOL, m_Symbol2);
	DDX_Radio(pDX, IDC_USEHEADREV, m_RevRadio1);
	DDX_Radio(pDX, IDC_USEHEADREV2, m_RevRadio2);
	DDX_Radio(pDX, IDC_RADIO_1_HEAD, m_DefaultRadio1);
	DDX_Radio(pDX, IDC_RADIO_2_HEAD, m_DefaultRadio2);
	DDX_Control(pDX, IDC_F_COMBO, m_TypeCombo1);
	DDX_Control(pDX, IDC_T_COMBO, m_TypeCombo2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDiff2Dlg, CDialog)
	//{{AFX_MSG_MAP(CDiff2Dlg)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_USEHEADREV, OnUseHeadRev1)
	ON_BN_CLICKED(IDC_F_ISREVNBR, OnFIsrevnbr1)
	ON_BN_CLICKED(IDC_F_ISSYMBOL, OnFIssymbol1)
	ON_BN_CLICKED(IDC_CLIENTFILE, OnUseClientFile1)
	ON_BN_CLICKED(IDC_F_BROWSE, On1Browse)
	ON_CBN_SELCHANGE(IDC_F_COMBO, On1ComboValueChg)
	ON_BN_CLICKED(IDC_USEHEADREV2, OnUseHeadRev2)
	ON_BN_CLICKED(IDC_T_ISREVNBR, OnFIsrevnbr2)
	ON_BN_CLICKED(IDC_T_ISSYMBOL, OnFIssymbol2)
	ON_BN_CLICKED(IDC_CLIENTFILE2, OnUseClientFile2)
	ON_BN_CLICKED(IDC_T_BROWSE, On2Browse)
	ON_CBN_SELCHANGE(IDC_T_COMBO, On2ComboValueChg)
	ON_WM_HELPINFO()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_BROWSECALLBACK1, On1BrowseCallBack)
	ON_MESSAGE(WM_BROWSECALLBACK2, On2BrowseCallBack)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiff2Dlg message handlers

BOOL CDiff2Dlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	GetWindowRect(&m_InitRect);
	m_LastWidth = m_InitRect.Width();

	int radio1 = m_DefaultRadio1;

	if (m_IsFolders)
	{
		SetWindowText(LoadStringResource(IDS_DIFF2FOLDERSCAPTION));
		GetDlgItem(IDC_STATIC_1)->SetWindowText(LoadStringResource(IDS_FIRSTFOLDER));
		GetDlgItem(IDC_STATIC_2)->SetWindowText(LoadStringResource(IDS_SECONDFOLDER));
		GetDlgItem(IDC_CLIENTFILE)->SetWindowText(LoadStringResource(IDS_DIRHAVEREV));
		GetDlgItem(IDC_CLIENTFILE2)->SetWindowText(LoadStringResource(IDS_DIRHAVEREV2));
		GetDlgItem(IDC_RADIO_1_HEAD)->EnableWindow( FALSE );
		GetDlgItem(IDC_RADIO_1_CLIENT)->EnableWindow( FALSE );
		GetDlgItem(IDC_RADIO_1_REV)->EnableWindow( FALSE );
		GetDlgItem(IDC_RADIO_2_HEAD)->EnableWindow( FALSE );
		GetDlgItem(IDC_RADIO_2_CLIENT)->EnableWindow( FALSE );
		GetDlgItem(IDC_RADIO_2_REV)->EnableWindow( FALSE );
		GetDlgItem(IDC_RADIO_1_HEAD)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_1_CLIENT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_1_REV)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_2_HEAD)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_2_CLIENT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_2_REV)->ShowWindow(SW_HIDE);
	}
	else
	{
		if (!m_HeadRev1)
		{
			GetDlgItem(IDC_USEHEADREV)->EnableWindow( FALSE );
			if (!radio1)
				radio1 = 1;
		}
		if (m_HaveRev1.IsEmpty())
		{
			radio1 = 1;
			GetDlgItem(IDC_USEHEADREV)->EnableWindow( FALSE );
			GetDlgItem(IDC_F_ISREVNBR)->EnableWindow( FALSE );
			GetDlgItem(IDC_F_ISSYMBOL)->EnableWindow( FALSE );
		}
		else if (m_HaveRev1 == _T('0'))
		{
			GetDlgItem(IDC_CLIENTFILE)->EnableWindow( FALSE );
			if (radio1 == 1)
				radio1 =  2;
		}
		if (m_RevNbr1 == _T('0'))
			m_RevNbr1.Empty();

		switch(m_RevRadio1 = radio1)
		{
		case 0:
			break;
		case 1:
			OnUseClientFile1();
			break;
		case 2:
			OnFIsrevnbr1();
			break;
		case 3:
			OnFIssymbol1();
			break;
		}

		int radio2 = m_DefaultRadio2;

		if (!m_HeadRev2)
		{
			GetDlgItem(IDC_USEHEADREV2)->EnableWindow( FALSE );
			if (!radio2)
				radio2 = 1;
		}
		if (m_HaveRev2.IsEmpty())
		{
			radio2 = 1;
			GetDlgItem(IDC_USEHEADREV2)->EnableWindow( FALSE );
			GetDlgItem(IDC_T_ISREVNBR)->EnableWindow( FALSE );
			GetDlgItem(IDC_T_ISSYMBOL)->EnableWindow( FALSE );
		}
		else if (m_HaveRev2 == _T('0'))
		{
			GetDlgItem(IDC_CLIENTFILE2)->EnableWindow( FALSE );
			if (radio2 == 1)
				radio2 =  2;
		}
		if (m_RevNbr2 == _T('0'))
			m_RevNbr2.Empty();

		switch(m_RevRadio2 = radio2)
		{
		case 0:
			break;
		case 1:
			OnUseClientFile2();
			break;
		case 2:
			OnFIsrevnbr2();
			break;
		case 3:
			OnFIssymbol2();
			break;
		}
	}

	GetDlgItem(IDC_F_STATIC)->ShowWindow(SW_HIDE);
	m_TypeCombo1.AddString(LoadStringResource(IDS_COMBO_CHGNBR));
	m_TypeCombo1.AddString(LoadStringResource(IDS_COMBO_LABEL));
	m_TypeCombo1.AddString(LoadStringResource(IDS_COMBO_DATE));
	m_TypeCombo1.AddString(LoadStringResource(IDS_COMBO_CLIENT));
	m_TypeCombo1.SetCurSel(0);

	GetDlgItem(IDC_T_STATIC)->ShowWindow(SW_HIDE);
	m_TypeCombo2.AddString(LoadStringResource(IDS_COMBO_CHGNBR));
	m_TypeCombo2.AddString(LoadStringResource(IDS_COMBO_LABEL));
	m_TypeCombo2.AddString(LoadStringResource(IDS_COMBO_DATE));
	m_TypeCombo2.AddString(LoadStringResource(IDS_COMBO_CLIENT));
	m_TypeCombo2.SetCurSel(0);

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDiff2Dlg::OnUseHeadRev1() 
{
	GetDlgItem(IDC_F_REVNBR)->EnableWindow( FALSE );
	GetDlgItem(IDC_F_SYMBOL)->EnableWindow( FALSE );
	GetDlgItem(IDC_F_COMBO)->EnableWindow( FALSE );
	GetDlgItem(IDC_F_BROWSE)->EnableWindow( FALSE );
}

void CDiff2Dlg::OnUseClientFile1()
{
	OnUseHeadRev1();
}

void CDiff2Dlg::OnFIsrevnbr1() 
{
	GetDlgItem(IDC_F_REVNBR)->EnableWindow( TRUE );
	GetDlgItem(IDC_F_SYMBOL)->EnableWindow( FALSE );
	GetDlgItem(IDC_F_COMBO)->EnableWindow( FALSE );
	GetDlgItem(IDC_F_BROWSE)->EnableWindow( FALSE );
	GotoDlgCtrl(GetDlgItem(IDC_F_REVNBR));
}

void CDiff2Dlg::OnFIssymbol1() 
{
	GetDlgItem(IDC_F_REVNBR)->EnableWindow( FALSE );
	GetDlgItem(IDC_F_SYMBOL)->EnableWindow( TRUE );
	GetDlgItem(IDC_F_COMBO)->EnableWindow( TRUE );
	On1ComboValueChg();
	GotoDlgCtrl(GetDlgItem(IDC_F_COMBO));
}

void CDiff2Dlg::On1ComboValueChg() 
{
	switch(m_TypeCombo1.GetCurSel())
	{
	default:
	case COMBO_CHGNBR:
		GetDlgItem(IDC_F_BROWSE)->EnableWindow( TRUE );
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

void CDiff2Dlg::On1Browse()
{
	HWND hWnd;
	switch(m_TypeCombo1.GetCurSel())
	{
	case COMBO_CLIENT:
		hWnd = MainFrame()->ClientWnd();
		break;
	case COMBO_LABEL:
		hWnd = MainFrame()->LabelWnd();
		break;
	case COMBO_CHGNBR:
		hWnd = MainFrame()->OldChgsWnd();
		break;
	default:
		hWnd = 0;
		break;
	}
	::SendMessage(hWnd, WM_FETCHOBJECTLIST, (WPARAM)(this->m_hWnd), WM_BROWSECALLBACK1);
	GotoDlgCtrl(GetDlgItem(IDC_F_SYMBOL));
}

LRESULT CDiff2Dlg::On1BrowseCallBack(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
	CString *str = (CString *)lParam;
	m_Symbol1 = *str;
	UpdateData(FALSE);
	GotoDlgCtrl(GetDlgItem(IDC_F_SYMBOL));
	return 0;
}

void CDiff2Dlg::OnUseHeadRev2() 
{
	GetDlgItem(IDC_T_REVNBR)->EnableWindow( FALSE );
	GetDlgItem(IDC_T_SYMBOL)->EnableWindow( FALSE );
	GetDlgItem(IDC_T_COMBO)->EnableWindow( FALSE );
	GetDlgItem(IDC_T_BROWSE)->EnableWindow( FALSE );
}

void CDiff2Dlg::OnUseClientFile2()
{
	OnUseHeadRev2();
}

void CDiff2Dlg::OnFIsrevnbr2() 
{
	GetDlgItem(IDC_T_REVNBR)->EnableWindow( TRUE );
	GetDlgItem(IDC_T_SYMBOL)->EnableWindow( FALSE );
	GotoDlgCtrl(GetDlgItem(IDC_T_REVNBR));
	GetDlgItem(IDC_T_COMBO)->EnableWindow( FALSE );
	GetDlgItem(IDC_T_BROWSE)->EnableWindow( FALSE );
}

void CDiff2Dlg::OnFIssymbol2() 
{
	GetDlgItem(IDC_T_REVNBR)->EnableWindow( FALSE );
	GetDlgItem(IDC_T_SYMBOL)->EnableWindow( TRUE );
	GetDlgItem(IDC_T_COMBO)->EnableWindow( TRUE );
	On2ComboValueChg();
	GotoDlgCtrl(GetDlgItem(IDC_T_COMBO));
}

void CDiff2Dlg::On2ComboValueChg() 
{
	switch(m_TypeCombo2.GetCurSel())
	{
	default:
	case COMBO_CHGNBR:
		GetDlgItem(IDC_T_BROWSE)->EnableWindow( TRUE );
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

void CDiff2Dlg::On2Browse()
{
	HWND hWnd;
	switch(m_TypeCombo2.GetCurSel())
	{
	case COMBO_CLIENT:
		hWnd = MainFrame()->ClientWnd();
		break;
	case COMBO_LABEL:
		hWnd = MainFrame()->LabelWnd();
		break;
	case COMBO_CHGNBR:
		hWnd = MainFrame()->OldChgsWnd();
		break;
	default:
		hWnd = 0;
		break;
	}
	::SendMessage(hWnd, WM_FETCHOBJECTLIST, (WPARAM)(this->m_hWnd), WM_BROWSECALLBACK2);
	GotoDlgCtrl(GetDlgItem(IDC_T_SYMBOL));
}

LRESULT CDiff2Dlg::On2BrowseCallBack(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
	CString *str = (CString *)lParam;
	m_Symbol2 = *str;
	UpdateData(FALSE);
	GotoDlgCtrl(GetDlgItem(IDC_T_SYMBOL));
	return 0;
}

void CDiff2Dlg::OnHelp() 
{
	AfxGetApp()->WinHelp(m_IsFolders ? TASK_COMPARING_FOLDERS : ALIAS_99_COMPARING_ANY_2_FILES);
}

BOOL CDiff2Dlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}

void CDiff2Dlg::OnOK() 
{
	UpdateData();

	int r1 = m_RevRadio1 ? m_RevRadio1==1 ? _tstoi(m_HaveRev1) : _tstoi(m_RevNbr1) : m_HeadRev1;
	int r2 = m_RevRadio2 ? m_RevRadio2==1 ? _tstoi(m_HaveRev2) : _tstoi(m_RevNbr2) : m_HeadRev2;
	if ((!r1 && m_RevRadio1 < 3 && m_RevRadio1 != 1)
	 || (!r2 && m_RevRadio2 < 3 && m_RevRadio2 != 1))
	{
		CString txt;
		txt.FormatMessage(IDS_CANT_DIFF_0_REV_FILES_s_n_s_n, 
			m_Edit1, r1, m_Edit2, r2);
		AfxMessageBox(txt);
		return;
	}

	if (m_RevRadio1 == 2)
	{
		m_RevNbr1.TrimRight();
		m_RevNbr1.TrimLeft();
		m_RevNbr1.TrimLeft(_T("#@"));
		if (!m_RevNbr1.IsEmpty())
			m_Edit1 += _T('#') + m_RevNbr1;
	}
	else if (m_RevRadio1 == 3)
	{
		m_Symbol1.TrimRight();
		m_Symbol1.TrimLeft();
		m_Symbol1.TrimLeft(_T("#@"));
		if (!m_Symbol1.IsEmpty())
			m_Edit1 += _T('@') + m_Symbol1;
	}
	if (m_RevRadio2 == 2)
	{
		m_RevNbr2.TrimRight();
		m_RevNbr2.TrimLeft();
		m_RevNbr2.TrimLeft(_T("#@"));
		if (!m_RevNbr2.IsEmpty())
			m_Edit2 += _T('#') + m_RevNbr2;
	}
	else if (m_RevRadio2 == 3)
	{
		m_Symbol2.TrimRight();
		m_Symbol2.TrimLeft();
		m_Symbol2.TrimLeft(_T("#@"));
		if (!m_Symbol2.IsEmpty())
			m_Edit2 += _T('@') + m_Symbol2;
	}
	GET_P4REGPTR()->SetDiff2Default1(m_DefaultRadio1);
	GET_P4REGPTR()->SetDiff2Default2(m_DefaultRadio2);
	UpdateData(FALSE);
	CDialog::OnOK();
}

void CDiff2Dlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	if (m_InitRect.Height())
	{
		lpMMI->ptMinTrackSize.x= m_InitRect.Width();
		lpMMI->ptMinTrackSize.y= lpMMI->ptMaxTrackSize.y= m_InitRect.Height();
	}
}

void CDiff2Dlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	CWnd *pWnd=GetDlgItem(IDC_EDIT1);
	if (!pWnd || !IsWindow(pWnd->m_hWnd))
		return;

	// Compute the change in width (dx)
	CRect rect;
	GetWindowRect(&rect);
	int dx = rect.Width() - m_LastWidth;
	// Save the new width
	m_LastWidth = rect.Width();

	// Widen the edit and group boxes by the change in width (dx)
	pWnd->GetWindowRect(&rect);
	pWnd->SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								   rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
	pWnd = GetDlgItem(IDC_EDIT2);
	pWnd->GetWindowRect(&rect);
	pWnd->SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								   rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
	pWnd = GetDlgItem(IDC_STATIC1);
	pWnd->GetWindowRect(&rect);
	pWnd->SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								   rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
	pWnd = GetDlgItem(IDC_STATIC2);
	pWnd->GetWindowRect(&rect);
	pWnd->SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								   rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
	// Slide the buttons to the right by the change in width
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
