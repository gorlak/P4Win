// merge2dlg.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "merge2dlg.h"
#include "Mainfrm.h"
#include "FileInfoDlg.h"
#include "Historydlg.h"
#include "ViewerDlg.h"
#include "cmd_fstat.h"
#include "cmd_history.h"
#include "cmd_opened.h"
#include "cmd_prepbrowse.h"
#include "hlp\p4win.hh"
#include <process.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define UPDATE_STATUS(x) ((CMainFrame *)AfxGetMainWnd())->UpdateStatus(x)

/////////////////////////////////////////////////////////////////////////////
// CMerge2Dlg dialog


CMerge2Dlg::CMerge2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMerge2Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMerge2Dlg)
	m_TheirFile = _T("");
	m_YourFile = _T("");
	m_FilesFlag = 0;
	//}}AFX_DATA_INIT
	m_InitRect.SetRect(100,0,0,0);
	m_WinPos.SetWindow( this, _T("MergeDlgs") );
	m_LastWidth = 0;
	m_pMerge= NULL;
}


void CMerge2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMerge2Dlg)
	DDX_Text(pDX, IDC_THEIRFILE, m_TheirFile);
	DDX_Text(pDX, IDC_YOURFILE, m_YourFile);
	DDX_Radio(pDX, IDC_RADIO1, m_FilesFlag);
	DDX_Control(pDX, IDC_BDIFF, m_DiffBtn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMerge2Dlg, CDialog)
	//{{AFX_MSG_MAP(CMerge2Dlg)
	ON_EN_SETFOCUS( IDC_YOURFILE, OnRadio1 )
	ON_EN_SETFOCUS( IDC_THEIRFILE, OnRadio2 )
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_BACCEPT, OnAccept)
	ON_BN_CLICKED(IDC_BDIFF, OnDiff)
	ON_BN_CLICKED(IDC_BDIFFMENU, OnDiffMenu)
	ON_BN_CLICKED(IDC_BEDIT, OnEdit)
	ON_BN_CLICKED(IDC_BPROPERTIES, OnFileInformation)
	ON_BN_CLICKED(IDC_BHISTORY, OnFileRevisionhistory)
	ON_BN_CLICKED(IDC_CANCEL_ALL, OnCancelAll)
	ON_COMMAND(IDC_BDIFFYOURSDEPOT, OnDiffyoursdepot)
	ON_COMMAND(IDC_BDIFFTHEIRSDEPOT, OnDifftheirsdepot)
	ON_WM_ACTIVATE()
	ON_WM_HELPINFO()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_P4FILEINFORMATION, OnP4FileInformation)
	ON_MESSAGE(WM_P4ENDFILEINFORMATION, OnP4EndFileInformation )
	ON_MESSAGE(WM_P4PREPBROWSE, OnP4PrepBrowse)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMerge2Dlg message handlers

//////////////////////////////////////////
// Fire up the diff app
//
void CMerge2Dlg::Diff(FileSys *file1, FileSys *file2, 
					  LPCTSTR flag1, LPCTSTR display1, LPCTSTR flag2, LPCTSTR display2)
{
	CString txt;
	Error e;

	if( (!file1->IsTextual() || !file2->IsTextual()) && !GET_P4REGPTR()->GetDiffAppIsBinary())
	{
	    if( file1->Compare( file2, &e ) )
		{									// they differ
			CString appName;
			CString extension = GetFilesExtension(CharToCString(file1->Name()));
			if (!extension.CompareNoCase(_T("tmp")))
				extension = GetFilesExtension(CharToCString(file2->Name()));
			if(!extension.IsEmpty())
				appName= GET_P4REGPTR()->GetAssociatedDiff(extension);

			if ( appName.IsEmpty() )
			{	// no assoc diff app for these binaries
				txt.FormatMessage(IDS_BINARY_FILES_DIFFER_BUT_UNABLE_TO_DISPLAY_DIFF_s_s, 
						CharToCString(file1->Name()), 
						CharToCString(file2->Name()));
				AfxMessageBox(txt, MB_ICONINFORMATION );
				return;
			}
		}
		else								// they are the same
		{
			txt.FormatMessage(IDS_BINARY_FILES_ARE_IDENTICAL_s_s, 
					CharToCString(file1->Name()), 
					CharToCString(file2->Name()));
			AfxMessageBox(txt, MB_ICONINFORMATION );
			return;
		}
	}
	CString errorText;
	BOOL isUnicode = file1->IsUnicode() + file2->IsUnicode();
	if ( isUnicode && ((file1->GetType() & FST_MASK) == FST_UTF16 
					|| (file2->GetType() & FST_MASK) == FST_UTF16 ))
		isUnicode = 16;
	if( !TheApp()->RunApp(DIFF_APP, RA_WAIT, m_hWnd, isUnicode, NULL, 
								errorText, 
                                CharToCString(file1->Name()), 
                                CharToCString(file2->Name()),
								flag1, display1, flag2, display2))
			AfxMessageBox(errorText, MB_ICONEXCLAMATION );
}

void CMerge2Dlg::OnDiffMenu() 
{
	UpdateData();
	CP4Menu popMenu;
	CPoint point;
	CRect rect;
	GetDlgItem(IDC_BDIFF)->GetWindowRect(&rect);
	point.x = rect.left;
	point.y = rect.bottom + 1;
	popMenu.LoadMenu(IDR_MERGE2DIFFYOURS + m_FilesFlag);
	if (GET_SERVERLEVEL() < 10)
	{
		popMenu.EnableMenuItem(IDC_BDIFFYOURSDEPOT, MF_DISABLED|MF_GRAYED);
		popMenu.EnableMenuItem(IDC_BDIFFTHEIRSDEPOT, MF_DISABLED|MF_GRAYED);
	}
	popMenu.GetSubMenu(0)->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	m_DiffBtn.ClearButtonPushed();
}

void CMerge2Dlg::OnDiff() // Diff a files against each other
{
	Diff(m_pMerge->YourFile(), m_pMerge->TheirFile(), _T("-l"), m_YourFile, _T("-r"), m_TheirFile);
	GotoDlgCtrl(GetDlgItem(IDC_BEDIT));
	InvalidateRect(NULL);
	UpdateWindow();
	GotoDlgCtrl(GetDlgItem(IDC_BDIFF));
}

void CMerge2Dlg::OnDiffyoursdepot() 
{
	m_DepotDiffSw = 0;
	Getdepotfile();
}

void CMerge2Dlg::OnDifftheirsdepot() 
{
	m_DepotDiffSw = 1;
	Getdepotfile();
}

void CMerge2Dlg::Getdepotfile()
{
	CCmd_PrepBrowse *pCmd= new CCmd_PrepBrowse;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK, m_Key);
	CString type = _T("text");
	if( pCmd->Run( m_pMerge->GetFilespec(), type, 0x80000000, TheApp()->m_bNoCRLF ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_FETCHING_FILE) );
	else
		delete pCmd;
	GotoDlgCtrl(GetDlgItem(IDC_BEDIT));
	InvalidateRect(NULL);
	UpdateWindow();
	GotoDlgCtrl(GetDlgItem(IDC_BDIFF));
}

LRESULT CMerge2Dlg::OnP4PrepBrowse( WPARAM wParam, LPARAM lParam )
{
	CCmd_PrepBrowse *pCmd= (CCmd_PrepBrowse *) wParam;
	MainFrame()->ClearStatus();

	if(!pCmd->GetError())
	{
		switch(m_DepotDiffSw)
		{
		case 0:
			Diff(pCmd->GetTempFile(), m_pMerge->YourFile(), _T("-l"), m_YourFile, 
															_T("-r"), pCmd->GetTempName());
			break;
		case 1:
			Diff(pCmd->GetTempFile(), m_pMerge->TheirFile(), _T("-l"), m_TheirFile, 
															 _T("-r"), pCmd->GetTempName());
			break;
		default:
			ASSERT(0);
			break;
		}
	}
	delete pCmd;
	return 0;
}

//////////////////////////////////////////
// Edit merged, yours or theis
//
void CMerge2Dlg::OnEdit() 
{
	UpdateData();
	switch(m_FilesFlag)
	{
	case 0:
		OnEdityours();
		break;
	case 1:
		OnEdittheirs();
		break;
	default:
		ASSERT(0);
		break;
	}
}

void CMerge2Dlg::OnEdittheirs() 
{
	Edit(m_pMerge->TheirFile());
}

void CMerge2Dlg::OnEdityours() 
{
	Edit(m_pMerge->YourFile());
}

void CMerge2Dlg::OnOK() 
{
	// Sometimes MFC gets confused and issues an OnOK()
	// even tho there no control to do so for this dialog(!)
	// So just ignore it.
}

void CMerge2Dlg::OnCancel() 
{
	m_WinPos.SaveWindowPosition();
	m_pMerge->SetStatus(CMS_SKIP);
	CDialog::OnCancel();
}

void CMerge2Dlg::OnCancelAll() 
{
	m_WinPos.SaveWindowPosition();
	m_pMerge->SetStatus(CMS_SKIP);
	CDialog::EndDialog(IDC_CANCEL_ALL);
}

BOOL CMerge2Dlg::OnInitDialog() 
{
	// Make sure SetMergeInfo() was called first
	ASSERT(m_pMerge != NULL);

	CDialog::OnInitDialog();
	GetWindowRect(&m_InitRect);
	m_LastWidth = m_InitRect.Width();
	
	// Set the 2 IDs for the Diff button
	m_DiffBtn.SetIDs(IDC_BDIFF, IDC_BDIFFMENU);

	// Set the filename edit boxes
	m_TheirFile= m_pMerge->TheirFileName();
	m_YourFile=m_pMerge->YourFileName();

	UpdateData(FALSE);
	switch(GET_P4REGPTR()->GetResolve2wayDefault())
	{
	default:
	case 0:
		OnRadio1();
		break;
	case 1:
		OnRadio2();
		break;
	}
	
	m_WinPos.RestoreWindowPosition();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//////////////////////////////////////////
// Accept merged, yours or theirs and get out
//
void CMerge2Dlg::OnAccept() 
{
	UpdateData();
	m_WinPos.SaveWindowPosition();
	switch(m_FilesFlag)
	{
	case 0:
		OnAcceptyours();
		break;
	case 1:
		OnAccepttheirs();
		break;
	default:
		ASSERT(0);
		break;
	}
}

void CMerge2Dlg::OnAccepttheirs() 
{
	if( !Verify(LoadStringResource(IDS_ACCEPTING_THEIR_FILE_WILL_RESULT_IN_LOSS_OF_YOUR_EDITS)) )
		return;
	m_pMerge->SetStatus(CMS_THEIRS);
	CDialog::OnOK();
}

void CMerge2Dlg::OnAcceptyours() 
{
	m_pMerge->SetStatus(CMS_YOURS);
	CDialog::OnOK();
}

//////////////////////////////////////////
// Fire up the editor
//
BOOL CMerge2Dlg::Edit(FileSys *file)
{
	BOOL bLaunched = FALSE;
	CString viewFilePath= CharToCString(file->Name());

	// First, get the file extension, if any, and find out if
	// its a text file
	CString extension;
	int slash= viewFilePath.ReverseFind(_T('\\'));
	if(slash != -1)
		extension=viewFilePath.Mid(slash+1);
	else
		extension=viewFilePath;

	int dot= extension.ReverseFind(_T('.'));
	if(dot == -1)
		extension.Empty();
	else
		extension=extension.Mid(dot+1);

	// We have the file, viewFilePath, try to display it
	while(1)
	{
		bLaunched = FALSE;

		// Ask the user to pick a viewer
		CViewerDlg dlg;
		if(dlg.DoModal() == IDCANCEL)
			break;

		CString viewer=dlg.GetViewer();
		if(viewer != _T("SHELLEXEC"))
			GET_P4REGPTR()->AddMRUViewer(viewer);

		if(viewer == _T("SHELLEXEC"))
		{
			CString assocViewer;

			// First, see if there a P4win file association
			if(!extension.IsEmpty())
				assocViewer= GET_P4REGPTR()->GetAssociatedApp(extension);
		
			// If we still havent found a viewer, set viewer to default text app
			// if user wishes to ignore windows associations
			if(assocViewer.IsEmpty() && GET_P4REGPTR()->GetIgnoreWinAssoc())
				assocViewer= GET_P4REGPTR()->GetEditApp();
		
			// Let windows take a crack at finding a viewer
			if(assocViewer.IsEmpty() && !extension.IsEmpty())
			{
				// Quick check for executeable extension, which will make ShellExec try to run the file
				HINSTANCE hinst=0;
				if( extension.CompareNoCase(_T("com")) != 0 && extension.CompareNoCase(_T("exe")) != 0 &&
					extension.CompareNoCase(_T("bat")) != 0 && extension.CompareNoCase(_T("cmd")) != 0)
				{										// give VS .NET 7.1 (non-standard!) a try
					hinst= ShellExecute( m_hWnd, _T("Open.VisualStudio.7.1"), viewFilePath, NULL, NULL, SW_SHOWNORMAL);
					if( (int) hinst > 32)
					{
						bLaunched = TRUE;
						break;  // successfull viewer launch
					}
					if( (int) hinst == SE_ERR_NOASSOC)	// give MSDEV (non-standard!) a try
					{
						hinst= ShellExecute( m_hWnd, _T("&Open with MSDEV"), viewFilePath, NULL, NULL, SW_SHOWNORMAL);
						if( (int) hinst > 32 ) 
						{
							bLaunched = TRUE;
							break;  // successfull MSDEV viewer launch
						}
					}
					if( (int) hinst == SE_ERR_NOASSOC)	// give standard "open" a try
					{
						hinst= ShellExecute( m_hWnd, _T("open"), viewFilePath, NULL, NULL, SW_SHOWNORMAL);
						if( (int) hinst > 32 ) 
						{
							bLaunched = TRUE;
							break;  // successfull MSDEV viewer launch
						}
					}
				}
			}

			// If windows doesnt have an associated viewer for a text file, we use the 
			// default text editor
			if(assocViewer.IsEmpty())
				assocViewer= GET_P4REGPTR()->GetEditApp();
			

			if ( TheApp()->RunViewerApp( assocViewer, viewFilePath ) )
			{
				bLaunched = TRUE;
				break;  // successfull viewer launch
			}
		}
		else
		{
			if ( TheApp()->RunViewerApp( viewer, viewFilePath ) )
			{
				bLaunched = TRUE;
				break;  // successfull viewer launch
			}
		}

		CString msg;
		msg.FormatMessage(IDS_UNABLE_TO_LAUNCH_VIEWER_s, viewFilePath);
		if(AfxMessageBox(msg, MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
			break;
	} // while
	return bLaunched;	// let caller know if we actually launched an editor
}

BOOL CMerge2Dlg::Verify( LPCTSTR txt )
{
	if(AfxMessageBox(txt, MB_ICONQUESTION|MB_YESNO) == IDYES)
		return TRUE;
	else
		return FALSE;
}

void CMerge2Dlg::OnHelp() 
{
	AfxGetApp()->WinHelp(ALIAS_53_RESOLVING_FILES);
}

BOOL CMerge2Dlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}

void CMerge2Dlg::OnRadio1() 
{
	UpdateData(TRUE);
	m_FilesFlag = 0;
	UpdateData(FALSE);
	SetDlgItemText( IDC_BACCEPT, LoadStringResource(IDS_BACCEPTY) );
	SetDlgItemText( IDC_BEDIT, LoadStringResource(IDS_BEDIT) );
	GetDlgItem(IDC_BPROPERTIES)->EnableWindow( TRUE );
	GetDlgItem(IDC_BHISTORY)->EnableWindow( TRUE );
	GetDlgItem(IDC_STATIC_1)->ShowWindow( SW_SHOW );
	GetDlgItem(IDC_STATIC_2)->ShowWindow( SW_HIDE );
	GotoDlgCtrl(GetDlgItem(IDC_YOURFILE));
}

void CMerge2Dlg::OnRadio2() 
{
	UpdateData(TRUE);
	m_FilesFlag = 1;
	UpdateData(FALSE);
	SetDlgItemText( IDC_BACCEPT, LoadStringResource(IDC_BACCEPTT) );
	SetDlgItemText( IDC_BEDIT, LoadStringResource(IDS_BVIEW) );
	GetDlgItem(IDC_BPROPERTIES)->EnableWindow( TRUE );
	GetDlgItem(IDC_BHISTORY)->EnableWindow( TRUE );
	GetDlgItem(IDC_STATIC_1)->ShowWindow( SW_HIDE );
	GetDlgItem(IDC_STATIC_2)->ShowWindow( SW_SHOW );
	GotoDlgCtrl(GetDlgItem(IDC_THEIRFILE));
}

void CMerge2Dlg::OnFileInformation() 
{
	UpdateData();
	int i;
	switch(m_FilesFlag)
	{
	case 0:
		m_itemStr = m_YourFile;
		break;
	case 1:
		m_itemStr = m_TheirFile;
		m_itemStr.TrimLeft();
		if ((i = m_itemStr.Find(_T('#'))) != -1)
			m_itemStr = m_itemStr.Left(i);
		break;
	case 2:
	default:
		ASSERT(0);
		return;
	}

	m_StringList.RemoveAll();
	m_StringList.AddHead(m_itemStr);
	
	CCmd_Opened *pCmd= new CCmd_Opened;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK, m_Key);
	pCmd->SetAlternateReplyMsg( WM_P4FILEINFORMATION );

	if( pCmd->Run( TRUE, FALSE, -1, &m_StringList ) )
		UPDATE_STATUS( LoadStringResource(IDS_REQUESTING_FILE_INFORMATION) );
	else
		delete pCmd;
}

LRESULT CMerge2Dlg::OnP4FileInformation( WPARAM wParam, LPARAM lParam )
{
	CCmd_Opened *pCmd= (CCmd_Opened *) wParam;
	
	m_StringList.RemoveAll();
	if(!pCmd->GetError())
	{
		CString thisuser=GET_P4REGPTR()->GetMyID();
				
		// Initialize the file info dialog
		CFileInfoDlg *dlg = new CFileInfoDlg(this);

		dlg->m_DepotPath = m_itemStr;

		int key= pCmd->GetServerKey();
		CCmd_Fstat *pCmd2= new CCmd_Fstat;
		
		pCmd2->Init(NULL, RUN_SYNC, HOLD_LOCK, dlg->m_Key = key);
		if (MainFrame()->IsQuitting())
			return FALSE;

		MSG msg;
		while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			//		get out if app is terminating
			//
			if ( msg.message == WM_QUIT )
				return FALSE;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		pCmd2->SetIncludeAddedFiles( TRUE );
		if( pCmd2->Run( FALSE, m_itemStr, 0 ) && !pCmd2->GetError() )
		{
			CObList *list = pCmd2->GetFileList ( );
			ASSERT_KINDOF( CObList, list );
			ASSERT( list->GetCount() <= 1 );
			POSITION pos = list->GetHeadPosition( );
			if( pos != NULL )
			{
				CP4FileStats *stats = ( CP4FileStats * )list->GetNext( pos );
				ASSERT_KINDOF( CP4FileStats, stats );
				dlg->m_DepotPath = stats->GetFullDepotPath( );
				dlg->m_ClientPath = stats->GetFullClientPath( );
				if(dlg->m_ClientPath.GetLength() == 0)
					dlg->m_ClientPath= LoadStringResource(IDS_NOT_IN_CLIENT_VIEW);
				
				dlg->m_HeadRev.Format(_T("%ld"), stats->GetHeadRev());
				dlg->m_HaveRev.Format(_T("%ld"), stats->GetHaveRev());
				
				dlg->m_HeadAction= stats->GetActionStr(stats->GetHeadAction());
				dlg->m_HeadChange.Format(_T("%ld"), stats->GetHeadChangeNum());
				dlg->m_HeadType= stats->GetHeadType();
				dlg->m_ModTime= stats->GetFormattedHeadTime();
				dlg->m_FileSize= stats->GetFileSize();

				// Check for open/lock by this user
				if(stats->IsMyLock())
					dlg->m_LockedBy= thisuser;
				
				delete stats;
			}		
			else dlg->m_ClientPath= LoadStringResource(IDS_NOT_IN_CLIENT_VIEW);
		}

		delete pCmd2;

		CObList *list= pCmd->GetList();
		ASSERT_KINDOF(CObList, list);

        POSITION pos= list->GetHeadPosition();
		while(pos != NULL)
		{
			CP4FileStats *fs= (CP4FileStats *) list->GetNext(pos);
			
			CString str;
			CString strUser;
			CString strChange;
			CString strAction;

			if( fs->GetOpenChangeNum() == 0 )
				strChange= LoadStringResource(IDS_DEFAULT_CHANGE);
			else
				strChange.FormatMessage(IDS_CHANGE_n, fs->GetOpenChangeNum()); 

			strUser= fs->GetOtherUsers();
			if( fs->IsMyOpen() && strUser.IsEmpty() )
			{
				strUser= thisuser;
				strAction= fs->GetActionStr(fs->GetMyOpenAction());
			}
			else
				strAction= fs->GetActionStr(fs->GetOtherOpenAction());

			str.Format(_T("%s - %s (%s)"), strUser, strChange, strAction);
			
			if( fs->IsOtherLock() )
				str += " " + LoadStringResource(IDS_STAR_LOCKED);
			
			dlg->m_StrList.AddHead( str );
			
			delete fs;
		}
		// Display the info
		if (!dlg->Create(IDD_FILE_INFORMATION, this))	// display the description dialog box
		{
			dlg->DestroyWindow();	// some error! clean up
			delete dlg;
		}
	}

	UPDATE_STATUS(_T(""));
	delete pCmd;
	
	return 0;
}

LRESULT CMerge2Dlg::OnP4EndFileInformation( WPARAM wParam, LPARAM lParam )
{
	CFileInfoDlg *dlg = (CFileInfoDlg *)lParam;
	dlg->DestroyWindow();
	return TRUE;
}

void CMerge2Dlg::OnFileRevisionhistory() 
{
	UpdateData();
	int i;
	int rev = -1;
	switch(m_FilesFlag)
	{
	case 0:
		m_itemStr = m_YourFile;
		break;
	case 1:
		m_itemStr = m_TheirFile;
		m_itemStr.TrimLeft();
		if ((i = m_itemStr.Find(_T('#'))) != -1)
		{
			rev = _tstoi(m_itemStr.Mid(i+1));
			m_itemStr = m_itemStr.Left(i);
		}
		break;
	case 2:
	default:
		ASSERT(0);
		return;
	}

	CCmd_History *pCmd= new CCmd_History;
	pCmd->Init( MainFrame()->GetDepotWnd(), RUN_ASYNC, HOLD_LOCK, m_Key);
	pCmd->SetCallingWnd(m_hWnd);
	pCmd->SetInitialRev(rev, m_itemStr);
	if( pCmd->Run( LPCTSTR(m_itemStr)) )
	{
		UPDATE_STATUS( LoadStringResource(IDS_REQUESTING_HISTORY) );
	}
	else
		delete pCmd;
}

void CMerge2Dlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	if (m_InitRect.Height())
	{
		lpMMI->ptMinTrackSize.x= m_InitRect.Width();
		lpMMI->ptMinTrackSize.y= lpMMI->ptMaxTrackSize.y= m_InitRect.Height();
	}
}

void CMerge2Dlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	CWnd *pWnd=GetDlgItem(IDC_YOURFILE);
	if (!pWnd || !IsWindow(pWnd->m_hWnd))
		return;

	// Compute the change in width (dx)
	CRect rect;
	GetWindowRect(&rect);
	int dx = rect.Width() - m_LastWidth;
	// Save the new width
	m_LastWidth = rect.Width();

	// Widen the edit and group boxes by the change in width (dx)
	pWnd->GetWindowRect(&rect);
	pWnd->SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								   rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
	pWnd = GetDlgItem(IDC_THEIRFILE);
	pWnd->GetWindowRect(&rect);
	pWnd->SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								   rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
	pWnd = GetDlgItem(IDC_STATIC1);
	pWnd->GetWindowRect(&rect);
	pWnd->SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								   rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
	// We have to show the radio group boxes in order to resize them
	UpdateData();
	GetDlgItem(IDC_STATIC_1)->ShowWindow( SW_SHOW );
	GetDlgItem(IDC_STATIC_2)->ShowWindow( SW_SHOW );

	pWnd = GetDlgItem(IDC_STATIC_1);
	pWnd->GetWindowRect(&rect);
	pWnd->SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								   rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
	pWnd = GetDlgItem(IDC_STATIC_2);
	pWnd->GetWindowRect(&rect);
	pWnd->SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								   rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
	GetDlgItem(IDC_STATIC_1)->ShowWindow( m_FilesFlag == 0 ? SW_SHOW : SW_HIDE );
	GetDlgItem(IDC_STATIC_2)->ShowWindow( m_FilesFlag == 1 ? SW_SHOW : SW_HIDE );

	// Slide the buttons to the right by the change in width
	pWnd = GetDlgItem(IDC_BDIFF);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);

	pWnd = GetDlgItem(IDC_BEDIT);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);

	pWnd = GetDlgItem(IDC_BPROPERTIES);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);

	pWnd = GetDlgItem(IDC_BHISTORY);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);

	pWnd = GetDlgItem(IDC_BACCEPT);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);

	pWnd = GetDlgItem(IDCANCEL);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);

	pWnd = GetDlgItem(IDC_CANCEL_ALL);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);

	pWnd = GetDlgItem(IDHELP);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);
}
