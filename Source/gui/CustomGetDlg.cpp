// CustomGetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "CustomGetDlg.h"
#include "MainFrm.h"
#include "p4api\P4Command.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomGetDlg dialog


CCustomGetDlg::CCustomGetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCustomGetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCustomGetDlg)
	m_LabelText = _T("");
	m_RevText = _T("");
	m_Radio = -1;
	m_Only = m_Force = FALSE;
	m_DepotWnd = pParent->m_hWnd;
	//}}AFX_DATA_INIT

	m_IsMinimized = FALSE;
	m_NbrSel = -1;
}

CCustomGetDlg::~CCustomGetDlg()
{
}

void CCustomGetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustomGetDlg)
	DDX_Text(pDX, IDC_EDITLABEL, m_LabelText);
	DDV_MaxChars(pDX, m_LabelText, 128);
	DDX_Text(pDX, IDC_REVNBR, m_RevText);
	DDV_MaxChars(pDX, m_RevText, 8);
	DDX_Radio(pDX, IDC_ISREVNBR, m_Radio);
	DDX_Control(pDX, IDC_COMBO, m_TypeCombo);
	DDX_Check(pDX, IDC_CHECKONLY, m_Only);
	DDX_Check(pDX, IDC_FORCE_RESYNC, m_Force);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCustomGetDlg, CDialog)
	//{{AFX_MSG_MAP(CCustomGetDlg)
	ON_BN_CLICKED(IDGETPREVIEW, OnGetpreview)
	ON_BN_CLICKED(IDGET, OnGet)
	ON_BN_CLICKED(IDC_ISREVNBR, OnRadioClick)
	ON_BN_CLICKED(IDC_ISSYMBOL, OnRadioClick)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_CBN_SELCHANGE(IDC_COMBO, OnComboValueChg)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_BROWSECALLBACK1, OnBrowseCallBack)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomGetDlg message handlers

BOOL CCustomGetDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	MainFrame()->SetModelessWnd(this);

	GetDlgItem(IDC_DATELABEL)->ShowWindow(SW_HIDE);
	m_TypeCombo.AddString(LoadStringResource(IDS_COMBO_CHGNBR));
	m_TypeCombo.AddString(LoadStringResource(IDS_COMBO_LABEL));
	m_TypeCombo.AddString(LoadStringResource(IDS_COMBO_DATE));
	m_TypeCombo.AddString(LoadStringResource(IDS_COMBO_CLIENT));
	m_TypeCombo.SetCurSel(0);
	
	int init = GET_P4REGPTR()->GetSyncDlgFlag();
	if (init >= 0)
	{
		if (!init)
			m_Radio = 0;
		else
		{
			m_Radio = 1;
			m_TypeCombo.SetCurSel(init - 1);
		}
	}

	UpdateData(FALSE);
	OnRadioClick();

	ShowWindow(SW_SHOW);
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCustomGetDlg::OnGetpreview() 
{
	UpdateData(TRUE);
	switch(m_Radio)
	{
	case 0:
		m_RevText.TrimLeft(_T(" #"));
		if(m_RevText.GetLength() == 0 || m_RevText.FindOneOf(_T("#@")) != -1)
		{
			AfxMessageBox(IDS_REVISION_NUMBER_IS_INVALID, MB_ICONEXCLAMATION);
			return;
		}
		if (m_RevText == _T("0"))
			m_RevText  = _T("none");
		else if (!_istdigit(m_RevText[0]) && m_RevText != _T("none") 
			  && m_RevText != _T("head") && m_RevText != _T("have"))
		{
			AfxMessageBox(IDS_REVISION_NUMBER_IS_INVALID, MB_ICONEXCLAMATION);
			return;
		}
		m_LabelText= _T("#") + m_RevText;
		if (m_RevText != _T("none") && m_NbrSel > 1 
		 && IDYES != AfxMessageBox(IDS_SYNC_MULTIFILES_TO_REVNBR, 
									MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2))
			return;
		GET_P4REGPTR()->SetSyncDlgFlag(0);
		break;

	case 1:
		m_LabelText.TrimLeft(_T(" @"));
		m_LabelText.TrimRight();
		if (m_LabelText.GetLength() == 0 || (m_Only && (m_LabelText.FindOneOf(_T("#@")) != -1)))
		{
			if (m_LabelText != _T("#none"))
			{
				GotoDlgCtrl(GetDlgItem(IDC_EDITLABEL));
				AfxMessageBox(IDS_CHGLABDATCLI_IS_INVALID, MB_ICONEXCLAMATION);
				return;
			}
		}
		if (m_LabelText.GetAt(0) != _T('#'))
		{
			m_LabelText = _T("@") + m_LabelText;
			if (m_Only)
			{
				if ((m_LabelText.Find(_T('/')) != -1) 
				 && (m_LabelText.Find(_T(':')) == -1))
					m_LabelText = m_LabelText + _T(":00:00:00,") + m_LabelText + _T(":23:59:59");
				else
					m_LabelText += _T(",") + m_LabelText;
			}
		}
		GET_P4REGPTR()->SetSyncDlgFlag(1 + m_TypeCombo.GetCurSel());
		break;
	}

	::PostMessage(m_DepotWnd, WM_DOCUSTOMGET,
					m_Force ? (WPARAM)IDGETFORCEPREVIEW : (WPARAM)IDGETPREVIEW,
					(LPARAM)((LPCTSTR)m_LabelText));
}

void CCustomGetDlg::OnGet() 
{
	UpdateData(TRUE);
	switch(m_Radio)
	{
	case 0:
		m_RevText.TrimLeft(_T(" #"));
		if(m_RevText.GetLength() == 0 || m_RevText.FindOneOf(_T("#@")) != -1)
		{
			AfxMessageBox(IDS_REVISION_NUMBER_IS_INVALID, MB_ICONEXCLAMATION);
			return;
		}
		if (m_RevText == _T("0"))
			m_RevText  = _T("none");
		else if (!_istdigit(m_RevText[0]) && m_RevText != _T("none") 
			  && m_RevText != _T("head") && m_RevText != _T("have"))
		{
			AfxMessageBox(IDS_REVISION_NUMBER_IS_INVALID, MB_ICONEXCLAMATION);
			return;
		}
		m_LabelText= _T("#") + m_RevText;
		if (m_RevText != _T("none") && m_NbrSel > 1 
		 && IDYES != AfxMessageBox(IDS_SYNC_MULTIFILES_TO_REVNBR, 
									MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2))
			return;
		GET_P4REGPTR()->SetSyncDlgFlag(0);
		break;

	case 1:
		m_LabelText.TrimLeft(_T(" @"));
		m_LabelText.TrimRight();
		if (m_LabelText.GetLength() == 0 || (m_Only && (m_LabelText.FindOneOf(_T("#@")) != -1)))
		{
			if (m_LabelText != _T("#none"))
			{
				GotoDlgCtrl(GetDlgItem(IDC_EDITLABEL));
				AfxMessageBox(IDS_CHGLABDATCLI_IS_INVALID, MB_ICONEXCLAMATION);
				return;
			}
		}
		if (m_LabelText.GetAt(0) != _T('#'))
		{
			m_LabelText = _T("@") + m_LabelText;
			if (m_Only)
			{
				if ((m_LabelText.Find(_T('/')) != -1) 
				 && (m_LabelText.Find(_T(':')) == -1))
					m_LabelText = m_LabelText + _T(":00:00:00,") + m_LabelText + _T(":23:59:59");
				else
					m_LabelText += _T(",") + m_LabelText;
			}
		}
		GET_P4REGPTR()->SetSyncDlgFlag(1 + m_TypeCombo.GetCurSel());
		break;
	}

	::PostMessage(m_DepotWnd, WM_DOCUSTOMGET,
					m_Force ? (WPARAM)IDGETFORCE : (WPARAM)IDGET, (LPARAM)((LPCTSTR)m_LabelText));
}

void CCustomGetDlg::OnCancel() 
{
	::PostMessage(m_DepotWnd, WM_DOCUSTOMGET, (WPARAM)IDCANCEL, (LPARAM)0);
}

// This signals the closing of a modeless dialog
// to MainFrame which will delete the 'this' object
void CCustomGetDlg::OnDestroy()
{
	::PostMessage(MainFrame()->m_hWnd, WM_P4DLGDESTROY, 0, (LPARAM)this);
}

void CCustomGetDlg::OnRadioClick() 
{
	UpdateData(TRUE);
	GetDlgItem(IDGET)->EnableWindow( TRUE );
	GetDlgItem(IDGETPREVIEW)->EnableWindow( TRUE );
	GetDlgItem(IDC_EDITLABEL)->EnableWindow( FALSE );
	GetDlgItem(IDC_REVNBR)->EnableWindow( FALSE );
	GetDlgItem(IDC_COMBO)->EnableWindow( FALSE );
	GetDlgItem(IDC_CHECKONLY)->EnableWindow( FALSE );
	switch(m_Radio)
	{
	case 0:
			GetDlgItem(IDC_REVNBR)->EnableWindow( TRUE );
			GotoDlgCtrl(GetDlgItem(IDC_REVNBR));
			break;
	case 1:
			GetDlgItem(IDC_EDITLABEL)->EnableWindow( TRUE );
			GetDlgItem(IDC_COMBO)->EnableWindow( TRUE );
			GetDlgItem(IDC_CHECKONLY)->EnableWindow( TRUE );
			OnComboValueChg();
			GotoDlgCtrl(GetDlgItem(IDC_COMBO));
			break;
	default:
			break;
	}
}

void CCustomGetDlg::OnComboValueChg() 
{
	switch(m_TypeCombo.GetCurSel())
	{
	default:
	case COMBO_CHGNBR:
		GetDlgItem(IDC_BROWSE)->EnableWindow( TRUE );
		GetDlgItem(IDC_DATELABEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECKONLY)->SetWindowText(LoadStringResource(IDS_SYNCONLYCHG));
		break;
	case COMBO_LABEL:
		GetDlgItem(IDC_BROWSE)->EnableWindow( TRUE );
		GetDlgItem(IDC_DATELABEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECKONLY)->SetWindowText(LoadStringResource(IDS_SYNCONLYLABEL));
		break;
	case COMBO_CLIENT:
		GetDlgItem(IDC_BROWSE)->EnableWindow( TRUE );
		GetDlgItem(IDC_DATELABEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECKONLY)->SetWindowText(LoadStringResource(IDS_SYNCONLYCLIENT));
		break;
	case COMBO_DATE:
		GetDlgItem(IDC_BROWSE)->EnableWindow( FALSE );
		GetDlgItem(IDC_DATELABEL)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHECKONLY)->SetWindowText(LoadStringResource(IDS_SYNCONLYDATE));
		break;
	}
}

void CCustomGetDlg::OnBrowse()
{
	HWND hWnd;
	switch(m_TypeCombo.GetCurSel())
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
	GotoDlgCtrl(GetDlgItem(IDC_EDITLABEL));
}

LRESULT CCustomGetDlg::OnBrowseCallBack(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
	CString *str = (CString *)lParam;
	m_LabelText = *str;
	UpdateData(FALSE);
	GotoDlgCtrl(GetDlgItem(IDC_EDITLABEL));
	return 0;
}

void CCustomGetDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	if (nType == SIZE_MINIMIZED)
	{
		m_IsMinimized = TRUE;
		return;
	}
	else if (m_IsMinimized)
	{
		m_IsMinimized = FALSE;
		return;
	}
}

void CCustomGetDlg::OnSysCommand(UINT nID, LPARAM lParam) 
{
	switch(nID)
	{
	case SC_MINIMIZE:
		GetDesktopWindow()->ArrangeIconicWindows();
		break;
	}

	CDialog::OnSysCommand(nID, lParam);
}
