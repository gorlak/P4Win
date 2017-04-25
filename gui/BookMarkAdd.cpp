// BookMarkAdd.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "BookMarkAdd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBookMarkAdd dialog


CBookMarkAdd::CBookMarkAdd(CWnd* pParent /*=NULL*/)
	: CDialog(CBookMarkAdd::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBookMarkAdd)
	m_MenuType = 0;
	//}}AFX_DATA_INIT
	m_LabelText=_T("");
	m_IsOK2Cr8SubMenu = FALSE;
	m_RadioShow = m_ChgPCU = 0;
}


void CBookMarkAdd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBookMarkAdd)
	DDX_Text(pDX, IDC_EDIT, m_Name);
	DDX_Radio(pDX, IDC_COMMAND, m_MenuType);
	DDX_Radio(pDX, IDC_RADIO1, m_ChgPCU);
	DDX_Text(pDX, IDC_PORT, m_Port);
	DDX_Text(pDX, IDC_CLIENT, m_Client);
	DDX_Text(pDX, IDC_USER, m_User);
	DDX_Text(pDX, IDC_DESCRIPTION, m_Desc);
	DDV_MaxChars(pDX, m_Desc, 32);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBookMarkAdd, CDialog)
	//{{AFX_MSG_MAP(CBookMarkAdd)
	ON_BN_CLICKED(IDC_COMMAND, OnCommand)
	ON_BN_CLICKED(IDC_SUBMENU, OnSubMenu)
	ON_BN_CLICKED(IDC_RADIO1, OnSamePCU)
	ON_BN_CLICKED(IDC_RADIO2, OnDiffPCU)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBookMarkAdd message handlers

BOOL CBookMarkAdd::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowText(m_Title);
	if (m_LabelText.GetLength())
		GetDlgItem(IDC_LABEL_TEXT)->SetWindowText(m_LabelText);

	CString marker = m_Name.Find(_T("//")) != -1 ? _T("//") : _T(":\\");
	int i;
	int ix = m_Name.Find(marker);
	if (((i = m_Name.Find(_T(' '))) != -1) && (i < ix))
	{
		m_Port = m_Name.Left(i);
		m_Name = m_Name.Mid(i);
		m_Name.TrimLeft();
		ix = m_Name.Find(marker);
	}
	if (((i = m_Name.Find(_T(' '))) != -1) && (i < ix))
	{
		m_Client = m_Name.Left(i);
		m_Name = m_Name.Mid(i);
		m_Name.TrimLeft();
		ix = m_Name.Find(marker);
	}
	if (((i = m_Name.Find(_T(' '))) != -1) && (i < ix))
	{
		m_User = m_Name.Left(i);
		m_Name = m_Name.Mid(i);
		m_Name.TrimLeft();
	}
	if ((i = m_Name.Find(_T('#'))) != -1)
	{
		m_Desc = m_Name.Mid(i+1);
		m_Desc.TrimLeft();
		m_Name = m_Name.Left(i);
	}

	if (m_MenuType)
	{
		m_Port = m_Client= m_User = m_Desc = _T("");
		GetDlgItem(IDC_PORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_CLIENT)->EnableWindow(FALSE);
		GetDlgItem(IDC_USER)->EnableWindow(FALSE);
		GetDlgItem(IDC_DESCRIPTION)->EnableWindow(FALSE);
	}
	else
	{
		if (m_Port == _T("*"))
			m_Port = _T("");
		if (m_Client == _T("*"))
			m_Client = _T("");
		if (m_User == _T("*"))
			m_User = _T("");
	}
	m_ChgPCU = (m_Port == _T("") && m_Client== _T("") && m_User == _T("")) ? 0 : 1;
	if (!m_ChgPCU)
	{
		m_Port = GET_P4REGPTR()->GetP4Port();
		m_Client = GET_P4REGPTR()->GetP4Client();
		m_User = GET_P4REGPTR()->GetP4User();
		OnSamePCU();
	}
	UpdateData( FALSE );
	if (!m_IsOK2Cr8SubMenu)
		GetDlgItem(IDC_SUBMENU)->EnableWindow(FALSE);
	if (m_RadioShow)
	{
		switch (m_RadioShow)
		{
		case 1:
			GetDlgItem(IDC_COMMAND)->EnableWindow(FALSE);
			break;
		case 2:
			GetDlgItem(IDC_SUBMENU)->EnableWindow(FALSE);
			break;
		case 3:
			GetDlgItem(IDC_STATIC_TITLE)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_COMMAND)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_SUBMENU)->ShowWindow(SW_HIDE);
			break;
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBookMarkAdd::OnCommand()
{
	if (!m_RadioShow)
	{
		GetDlgItem(IDC_PORT)->EnableWindow(TRUE);
		GetDlgItem(IDC_CLIENT)->EnableWindow(TRUE);
		GetDlgItem(IDC_USER)->EnableWindow(TRUE);
		GetDlgItem(IDC_DESCRIPTION)->EnableWindow(TRUE);
		GotoDlgCtrl(GetDlgItem(IDC_EDIT));
	}
}

void CBookMarkAdd::OnSubMenu()
{
	if (!m_RadioShow)
	{
		GetDlgItem(IDC_PORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_CLIENT)->EnableWindow(FALSE);
		GetDlgItem(IDC_USER)->EnableWindow(FALSE);
		GetDlgItem(IDC_DESCRIPTION)->EnableWindow(FALSE);
		GotoDlgCtrl(GetDlgItem(IDC_EDIT));
	}
}

void CBookMarkAdd::OnSamePCU()
{
	if (!m_RadioShow != 1)
	{
		GetDlgItem(IDC_PORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_CLIENT)->EnableWindow(FALSE);
		GetDlgItem(IDC_USER)->EnableWindow(FALSE);
		GotoDlgCtrl(GetDlgItem(IDC_DESCRIPTION));
	}
}

void CBookMarkAdd::OnDiffPCU()
{
	if (!m_RadioShow != 1)
	{
		GetDlgItem(IDC_PORT)->EnableWindow(TRUE);
		GetDlgItem(IDC_CLIENT)->EnableWindow(TRUE);
		GetDlgItem(IDC_USER)->EnableWindow(TRUE);
		GotoDlgCtrl(GetDlgItem(IDC_PORT));
	}
}

void CBookMarkAdd::OnOK() 
{
	UpdateData( );
	if (m_ChgPCU)
	{
		m_Port.TrimLeft();
		m_Port.TrimRight();
		m_Client.TrimLeft();
		m_Client.TrimRight();
		m_User.TrimLeft();
		m_User.TrimRight();
		if (m_Port.IsEmpty() || m_Client.IsEmpty() || m_User.IsEmpty())
		{
			AfxMessageBox(IDS_PORT_CLIENT_USER_MUST_BE_VALID, MB_OK|MB_ICONERROR);
			return;
		}
	}
	m_Name.TrimLeft();
	m_Name.TrimRight(_T("» "));
	if (m_ChgPCU)
		m_Name = m_Port + _T(" ") + m_Client + _T(" ") + m_User + _T(" ") + m_Name;
	if (!m_Desc.IsEmpty())
		m_Name += _T("#") + m_Desc;
	UpdateData(FALSE);
	EndDialog(IDOK);
	CDialog::OnOK();
}
