//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// HistoryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "HistoryDlg.h"
#include "viewerdlg.h"
#include "Mainfrm.h"
#include <process.h>
#include "cmd_diff.h"
#include "cmd_diff2.h"
#include "cmd_get.h"
#include "cmd_history.h"
#include "cmd_prepbrowse.h"
#include "DepotTreeCtrl.h"
#include "SpecDescDlg.h"
#include "hlp\p4win.hh"
#include "RegKeyEx.h"
#include "StringUtil.h"
#include "strops.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static LPCTSTR sRegKey = _T("Software\\Perforce\\P4Win\\Layout\\RevisionHistoryDlg");
static LPCTSTR sRegValue_ColumnWidths = _T("Column Widths");

#define FILELOGdashH 1
#define HOLD_LOCK_IF_HAVE_KEY (m_Key ? HOLD_LOCK : LOSE_LOCK)
#define UPDATE_STATUS(x) ((CMainFrame *)AfxGetMainWnd())->UpdateStatus(x)
int CALLBACK ListSort(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

static	BOOL	bReady = FALSE;

// Module global for use in sort callback
CHistoryDlg *pDlg;

/////////////////////////////////////////////////////////////////////////////
// HistoryDlg dialog

CHistoryDlg::CHistoryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHistoryDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHistoryDlg)
	m_CompleteHist = m_CompleteHistIC = m_CompleteHistI = FALSE;
	//}}AFX_DATA_INIT
	m_pParent = pParent;
	m_Busy = m_Rerun = m_InitiallyNotBusy = FALSE;
	m_SizeSet = FALSE;
	m_SortAscending=FALSE;
	m_LastSortColumn=1;
	pDlg=this;
	m_InitRect.SetRect(0,0,100,100);
	m_WinPos.SetWindow( this, _T("RevisionHistoryDlg") );
	m_listHeight = m_dlgHeight = m_lorgHeight = m_Key = 0;
	m_EnableShowIntegs = TheApp()->m_RevHistEnableShowIntegs;
	m_IsMinimized = FALSE;
	m_More = FALSE;
	m_pFRDlg = NULL;
	m_FindWhatFlags = FR_DOWN | FR_HIDEWHOLEWORD;
}

CHistoryDlg::~CHistoryDlg()
{
	// can't use MainFrame()-> construct
	// because mainfram might have closed.
	CMainFrame * mainWnd = MainFrame();
	if (mainWnd)
		mainWnd->SetGotUserInput( );
}

void CHistoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHistoryDlg)
	DDX_Control(pDX, IDC_REVFILETYPE, m_RevFileType);
	DDX_Control(pDX, IDC_REVISIONACTION, m_RevisionAction);
	DDX_Control(pDX, IDC_REVISIONINFO, m_RevisionInfo);
	DDX_Control(pDX, IDC_REVISIONLIST, m_ListCtl);
	DDX_Check(pDX, IDC_COMPLETEHIST, m_CompleteHistI);
	DDX_Radio(pDX, IDC_RADIO1, m_CompleteHistIC);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHistoryDlg, CDialog)
	//{{AFX_MSG_MAP(CHistoryDlg)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_REVISIONLIST, OnColumnclickRevlist)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_REVISIONLIST, OnItemchangedRevlist)
	ON_UPDATE_COMMAND_UI(ID_FILEREVISION_DIFF2, OnUpdateDiff2)
	ON_UPDATE_COMMAND_UI(ID_FILE_RECOVER, OnUpdateRecover)
	ON_UPDATE_COMMAND_UI(ID_FILEREVISION_GET, OnUpdateGet)
	ON_UPDATE_COMMAND_UI(ID_CHANGE_DESCRIBE, OnUpdateDescribeChg)
	ON_UPDATE_COMMAND_UI(ID_POSITIONDEPOT, OnUpdatePositionDepot)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateCopyRows)
	ON_UPDATE_COMMAND_UI(ID_FILE_REVISIONTREE, OnUpdateFileRevisiontree)
	ON_UPDATE_COMMAND_UI(ID_FILE_ANNOTATIONS, OnUpdateFileAnnotate)
	ON_UPDATE_COMMAND_UI(ID_FILE_ANNOTATE, OnUpdateFileAnnotate)
	ON_UPDATE_COMMAND_UI(ID_FILE_ANNOTATEALL, OnUpdateFileAnnotate)
	ON_UPDATE_COMMAND_UI(ID_FILE_ANNOTATECHG, OnUpdateFileAnnotate)
	ON_UPDATE_COMMAND_UI(ID_FILE_ANNOTATECHGALL, OnUpdateFileAnnotate)
	ON_COMMAND(ID_FILEREVISION_DIFF2, OnDiff2)
	ON_COMMAND(ID_FILE_RECOVER, OnRecover)
	ON_COMMAND(ID_FILEREVISION_GET, OnGet)
	ON_COMMAND(ID_CHANGE_DESCRIBE, OnDescribeChg)
	ON_COMMAND(ID_POSITIONDEPOT, OnPositionDepot)
	ON_COMMAND(ID_EDIT_COPY, OnCopyRows)
	ON_COMMAND(ID_FILE_REVISIONTREE, OnFileRevisionTree)
	ON_COMMAND(ID_FILE_ANNOTATIONS, OnFileTimeLapseView)
	ON_COMMAND(ID_FILE_ANNOTATE, OnFileAnnotate)
	ON_COMMAND(ID_FILE_ANNOTATEALL, OnFileAnnotateAll)
	ON_COMMAND(ID_FILE_ANNOTATECHG, OnFileAnnotateChg)
	ON_COMMAND(ID_FILE_ANNOTATECHGALL, OnFileAnnotateChgAll)
	ON_COMMAND(ID_PERFORCE_OPTIONS, OnPerforceOptions)
	ON_UPDATE_COMMAND_UI(ID_POSITIONTOPATTERN, OnUpdatePositionToPattern)
	ON_COMMAND(ID_POSITIONTOPATTERN, OnPositionToPattern)
	ON_UPDATE_COMMAND_UI(ID_FINDNEXT, OnUpdatePositionToNext)
	ON_COMMAND(ID_FINDNEXT, OnPositionToNext)
	ON_UPDATE_COMMAND_UI(ID_FINDPREV, OnUpdatePositionToPrev)
	ON_COMMAND(ID_FINDPREV, OnPositionToPrev)
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDB_DIFFVSCLIENT, OnDiffvsclient)
	ON_BN_CLICKED(IDC_COMPLETEHIST, OnCompletehist)
	ON_BN_CLICKED(IDC_RADIO1, OnCompletehist)
	ON_BN_CLICKED(IDC_RADIO2, OnCompletehist)
	ON_BN_CLICKED(IDC_RADIO3, OnCompletehist)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_MORE, OnMore)
	ON_BN_CLICKED(IDB_BROWSE, OnFileAutobrowse)
	ON_BN_CLICKED(IDB_DIFFREVISIONS, OnDiff2)
	ON_BN_CLICKED(IDB_GET, OnGet)
	ON_NOTIFY(LVN_KEYDOWN, IDC_REVISIONLIST, OnKeydownRevisionlist)
	ON_WM_CLOSE()
 	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(ID_FILE_BROWSER_1, ID_FILE_BROWSER_1+MAX_MRU_VIEWERS-1, OnFileMRUBrowser)
	ON_COMMAND(ID_FILE_NEWBROWSER, OnFileNewBrowser)
	ON_COMMAND(ID_FILE_QUICKBROWSE, OnFileQuickbrowse)
	ON_MESSAGE(WM_P4PREPBROWSE, OnP4ViewFile )
	ON_MESSAGE(WM_P4GET, OnP4Get )
	ON_MESSAGE(WM_P4DIFF, OnP4Diff )
	ON_MESSAGE(WM_P4DIFF2, OnP4Diff2 )
	ON_MESSAGE(WM_P4DESCRIBE, OnP4Describe )
	ON_MESSAGE(WM_P4ENDDESCRIBE, OnP4EndDescribe )
	ON_MESSAGE(WM_UPDATEHAVEREV, OnUpdateHaveRev )
	ON_MESSAGE(WM_QUITTING, OnQuitting )
	ON_WM_HELPINFO()
    ON_REGISTERED_MESSAGE( WM_FINDREPLACE, OnFindReplace )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// HistoryDlg message handlers

INT_PTR CHistoryDlg::DoModal() 
{
	return CDialog::DoModal();
}

void CHistoryDlg::Init(CHistory *hist, CString &ftype, BOOL isText, 
					   HWND depotWnd, int haveRev,
					   BOOL myOpenFile, const int initialRev /*=-1*/, 
					   LPCTSTR initialName /*=NULL*/, int key /*=0*/)
{
	ASSERT(IsWindow(depotWnd));
	ASSERT( haveRev > 0 || !myOpenFile );

	m_pHistory=hist;
	m_FileType=ftype;
	m_ViewFileIsText= isText;
	m_DepotWnd=depotWnd;
	m_HaveRev= haveRev;
	m_MyOpenFile= myOpenFile;
	m_InitialRev= initialRev;
	m_InitialName = initialName ? initialName : _T("");
	m_Key = key;
}

void CHistoryDlg::OnContextMenu(CPoint screen, int index) 
{
	if(m_Busy)
		return;

	// make a new selection new if reqd
	if(m_ListCtl.GetItemState(index,LVIS_SELECTED) != LVIS_SELECTED)
	{
		for(int i=m_ListCtl.GetItemCount(); i>=0; i-- )
			m_ListCtl.SetItemState(i, 0, LVIS_SELECTED);
		
		m_ListCtl.SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
	}

	// create an empty context menu
	CMenu popMenu;
	popMenu.CreatePopupMenu();
	CMenu viewMenu;
	viewMenu.CreatePopupMenu();
	int i;
	int actualMRUs=0;

	switch(m_ListCtl.GetSelectedCount())
	{
	case 1:
	  {
		// If revision is not deleted, create a view file submenu
		CRevision *rev= (CRevision *) m_ListCtl.GetItemData(index);
		ASSERT(rev);
		int  bAnn = 0;
		BOOL bTxt = FALSE;
		if ((rev->m_ChangeType.Find(_T("delete")) != 0) 
		 && (rev->m_ChangeType.Find(_T("purge")) != 0))
		{
			for(i=0; i < MAX_MRU_VIEWERS; i++)
			{
				if( GET_P4REGPTR()->GetMRUViewerName(i).GetLength() > 0 )
				{
					viewMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_FILE_BROWSER_1+i, 
										CString ( _T("&") + GET_P4REGPTR()->GetMRUViewerName(i)) );
					actualMRUs++;
				}
			}

			if( actualMRUs > 0)
				viewMenu.AppendMenu(MF_SEPARATOR);

			viewMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_FILE_QUICKBROWSE, 
								LoadStringResource( IDS_ASSOCVIEWER ));
			viewMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_FILE_NEWBROWSER, 
								LoadStringResource( IDS_OTHERVIEWER ));
			if (GET_SERVERLEVEL() >= 14)
			{
				bAnn = MainFrame()->HaveTLV();
				if (bAnn)
				{
					CString fname = GetSelectedFileName();
					bAnn = ((CDepotTreeCtrl *)m_pParent)->IsInRemoteDepot(&fname) ? 0 : bAnn;
				}
				bTxt = ((m_FileType.Find(_T("text")) != -1) 
					 || (m_FileType.Find(_T("symlink")) != -1)) ? TRUE : FALSE;
				if (bTxt)
				{
					viewMenu.AppendMenu(MF_SEPARATOR);
					viewMenu.AppendMenu( stringsON, ID_FILE_ANNOTATE, 
							  LoadStringResource ( IDS_FILE_ANNOTATE ) );
					viewMenu.AppendMenu( stringsON, ID_FILE_ANNOTATEALL, 
							  LoadStringResource ( IDS_FILE_ANNOTATEALL ) );
					viewMenu.AppendMenu( stringsON, ID_FILE_ANNOTATECHG, 
							  LoadStringResource ( IDS_FILE_ANNOTATECHG ) );
					viewMenu.AppendMenu( stringsON, ID_FILE_ANNOTATECHGALL, 
							  LoadStringResource ( IDS_FILE_ANNOTATECHGALL ) );
				}
			}
			popMenu.AppendMenu(MF_POPUP, (UINT_PTR) viewMenu.GetSafeHmenu(), LoadStringResource(IDS_VIEW_THIS_REVISION_USING));
		}

		if (rev->m_FName == m_LatestName)
			popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_FILEREVISION_GET, LoadStringResource(IDS_SYNC_TO_THIS_REVISION));
		if (m_LatestIsDeleted && rev->m_ChangeType.Find(_T("delete")) != 0)
			popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_FILE_RECOVER, LoadStringResource(IDS_RECOVERDELFILE));
		if (!((CDepotTreeCtrl *)m_pParent)->IsInRemoteDepot(&(rev->m_FName)))
			popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_CHANGE_DESCRIBE, LoadStringResource(IDS_DESCRIBESUBMITTED) );
		if (bAnn == 1)
		{
			popMenu.AppendMenu( stringsON, ID_FILE_REVISIONTREE, LoadStringResource( IDS_REVISIONTREE ));
			if (bTxt)
				popMenu.AppendMenu( stringsON, ID_FILE_ANNOTATIONS, LoadStringResource ( IDS_ANNOTATIONS ) );
		}
		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_POSITIONDEPOT, LoadStringResource(IDS_POSITIONDEPOT) );
		break;
	  }
	case 2:
	  {
		CRevision *rev1 = 0;
		CRevision *rev2 = 0;
		for(int i= 0; i < m_ListCtl.GetItemCount(); i++)
		{
			if(m_ListCtl.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
			{
				if (!rev1)
				{
					rev1= (CRevision *) m_ListCtl.GetItemData(i);
				}
				else
				{
					rev2= (CRevision *) m_ListCtl.GetItemData(i);
					break;
				}
			}
		}
		ASSERT(rev1);
		ASSERT(rev2);
		if (rev1->m_ChangeType.Find(_T("delete")) != 0 
		 && rev1->m_ChangeType.Find(_T("purge")) != 0 
		 && rev2->m_ChangeType.Find(_T("delete")) != 0
		 && rev2->m_ChangeType.Find(_T("purge")) != 0)
			popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_FILEREVISION_DIFF2, LoadStringResource(IDS_DIFF_TWO_REVISIONS));
		break;
	  }
	default:
		break;
	}

	if (m_ListCtl.GetSelectedCount())
		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_EDIT_COPY, LoadStringResource(IDS_amp_Copy_tab_Ctrl_C));
	popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_POSITIONTOPATTERN, LoadStringResource(IDS_FINDPATTERN));

	// Finally blast the menu onto the screen
	if(popMenu.GetMenuItemCount() > 0)
		popMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,	screen.x, screen.y, this);
	
}

void CHistoryDlg::OnUpdateDiff2(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_Busy && m_ListCtl.GetSelectedCount()==2);
}

void CHistoryDlg::OnUpdateRecover(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_Busy && m_ListCtl.GetSelectedCount()==1 
		                   && m_LatestIsDeleted);
}

void CHistoryDlg::OnUpdateGet(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_Busy && m_ListCtl.GetSelectedCount()==1);
}

void CHistoryDlg::OnUpdateDescribeChg(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_Busy && m_ListCtl.GetSelectedCount()==1);
}

void CHistoryDlg::OnUpdatePositionDepot(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_Busy && m_ListCtl.GetSelectedCount());
}

void CHistoryDlg::OnUpdateCopyRows(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_Busy && m_ListCtl.GetSelectedCount());
}

void CHistoryDlg::OnUpdateFileRevisiontree(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY() && GET_SERVERLEVEL() >= 14
					&& m_ListCtl.GetSelectedCount()==1);
}

void CHistoryDlg::OnUpdateFileAnnotate(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY() && GET_SERVERLEVEL() >= 14
					&& m_ListCtl.GetSelectedCount()==1);
}

void CHistoryDlg::OnFileMRUBrowser(UINT  nID)
{
	if(m_ListCtl.GetSelectedCount() != 1)
		{ ASSERT(0); return; }

	// Note which viewer is to be used
	m_Viewer= GET_P4REGPTR()->GetMRUViewer( nID - ID_FILE_BROWSER_1 );
	GET_P4REGPTR()->AddMRUViewer(m_Viewer);

	// Fetch the head revision of the file to a temp filename
	CCmd_PrepBrowse *pCmd= new CCmd_PrepBrowse;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK_IF_HAVE_KEY, m_Key);
	if( pCmd->Run( GetSelectedFileName(), m_FileType, GetSelectedRevision() ) )
	{
		m_Busy=TRUE;
		EnableButtons();
		UPDATE_STATUS(LoadStringResource(IDS_FETCHING_FILE));
	}
	else
		delete pCmd;
}

void CHistoryDlg::OnFileNewBrowser()
{
	if(m_ListCtl.GetSelectedCount() != 1)
		{ ASSERT(0); return; }

	// Fire up a common dlg to find new file
	CFileDialog fDlg(TRUE, _T("exe"), NULL,  
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NONETWORKBUTTON | OFN_PATHMUSTEXIST,
		LoadStringResource(IDS_HISTORY_BROWSE_FILTER), this,
		MainFrame()->m_osVer.dwMajorVersion < 5 ? OPENFILENAME_SIZE_VERSION_400 : sizeof(OPENFILENAME)); 

	if(fDlg.DoModal() == IDOK)
	{
		m_Viewer= fDlg.GetPathName();
		if(m_Viewer.GetLength() > 0)
		{
			// User selected a valid viewer, so try to run it
			GET_P4REGPTR()->AddMRUViewer(m_Viewer);
			OnFileMRUBrowser(ID_FILE_BROWSER_1);
		}
	}
}

void CHistoryDlg::OnFileQuickbrowse()
{
	FileQuickbrowse(FALSE);
}

void CHistoryDlg::FileQuickbrowse(BOOL ckifdeleted)
{
	if(m_ListCtl.GetSelectedCount() != 1)
		{ ASSERT(0); return; }

	if(ckifdeleted)
	{
		// don't allow quickbrowse for deleted files
		CRevision *rev = 0;
		for(int i= 0; i < m_ListCtl.GetItemCount(); i++)
		{
			if(m_ListCtl.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
			{
				rev= (CRevision *) m_ListCtl.GetItemData(i);
				break;
			}
		}
		ASSERT(rev);
		if (rev->m_ChangeType.Find(_T("delete")) == 0
		 || rev->m_ChangeType.Find(_T("purge")) == 0)
		{
			MessageBeep(0);
			return;
		}
	}

	m_Viewer=_T("SHELLEXEC");

	// Fetch the selected revision of the file to a temp filename
	CCmd_PrepBrowse *pCmd= new CCmd_PrepBrowse;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK_IF_HAVE_KEY, m_Key);
	if( pCmd->Run( GetSelectedFileName(), m_FileType, GetSelectedRevision() ) )
	{
		m_Busy=TRUE;
		EnableButtons();
		UPDATE_STATUS( LoadStringResource(IDS_FETCHING_FILE) );
	}
	else
		delete pCmd;
}


void CHistoryDlg::OnFileAutobrowse() 
{
	if(m_ListCtl.GetSelectedCount() != 1)
		{ ASSERT(0); return; }
	
	// Ask the user to pick a viewer
	CViewerDlg dlg;
	if(dlg.DoModal() == IDCANCEL)
		return;

	m_Viewer=dlg.GetViewer();
	if(m_Viewer != _T("SHELLEXEC"))
		GET_P4REGPTR()->AddMRUViewer(m_Viewer);

	// Fetch the head revision of the file to a temp filename
	CCmd_PrepBrowse *pCmd= new CCmd_PrepBrowse;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK_IF_HAVE_KEY, m_Key);
	if( pCmd->Run( GetSelectedFileName(), m_FileType, GetSelectedRevision() ) )
	{
		m_Busy=TRUE;
		EnableButtons();
		UPDATE_STATUS( LoadStringResource(IDS_FETCHING_FILE) );
	}
	else
		delete pCmd;
}

void CHistoryDlg::OnFileRevisionTree() 
{
	if(m_ListCtl.GetSelectedCount() != 1)
		{ ASSERT(0); return; }
	
	CString filename = GetSelectedFileName();
	TheApp()->CallP4RevisionTree(filename);	// use p4v.exe for revision tree
}

void CHistoryDlg::OnFileTimeLapseView() 
{
	if(m_ListCtl.GetSelectedCount() != 1)
		{ ASSERT(0); return; }
	
	CString filename = GetSelectedFileName();
	TheApp()->CallP4A(filename, _T(""), 0);	// use p4v.exe for annotate
}

void CHistoryDlg::OnFileAnnotate() 
{
	FileAnnotate(FALSE);
}

void CHistoryDlg::OnFileAnnotateAll() 
{
	FileAnnotate(TRUE);
}

void CHistoryDlg::OnFileAnnotateChg() 
{
	FileAnnotate(FALSE, TRUE);
}

void CHistoryDlg::OnFileAnnotateChgAll() 
{
	FileAnnotate(TRUE, TRUE);
}

void CHistoryDlg::FileAnnotate(BOOL bAll, BOOL bChg/*=FALSE*/) 
{
	if(m_ListCtl.GetSelectedCount() != 1)
		{ ASSERT(0); return; }
	
	CString filename = GetSelectedFileName();
	CString tempfilelogname;

	// Fetch the annotated file to a temp filename
	CCmd_PrepBrowse *pCmd= new CCmd_PrepBrowse;
	if (tempfilelogname.GetLength() > 0)
	{
		pCmd->Init(MainFrame()->GetDepotWnd(), RUN_ASYNC, HOLD_LOCK_IF_HAVE_KEY, m_Key);
		pCmd->SetTempFilelog(tempfilelogname);
		bAll = TRUE;
		bChg = FALSE;
	}
	else
		pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK_IF_HAVE_KEY, m_Key );
	if( pCmd->Run( FALSE, filename, m_FileType, 
		bAll, bChg, FALSE, GetSelectedRevision(), 
				GET_P4REGPTR()->GetAnnotateWhtSpace(),
				bChg ? GET_P4REGPTR()->GetAnnotateIncInteg() : FALSE ) ) 
	{
		m_Busy = tempfilelogname.GetLength() > 0 ? FALSE : TRUE;
		EnableButtons();
		UPDATE_STATUS( LoadStringResource(IDS_FETCHING_FILE) );
		m_Viewer=GET_P4REGPTR()->GetEditApp();
	}
	else
		delete pCmd;
}

CString CHistoryDlg::WriteTempHistFile()
{
	Error e;
	FileSys *pOutputFile= FileSys::Create( FST_TEXT );
	CString TempPath= GET_P4REGPTR()->GetTempDir();
	CString TempName;
	CString FileName = m_InitialName;
	int i;
	if (((i = FileName.ReverseFind(_T('\\'))) != -1)
		|| ((i = FileName.ReverseFind(_T('/'))) != -1))
		FileName = FileName.Mid(i+1);

	for(i=0; i< 100; i++)
	{
		e.Clear();
		TempName.Format(_T("%s\\FileLog-Rev-%d-%s.txt"), TempPath, i, FileName);
		pOutputFile->Set(CharFromCString(TempName));
		
		if( !e.Test() )
			pOutputFile->MkDir( &e );// Prepare write (makes dir as required)
		if( !e.Test() )
		{
			pOutputFile->Perms( FPM_RW );			// Open it
			pOutputFile->Open( FOM_WRITE, &e );
		}
		if(!e.Test())
			break;
	}
	if(e.Test())
	{
		CString txt;
		txt.Format(_T("Error opening temporary file:\n %s"), TempName);
		TheApp()->StatusAdd(txt, SV_ERROR);

		delete pOutputFile;
		return _T("");;
	}

	StrBuf sptr;
	CString str = m_InitialName + _T('\n');
	sptr.Set(const_cast<char*>((const char*)CharFromCString(str)));
	pOutputFile->Write( &sptr, &e );
	if(e.Test())
	{
		CString txt;
		txt.Format(_T("Error writing temporary file header:\n %s"), TempName);
		TheApp()->StatusAdd(txt, SV_ERROR);
		delete pOutputFile;
		return _T("");
	}
	BOOL b = FALSE;
	CRevision *rev=m_pHistory->GetHeadRevision();
	int prevrev;
	do
	{
		prevrev = rev->m_RevisionNum;
		if (m_InitialName == rev->m_FName)
		{
			b = TRUE;
			CString chgType = rev->m_ChangeType;
			CString integs = _T("");
			if ((i = chgType.Find(_T(": "))) != -1)
			{
				integs = chgType.Mid(i + 2);
				chgType = chgType.Left(i);
			}
			str.Format(_T("... #%d change %d %s on %s by %s %s"), 
				rev->m_RevisionNum, rev->m_ChangeNum, chgType, 
				rev->m_Date, rev->m_User, rev->m_ChangeDescription);
			str.Replace(_T("\n"), _T("\n        "));
			if (integs.GetLength() > 0)
			{
				integs.Replace(_T("; "), _T("\n... ... "));
				str += _T("\n\n... ... ") + integs;
			}
			str += _T("\n\n");
			str.Remove(_T('\r'));
			sptr.Set(const_cast<char*>((const char*)CharFromCString(str)));
			pOutputFile->Write( &sptr, &e );
			if(e.Test())
			{
				CString txt;
				txt.Format(_T("Error writing temporary file:\n %s"), TempName);
				TheApp()->StatusAdd(txt, SV_ERROR);
				delete pOutputFile;
				return _T("");
			}
		}
		rev=m_pHistory->GetPrevRevision();
	} while (rev && (rev->m_RevisionNum < prevrev));

	pOutputFile->ClearDeleteOnClose();
	pOutputFile->Close(&e);
	delete pOutputFile;
	return TempName;
}

LRESULT CHistoryDlg::OnP4Diff(WPARAM wParam, LPARAM lParam)
{
	CCmd_Diff *pCmd= (CCmd_Diff *) wParam;
	if(pCmd->GetDiffRunCount() == 0)
	{
		AfxMessageBox(pCmd->GetDiffErrCount() ? pCmd->GetDiffErrBuf() : LoadStringResource(IDS_FILES_ARE_IDENTICAL), 
			MB_ICONINFORMATION);
	}
	
	UPDATE_STATUS(_T(""));
	m_Busy=FALSE;
	EnableButtons();
	delete pCmd;
	return 0;
}

LRESULT CHistoryDlg::OnP4Diff2(WPARAM wParam, LPARAM lParam)
{
	CCmd_Diff2 *pCmd= (CCmd_Diff2 *) wParam;
	CString msg= pCmd->GetInfoText();
	if( ! msg.IsEmpty() )
	{
		AfxMessageBox( msg, MB_ICONINFORMATION);
	}
	
	UPDATE_STATUS(_T(""));
	m_Busy=FALSE;
	EnableButtons();
	delete pCmd;
	return 0;
}


LRESULT CHistoryDlg::OnP4Get(WPARAM wParam, LPARAM lParam)
{
	UPDATE_STATUS(_T(""));
		
	::SendMessage(m_DepotWnd, WM_P4GET, wParam, lParam);
	m_Busy=FALSE;
	EnableButtons();
	return 0;
}

// TODO: This code is pretty much a copy of the code in CDepotView::RunViewer()
// Might want to craft a single file viewing class that can be instantiated from
// anywhere, or perhaps make CMainFrame be responsible for all file viewing.
LRESULT CHistoryDlg::OnP4ViewFile(WPARAM wParam, LPARAM lParam)
{
	UPDATE_STATUS(_T(""));
	CString tempName;
	CString msg;

	m_Busy=FALSE;
	EnableButtons();

	CCmd_PrepBrowse *pCmd= (CCmd_PrepBrowse *) wParam;

	if(!pCmd->GetError())
	{
		m_ViewFilePath= pCmd->GetTempName();

		// First, get the file extension, if any, and find out if
		// its a text file
		CString extension;
		int slash= m_ViewFilePath.ReverseFind(_T('\\'));
		if(slash != -1)
			extension=m_ViewFilePath.Mid(slash+1);
		else
			extension=m_ViewFilePath;

		int dot= extension.ReverseFind(_T('.'));
		if(dot == -1)
			extension=_T("");
		else
			extension=extension.Mid(dot+1);

		// We have the file, m_ViewFilePath, try to display it
		while(1)
		{
			if(m_Viewer == _T("SHELLEXEC"))
			{
				CString assocViewer=_T("");

				// First, see if there a P4win file association
				if(!extension.IsEmpty())
					assocViewer= GET_P4REGPTR()->GetAssociatedApp(extension);
			
				// If we still havent found a viewer, set viewer to default text app
				// if user wishes to ignore windows associations
				if(assocViewer.IsEmpty() && m_ViewFileIsText && GET_P4REGPTR()->GetIgnoreWinAssoc())
					assocViewer= GET_P4REGPTR()->GetEditApp();
			
				// Let windows take a crack at finding a viewer
				if(assocViewer.IsEmpty() && !extension.IsEmpty())
				{
					// Quick check for executeable extension, which will make ShellExec try to run the file
					HINSTANCE hinst=0;
					if( extension.CompareNoCase(_T("com")) != 0 && extension.CompareNoCase(_T("exe")) != 0 &&
						extension.CompareNoCase(_T("bat")) != 0 && extension.CompareNoCase(_T("cmd")) != 0)
					{
						// first try the non-standard M$ IDE's - Have to be done first they're so BAAAD
						// give VS .NET (non-standard!) a try.
						hinst= ShellExecute( m_hWnd, _T("Open.VisualStudio.7.1"), m_ViewFilePath, NULL, NULL, SW_SHOWNORMAL);
						if( (INT_PTR) hinst > 32 ) 
							break;  // successfull VS .NET editor launch
						if( (INT_PTR) hinst == SE_ERR_NOASSOC)	// give MSDEV (non-standard!) a try
						{
							hinst= ShellExecute( m_hWnd, _T("&Open with MSDEV"), m_ViewFilePath, NULL, NULL, SW_SHOWNORMAL);
							if( (INT_PTR) hinst > 32 )
								break;  // successfull MSDEV editor launch
						}
						hinst= ShellExecute( m_hWnd, _T("open"), m_ViewFilePath, NULL, NULL, SW_SHOWNORMAL);
						if( (INT_PTR) hinst > 32)
						{
							break;  // successfull viewer launch
						}
					}
				}

				// If windows doesnt have an associated viewer for a text file, we use the 
				// default text editor
				if(assocViewer.IsEmpty() && m_ViewFileIsText)
					assocViewer= GET_P4REGPTR()->GetEditApp();
				

				if ( TheApp()->RunViewerApp( assocViewer, m_ViewFilePath ) )
					break;  // successfull viewer launch
			}
			else
			{
				if ( TheApp()->RunViewerApp( m_Viewer, m_ViewFilePath ) )
					break;  // successfull viewer launch
			}

			CString msg;
			msg.FormatMessage(IDS_UNABLE_TO_LAUNCH_VIEWER_s, m_ViewFilePath);
			if(AfxMessageBox(msg, MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
				break;

			// Try to find an alternate viewer
			CViewerDlg dlg;
			if(dlg.DoModal() == IDCANCEL)
				break;

			m_Viewer=dlg.GetViewer();
			if(m_Viewer != _T("SHELLEXEC"))
				GET_P4REGPTR()->AddMRUViewer(m_Viewer);
		} // while
	} // no command error
	
	delete pCmd;
	UPDATE_STATUS(_T(""));
	return 0;
}


LRESULT CHistoryDlg::OnUpdateHaveRev(WPARAM wParam, LPARAM lParam) 
{
	int ctr=2;
	m_HaveRev = (int)lParam;
	CString revStr;
	revStr.Format(_T("%d"), m_HaveRev);
	for(int iItem=0; ctr && iItem < m_pHistory->GetRevisionCount(); iItem++)
	{
		if (m_InitialName != m_ListCtl.GetItemText(iItem, 1))
			continue;
		CString txt = m_ListCtl.GetItemText(iItem, 0);
		if (txt.Find(_T('*')) != -1)
		{
			txt.TrimRight(_T("* "));
			m_ListCtl.SetItemText(iItem, 0, txt);
			ctr--;
		}
		CString txtr = txt;
		txtr.TrimLeft();
		if (txtr == revStr)
		{
			txt += _T('*');
			m_ListCtl.SetItemText(iItem, 0, txt);
			ctr--;
		}
	}
	return 0;
}

LRESULT CHistoryDlg::OnQuitting(WPARAM wParam, LPARAM lParam) 
{
	if (m_pParent)
		m_pParent->SendMessage(WM_P4ENDHISTORY, 0, (LPARAM)this);
	return 0;
}

void CHistoryDlg::OnClose() 
{
	OnOK();
}

void CHistoryDlg::OnCancel() 
{
	// What happens when the user accidentally hits escape
	OnOK();	// Now if they hit Escape, the window closes.
}

void CHistoryDlg::OnDiff2() 
{
	if(m_ListCtl.GetSelectedCount() != 2)
		{ ASSERT(0); return; }

	CRevision *rev[2];
	int i, revindex;

	// Get the two file revs - (in reverse order so newer is the 2nd diff arg)
	for(revindex=0, i=m_ListCtl.GetItemCount()-1; revindex < 2 && i >=0; i--)
	{
		if(m_ListCtl.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			rev[revindex]= (CRevision *) m_ListCtl.GetItemData(i);
			revindex++;
		}
	}

	ASSERT(revindex==2);

	CString ft0;
	CString ft1;
	if ((i = rev[0]->m_ChangeDescription.Find(_T(')'))) != -1)
		 ft0 = rev[0]->m_ChangeDescription.Left(i+1);
	else ft0 = m_FileType;
	if ((i = rev[1]->m_ChangeDescription.Find(_T(')'))) != -1)
		 ft1 = rev[1]->m_ChangeDescription.Left(i+1);
	else ft1 = m_FileType;
	if (!GET_P4REGPTR()->GetDiffAppIsBinary())
	{
		if (((ft0.Find(_T("text")) != -1) && (ft1.Find(_T("binary")) != -1))
		 || ((ft0.Find(_T("binary")) != -1) && (ft1.Find(_T("text")) != -1)))
		{
			CString txt;
			txt.FormatMessage(IDS_ONLY_DIFF_SAME_TYPES_s_n_s_s_n_s, 
				rev[0]->m_FName, rev[0]->m_RevisionNum, ft0,
				rev[1]->m_FName, rev[1]->m_RevisionNum, ft1);
			AfxMessageBox(txt);
			return;
		}
	}
	if ((ft0.GetAt(0) == _T('(')) && (ft0.Find(_T(' ')) == -1))
	{
		ft0.TrimLeft(_T('('));
		ft0.TrimRight(_T(')'));
	}
	else ft0 = m_FileType;
	if ((ft1.GetAt(0) == _T('(')) && (ft1.Find(_T(' ')) == -1))
	{
		ft1.TrimLeft(_T('('));
		ft1.TrimRight(_T(')'));
	}
	else ft1 = m_FileType;

	CCmd_Diff2 *pCmd= new CCmd_Diff2;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK_IF_HAVE_KEY, m_Key);
	if( pCmd->Run( rev[0]->m_FName, rev[1]->m_FName, 
		rev[0]->m_RevisionNum, rev[1]->m_RevisionNum, ft0, ft1) )
	{
		m_Busy=TRUE;
		EnableButtons();
		UPDATE_STATUS( LoadStringResource(IDS_DIFFING_FILES) );
	}
	else
		delete pCmd;
}

void CHistoryDlg::OnDiffvsclient() 
{
	if( m_ListCtl.GetSelectedCount() != 1 || m_HaveRev == 0)
		{ ASSERT(0); return; }

	if (!m_CompleteHist || (GetSelectedFileName() == m_LatestName))
	{
		CString depotName;
		depotName.Format(_T("%s#%d"), GetSelectedFileName(), GetSelectedRevision());

		m_FileList.RemoveAll();
		m_FileList.AddHead(depotName);

		CCmd_Diff *pCmd= new CCmd_Diff;
		pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK_IF_HAVE_KEY, m_Key);
		if( pCmd->Run( &m_FileList, ( !m_MyOpenFile && m_HaveRev == GetSelectedRevision() ) 
									? _T("-f") : NULL ) )
		{
			m_Busy=TRUE;
			EnableButtons();
			UPDATE_STATUS( LoadStringResource(IDS_DIFFING_FILE) );
		}
		else
			delete pCmd;
	}
	else
	{
		// Get the rev
		CRevision *rev = 0;
		int i;
		for(i= 0; i < m_ListCtl.GetItemCount(); i++)
		{
			if(m_ListCtl.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
			{
				rev= (CRevision *) m_ListCtl.GetItemData(i);
				break;
			}
		}
		CString ft0;
		if ((i = rev->m_ChangeDescription.Find(_T(')'))) != -1)
			ft0 = rev->m_ChangeDescription.Left(i+1);
		else ft0 = m_FileType;
		if (!GET_P4REGPTR()->GetDiffAppIsBinary())
		{
			if (((ft0.Find(_T("text")) != -1) && (m_FileType.Find(_T("binary")) != -1))
			 || ((ft0.Find(_T("binary")) != -1) && (m_FileType.Find(_T("text")) != -1)))
			{
				CString txt;
				txt.FormatMessage(IDS_ONLY_DIFF_SAME_TYPES_s_n_s_s_n_s, 
					rev->m_FName, rev->m_RevisionNum, ft0,
					m_LatestName, m_HaveRev, m_FileType);
				AfxMessageBox(txt);
				return;
			}
		}
		if ((ft0.GetAt(0) == _T('(')) && (ft0.Find(_T(' ')) == -1))
		{
			ft0.TrimLeft(_T('('));
			ft0.TrimRight(_T(')'));
		}
		else ft0 = m_FileType;
		CCmd_Diff2 *pCmd= new CCmd_Diff2;
		pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK_IF_HAVE_KEY, m_Key);
		if( pCmd->Run( rev->m_FName, m_LatestName, 
			rev->m_RevisionNum, m_HaveRev, ft0, m_FileType, FALSE, TRUE) )
		{
			m_Busy=TRUE;
			EnableButtons();
			UPDATE_STATUS( LoadStringResource(IDS_DIFFING_FILES) );
		}
		else
			delete pCmd;
	}
}


void CHistoryDlg::OnRecover() 
{
	if(m_ListCtl.GetSelectedCount() != 1)
		{ ASSERT(0); return; }

	m_Recover.AddHead(m_LatestName);
	OnGet();
}
	
void CHistoryDlg::OnGet() 
{
	if(m_ListCtl.GetSelectedCount() != 1)
		{ ASSERT(0); m_Recover.RemoveAll(); return; }

	CRevision *rev = 0;
	for(int i= 0; i < m_ListCtl.GetItemCount(); i++)
	{
		if(m_ListCtl.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			rev= (CRevision *) m_ListCtl.GetItemData(i);
			break;
		}
	}

    ASSERT(rev);

	CString name;
	name.Format(_T("%s#%ld"), rev->m_FName, rev->m_RevisionNum);
	
	m_FileList.AddHead(name);
	
	CCmd_Get *pCmd= new CCmd_Get;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK_IF_HAVE_KEY, m_Key);
	if( m_Recover.GetCount() > 0 )
	{
		for (POSITION pos = m_Recover.GetHeadPosition(); pos != NULL; )
		{
			CString str = m_Recover.GetNext(pos);
			if (str.Find(_T('%')) != -1)
			{
				StrBuf b;
				StrBuf f;
				f << CharFromCString(str);
				StrPtr *p = &f;
				StrOps::StrToWild(*p, b);
				str = CharToCString(b.Value());
			}
			pCmd->Add2Recover(str);
		}
		m_Recover.RemoveAll();
	}
	pCmd->SetRevHistWnd(m_hWnd);
	pCmd->SetRevReq(rev->m_RevisionNum);
	if( pCmd->Run( &m_FileList, FALSE ) )
	{
		m_Busy=TRUE;
		EnableButtons();
	}
	else
		delete pCmd;
	GotoDlgCtrl(GetDlgItem(IDC_REVISIONLIST));
}

void CHistoryDlg::OnDescribeChg() 
{
	long changeNumber = GetSelectedChgNbr();

	if(changeNumber != 0)
	{
		CString nbr;
		nbr.Format(_T("%ld"), changeNumber);
		CCmd_Describe *pCmd= new CCmd_Describe;
		pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK_IF_HAVE_KEY, m_Key);
		if( pCmd->Run( P4DESCRIBE, nbr) )
			UPDATE_STATUS(LoadStringResource(IDS_FETCHING_CHANGELIST_DESCRIPTION));
		else
			delete pCmd;
	}
}

void CHistoryDlg::OnDescribeChgLong(long changeNumber, int flag /*= 0*/) 
{
	if(changeNumber != 0)
	{
		CString nbr;
		nbr.Format(_T("%ld"), changeNumber);
		CCmd_Describe *pCmd= new CCmd_Describe;
		pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK_IF_HAVE_KEY, m_Key);
		if( pCmd->Run( P4DESCRIBELONG, nbr, NULL, FALSE, flag) )
			UPDATE_STATUS(LoadStringResource(IDS_FETCHING_CHANGELIST_DESCRIPTION));
		else
			delete pCmd;
	}
}

LRESULT CHistoryDlg::OnP4Describe(WPARAM wParam, LPARAM lParam)
{
	CCmd_Describe *pCmd= (CCmd_Describe *) wParam;

	if(!pCmd->GetError())
	{
		CString desc= MakeCRs(pCmd->GetDescription());
		
		int key;
		CSpecDescDlg *dlg = new CSpecDescDlg(this);
		dlg->SetIsModeless(TRUE);
		dlg->SetKey(key = pCmd->HaveServerLock()? pCmd->GetServerKey() : 0);
		dlg->SetItemName( pCmd->GetReference() );
		dlg->SetDescription(desc);
		dlg->SetCaption(LoadStringResource(IDS_PERFORCE_CHANGELIST_DESCRIPTION));
		dlg->SetShowNextPrev(TRUE);
		dlg->SetShowShowDiffs(TRUE);
		dlg->SetDiffFlag(pCmd->GetFlag());
		dlg->SetViewType(P4CHANGE_SPEC);
		dlg->SetFindStrFlags(&m_FindWhatStr, m_FindWhatFlags);
		dlg->SetKey(m_Key);
		if (!dlg->Create(IDD_SPECDESC, this))	// display the description dialog box
		{
			dlg->DestroyWindow();	// some error! clean up
			delete dlg;
		}
	}
	else	// had an error - need to turn painting back on
	{
		EnumChildWindows(AfxGetMainWnd()->m_hWnd, ChildSetRedraw, TRUE);
	}
	
	delete pCmd;
	UPDATE_STATUS(_T(""));
	return 0;
}

LRESULT CHistoryDlg::OnP4EndDescribe(WPARAM wParam, LPARAM lParam)
{
	CSpecDescDlg *dlg = (CSpecDescDlg *)lParam;
	CString ref = dlg->GetItemName();
	ASSERT(!ref.IsEmpty());

	switch(wParam)				// which button did they click to close the box?
	{
	case ID_SHOWDIFFS_NORMAL:
	case ID_SHOWDIFFS_SUMMARY:
	case ID_SHOWDIFFS_UNIFIED:
	case ID_SHOWDIFFS_CONTEXT:
	case ID_SHOWDIFFS_RCS:
	case ID_SHOWDIFFS_NONE:
	{
		long l = _ttol(ref);
		OnDescribeChgLong(l, (int)wParam);
		break;
	}
	case IDC_NEXTITEM:
	case IDC_PREVITEM:
	{
		BOOL b = FALSE;
		CRevision *rev;
		long l = _ttol(ref);

		// Get the rev
		int np = wParam == IDC_NEXTITEM ? 1 : -1;
		int count = m_ListCtl.GetItemCount();
		for(int ix = -1; ++ix < count; )
		{
			rev = (CRevision *) m_ListCtl.GetItemData(ix);
			if (rev && rev->m_ChangeNum == l)
			{
				if((ix + np < 0) || (ix + np >= count))
					break;
				// clear all previous selections
				for (int i= 0; m_ListCtl.GetSelectedCount(); i++)
					m_ListCtl.SetItemState(i, 0, LVIS_SELECTED|LVIS_FOCUSED);
				// select the one we want
				m_ListCtl.SetItemState(ix + np, LVIS_SELECTED|LVIS_FOCUSED,
												LVIS_SELECTED|LVIS_FOCUSED);
				OnDescribeChg();	// display the next/prev in the list on the screen
				b = TRUE;
				break;
			}
		}
		if (b)
			break;
		// fall thru to turn painting back on
	}
	default:	// clicked OK, pressed ESC or ENTER - need to turn painting back on
		EnumChildWindows(AfxGetMainWnd()->m_hWnd, ChildSetRedraw, TRUE);
		break;
	}
	dlg->DestroyWindow();
	return TRUE;
}

int CHistoryDlg::GetSelectedRevision() 
{
	if(m_ListCtl.GetSelectedCount() != 1)
		{ ASSERT(0); return 0; }

	CRevision *rev = 0;

	// Get the rev
	for(int i= 0; i < m_ListCtl.GetItemCount(); i++)
	{
		if(m_ListCtl.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			rev= (CRevision *) m_ListCtl.GetItemData(i);
			break;
		}
	}
    ASSERT(rev);
	return rev->m_RevisionNum;
}

CString CHistoryDlg::GetSelectedFileName() 
{
	if(m_ListCtl.GetSelectedCount() != 1)
		{ ASSERT(0); return m_pHistory->GetFileName(); }

	CRevision *rev = 0;

	// Get the rev
	for(int i= 0; i < m_ListCtl.GetItemCount(); i++)
	{
		if(m_ListCtl.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			rev= (CRevision *) m_ListCtl.GetItemData(i);
			break;
		}
	}

    ASSERT(rev);
	return rev->m_FName;
}

int CHistoryDlg::GetSelectedChgNbr() 
{
	if(m_ListCtl.GetSelectedCount() != 1)
		{ ASSERT(0); return 0; }

	CRevision *rev = 0;

	// Get the rev
	for(int i= 0; i < m_ListCtl.GetItemCount(); i++)
	{
		if(m_ListCtl.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			rev= (CRevision *) m_ListCtl.GetItemData(i);
			break;
		}
	}

    ASSERT(rev);
	return rev->m_ChangeNum;
}


BOOL CHistoryDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(SERVER_BUSY() && m_InitiallyNotBusy)
		return SET_BUSYCURSOR();
	else
		return CDialog::OnSetCursor(pWnd, nHitTest, message);
}


BOOL CHistoryDlg::OnInitDialog()
{
	CRect rect;

	bReady = FALSE;
    CDialog::OnInitDialog();

	if (m_pParent)
 		MainFrame()->SetModelessWnd(this);

	m_listHeight = GET_P4REGPTR()->GetHistListHeight();
	if (!m_EnableShowIntegs || !GET_P4REGPTR()->GetEnableRevHistShowIntegs( ))
	{
		GetDlgItem(IDC_COMPLETEHIST)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMPLETEHIST)->ShowWindow(SW_HIDE);
		m_CompleteHist = FALSE;
	}
	else
	{
		m_CompleteHist = m_CompleteHistIC = m_CompleteHistI = GET_P4REGPTR()->GetFetchCompleteHist();
#ifdef	FILELOGdashH
		if (GET_SERVERLEVEL() >= 24)	// 2007.3 or later?
		{
			GetDlgItem(IDC_COMPLETEHIST)->EnableWindow(FALSE);
			GetDlgItem(IDC_COMPLETEHIST)->ShowWindow(SW_HIDE);
			if (!m_Key)
			{
				GetDlgItem(IDC_STATIC_1)->EnableWindow(TRUE);
				GetDlgItem(IDC_STATIC_1)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_RADIO1)->EnableWindow(TRUE);
				GetDlgItem(IDC_RADIO1)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_RADIO2)->EnableWindow(TRUE);
				GetDlgItem(IDC_RADIO2)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_RADIO3)->EnableWindow(TRUE);
				GetDlgItem(IDC_RADIO3)->ShowWindow(SW_SHOW);
			}
		}
		else 
#endif
			if (m_CompleteHist > 1)
			m_CompleteHist = m_CompleteHistI = 1;
	}
	UpdateData(FALSE);

	// if this is being called from an in-progress command (such as p4 resolve)
	// don't show the Complete History checkbox because it's too difficult
	// to close and reopen this dialog without dropping the lock
	if (m_Key)
	{
		GetDlgItem(IDC_COMPLETEHIST)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMPLETEHIST)->ShowWindow(SW_HIDE);
	}

	// Set the dialog caption
	CString caption;
	caption.FormatMessage(IDS_REVISION_HISTORY_FOR_s, 
		!TheApp()->m_RevHistPath.IsEmpty() ? TheApp()->m_RevHistPath
			: !m_InitialName.IsEmpty() ? m_InitialName : m_pHistory->GetFileName());
	SetWindowText(caption);

	// Modify the list control style so that the entire selected row is highlighted
	LRESULT dwStyle = ::SendMessage(m_ListCtl.m_hWnd,LVM_GETEXTENDEDLISTVIEWSTYLE,0,0);
	dwStyle |= LVS_EX_FULLROWSELECT;
	::SendMessage(m_ListCtl.m_hWnd,LVM_SETEXTENDEDLISTVIEWSTYLE,0,dwStyle);

	// Make sure list control shows selection when not the focused control
	m_ListCtl.ModifyStyle(0, LVS_SHOWSELALWAYS, 0);

	// Get original size of controls
	CWnd *pBtn=GetDlgItem(IDHELP);
	pBtn->GetWindowRect(&rect);
	m_SmlWidth = rect.Width();

	m_ListCtl.GetWindowRect(&rect);
	m_lorgHeight = rect.Height();

	int colwidth[7] = {16,5,18,18,18,10,15};	// these start out as %-ages and are converted to pixels
	colwidth[0] = colwidth[0] * rect.Width()/100;	// 1st 5 cols widths are based on orig size
	colwidth[1] = colwidth[1] * rect.Width()/100;
	colwidth[2] = colwidth[2] * rect.Width()/100;
	colwidth[3] = colwidth[3] * rect.Width()/100;
	colwidth[4] = colwidth[4] * rect.Width()/100;

	// Record the initial window size, then see if there is a registry preference
	GetWindowRect(&m_InitRect);
	m_WinPos.RestoreWindowPosition();

	// Get new size of control after resized as specified in the registry
	m_ListCtl.GetWindowRect(&rect);
	colwidth[5] = colwidth[5] * rect.Width()/100;	// last 2 col widths are based on reg size
	colwidth[6] = colwidth[6] * rect.Width()/100;

	// Get any saved column widths from registry
	RestoreSavedWidths(colwidth, 7);

	// Make sure no column completely disappeared (because you can't get it back then)
	for (int i=-1; ++i < 7; )
	{
		if (colwidth[i] < 5)
			colwidth[i] = 5;
	}

	// Handle one-time case of going from fewer than 7 cols to 7 cols
	if (!colwidth[6])
	{
		int j = 0;
		for (int i = -1; ++i < 6; )
		{
			if (colwidth[i] > 7)
			{
				colwidth[i] -= colwidth[i]/8;
				j += colwidth[i]/8;
			}
			else if (!colwidth[i] && j)
			{
				colwidth[i] += j/(7-i);
				j -= j/(7-i);
			}
		}
		colwidth[6] = j;
	}

	// Insert the columns 
	int headers[7] = {IDS_P4REVISION, IDS_P4FILE_NAME, IDS_CHANGELIST, IDS_DATE, IDS_P4USER, IDS_P4ACTION, IDS_DESCRIPTION};
	int width= GetSystemMetrics(SM_CXVSCROLL);
	int retval;
	LV_COLUMN lvCol;
	int subItem;
	for(subItem=0; subItem < 7; subItem++)
	{
		lvCol.mask= LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT |LVCF_WIDTH;
		lvCol.fmt=LVCFMT_LEFT;
        CString header = LoadStringResource(headers[subItem]); 
		lvCol.pszText=const_cast<LPTSTR>((LPCTSTR)header);
		lvCol.iSubItem=subItem;
		if(subItem < 6)
		{
			lvCol.cx=colwidth[subItem];
			width+=lvCol.cx;
		}
		else
			lvCol.cx=rect.Width() - width - 4;  // expand last column to fill window
		retval=m_ListCtl.InsertColumn(subItem, &lvCol);
	}
 
	// Add the data
	CRevision *rev = 0;
	LV_ITEM lvItem;
	int iActualItem = -1;
	CString txt;
	BOOL bMultiFileNames = FALSE;
	int	fnbr= 0;
	for(int iItem=0; iItem < m_pHistory->GetRevisionCount(); iItem++)
	{
		if(iItem==0)
		{
			rev=m_pHistory->GetLatestRevision();
			lvItem.pszText= const_cast<LPTSTR>((LPCTSTR)txt); 
			txt=rev->m_ChangeDescription;
			if (txt.GetAt(0) == _T('('))
			{
				int i = txt.Find(_T(')'));
				if (i != -1)
				{
					txt.SetAt(0, _T(' '));
					m_RevFileType.SetWindowText(LoadStringResource(IDS_FILETYPE) + (LPCTSTR)txt.Left(i));
					txt = txt.Right(txt.GetLength() - i - 1);
					txt.TrimLeft();
				}
			}
			else m_RevFileType.SetWindowText(_T(""));
			m_RevisionInfo.SetWindowText((LPCTSTR)txt);
			txt=rev->m_ChangeType;
			txt.Replace(_T("; "), _T("\r\n"));
			m_RevisionAction.SetWindowText(txt);
			fnbr = rev->m_FNbr;
		}
		else
		{
			rev=m_pHistory->GetNextRevision();
			if (fnbr != rev->m_FNbr)
				bMultiFileNames = TRUE;
		}

		for(subItem=0; subItem < 7; subItem++)
		{
			lvItem.mask=LVIF_TEXT | 
					((subItem==0) ? LVIF_PARAM : 0);
			lvItem.iItem= (subItem==0) ? iItem : iActualItem;
            ASSERT(lvItem.iItem != -1);
			lvItem.iSubItem= subItem;
			lvItem.lParam=(LPARAM) rev;
			switch(subItem)
			{
			case 0:
				{
				int r = rev->m_RevisionNum;
				txt.Format(_T("%4d"), r);
				if (r == m_HaveRev && rev->m_FName == m_InitialName)
					txt += _T('*');
				lvItem.pszText= const_cast<LPTSTR>((LPCTSTR)txt); break;
				}
			case 1: lvItem.pszText= const_cast<LPTSTR>((LPCTSTR)rev->m_FName); break;
			case 2: 
				if (rev->m_ChangeNum >= 0)	// chg# == -1 is a flag to NOT display chg#
					 txt.Format(_T("%5d"),rev->m_ChangeNum);
				else txt.Empty();
				lvItem.pszText= const_cast<LPTSTR>((LPCTSTR)txt); break;
			case 3: lvItem.pszText= const_cast<LPTSTR>((LPCTSTR)rev->m_Date); break;
			case 4: lvItem.pszText= const_cast<LPTSTR>((LPCTSTR)rev->m_User); break;
			case 5: lvItem.pszText= const_cast<LPTSTR>((LPCTSTR)rev->m_ChangeType); break;
			case 6: txt=PadCRs(rev->m_ChangeDescription);
					if (txt.GetAt(0) == _T('('))
					{
						int i = txt.Find(_T(')'));
						if (i != -1)
						{
							txt = txt.Right(txt.GetLength() - i - 1);
							txt.TrimLeft();
						}
					}
					lvItem.pszText= const_cast<LPTSTR>((LPCTSTR)txt);
					break;
			}
			
			if(subItem==0)
				iActualItem=m_ListCtl.InsertItem(&lvItem);
			else
				m_ListCtl.SetItem(&lvItem);
		}
	}
	// remember the latest name because that's the only
	// name we can use to diff a revision against the client
	m_LatestName = rev->m_FName;
	m_LatestIsDeleted = rev->m_ChangeType.Find(_T("delete")) == 0;
	
	// Sort the list
	m_ListCtl.Sort( m_LastSortColumn = bMultiFileNames ? 1 : 0, m_SortAscending );
	
	// Make sure desired item is selected
	int iPos = 0;
	if (m_InitialRev != -1)
	{
		TCHAR initrev[16];
		_stprintf(initrev, _T("%4d"), m_InitialRev);
		LVFINDINFO lvfindinfo;
		lvfindinfo.flags = LVFI_STRING;
		lvfindinfo.psz = initrev;
		lvfindinfo.lParam = 0;
		lvfindinfo.pt.x = 0;
		lvfindinfo.pt.y = 0;
		lvfindinfo.vkDirection = VK_DOWN;
		iPos = m_ListCtl.FindItem( &lvfindinfo, -1 );
		if (iPos < 0)
			iPos = 0;
		else
		{
			rev= (CRevision *) m_ListCtl.GetItemData(iPos);
			if (rev->m_FName != m_InitialName)	// != means duplicate rev#s - gotta check names
			{
				int cnt = m_ListCtl.GetItemCount();
				for (iPos = -1; ++iPos < cnt; )
				{
					rev= (CRevision *) m_ListCtl.GetItemData(iPos);
					if ((rev->m_RevisionNum == m_InitialRev)
					 && (rev->m_FName == m_InitialName))
						break;
				}
				if (iPos >= m_ListCtl.GetItemCount())
					iPos = 0;
			}
		}
	}
	m_ListCtl.SetItemState(iPos, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	if (iPos)
		m_ListCtl.EnsureVisible(iPos, FALSE);

	// Set change description to match selected item in list
	rev= (CRevision *) m_ListCtl.GetItemData(iPos);
	txt= rev->m_ChangeDescription;
	if (txt.GetAt(0) == _T('('))
	{
		int i = txt.Find(_T(')'));
		if (i != -1)
		{
			txt.SetAt(0, _T(' '));
			m_RevFileType.SetWindowText(LoadStringResource(IDS_FILETYPE) + (LPCTSTR)txt.Left(i));
			txt = txt.Right(txt.GetLength() - i - 1);
			txt.TrimLeft();
		}
	}
	else m_RevFileType.SetWindowText(_T(""));
	m_RevisionInfo.SetWindowText(txt);
	txt=rev->m_ChangeType;
	txt.Replace(_T("; "), _T("\r\n"));
	m_RevisionAction.SetWindowText(txt);

	m_RevHistCount = m_ListCtl.GetItemCount();
	// if we are showing all records for this file 
	// or this is being called from an in-progress command (such as p4 resolve)
	// remove the More button.
	if (!TheApp()->m_RevHistLast || TheApp()->m_RevHistLast > m_RevHistCount || m_Key)
	{
		GetDlgItem(IDC_MORE)->EnableWindow(FALSE);
		GetDlgItem(IDC_MORE)->ShowWindow(SW_HIDE);
	}

	// And finally, set focus to the list control so that the first 'down'
	// keystroke can be used to scroll down
	m_ListCtl.SetFocus();

	EnableButtons();

	m_InitiallyNotBusy = !SERVER_BUSY();
	bReady = TRUE;
	ShowWindow(SW_SHOW);
	return TRUE;
}

void CHistoryDlg::OnColumnclickRevlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	if(pNMListView->iSubItem != m_LastSortColumn)
		m_LastSortColumn=pNMListView->iSubItem;
	else
		m_SortAscending= !m_SortAscending;

	if(pNMListView->iItem == -1)
		m_ListCtl.Sort( m_LastSortColumn, m_SortAscending );
		
	POSITION pos = m_ListCtl.GetFirstSelectedItemPosition();
	if (pos != NULL)
		m_ListCtl.EnsureVisible(m_ListCtl.GetNextSelectedItem(pos), FALSE);

	*pResult = 0;
}

int CALLBACK ListSort(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    CRevision const *rev1 = (CRevision const *)lParam1;
    CRevision const *rev2 = (CRevision const *)lParam2;
	CString txt1, txt2;
	int retval;

	switch(lParamSort)
	{
	case 0:  // rev Number
		retval=( rev1->m_RevisionNum - 
				 rev2->m_RevisionNum );
		break;
	case 1:	 // filename
		retval= 0 - ( rev1->m_FNbr - 
					  rev2->m_FNbr );
		if (!retval)
			 retval=( rev1->m_ChangeNum - 
					  rev2->m_ChangeNum );
		break;
	case 2:  // change Number
	case 3:	 // date
		retval=( rev1->m_ChangeNum - 
				 rev2->m_ChangeNum );
		break;
	case 4:	 // user
		txt1= rev2->m_User;
		txt2= rev1->m_User;
		retval= txt2.Compare(txt1);
		break;
	case 5:	 // action
		txt1= rev2->m_ChangeType;
		txt2= rev1->m_ChangeType;
		retval= txt2.Compare(txt1);
		break;
	case 6:	 // description
		txt1= rev2->m_ChangeDescription;
		txt2= rev1->m_ChangeDescription;
		retval= txt2.Compare(txt1);
		break;
	default:
		ASSERT(0);
		return 0;
	}
	if(pDlg->m_SortAscending)
		return retval;
	else
		return -retval;
}

void CHistoryDlg::OnItemchangedRevlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	CString txt, txt1;

	// Update display of revision description
	if(pNMListView->uNewState==3)
	{
		txt=((CRevision *) pNMListView->lParam)->m_ChangeDescription;
		if (txt.GetAt(0) == _T('('))
		{
			int i = txt.Find(_T(')'));
			if (i != -1)
			{
				txt.SetAt(0, _T(' '));
				m_RevFileType.SetWindowText(LoadStringResource(IDS_FILETYPE) + (LPCTSTR)txt.Left(i));
				txt = txt.Right(txt.GetLength() - i - 1);
				txt.TrimLeft();
			}
		}
		else m_RevFileType.SetWindowText(_T(""));
		m_RevisionInfo.SetWindowText(txt);
		txt=((CRevision *) pNMListView->lParam)->m_ChangeType;
		txt.Replace(_T("; "), _T("\r\n"));
		m_RevisionAction.SetWindowText(txt);
		txt.FormatMessage(IDS_ACTIONLABEL, ((CRevision *) pNMListView->lParam)->m_ChangeNum);
		GetDlgItem(IDC_ACTIONLABEL)->SetWindowText(txt);
		txt.FormatMessage(IDS_SUMMARYLABEL, ((CRevision *) pNMListView->lParam)->m_ChangeNum);
		GetDlgItem(IDC_SUMMARYLABEL)->SetWindowText(txt);
	}

	EnableButtons();
	
	*pResult = 0;
}


void CHistoryDlg::EnableButtons()
{
	if(m_Busy)
	{
		EnableCtrl(	GetDlgItem(IDB_BROWSE), FALSE );
		EnableCtrl( GetDlgItem(IDB_GET), FALSE );
		EnableCtrl( GetDlgItem(IDB_DIFFVSCLIENT), FALSE );
		EnableCtrl( GetDlgItem(IDB_DIFFREVISIONS), FALSE );
		EnableCtrl( GetDlgItem(ID_CHANGE_DESCRIBE), FALSE );
		EnableCtrl( GetDlgItem(ID_FILE_RECOVER), FALSE );
		EnableCtrl( GetDlgItem(IDOK), FALSE );
	}
	else
	{
		BOOL bDeleted  = FALSE;
		CRevision *rev = 0;
		EnableCtrl( GetDlgItem(IDOK), TRUE );

		switch(m_ListCtl.GetSelectedCount())
		{
		case 1:
			for(int i= 0; i < m_ListCtl.GetItemCount(); i++)
			{
				if(m_ListCtl.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
				{
					rev= (CRevision *) m_ListCtl.GetItemData(i);
					break;
				}
			}
			ASSERT(rev);
			bDeleted = (rev->m_ChangeType.Find(_T("delete")) == 0) 
					|| (rev->m_ChangeType.Find(_T("purge")) == 0);
			EnableCtrl(	GetDlgItem(IDB_BROWSE), !bDeleted );
			EnableCtrl( GetDlgItem(IDB_GET), !m_CompleteHist || rev->m_FName == m_LatestName );
			EnableCtrl( GetDlgItem(IDB_DIFFVSCLIENT), m_HaveRev && !bDeleted );
			EnableCtrl( GetDlgItem(IDB_DIFFREVISIONS), FALSE );
			EnableCtrl( GetDlgItem(ID_CHANGE_DESCRIBE), 
				!((CDepotTreeCtrl *)m_pParent)->IsInRemoteDepot(&(rev->m_FName)) );
			EnableCtrl( GetDlgItem(ID_FILE_RECOVER), m_LatestIsDeleted && !bDeleted);
			break;
		case 2:
		  {
			CRevision *rev1 = 0;
			CRevision *rev2 = 0;
			for(int i= 0; i < m_ListCtl.GetItemCount(); i++)
			{
				if(m_ListCtl.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
				{
					if (!rev1)
					{
						rev1= (CRevision *) m_ListCtl.GetItemData(i);
					}
					else
					{
						rev2= (CRevision *) m_ListCtl.GetItemData(i);
						break;
					}
				}
			}
			ASSERT(rev1);
			ASSERT(rev2);
			EnableCtrl(	GetDlgItem(IDB_BROWSE), FALSE );
			EnableCtrl( GetDlgItem(IDB_GET), FALSE );
			EnableCtrl( GetDlgItem(IDB_DIFFVSCLIENT), FALSE );
			EnableCtrl( GetDlgItem(IDB_DIFFREVISIONS), 
					   rev1->m_ChangeType.Find(_T("delete")) != 0 
					&& rev1->m_ChangeType.Find(_T("purge")) != 0 
					&& rev2->m_ChangeType.Find(_T("delete")) != 0
					&& rev2->m_ChangeType.Find(_T("purge")) != 0 );
			EnableCtrl( GetDlgItem(ID_CHANGE_DESCRIBE), FALSE );
			EnableCtrl( GetDlgItem(ID_FILE_RECOVER), FALSE );
			break;
		  }
		default:
			EnableCtrl(	GetDlgItem(IDB_BROWSE), FALSE );
			EnableCtrl( GetDlgItem(IDB_GET), FALSE );
			EnableCtrl( GetDlgItem(IDB_DIFFVSCLIENT), FALSE );
			EnableCtrl( GetDlgItem(IDB_DIFFREVISIONS), FALSE );
			EnableCtrl( GetDlgItem(ID_CHANGE_DESCRIBE), FALSE );
			EnableCtrl( GetDlgItem(ID_FILE_RECOVER), FALSE );
		}
	}
}

void CHistoryDlg::EnableCtrl( CWnd *ctl, BOOL enable )
{
	// Avoid flicker by only calling EnableWindow as reqd
	if(enable != ctl->IsWindowEnabled())
		ctl->EnableWindow(enable);
}

void CHistoryDlg::OnCompletehist() 
{
	OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// CHistList

CHistList::CHistList()
{
}

CHistList::~CHistList()
{
}


BEGIN_MESSAGE_MAP(CHistList, CListCtrl)
	//{{AFX_MSG_MAP(CHistList)
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_GETMINMAXINFO()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_FINDPATTERN, OnFindPattern )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHistList message handlers


void CHistList::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CListCtrl::OnLButtonDblClk(nFlags, point);
	((CHistoryDlg *)GetParent())->FileQuickbrowse(TRUE);
}

void CHistList::OnRButtonUp(UINT nFlags, CPoint local) 
{
	// find out what was hit
	LV_HITTESTINFO ht;
	ht.pt=local;
	int index= HitTest(&ht);

	if(index == -1)
		return;

	CPoint screen=local;
	ClientToScreen(&screen);
	((CHistoryDlg *)GetParent())->OnContextMenu(screen, index);
}


void CHistList::OnRButtonDown(UINT nFlags, CPoint point) 
{
	
 // Just eat the event, so a dbl click not required	

}

void CHistList::Sort( int column, BOOL ascending )
{
	SortItems( ListSort, column );
	m_headerctrl.SetSortImage(column, ascending);
}

void CHistList::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	RECT rectW, rectB;
	((CHistoryDlg *)GetParent())->GetWindowRect(&rectW);
	((CHistoryDlg *)GetParent())->GetDlgItem(IDOK)->GetWindowRect(&rectB);
	lpMMI->ptMinTrackSize.x = lpMMI->ptMaxSize.x + lpMMI->ptMaxPosition.x * 2;
	lpMMI->ptMinTrackSize.y = ((CHistoryDlg *)GetParent())->GetListTop() + 20;
	lpMMI->ptMaxTrackSize.y = (rectW.bottom - rectW.top) - (rectB.bottom - rectB.top) * 7;
	CListCtrl::OnGetMinMaxInfo(lpMMI);
}

void CHistList::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	if (bReady)
		lpwndpos->flags |= SWP_NOMOVE;
	CListCtrl::OnWindowPosChanging(lpwndpos);
}

void CHistList::OnSize(UINT nType, int cx, int cy) 
{
	CListCtrl::OnSize(nType, cx, cy);
	((CHistoryDlg *)GetParent())->RePaintMiddleWindows();
}

LRESULT CHistList::OnFindPattern(WPARAM wParam, LPARAM lParam)
{
	TCHAR str[ 1024 ];
	CString text;
	CString what = (TCHAR *)lParam;
	int flags = (int)wParam;
	int cnt = GetItemCount();
	int prv = -1;

	if (!(flags & FR_MATCHCASE))
		what.MakeLower();
	int i = -1;
	for( int k = 0;  k < cnt; k++ )
	{
		if( GetItemState( k, LVIS_SELECTED ) == LVIS_SELECTED )
		{
			SetItemState( k, 0, LVIS_SELECTED|LVIS_FOCUSED );
			prv = i = k;
		};
	}
	int j;
	if (i == -1)
		i = 0;
	int columns = GetHeaderCtrl()->GetItemCount();
	if (flags & FR_DOWN)
	{
		if (flags < 0)
			i++;
		while( i < cnt )
		{
			for (j =-1; ++j < columns; )
			{
				GetItemText( i, j, str, sizeof(str)/sizeof(TCHAR) );
				text = str;
				if (!(flags & FR_MATCHCASE))
					text.MakeLower();
				if (text.Find(what) != -1)
				{
					SetItemState( i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
					EnsureVisible(i, FALSE);
					MainFrame()->SetMessageText(LoadStringResource(IDS_FOUND));
					return 0;
				}
			}
			i++;
		}
	}
	else
	{
		if (flags < 0)
			i--;
		while( i >= 0 )
		{
			for (j =-1; ++j < columns; )
			{
				GetItemText( i, j, str, sizeof(str)/sizeof(TCHAR) );
				text = str;
				if (!(flags & FR_MATCHCASE))
					text.MakeLower();
				if (text.Find(what) != -1)
				{
					SetItemState( i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
					EnsureVisible(i, FALSE);
					MainFrame()->SetMessageText(LoadStringResource(IDS_FOUND));
					return 0;
				}
			}
			i--;
		}
	}
	MessageBeep(0);
	MainFrame()->SetMessageText(LoadStringResource(IDS_NOT_FOUND));
	if (prv != -1)
		SetItemState( prv, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
	return 0;
}

//
/////////////////////////////////////////////////////////////////////////////

void CHistoryDlg::OnUpdatePositionToPattern(CCmdUI* pCmdUI) 
{
	CString txt = LoadStringResource(IDS_FINDPATTERN);
	pCmdUI->SetText( txt );
	pCmdUI->Enable( !m_Busy && NULL == m_pFRDlg );
}

void CHistoryDlg::OnPositionToPattern() 
{
	if ( NULL == m_pFRDlg )
	{
		m_pFRDlg = new CFindReplaceDialog();  // Must be created on the heap

		m_pFRDlg->m_fr.lStructSize = sizeof(FINDREPLACE);
		m_pFRDlg->m_fr.hwndOwner = this->m_hWnd;
		m_pFRDlg->Create( TRUE, m_FindWhatStr, _T(""), m_FindWhatFlags | FR_HIDEWHOLEWORD, this ); 
	}
}

void CHistoryDlg::OnUpdatePositionToNext(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_Busy);
}

void CHistoryDlg::OnPositionToNext() 
{
	if (!m_FindWhatStr.IsEmpty())
	{
		::PostMessage(m_ListCtl.m_hWnd, WM_FINDPATTERN, 
				(WPARAM)m_FindWhatFlags | 0x80000000, (LPARAM)m_FindWhatStr.GetBuffer(0));
	}
}

void CHistoryDlg::OnUpdatePositionToPrev(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_Busy);
}

void CHistoryDlg::OnPositionToPrev() 
{
	if (!m_FindWhatStr.IsEmpty())
	{
		::PostMessage(m_ListCtl.m_hWnd, WM_FINDPATTERN, 
				((WPARAM)m_FindWhatFlags | 0x80000000) ^ FR_DOWN, 
				(LPARAM)m_FindWhatStr.GetBuffer(0));
	}
}

LRESULT CHistoryDlg::OnFindReplace(WPARAM wParam, LPARAM lParam)
{
	LPFINDREPLACE lpfp = (LPFINDREPLACE)lParam;
	if (m_pFRDlg->FindNext() || m_pFRDlg->IsTerminating())
	{
		m_FindWhatStr = lpfp->lpstrFindWhat;
		m_FindWhatFlags = lpfp->Flags;
		if (m_pFRDlg->FindNext())
		{
			::PostMessage(m_ListCtl.m_hWnd, WM_FINDPATTERN, 
				(WPARAM)(lpfp->Flags), (LPARAM)m_FindWhatStr.GetBuffer(0));
			delete m_pFRDlg;
		}
		m_pFRDlg = NULL;
	}
	return 0;
}

void CHistoryDlg::RePaintMiddleWindows()
{
	CWnd *pWin=GetDlgItem(IDC_ACTIONLABEL);
	pWin->InvalidateRect( NULL, TRUE );
	pWin=GetDlgItem(IDC_COMPLETEHIST);
	pWin->InvalidateRect( NULL, TRUE );
#ifdef	FILELOGdashH
	if (GET_SERVERLEVEL() >= 24)		// 2007.3 or later?
	{
		pWin=GetDlgItem(IDC_STATIC_1);
		pWin->InvalidateRect( NULL, TRUE );
		pWin=GetDlgItem(IDC_RADIO1);
		pWin->InvalidateRect( NULL, TRUE );
		pWin=GetDlgItem(IDC_RADIO2);
		pWin->InvalidateRect( NULL, TRUE );
		pWin=GetDlgItem(IDC_RADIO3);
		pWin->InvalidateRect( NULL, TRUE );
	}
#endif
}

void CHistoryDlg::OnSize(UINT nType, int cx, int cy) 
{
	CRect rect;

	CDialog::OnSize(nType, cx, cy);
	if (nType == SIZE_MINIMIZED)
	{
		m_IsMinimized = TRUE;
		return;
	}
	else if (m_IsMinimized)
	{
		m_IsMinimized = FALSE;
		return;
	}

	GetClientRect(&rect);
	int x=rect.Width();
	int y=rect.Height();

	// find the bottom of the buttons
	CWnd *pBtn=GetDlgItem(IDB_GET);
	if (!pBtn)
		return;
	pBtn->GetWindowRect(&rect);
	ScreenToClient(&rect);
	int bottomButtons= rect.bottom;

	// Increase the size of the list below buttons
	if (!m_listHeight)
		 m_listHeight = y*4/10;
	else if ((m_dlgHeight != y) && m_dlgHeight)
		m_listHeight = max(m_lorgHeight, 
						   (m_listHeight + (y > m_dlgHeight ? 1 : 0)) * y/m_dlgHeight);
	m_dlgHeight = y;
	CWnd *pList=GetDlgItem(IDC_REVISIONLIST);
	pList->MoveWindow(0, m_listTop = bottomButtons+4, x, m_listHeight, TRUE);
	pList->GetWindowRect(&rect);
	m_listHeight = rect.Height();

	SizeBottonOfHistory(x, y);

	InvalidateRect(NULL, TRUE);
}

void CHistoryDlg::SizeBottonOfHistory(int x, int y)
{
	int bottomList= m_listTop + m_listHeight - 1;
	CRect rect;
	int h;
	int w;

	// Position the rev file type text box
	CWnd *pText=GetDlgItem(IDC_REVFILETYPE);
	pText->GetWindowRect(&rect);
	pText->ScreenToClient(&rect);
	pText->MoveWindow(x - rect.Width() - 6, bottomList + rect.Height()/2 - 2, 
						  rect.Width(), rect.Height(), TRUE);

	// Position the action label
	CWnd *pLabel=GetDlgItem(IDC_ACTIONLABEL);
	pLabel->GetClientRect(&rect);
	pLabel->MoveWindow(4, bottomList+rect.Height()/2, rect.Width(), rect.Height(), TRUE);
	int bottomLabel= bottomList + rect.Height()*3/2;

	// Calculate some heights
	CWnd *pBtn=GetDlgItem(IDOK);
	pBtn->GetWindowRect(&rect);
	int btnWidth = rect.Width();
	int btnHeight= rect.Height();
	h = (y-bottomLabel)/2 - btnHeight - 6;

	// Position the action text box
	pText=GetDlgItem(IDC_REVISIONACTION);
	pText->MoveWindow(2, bottomLabel+4, x-4, h, TRUE);
	bottomList = bottomLabel+4 + h;

	// Position the description label
	pLabel=GetDlgItem(IDC_SUMMARYLABEL);
	pLabel->GetClientRect(&rect);
	pLabel->MoveWindow(4, bottomList+rect.Height()/2, x, rect.Height(), TRUE);
	bottomLabel= bottomList + rect.Height()*3/2;

	// Position the description text box
	pText=GetDlgItem(IDC_REVISIONINFO);
	pText->MoveWindow(2, bottomLabel+4, x-4, h, TRUE);
	int bottomDesc = bottomLabel+4 + h + 4;

	// Position the complete history checkbox
	pLabel=GetDlgItem(IDC_COMPLETEHIST);
	pLabel->GetClientRect(&rect);
	pLabel->MoveWindow(4, bottomDesc, rect.Width(), rect.Height(), TRUE);
	if (GET_SERVERLEVEL() < 8)
		pLabel->ShowWindow(SW_HIDE);

	// Position the branching history radio buttons
	pLabel=GetDlgItem(IDC_STATIC_1);
	pLabel->GetClientRect(&rect);
	pLabel->MoveWindow(4, bottomDesc, w = rect.Width(), rect.Height(), TRUE);
	if (GET_SERVERLEVEL() < 24)	// 2007.3 or later?
		pLabel->ShowWindow(SW_HIDE);

	pBtn=GetDlgItem(IDC_RADIO1);
	pBtn->GetClientRect(&rect);
	pBtn->MoveWindow(4 + w + 4, bottomDesc, rect.Width(), rect.Height(), TRUE);
	if (GET_SERVERLEVEL() < 24)	// 2007.3 or later?
		pBtn->ShowWindow(SW_HIDE);
	w += rect.Width();

	pBtn=GetDlgItem(IDC_RADIO2);
	pBtn->GetClientRect(&rect);
	pBtn->MoveWindow(4 + w + 8, bottomDesc, rect.Width(), rect.Height(), TRUE);
	if (GET_SERVERLEVEL() < 24)	// 2007.3 or later?
		pBtn->ShowWindow(SW_HIDE);
	w += rect.Width();

	pBtn=GetDlgItem(IDC_RADIO3);
	pBtn->GetClientRect(&rect);
	pBtn->MoveWindow(4 + w + 12, bottomDesc, rect.Width(), rect.Height(), TRUE);
	if (GET_SERVERLEVEL() < 24)	// 2007.3 or later?
		pBtn->ShowWindow(SW_HIDE);
	w += rect.Width();

	// Position the More, Close and Help buttons
	pBtn=GetDlgItem(IDHELP);
	pBtn->GetWindowRect(&rect);
	ScreenToClient(&rect);
	x = x - btnWidth - 5;
	pBtn->SetWindowPos(NULL, x, bottomDesc, btnWidth, rect.Height(), SWP_NOZORDER);

	pBtn=GetDlgItem(IDOK);
	pBtn->SetWindowPos(NULL, x-btnWidth-5, bottomDesc, btnWidth, rect.Height(), SWP_NOZORDER);

	pBtn=GetDlgItem(IDC_MORE);
	pBtn->SetWindowPos(NULL, x-(btnWidth*2)-10, bottomDesc, btnWidth, rect.Height(), SWP_NOZORDER);

	m_SizeSet = TRUE;
}

BOOL CHistoryDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	if ((wParam == IDC_REVISIONLIST) && m_SizeSet)
	{
		CRect rect;
		CWnd *pList=GetDlgItem(IDC_REVISIONLIST);
		pList->GetWindowRect(&rect);
		if (m_listHeight != rect.Height())
		{
			m_listHeight = rect.Height();
			GetClientRect(&rect);
			int x=rect.Width();
			int y=rect.Height();
			SizeBottonOfHistory(x, y);
		}
	}
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CHistoryDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	lpMMI->ptMinTrackSize.x= m_InitRect.Width();
	lpMMI->ptMinTrackSize.y= m_InitRect.Height();
}

void CHistList::PreSubclassWindow() 
{
	CListCtrl::PreSubclassWindow();

	// Add initialization code
	m_headerctrl.SubclassWindow( ::GetDlgItem(m_hWnd,0) );
}


void CHistoryDlg::OnOK() 
{
	UpdateData(TRUE);
#ifdef	FILELOGdashH
	m_CompleteHist = GET_SERVERLEVEL() >= 24 ? m_CompleteHistIC : m_CompleteHistI;	// 2007.3 or later?
#else
	m_CompleteHist = m_CompleteHistI;
#endif
	m_SizeSet = FALSE;
	m_WinPos.SaveWindowPosition();
	SaveColumnWidths();
	if(!GET_P4REGPTR()->SetHistListHeight( m_listHeight ) )
		AfxMessageBox( IDS_BAD_REGISTRY,  MB_ICONSTOP );
	if (m_pParent)
		m_pParent->PostMessage(WM_P4ENDHISTORY, 0, (LPARAM)this);
	CDialog::OnOK();
}

void CHistoryDlg::SaveColumnWidths() 
{
	// Save the column widths
	CString str;

	for(int i=0; i < 10; i++)
	{
		// Note that GetColumnWidth returns zero if i > numcols
    	CString num;
		num.Format(_T("%d"), m_ListCtl.GetColumnWidth(i));
		if(i)
			str+=_T(",");
		str+=num;
	}
	
    CRegKeyEx key;
    if(ERROR_SUCCESS == key.Create(HKEY_CURRENT_USER, sRegKey))
        key.SetValueString(str, sRegValue_ColumnWidths);
}

// Check the registry to see if we have recorded the column widths last
// used for this list view
void CHistoryDlg::RestoreSavedWidths(int *width, int numcols)
{
    CRegKeyEx key;
    if(ERROR_SUCCESS == key.Open(HKEY_CURRENT_USER, sRegKey, KEY_READ))
    {
        CString result = key.QueryValueString(sRegValue_ColumnWidths);
        if(!result.IsEmpty())
        {
			//		things can go wrong with the registry setting of the 
			//		widths. Use the defaults if the entry is all zeroes.
			//
			if ( result != _T("0,0,0,0,0,0,0,0,0,0") )
				for(int i=0; i< numcols; i++)
					width[i]= GetPositiveNumber(result);
        }
    }
}

void CHistoryDlg::OnHelp() 
{
	GotoDlgCtrl(GetDlgItem(IDC_REVISIONLIST));
	AfxGetApp()->WinHelp(TASK_DISPLAYING_REVISION_HISTORY);
}

BOOL CHistoryDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return(FALSE);
}

void CHistoryDlg::OnMore() 
{
	m_More = m_Rerun = TRUE;
	OnOK();
}

void CHistoryDlg::OnPositionDepot()
{
	if(m_ListCtl.GetSelectedCount() != 1)
		{ ASSERT(0); return; }

	for(int i= 0; i < m_ListCtl.GetItemCount(); i++)
	{
		if(m_ListCtl.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			CRevision *rev= (CRevision *) m_ListCtl.GetItemData(i);
			CString itemStr = rev->m_FName;
			if ((i = itemStr.Find(_T('#'))) != -1)
				itemStr = itemStr.Left( i );  // trim off rev# info
			else if ((i = itemStr.Find(_T("/..."))) != -1)
				itemStr = itemStr.Left( i );  // trim off "/..."
			((CMainFrame *) AfxGetMainWnd())->ExpandDepotString( itemStr, TRUE );
			break;
		}
	}
}

void CHistoryDlg::OnCopyRows() 
{
	CString txt;
	if(!m_ListCtl.GetSelectedCount())
		{ ASSERT(0); return; }

	CRevision *rev;

	// Get the rev
	for(int i= 0; i < m_ListCtl.GetItemCount(); i++)
	{
		if(m_ListCtl.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			rev= (CRevision *) m_ListCtl.GetItemData(i);
			CString item;
			item.Format(_T("%d\t%s\t%d\t%s\t%s\t%s\t%s\r\n"), rev->m_RevisionNum,
							rev->m_FName, rev->m_ChangeNum,  rev->m_Date,
							rev->m_User,  rev->m_ChangeType, PadCRs(rev->m_ChangeDescription));
			txt += item;
		}
	}

	CopyTextToClipboard(txt);
}

void CHistoryDlg::OnKeydownRevisionlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	if ((pLVKeyDow->wVKey == _T('C')) && ( ::GetKeyState(VK_CONTROL) & 0x8000 ))
		OnCopyRows();
	else if ((pLVKeyDow->wVKey == _T('F')) && ( ::GetKeyState(VK_CONTROL) & 0x8000 ))
		OnPositionToPattern();
	else if ((pLVKeyDow->wVKey == VK_F3) && ( !(::GetKeyState(VK_SHIFT) & 0x8000 )))
		OnPositionToNext();
	else if ((pLVKeyDow->wVKey == VK_F3) && ( ::GetKeyState(VK_SHIFT) & 0x8000 ))
		OnPositionToPrev();
	else if ((pLVKeyDow->wVKey == _T('T')) && ( ::GetKeyState(VK_CONTROL) & 0x8000 ))
		OnPerforceOptions();
	*pResult = 0;
}

void CHistoryDlg::OnPerforceOptions()
{
	if (m_Key)
	{
		MessageBeep(0);
		return;
	}

	BOOL allState = GET_P4REGPTR()->GetFetchAllHist();
	int  histCount = GET_P4REGPTR()->GetFetchHistCount();
	BOOL enableRevHistShowIntegs = GET_P4REGPTR()->GetEnableRevHistShowIntegs( );

	MainFrame()->OnPerforceOptions(TRUE, FALSE, 0, IDS_PAGE_REVISIONHISTORY);

	if (enableRevHistShowIntegs != GET_P4REGPTR()->GetEnableRevHistShowIntegs( )
	 || histCount != GET_P4REGPTR()->GetFetchHistCount()
	 || allState != GET_P4REGPTR()->GetFetchAllHist())
	{
		m_Rerun = TRUE;
		OnOK();
	}
}

// This signals the closing of a modeless dialog
// to MainFrame which will delete the 'this' object
void CHistoryDlg::OnDestroy()
{
	if (m_pParent)
		::PostMessage(MainFrame()->m_hWnd, WM_P4DLGDESTROY, 0, (LPARAM)this);
}

void CHistoryDlg::OnSysCommand(UINT nID, LPARAM lParam) 
{
	switch(nID)
	{
	case SC_MINIMIZE:
		GetDesktopWindow()->ArrangeIconicWindows();
		break;
	}

	CDialog::OnSysCommand(nID, lParam);
}
