//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// TempDirPage.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "MainFrm.h"
#include "TempDirPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTempDirPage property page

IMPLEMENT_DYNCREATE(CTempDirPage, CPropertyPage)

CTempDirPage::CTempDirPage() : CPropertyPage(CTempDirPage::IDD)
{
	//{{AFX_DATA_INIT(CTempDirPage)
	m_TempPath = _T("");
	m_AltExpl = _T("");
	m_DnDDefault = 0;
	//}}AFX_DATA_INIT
	m_Inited = FALSE;
}

CTempDirPage::~CTempDirPage()
{
}

void CTempDirPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTempDirPage)
	DDX_Text(pDX, IDC_P4TEMPFILES, m_TempPath);
	DDX_Text(pDX, IDC_P4EXPLORER, m_AltExpl);
	DDV_MaxChars(pDX, m_TempPath, 255);
	DDV_MaxChars(pDX, m_AltExpl, 255);
	DDX_Control(pDX, IDC_BROWSE, m_Browse);
	DDX_Control(pDX, IDC_BROWSE_DIR, m_BrowseDir);
	DDX_Radio(pDX, IDC_WINEXPLORER, m_Explorer);
	DDX_Control(pDX, IDC_USE_DBLPANE, m_2Panes);
	DDX_Radio(pDX, IDC_ADD, m_DnDDefault);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTempDirPage, CPropertyPage)
	//{{AFX_MSG_MAP(CTempDirPage)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_BROWSE_DIR, OnBrowseDir)
	ON_BN_CLICKED(IDC_WINEXPLORER, OnSetExplorer)
	ON_BN_CLICKED(IDC_ATLEXPLR, OnSetExplorer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTempDirPage message handlers

BOOL CTempDirPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// Init all data members from the registry
	m_TempPath = GET_P4REGPTR()->GetTempDir();
	m_AltExpl  = GET_P4REGPTR()->GetAltExpl();
	m_Explorer = GET_P4REGPTR()->GetExplorer();
	m_2Panes.SetCheck(GET_P4REGPTR()->Get2Panes());
	m_DnDDefault = GET_P4REGPTR()->GetDefaultDnDfromExp();

	// Update input fields
	UpdateData(FALSE);
	OnSetExplorer();

	m_Inited = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTempDirPage::OnOK() 
{
	if (!m_Inited)
		return;

	// This bit of silliness is required to ensure all the 
	// fields provide their data to the UpdateData() routine
	GotoDlgCtrl(GetParent()->GetDlgItem(IDC_APPLY));

	m_ErrorCount=0;

	if(UpdateData(TRUE))
	{
		m_TempPath.TrimRight();
		m_TempPath.TrimLeft();

		if(m_TempPath.GetLength())
		{
			if(TestTempDir(m_TempPath))
			{
				if( m_TempPath != GET_P4REGPTR()->GetTempDir() )
					if(!GET_P4REGPTR()->SetTempDir( m_TempPath ) )
						m_ErrorCount++;
			}
			else 
				m_ErrorCount++;
		}
		else
		{
			m_ErrorCount++;
			AfxMessageBox(IDS_TEMPORARY_FILES_DIRECTORY_NOT_WRITEABLE, MB_ICONSTOP);
		}

		if( m_AltExpl != GET_P4REGPTR()->GetAltExpl() )
		{
			if(!GET_P4REGPTR()->SetAltExpl( m_AltExpl ) )
				m_ErrorCount++;
		}
		if (!m_ErrorCount)
		{
			if(!GET_P4REGPTR()->SetExplorer( m_Explorer ) )
				m_ErrorCount++;
		}
		BOOL b2pane = TRUE;
		if( m_2Panes.GetCheck() == 0 )
			b2pane = FALSE;
		if( b2pane != GET_P4REGPTR()->Get2Panes( ) )
			if(!GET_P4REGPTR()->Set2Panes( b2pane ) )
				m_ErrorCount++;

		if (!m_ErrorCount)
		{
			if(!GET_P4REGPTR()->SetDefaultDnDfromExp(m_DnDDefault))
				m_ErrorCount++;
		}

		if(m_ErrorCount)
			AfxMessageBox(IDS_BAD_REGISTRY, MB_ICONSTOP);
	}		
}


BOOL CTempDirPage::TestTempDir( LPCTSTR path )
{
	BOOL success=FALSE;
	CString filename(path);
	
	FileSys *pFile= FileSys::Create( (enum FileSysType) FST_BINARY );
	pFile->SetDeleteOnClose();

	// TODO: calc a unique name.  but for time being, change that
	// a user has this exact file as readonly is as likely as
	// three atoms colliding
	CString TempName;
	TempName.Format( _T("%s\\p4win.p4wintempdir.test"), path);
    pFile->Set(CharFromCString(TempName));
	Error e;
	e.Clear();
	
	// Prepare write (makes dir as required)
	pFile->MkDir( &e );
	if( !e.Test() )
	{
		// Open it
		pFile->Perms( FPM_RO );
		pFile->Open( FOM_WRITE, &e );
		pFile->Close(&e);
	}
	
	if(!e.Test())
		success=TRUE;
	
	delete pFile;
	
	if(!success)
		AfxMessageBox(IDS_TEMPORARY_FILES_DIRECTORY_NOT_WRITEABLE, MB_ICONSTOP);
	
	return success;
}
  
void CTempDirPage::OnBrowse() 
{
	UpdateData();
	UpdateData(FALSE);
	CFileDialog fDlg(TRUE, NULL, NULL,  
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NONETWORKBUTTON | OFN_PATHMUSTEXIST,
		LoadStringResource(IDS_TEXT_EDITOR_FILTER), this,
		MainFrame()->m_osVer.dwMajorVersion < 5 ? OPENFILENAME_SIZE_VERSION_400 : sizeof(OPENFILENAME)); 
	
	TCHAR buf[MAX_PATH];
	// Zero 1st char so commdlg knows we aren't providing a default filename
	buf[0]=_T('\0');  

    fDlg.m_ofn.lpstrFile= buf; 
	fDlg.m_ofn.nMaxFile= MAX_PATH; 

	// Set the dlg caption
    CString title = LoadStringResource(IDS_SELECT_DEFAULT_TEXT_EDITOR_CAPTION);
	fDlg.m_ofn.lpstrTitle = title;
	// We dont need no stinking file title
	fDlg.m_ofn.lpstrFileTitle=NULL;
	
	// Set the initial directory
	CString initDir = _T("\\");
	fDlg.m_ofn.lpstrInitialDir=initDir;

	fDlg.m_ofn.lpstrCustomFilter= NULL;

	// Set filter index
	fDlg.m_ofn.nFilterIndex=1;
	
	int retcode=fDlg.DoModal();

	if(retcode == IDOK)
	{
		m_AltExpl = fDlg.GetPathName() + _T(' ');
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
}

void CTempDirPage::OnBrowseDir() 
{
	UpdateData();
	CString newfolder = TheApp()->BrowseForFolder(m_hWnd, m_TempPath, 
							LoadStringResource(IDS_CHOOSE_TEMP_DIR), BIF_NEWDIALOGSTYLE);
	if (!newfolder.IsEmpty())
	{
		m_TempPath = newfolder;
		UpdateData(FALSE);
	}
}

void CTempDirPage::OnSetExplorer() 
{
	UpdateData();
	CWnd *pwnd1=GetDlgItem(IDC_P4EXPLORER);
	CWnd *pwnd2=GetDlgItem(IDC_BROWSE);
	CWnd *pwnd3=GetDlgItem(IDC_USE_DBLPANE);
	if( m_Explorer )
	{
		pwnd1->EnableWindow(TRUE);
		pwnd2->EnableWindow(TRUE);
		pwnd3->EnableWindow(FALSE);
		if (m_Inited)
			GotoDlgCtrl(pwnd1);
	}
	else
	{
		pwnd1->EnableWindow(FALSE);
		pwnd2->EnableWindow(FALSE);
		pwnd3->EnableWindow(TRUE);
	}
}
