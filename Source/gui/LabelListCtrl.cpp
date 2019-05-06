// LabelListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "DepotView.h"
#include "LabelView.h"
#include "MainFrm.h"
#include "SpecDescDlg.h"
#include "P4SpecDlg.h"   
#include "FilterByOwnerDlg.h"
#include "Diff2ObjsDlg.h"
#include "cmd_editspec.h"
#include "cmd_delete.h"
#include "cmd_diff2.h"
#include "cmd_get.h"
#include "cmd_labels.h"
#include "cmd_labelsynch.h"
#include "cmd_fstat.h"
#include "templatenamedlg.h"
#include "ImageList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define IMG_INDEX(x) (x-IDB_PERFORCE)

//		the column names and their positions.
//
enum LabelSubItem
{
	LABEL_NAME,
	LABEL_OWNER,
	LABEL_OPTIONS,
	LABEL_UPDATEDATE,
	LABEL_DESC,
	LABEL_MAXCOL
};

/////////////////////////////////////////////////////////////////////////////
// CLabelListCtrl

IMPLEMENT_DYNCREATE(CLabelListCtrl, CP4ListCtrl)

BEGIN_MESSAGE_MAP(CLabelListCtrl, CP4ListCtrl)
	ON_COMMAND(ID_LABEL_DELETE, OnLabelDelete)
	ON_UPDATE_COMMAND_UI(ID_LABEL_DELETE, OnUpdateLabelDelete)
	ON_COMMAND(ID_LABEL_EDIT_SPEC, OnLabelEditspec)
	ON_COMMAND(ID_LABEL_NEW, OnLabelNew)
	ON_UPDATE_COMMAND_UI(ID_LABEL_NEW, OnUpdateLabelNew)
	ON_UPDATE_COMMAND_UI(ID_VIEW_UPDATE_RIGHT, OnUpdateViewUpdate)
	ON_WM_CONTEXTMENU()
	ON_UPDATE_COMMAND_UI(ID_LABEL_SYNC, OnUpdateLabelSync)
	ON_COMMAND(ID_LABEL_SYNC, OnLabelSync)
	ON_UPDATE_COMMAND_UI(ID_LABEL_SYNC_CLIENT, OnUpdateLabelSyncClient)
	ON_COMMAND(ID_LABEL_SYNC_CLIENT, OnLabelSyncClient)
	ON_UPDATE_COMMAND_UI(ID_LABEL_ADDTOLABELVIEW, OnUpdateAddToLabelView)
	ON_COMMAND(ID_LABEL_ADDTOLABELVIEW, OnAddToLabelView)
	ON_UPDATE_COMMAND_UI(ID_LABEL_DESCRIBE, OnUpdateLabelDescribe)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_LABEL_LISTFILES, OnLabelListfiles)
	ON_UPDATE_COMMAND_UI(ID_LABEL_LISTFILES, OnUpdateLabelListfiles)
	ON_COMMAND(ID_LABEL_DELETEFILES, OnLabelDeletefiles)
	ON_UPDATE_COMMAND_UI(ID_LABEL_DELETEFILES, OnUpdateLabelDeletefiles)
	ON_UPDATE_COMMAND_UI(ID_LABEL_EDIT_SPEC, OnUpdateLabelEditSpec)
	ON_COMMAND(ID_LABEL_DESCRIBE, OnDescribe)
	ON_COMMAND(ID_VIEW_UPDATE_RIGHT, OnViewUpdate)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnDeleteitem)
	ON_UPDATE_COMMAND_UI(ID_LABEL_TEMPLATE, OnUpdateLabelTemplate)
	ON_COMMAND(ID_LABEL_TEMPLATE, OnLabelTemplate)
	ON_COMMAND(ID_LABELFILTER_CLEAR, OnLabelFilterClear)
	ON_COMMAND(ID_LABELFILTER_CLEARVIEW, OnLabelFilterClear)
	ON_UPDATE_COMMAND_UI(ID_LABELFILTER_CLEARVIEW, OnUpdateLabelFilterClearview)
	ON_COMMAND(ID_LABELFILTER_SET, OnLabelFilterSetview)
	ON_COMMAND(ID_LABELFILTER_SETVIEW, OnLabelFilterSetview)
	ON_UPDATE_COMMAND_UI(ID_LABELFILTER_SETVIEW, OnUpdateLabelFilterSetview)
	ON_COMMAND(ID_LABELFILTER_SETVIEWREV, OnLabelFilterSetviewRev)
	ON_UPDATE_COMMAND_UI(ID_LABELFILTER_SETVIEWREV, OnUpdateLabelFilterSetviewRev)
	ON_COMMAND(ID_LABELDD_SYNC, OnLabelDDsetSync)
	ON_COMMAND(ID_LABELDD_DELETEFILES, OnLabelDDsetDeleteFiles)
	ON_COMMAND(ID_LABELDD_SYNC_CLIENT, OnLabelDDsetSyncClient)
	ON_COMMAND(ID_LABELDD_ADDTOLABELVIEW, OnLabelDDAddToLabelView)
	ON_COMMAND(ID_LABELFILTERDD_SETVIEW, OnLabelDDsetFilterSetview)
	ON_COMMAND(ID_LABELFILTERDD_SETVIEWREV, OnLabelDDsetFilterSetviewRev)
	ON_COMMAND(ID_LABELDD_SHOWMENU, OnLabelDDsetShowMenu)
	ON_UPDATE_COMMAND_UI(ID_LABELDD_SHOWMENU, OnUpdateLabelDDsetShowMenu)
	ON_COMMAND(ID_PERFORCE_OPTIONS, OnPerforceOptions)
	ON_UPDATE_COMMAND_UI(ID_FILTERBYOWNER, OnUpdateFilterByOwner)
	ON_COMMAND(ID_FILTERBYOWNER, OnFilterByOwner)
	ON_UPDATE_COMMAND_UI(ID_CLEAROWNERFILTER, OnUpdateClearFilterByOwner)
	ON_COMMAND(ID_CLEAROWNERFILTER, OnClearFilterByOwner)
	ON_UPDATE_COMMAND_UI(ID_LABEL_DIFF2, OnUpdateDiff2Labels)
	ON_COMMAND(ID_LABEL_DIFF2, OnDiff2Labels)
	ON_WM_CREATE()
	ON_MESSAGE(WM_P4LABELS, OnP4LabelList )
	ON_MESSAGE(WM_P4FSTAT, OnP4LabelContents )
	ON_MESSAGE(WM_P4EDITSPEC, OnP4LabelSpec )
	ON_MESSAGE(WM_P4ENDSPECEDIT, OnP4EndSpecEdit )
	ON_MESSAGE(WM_P4DELETE, OnP4Delete )
	ON_MESSAGE(WM_DOLABELSYNCCLI, OnDoLabelSyncCli )
	ON_MESSAGE(WM_DOLABELDELFILES, OnDoLabelDelFiles )
	ON_MESSAGE(WM_DOLABELSYNC, OnDoLabelSync )
	ON_MESSAGE(WM_P4LABELSYNC, OnP4LabelSync )
	ON_MESSAGE(WM_P4DESCRIBE, OnP4Describe )
    ON_MESSAGE(WM_P4ENDDESCRIBE, OnP4EndDescribe )
	ON_MESSAGE(WM_LABELDROPMENU, OnLabelDropMenu )
	ON_MESSAGE(WM_LABELOUFC, CallOnUpdateLabelFilterClearview)
	ON_MESSAGE(WM_NEWUSER, NewUser )
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()

CLabelListCtrl::CLabelListCtrl()
{
	m_User = GET_P4REGPTR()->GetP4User();
	m_SortAscending=TRUE;
	m_LastSortCol=0;
	m_AddReplaceDlg = 0;
	m_DelSyncDlg = 0;
	m_viewType = P4LABEL_SPEC;
	m_Need2DoNew = FALSE;
	m_FilterOwner = GET_P4REGPTR()->GetLabelFilterByOwner();
	m_FilterOwnerFlag = GET_P4REGPTR()->GetLabelFilterByOwnerFlag();
	m_FilterIncBlank = GET_P4REGPTR()->GetLabelFilterIncBlank();
	m_captionplain = LoadStringResource(IDS_PERFORCE_LABELS);
    m_caption = LoadStringResource(!m_FilterOwnerFlag 
			  ? IDS_PERFORCE_LABELS : IDS_PERFORCE_FILTERED_LABELS_BY_OWNER);
	m_AnyBlankOwner = FALSE;
	CString format;
	format.LoadString(IDS_DRAGFROMDEPOT);
	m_CF_DEPOT=RegisterClipboardFormat(format);
}

CLabelListCtrl::~CLabelListCtrl()
{
}

/////////////////////////////////////////////////////////////////////////////
// CLabelListCtrl diagnostics

#ifdef _DEBUG
void CLabelListCtrl::AssertValid() const
{
	CP4ListCtrl::AssertValid();
}

void CLabelListCtrl::Dump(CDumpContext& dc) const
{
	CP4ListCtrl::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLabelListCtrl message handlers

void CLabelListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	int index = GetHitItem ( point );
	if( index > -1 )
	{
		// If on an item, so select it and run edit
		SetItemState(index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_DROPHILITED|LVIS_SELECTED|LVIS_FOCUSED );
		OnDescribe();
	}
	else
		CP4ListCtrl::OnLButtonDblClk(nFlags, point);
}


void CLabelListCtrl::Clear()
{
	m_Active=GetSelectedItemText();
	if (!m_Active.IsEmpty())
		GET_P4REGPTR()->SetLastLabel(m_Active);

    SetRedraw(FALSE);
	DeleteAllItems();
    SetRedraw(TRUE);

	CP4ListCtrl::Clear();
}

void CLabelListCtrl::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	delete (CP4Label *) GetItemData(pNMListView->iItem);
			
	*pResult = 0;
}

void CLabelListCtrl::OnUpdateLabelSync(CCmdUI* pCmdUI) 
{
	CString sel = TruncateString(GetSelectedItemText(), 50);
    CString txt;
    txt.FormatMessage(IDS_ADD_REPLACE_FILES_IN_LABEL_s, sel);
	pCmdUI->SetText ( txt );
	pCmdUI->Enable(!SERVER_BUSY() && !sel.IsEmpty() 
		&& !SelectedItemIsLocked(LABEL_OPTIONS) && !MainFrame()->IsModlessUp());	
}

void CLabelListCtrl::OnLabelSync() 
{
	if (MainFrame()->IsModlessUp())
		return;

	CStringList list;
	m_Active= GetSelectedItemText();

	m_AddReplaceDlg = new CLabelAddReplace(this);
	if (!m_AddReplaceDlg)
	{
		ASSERT(0);
		AfxMessageBox(IDS_COULD_NOT_CREATE_LABEL_DIALOG_BOX, MB_ICONSTOP);
		return;
	}
	MainFrame()->SetModelessUp(TRUE);

	m_AddReplaceDlg->SetActive(m_AddReplaceName = m_Active);
	MainFrame()->AssembleDepotStringList(&list, TRUE);
	m_AddReplaceDlg->SetSelected(&list);
	m_AddReplaceDlg->SetOutputList(&m_AddReplaceList);
	m_AddReplaceDlg->m_FilesFlag = m_AddReplaceDlg->m_DefaultFlag 
								 = GET_P4REGPTR()->GetLabelAddRplDefault();
	m_AddReplaceDlg->m_RevFlag = 0;
	if (!m_AddReplaceDlg->Create(IDD_LABEL_ADD_REPLACE, this))	// display add/replace files dialog
	{
		delete m_AddReplaceDlg;
		MainFrame()->SetModelessUp(FALSE);
	}
}

LRESULT CLabelListCtrl::OnDoLabelSync(WPARAM wParam, LPARAM lParam)
{
	BOOL preview = FALSE;
	if (wParam == IDOK)
	{
		preview = !!lParam;
		if (!preview)
			GET_P4REGPTR()->SetLabelAddRplDefault(m_AddReplaceDlg->m_DefaultFlag);
		if (m_AddReplaceList.GetCount())
		{
			CCmd_LabelSynch *pCmd= new CCmd_LabelSynch;
			pCmd->Init( m_hWnd, RUN_ASYNC);
			if( pCmd->Run( m_AddReplaceName, &m_AddReplaceList, preview, FALSE, 
							m_AddReplaceDlg->m_FilesFlag==2 ) )
				MainFrame()->UpdateStatus( LoadStringResource(IDS_SYNCING_LABEL) );	
			else
				delete pCmd;
		}
		else AddToStatus(LoadStringResource(IDS_NOTHING_SELECTED_NOTHING_TO_DO));
	}
	if (m_AddReplaceDlg && !preview)
	{
		m_AddReplaceDlg->DestroyWindow();	// deletes m_AddReplaceDlg
		m_AddReplaceDlg = 0;
		MainFrame()->SetModelessUp(FALSE);
	}
	return 0;
}

void CLabelListCtrl::OnUpdateLabelSyncClient(CCmdUI* pCmdUI) 
{
	CString sel = TruncateString(GetSelectedItemText(), 50);
    CString txt;
    txt.FormatMessage(IDS_SYNC_CLIENT_s_TO_LABEL_s, 
        TruncateString(GET_P4REGPTR()->GetP4Client(), 50), sel);
	pCmdUI->SetText ( txt );
	pCmdUI->Enable(!SERVER_BUSY() && !sel.IsEmpty() && !MainFrame()->IsModlessUp());
}

void CLabelListCtrl::OnLabelSyncClient() 
{
	if (MainFrame()->IsModlessUp())
		return;

	m_Active = GetSelectedItemText();
	m_LabelFileCount=0;
	m_LabelFiles.Empty();

	m_DelSyncDlg = new CLabelDelSync(this);
	if (!m_DelSyncDlg)
	{
		ASSERT(0);
		AfxMessageBox(IDS_COULD_NOT_CREATE_LABEL_DIALOG_BOX, MB_ICONSTOP);
		return;
	}
	MainFrame()->SetModelessUp(TRUE);

	m_DelSyncDlg->SetActive(m_DelSyncName = m_Active);
	CString caption;
	caption.FormatMessage(IDS_SYNC_FILES_IN_LABEL_s, m_Active);
	m_DelSyncDlg->SetCaption( caption );
	m_DelSyncDlg->SetBtnText( LoadStringResource(IDS_SYNC_CLIENT_TO_SELECTED_FILE_REVISIONS) );
	MainFrame()->AssembleDepotStringList(&m_StringList, TRUE);
	m_DelSyncDlg->SetSelected(&m_StringList);
	m_DelSyncDlg->SetOutputList(&m_DelSyncList);
	m_DelSyncDlg->SetReturnMsg(WM_DOLABELSYNCCLI);
	if (!m_DelSyncDlg->Create(IDD_LABEL_DEL_SYNC, this))	// display the sync to label dialog box
	{
		delete m_DelSyncDlg;
		MainFrame()->SetModelessUp(FALSE);
	}
}

LRESULT CLabelListCtrl::OnDoLabelSyncCli(WPARAM wParam, LPARAM lParam)
{
	BOOL preview = FALSE;
	if (wParam == IDOK)
	{
		preview = !!lParam;
		if (m_DelSyncList.GetCount())
		{
			CCmd_Get *pCmd= new CCmd_Get;
			pCmd->Init( m_depotWnd, RUN_ASYNC );
			if( !pCmd->Run( &m_DelSyncList, preview ) )
				delete pCmd;
		}
		else
			AddToStatus(LoadStringResource(IDS_NOTHING_SELECTED_NOTHING_TO_DO));
	}
	if (m_DelSyncDlg && !preview)
	{
		m_DelSyncDlg->DestroyWindow();	// deletes m_DelSyncDlg
		m_DelSyncDlg = 0;
		MainFrame()->SetModelessUp(FALSE);
	}
	return 0;
}

void CLabelListCtrl::OnUpdateLabelDeletefiles(CCmdUI* pCmdUI) 
{
	CString selUser = TruncateString(GetSelectedItemText(), 50);
    CString txt;
    txt.FormatMessage(IDS_DELETE_FILES_AT_s, selUser);
	pCmdUI->SetText ( txt );
	pCmdUI->Enable(!SERVER_BUSY() && !selUser.IsEmpty()
		&& !SelectedItemIsLocked(LABEL_OPTIONS) && !MainFrame()->IsModlessUp());
}

void CLabelListCtrl::OnLabelDeletefiles() 
{
	if (MainFrame()->IsModlessUp())
		return;

	m_Active = GetSelectedItemText();
	m_LabelFileCount=0;
	m_LabelFiles.Empty();

	m_DelSyncDlg = new CLabelDelSync(this);
	if (!m_DelSyncDlg)
	{
		ASSERT(0);
		AfxMessageBox(IDS_COULD_NOT_CREATE_LABEL_DIALOG_BOX, MB_ICONSTOP);
		return;
	}
	MainFrame()->SetModelessUp(TRUE);

	m_DelSyncDlg->SetActive(m_DelSyncName = m_Active);
	CString caption;
	caption.FormatMessage(IDS_DELETE_FILES_FROM_LIST_IN_LABEL_s, m_Active);
	m_DelSyncDlg->SetCaption( caption );
	m_DelSyncDlg->SetBtnText( LoadStringResource(IDS_DELETE_CHECKED_FILES_FROM_LIST_IN_LABEL) );
	MainFrame()->AssembleDepotStringList(&m_StringList, TRUE);
	m_DelSyncDlg->SetSelected(&m_StringList);
	m_DelSyncDlg->SetOutputList(&m_DelSyncList);
	m_DelSyncDlg->SetReturnMsg(WM_DOLABELDELFILES);
	if (!m_DelSyncDlg->Create(IDD_LABEL_DEL_SYNC, this))	// display the remove files from label dialog
	{
		delete m_DelSyncDlg;
		MainFrame()->SetModelessUp(FALSE);
	}
}

LRESULT CLabelListCtrl::OnDoLabelDelFiles(WPARAM wParam, LPARAM lParam)
{
	BOOL preview = FALSE;
	if (wParam == IDOK)
	{
		preview = !!lParam;
		if (m_DelSyncList.GetCount())
		{
			POSITION pos2;
			for (POSITION pos1 = m_DelSyncList.GetHeadPosition(); ( pos2 = pos1 ) != NULL; )
			{
				CString str = m_DelSyncList.GetNext(pos1);
				int i = str.ReverseFind(_T('#'));
				if (i != -1)
					str = str.Left(i);
				str += _T("#none");
				m_DelSyncList.SetAt(pos2, str);
			}
			CCmd_LabelSynch *pCmd= new CCmd_LabelSynch;
			pCmd->Init( m_hWnd, RUN_ASYNC);
			if( pCmd->Run( m_DelSyncName, &m_DelSyncList, preview, FALSE, FALSE ) )
				MainFrame()->UpdateStatus( LoadStringResource(IDS_SYNCING_LABEL) );	
			else
				delete pCmd;
		}
		else
			AddToStatus(LoadStringResource(IDS_NOTHING_SELECTED_NOTHING_TO_DO));
	}
	if (m_DelSyncDlg && !preview)
	{
		m_DelSyncDlg->DestroyWindow();	// deletes m_DelSyncDlg
		m_DelSyncDlg = 0;
		MainFrame()->SetModelessUp(FALSE);
	}
	return 0;
}

void CLabelListCtrl::OnUpdateAddToLabelView(CCmdUI* pCmdUI) 
{
	CString sel = TruncateString(GetSelectedItemText(), 50);
    CString txt;
    txt.FormatMessage(IDS_ADDTOLABELVIEW_s, sel);
	pCmdUI->SetText ( txt );
	pCmdUI->Enable(!SERVER_BUSY() && !sel.IsEmpty() 
		&& !m_EditInProgress
		&& !SelectedItemIsLocked(LABEL_OPTIONS));
}

void CLabelListCtrl::OnAddToLabelView() 
{
	CString txt;
	txt.FormatMessage(IDS_CONFIRMADDTOLABELVIEW_s, GetSelectedItemText());
    if( !SERVER_BUSY() && !m_EditInProgress
		&& IDYES == AfxMessageBox(txt, MB_YESNO|MB_ICONQUESTION))
    {
	    ::SendMessage(m_depotWnd, WM_GETSELLIST, (WPARAM) &m_AddToViewList, 0);
	    if(m_AddToViewList.GetCount() > 0)
		{
			OnLabelEditspec();
		}
	}
}

void CLabelListCtrl::OnUpdateLabelDelete(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( OnUpdateShowMenuItem( pCmdUI, IDS_DELETE_s ) && !SelectedItemIsLocked(LABEL_OPTIONS) );	
}

void CLabelListCtrl::OnLabelDelete() 
{
	OnDelete( P4LABEL_DEL );
}


void CLabelListCtrl::OnUpdateLabelEditSpec(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( OnUpdateShowMenuItem( pCmdUI, IDS_EDITSPEC_s ) && !m_EditInProgress
		&& (!SelectedItemIsLocked(LABEL_OPTIONS) 
		  || !GetSelectedItemOwner(LABEL_OWNER).CompareNoCase(GET_P4REGPTR()->GetP4User())) );
}


void CLabelListCtrl::EditTheSpec(CString *name)
{
	m_Active=*name;
	m_NewLabel=FALSE;
	EditSpec( m_Active );	
}

void CLabelListCtrl::OnLabelEditspec() 
{
	m_Active=GetSelectedItemText();
	m_NewLabel=FALSE;
	EditSpec( m_Active );	
}

void CLabelListCtrl::OnUpdateLabelNew(CCmdUI* pCmdUI) 
{
	pCmdUI->SetText ( LoadStringResource( IDS_NEW ) );
	pCmdUI->Enable(!SERVER_BUSY() && !m_EditInProgress);	
	m_Need2DoNew = FALSE;
}

void CLabelListCtrl::OnLabelNew() 
{
	MainFrame()->ViewLabels();
	if (SERVER_BUSY())
		m_Need2DoNew = TRUE;
	else
	{
		m_Need2DoNew = FALSE;
		m_Active.Empty();
		m_NewLabel=TRUE;
		EditSpec( NEWSPECNAME );
	}
}

void CLabelListCtrl::OnUpdateViewUpdate(CCmdUI* pCmdUI) 
{
//	pCmdUI->SetText ( SetTextToRefresh( ) );
	pCmdUI->Enable(!SERVER_BUSY() && !MainFrame()->IsModlessUp());
}


void CLabelListCtrl::OnViewUpdate() 
{
	m_Active = GetSelectedItemText();
	CCmd_Labels *pCmd= new CCmd_Labels;
	pCmd->Init( m_hWnd, RUN_ASYNC);

	if (GET_SERVERLEVEL() >= 11)
	{
		// Make a copy of the filter view, because CCmd_Labels will
		// destroy that copy
		POSITION pos=m_LabelFilterView.GetHeadPosition();
		m_StrList.RemoveAll();
		while(pos != NULL)
			m_StrList.AddTail(m_LabelFilterView.GetNext(pos));
	}
	else if (m_LabelFilterView.GetCount())
	{
        OnLabelFilterClear();
		return;
	}

	if( pCmd->Run( &m_StrList ) )
	{
		MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_LABEL_LISTING) );
		MainFrame()->SetLabelUpdateTime(GetTickCount());
		Clear();		
		CP4ListCtrl::OnViewUpdate();
	}
	else
		delete pCmd;
}

void CLabelListCtrl::OnUpdateLabelListfiles(CCmdUI* pCmdUI) 
{
	CString selUser = TruncateString(GetSelectedItemText(), 50);
    CString txt;
    txt.FormatMessage(IDS_LIST_FILES_AT_s, selUser);
	pCmdUI->SetText ( txt );
	pCmdUI->Enable(!SERVER_BUSY() && !selUser.IsEmpty());	
}

void CLabelListCtrl::OnLabelListfiles() 
{
	m_Active = GetSelectedItemText();
	m_LabelFileCount=0;
	m_LabelFiles.Empty();
	m_LabelFilesInDialog = GET_P4REGPTR()->LabelFilesInDialog( );

	CString spec;
	spec.Format(_T("//...@%s"), m_Active);

	// Call Fstat, w/ suppress==FALSE
	CCmd_Fstat *pCmd= new CCmd_Fstat;
	pCmd->Init( m_hWnd, RUN_ASYNC, LOSE_LOCK);

	//		okay, this is weird, but let's set show entire depot
	//		to true, since we want this command to 
	//		read 'p4 fstat //...@mynumber WITHOUT the -C
	//		that would run otherwise. 
	//		after all, we all the files to show, not just
	//		the ones on the client view.
	//
	BOOL bshowEntireDepot = TRUE;
	if( pCmd->Run( FALSE, spec, bshowEntireDepot, 0 ) )
	{
		MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_LABEL_CONTENTS) );
	}
	else
	{
		delete pCmd;
		MainFrame()->ClearStatus();
	}
}

void CLabelListCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	//		make sure window is active
	//
	GetParentFrame()->ActivateFrame();

	// If one of the label modeless dialogs is up, don't display a context menu
	// instead set the focus to the modeless dialog.
	if (m_AddReplaceDlg)
	{
		m_AddReplaceDlg->SetFocus();
		return;
	}
	if (m_DelSyncDlg)
	{
		m_DelSyncDlg->SetFocus();
		return;
	}

	///////////////////////////////
	// See ContextMenuRules.txt for order of menu commands!

	//		create an empty context menu
	//
	CP4Menu popMenu;
	popMenu.CreatePopupMenu();

	int	index;
    SetIndexAndPoint( index, point );

	// Can always create new label
	popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_LABEL_NEW, LoadStringResource(ID_LABEL_NEW) );

	if(index != -1)
	{
		// Make sure item is selected
		SetItemState(index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_DROPHILITED|LVIS_SELECTED|LVIS_FOCUSED );

		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_LABEL_EDIT_SPEC, LoadStringResource(ID_LABEL_EDIT_SPEC) );
		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_LABEL_DESCRIBE, LoadStringResource(ID_LABEL_DESCRIBE) );
		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_LABEL_DELETE, LoadStringResource(ID_LABEL_DELETE) );
		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_LABEL_TEMPLATE, LoadStringResource(ID_LABEL_TEMPLATE) );
		popMenu.AppendMenu(MF_SEPARATOR);
		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_LABEL_LISTFILES, LoadStringResource(ID_LABEL_LISTFILES) );
		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_LABEL_SYNC, LoadStringResource(ID_LABEL_SYNC) );
		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_LABEL_DELETEFILES, LoadStringResource(ID_LABEL_DELETEFILES) );
		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_LABEL_SYNC_CLIENT, LoadStringResource(ID_LABEL_SYNC_CLIENT) );
		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_LABEL_ADDTOLABELVIEW, LoadStringResource(ID_LABEL_ADDTOLABELVIEW) );
		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_LABEL_DIFF2, LoadStringResource(IDS_LABEL_DIFF2) );
	}	

	popMenu.AppendMenu( MF_SEPARATOR );
	if(m_LabelFilterView.GetCount() > 0)
		popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_LABELFILTER_CLEARVIEW, LoadStringResource( IDS_CLEARFILTER ) );
	popMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_VIEW_UPDATE, LoadStringResource(IDS_REFRESH) );
	
	MainFrame()->AddToolsToContextMenu(&popMenu);

	popMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,	point.x, point.y, AfxGetMainWnd());
}

void CLabelListCtrl::InsertLabel(CP4Label *label, int index)
{
	// Add the data
	LV_ITEM lvItem;
	int iActualItem = -1;
	CString txt;

	ASSERT(label != NULL);
	m_iImage = CP4ViewImageList::VI_LABEL;

	for(int subItem=0; subItem < LABEL_MAXCOL; subItem++)
	{
		lvItem.mask=LVIF_TEXT | 
					((subItem==0) ? LVIF_IMAGE : 0) |
					((subItem==0) ? LVIF_PARAM : 0);

		lvItem.iItem= (subItem==0) ? index : iActualItem;
        ASSERT(lvItem.iItem != -1);
		lvItem.iSubItem= subItem;
		lvItem.iImage = CP4ViewImageList::VI_LABEL;
		lvItem.lParam=(LPARAM) label;

		switch(subItem)
		{
		case LABEL_NAME: 
			lvItem.pszText = const_cast<LPTSTR>( (LPCTSTR) label->GetLabelName()); break;
		case LABEL_OWNER: 
			lvItem.pszText = const_cast<LPTSTR>( (LPCTSTR) label->GetOwner()); break;
		case LABEL_OPTIONS: 
			lvItem.pszText = const_cast<LPTSTR>( (LPCTSTR) label->GetOptions()); break;
		case LABEL_UPDATEDATE: 
			lvItem.pszText = const_cast<LPTSTR>( (LPCTSTR) label->GetDate()); break;
		case LABEL_DESC:
			txt= PadCRs(label->GetDescription());
			lvItem.pszText = const_cast<LPTSTR>( (LPCTSTR) txt); break;
		}
			
		if(subItem==0)
			iActualItem=InsertItem(&lvItem);
		else
			SetItem(&lvItem);
	}
}


void CLabelListCtrl::UpdateLabel(CP4Label *label, int index)
{
	// After a spec edit, update the appropriate list item

	// First, switch the label data
	CP4Label *oldLabel= (CP4Label *) GetItemData(index);
	delete oldLabel;
	SetItemData(index, (LPARAM) label);

	// Then update the text
	SetItemText(index, LABEL_NAME, const_cast<LPTSTR>((LPCTSTR) label->GetLabelName()));
	SetItemText(index, LABEL_OWNER, const_cast<LPTSTR>((LPCTSTR) label->GetOwner()));
	SetItemText(index, LABEL_OPTIONS, const_cast<LPTSTR>((LPCTSTR) label->GetOptions()));
	SetItemText(index, LABEL_UPDATEDATE, const_cast<LPTSTR>((LPCTSTR) label->GetDate()));
	CString	txt= PadCRs(label->GetDescription());
	SetItemText(index, LABEL_DESC, const_cast<LPTSTR>((LPCTSTR)txt));
}

///////////////////////////////////////////////////
// Messages posted by server thread

LRESULT CLabelListCtrl::OnP4LabelContents(WPARAM wParam, LPARAM lParam)
{
	CString tmp;
    CCmd_Fstat *pCmd;

	if(lParam == 0)   // completion
	{
		pCmd= (CCmd_Fstat *) wParam;
		ASSERT_KINDOF(CCmd_Fstat,pCmd);

		if(!pCmd->GetError())
		{
			tmp.FormatMessage(IDS_LABEL_s_POINTS_TO_n_FILES, m_Active, m_LabelFileCount);
			AddToStatus(tmp, SV_COMPLETION);

			MainFrame()->ClearStatus();
			delete pCmd;
			if (m_LabelFilesInDialog && m_LabelFileCount)
			{
				m_LabelFiles += _T('\n') + tmp;
				int key;
				CSpecDescDlg *dlg = new CSpecDescDlg(this);
				dlg->SetIsModeless(TRUE);
				dlg->SetKey(key = pCmd->HaveServerLock()? pCmd->GetServerKey() : 0);
				dlg->SetDescription( m_LabelFiles );
				dlg->SetItemName( m_Active );
                CString caption;
                caption.FormatMessage(IDS_FILE_LIST_FOR_LABEL_s, m_Active);
				dlg->SetCaption( caption );
				dlg->SetViewType(P4LABEL_SPEC);
				if (!dlg->Create(IDD_SPECDESC, this))	// display the description dialog box
				{
					dlg->DestroyWindow();	// some error! clean up
					delete dlg;
				}
				m_LabelFiles.Empty();
			}
		}
		return 0;
	}
	else
	{
        // Pull a ptr to the command, as well as a batch of CP4FileStats
        // out of the wrapper
        CFstatWrapper *pWrap= (CFstatWrapper *) wParam;
        pCmd= (CCmd_Fstat *) pWrap->pCmd;
	    ASSERT_KINDOF(CCmd_Fstat, pCmd);
		CObList *list= (CObList *) pWrap->pList;
		ASSERT_KINDOF(CObList, list);
        
		POSITION pos= list->GetHeadPosition();
		while(pos != NULL)
		{
			// Get the filestats
			CP4FileStats *stats= (CP4FileStats *) list->GetNext(pos);
			ASSERT_KINDOF(CP4FileStats, stats);
		
			// Increment the counter
			m_LabelFileCount++;

			if (m_LabelFilesInDialog)
			{
				// Format the file, rev and type
				tmp.FormatMessage(IDS_s_n_s_CHANGELIST_n_s, 
					stats->GetFullDepotPath(),
					stats->GetHeadRev(),
					stats->GetHeadType(),
					stats->GetHeadChangeNum(),
					stats->GetActionStr(stats->GetHeadAction()));

				// And add to Description
				m_LabelFiles += tmp;
			}
			else
			{
				// Format the file, rev and type
				tmp.FormatMessage(IDS_LABEL_s_TO_s_n_s_CHANGELIST_n_s, 
					m_Active,
					stats->GetFullDepotPath(),
					stats->GetHeadRev(),
					stats->GetHeadType(),
					stats->GetHeadChangeNum(),
					stats->GetActionStr(stats->GetHeadAction()));

				// And add to the status window
				AddToStatus( tmp);
			}

			delete stats;

		} // while row batch not done

		delete list;
		delete pWrap;
		return 0;
	} // a batch of rows, we'll be called again so dont delete pCmd
}

LRESULT CLabelListCtrl::OnP4LabelSync(WPARAM wParam, LPARAM lParam)
{
	CCmd_LabelSynch *pCmd= (CCmd_LabelSynch *) wParam;

	CStringList *list= pCmd->GetList();
	ASSERT_KINDOF(CStringList, list);

	INT_PTR numFiles=list->GetCount();
	if (numFiles && GET_P4REGPTR()->LabelShowPreviewDetail() && pCmd->GetPreview())
	{
		POSITION pos=list->GetHeadPosition();
		while(pos != NULL)
			AddToStatus( list->GetNext(pos), SV_MSG);
	}
	CString txt;
	txt.FormatMessage(pCmd->GetPreview() 
		? IDS_LABEL_SYNCHRONIZED_n_FILE_REFS_WOULD_BE_UPDATED
		: IDS_LABEL_SYNCHRONIZED_n_FILE_REFERENCES_UPDATED, numFiles);
	AddToStatus( txt, SV_COMPLETION);
	
	MainFrame()->ClearStatus();
	delete pCmd;	
	return 0;
}

// Receives ak for label spec update
void CLabelListCtrl::OnUpdateLabelDescribe(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( OnUpdateShowMenuItem( pCmdUI, IDS_DESCRIBEIT_s ) );	
}

// Receives ak for label spec update
LRESULT CLabelListCtrl::OnP4LabelSpec(WPARAM wParam, LPARAM lParam)
{
	CCmd_EditSpec *pCmd= (CCmd_EditSpec *) wParam;

	pCmd->SetIsRequestingNew(m_NewLabel);
	pCmd->SetCaller(DYNAMIC_DOWNCAST(CView, GetParent()));
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
			specIn += _T("\t@\n");	//	A view that is @ means
									//	put a blank line in the list here.
		POSITION pos;
		CString filename;
		for (pos = m_AddToViewList.GetHeadPosition(); pos != NULL; )
		{
			filename = m_AddToViewList.GetNext(pos);
			if ((i = filename.Find(_T("<contains no files or folders>"))) != -1)
				filename = filename.Left(i-1) + _T("/...");
			if (filename.Find(_T(' ')))
				filename = _T('\"') + filename + _T('\"');
			specIn += _T('\t') + filename + _T('\n');
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
		if ( m_pNewSpec )
			delete m_pNewSpec;
		if (pCmd->HaveServerLock())
			pCmd->ReleaseServerLock();
		delete pCmd;
	}
	MainFrame()->ClearStatus();
	return 0;
}

LRESULT CLabelListCtrl::OnP4EndSpecEdit( WPARAM wParam, LPARAM lParam )
{
	CCmd_EditSpec *pCmd= (CCmd_EditSpec *) wParam;
	int index;

	if (lParam != IDCANCEL && lParam != IDABORT)
	{
		if (m_UpdateState == LIST_UPDATED)
		{
			if(m_NewLabel && FindInList(m_pNewSpec->GetLabelName()) == -1)
			{
				InsertLabel(m_pNewSpec, GetItemCount());
				ReSort();
				index= FindInList(m_pNewSpec->GetLabelName());
			}
			else
			{
				index= FindInList(m_pNewSpec->GetLabelName());
				UpdateLabel(m_pNewSpec, index);
			}
			EnsureVisible(index, TRUE );
			SetItemState(index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_DROPHILITED|LVIS_SELECTED|LVIS_FOCUSED );
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
	return 0;
}

LRESULT CLabelListCtrl::OnP4LabelList(WPARAM wParam, LPARAM lParam)
{
	CCmd_Labels *pCmd= (CCmd_Labels *) wParam;

	m_AnyBlankOwner = FALSE;
	if(!pCmd->GetError())
	{
		CString msg;
		CString filterowner;
    	CObList const *labels = pCmd->GetList();
		INT_PTR count = labels->GetCount();

        SetRedraw(FALSE);
    	int index = 0;
		if (m_FilterOwnerFlag)	// are we filtering by owner?
			filterowner = m_FilterOwnerFlag & 0x10 ? m_User : m_FilterOwner;
		for(POSITION pos= labels->GetHeadPosition(); pos != NULL; index++)
		{
        	CP4Label *label = (CP4Label *) labels->GetNext(pos);
			CString owner = label->GetOwner();
			if (owner.IsEmpty())
				m_AnyBlankOwner = TRUE;
			if (m_FilterOwnerFlag)	// are we filtering by owner?
			{
				if (owner.IsEmpty() && m_FilterIncBlank)
					;
				else if (filterowner != owner)
				{
					delete label;
					continue;
				}
			}
			InsertLabel(label, index);
			if ((index & 0x1FFF) == 0)
			{
				msg.FormatMessage(IDS_INSERTING_LABELS, count - index);
				MainFrame()->UpdateStatus(msg);
			}
		}
        SetRedraw(TRUE);

		msg.FormatMessage(IDS_NUMBER_OF_LABELS_n, index );
		AddToStatus( msg, SV_COMPLETION );

		// Make sure first item selected
		ReSort();
		if(labels->GetCount() > 0)
		{
			int i = FindInList(m_Active.IsEmpty() ? GET_P4REGPTR()->GetLastLabel() : m_Active);
			if (i < 0)	i=0;
			SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_DROPHILITED|LVIS_SELECTED|LVIS_FOCUSED);
			EnsureVisible(i, FALSE);
			m_Active=GetSelectedItemText();
		}
	
		CP4ListCtrl::SetUpdateDone();
		if (m_Need2DoNew)
			OnLabelNew();
		else
		{
			// Notify the mainframe that we have finished getting the labels,
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
// Sort callback

int CLabelListCtrl::OnCompareItems(LPARAM lParam1, LPARAM lParam2, int subItem)
{
    ASSERT(lParam1 && lParam2);
    CP4Label const *label1 = (CP4Label const*)lParam1;
    CP4Label const *label2 = (CP4Label const*)lParam2;


	CString txt1, txt2;
	switch(subItem)
	{
	case LABEL_NAME:	 // label name
		txt1= label1->GetLabelName();
		txt2= label2->GetLabelName();
		break;

	case LABEL_OWNER:	 // label owner
		txt1= label1->GetOwner();
		txt2= label2->GetOwner();
		break;

	case LABEL_OPTIONS:	 // label options
		txt1= label1->GetOptions();
		txt2= label2->GetOptions();
		break;

	case LABEL_UPDATEDATE:	 // label update date
		txt1= label1->GetDate();
		txt2= label2->GetDate();
		ConvertDates( txt1, txt2 );
		break;

	case LABEL_DESC:	 // label desc
		txt1= label1->GetDescription();
		txt2= label2->GetDescription();
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

void CLabelListCtrl::EditSpec( const CString &sItem )
{
	if (m_EditInProgress)
	{
		CantEditRightNow(IDS_LABEL);
		return;
	}

	m_pNewSpec= new CP4Label;
	CCmd_EditSpec *pCmd= new CCmd_EditSpec;
	pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK);
	if( pCmd->Run( P4LABEL_SPEC, sItem, m_pNewSpec ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_EDITING_LABEL_SPEC) );	
	else
	{
		delete pCmd;
		delete m_pNewSpec;
	}
}


void CLabelListCtrl::OnLabelTemplate()
{
	if( SERVER_BUSY() || m_Active.IsEmpty() || m_EditInProgress || GET_SERVERLEVEL() < 6)
		return;
	
	CStringList list;
    m_Active= GetSelectedItemText();
	list.AddHead( m_Active );

    // Prompt for the new label name
    CTemplateNameDlg dlg;
    dlg.SetTemplateName( m_Active );
    dlg.SetVerbotenSpecNames(&list);
    dlg.SetNewSpecName(_T(""));
	dlg.SetSpecType( P4LABEL_SPEC );

    if(dlg.DoModal() != IDCANCEL)
    {
        CString newName=dlg.GetNewSpecName();
		
		// Let OnP4LabelSpec() determine if this is a new label
		m_NewLabel=TRUE;
		
		m_pNewSpec= new CP4Label;
		CCmd_EditSpec *pCmd= new CCmd_EditSpec;
		pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK);
		if( pCmd->Run( P4LABEL_SPEC, newName, m_Active, m_pNewSpec ) )
			MainFrame()->UpdateStatus( LoadStringResource(IDS_EDITING_LABEL_SPEC) );	
		else
		{
			delete pCmd;
			delete m_pNewSpec;
		}
	}
}

void CLabelListCtrl::OnUpdateLabelTemplate(CCmdUI* pCmdUI)  
{
	m_Active = GetSelectedItemText();
		
    CString prompt;
    prompt.FormatMessage(IDS_CREATE_UPDATE_LABEL_USING_s_AS_TEMPLATE, TruncateString(m_Active, 50));
	pCmdUI->SetText ( prompt );

	pCmdUI->Enable(	GET_SERVERLEVEL() >= 6 && !SERVER_BUSY()
		&& !m_Active.IsEmpty() && !m_EditInProgress );	
}

/////////////////////////////////////////////////////////////////////
// Handlers for setting filters for labels view
/////////////////////////////////////////////////////////////////////

void CLabelListCtrl::OnLabelFilterClear() 
{
	ClearLabelFilter();
    m_caption = LoadStringResource(!m_FilterOwnerFlag 
			  ? IDS_PERFORCE_LABELS : IDS_PERFORCE_FILTERED_LABELS_BY_OWNER);
	OnViewReloadall();
}

void CLabelListCtrl::ClearLabelFilter()
{
	Clear();
	m_LabelFilterView.RemoveAll(); 
	m_caption = LoadStringResource(!m_FilterOwnerFlag 
			  ? IDS_PERFORCE_LABELS : IDS_PERFORCE_FILTERED_LABELS_BY_OWNER);
	CP4PaneContent::GetView()->SetCaption();
}

void CLabelListCtrl::OnViewReloadall() 
{
	CP4PaneContent::GetView()->SetCaption();
	m_Active = GetSelectedItemText();
	Clear();
	OnViewUpdate();
}

void CLabelListCtrl::OnLabelFilterSetview()
{
    if( !SERVER_BUSY() && (GET_SERVERLEVEL() >= 11))
    {
	    ::SendMessage(m_depotWnd, WM_GETSELLIST, (WPARAM) &m_LabelFilterView, 0);
	    if(m_LabelFilterView.GetCount() > 0)
			m_caption = LoadStringResource(!m_FilterOwnerFlag 
					  ? IDS_PERFORCE_FILTERED_LABELS : IDS_PERFORCE_FILTERED_LABELS_BY_BOTH);
	    OnViewReloadall();	
    }
}

void CLabelListCtrl::OnLabelFilterSetviewRev()
{
    if( !SERVER_BUSY() && (GET_SERVERLEVEL() >= 11))
    {
	    ::SendMessage(m_depotWnd, WM_GETSELLIST, (WPARAM) &m_LabelFilterView, 0);
	    if(m_LabelFilterView.GetCount() > 0)
		{
			// make a temp copy of the selection strings
			CStringList tempList;
			POSITION pos=m_LabelFilterView.GetHeadPosition();
			while(pos != NULL)
				tempList.AddTail(m_LabelFilterView.GetNext(pos));
			// now copy the selection back to m_LabelFilterView
			// and add #have,have to the end to filter on just the current rev
			m_LabelFilterView.RemoveAll();
			pos=tempList.GetHeadPosition();
			while(pos != NULL)
			{
				CString str = tempList.GetNext(pos) + _T("#have,have");
				m_LabelFilterView.AddTail(str);
			}
			m_caption = LoadStringResource(!m_FilterOwnerFlag 
					  ? IDS_PERFORCE_FILTERED_LABELS : IDS_PERFORCE_FILTERED_LABELS_BY_BOTH);
		}
	    OnViewReloadall();	
    }
}

LRESULT CLabelListCtrl::CallOnUpdateLabelFilterClearview(WPARAM wParam, LPARAM lParam)
{
	OnUpdateLabelFilterClearview((CCmdUI *)lParam);
	return 0;
}

void CLabelListCtrl::OnUpdateLabelFilterClearview(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY() 
		&& m_LabelFilterView.GetCount() > 0));
}

void CLabelListCtrl::OnUpdateLabelFilterSetview(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY() 
		&& (GET_SERVERLEVEL() >= 11)
		&& ::SendMessage(m_depotWnd, WM_GETSELCOUNT, 0, 0) > 0));
}

void CLabelListCtrl::OnUpdateLabelFilterSetviewRev(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY() 
		&& (GET_SERVERLEVEL() >= 11)
		&& ::SendMessage(m_depotWnd, WM_GETSELCOUNT, 0, 0) > 0));
}

void CLabelListCtrl::OnLabelFilterClearview() 
{
    if( !SERVER_BUSY() )
        OnLabelFilterClear();
}

/////////////////////////////////////////////////////////////////////
// OLE drag-drop support, to accept depot files or folders
// which will be used as per the user's choice on a context menu
/////////////////////////////////////////////////////////////////////

DROPEFFECT CLabelListCtrl::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	m_DropEffect=DROPEFFECT_NONE;
	m_DragDataFormat=0;
	CString fname;

	// Dont allow a drop if the server is busy, since a drop immediately attempts to
	// invoke a server command
	if(SERVER_BUSY() || m_EditInProgress)
		return DROPEFFECT_NONE;
		

	if(pDataObject->IsDataAvailable( (unsigned short) m_CF_DEPOT))
	{
		m_DropEffect=DROPEFFECT_COPY;
		m_DragDataFormat=m_CF_DEPOT;
	}
	
	return m_DropEffect;
}


DROPEFFECT CLabelListCtrl::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	// Dont allow a drop if the server is busy, since a drop immediately attempts to
	// invoke a server command
	if(SERVER_BUSY() || m_EditInProgress)
		m_DropEffect= DROPEFFECT_NONE;
		
	if(m_DragDataFormat == m_CF_DEPOT)
	{
		int i = GetSelectedItem( );
		if ( i > -1 )
			SetItemState(i, 0, LVIS_DROPHILITED|LVIS_SELECTED|LVIS_FOCUSED );
		ClientToScreen(&point);
		int index= GetContextItem( point );
		if (index > -1)
			SetItemState(index, LVIS_DROPHILITED|LVIS_SELECTED|LVIS_FOCUSED,
												  LVIS_DROPHILITED|LVIS_SELECTED|LVIS_FOCUSED );
	}
	return m_DropEffect;
}


BOOL CLabelListCtrl::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	CString fname;
	
	if(SERVER_BUSY() || m_EditInProgress)
	{
		// OnDragEnter() and OnDragOver() should avoid a drop at 
		// the wrong time!
		ASSERT(0);
		return FALSE;
	}
	
	MainFrame()->SetRightSplitter(1);

	if (m_DragDataFormat == m_CF_DEPOT)
	{
		int i = GetSelectedItem( );
		if ( i > -1 )
			SetItemState(i, 0, LVIS_DROPHILITED|LVIS_SELECTED|LVIS_FOCUSED );
		ClientToScreen(&point);
		int index= GetContextItem( point );
		if (index > -1)
			SetItemState(index, LVIS_SELECTED|LVIS_FOCUSED,
												  LVIS_SELECTED|LVIS_FOCUSED );
		if ((i > -1) || (GET_P4REGPTR()->GetLabelDragDropOption() == LDD_FILTER))
			::SendMessage(m_depotWnd, WM_DROPTARGET, LABELVIEW, MAKELPARAM(point.x,point.y));
		else
			::SendMessage(m_depotWnd, WM_DROPTARGET, LABELNOSEL, MAKELPARAM(point.x,point.y));
		return TRUE;
	}
	// Return false, so depot window doesnt start a file-open operation
	return FALSE;
}

void CLabelListCtrl::OnDragLeave( )
{
	if(m_DragDataFormat == m_CF_DEPOT)
	{
		int i = GetSelectedItem( );
		if ( i > -1 )
			SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_DROPHILITED|LVIS_SELECTED|LVIS_FOCUSED );
	}
}

LRESULT CLabelListCtrl::OnLabelDropMenu(WPARAM wParam, LPARAM lParam)
{
	// label view accepted a drop from the depot
	// create a context menu

	CPoint *dropTargetPt = (CPoint *)lParam;

	CP4Menu popMenu;
	popMenu.LoadMenu(IDR_LABELDRAGDROP);

	if (!wParam)
	{
		popMenu.RemoveMenu(ID_LABEL_SYNC, MF_BYCOMMAND);
		popMenu.RemoveMenu(ID_LABEL_DELETEFILES, MF_BYCOMMAND);
		popMenu.RemoveMenu(ID_LABEL_SYNC_CLIENT, MF_BYCOMMAND);
		popMenu.RemoveMenu(ID_LABEL_ADDTOLABELVIEW, MF_BYCOMMAND);
		popMenu.GetSubMenu(0)->RemoveMenu(0, MF_BYPOSITION);	// remove separator
	}

	// Pause the auto refresh timer
	SET_APP_HALTED(TRUE);

	// Finally blast the menu onto the screen
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
						dropTargetPt->x, dropTargetPt->y, AfxGetMainWnd());

	//Release the auto refresh timer
	SET_APP_HALTED(FALSE);

	return 0;
}

int CLabelListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CP4ListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CStringArray colnames;
	colnames.Add ( LoadStringResource(IDS_P4LABEL) );
	colnames.Add ( LoadStringResource(IDS_OWNER) );
	colnames.Add ( LoadStringResource(IDS_LOCKSTR) );
	colnames.Add ( LoadStringResource(IDS_DATE) );
	colnames.Add ( LoadStringResource(IDS_DESCRIPTION) );
	ASSERT( LABEL_MAXCOL == colnames.GetSize( ) );
	int width[LABEL_MAXCOL]={90,90,60,90,250};
	RestoreSavedWidths(width, (int)colnames.GetSize( ) , _T("Label List"));
	if (width[3] > 5000 && width[4] > 5000)
		width[2] = width[3] = width[4] = 90;
	InsertColumnHeaders( colnames, width );

	return 0;
}

void CLabelListCtrl::OnLabelDDsetSync()
{
	GET_P4REGPTR()->SetLabelDragDropOption(LDD_ADD);
}

void CLabelListCtrl::OnLabelDDsetDeleteFiles()
{
	GET_P4REGPTR()->SetLabelDragDropOption(LDD_DELETE);
}

void CLabelListCtrl::OnLabelDDsetSyncClient()
{
	GET_P4REGPTR()->SetLabelDragDropOption(LDD_SYNC);
}

void CLabelListCtrl::OnLabelDDAddToLabelView()
{
	GET_P4REGPTR()->SetLabelDragDropOption(LDD_ADD2VIEW);
}

void CLabelListCtrl::OnLabelDDsetFilterSetview()
{
	GET_P4REGPTR()->SetLabelDragDropOption(LDD_FILTER);
}

void CLabelListCtrl::OnLabelDDsetFilterSetviewRev()
{
	GET_P4REGPTR()->SetLabelDragDropOption(LDD_FILTERREV);
}

void CLabelListCtrl::OnLabelDDsetShowMenu()
{
	GET_P4REGPTR()->SetLabelDragDropOption(LDD_MENU);
}

void CLabelListCtrl::OnUpdateLabelDDsetShowMenu(CCmdUI* pCmdUI) 
{
	if (pCmdUI->m_pMenu)
	{
		UINT lld;
		switch (GET_P4REGPTR()->GetLabelDragDropOption())
		{
		case LDD_ADD:
			lld = ID_LABELDD_SYNC;
			break;
		case LDD_DELETE:
			lld = ID_LABELDD_DELETEFILES;
			break;
		case LDD_SYNC:
			lld = ID_LABELDD_SYNC_CLIENT;
			break;
		case LDD_ADD2VIEW:
			lld = ID_LABELDD_ADDTOLABELVIEW;
			break;
		case LDD_FILTER:
			lld = ID_LABELFILTERDD_SETVIEW;
			break;
		case LDD_FILTERREV:
			lld = ID_LABELFILTERDD_SETVIEWREV;
			break;
		default:
			lld = ID_LABELDD_SHOWMENU;
			break;
		}
		pCmdUI->m_pMenu->CheckMenuRadioItem( ID_LABELDD_SYNC, ID_LABELDD_SHOWMENU, lld, MF_BYCOMMAND  ) ;
	}
	pCmdUI->Enable( TRUE );
}

void CLabelListCtrl::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CP4ListCtrl::OnActivate(nState, pWndOther, bMinimized);

	if(nState != WA_INACTIVE && m_Active.IsEmpty() && GetItemCount() > 1)
	{
		int i = FindInList(GET_P4REGPTR()->GetLastLabel());
		if (i < 0)	
			i=0;
		SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		EnsureVisible(i, FALSE);
		m_Active=GetSelectedItemText();
	}
	else
	{
		m_Active=GetSelectedItemText();
		if (!m_Active.IsEmpty())
			GET_P4REGPTR()->SetLastLabel(m_Active);
	}
}

void CLabelListCtrl::OnPerforceOptions()
{
	MainFrame()->OnPerforceOptions(TRUE, FALSE, IDS_PAGE_LABEL);
}

void CLabelListCtrl::OnUpdateFilterByOwner(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( MainFrame()->SetMenuIcon(pCmdUI, !SERVER_BUSY()) );
}

void CLabelListCtrl::OnFilterByOwner()
{
	CFilterByOwnerDlg dlg;
	dlg.m_NotUser = GET_P4REGPTR()->GetLabelFilterByOwnerFlag() & 0x01;
	dlg.m_bShowIncBlanks = m_AnyBlankOwner;
	if (dlg.DoModal() == IDOK)
	{
		GET_P4REGPTR()->SetLabelFilterByOwner(m_FilterOwner = dlg.m_Owner);
		GET_P4REGPTR()->SetLabelFilterByOwnerFlag(m_FilterOwnerFlag = (dlg.m_NotUser ? 0x1 : 0x10));
		GET_P4REGPTR()->SetLabelFilterIncBlank(m_FilterIncBlank = dlg.m_IncBlank);
	    m_caption = LoadStringResource(m_LabelFilterView.IsEmpty() 
				  ? IDS_PERFORCE_FILTERED_LABELS_BY_OWNER : IDS_PERFORCE_FILTERED_LABELS_BY_BOTH);
		OnViewReloadall();
	}
}

void CLabelListCtrl::OnUpdateClearFilterByOwner(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( MainFrame()->SetMenuIcon(pCmdUI, m_FilterOwnerFlag && !SERVER_BUSY()) );
}

void CLabelListCtrl::OnClearFilterByOwner()
{
	GET_P4REGPTR()->SetLabelFilterByOwnerFlag(m_FilterOwnerFlag = 0);
    m_caption = LoadStringResource(m_LabelFilterView.IsEmpty() 
			  ? IDS_PERFORCE_LABELS : IDS_PERFORCE_FILTERED_LABELS);
	OnViewReloadall();
}

void CLabelListCtrl::OnUpdateClearFilterLabels(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!SERVER_BUSY() && (m_FilterOwnerFlag || !m_LabelFilterView.IsEmpty()));
}

void CLabelListCtrl::OnUpdateDiff2Labels(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( !SERVER_BUSY() );
}

void CLabelListCtrl::OnDiff2Labels()
{
	CDiff2ObjsDlg dlg;
	dlg.m_Type = COMBO_LABEL;
	m_Active=GetSelectedItemText();
	if (!m_Active.IsEmpty())
		dlg.m_Edit1 = m_Active;
	SET_APP_HALTED(TRUE);
	INT_PTR rc=dlg.DoModal();  
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

LRESULT CLabelListCtrl::NewUser(WPARAM wParam, LPARAM lParam)
{
	m_User = GET_P4REGPTR()->GetP4User();
	if (m_FilterOwnerFlag & 0x10)
		Clear();
	return 0;
}
