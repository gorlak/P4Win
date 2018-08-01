// BranchListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "BranchListCtrl.h"
#include "MainFrm.h"
#include "P4SpecDlg.h"   
#include "ImageList.h"
#include "FilterByOwnerDlg.h"
#include "Cmd_Delete.h"
#include "Cmd_Describe.h"
#include "Cmd_EditSpec.h"
#include "Cmd_Branches.h"
#include "Cmd_Integrate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static LPCTSTR sRegKey_ColumnNames = _T("Branch List");

enum BranchSubItem
{
	BRANCH_NAME,
	BRANCH_OWNER,
	BRANCH_OPTIONS,
	BRANCH_UPDATEDATE,
	BRANCH_DESC,
	BRANCH_MAXCOL
};

/////////////////////////////////////////////////////////////////////////////
// CBranchListCtrl

IMPLEMENT_DYNCREATE(CBranchListCtrl, CP4ListCtrl)

BEGIN_MESSAGE_MAP(CBranchListCtrl, CP4ListCtrl)
	ON_COMMAND(ID_BRANCH_DELETE, OnBranchDelete)
	ON_UPDATE_COMMAND_UI(ID_BRANCH_DELETE, OnUpdateBranchDelete)
	ON_COMMAND(ID_BRANCH_EDITSPEC, OnBranchEditspec)
	ON_UPDATE_COMMAND_UI(ID_BRANCH_EDITSPEC, OnUpdateBranchEditspec)
	ON_COMMAND(ID_BRANCH_NEW, OnBranchNew)
	ON_UPDATE_COMMAND_UI(ID_BRANCH_NEW, OnUpdateBranchNew)
	ON_UPDATE_COMMAND_UI(ID_VIEW_UPDATE_RIGHT, OnUpdateViewUpdate)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_BRANCH_INTEG, OnBranchInteg)
	ON_UPDATE_COMMAND_UI(ID_BRANCH_INTEG, OnUpdateBranchInteg)
	ON_UPDATE_COMMAND_UI(ID_BRANCH_DESCRIBE, OnUpdateBranchDescribe)
	ON_COMMAND(ID_BRANCH_DESCRIBE, OnDescribe)
	ON_UPDATE_COMMAND_UI(ID_FILTERBRANCHBYOWNER, OnUpdateFilterByOwner)
	ON_COMMAND(ID_FILTERBRANCHBYOWNER, OnFilterByOwner)
	ON_UPDATE_COMMAND_UI(ID_CLEARBRANCHOWNERFILTER, OnUpdateClearFilterByOwner)
	ON_COMMAND(ID_CLEARBRANCHOWNERFILTER, OnClearFilterByOwner)
	ON_COMMAND(ID_VIEW_UPDATE_RIGHT, OnViewUpdate)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnDeleteitem)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
	ON_WM_LBUTTONDBLCLK()
	ON_MESSAGE(WM_P4BRANCHES, OnP4BranchList )
	ON_MESSAGE(WM_P4EDITSPEC, OnP4BranchSpec )
	ON_MESSAGE(WM_P4ENDSPECEDIT, OnP4EndSpecEdit )
	ON_MESSAGE(WM_P4DELETE, OnP4Delete )
	ON_MESSAGE(WM_P4DESCRIBE, OnP4Describe )
    ON_MESSAGE(WM_P4ENDDESCRIBE, OnP4EndDescribe )
	ON_MESSAGE(WM_REQUESTBRANCHLIST, OnRequestBranchesList )
	ON_MESSAGE(WM_NEWUSER, NewUser )
	ON_WM_CREATE()
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()

CBranchListCtrl::CBranchListCtrl()
{
	m_SortAscending=TRUE;
	m_LastSortCol=0;
	m_pNewSpec = NULL;
	m_viewType = P4BRANCH_SPEC;
	m_Need2DoNew = FALSE;
	m_caption = m_captionplain = LoadStringResource(IDS_PERFORCE_BRANCHES);
}

CBranchListCtrl::~CBranchListCtrl()
{
}

/////////////////////////////////////////////////////////////////////////////
// CBranchListCtrl diagnostics

#ifdef _DEBUG
void CBranchListCtrl::AssertValid() const
{
	CP4ListCtrl::AssertValid();
}

void CBranchListCtrl::Dump(CDumpContext& dc) const
{
	CP4ListCtrl::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBranchListCtrl message handlers


void CBranchListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	int index = GetHitItem ( point );
	if( index > -1 )
	{
		SetItemState( index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
		OnDescribe();
	}
	else
		CP4ListCtrl::OnLButtonDblClk(nFlags, point);
}

LRESULT CBranchListCtrl::NewUser(WPARAM wParam, LPARAM lParam)
{
	if (GET_P4REGPTR()->GetBranchFilteredFlags() & 0x10)
		Clear();
	return 0;
}

void CBranchListCtrl::Clear()
{
	m_Active=GetSelectedItemText();
	if (!m_Active.IsEmpty())
		GET_P4REGPTR()->SetLastBranch(m_Active);

	SetRedraw(FALSE);
	DeleteAllItems();
    SetRedraw(TRUE);

	m_pNotifyWnd= NULL;
	CP4ListCtrl::Clear();
}

void CBranchListCtrl::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	delete (CP4Branch *) GetItemData(pNMListView->iItem);
			
	*pResult = 0;
}

void CBranchListCtrl::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
//	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	CString str=GetSelectedItemText();
	if (!str.IsEmpty())
		GET_P4REGPTR()->SetLastBranch(m_Active = str);

	*pResult = 0;
}

void CBranchListCtrl::OnUpdateBranchDelete(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( OnUpdateShowMenuItem( pCmdUI, IDS_DELETE_s ) 
					&& !MainFrame()->IsModlessUp() 
					&& !SelectedItemIsLocked(BRANCH_OPTIONS) );
}

void CBranchListCtrl::OnBranchDelete() 
{
	OnDelete( P4BRANCH_DEL ) ;
}

void CBranchListCtrl::OnUpdateBranchEditspec(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( OnUpdateShowMenuItem( pCmdUI, IDS_EDITSPEC_s )
					&& !MainFrame()->IsModlessUp() && !m_EditInProgress
					&& (!SelectedItemIsLocked(BRANCH_OPTIONS) 
					  || !GetSelectedItemOwner(BRANCH_OWNER).CompareNoCase(GET_P4REGPTR()->GetP4User())) );
}

void CBranchListCtrl::EditTheSpec(CString *name) 
{
	m_Active=*name;
	m_NewBranch=FALSE;
	OnEditSpec( m_Active );
}

void CBranchListCtrl::OnBranchEditspec() 
{
	m_Active=GetSelectedItemText();
	m_NewBranch=FALSE;
	OnEditSpec( m_Active );
}

void CBranchListCtrl::OnUpdateBranchNew(CCmdUI* pCmdUI) 
{
	pCmdUI->SetText ( LoadStringResource( IDS_NEW ) );
	pCmdUI->Enable(!SERVER_BUSY() && ! m_EditInProgress && !MainFrame()->IsModlessUp());	
	m_Need2DoNew = FALSE;
}

void CBranchListCtrl::OnBranchNew() 
{
	MainFrame()->ViewBranches();
	if (SERVER_BUSY())
		m_Need2DoNew = TRUE;
	else
	{
		m_Need2DoNew = FALSE;
		m_Active.Empty();
		m_NewBranch=TRUE;
        //FIXTHIS: string
		OnEditSpec( CString(NEWSPECNAME) );
	}
}

void CBranchListCtrl::OnBranchInteg() 
{
	GET_P4REGPTR()->SetLastBranch(m_Active=GetSelectedItemText());
	
	// Ask the depot window to perform the integrate task, including
	// the fetching of change numbers, branch names, popping the dlg,
	// and getting results displayed
	::SendMessage(m_depotWnd, WM_BRANCHINTEG, (WPARAM) LPCTSTR(m_Active), 0);
}

void CBranchListCtrl::OnUpdateBranchInteg(CCmdUI* pCmdUI) 
{
	CString prompt;
    prompt.FormatMessage(IDS_INTEGRATEUSING_s, TruncateString(GetSelectedItemText(),50));
	pCmdUI->SetText( prompt );
	pCmdUI->Enable( !SERVER_BUSY() && GetItemCount() 
		&& !GetSelectedItemText().IsEmpty( ) 
		&& !MainFrame()->IsModlessUp()
		&& !MainFrame()->IsPendChgEditInProgress() );	
}

void CBranchListCtrl::OnUpdateBranchDescribe(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( OnUpdateShowMenuItem( pCmdUI, IDS_DESCRIBEIT_s ) );	
}

void CBranchListCtrl::OnUpdateViewUpdate(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY() && !MainFrame()->IsModlessUp());
}

void CBranchListCtrl::OnViewUpdate() 
{
	m_Active = GetSelectedItemText();
	SetCaption();
	CCmd_Branches *pCmd= new CCmd_Branches;
	pCmd->Init( m_hWnd, RUN_ASYNC);
	if( pCmd->Run( ) )
	{
		MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_BRANCH_LISTING) );
		MainFrame()->SetBranchUpdateTime(GetTickCount());
		Clear();
		CP4ListCtrl::OnViewUpdate();
	}
	else
		delete pCmd;
}

CString CBranchListCtrl::SetCaption()
{
	int i = GET_P4REGPTR()->GetBranchFilteredFlags();
	if (i)
    {
        CString filter;
		CString txt;

		if (i & 0x10)
			txt = LoadStringResource(IDS_CURRENT_USER);
		else if (i & 0x01)
			txt = GET_P4REGPTR()->GetBranchFilterOwner();
		filter.FormatMessage(IDS_FILTERED, txt);
		m_caption = LoadStringResource(IDS_PERFORCE_BRANCHES) + filter;
    }
	else
		m_caption = LoadStringResource(IDS_PERFORCE_BRANCHES);

	CP4PaneContent::GetView()->SetCaption();

	return m_caption;
}

void CBranchListCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	//		make sure window is active
	//
	GetParentFrame()->ActivateFrame();

	///////////////////////////////
	// See ContextMenuRules.txt for order of menu commands!

	//		create an empty context menu 
	//
	CP4Menu popMenu;
	popMenu.CreatePopupMenu();
	    
    int	index;
    SetIndexAndPoint( index, point );
    
	popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_BRANCH_NEW, LoadStringResource(ID_BRANCH_NEW) );
	if( index != -1 )
	{
		// Make sure the item gets selected
		SetItemState( index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
		
		// Then add menu commands that are relevant
		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_BRANCH_EDITSPEC, LoadStringResource(ID_BRANCH_EDITSPEC) );
		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_BRANCH_DESCRIBE, LoadStringResource(ID_BRANCH_DESCRIBE) );
		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_BRANCH_DELETE, LoadStringResource(ID_BRANCH_DELETE) );
		popMenu.AppendMenu( MF_SEPARATOR );
		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_BRANCH_INTEG, LoadStringResource(ID_BRANCH_INTEG) );
	}
	popMenu.AppendMenu( MF_SEPARATOR );
	popMenu.AppendMenu( MF_ENABLED | MF_STRING, ID_FILTERBRANCHBYOWNER, 
										LoadStringResource(IDS_FILTERBRANCHBYOWNER) );
	popMenu.AppendMenu( MF_ENABLED | MF_STRING, ID_CLEARBRANCHOWNERFILTER, 
										LoadStringResource(IDS_CLEARBRANCHOWNERFILTER) );
	popMenu.AppendMenu( MF_SEPARATOR );
	popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_VIEW_UPDATE, LoadStringResource(IDS_REFRESH) );

	MainFrame()->AddToolsToContextMenu(&popMenu);

	popMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,	point.x, point.y, AfxGetMainWnd());
}

void CBranchListCtrl::InsertBranch(CP4Branch *branch, int index)
{
	LV_ITEM lvItem;
	int iActualItem = 0;
	CString txt;
	m_iImage = CP4ViewImageList::VI_BRANCH;

	ASSERT(branch != NULL);
	BOOL bFilteredOut = MainFrame()->IsBranchFilteredOut(branch);
	
	for(int subItem=0; subItem < BRANCH_MAXCOL; subItem++)
	{
		lvItem.mask=LVIF_TEXT | 
					((subItem==0) ? LVIF_IMAGE : 0) |
					((subItem==0) ? LVIF_PARAM : 0);

		lvItem.iItem= (subItem==0) ? index : iActualItem;
        ASSERT(lvItem.iItem != -1 || bFilteredOut);

		lvItem.iSubItem= subItem;
		lvItem.iImage = CP4ViewImageList::VI_BRANCH;
		lvItem.lParam=(LPARAM) branch;

		switch(subItem)
		{
		case BRANCH_NAME: 
			lvItem.pszText= const_cast<LPTSTR>((LPCTSTR) branch->GetBranchName()); break;
		case BRANCH_OWNER: 
			lvItem.pszText = const_cast<LPTSTR>( (LPCTSTR) branch->GetOwner()); break;
		case BRANCH_OPTIONS: 
			lvItem.pszText = const_cast<LPTSTR>( (LPCTSTR) branch->GetOptions()); break;
		case BRANCH_UPDATEDATE:                              
			lvItem.pszText= const_cast<LPTSTR>((LPCTSTR) branch->GetDate()); break;
		case BRANCH_DESC: 
			txt= PadCRs(branch->GetDescription());
			lvItem.pszText = const_cast<LPTSTR>( (LPCTSTR) txt); break;
		default:
			ASSERT( 0 ); lvItem.pszText = _T("@"); break;
		}
			
		m_ListAll.column[subItem].SetAtGrow(index, lvItem.pszText);
		if (bFilteredOut)
			continue;

		if(subItem==0)
			iActualItem=InsertItem(&lvItem);
		else
			SetItem(&lvItem);
	}
	if (bFilteredOut)
		delete branch;
}


void CBranchListCtrl::UpdateBranch(CP4Branch *branch, int index)
{
	// After a spec edit, update the appropriate list item

	// First, switch the branch data
	CP4Branch *oldBranch= (CP4Branch *) GetItemData(index);
	delete oldBranch;
	SetItemData(index, (LPARAM) branch);

	// Then update the text
	SetItemText(index, BRANCH_NAME, const_cast<LPTSTR>((LPCTSTR) branch->GetBranchName()));
	SetItemText(index, BRANCH_OWNER, const_cast<LPTSTR>((LPCTSTR) branch->GetOwner()));
	SetItemText(index, BRANCH_OPTIONS, const_cast<LPTSTR>((LPCTSTR) branch->GetOptions()));
	SetItemText(index, BRANCH_UPDATEDATE, const_cast<LPTSTR>((LPCTSTR) branch->GetDate()));
	CString	txt= PadCRs(branch->GetDescription());
	SetItemText(index, BRANCH_DESC, const_cast<LPTSTR>((LPCTSTR)txt));
}

void CBranchListCtrl::UpdateBranchAll(CP4Branch *branch, int index)
{
	m_ListAll.column[BRANCH_NAME].SetAt(index, const_cast<LPTSTR>((LPCTSTR)branch->GetBranchName()));
	m_ListAll.column[BRANCH_OWNER].SetAt(index, const_cast<LPTSTR>((LPCTSTR)branch->GetOwner()));
	m_ListAll.column[BRANCH_OPTIONS].SetAt(index, const_cast<LPTSTR>((LPCTSTR)branch->GetOptions()));
	m_ListAll.column[BRANCH_UPDATEDATE].SetAt(index, const_cast<LPTSTR>((LPCTSTR)branch->GetDate()));
	CString	txt= PadCRs(branch->GetDescription());
	m_ListAll.column[BRANCH_DESC].SetAt(index, const_cast<LPTSTR>((LPCTSTR)txt));
}

///////////////////////////////////////////////////
// Support for request of branches list, including delayed notification to
// requesting window

// Return a list of branches if this window is up-to-date and lParam == 0,
// otherwise start a refresh of the branches list, and save the pointer of
// the window to be notified when the list is ready
LRESULT CBranchListCtrl::OnRequestBranchesList(WPARAM wParam, LPARAM lParam)
{
	CWnd *pWnd= (CWnd *) wParam;
	ASSERT_KINDOF(CWnd, pWnd);

	// Clear the branches list
	for( INT_PTR index= m_BranchesList.GetUpperBound(); index >= 0; index-- )
		delete m_BranchesList.GetAt( index );
	m_BranchesList.RemoveAll();

	if((m_UpdateState != LIST_UPDATED) || lParam)
	{
		// Note: OnViewUpdate() calls Clear(), which resets m_pNotifyWnd to NULL,
		//       so be sure to run OnViewUpdate() BEFORE setting the notify wnd
		OnViewUpdate();
		m_pNotifyWnd= pWnd;
	}
	else
	{
		FillBranchesList();
		m_pNotifyWnd= NULL;
	}

	// Return the branches list, even if it is still empty
	return (LRESULT) &m_BranchesList;
}

void CBranchListCtrl::FillBranchesList()
{
	ASSERT(m_BranchesList.GetSize() == 0);

	// Rummage through the list, adding branch names to stringlist
	for(int index=0; index < GetItemCount(); index++)
	{
		CP4Branch *branch= (CP4Branch *) GetItemData(index);
		
		CP4Branch *newBranch= new CP4Branch;

		newBranch->Create( branch->GetBranchName(), branch->GetOwner(), branch->GetOptions(),
													branch->GetDate(), branch->GetDescription() );
		m_BranchesList.Add( newBranch );
	}
	if (!m_Active.IsEmpty())
		GET_P4REGPTR()->SetLastBranch(m_Active);
}

void CBranchListCtrl::NotifyNewBranchesList()
{
	ASSERT(m_pNotifyWnd != NULL);

	FillBranchesList();
	try
	{
		if(m_pNotifyWnd != NULL && IsWindow(m_pNotifyWnd->m_hWnd))
		{
			m_pNotifyWnd->SendMessage(WM_NEWBRANCHESLIST, (WPARAM) &m_BranchesList, 0);
		}
	}
	catch(...)
	{
		ASSERT(0);
		// It is possible that the window and/or the CWnd object which
		// we were supposed to notify is no longer in existence, so this
		// is an error condition that we can sweep under the rug
	}
}


///////////////////////////////////////////////////
// Messages posted by server thread

LRESULT CBranchListCtrl::OnP4BranchSpec(WPARAM wParam, LPARAM lParam)
{
	CCmd_EditSpec *pCmd= (CCmd_EditSpec *) wParam;

	pCmd->SetIsRequestingNew(m_NewBranch);
	pCmd->SetCaller(DYNAMIC_DOWNCAST(CView, GetParent()));
	if(!pCmd->GetError() && !m_EditInProgress && pCmd->DoSpecDlg(this))
	{
		m_EditInProgress = TRUE;
		m_EditInProgressWnd = pCmd->GetSpecSheet();
	}
	else
	{
		if ( m_pNewSpec )
			delete m_pNewSpec;
		if (pCmd->HaveServerLock())
			pCmd->ReleaseServerLock();
		delete pCmd;
	}
	MainFrame()->ClearStatus();
	return 0;
}

LRESULT CBranchListCtrl::OnP4EndSpecEdit( WPARAM wParam, LPARAM lParam )
{
	BOOL need2Refresh = FALSE;
	CCmd_EditSpec *pCmd= (CCmd_EditSpec *) wParam;

	if (lParam != IDCANCEL && lParam != IDABORT)
	{
		if (m_UpdateState == LIST_UPDATED)
		{
			// we have to set 'index' again in case branch's name got changed
			int index= FindInList(m_pNewSpec->GetBranchName());
			int ixAll= FindInListAll(m_pNewSpec->GetBranchName());
			if (ixAll == -1)	// not in either list
			{
				ASSERT(index == -1);
				InsertBranch(m_pNewSpec, GetItemCount());
				ReSort();
				index= FindInList(m_pNewSpec->GetBranchName());
				if (index == -1)
					index= FindInList(m_Active);
			}
			else if (index > -1)	// in both lists
			{
				ASSERT(ixAll > -1);
				if (MainFrame()->IsBranchFilteredOut(m_pNewSpec))	// should it no longer be shown?
				{
					need2Refresh = TRUE;
					delete m_pNewSpec;
				}
				else
				{
					UpdateBranch(m_pNewSpec, index);
					UpdateBranchAll(m_pNewSpec, ixAll);
				}
			}
			else	// not in visible list; is in list of all
			{
				if (MainFrame()->IsBranchFilteredOut(m_pNewSpec))	// should it now be shown?
					need2Refresh = TRUE;
				else
					UpdateBranchAll(m_pNewSpec, ixAll);
				delete m_pNewSpec;
				index= FindInList(m_Active);
			}
			EnsureVisible( index, TRUE );
			SetItemState( index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
		}
		else
			if ( m_pNewSpec ) delete m_pNewSpec;
	}
	else
		if ( m_pNewSpec ) delete m_pNewSpec;

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

	if (need2Refresh)
		OnViewUpdate();
	return 0;
}

LRESULT CBranchListCtrl::OnP4BranchList(WPARAM wParam, LPARAM lParam)
{
	CCmd_Branches *pCmd= (CCmd_Branches *) wParam;
    ASSERT_KINDOF(CCmd_Branches,pCmd);

	if(!pCmd->GetError())
	{
    	CObList *branches= pCmd->GetList();

        SetRedraw(FALSE);
    	int index = 0;
		for(POSITION pos= branches->GetHeadPosition(); pos != NULL; index++)
		{
	        CP4Branch *branch=(CP4Branch *) branches->GetNext(pos);
            ASSERT_KINDOF(CP4Branch,branch);
			InsertBranch(branch, index);
		}
        SetRedraw(TRUE);

		CString msg;
        msg.FormatMessage( IDS_NUMBER_OF_BRANCHES_n, index );
		AddToStatus( msg, SV_COMPLETION );

		ReSort();

		// Make sure previous item is re-selected
		if(branches->GetCount() > 0)
		{
			int i = FindInList(m_Active.IsEmpty() ? GET_P4REGPTR()->GetLastBranch() : m_Active);
			if (i < 0)	i=0;
			SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			EnsureVisible(i, FALSE);
			m_Active=GetSelectedItemText();
		}

		// If another window was waiting for a list of branches, send it the list
		if(m_pNotifyWnd != NULL)
			NotifyNewBranchesList();
	
		CP4ListCtrl::SetUpdateDone();
		if (m_Need2DoNew)
			OnBranchNew();
		else
		{
			// Notify the mainframe that we have finished getting the branches,
			// hence the entire set of async command have finished.
			MainFrame()->ExpandDepotIfNeedBe();
		}
		if (m_PostViewUpdateMsg)
			PostMessage(m_PostViewUpdateMsg, m_PostViewUpdateWParam, m_PostViewUpdateLParam);
	}
	else
	{
		CP4ListCtrl::SetUpdateFailed();
		m_Need2DoNew = FALSE;
	}
	
	delete pCmd;
	m_PostViewUpdateMsg = 0;
	MainFrame()->ClearStatus();
	return 0;
}


//////////////////////////////////////////////////////////////////////////
// Sort callback, not in class

int CBranchListCtrl::OnCompareItems(LPARAM lParam1, LPARAM lParam2, int subItem)
{
    ASSERT(lParam1 && lParam2);
    CP4Branch const *branch1 = (CP4Branch const*)lParam1;
    CP4Branch const *branch2 = (CP4Branch const*)lParam2;
    ASSERT_KINDOF(CP4Branch,branch1);
    ASSERT_KINDOF(CP4Branch,branch2);


	CString txt1, txt2;
	switch(subItem)
	{
	case BRANCH_NAME:	 // branch name
		txt1= branch1->GetBranchName();
		txt2= branch2->GetBranchName();
		break;

	case BRANCH_OWNER:	 // branch owner
		txt1= branch1->GetOwner();
		txt2= branch2->GetOwner();
		break;

	case BRANCH_OPTIONS:	 // branch options
		txt1= branch1->GetOptions();
		txt2= branch2->GetOptions();
		break;

	case BRANCH_UPDATEDATE:	 // branch update date
		txt1= branch1->GetDate();
		txt2= branch2->GetDate();
		ConvertDates( txt1, txt2 );
		break;

	case BRANCH_DESC:	 // branch root
		txt1= branch1->GetDescription();
		txt2= branch2->GetDescription();
		break;

	default:
		ASSERT(0);
		return 0;
	}
	txt1.MakeUpper();
	txt2.MakeUpper();

	int rc;

	if(m_SortAscending)
		rc = txt1.Compare(txt2);
	else
		rc = txt2.Compare(txt1);

	return rc;
}

void CBranchListCtrl::OnEditSpec( LPCTSTR sItem )
{
	if (m_EditInProgress)
	{
		CantEditRightNow(IDS_BRANCH);
		return;
	}

	m_pNewSpec = new CP4Branch;

	CCmd_EditSpec *pCmd= new CCmd_EditSpec;
	pCmd->Init( m_hWnd, RUN_ASYNC, TRUE );
	if( pCmd->Run( P4BRANCH_SPEC, sItem, m_pNewSpec ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_EDITING_BRANCH_SPEC) ) ;
	else
	{
		delete pCmd;
		delete m_pNewSpec;
	}
}

int CBranchListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CP4ListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	CStringArray colnames;
	colnames.Add ( LoadStringResource(IDS_BRANCH) );
	colnames.Add ( LoadStringResource(IDS_OWNER) );
	colnames.Add ( LoadStringResource(IDS_LOCKSTR) );
	colnames.Add ( LoadStringResource(IDS_DATE) );
	colnames.Add ( LoadStringResource(IDS_DESCRIPTION) );
	ASSERT( BRANCH_MAXCOL == colnames.GetSize( ) );
    //FIXTHIS: move default widths into resource
	int width[BRANCH_MAXCOL]={90,90,60,90,250};
	RestoreSavedWidths(width, static_cast<int>(colnames.GetSize()), sRegKey_ColumnNames);
	if (width[BRANCH_UPDATEDATE] > 5000 && width[BRANCH_DESC] > 5000)
		width[BRANCH_OPTIONS] = width[BRANCH_UPDATEDATE] = width[BRANCH_DESC] = 90;
	InsertColumnHeaders( colnames, width );

	return 0;
}

void CBranchListCtrl::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CP4ListCtrl::OnActivate(nState, pWndOther, bMinimized);

	if(nState != WA_INACTIVE && m_Active.IsEmpty() && GetItemCount() > 1)
	{
		int i = FindInList(GET_P4REGPTR()->GetLastBranch());
		if (i < 0)	i=0;
		SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		EnsureVisible(i, FALSE);
		m_Active=GetSelectedItemText();
	}
	else
	{
		m_Active=GetSelectedItemText();
		if (!m_Active.IsEmpty())
			GET_P4REGPTR()->SetLastBranch(m_Active);
	}
}

void CBranchListCtrl::OnUpdateFilterByOwner(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY()) );
}

void CBranchListCtrl::OnFilterByOwner()
{
	CFilterByOwnerDlg dlg;
	dlg.m_NotUser = GET_P4REGPTR()->GetBranchFilteredFlags() & 0x01;
	dlg.m_Owner = GET_P4REGPTR()->GetBranchFilterOwner();
	dlg.m_bShowIncBlanks = FALSE;
	if (dlg.DoModal() == IDOK)
	{
		GET_P4REGPTR()->SetBranchFilteredFlags(dlg.m_NotUser ? 0x01 : 0x10);
		GET_P4REGPTR()->SetBranchFilterOwner(dlg.m_Owner);
		OnViewUpdate();
	}
}

void CBranchListCtrl::OnUpdateClearFilterByOwner(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( MainFrame()->SetMenuIcon(pCmdUI, GET_P4REGPTR()->GetBranchFilteredFlags()
														&& !SERVER_BUSY()) );
}

void CBranchListCtrl::OnClearFilterByOwner()
{
	GET_P4REGPTR()->SetBranchFilteredFlags(0);
	OnViewUpdate();
}
