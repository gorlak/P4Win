// UserListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "UserView.h"
#include "MainFrm.h"
#include "cmd_editspec.h"
#include "cmd_describe.h"
#include "cmd_delete.h"
#include "cmd_users.h"
#include "newclientdlg.h"
#include "setpwddlg.h"
#include "ImageList.h"
#include <afxpriv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static LPCTSTR sRegValue_UserList = _T("User List");

enum UserSubItem
{
    USER_NAME,
    USER_EMAIL,
    USER_FULLNAME,
    USER_DATEACCESS,
    USER_MAXCOL
};

/////////////////////////////////////////////////////////////////////////////
// CUserListCtrl

BEGIN_MESSAGE_MAP(CUserListCtrl, CP4ListCtrl)
	ON_COMMAND(ID_USER_DELETE, OnUserDelete)
	ON_UPDATE_COMMAND_UI(ID_USER_DELETE, OnUpdateUserDelete)
	ON_UPDATE_COMMAND_UI(ID_VIEW_UPDATE_RIGHT, OnUpdateViewUpdate)
	ON_WM_CONTEXTMENU()
	ON_UPDATE_COMMAND_UI(ID_USER_DESCRIBE, OnUpdateUserDescribe)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_USER_CREATENEWUSER, OnUserCreatenewuser)
	ON_UPDATE_COMMAND_UI(ID_USER_SWITCHTOUSER, OnUpdateUserSwitchtouser)
	ON_COMMAND(ID_USER_SWITCHTOUSER, OnUserSwitchtouser)
	ON_UPDATE_COMMAND_UI(ID_SETDEFUSER, OnUpdateSetDefUser)
	ON_COMMAND(ID_SETDEFUSER, OnSetDefUser)
	ON_UPDATE_COMMAND_UI(ID_USER_PASSWORD, OnUpdateUserPassword)
	ON_WM_CREATE()
	ON_COMMAND(ID_USER_DESCRIBE, OnDescribe)
	ON_COMMAND(ID_VIEW_UPDATE_RIGHT, OnViewUpdate)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnDeleteitem)
	ON_COMMAND(ID_USER_PASSWORD, OnUserPassword)
	ON_UPDATE_COMMAND_UI(ID_ADD_REVIEWS, OnUpdateAddReviews)
	ON_COMMAND(ID_ADD_REVIEWS, OnAddReviews)
	ON_MESSAGE(WM_P4USERS, OnP4UserList )
	ON_MESSAGE(WM_P4EDITSPEC, OnP4UserSpec )
	ON_MESSAGE(WM_P4ENDSPECEDIT, OnP4EndSpecEdit )
	ON_MESSAGE(WM_P4DELETE, OnP4Delete )
    ON_MESSAGE(WM_P4DESCRIBE, OnP4Describe )
    ON_MESSAGE(WM_P4ENDDESCRIBE, OnP4EndDescribe )
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CUserListCtrl, CP4ListCtrl)

CUserListCtrl::CUserListCtrl()
{
    m_SortAscending=TRUE;
    m_viewType = P4USER_SPEC;
	m_OldDefUser = m_OldCurUser = _T("@");

    m_CF_DEPOT = static_cast<CLIPFORMAT>(RegisterClipboardFormat(LoadStringResource(IDS_DRAGFROMDEPOT)));
	m_CF_USER  = static_cast<CLIPFORMAT>(RegisterClipboardFormat(LoadStringResource(IDS_DRAGFROMUSER)));
	m_caption  = m_captionplain = LoadStringResource(IDS_PERFORCE_USERS);
}

CUserListCtrl::~CUserListCtrl()
{
}

/////////////////////////////////////////////////////////////////////////////
// CUserListCtrl diagnostics

#ifdef _DEBUG
void CUserListCtrl::AssertValid() const
{
	CP4ListCtrl::AssertValid();
}

void CUserListCtrl::Dump(CDumpContext& dc) const
{
	CP4ListCtrl::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CUserListCtrl message handlers

void CUserListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	int index = GetHitItem ( point );
	if( index > -1 )
	{
		SetItemState(index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		OnDescribe( );
	}
	else
		CP4ListCtrl::OnLButtonDblClk(nFlags, point);
}


void CUserListCtrl::Clear() 
{
    SetRedraw(FALSE);
	DeleteAllItems();
	SetRedraw(TRUE);

	CP4ListCtrl::Clear();
}


void CUserListCtrl::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	delete (CP4User *) GetItemData(pNMListView->iItem);
			
	*pResult = 0;
}


/*
	_________________________________________________________________

	everything you need for a delete, the cmd ui, the first part of
	the delete, that sends a window message to this window, that comes
	right back and calls the second function for a delete.
	_________________________________________________________________
*/

void CUserListCtrl::OnUpdateUserDelete(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( OnUpdateShowMenuItem( pCmdUI, IDS_DELETE_s )
			&& GetSelectedItemText() == GET_P4REGPTR()->GetP4User()
			&& !MainFrame()->IsModlessUp() );
}

BOOL CUserListCtrl::OKToDelete( ) 
{
	if ( GetSelectedItemText() == GET_P4REGPTR()->GetP4User() )
		return TRUE;	
	else
	{
		AfxMessageBox(IDS_YOU_DO_NOT_HAVE_PERMISSION_TO_DELETE_OTHER_USERS, MB_ICONINFORMATION);
		return FALSE;
	}
}

void CUserListCtrl::OnUserDelete() 
{
	OnDelete( P4USER_DEL );
}

void CUserListCtrl::EditTheSpec(CString *name)
{
	if ( *name == GET_P4REGPTR( )->GetP4User( ) )
		OnUserEditmy();
}

void CUserListCtrl::OnUserEditmy() 
{
	m_Active = GET_P4REGPTR()->GetP4User();
	
	int index = FindInList(GET_P4REGPTR()->GetP4User());
	if(index > -1)
	{
		SetItemState(index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		EnsureVisible(index, FALSE);
	}

    m_olduser= m_Active;
	OnEditSpec( m_Active );
}


void CUserListCtrl::OnUpdateUserDescribe(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( OnUpdateShowMenuItem( pCmdUI, IDS_DESCRIBEIT_s )
					&& !MainFrame()->IsModlessUp() );	
}

void CUserListCtrl::OnUserDescribe() 
{
	OnDescribe();
}

void CUserListCtrl::OnUpdateViewUpdate(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY() && !MainFrame()->IsModlessUp());
}

void CUserListCtrl::OnViewUpdate() 
{	
	m_Active = GetSelectedItemText();
	CCmd_Users *pCmd= new CCmd_Users;
	pCmd->Init( m_hWnd, RUN_ASYNC);
	if( pCmd->Run( ) )
	{
        MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_USER_LISTING) );
		MainFrame()->SetUserUpdateTime(GetTickCount());
		Clear();
		CP4ListCtrl::OnViewUpdate();
	}
	else
		delete pCmd;
}


void CUserListCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	//		make sure window is active
	//
	GetParentFrame()->ActivateFrame();


	///////////////////////////////
	// See ContextMenuRules.txt for order of menu commands!

	//		create an empty context menu
	//
	CP4Menu popMenu;
    popMenu.LoadMenu(IDR_USER);

	int	index;
    SetIndexAndPoint( index, point );

	if( index != -1 )
	{
		// Can only edit or delete my user
		if ( GetSelectedItemText( ) == GET_P4REGPTR( )->GetP4User( ) )
		{
            // can't switch if already there
            popMenu.GetSubMenu(0)->DeleteMenu(ID_USER_SWITCHTOUSER,MF_BYCOMMAND);
        }
        else
        {
            // can't edit, delete, set password or set as default if not current
            popMenu.GetSubMenu(0)->DeleteMenu(ID_USER_EDITMY,MF_BYCOMMAND);
            popMenu.GetSubMenu(0)->DeleteMenu(ID_USER_DELETE,MF_BYCOMMAND);
            popMenu.GetSubMenu(0)->DeleteMenu(ID_USER_PASSWORD,MF_BYCOMMAND);
            popMenu.GetSubMenu(0)->DeleteMenu(ID_SETDEFUSER,MF_BYCOMMAND);
        }
	}
    else
    {
        // can't do much if no user selected
        popMenu.GetSubMenu(0)->DeleteMenu(ID_USER_EDITMY,MF_BYCOMMAND);
        popMenu.GetSubMenu(0)->DeleteMenu(ID_USER_DESCRIBE,MF_BYCOMMAND);
        popMenu.GetSubMenu(0)->DeleteMenu(ID_USER_DELETE,MF_BYCOMMAND);
        popMenu.GetSubMenu(0)->DeleteMenu(ID_USER_SWITCHTOUSER,MF_BYCOMMAND);
        popMenu.GetSubMenu(0)->DeleteMenu(ID_USER_PASSWORD,MF_BYCOMMAND);
        popMenu.GetSubMenu(0)->DeleteMenu(ID_SETDEFUSER,MF_BYCOMMAND);
        // clobber extra separator
        popMenu.GetSubMenu(0)->DeleteMenu(1,MF_BYPOSITION);

    }

	MainFrame()->AddToolsToContextMenu((CP4Menu *)(popMenu.GetSubMenu(0)));

	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,	point.x, point.y, AfxGetMainWnd());
}


/////////////////////////////////////////////////////////////////////////////
// CUserListCtrl message handlers

void CUserListCtrl::InsertUser(CP4User *user, int index)
{
	ASSERT(user != NULL);
	m_iImage = CP4ViewImageList::VI_USER;

	LV_ITEM lvItem;
	int iActualItem = -1;
	CString curuser = GET_P4REGPTR()->GetP4User();
	CString defuser = GET_P4REGPTR()->GetP4User(TRUE);

	for(int subItem=USER_NAME; subItem < USER_MAXCOL; subItem++)
	{
        lvItem.mask=LVIF_TEXT | 
                    ((subItem==USER_NAME) ? LVIF_IMAGE : 0) |
                    ((subItem==USER_NAME) ? LVIF_PARAM : 0);

        lvItem.iItem= (subItem==USER_NAME) ? index : iActualItem;
        ASSERT(lvItem.iItem != -1);

        lvItem.iSubItem= subItem;

		switch(subItem)
		{
		case USER_NAME: 
			{
			lvItem.pszText= const_cast<LPTSTR>(user->GetUserName());
			bool isCurrent = curuser.Compare(lvItem.pszText) == 0;
			bool isDefault = defuser.Compare(lvItem.pszText) == 0;
			lvItem.iImage = CP4ViewImageList::GetUserIndex(isCurrent, isDefault);
			if(isDefault)
				m_OldDefUser = lvItem.pszText;
			if(isCurrent)
				m_OldCurUser = lvItem.pszText;
	        lvItem.lParam=(LPARAM) user;
			}
			break;
		case USER_EMAIL: 
			lvItem.pszText= const_cast<LPTSTR>(user->GetEmail()); break;
		case USER_FULLNAME: 
			lvItem.pszText= const_cast<LPTSTR>(user->GetFullName()); break;
		case USER_DATEACCESS: 
			lvItem.pszText= const_cast<LPTSTR>(user->GetLastAccess()); break;
		}
			
		if(subItem==0)
			iActualItem = InsertItem(&lvItem);
		else
			SetItem(&lvItem);
	}
}


void CUserListCtrl::UpdateUser(CP4User *user, int index)
{
	// After a spec edit, update the appropriate list item

	// First, switch the user data
	CP4User *oldUser= (CP4User *) GetItemData(index);
	delete oldUser;
	SetItemData(index, (LPARAM) user);

	// Then update the text
	SetItemText(index, USER_NAME, const_cast<LPTSTR>(user->GetUserName()));
	SetItemText(index, USER_EMAIL, const_cast<LPTSTR>(user->GetEmail()));
	SetItemText(index, USER_FULLNAME, const_cast<LPTSTR>(user->GetFullName()));
	SetItemText(index, USER_DATEACCESS, const_cast<LPTSTR>(user->GetLastAccess()));
}


// Receives ak for user spec update
LRESULT CUserListCtrl::OnP4UserSpec(WPARAM wParam, LPARAM lParam)
{
	CCmd_EditSpec *pCmd= (CCmd_EditSpec *) wParam;

	BOOL newUser;				        // Save whether an edit operation refer to a new user
	pCmd->SetIsRequestingNew(newUser = pCmd->GetIsNewUser());
	pCmd->SetCaller(DYNAMIC_DOWNCAST(CView, GetParent()));

	int i, j;

    CString specIn(pCmd->GetSpecIn());
    if (m_ReviewsList.GetCount() > 0)	// We have files to add to the Reviews list
	{
		if ((i = specIn.Find(_T("\n\nReviews:\n"))) == -1)
		{
			i = specIn.GetLength();
			specIn += _T("\n\nReviews:\n");
		}
		else
			specIn += _T("\t#\n");	//	A review that is a single # means
										//	put a blank line in the list here.
		POSITION pos;
		for(pos = m_ReviewsList.GetHeadPosition(); pos != NULL; )
		{
		    CString filename = m_ReviewsList.GetNext(pos);
			if ((i = filename.Find(_T("<contains no files or folders>"))) != -1)
				filename = filename.Left(i-1) + _T("/...");
			if (filename.Find(_T(' ')) != -1)
				filename = _T('\"') + filename + _T('\"');
			if (specIn.Find(filename) == -1)
				specIn += _T('\t') + filename + _T('\n');
		}
		m_ReviewsList.RemoveAll();		// this info is no longer needed
        pCmd->SetSpecIn(specIn);
	}
	m_oldJobView = _T("");
	if ((i = specIn.Find(_T("\n\nJobView:\t"))) != -1)
	{
		i += lstrlen(_T("\n\nJobView:\n"));
		if ((j = specIn.Find(_T('\n'),i)) != -1)
			m_oldJobView = specIn.Mid(i, j-i);
	}
	if(!pCmd->GetError() && !m_EditInProgress && pCmd->DoSpecDlg(this))
	{
		m_EditInProgress = TRUE;
		m_EditInProgressWnd = pCmd->GetSpecSheet();
	}
	else
	{
		delete m_pNewSpec;
		CString txt;
		txt.FormatMessage(IDS_A_USER_HAS_BEEN_CREATED_DELETE_IT_s, m_Active);
		if (newUser && (FindInList(m_Active) == -1) 
					&& (CString(pCmd->GetErrorText()).Find(_T(" - over license quota")) == -1)
			        && (pCmd->GetError() || (IDYES == AfxMessageBox(txt, 
												MB_YESNO | MB_DEFBUTTON1 | MB_ICONQUESTION))))
		{
			CCmd_Delete *pCmd2 = new CCmd_Delete;
			pCmd2->Init( NULL, RUN_SYNC, TRUE, pCmd->GetServerKey() );
			if (pCmd2->Run( P4USER_DEL, m_Active ))
			{
				// nothing to do
			}
			else
			{
				::PostMessage(MainFrame()->m_hWnd, WM_COMMAND, ID_VIEW_UPDATE, 0);
			}
			delete pCmd2;
		}
		else
			::PostMessage(MainFrame()->m_hWnd, WM_COMMAND, ID_VIEW_UPDATE, 0);
		if (GET_P4REGPTR()->GetExpandFlag() == 1)
			GET_P4REGPTR()->AddMRUPcuPath(MainFrame()->GetCurrentItemPath());
		m_Active = m_olduser;
		GET_P4REGPTR()->SetP4User( m_olduser, TRUE, FALSE, FALSE );
		MainFrame()->UpdateCaption( ) ;	

		if (pCmd->HaveServerLock())
			pCmd->ReleaseServerLock();
		delete pCmd;
	}
	
	MainFrame()->ClearStatus();
	return 0;
}

LRESULT CUserListCtrl::OnP4EndSpecEdit( WPARAM wParam, LPARAM lParam )
{
	CCmd_EditSpec *pCmd= (CCmd_EditSpec *) wParam;
	int i, j;

	if (lParam != IDCANCEL && lParam != IDABORT)
	{
		if (m_UpdateState == LIST_UPDATED)
		{
			// we have to set 'index' again in case user's name got changed
			int index = FindInList(m_pNewSpec->GetUserName());
			if(index > -1 )
				UpdateUser(m_pNewSpec, index);
			else
			{
				InsertUser(m_pNewSpec, GetItemCount());
				ReSort();

				if( m_Active != m_olduser )
				{
					if (GET_P4REGPTR()->GetExpandFlag() == 1)
						GET_P4REGPTR()->AddMRUPcuPath(MainFrame()->GetCurrentItemPath());
					m_Active = m_olduser;
					GET_P4REGPTR()->SetP4User( m_olduser, TRUE, FALSE, FALSE );
					MainFrame()->UpdateCaption( ) ;	
				}
			}
		}
		else
			if ( m_pNewSpec ) delete m_pNewSpec;

	    CString specOut(pCmd->GetSpecOut());
		if ((i = specOut.Find(_T("\n\nJobView:\t"))) != -1)
		{
			i += lstrlen(_T("\n\nJobView:\n"));
			if ((j = specOut.Find(_T('\n'),i)) != -1)
			{
				// if JobView has changed, we have to refresh the changelists (and depot)
				if (m_oldJobView != specOut.Mid(i, j-i))
					MainFrame()->UpdateDepotandChangeViews(TRUE);
			}
		}
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

	if ((lParam == IDCANCEL) && pCmd->GetIsNewUser())	// if canceled, cleanup newly created user
	{
		m_Active = pCmd->GetItemName();
		CString msg;
		msg.FormatMessage ( IDS_DELETENEWUSER_s, m_Active );
		// ask if they want to delete the newly created user
		if( AfxMessageBox( msg, MB_YESNO|MB_ICONQUESTION ) == IDYES)
		{
			// fire off the delete of the newly created user
			CCmd_Delete *pCmdDel = new CCmd_Delete;
			pCmdDel->Init( m_hWnd, RUN_ASYNC );
			pCmdDel->SetSwitch2User( m_olduser );
			if( pCmdDel->Run( P4USER_DEL, m_Active ) )
				MainFrame()->UpdateStatus( LoadStringResource(IDS_DELETING) );
			else
				delete pCmdDel;
		}
		else
		{
			OnViewUpdate();
		}
	}

	delete pCmd;
	m_EditInProgress = FALSE;
	return 0;
}

LRESULT CUserListCtrl::OnP4UserList(WPARAM wParam, LPARAM lParam)
{
	CCmd_Users *pCmd= (CCmd_Users *) wParam;

	if(!pCmd->GetError())
	{
	    CObList const *users = pCmd->GetList();

        SetRedraw(FALSE);
    	int index = 0;
		for(POSITION pos= users->GetHeadPosition(); pos != NULL; index++)
		{
        	CP4User *user = (CP4User *) users->GetNext(pos);
			InsertUser(user, index);
		}
        SetRedraw(TRUE);
		
		CString msg;
		msg.FormatMessage( IDS_NUMBER_OF_USERS_n, index );
		AddToStatus( msg, SV_COMPLETION );

		ReSort();

		// Make sure previous item is re-selected
		if(users->GetCount() > 0)
		{
			int i = FindInList(m_Active.IsEmpty() 
				? GET_P4REGPTR()->GetP4User() : m_Active);
			if (i < 0)	
				i=0;
			SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, 
				LVIS_SELECTED|LVIS_FOCUSED);
			EnsureVisible(i, FALSE);
			// If m_Active is empty but we found the current user in the list,
			// then set m_Active to the correct value
			if (m_Active.IsEmpty() && i >= 0)
				m_Active = GET_P4REGPTR()->GetP4User();
		}

		CP4ListCtrl::SetUpdateDone();
		// Notify the mainframe that we have finished getting the users,
		// hence the entire set of async command have finished.
		MainFrame()->ExpandDepotIfNeedBe();
		if (m_PostViewUpdateMsg)
			PostMessage(m_PostViewUpdateMsg, m_PostViewUpdateWParam, m_PostViewUpdateLParam);
	}
	else
		CP4ListCtrl::SetUpdateFailed();
	
		
	delete pCmd;
	m_PostViewUpdateMsg = 0;
	MainFrame()->ClearStatus();
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// Sort callback


int CUserListCtrl::OnCompareItems(LPARAM lParam1, LPARAM lParam2, int subItem)
{
    ASSERT(lParam1 && lParam2);
    CP4User const *user1 = (CP4User const*)lParam1;
    CP4User const *user2 = (CP4User const*)lParam2;

	CString txt1, txt2;
	switch(subItem)
	{
	case USER_NAME:	
		txt1= user1->GetUserName();
		txt2= user2->GetUserName();
		break;

	case USER_EMAIL:
		txt1= user1->GetEmail();
		txt2= user2->GetEmail();
		break;

	case USER_FULLNAME:
		txt1= user1->GetFullName();
		txt2= user2->GetFullName();
		break;

	case USER_DATEACCESS:
		txt1= user1->GetLastAccess();
		txt2= user2->GetLastAccess();
		ConvertDates( txt1, txt2 );
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

void CUserListCtrl::OnUserCreatenewuser() 
{
	if (m_EditInProgress)
	{
		CantEditRightNow(IDS_USER);
		return;
	}

	MainFrame()->ViewUsers();
	m_olduser = GET_P4REGPTR()->GetP4User( );

	//		let user type in the new name. if it's blank the user bailed.
	//

	CNewClientDlg newdlg;
	newdlg.SetNew( NEWUSER );
	if (FindInList(m_Active) != -1)
		newdlg.m_Active = m_Active;
	if( newdlg.DoModal( ) == IDCANCEL )
		return;

	CString saveActive = m_Active;
	m_Active = newdlg.GetName( ) ;
	if ( m_Active.IsEmpty( ) )
	{
		m_Active = saveActive;
		return;
	}
	if (FindInListNoCase(m_Active) != -1)
	{
		CString msg;
		UINT nType;
		if (FindInList(m_Active) != -1)
		{
			msg.FormatMessage ( IDS_USER_s_ALREADY_EXIST, m_Active );
			nType = MB_OK;
		}
		else
		{
			msg.FormatMessage ( IDS_USER_s_DIFFCASE_EXIST, m_Active );
			nType = MB_YESNO;
		}
		if (IDYES != AfxMessageBox( msg, nType ))
		{
			m_Active = saveActive;
			return;
		}
	}

    if ( SetP4User( ) ) 
		OnEditSpec( m_Active, TRUE );
}


BOOL CUserListCtrl::SetP4User( )
{
	//		not that we'd ever get here... but if they are the same, there 
	//		is nothing to do, so bail.
	//
	if ( m_Active == GET_P4REGPTR()->GetP4User() ) 
		return FALSE;

	if (GET_P4REGPTR()->GetExpandFlag() == 1)
		GET_P4REGPTR()->AddMRUPcuPath(MainFrame()->GetCurrentItemPath());

	//		okay, change the session's user. 
	//		(either we're setting the active user back to the registered one 
	//		after using another user, or we are officially resetting 
	//		the registered user to the new active one.)
	//

	if( !GET_P4REGPTR()->SetP4User( m_Active, TRUE, FALSE, FALSE ) )
	{
		AfxMessageBox( IDS_UNABLE_TO_WRITE_P4USER_TO_THE_REGISTRY, MB_ICONEXCLAMATION);
		m_Active  = GET_P4REGPTR()->GetP4User();
		return FALSE;
	}

    MainFrame()->UpdateCaption( ) ;	

	return TRUE;
}


void CUserListCtrl::OnUpdateUserSwitchtouser(CCmdUI* pCmdUI) 
{
	CString selUser = GetSelectedItemText();
    CString prompt;
    prompt.FormatMessage(IDS_SWITCH_TO_s, TruncateString(selUser, 50));
	pCmdUI->SetText ( prompt );
	pCmdUI->Enable( !SERVER_BUSY() 
					&& !selUser.IsEmpty() 
					&& selUser != GET_P4REGPTR()->GetP4User( )
					&& !MainFrame()->IsModlessUp() );
}

void CUserListCtrl::OnUserSwitchtouser() 
{
	m_Active = GetSelectedItemText();

	if ( SetP4User( ) )
		// We just added a user, so make sure the depot and changes
		// lists are updated
		MainFrame()->OnPerforceOptions( FALSE ) ;
}

void CUserListCtrl::OnUpdateSetDefUser(CCmdUI* pCmdUI) 
{
	CString selUser = GetSelectedItemText();
	pCmdUI->SetText ( LoadStringResource(IDS_SET_DEFAULT_USER_TO) + TruncateString(selUser, 50) );
	pCmdUI->Enable( !SERVER_BUSY() 
					&& !selUser.IsEmpty()
					&& selUser == GET_P4REGPTR()->GetP4User( FALSE )
					&& selUser != GET_P4REGPTR()->GetP4User( TRUE )
					&& !MainFrame()->IsModlessUp() );
}

void CUserListCtrl::OnSetDefUser() 
{
	if (m_Active != GetSelectedItemText())
	{
		ASSERT(0);
		return;
	}

	if (GET_P4REGPTR()->GetExpandFlag() == 1)
		GET_P4REGPTR()->AddMRUPcuPath(MainFrame()->GetCurrentItemPath());

	if( !GET_P4REGPTR()->SetP4User( m_Active, TRUE, TRUE, TRUE ) )
	{
		AfxMessageBox( IDS_UNABLE_TO_WRITE_P4USER_TO_THE_REGISTRY, MB_ICONEXCLAMATION);
		m_Active = GetSelectedItemText();
	}
	else
    {
        CString txt;
        txt.FormatMessage(IDS_DEFAULT_USER_SET_TO_s, m_Active);
		AddToStatus( txt );  
    }
}

void CUserListCtrl::OnEditSpec( LPCTSTR sItem, BOOL bNew/*=FALSE*/ )
{
	if (m_EditInProgress)
	{
		CantEditRightNow(IDS_USER);
		return;
	}

	m_pNewSpec = new CP4User;
	
	CCmd_EditSpec *pCmd = new CCmd_EditSpec;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK );
	pCmd->SetIsNewUser(bNew);
	if( pCmd->Run( P4USER_SPEC, sItem, m_pNewSpec ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_EDITING_USER_SPEC) ) ;
	else
	{
		delete pCmd;
		delete m_pNewSpec;
	}
}

void CUserListCtrl::OnUpdateUserPassword(CCmdUI* pCmdUI) 
{
	OnUpdateUserPassword( pCmdUI, TruncateString(GET_P4REGPTR()->GetP4User(), 50) );
}

void CUserListCtrl::OnUpdateUserPassword(CCmdUI* pCmdUI, LPCTSTR userName) 
{
    CString txt;
    txt.FormatMessage(IDS_SET_PASSWORD_FOR_s, TruncateString(userName, 50));
	pCmdUI->SetText ( txt );
	pCmdUI->Enable( !SERVER_BUSY() && GET_SERVERLEVEL() >= 6 
					&& lstrlen(userName)
					&& !MainFrame()->IsModlessUp());
}

void CUserListCtrl::OnUserPassword() 
{
	if(	GET_SERVERLEVEL() < 6 || !lstrlen(GET_P4REGPTR()->GetP4User()) )
		{ ASSERT(0); return; }
	
	OnUserPasswordDlg(FALSE, NULL);
}

int CUserListCtrl::OnUserPasswordDlg(BOOL bLogin, int key) 
{
	CSetPwdDlg dlg;
	dlg.m_bLogin = bLogin;
	dlg.m_Key = key;
	if (bLogin)
		dlg.m_Caption = LoadStringResource(IDS_MUST_SET_PASSWORD);
	return dlg.DoModal();
}


/////////////////////////////////////////////////////////////////////
// OLE drag-drop support, to accept depot files or folders
// which will define a view to be used to filter the submitted
// changes that this window displays
/////////////////////////////////////////////////////////////////////

DROPEFFECT CUserListCtrl::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	m_DropEffect=DROPEFFECT_NONE;
	m_DragDataFormat=0;

	// Dont allow a drop if the server is busy, since a drop immediately attempts to
	// invoke a server command
	// Also don't allow a drop if we are in local syntax because
	// the reviews should be specified in depot syntax
	if(SERVER_BUSY() || m_EditInProgress || GET_P4REGPTR( )->ShowEntireDepot( ) > SDF_DEPOT)
		return DROPEFFECT_NONE;
		
	if(pDataObject->IsDataAvailable( m_CF_DEPOT))
	{
		m_DropEffect=DROPEFFECT_COPY;
		m_DragDataFormat=m_CF_DEPOT;
	}
	
	return m_DropEffect;
}

DROPEFFECT CUserListCtrl::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	// Dont allow a drop if the server is busy, since a drop immediately attempts to
	// invoke a server command
	if(SERVER_BUSY() || m_EditInProgress)
		m_DropEffect= DROPEFFECT_NONE;
		
	return m_DropEffect;
}

BOOL CUserListCtrl::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	if(SERVER_BUSY() || m_EditInProgress)
	{
		// OnDragEnter() and OnDragOver() should avoid a drop at 
		// the wrong time!
		ASSERT(0);
		return FALSE;
	}
	
	if(m_DragDataFormat == m_CF_DEPOT)
	{
		ClientToScreen(&point);
		::SendMessage(m_depotWnd, WM_DROPTARGET, USERVIEW, MAKELPARAM(point.x,point.y));
		return TRUE;
	}
	// Return false, so depot window doesnt start a file-open operation
	return FALSE;
}

int CUserListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CP4ListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CStringArray colnames;
	colnames.Add ( LoadStringResource(IDS_NAME) );
	colnames.Add ( LoadStringResource(IDS_E_MAIL) );
	colnames.Add ( LoadStringResource(IDS_FULL_NAME) );
	colnames.Add ( LoadStringResource(IDS_ACCESS) );
	ASSERT( USER_MAXCOL == colnames.GetSize( ) );
	int width[ USER_MAXCOL ]={90,150,150,90};
	RestoreSavedWidths( width, colnames.GetSize( ), sRegValue_UserList );
	InsertColumnHeaders( colnames, width );

	return 0;
}

void CUserListCtrl::OnUpdateAddReviews(CCmdUI* pCmdUI) 
{
	CString txt;
    txt.FormatMessage(IDS_ADD_FILES_TO_USER_s_REVIEWS, GET_P4REGPTR()->GetP4User());
	pCmdUI->SetText ( txt );
	pCmdUI->Enable( !SERVER_BUSY() && !m_EditInProgress
								   && GET_P4REGPTR()->ShowEntireDepot() <= SDF_DEPOT );
}

void CUserListCtrl::OnAddReviews() 
{
    if( ! SERVER_BUSY() )
    {
	    ::SendMessage(m_depotWnd, WM_GETSELLIST, (WPARAM) &m_ReviewsList, 0);
	    if(m_ReviewsList.GetCount() > 0)
		{
			OnUserEditmy();
		}
	}
}

BOOL CUserListCtrl::TryDragDrop( )
{
	// Store the job this is from
	m_DragFromItemName = GetSelectedItemText();

	m_OLESource.DelayRenderData( (unsigned short) m_CF_USER);

	return m_OLESource.DoDragDrop(DROPEFFECT_COPY, &m_DragSourceRect, NULL)
			== DROPEFFECT_NONE ? FALSE : TRUE;
}

void CUserListCtrl::OnNewUser(WPARAM wParam, LPARAM lParam)
{
	if (!IsClear())
	{
		CString olduser = wParam ? m_OldDefUser : m_OldCurUser;
		CString newuser = GET_P4REGPTR()->GetP4User((BOOL)wParam);
		CString defuser = GET_P4REGPTR()->GetP4User(TRUE);
		LV_ITEM lvItem;
		lvItem.mask = LVIF_IMAGE;
		lvItem.iSubItem = 0;
		lvItem.state = lvItem.stateMask = 0;

		lvItem.iItem = FindInList(olduser);
		if(lvItem.iItem > -1)
		{
			lvItem.iImage = CP4ViewImageList::GetUserIndex(false,
				olduser == defuser);
			SetItem(&lvItem);
		}

		lvItem.iItem = FindInList(newuser);
		if(lvItem.iItem > -1)
		{
			lvItem.iImage = CP4ViewImageList::GetUserIndex(true,
				newuser == defuser);
			SetItem(&lvItem);
		}
		if (wParam)
			m_OldDefUser = newuser;
		else
			m_OldCurUser = newuser;
	}
	::PostMessage(m_clientWnd, WM_NEWUSER, 0, 0);
	::PostMessage(m_branchWnd, WM_NEWUSER, 0, 0);
	::PostMessage(m_labelWnd,  WM_NEWUSER, 0, 0);
}
