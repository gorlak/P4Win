// RevRange.cpp : implementation file
//

#include "stdafx.h"
#include "..\p4win.h"
#include "RevRange.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIntegRevRange property page

IMPLEMENT_DYNCREATE(CIntegRevRange, CPropertyPage)

CIntegRevRange::CIntegRevRange() : CPropertyPage(CIntegRevRange::IDD)
{
	//{{AFX_DATA_INIT(CIntegRevRange)
	m_f_revnbr = _T("");
	m_f_symbol = _T("");
	m_t_revnbr = _T("");
	m_t_symbol = _T("");
	m_from_flag = 0;
	m_to_flag = 0;
	m_RevRangeFrom = _T("");
	m_RevRangeTo = _T("");
	//}}AFX_DATA_INIT

	m_f_TypeVal = 0;
	m_t_TypeVal = 0;
	m_Initialized = FALSE;
}

CIntegRevRange::~CIntegRevRange()
{
}

void CIntegRevRange::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIntegRevRange)
	DDX_Text(pDX, IDC_F_REVNBR, m_f_revnbr);
	DDX_Text(pDX, IDC_F_SYMBOL, m_f_symbol);
	DDX_Text(pDX, IDC_T_REVNBR, m_t_revnbr);
	DDX_Text(pDX, IDC_T_SYMBOL, m_t_symbol);
	DDX_Radio(pDX, IDC_F_IS1STREV, m_from_flag);
	DDX_Radio(pDX, IDC_T_ISHEADREV, m_to_flag);
	DDX_Control(pDX, IDC_F_COMBO, m_f_TypeCombo);
	DDX_Control(pDX, IDC_T_COMBO, m_t_TypeCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIntegRevRange, CPropertyPage)
	//{{AFX_MSG_MAP(CIntegRevRange)
	ON_BN_CLICKED(IDC_F_IS1STREV, OnFromClick)
	ON_BN_CLICKED(IDC_F_ISREVNBR, OnFromClick)
	ON_BN_CLICKED(IDC_F_ISSYMBOL, OnFromClick)
	ON_BN_CLICKED(IDC_F_BROWSE, OnFromBrowse)
	ON_CBN_SELCHANGE(IDC_F_COMBO, OnFromComboValueChg)
	ON_BN_CLICKED(IDC_T_ISHEADREV, OnToClick)
	ON_BN_CLICKED(IDC_T_ISREVNBR, OnToClick)
	ON_BN_CLICKED(IDC_T_ISSYMBOL, OnToClick)
	ON_BN_CLICKED(IDC_T_BROWSE, OnToBrowse)
	ON_CBN_SELCHANGE(IDC_T_COMBO, OnToComboValueChg)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_BROWSECALLBACK1, OnFromBrowseCallBack)
	ON_MESSAGE(WM_BROWSECALLBACK2, OnToBrowseCallBack)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIntegRevRange message handlers

int CIntegRevRange::DoModal() 
{
	return CPropertyPage::DoModal();
}

BOOL CIntegRevRange::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	m_Initialized = TRUE;
	switch(m_from_flag)
	{
	case 1:
		m_f_revnbr = m_RevRangeFrom;
		break;
	case 2:
		m_f_symbol = m_RevRangeFrom;
		break;
	}
	switch(m_to_flag)
	{
	case 1:
		m_t_revnbr = m_RevRangeTo;
		break;
	case 2:
		m_t_symbol = m_RevRangeTo;
		break;
	}

	GetDlgItem(IDC_F_STATIC)->ShowWindow(SW_HIDE);
	m_f_TypeCombo.AddString(LoadStringResource(IDS_COMBO_CHGNBR));
	m_f_TypeCombo.AddString(LoadStringResource(IDS_COMBO_LABEL));
	m_f_TypeCombo.AddString(LoadStringResource(IDS_COMBO_DATE));
	m_f_TypeCombo.AddString(LoadStringResource(IDS_COMBO_CLIENT));
	m_f_TypeCombo.SetCurSel(m_f_TypeVal);

	GetDlgItem(IDC_T_STATIC)->ShowWindow(SW_HIDE);
	m_t_TypeCombo.AddString(LoadStringResource(IDS_COMBO_CHGNBR));
	m_t_TypeCombo.AddString(LoadStringResource(IDS_COMBO_LABEL));
	m_t_TypeCombo.AddString(LoadStringResource(IDS_COMBO_DATE));
	m_t_TypeCombo.AddString(LoadStringResource(IDS_COMBO_CLIENT));
	m_t_TypeCombo.SetCurSel(m_t_TypeVal);

	UpdateData(FALSE);
	OnFromClick();
	OnToClick();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CIntegRevRange::OnFromClick() 
{
	UpdateData();
	GetDlgItem(IDC_F_REVNBR)->EnableWindow( FALSE );
	GetDlgItem(IDC_F_SYMBOL)->EnableWindow( FALSE );
	switch(m_from_flag)
	{
	case 1:
			GetDlgItem(IDC_F_REVNBR)->EnableWindow( TRUE );
			GetDlgItem(IDC_F_COMBO)->EnableWindow( FALSE );
			GetDlgItem(IDC_F_BROWSE)->EnableWindow( FALSE );
			GotoDlgCtrl(GetDlgItem(IDC_F_REVNBR));
			break;
	case 2:
			GetDlgItem(IDC_F_SYMBOL)->EnableWindow( TRUE );
			GetDlgItem(IDC_F_COMBO)->EnableWindow( TRUE );
			OnFromComboValueChg();
			GotoDlgCtrl(GetDlgItem(IDC_F_COMBO));
			break;
	default:
			GetDlgItem(IDC_F_COMBO)->EnableWindow( FALSE );
			GetDlgItem(IDC_F_BROWSE)->EnableWindow( FALSE );
			break;
	}
}

void CIntegRevRange::OnFromComboValueChg() 
{
	switch(m_f_TypeVal = m_f_TypeCombo.GetCurSel())
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

void CIntegRevRange::OnFromBrowse()
{
	HWND hWnd;
	switch(m_f_TypeVal = m_f_TypeCombo.GetCurSel())
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
	::SendMessage(hWnd, WM_FETCHOBJECTLIST, (WPARAM)(this->m_hWnd), WM_BROWSECALLBACK1);
}

LRESULT CIntegRevRange::OnFromBrowseCallBack(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
	CString *str = (CString *)lParam;
	m_f_symbol = *str;
	UpdateData(FALSE);
	return 0;
}

void CIntegRevRange::OnToClick() 
{
	UpdateData();
	GetDlgItem(IDC_T_REVNBR)->EnableWindow( FALSE );
	GetDlgItem(IDC_T_SYMBOL)->EnableWindow( FALSE );
	switch(m_to_flag)
	{
	case 1:
			GetDlgItem(IDC_T_REVNBR)->EnableWindow( TRUE );
			GetDlgItem(IDC_T_COMBO)->EnableWindow( FALSE );
			GetDlgItem(IDC_T_BROWSE)->EnableWindow( FALSE );
			GotoDlgCtrl(GetDlgItem(IDC_T_REVNBR));
			break;
	case 2:
			GetDlgItem(IDC_T_SYMBOL)->EnableWindow( TRUE );
			GetDlgItem(IDC_T_COMBO)->EnableWindow( TRUE );
			OnToComboValueChg();
			GotoDlgCtrl(GetDlgItem(IDC_T_COMBO));
			break;
	default:
			GetDlgItem(IDC_T_COMBO)->EnableWindow( FALSE );
			GetDlgItem(IDC_T_BROWSE)->EnableWindow( FALSE );
			break;
	}
}

void CIntegRevRange::OnToComboValueChg() 
{
	switch(m_t_TypeVal = m_t_TypeCombo.GetCurSel())
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

void CIntegRevRange::OnToBrowse()
{
	HWND hWnd;
	switch(m_t_TypeVal = m_t_TypeCombo.GetCurSel())
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
	::SendMessage(hWnd, WM_FETCHOBJECTLIST, (WPARAM)(this->m_hWnd), WM_BROWSECALLBACK2);
}

LRESULT CIntegRevRange::OnToBrowseCallBack(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
	CString *str = (CString *)lParam;
	m_t_symbol = *str;
	UpdateData(FALSE);
	return 0;
}


void CIntegRevRange::OnOK() 
{
	m_RevRangeFrom.Empty();
	m_RevRangeTo.Empty();
	UpdateData();
	switch(m_from_flag)
	{
	case 1:
		m_RevRangeFrom = m_f_revnbr;
		break; 
	case 2: 
		m_RevRangeFrom = m_f_symbol;
		break; 
	} 
	switch(m_to_flag) 
	{ 
	case 1: 
		m_RevRangeTo = m_t_revnbr;
		break; 
	case 2: 
		m_RevRangeTo = m_t_symbol;
		break; 
	}
	m_RevRangeFrom.TrimLeft(_T(" @#"));
	m_RevRangeFrom.TrimRight();
	m_RevRangeTo.TrimLeft(_T(" @#"));
	m_RevRangeTo.TrimRight();
	CPropertyPage::OnOK();
}
