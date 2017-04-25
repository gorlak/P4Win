// ToolsArgs.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "MainFrm.h"
#include "ToolsArgs.h"
#include "cmd_info.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolsArgs dialog


CToolsArgs::CToolsArgs(CWnd* pParent /*=NULL*/)
	: CDialog(CToolsArgs::IDD, pParent)
{
	//{{AFX_DATA_INIT(CToolsArgs)
	//}}AFX_DATA_INIT
}


void CToolsArgs::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolsArgs)
	DDX_Control(pDX, IDOK, m_OkBtn);
	DDX_Control(pDX, IDC_EDIT, m_ArgsCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CToolsArgs, CDialog)
	//{{AFX_MSG_MAP(CToolsArgs)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolsArgs message handlers

BOOL CToolsArgs::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowText(m_Title);

	// Make sure the combo is empty
	m_ArgsCombo.Clear();

	// Load all MRU tool arguments into combo's list box
	for(int i=0; i < MAX_MRU_TOOLARGS; i++)
	{
		if(GET_P4REGPTR()->GetMRUToolArg(i).GetLength() > 0)
		{
			m_ArgsCombo.AddString(GET_P4REGPTR()->GetMRUToolArg(i));
		}
	}

	// Clear the combo's edit box
	m_ArgsCombo.SetCurSel(-1);

	if (!m_ShowBrowse)
	{
		CRect rect;
		GetDlgItem(IDC_BROWSE)->GetWindowRect(&rect);
		GetDlgItem(IDC_BROWSE)->ShowWindow(SW_HIDE);
		ScreenToClient(&rect);
		GetDlgItem(IDOK)->MoveWindow(&rect, TRUE);
	}

	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CToolsArgs::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	MainFrame()->SetMessageText(m_StatusMsg);
}

void CToolsArgs::GetArgsValue()
{
	UpdateData();
	if(m_ArgsCombo.GetCurSel() != CB_ERR)
		m_ArgsCombo.GetLBText(m_ArgsCombo.GetCurSel(), m_Args);
	else
		m_ArgsCombo.GetWindowText( m_Args );
}

void CToolsArgs::OnOK() 
{
	GetArgsValue();
	if (m_Args.GetLength() > 0)
		GET_P4REGPTR()->AddMRUToolArg( m_Args );

	EndDialog(IDOK);
	CDialog::OnOK();
}

void CToolsArgs::OnBrowse() 
{
	static TCHAR filter[]=
		_T("All Files (*.*)|*.*||");

	UpdateData();
	UpdateData(FALSE);
	CFileDialog fDlg(TRUE, NULL, NULL,  
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NONETWORKBUTTON | OFN_PATHMUSTEXIST,
		LoadStringResource(IDS_TOOLARGS_FILTER), this,
		MainFrame()->m_osVer.dwMajorVersion < 5 ? OPENFILENAME_SIZE_VERSION_400 : sizeof(OPENFILENAME)); 
	
	TCHAR buf[MAX_PATH+1];
	// Zero 1st char so commdlg knows we aren't providing a default filename
	buf[0]='\0';  

    fDlg.m_ofn.lpstrFile= buf; 
	fDlg.m_ofn.nMaxFile= MAX_PATH; 

	// Set the dlg caption
    CString title = LoadStringResource(IDS_TOOLARGS_TITLE);
	fDlg.m_ofn.lpstrTitle = title;
	// We dont need no stinking file title
	fDlg.m_ofn.lpstrFileTitle=NULL;
	
	// Set the initial directory to the client root
	if (m_InitDir.IsEmpty())
	{
		if (TheApp()->m_ClientRoot.IsEmpty())
		{
			CCmd_Info cmd;
			cmd.Init( NULL, RUN_SYNC );
			if( cmd.Run( ) && !cmd.GetError() )
			{
				CP4Info const &info = cmd.GetInfo();
				if (!info.m_ClientRoot.IsEmpty( ))
					m_InitDir = info.m_ClientRoot;
			}
		}
		else m_InitDir = TheApp()->m_ClientRoot;
		if (m_InitDir.IsEmpty())
			m_InitDir = _T("\\");
	}
	fDlg.m_ofn.lpstrInitialDir=m_InitDir;

	fDlg.m_ofn.lpstrCustomFilter= NULL;

	// Set filter index
	fDlg.m_ofn.nFilterIndex=1;
	
	int retcode=fDlg.DoModal();

	if(retcode == IDOK)
	{
		GetArgsValue();
		if (m_Args.IsEmpty())
			 m_Args = fDlg.GetPathName();
		else m_Args += _T(' ') + fDlg.GetPathName();
		m_ArgsCombo.SetWindowText( m_Args );

		int i;
		m_InitDir = m_Args;
		if ((i = m_InitDir.ReverseFind(_T('\\'))) != -1)
			m_InitDir.SetAt(i, _T('\0'));
	}
	else
	{
		CString errorTxt;
		if(retcode == IDCANCEL)  // an error
		{
			DWORD exError=CommDlgExtendedError();
			if(exError != 0)
				errorTxt.FormatMessage(IDS_EXTENDED_ERROR_n_IN_FILEDIALOG, exError);
		}
		else
		{
			DWORD error=GetLastError();
			if(error)
			{
				LPVOID lpMsgBuf;
				FormatMessage( 
					FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					FORMAT_MESSAGE_FROM_SYSTEM | 
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					error,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
					(LPTSTR) &lpMsgBuf,
					0,
					NULL 
				);
				errorTxt = (TCHAR *)lpMsgBuf;
			}
			else
				errorTxt=LoadStringResource(IDS_UNKNOWN_FILEDIALOG_ERROR);
		}
		if(errorTxt.GetLength() > 0)
			AfxMessageBox(errorTxt, MB_ICONSTOP);
	}
	UpdateData(FALSE);
	m_OkBtn.SetFocus();
	m_ArgsCombo.SetFocus();
}
