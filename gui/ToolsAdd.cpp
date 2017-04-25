// ToolsAdd.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "ToolsAdd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolsAdd dialog


CToolsAdd::CToolsAdd(CWnd* pParent /*=NULL*/)
	: CDialog(CToolsAdd::IDD, pParent)
{
	//{{AFX_DATA_INIT(CToolsAdd)
	m_MenuType = 0;
	//}}AFX_DATA_INIT
	m_LabelText=_T("");
	m_IsOK2Cr8SubMenu = FALSE;
	m_RadioShow = 0;
}


void CToolsAdd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolsAdd)
	DDX_Text(pDX, IDC_EDIT, m_Name);
	DDX_Radio(pDX, IDC_COMMAND, m_MenuType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CToolsAdd, CDialog)
	//{{AFX_MSG_MAP(CToolsAdd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolsAdd message handlers

BOOL CToolsAdd::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowText(m_Title);
	if (m_LabelText.GetLength())
		GetDlgItem(IDC_LABEL_TEXT)->SetWindowText(m_LabelText);
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

void CToolsAdd::OnOK() 
{
	UpdateData( );
	m_Name.TrimLeft();
	m_Name.TrimRight(_T("» "));
	UpdateData(FALSE);
	EndDialog(IDOK);
	CDialog::OnOK();
}
