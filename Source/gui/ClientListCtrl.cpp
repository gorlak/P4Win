// ClientListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "ClientListCtrl.h"
#include "ClientWizSheet.h"
#include "MainFrm.h"
#include "cmd_editspec.h"
#include "cmd_delete.h"
#include "cmd_describe.h"
#include "cmd_diff2.h"
#include "cmd_clients.h"
#include "cmd_get.h"
#include "cmd_changes.h"
#include "newclientdlg.h"
#include "TemplateNameDlg.h"
#include "Diff2ObjsDlg.h"
#include "ClientFilterDlg.h"
#include "MsgBox.h"
#include "ImageList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IMG_INDEX(x) (x-IDB_PERFORCE)

static LPCTSTR sRegValue_ClientList = _T("Client List");

enum ClientSubItems
{
	CLIENT_NAME,
	CLIENT_OWNER,
	CLIENT_HOST,
	CLIENT_ACCESSDATE,
	CLIENT_ROOT,
	CLIENT_DESC,
	CLIENT_MAXCOL
};

/////////////////////////////////////////////////////////////////////////////
// CClientListCtrl

IMPLEMENT_DYNCREATE(CClientListCtrl, CP4ListCtrl)

BEGIN_MESSAGE_MAP(CClientListCtrl, CP4ListCtrl)
	ON_COMMAND(ID_CLIENT_DELETE, OnClientDelete)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_DELETE, OnUpdateClientDelete)
	ON_UPDATE_COMMAND_UI(ID_VIEW_UPDATE_RIGHT, OnUpdateViewUpdate)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CLIENT_TEMPLATE, OnClientTemplate)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_TEMPLATE, OnUpdateClientTemplate)
	ON_COMMAND(ID_CLIENT_USEASLOCALTEMPLATE, OnUseAsLocalTemplate)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_USEASLOCALTEMPLATE, OnUpdateUseAsLocalTemplate)
	ON_COMMAND(ID_CLIENT_CLEARLOCALTEMPLATE, OnClearLocalTemplate)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_CLEARLOCALTEMPLATE, OnUpdateClearLocalTemplate)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_DESCRIBE, OnUpdateClientDescribe)
	ON_COMMAND(ID_CLIENT_DESCRIBE, OnDescribe)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_CLIENTSPEC_SWITCH, OnClientspecSwitch)
	ON_UPDATE_COMMAND_UI(ID_CLIENTSPEC_SWITCH, OnUpdateClientspecSwitch)
	ON_COMMAND(ID_SETDEFCLIENT, OnSetDefClient)
	ON_UPDATE_COMMAND_UI(ID_SETDEFCLIENT, OnUpdateSetDefClient)
	ON_COMMAND(ID_VIEW_UPDATE_RIGHT, OnViewUpdate)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnDeleteitem)
	ON_COMMAND(ID_CLIENTSPEC_NEW, OnClientspecNew)
	ON_UPDATE_COMMAND_UI(ID_CLIENTSPEC_NEW, OnUpdateClientspecNew)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_ADD_TOVIEW, OnUpdateAddToClientView)
	ON_COMMAND(ID_ADD_TOVIEW, OnAddToClientView)
	ON_COMMAND(ID_PERFORCE_OPTIONS, OnPerforceOptions)
	ON_UPDATE_COMMAND_UI(ID_FILE_DIFF2, OnUpdateDiff2Clients)
	ON_COMMAND(ID_FILE_DIFF2, OnDiff2Clients)
	ON_UPDATE_COMMAND_UI(ID_FILTERCLIENTS, OnUpdateFilterClients)
	ON_COMMAND(ID_FILTERCLIENTS, OnFilterClients)
	ON_UPDATE_COMMAND_UI(ID_CLEARCLIENTFILTER, OnUpdateClearClientFilter)
	ON_COMMAND(ID_CLEARCLIENTFILTER, OnClearClientFilter)
	ON_MESSAGE(WM_P4CLIENTS, OnP4ClientList )
	ON_MESSAGE(WM_P4EDITSPEC, OnP4ClientSpec )
	ON_MESSAGE(WM_P4ENDSPECEDIT, OnP4EndSpecEdit )
	ON_MESSAGE(WM_P4DELETE, OnP4Delete )
	ON_MESSAGE(WM_P4DESCRIBE, OnP4Describe )
    ON_MESSAGE(WM_P4ENDDESCRIBE, OnP4EndDescribe )
	ON_MESSAGE(WM_P4GET, OnP4Sync )
	ON_MESSAGE(WM_NEWUSER, NewUser )
	ON_MESSAGE(WM_P4DESCRIBEALT, OnP4UseAsLocalTemplate )
END_MESSAGE_MAP()

CClientListCtrl::CClientListCtrl()
{
	m_SortAscending=TRUE;
	m_LastSortCol=0;
	m_OldClient = GET_P4REGPTR()->GetP4Client();
	m_ClientNew.Empty();
	m_NewClicked = m_NewClient = FALSE;
	m_viewType = P4CLIENT_SPEC;
	m_OldDefClient = m_OldCurClient = _T("@");
	m_PrevNbrCli = -1;

	m_CF_DEPOT = RegisterClipboardFormat(LoadStringResource(IDS_DRAGFROMDEPOT));
	m_CF_CLIENT= RegisterClipboardFormat(LoadStringResource(IDS_DRAGFROMCLIENT));
	m_caption =  m_captionplain = LoadStringResource(IDS_PERFORCE_CLIENTS);
}

CClientListCtrl::~CClientListCtrl()
{
}




/////////////////////////////////////////////////////////////////////////////
// CClientListCtrl diagnostics

#ifdef _DEBUG
void CClientListCtrl::AssertValid() const
{
	CP4ListCtrl::AssertValid();
}

void CClientListCtrl::Dump(CDumpContext& dc) const
{
	CP4ListCtrl::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CClientListCtrl message handlers


/*
	_________________________________________________________________

	let user edit the client spec s/he clicked on.
	_________________________________________________________________
*/

void CClientListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	int index = GetHitItem ( point );
	if(index > -1)
	{
		SetItemState( index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
		OnDescribe( );
	}
	else
		CP4ListCtrl::OnLButtonDblClk(nFlags, point);
}


/*
	_________________________________________________________________
*/

void CClientListCtrl::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	delete (CP4Client *) GetItemData(pNMListView->iItem);
			
	*pResult = 0;
}


/*
	_________________________________________________________________

	define a new client, which means
		set the current client to this new one (true? maybe i should ask the user)
		sometimes change the registry
		edit this new client's spec

	this used to be done in the options dialogue's connection
	parameters
	_________________________________________________________________
*/

void CClientListCtrl::OnUpdateClientspecNew(CCmdUI* pCmdUI) 
{
	pCmdUI->SetText ( LoadStringResource( IDS_NEW ) );
	pCmdUI->Enable( !SERVER_BUSY() && !m_EditInProgress && !MainFrame()->IsModlessUp() );
}


/*
	_________________________________________________________________
*/

void CClientListCtrl::ClientspecNew()
{
	OnClientspecNew( ) ;
}


/*
	_________________________________________________________________
*/

void CClientListCtrl::OnClientspecNew() 
{
	DoClientspecNew(FALSE, NULL);
}

void CClientListCtrl::DoClientspecNew(BOOL bUseDefTemplate, LPCTSTR defName ) 
{
	MainFrame()->ViewClients();
	m_ClientNew.Empty();
	m_OldClient= GET_P4REGPTR()->GetP4Client();

	//		let user type in the new name. if it's blank the user bailed.
	//
	CNewClientDlg newdlg;
	newdlg.SetNew( NEWCLIENT );
	if (FindInListAll(m_Active) != -1)
		newdlg.m_Active = m_Active;
	if (defName)
		newdlg.m_Name = defName;
	if( newdlg.DoModal( ) == IDCANCEL )
		return;

	if (newdlg.m_RunWizard)
	{
		CString txt;
		CString port = GET_P4REGPTR()->GetP4Port();
		CString clientname = newdlg.GetName();
		txt.FormatMessage( IDS_NOCLIENT_s_s_s, clientname, port, GET_P4REGPTR()->GetP4User() );
		int i;
		if ((i = txt.Find(_T('\n'))) != -1)
		{
			txt = txt.Mid(i);
			txt.TrimLeft();
		}
		CClientWizSheet dlg(LoadStringResource(IDS_CLIENTWORKSPACEWIZARD), this, 1);
		dlg.SetMsg(txt);
		dlg.SetName(clientname);
		dlg.SetPort(port);
		dlg.AllowBrowse(FALSE);
		int rc;
		while(1)
		{
			if( (rc = dlg.DoModal()) == IDCANCEL )	// user bailed
			{
				break;
			}
			if ( rc == IDIGNORE )	// user chose to browse existing clients
			{
				ViewUpdate();
				break;
			}
			else	// user clicked the Finish button on the create client page
			{
				CString newClient = dlg.GetName();
				CString newRoot   = dlg.GetRoot();
				BOOL bEdit        = dlg.IsEdit();
				BOOL bTmpl        = dlg.IsTmpl();
				CString tmplate   = dlg.GetTmpl();
				while (m_UpdateState == LIST_UPDATING)
					Sleep(100);
				if (FindInListAll(newClient) != -1)
				{
					CString msg;
					msg.FormatMessage(IDS_CANT_RECREATE_EXISTING_CLIENT_s, newClient);
					AfxMessageBox( msg, MB_ICONSTOP );
				}
				else if (!newClient.IsEmpty() && !newRoot.IsEmpty())
				{
					if (AutoCreateClientSpec(newClient, newRoot, bEdit, bTmpl, tmplate))
						break;;
				}
			}
		}
		return;
	}

	CString saveActive = m_Active;
	m_Active = newdlg.GetName( ) ;
	if ( m_Active.IsEmpty( ) )
		return;

	if ( SetP4Client( TRUE ) ) 
	{
		// rather than use the current directory as the default client root
		// switch to the drive's root and use that; switch back at the end.
		TCHAR	buf[MAX_PATH+1];
		TCHAR	root[4];
		if (GetCurrentDirectory(MAX_PATH, buf))
		{
			_tcsncpy(root, buf, 3);
			root[3] = '\0';
			SetCurrentDirectory(root);
		}
		else
		{
			buf[0] = _T('\0');
		}

		// Get the edit of the new spec underway
		m_NewClient  = (saveActive != m_Active);
		m_NewClicked = TRUE;
		OnEditSpec( m_Active, bUseDefTemplate );
		m_ClientNew = m_Active;

		// restore previous working directory
		if (buf[0])
			SetCurrentDirectory(buf);
	}
}


/*
	_________________________________________________________________
*/

void CClientListCtrl::OnUpdateClientDelete(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( OnUpdateShowMenuItem( pCmdUI, IDS_DELETE_s )
			&& GetSelectedItemText() == GET_P4REGPTR()->GetP4Client() 
			&& !MainFrame()->IsModlessUp() );
}

BOOL CClientListCtrl::OKToDelete( ) 
{
	if ( GetSelectedItemText() == GET_P4REGPTR()->GetP4Client() )
		return TRUE;
	else
	{
		AfxMessageBox(IDS_NO_PERMISSION_TO_DELETE_OTHER_CLIENTS, MB_ICONINFORMATION);
		return FALSE;
	}
}


/*
	_________________________________________________________________
*/

void CClientListCtrl::OnClientDelete() 
{
	OnDelete( P4CLIENT_DEL );
}


/*
	_________________________________________________________________
*/

void CClientListCtrl::EditTheSpec(CString *name) 
{
	if (*name == GET_P4REGPTR()->GetP4Client())
		OnClientEditmy();
}

void CClientListCtrl::OnClientEditmy() 
{
	m_Active = GET_P4REGPTR()->GetP4Client();
	m_OldClient= m_Active;
	m_NewClicked = m_NewClient = FALSE;

	int index = FindInList( m_Active ) ;
	if ( index  > -1 )
	{
		SetItemState(index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		EnsureVisible(index, FALSE);
	}

	OnEditSpec( m_Active );
}


void CClientListCtrl::OnUpdateClientTemplate(CCmdUI* pCmdUI) 
{
	m_Active = GetSelectedItemText();
	CString sCurrentClient = GET_P4REGPTR()->GetP4Client();

    CString prompt;
    prompt.FormatMessage(IDS_CREATE_UPDATE_CLIENT_USING_s_AS_TEMPLATE,
        m_Active.IsEmpty( ) ? TruncateString(sCurrentClient, 50) : TruncateString(m_Active, 50));
	pCmdUI->SetText ( prompt );

	pCmdUI->Enable(	!SERVER_BUSY() && !m_EditInProgress
					&& !m_Active.IsEmpty() 
					&& !MainFrame()->IsModlessUp() );
}


/*
	_________________________________________________________________
*/

void CClientListCtrl::OnClientTemplate() 
{
	m_Template = GetSelectedItemText(); 
	m_OldClient= GET_P4REGPTR()->GetP4Client();
	m_Active.Empty();

	// rather than use the current directory as the default client root
	// switch to the drive's root and use that; switch back at the end.
	TCHAR	buf[MAX_PATH+1];
	TCHAR	root[4];
	if (GetCurrentDirectory(MAX_PATH, buf))
	{
		_tcsncpy(root, buf, 3);
		root[3] = '\0';
		SetCurrentDirectory(root);
	}
	else
	{
		buf[0] = _T('\0');
	}

	OnEditTemplate( m_Template );

	// restore previous working directory
	if (buf[0])
		SetCurrentDirectory(buf);
}

void CClientListCtrl::OnUpdateClearLocalTemplate(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(	!CString(GET_P4REGPTR()->GetLocalCliTemplate()).IsEmpty() );
}

void CClientListCtrl::OnClearLocalTemplate()
{
	GET_P4REGPTR()->SetLocalCliTemplate(_T(""));
}

void CClientListCtrl::OnUpdateUseAsLocalTemplate(CCmdUI* pCmdUI) 
{
	m_Active = GetSelectedItemText();
	CString sCurrentClient = GET_P4REGPTR()->GetP4Client();

    CString prompt;
    prompt.FormatMessage(IDS_CLIENT_USEASLOCALTEMPLATE,
        m_Active.IsEmpty( ) ? TruncateString(sCurrentClient, 50) : TruncateString(m_Active, 50));
	pCmdUI->SetText ( prompt );

	pCmdUI->Enable(	!SERVER_BUSY() && !m_EditInProgress
					&& !m_Active.IsEmpty() 
					&& !MainFrame()->IsModlessUp()
					&& GET_P4REGPTR()->LocalCliTemplateSw() );
}

void CClientListCtrl::OnUseAsLocalTemplate()
{
	m_Active = GetSelectedItemText();

	if ( m_Active.IsEmpty( ) )
		return;

	CCmd_Describe *pCmd = new CCmd_Describe;
	pCmd->Init( m_hWnd, RUN_ASYNC );
	pCmd->SetAlternateReplyMsg( WM_P4DESCRIBEALT );
	if( pCmd->Run( m_viewType, m_Describing = m_Active ) )
	{
		MainFrame()->UpdateStatus( LoadStringResource(IDS_FETCHING_SPEC) );	
		return;
	}
	else
	{
		delete pCmd;
		return;
	}
}

LRESULT CClientListCtrl::OnP4UseAsLocalTemplate(WPARAM wParam, LPARAM lParam)
{
	CCmd_Describe *pCmd = (CCmd_Describe *) wParam;
	CString ref = pCmd->GetReference();
	CString tmplate = pCmd->GetDescription();
	int i;
	if ((i = tmplate.Find(_T("\n\nClient:\t"))) != -1)
	{
		tmplate = tmplate.Mid(i);
		tmplate.Replace(ref, LOCALDEFTEMPLATE);
		if (GET_P4REGPTR()->SetLocalCliTemplate(tmplate))
		{
			CString msg;
			msg.FormatMessage(IDS_LOCALDEFTEMPLATESET_s, ref);
			AddToStatus(msg, SV_COMPLETION);
		}
	}
	delete pCmd;
	MainFrame()->ClearStatus();
	return 0;
}


void CClientListCtrl::OnUpdateClientDescribe(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( OnUpdateShowMenuItem( pCmdUI, IDS_DESCRIBEIT_s ) 
					&& !MainFrame()->IsModlessUp() );	
}


/*
	_________________________________________________________________

	whether to show or grey out the F5 menu item on the context menu
	_________________________________________________________________
*/

void CClientListCtrl::OnUpdateViewUpdate(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!SERVER_BUSY() && !MainFrame()->IsModlessUp());
}


/*
	_________________________________________________________________

	user hit F5 or equiv
	_________________________________________________________________
*/

void CClientListCtrl::OnViewUpdate() 
{
	m_Active = GetSelectedItemText();
	SetCaption();
	CCmd_Clients *pCmd= new CCmd_Clients;
	pCmd->Init( m_hWnd, RUN_ASYNC);
	if( pCmd->Run( ) )
	{
		MainFrame()->UpdateStatus(LoadStringResource(IDS_REQUESTING_CLIENT_LISTING));
		MainFrame()->SetClientUpdateTime(GetTickCount());
		Clear();
		CP4ListCtrl::OnViewUpdate();
	}
	else
		delete pCmd;
}

CString CClientListCtrl::SetCaption()
{
	int i = GET_P4REGPTR()->GetClientFilteredFlags();
	if (i)
    {
        CString filter;
		CString txt;

		if (i & 0x10)
			txt = LoadStringResource(IDS_CURRENT_USER);
		else if (i & 0x01)
			txt = GET_P4REGPTR()->GetClientFilterOwner();
		if (i & 0x22)
		{
			if (!txt.IsEmpty())
				txt += _T(", ");
			txt += i & 0x20 ? GET_P4REGPTR()->GetHostname() : GET_P4REGPTR()->GetClientFilterHost();
		}
		if (i & 0x04)
		{
			if (!txt.IsEmpty())
				txt += _T(", ");
			txt += CString(_T("\"")) + GET_P4REGPTR()->GetClientFilterDesc() + _T("\"");
		}
		filter.FormatMessage(IDS_FILTERED, txt);
		m_caption = LoadStringResource(IDS_PERFORCE_CLIENTS) + filter;
    }
	else
		m_caption = LoadStringResource(IDS_PERFORCE_CLIENTS);

	CP4PaneContent::GetView()->SetCaption();

	return m_caption;
}


/*
	_________________________________________________________________
*/

LRESULT CClientListCtrl::NewUser(WPARAM wParam, LPARAM lParam)
{
	if (GET_P4REGPTR()->GetClientFilteredFlags() & 0x10)
		Clear();
	return 0;
}

void CClientListCtrl::Clear() 
{
    SetRedraw(FALSE);
	DeleteAllItems();
    SetRedraw(TRUE);

	CP4ListCtrl::Clear();
}


/*
	_________________________________________________________________

	called when user right clicks on a clientspec. create one of two
	menus, depending on where the user clicked.

	if user has highlighted a client, allow them to do the usual
	things to a client view. (describe it, switch to it, use it as 
	a template to create a new client)
	(edit and delete -- i need to know the owner. but delete is already there!)
	
	if the user clicked on a random part of the pane, show
	only the general choices, like refresh, create a new client
	-- and maybe edit the current client?
	_________________________________________________________________
*/

void CClientListCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
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

	// Can always create a new client
	popMenu.AppendMenu( stringsON, ID_CLIENTSPEC_NEW );

	// Only offer edit if no other client is selected
	if ( index == -1 || GetSelectedItemText() == GET_P4REGPTR()->GetP4Client() )
		popMenu.AppendMenu( stringsON, ID_CLIENT_EDITMY );

	if( index != -1 )
	{
		// Make sure item gets selected
		SetItemState( index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
			
		popMenu.AppendMenu( stringsON, ID_CLIENT_DESCRIBE ); //or client edit if user is the owner
		popMenu.AppendMenu( stringsON, ID_CLIENT_DELETE );

		popMenu.AppendMenu( MF_SEPARATOR );

		// Can only switch to a client if its not my current client
		if ( GetSelectedItemText() != GET_P4REGPTR()->GetP4Client() )
			popMenu.AppendMenu( stringsON, ID_CLIENTSPEC_SWITCH );
		else	// can only set as default if it is my current client
			popMenu.AppendMenu( stringsON, ID_SETDEFCLIENT );

		popMenu.AppendMenu( stringsON, ID_CLIENT_TEMPLATE );
		if (GET_P4REGPTR()->LocalCliTemplateSw())
		{
			popMenu.AppendMenu( MF_SEPARATOR );
			popMenu.AppendMenu( stringsON, ID_CLIENT_USEASLOCALTEMPLATE, LoadStringResource(IDS_CLIENT_USEASLOCALTEMPLATE) );
			popMenu.AppendMenu( stringsON, ID_CLIENT_CLEARLOCALTEMPLATE, LoadStringResource(IDS_CLIENT_CLEARLOCALTEMPLATE) );
		}
	}
	popMenu.AppendMenu( MF_SEPARATOR );
	popMenu.AppendMenu( stringsON, ID_FILTERCLIENTS, LoadStringResource(IDS_FILTERCLIENTS) );
	popMenu.AppendMenu( stringsON, ID_CLEARCLIENTFILTER, LoadStringResource(IDS_CLEARCLIENTFILTER) );
	popMenu.AppendMenu( MF_SEPARATOR );
	CString txt = GetSelectedItemText();
	if (txt.IsEmpty())
		txt = GET_P4REGPTR()->GetP4Client( );
	txt = LoadStringResource(IDS_CHGS_BY_CLIENT) + txt;
	txt = TruncateString(txt, 50);
	popMenu.AppendMenu( stringsON, ID_CHGS_BY_CLIENT, txt );
	popMenu.AppendMenu( stringsON, ID_VIEW_UPDATE, LoadStringResource(IDS_REFRESH) );

	MainFrame()->AddToolsToContextMenu(&popMenu);

	//		put up the menu 
	//
	popMenu.TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd( ) );
}


/////////////////////////////////////////////////////////////////////////////
// CClientListCtrl message handlers

/*
	_________________________________________________________________
*/

void CClientListCtrl::OnUpdateClientspecSwitch(CCmdUI* pCmdUI) 
{
	CString selClient = GetSelectedItemText();
    CString prompt;
    prompt.FormatMessage(IDS_SWITCH_TO_s, TruncateString(selClient, 50));
	pCmdUI->SetText ( prompt );
	pCmdUI->Enable( !SERVER_BUSY() 
					&& !selClient.IsEmpty()
					&& selClient != GET_P4REGPTR()->GetP4Client( )
					&& !MainFrame()->IsModlessUp());
}


/*
	_________________________________________________________________
*/

void CClientListCtrl::OnClientspecSwitch() 
{
	ClientSpecSwitch(GetSelectedItemText());
}

BOOL CClientListCtrl::ClientSpecSwitch(CString switchTo, BOOL bAlways /*=FALSE*/, BOOL portWasChanged /*= FALSE*/) 
{
	CCmd_Describe *pCmd = new CCmd_Describe;
	pCmd->Init( NULL, RUN_SYNC );
	BOOL cmdStarted= pCmd->Run( P4CLIENT_SPEC, switchTo );
	if(cmdStarted && !pCmd->GetError())
	{
		CString options= TheApp()->GetClientSpecField( _T("Options"), pCmd->GetDescription() );
		if (( options.Find(_T("unlocked")) == -1 ) && ( options.Find(_T("locked")) != -1 ))
		{
			CString lockedby= TheApp()->GetClientSpecField( _T("Owner"), pCmd->GetDescription() );
			if ( lockedby.Compare( GET_P4REGPTR()->GetP4User() ) != 0 )
			{
				CString errmsg;
                errmsg.FormatMessage(IDS_CLIENT_s_IS_LOCKED_BY_USER_s_AND_CANT_BE_USED_BY_s,
                    switchTo, lockedby, GET_P4REGPTR()->GetP4User());
				AddToStatus( errmsg, SV_WARNING );  
				AfxMessageBox( errmsg, MB_ICONEXCLAMATION );
				delete pCmd;
				return FALSE;
			}
		}
	}
	delete pCmd;

	m_Active = switchTo;

	if ( SetP4Client( bAlways ) )
		MainFrame()->OnPerforceOptions( FALSE, portWasChanged ) ;
	return TRUE;
}


/*
	_________________________________________________________________
*/

void CClientListCtrl::OnUpdateSetDefClient(CCmdUI* pCmdUI) 
{
	CString selClient = GetSelectedItemText();
    CString prompt;
    prompt.FormatMessage(IDS_SETDEFCLIENT_s, TruncateString(selClient, 50));
	pCmdUI->SetText ( prompt );
	pCmdUI->Enable( !SERVER_BUSY() 
					&& !selClient.IsEmpty()
					&& selClient == GET_P4REGPTR()->GetP4Client( FALSE )
					&& selClient != GET_P4REGPTR()->GetP4Client( TRUE )
					&& !MainFrame()->IsModlessUp() );
}


/*
	_________________________________________________________________
*/

void CClientListCtrl::OnSetDefClient() 
{
	if (m_Active != GetSelectedItemText())
	{
		ASSERT(0);
		return;
	}

	if (GET_P4REGPTR()->GetExpandFlag() == 1)
		GET_P4REGPTR()->AddMRUPcuPath(MainFrame()->GetCurrentItemPath());

	if( !GET_P4REGPTR()->SetP4Client( m_Active, TRUE, TRUE, TRUE ) )
	{
		AfxMessageBox(IDS_UNABLE_TO_WRITE_P4CLIENT_TO_THE_REGISTRY, MB_ICONEXCLAMATION);
		m_Active = GetSelectedItemText();
	}
	else
    {
        CString msg;
        msg.FormatMessage(IDS_DEFAULT_CLIENT_SET_TO_s, m_Active);
		AddToStatus( msg );
    }
}


/*
	_________________________________________________________________
*/

//fanny: damn! p4clients doesn't give you the owner! how can i put up an
//edit menu item without checking first?

void CClientListCtrl::InsertClient(CP4Client *client, int index, 
								   CString *curclient, CString *defclient, CString *user)
{
	ASSERT(client != NULL);
	m_iImage = CP4ViewImageList::VI_CLIENT;

	LV_ITEM lvItem;
	int iActualItem = -1;
	CString txt;
	BOOL bFilteredOut = MainFrame()->IsClientFilteredOut(client, user, curclient, defclient);

	for(int subItem=0; subItem < CLIENT_MAXCOL; subItem++)
	{
		lvItem.mask=LVIF_TEXT | 
					((subItem==0) ? LVIF_IMAGE : 0) |
					((subItem==0) ? LVIF_PARAM : 0);

		lvItem.iItem= (subItem==0) ? index : iActualItem;
        ASSERT(lvItem.iItem != -1 || bFilteredOut);

		lvItem.iSubItem= subItem;

		switch(subItem)
		{
		case CLIENT_NAME: 
			{
			lvItem.pszText = const_cast<LPTSTR>(client->GetClientName());
			bool isCurrent = curclient->Compare(lvItem.pszText) == 0;
			bool isDefault = defclient->Compare(lvItem.pszText) == 0;
			lvItem.iImage = CP4ViewImageList::GetClientIndex(isCurrent, isDefault);
			if(isDefault)
				m_OldDefClient = lvItem.pszText;
			if(isCurrent)
				m_OldCurClient = lvItem.pszText;
	        lvItem.lParam=(LPARAM) client;
			}
			break;
		case CLIENT_OWNER: 
			lvItem.pszText = const_cast<LPTSTR>( (LPCTSTR) client->GetOwner()); break;
		case CLIENT_HOST: 
			lvItem.pszText = const_cast<LPTSTR>( (LPCTSTR) client->GetHost()); break;
		case CLIENT_ACCESSDATE: 
			lvItem.pszText = const_cast<LPTSTR>( (LPCTSTR) client->GetDate()); break;
		case CLIENT_ROOT: 
			lvItem.pszText = const_cast<LPTSTR>( (LPCTSTR) client->GetRoot()); break;
		case CLIENT_DESC: 
			txt= PadCRs(client->GetDescription());
			lvItem.pszText = const_cast<LPTSTR>( (LPCTSTR) txt); break;
		default:
			ASSERT( 0 ); lvItem.pszText = _T("@"); break;
		}

		m_ListAll.column[subItem].SetAtGrow(index, lvItem.pszText);
		if (bFilteredOut)
			continue;
			
		if (subItem==0)
		{
			static	BOOL	bErrDispalyed=FALSE;
			iActualItem=InsertItem(&lvItem);
			if (iActualItem==-1 && !bErrDispalyed)
			{
				AfxMessageBox(_T("Client item insert failure"), MB_ICONSTOP);
				bErrDispalyed = TRUE;
			}
		}
		else
			SetItem(&lvItem);
	}
	if (bFilteredOut)
		delete client;
}

/*
	_________________________________________________________________
*/

void CClientListCtrl::UpdateClient(CP4Client *client, int index)
{
	// After a spec edit, update the appropriate list item
	// First, switch the client data
	CP4Client *oldClient= (CP4Client *) GetItemData(index);
	delete oldClient;
	SetItemData(index, (LPARAM) client);

	// Then update the text
	SetItemText(index, CLIENT_NAME,    const_cast<LPTSTR>( (LPCTSTR) client->GetClientName()));
	SetItemText(index, CLIENT_OWNER,   const_cast<LPTSTR>( (LPCTSTR) client->GetOwner()));
	SetItemText(index, CLIENT_HOST,    const_cast<LPTSTR>( (LPCTSTR) client->GetHost()));
	SetItemText(index, CLIENT_ACCESSDATE, const_cast<LPTSTR>( (LPCTSTR) client->GetDate()));
	SetItemText(index, CLIENT_ROOT,    const_cast<LPTSTR>( (LPCTSTR) client->GetRoot()));
	CString	txt= PadCRs(client->GetDescription());
	SetItemText(index, CLIENT_DESC,    const_cast<LPTSTR>((LPCTSTR)txt));
}

void CClientListCtrl::UpdateClientAll(CP4Client *client, int index)
{
	m_ListAll.column[CLIENT_NAME].SetAt(index, const_cast<LPTSTR>((LPCTSTR)client->GetClientName()));
	m_ListAll.column[CLIENT_OWNER].SetAt(index, const_cast<LPTSTR>((LPCTSTR)client->GetOwner()));
	m_ListAll.column[CLIENT_HOST].SetAt(index, const_cast<LPTSTR>((LPCTSTR)client->GetHost()));
	m_ListAll.column[CLIENT_ACCESSDATE].SetAt(index, const_cast<LPTSTR>((LPCTSTR)client->GetDate()));
	m_ListAll.column[CLIENT_ROOT].SetAt(index, const_cast<LPTSTR>((LPCTSTR)client->GetRoot()));
	CString	txt= PadCRs(client->GetDescription());
	m_ListAll.column[CLIENT_DESC].SetAt(index, const_cast<LPTSTR>((LPCTSTR)txt));
}


/*
	_________________________________________________________________

	Receives ak for client spec edit
	_________________________________________________________________
*/

LRESULT CClientListCtrl::OnP4ClientSpec(WPARAM wParam, LPARAM lParam)
{
	m_PrevNbrCli = m_UpdateState == LIST_CLEAR ? -1 : GetItemCount();
	m_saveclientnew = m_ClientNew;	// save name of freshly created client
	CCmd_EditSpec *pCmd = (CCmd_EditSpec *) wParam;

	if (pCmd->GetUseLocalDefTmplate())
	{
        CString specIn(pCmd->GetSpecIn());
		int i;
		if ((i = specIn.Find(_T("\n\nClient:\t"))) != -1)
		{
			CString tmplate = GET_P4REGPTR()->GetLocalCliTemplate();
			tmplate.Replace(LOCALDEFTEMPLATE, pCmd->GetItemName());
			pCmd->SetSpecIn(specIn.Left(i) + tmplate);
		}
	}

	m_ClientNew.Empty();

	// let the dialogbox know whether this is a new client or an edit of an existing one
	int index= FindInListAll(m_Active.IsEmpty() ? pCmd->GetItemName() : m_Active);
	if ((index == -1) || !GetItemCount())	// if name not in client list
		 index = m_NewClient ? -1 : -2;		//	it's new if and only if they clicked 'New'
	pCmd->SetIsNewClient(m_isNew = (index == -1));
	pCmd->SetIsRequestingNew(m_NewClicked);
	pCmd->SetOldClient(m_OldClient);
	pCmd->SetCaller(DYNAMIC_DOWNCAST(CView, GetParent()));
	m_NewClicked = m_NewClient = FALSE;	// clear these now, cuz no longer needed
    if (m_AddToViewList.GetCount() > 0)	// We have files to add to the Reviews list
	{
        CString specIn(pCmd->GetSpecIn());
		int i;
		if ((i = specIn.Find(_T("\n\nView:\n"))) == -1)
		{
			i = specIn.GetLength();
			specIn += _T("\n\nView:\n");
		}
		else
			specIn += _T("\t@\t@\n");	//	A view that is @[TAB]@ means
											//	put a blank line in the list here.
		POSITION pos;
		CString filename;
		CString localname;
		BOOL b = GET_P4REGPTR()->UseDepotInClientView();
		for (pos = m_AddToViewList.GetHeadPosition(); pos != NULL; )
		{
			filename = m_AddToViewList.GetNext(pos);
			if ((i = filename.Find(_T("<contains no files or folders>"))) != -1)
				filename = filename.Left(i-1) + _T("/...");
			localname = CString ( _T("//") ) + GET_P4REGPTR()->GetP4Client();
			if (!b && ((i = filename.Find(_T('/'), 2)) > 2))
				localname += filename.Right(filename.GetLength() - i);
			else
				localname += filename.Right(filename.GetLength() - 1);
			if (filename.Find(_T(' ')))
				filename = _T('\"') + filename + _T('\"');
			if (localname.Find(_T(' ')))
				localname = _T('\"') + localname + _T('\"');
			specIn += _T('\t') + filename + _T('\t') + localname + _T('\n');
		}
		m_AddToViewList.RemoveAll();		// this info is no longer needed
        pCmd->SetSpecIn(specIn);
	}
	// show the dialog box
	if(!pCmd->GetError() && !m_EditInProgress && pCmd->DoSpecDlg(this))
	{
		m_EditInProgress = TRUE;
		m_EditInProgressWnd = pCmd->GetSpecSheet();
	}
	else
	{
		if (GET_P4REGPTR()->GetExpandFlag() == 1)
			GET_P4REGPTR()->AddMRUPcuPath(MainFrame()->GetCurrentItemPath());
		m_Active = m_OldClient;
		GET_P4REGPTR()->SetP4Client( m_Active, TRUE, FALSE, FALSE);
		MainFrame()->UpdateCaption( ) ;	
        delete m_pNewSpec;

		if (pCmd->HaveServerLock())
			pCmd->ReleaseServerLock();
		delete pCmd;
	}

	MainFrame()->ClearStatus();
	return 0;
}

LRESULT CClientListCtrl::OnP4EndSpecEdit( WPARAM wParam, LPARAM lParam )
{
	CCmd_EditSpec *pCmd= (CCmd_EditSpec *) wParam;
	BOOL chainedCommand = FALSE;
	BOOL need2Refresh = FALSE;

	if (lParam != IDCANCEL && lParam != IDABORT)
	{
		// Get the Sync flag
		int syncAfter = SYNC_DONT;
		BOOL bHasChgs = TRUE;
		if (m_PrevNbrCli < 1)
		{
			int key= pCmd->HaveServerLock() ? pCmd->GetServerKey() : 0;
			CCmd_Changes *pCmd2= new CCmd_Changes;
			pCmd2->Init( m_hWnd, RUN_SYNC, key ? HOLD_LOCK : LOSE_LOCK, key);
			if( pCmd2->Run(SUBMITTED_CHANGES, FALSE, NULL, 1) )
			{
				if (!pCmd2->GetChanges()->GetCount())
					bHasChgs = FALSE;
			}
			delete pCmd2;
		}
		if (m_isNew && !GET_P4REGPTR()->DontShowYouHaveCr8NewClient() && bHasChgs)
		{
			BOOL b;
			switch(MsgBox(IDS_YOU_HAVE_CREATED_NEW_CLIENT,
						MB_ICONEXCLAMATION | MB_DEFBUTTON1, 0, this, &b))
			{
			case IDC_BUTTON1:
				syncAfter = SYNC_HEAD;
				break;
			case IDC_BUTTON2:
				syncAfter = SYNC_DONT;
				break;
			}
			GET_P4REGPTR()->SetDontShowYouHaveCr8NewClient(b);
		}
		else if ((pCmd->IsSyncAfter() || pCmd->IsAutoUpdateSpec())
			  && !GET_P4REGPTR()->DontShowYouHaveChgClientView() && bHasChgs)
		{
			BOOL b;
			switch(MsgBox(pCmd->IsAutoUpdateSpec() ? IDS_DOYOUWANTOTSYNCYOURNEWCLIENT 
												   : IDS_YOU_HAVE_CHANGED_CLIENTS_VIEW,
												   MB_ICONQUESTION | MB_DEFBUTTON1, 0, this, 
												   pCmd->IsAutoUpdateSpec() ? NULL : &b))
			{
			case IDC_BUTTON1:
				syncAfter = SYNC_HEAD;
				break;
			case IDC_BUTTON2:
				syncAfter = pCmd->IsAutoUpdateSpec() ? SYNC_DONT : SYNC_HAVE;
				break;
			case IDC_BUTTON3:
				syncAfter = SYNC_DONT;
				break;
			}
			if (!pCmd->IsAutoUpdateSpec())
				GET_P4REGPTR()->SetDontShowYouHaveChgClientView(b);
		}

		BOOL bDeld = FALSE;
		if (m_UpdateState == LIST_UPDATED)
		{
			// we have to set 'index' again in case client's name got changed
			int index= FindInList(m_pNewSpec->GetClientName());
			int ixAll= FindInListAll(m_pNewSpec->GetClientName());
			if (ixAll == -1)	// not in either list
			{
				ASSERT(index == -1);
				CString curclient = GET_P4REGPTR()->GetP4Client();
				CString defclient = GET_P4REGPTR()->GetP4Client(TRUE);
				InsertClient(m_pNewSpec, GetItemCount(), &curclient, &defclient);
			}
			else if (index > -1)	// in both lists
			{
				ASSERT(ixAll > -1);
				if (MainFrame()->IsClientFilteredOut(m_pNewSpec))	// should it no longer be shown?
				{
					need2Refresh = TRUE;
					delete m_pNewSpec;
				}
				else
				{
					UpdateClient(m_pNewSpec, index);
					UpdateClientAll(m_pNewSpec, ixAll);
				}
			}
			else	// not in visible list; is in list of all
			{
				if (MainFrame()->IsClientFilteredOut(m_pNewSpec))	// should it now be shown?
					need2Refresh = TRUE;
				else
					UpdateClientAll(m_pNewSpec, ixAll);
				if (pCmd->IsAutoUpdateSpec())
					m_OldClient = m_pNewSpec->GetClientName();
				delete m_pNewSpec;
				bDeld = TRUE;
			}
			ReSort();
			if (pCmd->IsAutoUpdateSpec() || pCmd->IsSpecForceSwitch())
			{
				if (bDeld)
					need2Refresh = TRUE;
				else
				{
					int i = FindInList(m_OldClient = m_pNewSpec->GetClientName());
					if (i < 0)	
						i = 0;
					SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
					EnsureVisible(i, FALSE);
				}
				m_Active = _T("@");			// force switch to new client
			}
		}
		else
			if ( m_pNewSpec ) delete m_pNewSpec;

		int key= pCmd->HaveServerLock() ? pCmd->GetServerKey() : 0;
		if( m_Active == m_saveclientnew )	// active-client == saved new client name
			m_OldClient = m_saveclientnew;	//   means they decided to edit the current client
		if( m_Active != m_OldClient )		// If current client is not now the client when this
		{									//   all started, switch to the new current client
			if (GET_P4REGPTR()->GetExpandFlag() == 1)
				GET_P4REGPTR()->AddMRUPcuPath(MainFrame()->GetCurrentItemPath());
			m_Active = m_OldClient;
			GET_P4REGPTR()->SetP4Client( m_Active, TRUE, FALSE, FALSE);
			MainFrame()->UpdateCaption( );
			if (syncAfter)
				chainedCommand = SyncAfter(key, syncAfter);
        }
		else if( syncAfter || GET_P4REGPTR()->GetClearAndReload()
				|| AfxMessageBox(IDS_YOU_HAVE_UPDATED_YOUR_CLIENT_WORKSPACE__CLEAR_AND_RELOAD, 
								MB_YESNO |MB_ICONQUESTION ) == IDYES )
		{
			if (syncAfter)
				chainedCommand = SyncAfter(key, syncAfter);
			else
			{
				int key= pCmd->HaveServerLock() ? pCmd->GetServerKey() : 0;
				MainFrame()->UpdateDepotandChangeViews(REDRILL, key);
				chainedCommand=TRUE;
			}
		}
		if (GET_SERVERLEVEL() >= 22)
			TheApp()->Set_m_ClientSubOpts(TheApp()->GetClientSpecField( _T("SubmitOptions"), pCmd->GetSpecOut()));
    }
	else
	{
		if ( m_pNewSpec )
			delete m_pNewSpec;
		if (lParam == IDCANCEL && pCmd->GetIsRequestingNew())
		{
			m_Active = pCmd->GetOldClient();	// switch back to the previous client
			GET_P4REGPTR()->SetP4Client( m_Active, TRUE, FALSE, FALSE);
			MainFrame()->UpdateCaption( );
		}
	}

	if (lParam != IDABORT)
	{
		MainFrame()->ClearStatus();
		if (!chainedCommand && pCmd->HaveServerLock())
			pCmd->ReleaseServerLock();
		CDialog *dlg = (CDialog *)pCmd->GetSpecSheet();
		dlg->DestroyWindow();
	}
	delete pCmd;
	m_EditInProgress = FALSE;

	if (need2Refresh && !chainedCommand)
		OnViewUpdate();

	if (TheApp()->m_RunClientWizOnly && !chainedCommand)
		::PostMessage(MainFrame()->m_hWnd, WM_COMMAND, ID_APP_EXIT, 0);
	return 0;
}

// After a client spec has been update, this may get called to sync the client
BOOL CClientListCtrl::SyncAfter(int key, int syncAfter)
{
	CCmd_Get *pCmd= new CCmd_Get;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK, key);
	m_AddToViewList.RemoveAll();
	m_AddToViewList.AddHead(CString(syncAfter == SYNC_HEAD ? _T("#head") : _T("#have")));
	if( pCmd->Run( &m_AddToViewList, FALSE, FALSE ) )
	{
		MainFrame()->UpdateStatus( LoadStringResource(IDS_FILE_SYNC) );
		return TRUE;
	}
	else
	{
		delete pCmd;
		return FALSE;
	}
}

LRESULT CClientListCtrl::OnP4Sync(WPARAM wParam, LPARAM lParam)
{
	CCmd_Get *pCmd= (CCmd_Get *) wParam;
	if (TheApp()->m_RunClientWizOnly)
	{
		if (pCmd->GetWarningsCount() == 0)
		{
			pCmd->ReleaseServerLock();
			::PostMessage(MainFrame()->m_hWnd, WM_COMMAND, ID_APP_EXIT, 0);
		}
		else
		{
			TheApp()->m_RunClientWizOnly = FALSE;
			::ShowWindow(MainFrame()->m_hWnd, SW_RESTORE);
			MainFrame()->UpdateDepotandChangeViews(REDRILL, pCmd->GetServerKey());
		}
	}
	else
		MainFrame()->UpdateDepotandChangeViews(REDRILL, pCmd->GetServerKey());
	delete pCmd;
	return 0;
}

/*
	_________________________________________________________________
*/

LRESULT CClientListCtrl::OnP4ClientList(WPARAM wParam, LPARAM lParam)
{
	CCmd_Clients *pCmd= (CCmd_Clients *) wParam;

	if(!pCmd->GetError())
	{
		CString msg;
		CObList const *clients= pCmd->GetList();
		int count = clients->GetCount();

		SetRedraw(FALSE);
    	int index = 0;
		CString curclient = GET_P4REGPTR()->GetP4Client();
		CString defclient = GET_P4REGPTR()->GetP4Client(TRUE);
		CString user      = GET_P4REGPTR()->GetP4User();
		for(int subItem=0; subItem < CLIENT_MAXCOL; subItem++)
			m_ListAll.column[subItem].SetSize(clients->GetCount(), 100);
		for(POSITION pos= clients->GetHeadPosition(); pos != NULL; index++)
		{
			CP4Client *client=(CP4Client *) clients->GetNext(pos);
			InsertClient(client, index, &curclient, &defclient, &user);
			if ((index & 0x1FFF) == 0)
			{
				msg.FormatMessage(IDS_INSERTING_CLIENTS, count - index);
				MainFrame()->UpdateStatus(msg);
			}
		}
        SetRedraw(TRUE);

		msg.FormatMessage( IDS_NUMBER_OF_CLIENTS_n, index );
		AddToStatus( msg, SV_COMPLETION );

		ReSort();

		// Make sure previous item is re-selected
		if(clients->GetCount() > 0)
		{
			int i = FindInList(m_Active.IsEmpty() ? GET_P4REGPTR()->GetP4Client() : m_Active);
			if (i < 0)	i=0;
			SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			EnsureVisible(i, FALSE);
		}

		CP4ListCtrl::SetUpdateDone();
		// Notify the mainframe that we have finished getting the clients,
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


/*
	_________________________________________________________________

	Sort callback, not in class
	_________________________________________________________________
*/

int CClientListCtrl::OnCompareItems(LPARAM lParam1, LPARAM lParam2, int subItem)
{
    ASSERT(lParam1 && lParam2);
    CP4Client const *client1 = (CP4Client const *)lParam1;
    CP4Client const *client2 = (CP4Client const *)lParam2;
	CString txt1, txt2;
	switch(subItem)
	{
	case CLIENT_NAME:
		txt1 = client1->GetClientName();
		txt2 = client2->GetClientName();
		break;

	case CLIENT_OWNER:
		txt1 = client1->GetOwner();
		txt2 = client2->GetOwner();
		break;

	case CLIENT_HOST:
		txt1 = client1->GetHost();
		txt2 = client2->GetHost();
		break;

	case CLIENT_ACCESSDATE:
		txt1 = client1->GetDate();
		txt2 = client2->GetDate();
		ConvertDates( txt1, txt2 );
		break;

	case CLIENT_ROOT:
		txt1 = client1->GetRoot();
		txt2 = client2->GetRoot();
		break;

	case CLIENT_DESC:
		txt1 = client1->GetDescription();
		txt2 = client2->GetDescription();
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


/*
	_________________________________________________________________
	
	when creating a new client or switching to another one.
	_________________________________________________________________
*/

BOOL CClientListCtrl::SetP4Client( BOOL bAlways )
{
	//		not that we'd ever get here... but if they are the same, there 
	//		is nothing to do, so bail.
	//
	if ( !bAlways && ( m_Active == GET_P4REGPTR()->GetP4Client() ) )
		return FALSE;

	if (GET_P4REGPTR()->GetExpandFlag() == 1)
		GET_P4REGPTR()->AddMRUPcuPath(MainFrame()->GetCurrentItemPath());

	//		okay, change the session's client. 
	//		(either we're setting the active client back to the registered one 
	//		after using another client, or we are officially resetting 
	//		the registered client to the new active one.)
	//

	if( !GET_P4REGPTR()->SetP4Client( m_Active, TRUE, FALSE, FALSE ) )
	{
		AfxMessageBox(IDS_UNABLE_TO_WRITE_P4CLIENT_TO_THE_REGISTRY, MB_ICONEXCLAMATION);
		m_Active  = GET_P4REGPTR()->GetP4Client();
		return FALSE;
	}

	MainFrame()->UpdateCaption( ) ;	

	return TRUE;
}


/*
	_________________________________________________________________
*/

BOOL CClientListCtrl::AutoCreateClientSpec(LPCTSTR clientName, LPCTSTR clientRoot, 
										   BOOL bEdit, BOOL bTmpl, LPCTSTR tmplate)
{
	m_pNewSpec= new CP4Client;

	CCmd_EditSpec *pCmd= new CCmd_EditSpec;
	pCmd->Init(m_hWnd, RUN_ASYNC, HOLD_LOCK);
	if (bTmpl && !(CString(GET_P4REGPTR()->GetLocalCliTemplate()).IsEmpty())
	 && CString(tmplate) == CString(LOCALDEFTEMPLATE))
	{
		pCmd->SetUseLocalDefTmplate(TRUE);
		bTmpl = FALSE;
		bEdit = TRUE;
	}
	else
		pCmd->SetNewClientRoot(clientRoot);
	pCmd->SetAutoUpdateSpec(bEdit ? FALSE : TRUE);
	BOOL b;
	if (bTmpl)
		b = pCmd->Run( P4CLIENT_SPEC, clientName, tmplate, m_pNewSpec );
	else
		b = pCmd->Run( P4CLIENT_SPEC, clientName, m_pNewSpec );
	if( b )
	{
		MainFrame()->UpdateStatus( LoadStringResource(IDS_EDITING_CLIENT_SPEC) ) ;
		return TRUE;
	}
	delete pCmd;
	delete m_pNewSpec;
	return FALSE;
}

void CClientListCtrl::OnEditSpec( LPCTSTR clientName, BOOL bUseDefTemplate /*=FALSE*/ )
{
	if (m_EditInProgress)
	{
		CantEditRightNow(IDS_CLIENT);
		return;
	}

	m_pNewSpec= new CP4Client;

	CCmd_EditSpec *pCmd= new CCmd_EditSpec;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK);
	pCmd->SetUseLocalDefTmplate(bUseDefTemplate);
	if( pCmd->Run( P4CLIENT_SPEC, clientName, m_pNewSpec ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_EDITING_CLIENT_SPEC) ) ;
	else
	{
		delete pCmd;
		delete m_pNewSpec;
	}
}

void CClientListCtrl::OnEditTemplate( LPCTSTR templateName )
{
    // Get a list of all clients, so the dialog can check
    // for a bungle like trying to create a client that
    // already exists
    CStringList list;
    int index=-1;
	m_NewClicked=m_NewClient=FALSE;
	if( GET_SERVERLEVEL() >= 6 )
		list.AddHead( templateName );
	else
	{
		while(1)
		{
			index=GetNextItem(index, LVNI_ALL);
			if( index == -1)
				break;

			TCHAR str[ 1024 ];
			GetItemText(index, 0, str, 1024 );
			list.AddHead(str);
		}
    }

    // Prompt for the new client name, using p4client as a
    // default value
    CTemplateNameDlg dlg;
    dlg.SetTemplateName(templateName);
    dlg.SetVerbotenSpecNames(&list);
    dlg.SetNewSpecName(GET_P4REGPTR()->GetP4Client( ));
	dlg.SetSpecType( P4CLIENT_SPEC );

    if(dlg.DoModal() != IDCANCEL)
    {
		CString saveActive = m_Active;
        m_Active = dlg.GetNewSpecName();	//set and switch to new client
		if (SetP4Client( TRUE ))
		{
			m_pNewSpec= new CP4Client;
			CCmd_EditSpec *pCmd= new CCmd_EditSpec;
			pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK);
			if( pCmd->Run( P4CLIENT_SPEC, m_Active, templateName, m_pNewSpec ) )
			{
				MainFrame()->UpdateStatus( LoadStringResource(IDS_EDITING_CLIENT_SPEC) ) ;
				m_NewClicked = m_NewClient = TRUE;
				m_ClientNew = m_Active;
			}
			else
			{
				delete pCmd;
				delete m_pNewSpec;
				m_Active = saveActive;
				SetP4Client( TRUE );
			}
		}
    }
}

/////////////////////////////////////////////////////////////////////
// OLE drag-drop support, to accept depot files or folders
// which will define a view to be used to filter the submitted
// changes that this window displays
/////////////////////////////////////////////////////////////////////

DROPEFFECT CClientListCtrl::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	m_DropEffect=DROPEFFECT_NONE;
	m_DragDataFormat=0;
	CString fname;

	// Dont allow a drop if the server is busy, since a drop immediately attempts to
	// invoke a server command
	// Also don't allow a drop if we are in local syntax because
	// the left side of the view needs to be specified in depot syntax
	if(SERVER_BUSY() || m_EditInProgress || GET_P4REGPTR( )->ShowEntireDepot( ) > SDF_DEPOT)
		return DROPEFFECT_NONE;
		
	if(pDataObject->IsDataAvailable( (unsigned short) m_CF_DEPOT))
	{
		m_DropEffect=DROPEFFECT_COPY;
		m_DragDataFormat=m_CF_DEPOT;
	}
	
	return m_DropEffect;
}

DROPEFFECT CClientListCtrl::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	// Dont allow a drop if the server is busy, since a drop immediately attempts to
	// invoke a server command
	if(SERVER_BUSY() || m_EditInProgress)
		m_DropEffect= DROPEFFECT_NONE;
		
	return m_DropEffect;
}

BOOL CClientListCtrl::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	CString fname;
	
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
		::SendMessage(m_depotWnd, WM_DROPTARGET, CLIENTVIEW, MAKELPARAM(point.x,point.y));
		return TRUE;
	}
	// Return false, so depot window doesnt start a file-open operation
	return FALSE;
}

int CClientListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CP4ListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	CStringArray colnames;
	colnames.Add ( LoadStringResource(IDS_CLIENT) );
	colnames.Add ( LoadStringResource(IDS_OWNER) );
	colnames.Add ( LoadStringResource(IDS_HOST) );
	colnames.Add ( LoadStringResource(IDS_DATE) );
	colnames.Add ( LoadStringResource(IDS_ROOT) );
	colnames.Add ( LoadStringResource(IDS_DESCRIPTION) );
	ASSERT( CLIENT_MAXCOL == colnames.GetSize( ) );
    //FIXTHIS: default column widths should be in resource
	int width[ CLIENT_MAXCOL ]={90,90,90,90,150,150};
	RestoreSavedWidths(width, colnames.GetSize( ), sRegValue_ClientList );
	InsertColumnHeaders( colnames, width );

	return 0;
}

void CClientListCtrl::OnUpdateAddToClientView(CCmdUI* pCmdUI) 
{
	CString txt;
    txt.FormatMessage(IDS_ADD_FILES_TO_CLIENT_s_VIEW, GET_P4REGPTR()->GetP4Client());
	pCmdUI->SetText ( txt );
	pCmdUI->Enable( !SERVER_BUSY() && !m_EditInProgress
		&& GET_P4REGPTR()->ShowEntireDepot() <= SDF_DEPOT );
}

void CClientListCtrl::OnAddToClientView() 
{
    if( ! SERVER_BUSY() && IDYES == AfxMessageBox(IDS_CONFIRMADDTOVIEW, MB_YESNO|MB_ICONQUESTION))
    {
	    ::SendMessage(m_depotWnd, WM_GETSELLIST, (WPARAM) &m_AddToViewList, 0);
	    if(m_AddToViewList.GetCount() > 0)
		{
			OnClientEditmy();
		}
	}
}

BOOL CClientListCtrl::TryDragDrop( )
{
	// Store the client this is from
	m_DragFromItemName = GetSelectedItemText();

	m_OLESource.DelayRenderData( (unsigned short) m_CF_CLIENT);

	return m_OLESource.DoDragDrop(DROPEFFECT_COPY, &m_DragSourceRect, NULL)
			== DROPEFFECT_NONE ? FALSE : TRUE;
}

void CClientListCtrl::OnNewClient(WPARAM wParam, LPARAM lParam)
{
	if (!IsClear())
	{
		CString oldclient = wParam ? m_OldDefClient : m_OldCurClient;
		CString newclient = GET_P4REGPTR()->GetP4Client((BOOL)wParam);
		CString curclient = GET_P4REGPTR()->GetP4Client();
		CString defclient = GET_P4REGPTR()->GetP4Client(TRUE);
		LV_ITEM lvItem;
		lvItem.mask = LVIF_IMAGE;
		lvItem.iSubItem = 0;
		lvItem.state = lvItem.stateMask = 0;

		lvItem.iItem = FindInList(oldclient);
		if(lvItem.iItem > -1)
		{
			lvItem.iImage = CP4ViewImageList::GetClientIndex(false,
				oldclient == defclient);
			SetItem(&lvItem );
		}

		lvItem.iItem = FindInList(newclient);
		if(lvItem.iItem > -1)
		{
	        lvItem.iImage = CP4ViewImageList::GetClientIndex(true,
				newclient == defclient);
			SetItem(&lvItem );
		}
		else if (GET_P4REGPTR()->GetClientFilteredFlags())
		{
			int index = FindInListAll(newclient);
			if (index > -1)
			{
				CP4Client * client = new CP4Client;
				client->Create(m_ListAll.column[CLIENT_NAME].GetAt(index),
							  m_ListAll.column[CLIENT_OWNER].GetAt(index),
							  m_ListAll.column[CLIENT_HOST].GetAt(index),
							  m_ListAll.column[CLIENT_ACCESSDATE].GetAt(index),
							  m_ListAll.column[CLIENT_ROOT].GetAt(index),
							  m_ListAll.column[CLIENT_DESC].GetAt(index));
				m_ListAll.column[0].SetAt(index, _T("@@"));	// Removes from list of all clients
				InsertClient(client, GetItemCount(), &curclient, &defclient);
				ReSort();
			}
		}
		if (wParam)
			m_OldDefClient = newclient;
		else
			m_OldCurClient = newclient;
	}
}

void CClientListCtrl::OnPerforceOptions()
{
	MainFrame()->OnPerforceOptions(TRUE, FALSE, IDS_PAGE_CLIENT);
}

void CClientListCtrl::OnUpdateDiff2Clients(CCmdUI* pCmdUI) 
{
	// For now this feature is permanently turned off
	// Remove the 'FALSE' and the /* */ to turn on again
	//
	// However for now this feature just uses File > Diff 2
	// If this is going out, the Client menu should have Diff 2
	pCmdUI->Enable( FALSE /*!SERVER_BUSY()*/ );
}

void CClientListCtrl::OnDiff2Clients()
{
	CDiff2ObjsDlg dlg;
	dlg.m_Type = COMBO_CLIENT;
	m_Active=GetSelectedItemText();
	if (!m_Active.IsEmpty())
		dlg.m_Edit1 = m_Active;
	SET_APP_HALTED(TRUE);
	int rc=dlg.DoModal();  
	SET_APP_HALTED(FALSE);
	if (rc == IDOK)
	{
		CString x = "";
		CCmd_Diff2 *pCmd= new CCmd_Diff2;
		pCmd->Init( m_depotWnd, RUN_ASYNC);
		pCmd->SetOutput2Dlg(GET_P4REGPTR()->Diff2InDialog());
		if( pCmd->Run( _T("//...@") + dlg.m_Edit1, _T("//...@") + dlg.m_Edit2, 
						0, 0, x, x, FALSE, FALSE, TRUE) )
			MainFrame()->UpdateStatus( LoadStringResource(IDS_DIFFING_FILES) );
		else
			delete pCmd;
	}
}

void CClientListCtrl::OnUpdateFilterClients(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY()) );
}

void CClientListCtrl::OnFilterClients()
{
	CClientFilterDlg dlg;
	if(dlg.DoModal() == IDCANCEL)
		return;
	OnViewUpdate();
}

void CClientListCtrl::OnUpdateClearClientFilter(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY() 
					&& GET_P4REGPTR()->GetClientFilteredFlags()) );
}

void CClientListCtrl::OnClearClientFilter()
{
	GET_P4REGPTR()->SetClientFilteredFlags(0);
	OnViewUpdate();
}
