//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// OldChgListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "Cmd_Changes.h"
#include "Cmd_EditSpec.h"
#include "Cmd_MaxChange.h"
#include "OldChgView.h"
#include "SpecDescDlg.h"
#include "MainFrm.h"
#include "JobListDlg.h"
#include "DeleteFixes.h"
#include "P4Fix.h"
#include "ChgDescribe.h"
#include "Cmd_Fix.h"
#include "cmd_fixes.h"
#include "cmd_get.h"
#include "cmd_jobs.h"
#include "cmd_where.h"
#include "RegKeyEx.h"
#include "newclientdlg.h"
#include "OldChgFilterDlg.h"
#include "ImageList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IMG_INDEX(x) (x-IDB_PERFORCE)

#define NUMFORMAT _T("%05d")

static LPCTSTR sRegValue_SubmittedChanges = _T("Submitted Changes");

enum OldChgSubItem
{
	OLDCHG_NAME,
	OLDCHG_DATE,
	OLDCHG_USER,
	OLDCHG_DESC,
	OLDCHG_MAXCOL
};

/////////////////////////////////////////////////////////////////////////////
// COldChgListCtrl

IMPLEMENT_DYNCREATE(COldChgListCtrl, CP4ListCtrl)

BEGIN_MESSAGE_MAP(COldChgListCtrl, CP4ListCtrl)
	ON_UPDATE_COMMAND_UI(ID_VIEW_UPDATE_RIGHT, OnUpdateViewUpdate)
	ON_UPDATE_COMMAND_UI(ID_CHANGE_DESCRIBE, OnUpdateDescribe)
	ON_COMMAND(ID_CHANGE_DESCRIBE, OnDescribe)
	ON_UPDATE_COMMAND_UI(ID_SYNC_CHANGE, OnUpdateSyncChg)
	ON_COMMAND(ID_SYNC_CHANGE, OnSyncChg)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_VIEW_RELOADALL, OnViewReloadall)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RELOADALL, OnUpdateViewReloadall)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_FILTER_SETVIEW, OnFilterSetview)
	ON_COMMAND(ID_FILTER_SETVIEW_DROP, OnFilterSetviewDrop)
	ON_UPDATE_COMMAND_UI(ID_FILTER_SETVIEW, OnUpdateFilterSetview)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_FILTER_CLEARVIEW, OnUpdateFilterClearview)
	ON_COMMAND(ID_FILTER_CLEARVIEW, OnFilterClearview)
	ON_UPDATE_COMMAND_UI(ID_CHANGE_EDSPEC, OnUpdateChangeEdspec)
	ON_COMMAND(ID_CHANGE_EDSPEC, OnChangeEdspec)
	ON_UPDATE_COMMAND_UI(ID_CHANGE_ADDJOBFIX, OnUpdateAddjobfix)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_COMMAND(ID_VIEW_UPDATE_RIGHT, OnViewReloadall)
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnDeleteitem)
	ON_COMMAND(ID_CHANGE_ADDJOBFIX, OnAddjobfix)
	ON_COMMAND(ID_FILE_INTEGRATE, OnFileIntegrate)
	ON_UPDATE_COMMAND_UI(ID_FILE_INTEGRATE, OnUpdateFileIntegrate)
	ON_COMMAND(ID_FILE_INTEGSPEC, OnFileIntegspec)
	ON_UPDATE_COMMAND_UI(ID_FILE_INTEGSPEC, OnUpdateFileIntegrate)
	ON_COMMAND(ID_PERFORCE_OPTIONS, OnPerforceOptions)
	ON_UPDATE_COMMAND_UI(ID_POSITIONDEPOT, OnUpdatePositionDepot)
	ON_COMMAND(ID_POSITIONDEPOT, OnPositionDepot)
	ON_UPDATE_COMMAND_UI(ID_CHANGE_REMOVEFIX, OnUpdateRemovefix)
	ON_COMMAND(ID_CHANGE_REMOVEFIX, OnRemovefix)
	ON_MESSAGE(WM_P4FIXES, OnP4Fixes )
	ON_MESSAGE(WM_P4FIX, OnP4Fix )
	ON_MESSAGE(WM_ONDODELETEFIXES, OnDoDeleteFixes )
	ON_MESSAGE(WM_RUNUPDATE, OnUpdateRequest )
	ON_MESSAGE(WM_P4CHANGES, OnP4Change)
	ON_MESSAGE(WM_P4DESCRIBE, OnP4Describe )
    ON_MESSAGE(WM_P4ENDDESCRIBE, OnP4EndDescribe )
	ON_MESSAGE(WM_P4MAXCHANGE, OnP4MaxChange )
	ON_MESSAGE(WM_P4EDITSPEC, OnP4ChangeSpec )
	ON_MESSAGE(WM_P4ENDSPECEDIT, OnP4EndSpecEdit )
    ON_MESSAGE(WM_P4JOBS, OnP4JobList )
    ON_MESSAGE(WM_P4GET, OnP4Get )
	ON_MESSAGE(WM_DOCUSTOMGET, OnDoSyncChg )
	ON_MESSAGE(WM_P4INTEGCHG, OnIntegChg )
	ON_MESSAGE(WM_SUBCHGOUFC, CallOnUpdateFilterClearview )
	ON_MESSAGE(WM_P4CHGROLLBACK, OnP4ChgRollback )
	ON_MESSAGE(WM_P4CHGROLLBACKPREVIEW, OnP4ChgRollbackPreview )
	ON_MESSAGE(WM_P4DESCRIBEALT, OnP4ChgSelectAll )
END_MESSAGE_MAP()

COldChgListCtrl::COldChgListCtrl()
{
    m_viewType = P4CHANGE_SPEC;
	m_MaxChange=0;
	m_ItemCount=0;
	m_LastUpdateTime=0;
	m_SortAscending=FALSE;
	m_FilterInteg = m_FilterSpecial = m_ForceFocusHere = FALSE;
	m_FilteredByUser = GET_P4REGPTR()->GetFilteredByUser();
	m_UserFilter = GET_P4REGPTR()->GetUserFilter();
	m_FilteredByClient = GET_P4REGPTR()->GetFilteredByClient();
	m_ClientFilter = GET_P4REGPTR()->GetClientFilter();
	m_LastSortCol=0;
	m_LastDescNbr=_T("");
	m_captionplain = LoadStringResource(IDS_SUBMITTED_PERFORCE_CHANGELISTS);
	m_DeleteFixesDlg = 0;

	m_CF_DEPOT = RegisterClipboardFormat(LoadStringResource(IDS_DRAGFROMDEPOT));
	m_CF_CLIENT= RegisterClipboardFormat(LoadStringResource(IDS_DRAGFROMCLIENT));
	m_CF_USER  = RegisterClipboardFormat(LoadStringResource(IDS_DRAGFROMUSER));
	m_CF_JOB   = RegisterClipboardFormat(LoadStringResource(IDS_DRAGFROMJOB));
}

COldChgListCtrl::~COldChgListCtrl()
{
}

/////////////////////////////////////////////////////////////////////////////
// COldChgListCtrl diagnostics

#ifdef _DEBUG
void COldChgListCtrl::AssertValid() const
{
	CP4ListCtrl::AssertValid();
}

void COldChgListCtrl::Dump(CDumpContext& dc) const
{
	CP4ListCtrl::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COldChgListCtrl message handlers


void COldChgListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// Find out what item was clicked and then run describe
	CPoint local = point;
	
	// find out what was hit
	LV_HITTESTINFO ht;
	ht.pt=local;
	ht.flags=LVHT_ONITEMICON | LVHT_ONITEMLABEL;
	int index=HitTest( &ht	);

	if(index != -1)
	{
		// If on an item, so select it and run edit
		SetItemState( index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
		OnDescribe();
	}
	else
		CP4ListCtrl::OnLButtonDblClk(nFlags, point);}


void COldChgListCtrl::Clear()
{
	// Clear the list view
    SetRedraw(FALSE);
	DeleteAllItems();
    SetRedraw(TRUE);
	m_ItemCount=0;
	m_MaxChange=0;
	m_LastUpdateTime=0;
	CP4ListCtrl::Clear();
}


void COldChgListCtrl::ClearFilter()
{
	Clear();
	GET_P4REGPTR()->SetFilteredByUser(FALSE);
	GET_P4REGPTR()->SetFilteredByClient(FALSE);
	m_FilterInteg = m_FilterSpecial = m_FilteredByUser = m_FilteredByClient = FALSE;
	m_FilterView.RemoveAll(); 
	SetCaption();
	PersistentChgFilter( KEY_WRITE );
}

void COldChgListCtrl::InsertChange(CP4Change *change, int index)
{
	// Add the data
	LV_ITEM lvItem;
	int iActualItem = -1;
	CString txt, txtout;
	int i;

	ASSERT(change != NULL);
	m_iImage = CP4ViewImageList::VI_OLDCHANGE;

	if (GET_SERVERLEVEL() < 12)
	{
		// Are we filtering by user on an old server?
		// (on a new server we don't have anything to do 
		//  because p4 changes -u did it for us)
		if (m_FilteredByUser)
		{
			txt = const_cast<LPTSTR>((LPCTSTR)change->GetUser());
			if ((i = txt.Find(_T('@'))) != -1)
				txt = txt.Left(i);
			// is this the user we want?
			if (txt != m_UserFilter)
			{
				// no - so we must delete the change because no one 
				// will know about it now that it is not in the list
				delete change;
				return;
			}
		}
		// Are we filtering by client on an old server?
		// (on a new server we don't have anything to do 
		//  because p4 changes -u did it for us)
		if (m_FilteredByClient)
		{
			txt = const_cast<LPTSTR>((LPCTSTR)change->GetUser());
			if ((i = txt.Find(_T('@'))) != -1)
				txt = txt.Mid(i+1);
			// is this the client we want?
			if (txt != m_ClientFilter)
			{
				// no - so we must delete the change because no one 
				// will know about it now that it is not in the list
				delete change;
				return;
			}
		}
	}
	
	for(int subItem=0; subItem < 4; subItem++)
	{
		lvItem.mask=LVIF_TEXT | 
					((subItem==0) ? LVIF_IMAGE : 0) |
					((subItem==0) ? LVIF_PARAM : 0);

		lvItem.iItem= (subItem==0) ? index : iActualItem;
        ASSERT(lvItem.iItem != -1);
		lvItem.iSubItem= subItem;
		lvItem.iImage = CP4ViewImageList::VI_OLDCHANGE;
		lvItem.lParam=(LPARAM) change;

		switch(subItem)
		{
		case 0: 
			txt.Format(NUMFORMAT, change->GetChangeNumber());
			lvItem.pszText= const_cast<LPTSTR>((LPCTSTR)txt); break;
		case 1: 
			lvItem.pszText= const_cast<LPTSTR>((LPCTSTR)change->GetChangeDate()); break;
		case 2: 
			lvItem.pszText= const_cast<LPTSTR>((LPCTSTR)change->GetUser()); break;
		case 3: 
			txt= change->GetDescription(); 
			// Dont display cr-lf and tab chars
			int len=txt.GetLength();
			LPTSTR ptr=txt.GetBuffer(len);
			LPTSTR ptrout=txtout.GetBuffer(len);
			for(int i=0; i<len; i++)
			{
				if(*ptr==_T('\r'))
				{
					*ptrout=_T(' ');
					ptrout++;
				}
				else if(*ptr!=_T('\n') && *ptr!=_T('\t'))
				{
					*ptrout=*ptr;
					ptrout++;
				}
				ptr++;
			}
			*ptrout=_T('\0');
			txtout.ReleaseBuffer();
			lvItem.pszText=const_cast<LPTSTR>((LPCTSTR)txtout);
			break;
		}
			
		if(subItem==0)
			iActualItem=InsertItem(&lvItem);
		else
			SetItem(&lvItem);
		
	}
}


LRESULT COldChgListCtrl::OnP4Change(WPARAM wParam, LPARAM lParam)
{
	// Note:  Output of 'P4 changes' arrives highest change number to
	//        lowest change number, so use m_NewMaxChange as a temp value
	//        and only update m_MaxChange at the end of the operation.
	if (lParam)
	{
		// Just got a list of changes
		CObList *list= (CObList *) wParam;
		ASSERT_KINDOF(CObList, list);

		POSITION pos= list->GetHeadPosition();
        SetRedraw(FALSE);
		while(pos != NULL)
		{
			CP4Change *change= (CP4Change *) list->GetNext(pos);
			ASSERT_KINDOF(CP4Change, change);

			if(change->GetChangeNumber() > m_MaxChange)
			{
				// Note: Do not delete change if inserted in list, because  
				//       DeleteItem() will get rid of the change later.
				if(change->GetChangeNumber() > m_NewMaxChange)
					m_NewMaxChange=change->GetChangeNumber();
				InsertChange(change, m_ItemCount);
				m_ItemCount++;
			}
			else
				delete change;
			
		}
        SetRedraw(TRUE);
		delete list;
	}
	else
	{
		CCmd_Changes *pCmd= (CCmd_Changes *) wParam;
		ASSERT_KINDOF(CCmd_Changes, pCmd);

		if(!pCmd->GetError())
		{
			CP4ListCtrl::SetUpdateDone();
			m_LastUpdateTime=GetTickCount();
			if (!m_ItemCount)
				m_UpdateState = LIST_CLEAR;
		}
		else
		{
			CP4ListCtrl::SetUpdateFailed();
			m_ForceFocusHere = FALSE;
		}

        // Record the new max change
		if (m_NewMaxChange > m_MaxChange)
			m_MaxChange = m_NewMaxChange;

		// Sort the view
		ReSort();
		
		if( m_ItemCount > 0)
		{
			int i = FindInList(m_Active);
			if (i < 0)	i=0;
			SetItemState( i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
			EnsureVisible(i, FALSE);
		}

		CString msg;
		msg.FormatMessage( IDS_NUMBER_OF_SUBMITTED_CHANGELISTS_n, m_ItemCount );
		AddToStatus( msg, SV_DEBUG );

		if( pCmd->HaveServerLock() )
            pCmd->ReleaseServerLock();
		delete pCmd;

		MainFrame()->ClearStatus();

		// Notify the mainframe that we have finished getting the submitted changlists,
		// hence the entire set of port connection async command have finished.
		MainFrame()->FinishedGettingChgs(FALSE);

		// and we are done - must explicitly call this in case filter yielded 0 chglists
		CP4ListCtrl::SetUpdateDone();

		// if we did this update to fill a Browse listbox, call back to fill the listbox.
		if (m_PostViewUpdateMsg)
		{
			PostMessage(m_PostViewUpdateMsg, m_PostViewUpdateWParam, m_PostViewUpdateLParam);
			m_PostViewUpdateMsg = 0;
		}

		// if we just finished processing a Drop, force the focus to this pane
		if (m_ForceFocusHere)
		{
			m_ForceFocusHere = FALSE;
			MainFrame()->OnViewSubmitted();
		}
	}

	return 0;
}


LRESULT COldChgListCtrl::OnP4Describe(WPARAM wParam, LPARAM lParam)
{
	CCmd_Describe *pCmd= (CCmd_Describe *) wParam;

	if(!pCmd->GetError())
	{
		CString desc= MakeCRs(pCmd->GetDescription());
		
		int key;
		CSpecDescDlg *dlg = new CSpecDescDlg(this);
		dlg->SetIsModeless(TRUE);
		dlg->SetKey(key = pCmd->HaveServerLock()? pCmd->GetServerKey() : 0);
		dlg->SetDescription(LPCTSTR(desc));
		dlg->SetItemName( pCmd->GetReference() );
		dlg->SetCaption(LoadStringResource(IDS_PERFORCE_CHANGELIST_DESCRIPTION));
		dlg->SetShowNextPrev(m_ItemCount ? TRUE : FALSE);
		dlg->SetShowShowDiffs(TRUE);
		dlg->SetDiffFlag(pCmd->GetFlag());
		dlg->SetShowEditBtn(!key && !m_EditInProgress ? TRUE : FALSE);
		dlg->SetViewType(P4CHANGE_SPEC);
		if (!dlg->Create(IDD_SPECDESC, this))	// display the description dialog box
		{
			EnumChildWindows(AfxGetMainWnd()->m_hWnd, ChildSetRedraw, TRUE);
			dlg->DestroyWindow();	// some error! clean up
			delete dlg;
		}
	}
	else	// had an error - need to turn painting back on
	{
		EnumChildWindows(AfxGetMainWnd()->m_hWnd, ChildSetRedraw, TRUE);
	}
	
	delete pCmd;
	MainFrame()->ClearStatus();
	return 0;
}

LRESULT COldChgListCtrl::OnP4EndDescribe( WPARAM wParam, LPARAM lParam )
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
		OnDescribeLong(l, wParam);
		break;
	}
	case IDC_NEXTITEM:
	case IDC_PREVITEM:
	{
		long l = _ttol(ref);
		ref.Format(_T("%05d"), l);
		if (SetToNextPrevItem(ref, wParam == IDC_NEXTITEM ? 1 : -1, this))
		{
			OnDescribe();	// display the next/prev in the list on the screen
			break;
		}
	}
	case IDC_EDITIT:
		if (wParam == IDC_EDITIT)	// note fall-thru from above!
		{
			CString name = ref;
			BOOL uFlag = FALSE;
			if (GET_SERVERLEVEL() >= 23)	// 2007.2 or later?
			{
				int i;
				CString desc = dlg->GetDescription();
				if ((i = desc.Find(_T('@'))) != -1)
				{
					CString str = desc.Left(i);
					if ((i = str.ReverseFind(_T(' '))) != -1)
					{
						CString user = str.Mid(i+1);
						uFlag = !Compare(user, GET_P4REGPTR()->GetP4User());
					}
				}
			}
			EditTheSpec(&name, uFlag);
		}
	default:	// clicked OK, pressed ESC or ENTER - need to turn painting back on
		EnumChildWindows(AfxGetMainWnd()->m_hWnd, ChildSetRedraw, TRUE);
		break;
	}
	dlg->DestroyWindow();
	return TRUE;
}

void COldChgListCtrl::OnUpdateViewUpdate(CCmdUI* pCmdUI) 
{
//	pCmdUI->SetText ( SetTextToRefresh( ) );
	pCmdUI->Enable(!SERVER_BUSY() && !MainFrame()->IsModlessUp());
}

void COldChgListCtrl::OnUpdateViewReloadall(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY());
}


LRESULT COldChgListCtrl::OnUpdateRequest(WPARAM wParam, LPARAM lParam)
{
	OnViewUpdate();
	return 0;
}

void COldChgListCtrl::OnViewReloadall() 
{
	m_Active = GetSelectedItemText();
	m_MaxChange=0;
	Clear();
	OnViewUpdate();
}

void COldChgListCtrl::OnViewUpdate() 
{
	MainFrame()->SetOldChgUpdateTime(GetTickCount());
	CString str = GetSelectedItemText();
	if (!str.IsEmpty())
		m_Active = str;

	// For a full refresh, proceed to GetChanges
	if(m_MaxChange == 0)
    {
        int numChanges;
        if( GET_P4REGPTR()->GetFetchAllChanges() )
            numChanges= 0;
        else
            numChanges= GET_P4REGPTR()->GetFetchChangeCount();
	
		GetChanges(numChanges);
    }
	else
	{
		// If it has been over 20 minutes since we updated, run MaxChange
		long time=GetTickCount();
		long elapsedTime= time-m_LastUpdateTime;
		if(elapsedTime < 0 || elapsedTime > 1200000L)
		{
			CCmd_MaxChange *pCmd= new CCmd_MaxChange;
			pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK);
			if( pCmd->Run() )
			{
				MainFrame()->UpdateStatus(LoadStringResource(IDS_VERIFYING_HEAD_CHANGELIST));
			}
			else
			{
				delete pCmd;
				MainFrame()->ClearStatus();
			}
		}
		else
		{
			// Make a conservative estimate of changes submitted since last update
			int numchanges=20 + 10 * (elapsedTime/60000);
            if( !GET_P4REGPTR()->GetFetchAllChanges() &&
                numchanges > GET_P4REGPTR()->GetFetchChangeCount() )
            {
                numchanges = GET_P4REGPTR()->GetFetchChangeCount();
                Clear();
            }
            GetChanges(numchanges);
		}
	}
}

LRESULT COldChgListCtrl::OnP4MaxChange(WPARAM wParam, LPARAM lParam)
{
	CCmd_MaxChange *pCmd= (CCmd_MaxChange *) wParam;
	if(pCmd->GetError() || MainFrame()->IsQuitting())
	{
        pCmd->ReleaseServerLock();
		delete pCmd;
		MainFrame()->ClearStatus();
		return 0;
	}

    int key= pCmd->GetServerKey();

	// Get the new max change, for use if the update is a success
	long headChange= pCmd->GetMaxChange();
	
	if(headChange == m_MaxChange)
	{
		// No work to do
		pCmd->ReleaseServerLock();
		MainFrame()->ClearStatus();
	}
	else if(headChange < m_MaxChange)
	{
		// How did max change number go down??
		ASSERT(0);  
		Clear();
        int numChanges;
        if( GET_P4REGPTR()->GetFetchAllChanges() )
            numChanges= 0;
        else
            numChanges= GET_P4REGPTR()->GetFetchChangeCount();
	
		GetChanges(numChanges, key);
	}
	else
	{
		// Add 10 changes worth of "padding" in case a change is submitted between 
		// the MaxChange and Changes commands
		GetChanges(10 + headChange-m_MaxChange, key);
	}
    delete pCmd;
	return 0;
}

void COldChgListCtrl::GetChanges(long numToFetch, int key/*=0*/)
{	
	m_NewMaxChange=0;
	CCmd_Changes *pCmd= new CCmd_Changes;
    if( key==0 )
	    pCmd->Init( m_hWnd, RUN_ASYNC);
    else
        pCmd->Init( m_hWnd, RUN_ASYNC, LOSE_LOCK, key);

	// Make a copy of the filter view, because CCmdChanges will
	// destroy that copy
	POSITION pos=m_FilterView.GetHeadPosition();
	m_StrList.RemoveAll();
	while(pos != NULL)
		m_StrList.AddTail(m_FilterView.GetNext(pos));

	if( pCmd->Run(SUBMITTED_CHANGES, 
		(GET_SERVERLEVEL() >= 19 && GET_P4REGPTR()->GetUseShortSubmittedDesc()) ? 2 : 1, 
		&m_StrList, numToFetch, m_FilterInteg, 
		!m_FilteredByUser || m_UserFilter.IsEmpty() ? NULL : &m_UserFilter, 
		!m_FilteredByClient || m_ClientFilter.IsEmpty() ? NULL : &m_ClientFilter) )
	{
		MainFrame()->UpdateStatus(LoadStringResource(IDS_REQUESTING_CHANGES));
		CP4ListCtrl::OnViewUpdate();
	}
	else
	{
        if(pCmd->HaveServerLock())
            pCmd->ReleaseServerLock();
		delete pCmd;
		MainFrame()->ClearStatus();
	}
}

void COldChgListCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// make sure window is active
	GetParentFrame()->ActivateFrame();

	int	index;
    SetIndexAndPoint( index, point );

	// If on an item, make sure it's selected
	if(index != -1)
		SetItemState( index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
	
	///////////////////////////////
	// See ContextMenuRules.txt for order of menu commands!

	// create an empty context menu
	CP4Menu popMenu;
	popMenu.CreatePopupMenu();

	CP4Menu integMenu;
	integMenu.CreatePopupMenu();

	if(index != -1)
	{
		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_CHANGE_EDSPEC, LoadStringResource( IDS_EDIT ) );
		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_CHANGE_DESCRIBE );
		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_CHANGE_ADDJOBFIX, LoadStringResource( IDS_ADDJOBFIX ) );
		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_CHANGE_REMOVEFIX, LoadStringResource( IDS_CHANGE_REMOVEFIX ) );	
		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_SYNC_CHANGE, LoadStringResource( IDS_SYNC_CHANGE ) );
		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_POSITIONDEPOT, LoadStringResource( IDS_FINDCHGFILESINDEPOT ) );
		integMenu.AppendMenu( stringsON, ID_FILE_INTEGSPEC, LoadStringResource( IDS_USINGFILESPEC ) );
		integMenu.AppendMenu( stringsON, ID_FILE_INTEGRATE, LoadStringResource( IDS_USINGBRANCHSPEC ) );
		popMenu.AppendMenu(MF_POPUP, (UINT) integMenu.GetSafeHmenu(), LoadStringResource( IDS_INTEGRATE ) );
		popMenu.AppendMenu(MF_SEPARATOR);
	}

	popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_FILTER_SETVIEW, LoadStringResource(IDS_FILTER_SETVIEW));
	UINT flags = (m_FilterView.GetCount() || m_FilteredByClient || m_FilteredByUser) 
		? MF_ENABLED | MF_STRING : MF_DISABLED | MF_STRING;
	popMenu.AppendMenu(flags, ID_FILTER_CLEARVIEW, LoadStringResource(IDS_CLEARFILTER));

	popMenu.AppendMenu( MF_SEPARATOR );
	popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_VIEW_UPDATE, LoadStringResource(IDS_REFRESH));

	MainFrame()->AddToolsToContextMenu(&popMenu);

	// Finally blast the menu onto the screen
	popMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,	point.x, point.y, AfxGetMainWnd());
}


void COldChgListCtrl::OnUpdateSyncChg(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( OnUpdateShowMenuItem( pCmdUI, IDS_SYNC_CHANGE_s ) 
						&& !MainFrame()->IsModlessUp() );	
}

void COldChgListCtrl::OnSyncChg() 
{
	if(!GetSelectedChange())
		return;

	m_SyncChangeDlg = new CSyncChange(this);
	if (!m_SyncChangeDlg)
	{
		ASSERT(0);
		AfxMessageBox(IDS_COULD_NOT_CREATE_CUSTOM_SYNC_DIALOG_BOX, MB_ICONSTOP);
		return;
	}

	MainFrame()->SetModelessUp(TRUE);
	if (!m_SyncChangeDlg->Create(IDD_CHANGE_SYNC, this))
	{
		delete m_SyncChangeDlg;
		MainFrame()->SetModelessUp(FALSE);
	}
}

LRESULT COldChgListCtrl::OnDoSyncChg(WPARAM wParam, LPARAM lParam)
{
	BOOL preview=FALSE;
	long changeNumber= GetSelectedChange();

	switch(wParam)
	{
	case IDCANCEL:
		break;
	case IDGETPREVIEW:
		preview=TRUE;
		// fall thru
	case IDGET:
		if (changeNumber)
		{
			CString syncTxt;
			if (lParam == 2)
			{
				// They want to roll back the files in this chg, so get the chg description
				CString changeTxt;
				changeTxt.Format(_T("%ld"), changeNumber);
				CCmd_Describe *pCmd= new CCmd_Describe;
				pCmd->Init( m_hWnd, RUN_ASYNC);
				pCmd->SetAlternateReplyMsg(preview ? WM_P4CHGROLLBACKPREVIEW : WM_P4CHGROLLBACK);
				if( pCmd->Run( P4DESCRIBE, changeTxt) )
					MainFrame()->UpdateStatus(LoadStringResource(IDS_FETCHING_CHANGELIST_DESCRIPTION));
				else
					delete pCmd;
			}
			else if (lParam)
			{
				if (GET_SERVERLEVEL() >= 17)
					syncTxt.Format(_T("@=%ld"), changeNumber);
				else
					syncTxt.Format(_T("@%ld,%ld"), changeNumber,changeNumber);
			}
			else
				syncTxt.Format(_T("@%ld"), changeNumber);
			m_StrList.RemoveAll();
			m_StrList.AddHead(syncTxt);
			CCmd_Get *pCmd= new CCmd_Get;
			pCmd->Init( m_hWnd, RUN_ASYNC);
			if( pCmd->Run( &m_StrList, preview ) )
				MainFrame()->UpdateStatus( LoadStringResource(IDS_FILE_SYNC) );
			else
				delete pCmd;
			break;
		}
	default:
		ASSERT(0);
		break;
	}
	if (m_SyncChangeDlg && (wParam != IDGETPREVIEW))
	{
		m_SyncChangeDlg->DestroyWindow();
		delete m_SyncChangeDlg;
		MainFrame()->SetModelessUp(FALSE);
	}
	return 0;
}
	
void COldChgListCtrl::OnUpdateDescribe(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( OnUpdateShowMenuItem( pCmdUI, IDS_DESCRIBESUBMITTED_s ) );	
}

LRESULT COldChgListCtrl::OnP4Get(WPARAM wParam, LPARAM lParam)
{
	MainFrame()->ClearStatus();
	::SendMessage(m_depotWnd, WM_P4GET, wParam, lParam);
	return 0;
}


void COldChgListCtrl::OnDescribeLong(long changeNumber /*= 0*/, int flag  /*= 0*/) 
{
	if (!changeNumber)
		changeNumber= GetSelectedChange();
	
	if(changeNumber != 0)
	{
		CString changeTxt;
		changeTxt.Format(_T("%ld"), changeNumber);

		CCmd_Describe *pCmd= new CCmd_Describe;
		pCmd->Init( m_hWnd, RUN_ASYNC);
		if( pCmd->Run( P4DESCRIBELONG, changeTxt, NULL, FALSE, flag) )
			MainFrame()->UpdateStatus(LoadStringResource(IDS_FETCHING_CHANGELIST_DESCRIPTION));
		else
			delete pCmd;
	}
	else
		AfxMessageBox ( IDS_PLEASE_SELECT_A_CHANGELIST );
}

void COldChgListCtrl::OnDescribe() 
{
	long changeNumber= GetSelectedChange();
	
	if(changeNumber != 0)
	{
		CString changeTxt;
		changeTxt.Format(_T("%ld"), changeNumber);

		CCmd_Describe *pCmd= new CCmd_Describe;
		pCmd->Init( m_hWnd, RUN_ASYNC);
		if( pCmd->Run( P4DESCRIBE, changeTxt) )
			MainFrame()->UpdateStatus(LoadStringResource(IDS_FETCHING_CHANGELIST_DESCRIPTION));
		else
			delete pCmd;
	}
	else
		AfxMessageBox ( IDS_PLEASE_SELECT_A_CHANGELIST );
}

void COldChgListCtrl::OnDescribeChg() 
{
	//		let user type in the changelist number. if it's blank the user bailed.
	//
	CChgDescribe dlg;
	dlg.SetNbr(m_LastDescNbr);
	if( dlg.DoModal( ) == IDCANCEL )
		return;

	CString nbr = dlg.GetNbr( ) ;
	if ( nbr.IsEmpty( ) )
		return;

	m_LastDescNbr = nbr;
	CCmd_Describe *pCmd= new CCmd_Describe;
	pCmd->Init( m_hWnd, RUN_ASYNC);
	if( pCmd->Run( P4DESCRIBE, nbr) )
		MainFrame()->UpdateStatus(LoadStringResource(IDS_FETCHING_CHANGELIST_DESCRIPTION));
	else
		delete pCmd;
}

void COldChgListCtrl::OnUpdateChangeEdspec(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_EditInProgress);
}

void COldChgListCtrl::OnChangeEdspec() 
{
	BOOL uFlag = FALSE;
	if (GET_SERVERLEVEL() >= 23)	// 2007.2 or later?
	{
		CString usercli;
		GetUserClientForSelectedChg(&usercli);
		int i;
		if ((i = usercli.Find(_T('@'))) != -1)
		{
			CString user = usercli.Left(i);
			uFlag = !Compare(user, GET_P4REGPTR()->GetP4User());
		}
	}
	EditChangeSpec(GetSelectedChange(), uFlag);
}

void COldChgListCtrl::EditChangeSpec(long changeNumber, BOOL uFlag) 
{
	if (m_EditInProgress)
	{
		CantEditRightNow(IDS_CHANGELIST);
		return;
	}

	if(changeNumber != 0)
	{
		// Fire up CCmd_EditSpec. The spec edit dialog will be invoked by CCmd_EditSpec.
		MainFrame()->UpdateStatus( LoadStringResource(IDS_RETRIEVING_CHANGELIST_SPEC) );
		CCmd_EditSpec *pCmd= new CCmd_EditSpec;
		pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK );
		if( pCmd->Run( changeNumber, FALSE, !uFlag, FALSE, FALSE, uFlag) )
			MainFrame()->UpdateStatus( LoadStringResource(IDS_EDITING_CHANGELIST_SPEC) );	
		else
			delete pCmd;
	}
}

void COldChgListCtrl::EditTheSpec(CString *name, BOOL uFlag)
{
	EditChangeSpec(_ttol(*name), uFlag);
}

long COldChgListCtrl::GetSelectedChange()
{
	int index = CP4ListCtrl::GetSelectedItem();

	if (index==-1)
		return 0;
	else
	{
		TCHAR str[255];
		GetItemText(index, 0, str, 254);
		return _ttol(str);
	}
}

BOOL COldChgListCtrl::GetUserClientForSelectedChg(CString *user)
{
	int index = CP4ListCtrl::GetSelectedItem();

	if (index==-1)
		return FALSE;
	TCHAR str[255];
	GetItemText(index, 2, str, 254);
	*user = str;
	return TRUE;
}


// A change description was sent to the server, 'P4 change -i".  This should be a confirming
// message. 
LRESULT COldChgListCtrl::OnP4ChangeSpec(WPARAM wParam, LPARAM lParam)
{
	CCmd_EditSpec *pCmd= (CCmd_EditSpec *) wParam;
    MainFrame()->ClearStatus();

	if(!pCmd->GetError() && !m_EditInProgress
	 && pCmd->PreprocessChgSpec() && pCmd->DoSpecDlg(this))
	{
		m_EditInProgress = TRUE;
		m_EditInProgressWnd = pCmd->GetSpecSheet();
	}
	else
	{
		if (pCmd->HaveServerLock())
			pCmd->ReleaseServerLock();
   		delete pCmd;
	}
	return 0;
}

LRESULT COldChgListCtrl::OnP4EndSpecEdit( WPARAM wParam, LPARAM lParam )
{
	CCmd_EditSpec *pCmd= (CCmd_EditSpec *) wParam;

	if (lParam != IDCANCEL && lParam != IDABORT)
    {
		// The change description may have changed, so update the display.
		CString desctxt=PadCRs(pCmd->GetChangeDesc());

		int changeNum= pCmd->GetNewChangeNum();
		CString search;
		search.Format( NUMFORMAT, changeNum );

		int index= FindInList( search );
		if( index > -1 )
		{
			SetItemText( index, 3, desctxt );
			CP4Change *change= (CP4Change *) GetItemData(index);
			change->SetDescription(desctxt);
		}

		CString txt;
		txt.FormatMessage(IDS_CHANGE_n_UPDATED, (long) pCmd->GetNewChangeNum());
		AddToStatus(txt);
	}

	if (lParam != IDABORT)
	{
		MainFrame()->ClearStatus();
		if (pCmd->HaveServerLock())
			pCmd->ReleaseServerLock();
		CDialog *dlg = (CDialog *)pCmd->GetSpecSheet();
		dlg->DestroyWindow();
	}
	delete pCmd;
	m_EditInProgress = FALSE;
	return 0;
}


//////////////////////////////////////////////////////////////////////////
// Sort callback, not in class

int COldChgListCtrl::OnCompareItems(LPARAM lParam1, LPARAM lParam2, int subItem)
{
    ASSERT(lParam1 && lParam2);
    CP4Change const *change1 = (CP4Change const *)lParam1;
    CP4Change const *change2 = (CP4Change const *)lParam2;


	CString txt1, txt2;
	switch(subItem)
	{
	case OLDCHG_NAME:
		if(m_SortAscending)
			return( change1->GetChangeNumber() -
					change2->GetChangeNumber() );
		else
			return( change2->GetChangeNumber() -
					change1->GetChangeNumber() );
		break;

	case OLDCHG_DATE:
		txt1= change1->GetChangeDate();
		txt2= change2->GetChangeDate();
		ConvertDates( txt1, txt2 );
		break;

	case OLDCHG_USER:
		txt1= change1->GetUser();
		txt2= change2->GetUser();
		break;

	case OLDCHG_DESC:
		txt1= change1->GetDescription();
		txt2= change2->GetDescription();
		break;

	default:
		ASSERT(0);
		return 0;
	}

	int rc;

	if(m_SortAscending)
	{
		if ( subItem == OLDCHG_USER )
			rc = Compare(txt1, txt2);
		else
			rc = txt1.Compare(txt2);
	}
	else
	{
		if ( subItem == OLDCHG_USER )
			rc = Compare(txt2, txt1);
		else
			rc = txt2.Compare(txt1);
	}

	return rc;
}

void COldChgListCtrl::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	delete (CP4Change *) GetItemData(pNMListView->iItem);
			
	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////
// Handlers for setting filters for submitted changes view
/////////////////////////////////////////////////////////////////////

void COldChgListCtrl::OnFilterClearview() 
{
    if( SERVER_BUSY() )
		return;

	ClearFilter();
	OnViewReloadall();
}

void COldChgListCtrl::OnFilterSetviewDrop()
{
    ::SendMessage(m_depotWnd, WM_GETSELLIST, (WPARAM) &m_FilterView, (LPARAM)TRUE);
	if (m_FilterView.GetCount())
	{
		// save filter changes and update views
		PersistentChgFilter( KEY_WRITE );
		SetCaption();
		OnViewReloadall();
	}
	else
		TheApp()->StatusAdd(LoadStringResource(IDS_NO_SELECTED_FILES_IN_LOCAL_DEPOTS), SV_WARNING);
}

void COldChgListCtrl::OnFilterSetview() 
{
//FIXTHIS need to enable include integ using this?:	GET_P4REGPTR()->GetEnableSubChgIntegFilter( )
	COldChgFilterDlg dlg;

	// initialize filter vars
	dlg.m_useClient = m_FilteredByClient;
	dlg.m_client = m_ClientFilter;
	dlg.m_useUser = m_FilteredByUser;
	dlg.m_user = m_UserFilter;
	dlg.m_includeIntegrations = m_FilterInteg;

	// get selected files from depot view and convert to string
	CStringList selected;
    ::SendMessage(m_depotWnd, WM_GETSELLIST, (WPARAM) &selected, 0);
	CString selectedTxt;
	POSITION pos = selected.GetHeadPosition();
	int i;
	for(i=0; pos != NULL; i++)
	{
		CString sel = selected.GetNext(pos);
		selectedTxt += sel + _T(" ");
		dlg.m_selected.AddTail(sel);
	}
	selectedTxt.TrimRight();
	dlg.m_selectedFiles = selectedTxt;

	// make a radio button selection based on various strings
	if(m_FilterView.IsEmpty())
		dlg.m_filterFiles = 0;
	else
	{
		// convert current view to a string
		CString currentTxt;
		pos = m_FilterView.GetHeadPosition();
		for(i=0; pos != NULL; i++)
			currentTxt += m_FilterView.GetNext(pos) + _T(" ");
		currentTxt.TrimRight();
		if(currentTxt == selectedTxt)
			dlg.m_filterFiles = 3;
		else
			dlg.m_filterFiles = 2;
	}

	if(dlg.DoModal() == IDCANCEL)
		return;

	// get client and user filter settings
	m_FilteredByClient = dlg.m_useClient;
	GET_P4REGPTR()->SetFilteredByClient(m_FilteredByClient);
	if(m_FilteredByClient)
	{
        m_ClientFilter = dlg.m_client;
		GET_P4REGPTR()->SetClientFilter(m_ClientFilter);
	}

	m_FilteredByUser = dlg.m_useUser;
	GET_P4REGPTR()->SetFilteredByUser(m_FilteredByUser);
	if(m_FilteredByUser)
	{
		m_UserFilter = dlg.m_user;
		GET_P4REGPTR()->SetUserFilter(m_UserFilter);
	}

	// get include integrations option
	m_FilterInteg = dlg.m_includeIntegrations;

	// get the filter view
	switch(dlg.m_filterFiles)
	{
	case 0:	// all files
		m_FilterView.RemoveAll();
		break;
	case 1:	// my client files
		m_FilterView.RemoveAll();
		m_FilterView.AddTail(CString(_T("//")) + GET_P4REGPTR()->GetP4Client( ) + _T("/..."));
		break;
	case 2:	// filespec
		{
		// convert filespec into view stringlist
		m_FilterView.RemoveAll();
		CString filespec = dlg.m_filespec;
		int i;
		while ((i = filespec.Find(_T("//"), 2)) != -1)
		{
			if (filespec.GetAt(i-1) == _T('\"'))
				i--;
			CString txt = filespec.Left(i);
			txt.TrimRight();
			txt.TrimRight(_T('\"'));
			txt.TrimLeft(_T('\"'));
			m_FilterView.AddTail(txt);
			filespec = filespec.Mid(i);
		}
		filespec.TrimRight();
		filespec.TrimRight(_T('\"'));
		filespec.TrimLeft(_T('\"'));
		if (!filespec.IsEmpty())
			m_FilterView.AddTail(filespec);
		}
		break;
	case 3:	// selected files
	  {
		m_FilterView.RemoveAll();
		POSITION pos = selected.GetHeadPosition();
		while (pos != NULL)
		{
			CString str = selected.GetNext(pos);
			if (dlg.m_UseClientSyntax)
			{
				// user wants to convert to client syntax
				CCmd_Where *pCmd1 = new CCmd_Where;
				pCmd1->Init(NULL, RUN_SYNC);
				if ( pCmd1->Run(str) && !pCmd1->GetError() 
				  && pCmd1->GetClientFiles()->GetCount() )
				{
					CStringList * list = pCmd1->GetClientFiles();
					POSITION pos2 = list->GetHeadPosition();
					while (pos2 != NULL)
						m_FilterView.AddTail(list->GetNext(pos2));
				}
				else	// p4 where failed - use depot syntax after all
					m_FilterView.AddTail(str);
				delete pCmd1;
			}
			else
				m_FilterView.AddTail(str);
		}

		// get the filter rev range, if any was set
		CString revRange;
		dlg.GetFilterRevRange(revRange);

		// apply the filter range to the view if applicable
		if (!revRange.IsEmpty())
		{
			POSITION pos=m_FilterView.GetHeadPosition();
			m_StrList.RemoveAll();
			while(pos != NULL)
				m_StrList.AddTail(m_FilterView.GetNext(pos) + revRange);
			pos=m_StrList.GetHeadPosition();
			m_FilterView.RemoveAll();
			while(pos != NULL)
				m_FilterView.AddTail(m_StrList.GetNext(pos));
		}
		break;
	  }
	}


	// save filter changes and update views
	PersistentChgFilter( KEY_WRITE );
	SetCaption();
	OnViewReloadall();
}

LRESULT COldChgListCtrl::CallOnUpdateFilterClearview(WPARAM wParam, LPARAM lParam)
{
	OnUpdateFilterClearview((CCmdUI *)lParam);
	return 0;
}

void COldChgListCtrl::OnUpdateFilterClearview(CCmdUI* pCmdUI) 
{
	pCmdUI->SetText(LoadStringResource(IDS_FILTER_CLEARVIEW));
	pCmdUI->Enable(MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY() && 
		(m_FilterView.GetCount() || m_FilteredByClient || m_FilteredByUser)));
}

void COldChgListCtrl::OnUpdateFilterSetview(CCmdUI* pCmdUI) 
{
	pCmdUI->SetText(LoadStringResource(IDS_FILTER_SETVIEW));
	pCmdUI->Enable( MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY()) );
}

void COldChgListCtrl::PersistentChgFilter( REGSAM accessmask )
{
	LPCTSTR sKey = _T("Software\\Perforce\\P4Win\\");
	LPCTSTR sEntry = _T("ChgFilter");
	LPCTSTR sType  = _T("ChgFiltype");
	LPCTSTR sPort  = _T("ChgFilPort");

    CRegKeyEx key;
    if(ERROR_SUCCESS == key.Create(HKEY_CURRENT_USER, sKey, REG_NONE, REG_OPTION_NON_VOLATILE, accessmask))
    {
        if(accessmask == KEY_WRITE)
        {
			POSITION pos=m_FilterView.GetHeadPosition();
            CString filter;
            CString txt;
			while(pos != NULL)
			{
				txt = m_FilterView.GetNext(pos);
				filter += txt + _T('\t');
			}

            key.SetValueString(filter, sEntry);
			if (filter.GetLength() > 0)						// if filter is not blank
				GET_P4REGPTR()->AddMRUChgFilter( filter );	// save as most recently used in Reg


			txt.Format(_T("%d"), m_FilterInteg);
            key.SetValueString(txt, sType);
            key.SetValueString(GET_P4REGPTR()->GetP4Port(), sPort);
        }
		else
		{
			m_FilterView.RemoveAll();
			m_FilterInteg = m_FilterSpecial = FALSE;
			
            CString port = key.QueryValueString(sPort);
            if(!port.IsEmpty() && port == GET_P4REGPTR()->GetP4Port())
			{
				// in older versions, filter elements were written to the 
				// registry using '@' as a separator.  Now we use '\t', but 
				// we have to handle finding an older setting.  It's easy, 
				// since the whole thing will be terminated with the separator 
				// character.
                CString entry = key.QueryValueString(sEntry);
				bool oldVersion = entry.Right(1) == _T("@");
				m_FilterSpecial = !oldVersion && (entry.FindOneOf(_T("@#")) != -1);
				CString separator = oldVersion ? _T("@") : _T("\t");
                while(!entry.IsEmpty())
				{
                    CString elem = entry.SpanExcluding(separator);

                    if(!elem.IsEmpty())
                    {
                        m_FilterView.AddTail(elem);
                        entry = entry.Mid(elem.GetLength()+1);
                    }
                }
                CString type = key.QueryValueString(sType);
                if(!type.IsEmpty())
				{
					m_FilterInteg = type[0] & 0x01;
					m_FilterSpecial += m_FilterInteg;
				}
			}
			else
			{
				GET_P4REGPTR()->SetFilteredByUser(m_FilteredByUser = FALSE);
				GET_P4REGPTR()->SetFilteredByUser(m_FilteredByClient = FALSE);
			}
			SetCaption();
		}
    }
}

void COldChgListCtrl::SetCaption()
{
	if (m_FilterView.GetCount() > 0 || m_FilteredByUser || m_FilteredByClient)
	{
		CString txt = _T("");
		if (m_FilteredByClient)
			txt = LoadStringResource(IDS_CLIENT) + _T(' ') + m_ClientFilter;
		if (m_FilteredByUser)
		{
			if (!txt.IsEmpty())
				txt += _T("; ");
			txt += LoadStringResource(IDS_USER) + _T(' ') + m_UserFilter;
		}
		if (m_FilterView.GetCount() > 0)
		{
			if (!txt.IsEmpty())
				txt += _T("; ");
			txt += m_FilterView.GetHead();
			if (m_FilterView.GetCount() > 1)
			{
				POSITION pos= m_FilterView.GetHeadPosition();
				m_FilterView.GetNext(pos);
				while( pos != NULL )
					txt += _T(", ") + m_FilterView.GetNext(pos);
			}
		}
		m_caption.FormatMessage(IDS_SUBMITTED_PERFORCE_CHANGELISTS_FILTERED, txt);
	}
	else
		m_caption = LoadStringResource(IDS_SUBMITTED_PERFORCE_CHANGELISTS);

	CP4PaneContent::GetView()->SetCaption();
}

/////////////////////////////////////////////////////////////////////
// OLE drag-drop support, to accept depot files or folders
// or accept user or client names which will
// define a view to be used to filter the submitted
// changes that this window displays.
// Also can drop Jobs to be Fixed.
/////////////////////////////////////////////////////////////////////

DROPEFFECT COldChgListCtrl::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	m_DropEffect=DROPEFFECT_NONE;
	m_DragDataFormat=0;

	// Dont allow a drop if the server is busy, since a drop immediately attempts to
	// invoke a server command
	if(SERVER_BUSY())
		return DROPEFFECT_NONE;
		
	if(pDataObject->IsDataAvailable( (unsigned short) m_CF_DEPOT))
	{
		m_DropEffect=DROPEFFECT_COPY;
		m_DragDataFormat=m_CF_DEPOT;
	}
	else if(pDataObject->IsDataAvailable( (unsigned short) m_CF_JOB))
	{
		// Set the display of the drag-from items
		m_DropEffect=DROPEFFECT_COPY;
		m_DragDataFormat=m_CF_JOB;
		// Set the scrolling data
	    RECT rc;
		GetItemRect(0, &rc, LVIR_BOUNDS);
		m_ItemHeight = rc.bottom - rc.top;
		HWND hwnd = GetHeaderCtrl()->m_hWnd;
		if (hwnd)
		{
			::GetWindowRect(hwnd, &rc);
			m_HdrHeight = rc.bottom - rc.top;
		}
		else
			m_HdrHeight = m_ItemHeight;
		m_BottomOfPage = GetCountPerPage() * m_ItemHeight + m_HdrHeight;
		m_bStarting = TRUE;
	}
	else if(pDataObject->IsDataAvailable( (unsigned short) m_CF_CLIENT))
	{
		m_DropEffect=DROPEFFECT_COPY;
		m_DragDataFormat=m_CF_CLIENT;
	}
	else if(pDataObject->IsDataAvailable( (unsigned short) m_CF_USER))
	{
		m_DropEffect=DROPEFFECT_COPY;
		m_DragDataFormat=m_CF_USER;
	}
#ifdef UNICODE
	else if(pDataObject->IsDataAvailable( (unsigned short) CF_UNICODETEXT))
	{
		m_DropEffect=DROPEFFECT_COPY;
		m_DragDataFormat=CF_UNICODETEXT;
	}
#else
	else if(pDataObject->IsDataAvailable( (unsigned short) CF_TEXT))
	{
		m_DropEffect=DROPEFFECT_COPY;
		m_DragDataFormat=CF_TEXT;
	}
#endif

	return m_DropEffect;
}


DROPEFFECT COldChgListCtrl::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	// Dont allow a drop if the server is busy, since a drop immediately attempts to
	// invoke a server command
	if(SERVER_BUSY())
		m_DropEffect= DROPEFFECT_NONE;
		
	if(pDataObject->IsDataAvailable( (unsigned short) m_CF_JOB))
	{
		if (point.y > (m_HdrHeight + m_ItemHeight*2))
			m_bStarting = FALSE;
		if (!m_bStarting)
		{
			if (point.y < (m_HdrHeight + m_ItemHeight/2))
				Scroll(CSize(0, -m_ItemHeight));
			else if (point.y > (m_BottomOfPage - m_ItemHeight/2))
				Scroll(CSize(0, m_ItemHeight));
		}
	}
	return m_DropEffect;
}


BOOL COldChgListCtrl::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	CString fname;
	
	if(SERVER_BUSY())
	{
		// OnDragEnter() and OnDragOver() should avoid a drop at 
		// the wrong time!
		ASSERT(0);
		return FALSE;
	}
	
	if(m_DragDataFormat == m_CF_DEPOT)
	{
		ClientToScreen(&point);
		::SendMessage(m_depotWnd, WM_DROPTARGET, SUBMITTEDCHG, MAKELPARAM(point.x,point.y));
		m_ForceFocusHere = TRUE;
		return TRUE;
	}
	if(m_DragDataFormat == m_CF_JOB)
	{
		int i = GetSelectedItem( );
		if ( i > -1 )
			SetItemState( i, 0, LVIS_DROPHILITED|LVIS_SELECTED|LVIS_FOCUSED );
		ClientToScreen(&point);
		int index= GetContextItem( point );
		if (index > -1)
			SetItemState( index, LVIS_SELECTED|LVIS_FOCUSED,
												  LVIS_SELECTED|LVIS_FOCUSED );
		CString jobname;
		CStringList jobnames;
		jobnames.AddHead(jobname = MainFrame()->GetDragFromJob());
		long changeNumber = _tstol(GetSelectedItemText());
		TCHAR str[ LISTVIEWNAMEBUFSIZE + 1 ];
		GetItemText( index, 3, str, LISTVIEWNAMEBUFSIZE );

		CString txt;
		txt.FormatMessage(IDS_ADD_JOB_FIX, jobname, changeNumber, str);
		if (IDYES == AfxMessageBox(txt, MB_ICONQUESTION|MB_YESNO))
			AddJobFixes(&jobnames, changeNumber, NULL);
		return TRUE;
	}
	if(m_DragDataFormat == m_CF_CLIENT)
	{
		FilterByClient(MainFrame()->GetDragFromClient());
		return TRUE;
	}
	if(m_DragDataFormat == m_CF_USER)
	{
		FilterByUser(MainFrame()->GetDragFromUser());
		return TRUE;
	}
#ifdef UNICODE
	if(m_DragDataFormat == CF_UNICODETEXT)
	{
		HGLOBAL hGlob = pDataObject->GetGlobalData(CF_UNICODETEXT);
#else
	if(m_DragDataFormat == CF_TEXT)
	{
		HGLOBAL hGlob = pDataObject->GetGlobalData(CF_TEXT);
#endif
		LPCTSTR p;

		if ((hGlob != NULL)	&& ((p = (LPCTSTR)::GlobalLock(hGlob)) != NULL))
		{
			CString itemStr = p;
			::GlobalUnlock(hGlob);
			itemStr.Format(_T("%05d"), _ttoi(itemStr));
			OnEditPaste( itemStr );
		}
		return TRUE;
	}
	// Return false, so depot window doesnt start a file-open operation
	return FALSE;
}

int COldChgListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CP4ListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetImageList(TheApp()->GetImageList(), LVSIL_SMALL);
	
	// Insert the columns 
	int headers[OLDCHG_MAXCOL]={IDS_CHANGELIST, IDS_DATE, IDS_P4USER, IDS_DESCRIPTION};
	int width[OLDCHG_MAXCOL]={90,90,130,200};
	RestoreSavedWidths(width, OLDCHG_MAXCOL, sRegValue_SubmittedChanges);
	
	int retval;
	LV_COLUMN lvCol;
	for(int subItem=0; subItem < 4; subItem++)
	{
		lvCol.mask= LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT |LVCF_WIDTH;
		lvCol.fmt=LVCFMT_LEFT;
        CString header = LoadStringResource(headers[subItem]);
		lvCol.pszText=const_cast<LPTSTR>((LPCTSTR)header);
		lvCol.iSubItem=subItem;
		lvCol.cx=width[subItem];
		
		retval= InsertColumn(subItem, &lvCol);
	}
    
    m_ColsInited = TRUE;

	return 0;
}


void COldChgListCtrl::OnUpdateAddjobfix(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY());
}

void COldChgListCtrl::OnAddjobfix() 
{
	long changeNumber= GetSelectedChange();
	
    if (SERVER_BUSY() || (changeNumber == 0))
	{
		EnumChildWindows(AfxGetMainWnd()->m_hWnd, ChildSetRedraw, TRUE);
        ASSERT(0);
        return;
    }
	
    // Ask jobs window to update itself (if required) and send WM_P4JOBS when done
    // The reply message will be handled below
    SET_APP_HALTED(TRUE);
    ::SendMessage( m_jobWnd, WM_FETCHJOBS, (WPARAM)changeNumber, (LPARAM)m_hWnd);
}

LRESULT COldChgListCtrl::OnP4JobList(WPARAM wParam, LPARAM lParam)
{
    long changeNumber= wParam;

    // Get the list of jobs
    CObList *jobs= (CObList *) ::SendMessage( m_jobWnd, WM_QUERYJOBS, 0, 0);

	CString *spec= (CString *) ::SendMessage( m_jobWnd, WM_QUERYJOBSPEC, 0, 0);

	CStringArray *cols= (CStringArray *) ::SendMessage( m_jobWnd, WM_QUERYJOBCOLS, 0, 0);

	CString *curr= (CString *) ::SendMessage( m_jobWnd, WM_QUERYJOBSELECTION, 0, 0);

	CJobListDlg dlg;
	dlg.SetJobFont(GetFont());
	dlg.SetJobList(jobs);
	dlg.SetJobSpec(spec);
	dlg.SetJobCols(cols);
	dlg.SetJobCurr(curr);
	CStringList *jobnames= dlg.GetSelectedJobs();

	EnumChildWindows(AfxGetMainWnd()->m_hWnd, ChildSetRedraw, TRUE);
	int retcode= dlg.DoModal();
    SET_APP_HALTED(FALSE);

	// Delete the job list
	for(POSITION pos=jobs->GetHeadPosition(); pos!=NULL; )
		delete (CP4Job *) jobs->GetNext(pos);
    delete jobs;

	if(retcode == IDOK && jobnames->GetCount() > 0)
	{
		AddJobFixes(jobnames, changeNumber, 
			dlg.m_JobStatusValue.GetLength() ? LPCTSTR(dlg.m_JobStatusValue) : NULL);
    } // if IDOK
	else if (retcode == IDRETRY)
	{
	    ::SendMessage( m_jobWnd, WM_CLEARLIST, 0, 0);
		PostMessage(WM_COMMAND, ID_CHANGE_ADDJOBFIX, 0);
	}

	MainFrame()->ClearStatus();
    return 0;
}

void COldChgListCtrl::AddJobFixes(CStringList *jobnames, long changeNumber, LPCTSTR jobstatusvalue)
{
    POSITION pos;
    CString str;
    
	// Copy the joblist
	m_JobList.RemoveAll();
	for(pos= jobnames->GetHeadPosition(); pos != NULL; )
	{
		str= jobnames->GetNext(pos);
		m_JobList.AddHead(str);
	}

	CCmd_Fix *pCmdFix= new CCmd_Fix;
	pCmdFix->Init( m_hWnd, RUN_ASYNC, LOSE_LOCK);
	if( pCmdFix->Run( &m_JobList, changeNumber, FALSE, jobstatusvalue ) )
	{
	    MainFrame()->UpdateStatus( LoadStringResource(IDS_FIXING_JOBS) );	
	}	
	else
		delete pCmdFix;
}

void COldChgListCtrl::OnUpdateFileIntegrate(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY());	
}

void COldChgListCtrl::OnFileIntegspec() 
{
	OnIntegrate(FALSE);
}

void COldChgListCtrl::OnFileIntegrate() 
{
	CString str = _T("");;
//	long changeNumber = GetSelectedChange();
//	str.Format(_T("@%ld,%ld"), changeNumber, changeNumber);
	CStringList stringlist;
	stringlist.AddHead(str);

	INTEGCHG integChg;
	integChg.useBranch = TRUE;
	integChg.changeList= GetSelectedChange();
	integChg.filelist  = &stringlist;
	::SendMessage(m_depotWnd, WM_CHANGELISTINTEG, 0, (LPARAM)&integChg);
}

void COldChgListCtrl::OnIntegrate(BOOL useBranch)
{
	if (MainFrame()->IsModlessUp())
		return;

	long changeNumber= GetSelectedChange();
	
	if(changeNumber != 0)
	{
		m_UseBranch = useBranch;
		CString changeTxt;
		changeTxt.Format(_T("%ld"), changeNumber);

		CCmd_Describe *pCmd= new CCmd_Describe;
		pCmd->Init( m_hWnd, RUN_ASYNC);
		pCmd->SetAlternateReplyMsg(WM_P4INTEGCHG);
		if( pCmd->Run( P4DESCRIBE, changeTxt) )
			MainFrame()->UpdateStatus(LoadStringResource(IDS_FETCHING_CHANGELIST_DESCRIPTION));
		else
			delete pCmd;
	}
	else
		AfxMessageBox ( IDS_PLEASE_SELECT_A_CHANGELIST );
}

LRESULT COldChgListCtrl::OnIntegChg(WPARAM wParam, LPARAM lParam)
{
	CCmd_Describe *pCmd= (CCmd_Describe *) wParam;

	if(!pCmd->GetError())
	{
		int i, j;
		CString desc= MakeCRs(pCmd->GetDescription());
		CString temp;
		CStringList stringlist;
		if ((i = desc.Find(_T("\nAffected files"))) != -1)
		{
			while ((j = desc.Find(_T("//"), i)) != -1)
			{
				if ((i = desc.Find(_T('#'), j)) == -1)
				{
					AfxMessageBox(_T("Unable to parse filenames from changelist - Integrate fails."));
					goto breakout;
				}
				temp = desc.Mid(j, i-j);
				stringlist.AddTail(temp);
			}
			if (stringlist.GetCount() > 0)
			{
				INTEGCHG integChg;
				integChg.useBranch = m_UseBranch;
				integChg.changeList= GetSelectedChange();
				integChg.filelist  = &stringlist;
				::SendMessage(m_depotWnd, WM_CHANGELISTINTEG, 0, (LPARAM)&integChg);
			}
		}
	}	
breakout:
	delete pCmd;
	MainFrame()->ClearStatus();
	return 0;
}

void COldChgListCtrl::FilterByUser(CString user)
{
	GET_P4REGPTR()->SetUserFilter(m_UserFilter = user);
	GET_P4REGPTR()->SetFilteredByUser(m_FilteredByUser = m_UserFilter.IsEmpty( ) ? FALSE : TRUE);
	PersistentChgFilter( KEY_WRITE );
	SetCaption();
	OnViewReloadall();
}

void COldChgListCtrl::FilterByClient(CString client)
{
	GET_P4REGPTR()->SetClientFilter(m_ClientFilter = client);
	GET_P4REGPTR()->SetFilteredByClient(m_FilteredByClient = m_ClientFilter.IsEmpty( ) ? FALSE : TRUE);
	PersistentChgFilter( KEY_WRITE );
	SetCaption();
	OnViewReloadall();
}

void COldChgListCtrl::OnUpdatePositionDepot(CCmdUI* pCmdUI) 
{
	if (pCmdUI->m_pParentMenu == MainFrame()->GetMenu())
		pCmdUI->SetText(LoadStringResource(IDS_FINDCHGFILESINDEPOT));
	pCmdUI->Enable(!SERVER_BUSY());
}

void COldChgListCtrl::OnPositionDepot()
{
	CString changeTxt;
	changeTxt.Format(_T("%ld"), GetSelectedChange());
	CCmd_Describe *pCmd= new CCmd_Describe;
	pCmd->Init( m_hWnd, RUN_ASYNC);
	pCmd->SetAlternateReplyMsg(WM_P4DESCRIBEALT);
	if( pCmd->Run( P4DESCRIBE, changeTxt) )
		MainFrame()->UpdateStatus(LoadStringResource(IDS_FETCHING_CHANGELIST_DESCRIPTION));
	else
		delete pCmd;
}

void COldChgListCtrl::OnPerforceOptions()
{
	MainFrame()->OnPerforceOptions(TRUE, FALSE, IDS_PAGE_CHANGELIST);
}

LRESULT COldChgListCtrl::OnP4ChgRollback(WPARAM wParam, LPARAM lParam)
{
	return OnP4DescribeAlt(wParam, 0);
}

LRESULT COldChgListCtrl::OnP4ChgRollbackPreview(WPARAM wParam, LPARAM lParam)
{
	return OnP4DescribeAlt(wParam, 1);
}

LRESULT COldChgListCtrl::OnP4ChgSelectAll(WPARAM wParam, LPARAM lParam)
{
	return OnP4DescribeAlt(wParam, 2);
}

LRESULT COldChgListCtrl::OnP4DescribeAlt(WPARAM wParam, LPARAM lParam)
{
	CCmd_Describe *pCmd= (CCmd_Describe *) wParam;

	if(!pCmd->GetError())
	{
		int i, j, count=0;
		BOOL preview = lParam==1;
		BOOL selectAll = lParam==2;
		CString desc= MakeCRs(pCmd->GetDescription());
		MainFrame()->SetAdd2ExpandItemList(selectAll);
		m_StrList.RemoveAll();
		CString temp;
		if ((i = desc.Find(_T("\nAffected files"))) != -1)
		{
			while ((j = desc.Find(_T("//"), i)) != -1)
			{
				if ((i = desc.Find(_T('#'), j)) == -1)
				{
					AfxMessageBox(_T("Unable to parse filenames from changelist - Rollback fails."));
					goto breakout;
				}
				if (selectAll)
				{
					MainFrame()->ExpandDepotString( desc.Mid(j, i-j), TRUE );
					while (MainFrame()->IsExpandDepotContinuing()
							|| SERVER_BUSY())
					{
						if ( !MainFrame()->PumpMessages( ) )
							break;
						Sleep(250);
					}
					count++;
				}
				else
				{
					int rev = _tstoi(desc.Mid(i+1));
					if (rev > 1)
						temp.Format(_T("%s#%d"), desc.Mid(j, i-j), rev-1);
					else
						temp.Format(_T("%s#none"), desc.Mid(j, i-j));
					m_StrList.AddTail(temp);
				}
			}
			if (selectAll)
			{
				MainFrame()->SetAdd2ExpandItemList(FALSE);
				MainFrame()->SelectExpandItemList();
				if (MainFrame()->GetExpandItemListCount() < count)
				{
					CString txt;
					int n = count - MainFrame()->GetExpandItemListCount();
					txt.FormatMessage(IDS_NOTALLITEMSSELECTED_d, n, n==1 ? _T("") : _T("s"));
					TheApp()->StatusAdd( txt, SV_WARNING );
				}
			}
			else if (m_StrList.GetCount() > 0)
			{
				CCmd_Get *pCmd2= new CCmd_Get;
				pCmd2->Init( m_hWnd, RUN_ASYNC);
				if( pCmd2->Run( &m_StrList, preview ) )
					MainFrame()->UpdateStatus( LoadStringResource(IDS_FILE_SYNC) );
				else
					delete pCmd2;
			}
		}
	}	
breakout:
	delete pCmd;
	MainFrame()->ClearStatus();
	return 0;
}

void COldChgListCtrl::OnUpdateRemovefix(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_DeleteFixesDlg && !SERVER_BUSY() && GetSelectedChange());	
}

void COldChgListCtrl::OnRemovefix() 
{
	int changeNum = GetSelectedChange();
	if (!changeNum || m_DeleteFixesDlg)
		return;

	// Now fire off the request for all the fixes for the changelist
	CCmd_Fixes *pCmdFixes= new CCmd_Fixes;
	pCmdFixes->Init( m_hWnd, RUN_ASYNC);
	if( pCmdFixes->Run(changeNum, NULL) )
	{
		MainFrame()->UpdateStatus( LoadStringResource(IDS_UPDATING_JOB_FIXES) );
	}
	else
	{
		delete pCmdFixes;
		RedrawWindow();
       	MainFrame()->ClearStatus();
	}
}

LRESULT COldChgListCtrl::OnP4Fixes(WPARAM wParam, LPARAM lParam)
{
	CCmd_Fixes *pCmd= (CCmd_Fixes *) wParam;
	CString txt;

	if(!pCmd->GetError())
	{
		m_DelFixesChgNbr = pCmd->GetFixedChangeNumber();
		CObList *fixes = pCmd->GetList();

		if (fixes->GetCount() > 0 && !m_DeleteFixesDlg)
		{
			m_DelFixesList.RemoveAll();
			for(POSITION pos= fixes->GetHeadPosition(); pos != NULL; )
			{
				CP4Fix *fix=(CP4Fix *) fixes->GetNext(pos);
				ASSERT_KINDOF(CP4Fix,fix);
				ASSERT(fix->GetChangeNum() == m_DelFixesChgNbr);
				txt.FormatMessage(IDS_s_FIXED_BY_s_ON_s, fix->GetJobName(), 
									fix->GetUser(), fix->GetFixDate());
				m_DelFixesList.AddTail(txt);
				delete fix;
			}

			m_DeleteFixesDlg = new CDeleteFixes(this);
			if (!m_DeleteFixesDlg)
			{
				ASSERT(0);
				AfxMessageBox(IDS_COULD_NOT_CREATE_DELFIX_DIALOG_BOX, MB_ICONSTOP);
				delete pCmd;
				return 0;
			}

			m_pJobSpec = (CString *) ::SendMessage( m_jobWnd, WM_QUERYJOBSPEC, 0, 0);
			if (!m_pJobSpec || m_pJobSpec->IsEmpty())
			{
				CCmd_JobSpec *pCmd = new CCmd_JobSpec;
				pCmd->Init( m_hWnd, RUN_SYNC );
				if( !pCmd->Run() || pCmd->GetError() )
				{
					AddToStatus(_T("Unable to obtain the JobSpec"), SV_WARNING);	//FIXME!
				}
				else
				{
					pCmd->GetSpec( m_JobSpec );
					m_DeleteFixesDlg->SetJobSpec(&m_JobSpec);
					m_pJobSpec = NULL;
				}
				delete pCmd;
			}
			else
				m_DeleteFixesDlg->SetJobSpec(m_pJobSpec);

			txt.FormatMessage(IDS_REMOVE_FIXES_FROM_CHGLIST_d, m_DelFixesChgNbr);
			m_DeleteFixesDlg->SetCaption(txt);
			m_DeleteFixesDlg->SetSelected(&m_DelFixesList);
			m_DeleteFixesDlg->SetOutputList(&m_DelFixesList);
			m_DeleteFixesDlg->SetOutputStatus(&m_DelFixesStatus);
			m_DeleteFixesDlg->SetReturnMsg(WM_ONDODELETEFIXES);
			if (!m_DeleteFixesDlg->Create(IDD_DELETEFIXES, this))	// display the rm fixes dialog
			{
				delete m_DeleteFixesDlg;
				MainFrame()->SetModelessUp(FALSE);
			}
		}
		else
		{
			txt.FormatMessage(IDS_CHGLIST_d_HAS_NO_JOBS, m_DelFixesChgNbr);
			AddToStatus(txt);
		}
	}
	
   	MainFrame()->ClearStatus();
		
	delete pCmd;
	return 0;
}

LRESULT COldChgListCtrl::OnDoDeleteFixes(WPARAM wParam, LPARAM lParam)
{
	if (wParam == IDOK)
	{
		if (m_DelFixesList.GetCount())
		{
			int i;
			CString job;
			m_JobList.RemoveAll();
			for (POSITION pos = m_DelFixesList.GetHeadPosition(); pos != NULL; )
			{
				job = m_DelFixesList.GetNext(pos);
				if ((i = job.Find(_T(' '))) != -1)
				{
					job = job.Left(i);
					m_JobList.AddTail(job);
				}
			}

			CCmd_Fix *pCmd= new CCmd_Fix;
			pCmd->Init( m_hWnd, RUN_ASYNC, LOSE_LOCK);
			if( pCmd->Run( &m_JobList, m_DelFixesChgNbr, TRUE, 
					m_DelFixesStatus.IsEmpty() ? NULL : m_DelFixesStatus.GetBuffer(40) ) )
				MainFrame()->UpdateStatus( LoadStringResource(IDS_UNFIXING_JOB) );	
			else
				delete pCmd;

			m_DelFixesList.RemoveAll();
		}
		else
			AddToStatus(LoadStringResource(IDS_NOTHING_SELECTED_NOTHING_TO_DO));
	}
	if (m_DeleteFixesDlg)
	{
		m_DeleteFixesDlg->DestroyWindow();	// deletes m_DelSyncDlg
		m_DeleteFixesDlg = 0;
		MainFrame()->SetModelessUp(FALSE);
	}
	return 0;
}

LRESULT COldChgListCtrl::OnP4Fix(WPARAM wParam, LPARAM lParam)
{
	CP4Fix *fix;
	CString text;
	BOOL b = FALSE;
	
	m_JobList.RemoveAll();
	CCmd_Fix *pCmd= (CCmd_Fix *) wParam;
	if(!pCmd->GetError())
	{
		CObList *list= pCmd->GetList();
	
		if(list->GetCount() > 0)
		{
			b = pCmd->IsUnfixing() && pCmd->IsNewStatus();
			POSITION pos= list->GetHeadPosition();
			while( pos != NULL )
			{
				fix= (CP4Fix *) list->GetNext(pos);

				UINT	msgnbr;
				if (b)
				{
					m_JobList.AddTail(fix->GetJobName());
					msgnbr = IDS_s_STATUSCHGED;
				}
				else if (pCmd->IsUnfixing())
				{
					msgnbr = IDS_s_REMOVEDFROM_s;
				}
				else
				{
					msgnbr = IDS_s_ADDEDTO_s;
				}

				// Find the fix and display a message
				CString txt;
				txt.FormatMessage(msgnbr, fix->GetJobName(), fix->GetChangeNum());
				AddToStatus(txt);

				delete fix;
			} //while
		} // if
	}
	
   	MainFrame()->ClearStatus();
	delete pCmd;

	if (b && !m_JobList.IsEmpty())
	{
		pCmd= new CCmd_Fix;
		pCmd->Init( m_hWnd, RUN_ASYNC, LOSE_LOCK );
		if( pCmd->Run( &m_JobList, m_DelFixesChgNbr, TRUE ) )
			MainFrame()->UpdateStatus( LoadStringResource(IDS_UNFIXING_JOB) );	
		else
			delete pCmd;
	}
	return 0;
}
