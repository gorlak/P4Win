// FileType.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "FileType.h"
#include "hlp\p4win.hh"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileType dialog


CFileType::CFileType(CWnd* pParent /*=NULL*/)
	: CDialog(CFileType::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileType)
	m_Action = 0;
	m_BaseType = -1;
	m_StoreType = -1;
	m_TypeK = FALSE;
	m_TypeW = FALSE;
	m_TypeX = FALSE;
	m_TypeM = FALSE;
	m_TypeO = FALSE;
	m_TypeL = FALSE;
	m_NbrRevs = 1;
	//}}AFX_DATA_INIT
}


void CFileType::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileType)
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Control(pDX, ID_FILETYPEHELP, m_FileTypeHelp);
	DDX_Radio(pDX, IDC_CHANGE, m_Action);
	DDX_Radio(pDX, IDC_TEXT, m_BaseType);
	DDX_Radio(pDX, IDC_FILETYPE_DEF, m_StoreType);
	DDX_Check(pDX, IDC_FILETYPE_K, m_TypeK);
	DDX_Check(pDX, IDC_FILETYPE_W, m_TypeW);
	DDX_Check(pDX, IDC_FILETYPE_X, m_TypeX);
	DDX_Check(pDX, IDC_FILETYPE_M, m_TypeM);
	DDX_Check(pDX, IDC_FILETYPE_O, m_TypeO);
	DDX_Check(pDX, IDC_FILETYPE_L, m_TypeL);
	DDX_Check(pDX, IDC_FILETYPE_S, m_TypeS);
	DDX_Control(pDX, IDC_COMBO1, m_NbrRevsCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileType, CDialog)
	//{{AFX_MSG_MAP(CFileType)
	ON_BN_CLICKED(IDC_CHANGE, OnActionChange)
	ON_BN_CLICKED(IDC_ADD, OnActionAdd)
	ON_BN_CLICKED(ID_FILETYPEHELP, OnFiletypehelp)
	ON_BN_CLICKED(IDC_FILETYPE_K, OnFileTypeK)
	ON_BN_CLICKED(IDC_FILETYPE_O, OnFileTypeO)
	ON_BN_CLICKED(IDC_FILETYPE_S, OnFileTypeS)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileType message handlers

BOOL CFileType::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	BOOL unknown;
	TheApp()->GetFileType(m_itemStr, m_BaseType, m_StoreType,
											 m_TypeK, m_TypeW, m_TypeX, m_TypeO,
											 m_TypeM, m_TypeL, m_TypeS, m_NbrRevs, unknown);
	if (unknown)
	{
        // get just filename part of item string
        CString item = m_itemStr;
        int i = item.ReverseFind(_T('<'));
        if(i > -1)
            item = item.Left(i);

		CString txt;
		txt.FormatMessage(IDS_WARN_CHANGING_UNKNOWN_FILETYPE_s, m_itemStr);
		AfxMessageBox(txt, MB_ICONEXCLAMATION);
	}

	m_NbrRevsCombo.AddString(_T("   1"));
	m_NbrRevsCombo.SetItemData(0, 1);
	m_NbrRevsCombo.SetCurSel(0);
	if (GET_SERVERLEVEL() < 23)	// pre 2007.2?
	{
		GetDlgItem(IDC_UTF16)->EnableWindow(FALSE);
		if (GET_SERVERLEVEL() < 11)
		{
			GetDlgItem(IDC_UNICODE)->EnableWindow(FALSE);
			GetDlgItem(IDC_FILETYPE_L)->EnableWindow(FALSE);

			if (GET_SERVERLEVEL() < 9)
			{
				GetDlgItem(IDC_FILETYPE_M)->EnableWindow(FALSE);
				if (m_TypeK)
					m_TypeO = TRUE;
			}
		}
	}
	if (GET_SERVERLEVEL() >= 23)	// 2007.2 or later?
	{
		for (int i=1; ++i <= 16; )
		{
			TCHAR buf[8];
			int j;

			if (i <= 10)
				_stprintf(buf, _T(" %3d"), j = i);
			else
				_stprintf(buf, _T(" %3d"), j = 16 << (i-11));
			m_NbrRevsCombo.AddString(buf);
			m_NbrRevsCombo.SetItemData(i-1, j);
			if (j == m_NbrRevs)
				m_NbrRevsCombo.SetCurSel(i-1);
		}
	}

	if (!m_TypeS || GET_SERVERLEVEL() < 23)
		GetDlgItem(IDC_COMBO1)->EnableWindow(FALSE);

	m_SaveBaseType = m_BaseType;
	m_SaveStoreType = m_StoreType;
	m_SaveTypeK = m_TypeK;
	m_SaveTypeW = m_TypeW;
	m_SaveTypeX = m_TypeX;
	m_SaveTypeM = m_TypeM;
	m_SaveTypeO = m_TypeO;
	m_SaveTypeL = m_TypeL;
	m_SaveTypeS = m_TypeS;
	m_SaveNbrRevs = m_NbrRevs;

	if (m_Action)
		OnActionAdd();

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFileType::OnFiletypehelp() 
{
	AfxGetApp()->WinHelp(TASK_CHANGING_FILE_TYPE);
}

BOOL CFileType::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnFiletypehelp();
	return TRUE;
}

void CFileType::OnCancel() 
{
	CDialog::OnCancel();
}

void CFileType::OnOK() 
{
	UpdateData();
	m_NbrRevs = m_NbrRevsCombo.GetItemData(m_NbrRevsCombo.GetCurSel());
	switch(m_BaseType)
	{
	case -1:
		m_fileType = _T("");
		break;
	case 0:
		m_fileType = _T("text");
		break;
	case 1:
		m_fileType = _T("binary");
		break;
	case 2:
		m_fileType = _T("symlink");
		break;
	case 3:
		m_fileType = _T("resource");
		break;
	case 4:
		m_fileType = _T("apple");
		break;
	case 5:
		m_fileType = _T("unicode");
		break;
	case 6:
		m_fileType = _T("utf16");
		break;
	}
	
	BOOL needPlus = FALSE;
	switch (m_StoreType)
	{
	case 1:
		if (m_BaseType != 1)
			m_fileType += _T("+C");
		else needPlus = TRUE;
		break;
	case 2:
		if (m_BaseType != 0)
			m_fileType += _T("+D");
		else needPlus = TRUE;
		break;
	case 3:
		m_fileType += _T("+F");
		break;
	case 0:
	default:
		needPlus = TRUE;
		break;
	}

	if (m_TypeX)
	{
		if (needPlus)
		{
			m_fileType += _T("+");
			needPlus = FALSE;
		}
		m_fileType += _T("x");
	}
	if (m_TypeW)
	{
		if (needPlus)
		{
			m_fileType += _T("+");
			needPlus = FALSE;
		}
		m_fileType += _T("w");
	}
	if (m_TypeO && (GET_SERVERLEVEL() >= 9))
	{
		if (needPlus)
		{
			m_fileType += _T("+");
			needPlus = FALSE;
		}
		m_fileType += _T("o");
	}
	else if (m_TypeK)
	{
		if (needPlus)
		{
			m_fileType += _T("+");
			needPlus = FALSE;
		}
		m_fileType += _T("k");
	}
	if (m_TypeM && (GET_SERVERLEVEL() >= 9))
	{
		if (needPlus)
		{
			m_fileType += _T("+");
			needPlus = FALSE;
		}
		m_fileType += _T("m");
	}
	if (m_TypeL && (GET_SERVERLEVEL() >= 11))
	{
		if (needPlus)
		{
			m_fileType += "+";
			needPlus = FALSE;
		}
		m_fileType += _T("l");
	}
	if (m_TypeS)
	{
		if (needPlus)
		{
			m_fileType += "+";
			needPlus = FALSE;
		}
		m_fileType += _T("S");
		if (GET_SERVERLEVEL() >= 23)
		{
			TCHAR buf[8];
			_stprintf(buf, _T("%d"), m_NbrRevs);
			m_fileType += buf;
		}
	}

	CDialog::OnOK();
}

void CFileType::OnFileTypeK() 
{
	UpdateData();
	m_NbrRevs = m_NbrRevsCombo.GetItemData(m_NbrRevsCombo.GetCurSel());
	if (!m_TypeK)
		m_TypeO = FALSE;
	else if (GET_SERVERLEVEL() < 9)
		m_TypeO = m_TypeK;
	UpdateData(FALSE);
}

void CFileType::OnFileTypeO() 
{
	UpdateData();
	m_NbrRevs = m_NbrRevsCombo.GetItemData(m_NbrRevsCombo.GetCurSel());
	if (m_TypeO)
		m_TypeK = TRUE;
	else if (GET_SERVERLEVEL() < 9)
	{
		m_TypeO = m_TypeK;
		MessageBeep(0);
	}
	UpdateData(FALSE);
}

void CFileType::OnFileTypeS()
{
	UpdateData();
	m_NbrRevs = m_NbrRevsCombo.GetItemData(m_NbrRevsCombo.GetCurSel());
	GetDlgItem(IDC_COMBO1)->EnableWindow(m_TypeS && GET_SERVERLEVEL() >= 23);
	UpdateData(FALSE);
}

void CFileType::OnActionChange()
{
	UpdateData();
	m_NbrRevs = m_NbrRevsCombo.GetItemData(m_NbrRevsCombo.GetCurSel());
	m_BaseType = m_SaveBaseType;
	m_StoreType = m_SaveStoreType;
	m_TypeK = m_SaveTypeK;
	m_TypeW = m_SaveTypeW;
	m_TypeX = m_SaveTypeX;
	m_TypeM = m_SaveTypeM;
	m_TypeO = m_SaveTypeO;
	m_TypeL = m_SaveTypeL;
	m_TypeS = m_SaveTypeS;
	m_NbrRevs = m_SaveNbrRevs;
	GetDlgItem(ID_STATIC1)->EnableWindow(TRUE);
	GetDlgItem(IDC_TEXT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BINARY)->EnableWindow(TRUE);
	GetDlgItem(IDC_SYMLINK)->EnableWindow(TRUE);
	GetDlgItem(IDC_RESOURCE)->EnableWindow(TRUE);
	GetDlgItem(IDC_APPLE)->EnableWindow(TRUE);
	GetDlgItem(IDC_UNICODE)->EnableWindow(GET_SERVERLEVEL() >= 11);
	GetDlgItem(IDC_UTF16)->EnableWindow(GET_SERVERLEVEL() >= 23);	// 2007.2 or later?
	UpdateData(FALSE);
	UpdateNbrRevs();
}

void CFileType::OnActionAdd()
{
	UpdateData();
	m_NbrRevs = m_NbrRevsCombo.GetItemData(m_NbrRevsCombo.GetCurSel());
	m_SaveBaseType = m_BaseType;
	m_SaveStoreType = m_StoreType;
	m_SaveTypeK = m_TypeK;
	m_SaveTypeW = m_TypeW;
	m_SaveTypeX = m_TypeX;
	m_SaveTypeM = m_TypeM;
	m_SaveTypeO = m_TypeO;
	m_SaveTypeL = m_TypeL;
	m_SaveTypeS = m_TypeS;
	m_SaveNbrRevs = m_NbrRevs;
	m_BaseType = -1;
	m_StoreType = -1;
	m_TypeK = FALSE;
	m_TypeW = FALSE;
	m_TypeX = FALSE;
	m_TypeM = FALSE;
	m_TypeO = FALSE;
	m_TypeL = FALSE;
	m_TypeS = FALSE;
	GetDlgItem(ID_STATIC1)->EnableWindow(FALSE);
	GetDlgItem(IDC_TEXT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BINARY)->EnableWindow(FALSE);
	GetDlgItem(IDC_SYMLINK)->EnableWindow(FALSE);
	GetDlgItem(IDC_RESOURCE)->EnableWindow(FALSE);
	GetDlgItem(IDC_APPLE)->EnableWindow(FALSE);
	GetDlgItem(IDC_UNICODE)->EnableWindow(FALSE);
	GetDlgItem(IDC_UTF16)->EnableWindow(FALSE);
	UpdateData(FALSE);
	UpdateNbrRevs();
}

void CFileType::UpdateNbrRevs()
{
	m_NbrRevsCombo.SetCurSel(0);
	if (GET_SERVERLEVEL() >= 23)
	{
		for (int i=1; ++i <= 16; )
		{
			int j;

			if (i <= 10)
				j = i;
			else
				j = 16 << (i-11);
			if (j == m_NbrRevs)
			{
				m_NbrRevsCombo.SetCurSel(i-1);
				break;
			}
		}
	}
	OnFileTypeS();
}
