// FavEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "FavEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFavEditDlg dialog


CFavEditDlg::CFavEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFavEditDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFavEditDlg)
	m_MenuType = 0;
	//}}AFX_DATA_INIT
	m_IsOK2Cr8SubMenu = m_DescFocus = FALSE;
	m_RadioShow = 0;
}


void CFavEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFavEditDlg)
	DDX_Text(pDX, IDC_PORT, m_Port);
	DDX_Text(pDX, IDC_CLIENT, m_Client);
	DDX_Text(pDX, IDC_USER, m_User);
	DDX_Text(pDX, IDC_DESCRIPTION, m_Desc);
	DDV_MaxChars(pDX, m_Desc, 32);
	DDX_Radio(pDX, IDC_COMMAND, m_MenuType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFavEditDlg, CDialog)
	//{{AFX_MSG_MAP(CFavEditDlg)
	ON_BN_CLICKED(IDC_COMMAND, OnCommand)
	ON_BN_CLICKED(IDC_SUBMENU, OnSubMenu)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFavEditDlg message handlers

BOOL CFavEditDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (!m_Title.IsEmpty())
		SetWindowText(m_Title);

	if (m_MenuType)
	{
		m_Desc = m_Name;
		m_Port = m_Client= m_User = _T("");
		GetDlgItem(IDC_PORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_CLIENT)->EnableWindow(FALSE);
		GetDlgItem(IDC_USER)->EnableWindow(FALSE);
	}
	else if (!m_Name.IsEmpty())
	{
		int i;
		int firstsep;
		int secondsep;
		if ((i = m_Name.Find(_T('\t'))) != -1)
		{
			m_Desc = m_Name.Mid(i+1);
			m_Name = m_Name.Left(i);
		}

		if (m_Name.GetAt(0) == _T('\"'))
			firstsep = m_Name.Find(_T('\"'),1) + 1;
		else
			firstsep = m_Name.Find(_T(' '));
		if (firstsep == -1)
		{
			m_Port  = m_Name;
			m_Client= m_User = _T("");
		}
		else
		{
			m_Port  = m_Name.Left(firstsep);
			m_Port.TrimLeft(_T('\"'));
			m_Port.TrimRight(_T('\"'));
			secondsep= m_Name.Find(_T(' '), firstsep+1);
			if (secondsep == -1)
			{
				m_Client= m_Name.Mid(firstsep+1);
				m_User  = _T("");
			}
			else
			{
				m_Client= m_Name.Mid(firstsep+1, secondsep - firstsep - 1);
				m_User  = m_Name.Right(m_Name.GetLength() - secondsep - 1);
			}
		}
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

	if (m_DescFocus)
	{
		GotoDlgCtrl(GetDlgItem(IDC_DESCRIPTION));
		return FALSE;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFavEditDlg::OnCommand()
{
	if (!m_RadioShow)
	{
		GetDlgItem(IDC_PORT)->EnableWindow(TRUE);
		GetDlgItem(IDC_CLIENT)->EnableWindow(TRUE);
		GetDlgItem(IDC_USER)->EnableWindow(TRUE);
		GotoDlgCtrl(GetDlgItem(IDC_PORT));
	}
}

void CFavEditDlg::OnSubMenu()
{
	if (!m_RadioShow)
	{
		GetDlgItem(IDC_PORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_CLIENT)->EnableWindow(FALSE);
		GetDlgItem(IDC_USER)->EnableWindow(FALSE);
		GotoDlgCtrl(GetDlgItem(IDC_DESCRIPTION));
	}
}

void CFavEditDlg::OnOK() 
{
	UpdateData( );
	if (m_MenuType)
	{
		m_Name = m_Desc;
	}
	else
	{
		m_Port.TrimLeft();
		m_Port.TrimRight();
		if (m_Port.FindOneOf(_T("@#")) != -1)
		{
			CString txt;
			txt.FormatMessage(IDS_FAV_CANT_HAVE_AT_OR_POUND_IN_PORT_s, m_Port);
			AfxMessageBox( txt, MB_ICONEXCLAMATION );
			return;
		}
		if (m_Port.Find(_T(' ')) != -1)
		{
			m_Port.TrimLeft(_T('\"'));
			m_Port.TrimRight(_T('\"'));
			m_Port = _T('\"') + m_Port + _T('\"');
		}
		m_Client.TrimLeft();
		m_Client.TrimRight();
		m_User.TrimLeft();
		m_User.TrimRight();
		m_Desc.TrimLeft();
		m_Desc.TrimRight();
		m_Name = m_Port;
		if (!m_Client.IsEmpty())
		{
			m_Name += _T(' ') + m_Client;
			if (!m_User.IsEmpty())
				m_Name += _T(' ') + m_User;
		}
		if (!m_Desc.IsEmpty())
			m_Name += _T('\t') + m_Desc;
	}
	UpdateData(FALSE);
	EndDialog(IDOK);
	CDialog::OnOK();
}
