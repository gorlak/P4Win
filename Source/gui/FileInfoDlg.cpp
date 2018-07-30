// FileInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "FileInfoDlg.h"
#include "Mainfrm.h"
#include "SpecDescDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define UPDATE_STATUS(x) ((CMainFrame *)AfxGetMainWnd())->UpdateStatus(x)

/////////////////////////////////////////////////////////////////////////////
// CFileInfoDlg dialog


CFileInfoDlg::CFileInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileInfoDlg::IDD, pParent)
{
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CFileInfoDlg)
	m_ClientPath = _T("");
	m_DepotPath = _T("");
	m_HaveRev = _T("");
	m_HeadAction = _T("");
	m_HeadChange = _T("");
	m_HeadRev = _T("");
	m_HeadType = _T("");
	m_LockedBy = _T("");
	m_ModTime = _T("");
	m_FileSizeFld = _T("");
	//}}AFX_DATA_INIT
	m_IsMinimized = FALSE;
	m_Key = m_FileSize = 0;
}


void CFileInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileInfoDlg)
	DDX_Control(pDX, IDC_DESCRIBE, m_Describe);
	DDX_Control(pDX, IDC_OPENEDLIST, m_OpenedList);
	DDX_Text(pDX, IDC_CLIENTPATH, m_ClientPath);
	DDX_Text(pDX, IDC_DEPOTPATH, m_DepotPath);
	DDX_Text(pDX, IDC_HAVEREV, m_HaveRev);
	DDX_Text(pDX, IDC_HEADACTION, m_HeadAction);
	DDX_Text(pDX, IDC_HEADCHANGE, m_HeadChange);
	DDX_Text(pDX, IDC_HEADREV, m_HeadRev);
	DDX_Text(pDX, IDC_HEADTYPE, m_HeadType);
	DDX_Text(pDX, IDC_LOCKEDBY, m_LockedBy);
	DDX_Text(pDX, IDC_MODTIME, m_ModTime);
	DDX_Text(pDX, IDC_FILESIZE, m_FileSizeFld);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CFileInfoDlg)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_DESCRIBE, OnDescribeChg)
	ON_LBN_SELCHANGE(IDC_OPENEDLIST, OnSelchangeOpenedlist)
	ON_LBN_DBLCLK(IDC_OPENEDLIST, OnDblclkOpenedlist)
	ON_COMMAND(IDCANCEL, OnClose)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_P4DESCRIBE, OnP4Describe )
    ON_MESSAGE(WM_P4ENDDESCRIBE, OnP4EndDescribe )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileInfoDlg message handlers

BOOL CFileInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (m_pParent)
		MainFrame()->SetModelessWnd(this);

	// Save the caption
	GetWindowText(m_Caption);

	m_Brush.CreateSolidBrush(GetSysColor(COLOR_3DFACE));
	
	POSITION pos= m_StrList.GetHeadPosition();
	
	// Go top to bottom thru list
	CString listrow;

	while( pos != NULL )
	{
		BOOL locked=FALSE;
		// Get the change w/username
		listrow= m_StrList.GetNext(pos);
		listrow.TrimLeft();

		// See if locked
		if(listrow.Find(LoadStringResource(IDS_STAR_LOCKED)) != -1)
		{
			locked=TRUE;
			listrow=listrow.Left(listrow.Find(LoadStringResource(IDS_STAR_LOCKED)));
			listrow.TrimRight(_T(" "));
		}
		
		// Append to list of users
		m_OpenedList.AddString(listrow);

		// Update locked edit box if required
		if(locked)
		{
			// may be either '<changelist> - user@client' or
			// 'user@client - <changelist>'
			int changeSep = listrow.Find(_T(" - "));
			int userSep = listrow.Find(_T('@'));
			if(changeSep < userSep)
				listrow=listrow.Mid(changeSep + lstrlen(_T(" - ")));
			
			// then remove trailing baggage, if present
			int n= listrow.Find(_T(' '));
			if(n != -1)
				listrow=listrow.Left(n);
			m_LockedBy=listrow;
		}
	}

	// if filesize is not zero, display it; otherwise hide the field and caption
	if (m_FileSize)
	{
		unsigned n = m_FileSize > 9999 ? (m_FileSize+512)/1024 : m_FileSize;
		m_FileSizeFld.Format(_T("%lu"), n);
		for (int lgth = m_FileSizeFld.GetLength(); lgth > 3; )
		{
			lgth -= 3;
			m_FileSizeFld.Insert(lgth, _T(','));
		}
		if (m_FileSize > 9999)
			m_FileSizeFld += _T(" KB");
	}
	else
	{
		GetDlgItem(IDC_STATIC1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FILESIZE)->ShowWindow(SW_HIDE);
	}

	// Make sure m_LockedBy is displayed
	UpdateData(FALSE);

	ShowWindow(SW_SHOW);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CFileInfoDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if(pWnd->m_hWnd == m_OpenedList.m_hWnd)
	{
		pDC->SetBkColor(GetSysColor(COLOR_3DFACE));
		return (HBRUSH) m_Brush.GetSafeHandle();
	}
	
	return hbr;
}

void CFileInfoDlg::OnSelchangeOpenedlist() 
{
	int i;
	CString txt;
	BOOL bEnable = FALSE;
	int idx = m_OpenedList.GetCurSel( );
	if (idx != LB_ERR)
	{
		CString chgmarker = LoadStringResource(IDS_CHANGE_MARKER);
		m_OpenedList.GetText(idx, txt);
		if ((i = txt.Find(chgmarker, 0)) > -1)
		{
			m_Chg = txt.Right(txt.GetLength() - i - chgmarker.GetLength());
			if ((i = m_Chg.Find(_T('('))) > -1)
			{
				m_Chg = m_Chg.Left(i);
				m_Chg.TrimLeft();
				m_Chg.TrimRight();
				if (!m_Chg.IsEmpty())
					bEnable = TRUE;
			}
		}
	}
	m_Describe.EnableWindow(bEnable);
}

void CFileInfoDlg::OnDescribeChg() 
{
	long changeNumber = _ttol(m_Chg);

	if(changeNumber != 0)
	{
		CString nbr;
		nbr.Format(_T("%ld"), changeNumber);
		CCmd_Describe *pCmd= new CCmd_Describe;
		if (m_Key)
			pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK, m_Key);
		else
			pCmd->Init( m_hWnd, RUN_ASYNC);
		if( pCmd->Run( P4DESCRIBE, nbr) )
			UPDATE_STATUS(LoadStringResource(IDS_FETCHING_CHANGELIST_DESCRIPTION));
		else
			delete pCmd;
	}
}

LRESULT CFileInfoDlg::OnP4Describe(WPARAM wParam, LPARAM lParam)
{
	CCmd_Describe *pCmd= (CCmd_Describe *) wParam;

	if(!pCmd->GetError())
	{
		CString desc= MakeCRs(pCmd->GetDescription());
		
		int key;
		CSpecDescDlg *dlg = new CSpecDescDlg(this);
		dlg->SetIsModeless(TRUE);
		dlg->SetKey(key = pCmd->HaveServerLock()? pCmd->GetServerKey() : 0);
		dlg->SetDescription(desc);
		dlg->SetItemName( pCmd->GetReference() );
		dlg->SetCaption(LoadStringResource(IDS_PERFORCE_CHANGELIST_DESCRIPTION));
		dlg->SetShowNextPrev(FALSE);
		dlg->SetShowShowDiffs(FALSE);
		dlg->SetViewType(P4CHANGE_SPEC);
		dlg->SetKey(m_Key);
		if (!dlg->Create(IDD_SPECDESC, this))	// display the description dialog box
		{
			dlg->DestroyWindow();	// some error! clean up
			delete dlg;
		}
	}
	
	delete pCmd;
	UPDATE_STATUS(_T(""));
	return 0;
}

LRESULT CFileInfoDlg::OnP4EndDescribe(WPARAM wParam, LPARAM lParam)
{
	CSpecDescDlg *dlg = (CSpecDescDlg *)lParam;
	dlg->DestroyWindow();
	return TRUE;
}

void CFileInfoDlg::OnDblclkOpenedlist() 
{
	if (!m_Chg.IsEmpty())
		OnDescribeChg();
}

void CFileInfoDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	if (nType == SIZE_MINIMIZED)
	{
		m_IsMinimized = TRUE;
		if (m_MinCaption.IsEmpty())
			m_MinCaption = LoadStringResource(IDS_FILEINFO_STR);
		SetWindowText(m_MinCaption);
		return;
	}
	else if (m_IsMinimized)
	{
		SetWindowText(m_Caption);
		m_IsMinimized = FALSE;
		return;
	}
}

void CFileInfoDlg::OnClose() 
{
	if (m_pParent)
		m_pParent->PostMessage(WM_P4ENDFILEINFORMATION, 0, (LPARAM)this);
	CDialog::OnOK();
}

// This signals the closing of a modeless dialog
// to MainFrame which will delete the 'this' object
void CFileInfoDlg::OnDestroy()
{
	if (m_pParent)
		::PostMessage(MainFrame()->m_hWnd, WM_P4DLGDESTROY, 0, (LPARAM)this);
}

void CFileInfoDlg::OnSysCommand(UINT nID, LPARAM lParam) 
{
	switch(nID)
	{
	case SC_MINIMIZE:
		GetDesktopWindow()->ArrangeIconicWindows();
		break;
	}

	CDialog::OnSysCommand(nID, lParam);
}
