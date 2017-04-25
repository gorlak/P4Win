// AddFilterDlg1.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "AddFilterDlg.h"
#include "hlp\p4win.hh"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddFilterDlg dialog


CAddFilterDlg::CAddFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddFilterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddFilterDlg)
	m_Ext = _T("");
	m_Exclude = GET_P4REGPTR()->GetAddFilterExclude();
	//}}AFX_DATA_INIT
}


void CAddFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddFilterDlg)
	DDX_Control(pDX, IDC_EXTLIST, m_ExtList);
	DDX_Text(pDX, IDC_EDIT1, m_Ext);
	DDX_Radio(pDX, IDC_RADIO1, m_Exclude);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddFilterDlg, CDialog)
	//{{AFX_MSG_MAP(CAddFilterDlg)
	ON_BN_CLICKED(IDC_ADD2FILTER, OnAddToFilter)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_DELETEALL, OnDeleteAll)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_LBN_SELCHANGE(IDC_EXTLIST, OnSelchangeList)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddFilterDlg message handlers

BOOL CAddFilterDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString ext;
	int i;

	while ((i = m_Extensions.Find(_T(';'))) != -1)
	{
		ext = m_Extensions.Left(i);
		ext.TrimLeft(_T(" *."));
		ext.TrimRight();
		if (ext.GetLength())
			m_ExtList.AddString(ext);
		m_Extensions = m_Extensions.Right(m_Extensions.GetLength() - i - 1);
	}
	if (m_Extensions.GetLength())
		m_ExtList.AddString(m_Extensions);

	OnSelchangeList();
	OnRadio();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAddFilterDlg::OnAddToFilter() 
{
	UpdateData( );
	if (!m_Ext.GetLength())
	{
		GotoDlgCtrl(GetDlgItem(IDC_EDIT1));
		return;
	}
	m_Ext.TrimLeft(_T(" *."));
	m_Ext.TrimRight();
	if ((m_Ext.FindOneOf(_T("*?.\\:")) != -1) || (!m_Ext.GetLength()))
	{
		AfxMessageBox(IDS_BADADDFILTER, MB_OK);
		GotoDlgCtrl(GetDlgItem(IDC_EDIT1));
		return;
	}
	m_ExtList.AddString(m_Ext);
	m_ExtList.SelectString( -1, m_Ext );
	m_Ext.Empty();
	UpdateData(FALSE);
	OnSelchangeList();
	GotoDlgCtrl(GetDlgItem(IDC_EDIT1));
}

void CAddFilterDlg::OnSelchangeList() 
{
	GetDlgItem(IDC_DELETE)->EnableWindow(m_ExtList.GetCurSel() != LB_ERR);
	GetDlgItem(IDC_DELETEALL)->EnableWindow(m_ExtList.GetCount() > 0);
}

void CAddFilterDlg::OnDelete() 
{
	int item;

	if ((item = m_ExtList.GetCurSel( )) != LB_ERR)
		m_ExtList.DeleteString(item);
	else MessageBeep(0);
	OnSelchangeList();
	GotoDlgCtrl(GetDlgItem(IDC_EDIT1));
}

void CAddFilterDlg::OnDeleteAll() 
{
	UpdateData();
	// it is necessary to remove the focus from the
	// Delete All button to make the disable look correct
	GotoDlgCtrl(GetDlgItem(IDC_ADD2FILTER));
	m_ExtList.ResetContent();
	OnSelchangeList();	// disables the Delete All button
	m_Exclude = -1;
	UpdateData(FALSE);
	OnRadio();
	// Now set the focus where we really want it
	GotoDlgCtrl(GetDlgItem(IDC_EDIT1));
}

void CAddFilterDlg::OnRadio()
{
	UpdateData();
	GetDlgItem(IDOK)->EnableWindow(m_Exclude != -1);
}

void CAddFilterDlg::OnOK() 
{
	UpdateData();

	if (m_Exclude == -1)
	{
		MessageBeep(0);
		ASSERT(0);
		return;
	}

	CString ext;

	m_Exts.RemoveAll();
	m_Extensions.Empty();
	for (int i = -1; ++i < m_ExtList.GetCount(); )
	{
		m_ExtList.GetText( i, ext );
		ext.TrimLeft(_T(" *."));
		ext.TrimRight();
		if (ext.GetLength())
		{
			m_Extensions += ext + _T(';');
			m_Exts.AddTail(ext);
		}
	}
	GET_P4REGPTR()->SetAddFilterExclude(m_Exclude);
	CDialog::EndDialog(IDOK); 
}

void CAddFilterDlg::OnHelp() 
{
	AfxGetApp()->WinHelp(ALIAS_96_ADDING_FILES);
}

BOOL CAddFilterDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
