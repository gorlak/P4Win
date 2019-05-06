// P4ListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "P4ListCtrl.h"
#include "P4PaneView.h"
#include "MainFrm.h"
#include "P4Object.h"
#include "P4ListBrowse.h"
#include "cmd_delete.h"
#include "cmd_editspec.h"
#include "SpecDescDlg.h"
#include "RegKeyEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static LPCTSTR sRegKey = _T("Software\\Perforce\\P4Win\\Layout\\");
static LPCTSTR sRegValue_ColumnWidths = _T("Column Widths");
static LPCTSTR sRegValue_SortColumns = _T("Sort Columns");
static LPCTSTR sRegValue_ColumnNames = _T("Column Names");



/////////////////////////////////////////////////////////////////////////////
// CP4ListCtrl

IMPLEMENT_DYNCREATE(CP4ListCtrl, CListCtrl)

CP4ListCtrl::CP4ListCtrl()
{
	m_UpdateState= LIST_CLEAR;
	m_SortAscending=FALSE;
	m_ActivatedBefore=FALSE;
	m_EditInProgress=FALSE;
	m_LastSortCol=0;
    m_ContextContext= KEYSTROKED;
	m_ReadSavedWidths = m_ColsInited = FALSE;
	m_PostViewUpdateMsg = 0;
	m_LastSelIx = -1;
	for (int i = -1; ++i < MAX_SORT_COLUMNS; )
		m_SortColumns[i] = 0;
}

CP4ListCtrl::~CP4ListCtrl()
{
}

void CP4ListCtrl::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (m_EditInProgress && (bShow || GET_P4REGPTR()->AutoMinEditDlg()))
		m_EditInProgressWnd->ShowWindow(bShow ? SW_RESTORE : SW_SHOWMINNOACTIVE);
}

LRESULT CP4ListCtrl::OnActivateModeless(WPARAM wParam, LPARAM lParam)
{
	if (m_EditInProgress && wParam == WA_ACTIVE)
		::SetFocus(m_EditInProgressWnd->m_hWnd);
	return 0;
}

void CP4ListCtrl::SetUpdateDone()
{ 
    m_UpdateState = LIST_UPDATED; 
    MainFrame()->SetLastUpdateTime(UPDATE_SUCCESS);
}
void CP4ListCtrl::SetUpdateFailed() 
{ 
    m_UpdateState = LIST_CLEAR; 
    MainFrame()->SetLastUpdateTime(UPDATE_FAILED);
}

// Last line of OnViewUpdate() should ALWAYS call this base fn
void CP4ListCtrl::OnViewUpdate() 
{ 
	m_UpdateState= LIST_UPDATING; 
}

BEGIN_MESSAGE_MAP(CP4ListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CP4ListCtrl)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_WM_SHOWWINDOW()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnKeydown)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_FINDPATTERN, OnFindPattern )
	ON_MESSAGE( WM_FETCHOBJECTLIST, OnP4ObjectFetch )
	ON_MESSAGE( WM_GETOBJECTLIST, OnP4ObjectList )
	ON_MESSAGE( WM_WIZFETCHOBJECTLIST, OnP4WizObjectFetch )
	ON_MESSAGE( WM_WIZGETOBJECTLIST, OnP4WizObjectList )
	ON_MESSAGE( WM_INTEGFETCHOBJECTLIST, OnP4IntegObjectFetch )
	ON_MESSAGE( WM_INTEGGETOBJECTLIST, OnP4IntegObjectList )
	ON_MESSAGE( WM_SELECTTHIS, OnSelectThis )
	ON_MESSAGE( WM_ACTIVATEMODELESS, OnActivateModeless )
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CP4ListCtrl diagnostics

#ifdef _DEBUG
void CP4ListCtrl::AssertValid() const
{
	CListCtrl::AssertValid();
}

void CP4ListCtrl::Dump(CDumpContext& dc) const
{
	CListCtrl::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CP4ListCtrl message handlers

void CP4ListCtrl::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	if(pNMListView->iItem == -1)
	{
		if(pNMListView->iSubItem == m_LastSortCol)
			m_SortAscending= !m_SortAscending;
		else
			m_LastSortCol=pNMListView->iSubItem;

		ReSort();
	}
	
	*pResult = 0;
}

struct SortParam
{
	CP4ListCtrl * listCtrl;
	int subItem;
};

static int CALLBACK SortCallback(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	SortParam *sp = (SortParam*)lParamSort;
	int rc = sp->listCtrl->OnCompareItems(lParam1, lParam2, sp->subItem);

	// check for duplicate keys; if so, sort on next sort columns
	int nextcol;
	if (!rc && sp->subItem && 
		((nextcol = sp->listCtrl->NextSortColumn(sp->subItem)) != 0))
	{
		// nextcol now contains a value like 1 for col-0-ascending or like -1 for col-0-decending
		BOOL saveSortAscending = sp->listCtrl->m_SortAscending;
		if (nextcol < 0)
		{
			nextcol = 0 - nextcol;
			sp->listCtrl->m_SortAscending = FALSE;
		}
		else
			sp->listCtrl->m_SortAscending = TRUE;
		rc = sp->listCtrl->OnCompareItems(lParam1, lParam2, (LPARAM)(nextcol - 1));
		sp->listCtrl->m_SortAscending = saveSortAscending;
	}
	return rc;
}

void CP4ListCtrl::ReSort()
{
	// update sort column and/or direction
	AddSortColumn(m_LastSortCol, m_SortAscending);

	// actually sort the list items
	SortParam sp;
	sp.listCtrl = this;
	sp.subItem = m_LastSortCol;
        SetRedraw(FALSE);
	SortItems(SortCallback, (DWORD_PTR)&sp);
        SetRedraw(TRUE);
	
	// update the header and make sure the selected item is visible
	m_headerctrl.SetSortImage( m_LastSortCol, m_SortAscending );
	if( GetSelectedItem() != -1 )
		EnsureVisible(GetSelectedItem(), FALSE);
}

BOOL CP4ListCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(SERVER_BUSY())
		return SET_BUSYCURSOR();
	else
		return CListCtrl::OnSetCursor(pWnd, nHitTest, message);
}


void CP4ListCtrl::SetIndexAndPoint( int &index, CPoint &point )
{
    if( m_ContextContext == MOUSEHIT )
    {
	    index= GetContextItem( point );
        m_ContextContext= KEYSTROKED;
    }
    else
    {
        CRect rect;
        GetClientRect(&rect);   
        point= rect.CenterPoint();
        ClientToScreen(&point);
        index= GetSelectedItem();
    }
}

int CP4ListCtrl::GetContextItem( const CPoint & point )
{
	int index = GetHitItem ( point, TRUE );
	if ( index < 0 )
	{
		//		user right clicked on the empty part of the pane.
		//		if something is selected, unselect it so user doesn't get confused.
		//
		int i = GetSelectedItem( );
		if ( i > -1 )
			SetItemState( i, 0 , LVIS_SELECTED|LVIS_FOCUSED );
	}
	else
	{
		//		we got a hit on a name. select it, since another name may have
		//		been selected -- if the user has right-clicked on a new name,
		//		let's assume s/he wants this new one to be selected as well.
		//
		SetItemState( index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
	}

	return index;
}


//////////////////////////////////////////////////////////////////////////////
// Windows NT3.51 screws up context menues in listview, just as NT4 screws these
// up in tree views.  Manually implement context menu invokation.

void CP4ListCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// Just eat the message
}

void CP4ListCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// Right click context menu is broken in CListViews - a dbl right click is what
	// it wants.  
	CPoint screenPt=point;
	ClientToScreen(&screenPt);

    m_ContextContext= MOUSEHIT;
	OnContextMenu(NULL, screenPt);
}

// Save current column widths to the registry
void CP4ListCtrl::OnDestroy() 
{
	SaveColumnWidths();
	CListCtrl::OnDestroy();
}


// Save current column widths to the registry
void CP4ListCtrl::SaveColumnWidths() 
{
	// Save the column widths
	if(m_ColsInited && !m_SubKey.IsEmpty())
	{
		
		CString theKey = sRegKey + m_SubKey;
        CRegKeyEx key;
        if(ERROR_SUCCESS == key.Create(HKEY_CURRENT_USER, theKey))
        {
		    CString str;
            int i;
		    for(i=0; i < MAX_P4OBJECTS_COLUMNS; i++)
		    {
			    // Note that GetColumnWidth returns zero if i > numcols
    		    CString num;
			    num.Format(_T("%d"), GetColumnWidth(i));
			    if(i)
				    str+=_T(",");
			    str+=num;
		    }
            key.SetValueString(str, sRegValue_ColumnWidths);

            str.Empty();
			for(i=0; i < MAX_SORT_COLUMNS; i++)
			{
                CString num;
				num.Format(_T("%d"), m_SortColumns[i]);
				if(i)
					str+=_T(",");
				str+=num;
			}
            key.SetValueString(str, sRegValue_SortColumns);

        }
	}
}

// Check the registry to see if we have recorded the column widths last
// used for this list view
void CP4ListCtrl::RestoreSavedWidths(int *width, int numcols, LPCTSTR subkey)
{
	m_SubKey = subkey;
	if(!m_SubKey.IsEmpty())
    {
	    CString theKey = sRegKey + m_SubKey;
        CRegKeyEx key;
        if(ERROR_SUCCESS == key.Open(HKEY_CURRENT_USER, theKey, KEY_READ))
        {
            CString colWidths = key.QueryValueString(sRegValue_ColumnWidths);
            CString sortCols = key.QueryValueString(sRegValue_SortColumns);
            if(!colWidths.IsEmpty())
            {
			    //		things can go wrong with the registry setting of the 
			    //		widths. 
			    //		use the defaults if the entry is all zeroes.
			    //
			    if ( colWidths != _T("0,0,0,0,0,0,0,0,0,0") )
				    for(int i=0; i< numcols; i++)
                    {
		                // Always use at least 30, so no column gets hidden.  Very important
		                // when changing p4port and hitting a server whose job spec has more
		                // columns than the last server had.  Allowing the width of new columns
		                // to default to zero can confound someone unfamiliar with spreadsheet
		                // operation, since it is not obvious that you can drag them back to a
		                // visible width
						//
						// Also check that the column width has not been set too large
						// XP seems to lose it if the column width is very large - job006177.

					    width[i] = GetPositiveNumber(colWidths);
					    if (width[i] < 30)
                            width[i] = 30;
						else if (width[i] > 3000)
							width[i] = 150;
                    }
            }
            if(!sortCols.IsEmpty())
            {
			    for(int i=0; i< MAX_SORT_COLUMNS; i++)
                {
				    m_SortColumns[i]= GetANumber(sortCols);
                    if(!i && !m_SortColumns[i])
                        m_SortColumns[i] = 1;
                }
			    m_LastSortCol=abs(m_SortColumns[0]) - 1; // SortColumns are signed & 1-relative
			    m_SortAscending = m_SortColumns[0] >= 0 ? TRUE : FALSE;
            }
        }

		m_ReadSavedWidths= TRUE;
	}

	// Make sure no column completely disappeared (because you can't get it back then)
	for (int i=-1; ++i < numcols; )
	{
		if (width[i] < 5)
			width[i] = 5;
	}
}


// Check the registry to see if we have recorded column names last
// used for this list view
BOOL CP4ListCtrl::GetSavedColumnNames(CStringArray &colNames, LPCTSTR subkey)
{
	BOOL rc = TRUE;
	CString allCols = _T(":101 :102 :103 :104 :105 ");	// note ' ' must follow last fieldname
	m_SubKey = subkey;
	CString theKey = sRegKey + m_SubKey;

    CRegKeyEx key;
    if(ERROR_SUCCESS == key.Open(HKEY_CURRENT_USER, theKey, KEY_READ))
    {
        CString colNames = key.QueryValueString(sRegValue_ColumnNames);
        if(!colNames.IsEmpty())
        {
			allCols = colNames;
			allCols += _T(' ');
			rc = FALSE;
        }
    }

	int i;
	int j = 0;
	while ((j++ < MAX_P4OBJECTS_COLUMNS) && ((i = allCols.Find(_T(' '))) != -1))
	{
		CString name = allCols.Left(i);
		colNames.Add(name);
		allCols = allCols.Right(allCols.GetLength() - i);
		allCols.TrimLeft();
	}
	return rc;
}

void CP4ListCtrl::SaveColumnNames(CString &colNames, LPCTSTR subkey)
{
	// Save the column names
	if(!m_SubKey.IsEmpty())
	{
		CString theKey = sRegKey + m_SubKey;
        CRegKeyEx key;
        if(ERROR_SUCCESS == key.Create(HKEY_CURRENT_USER, theKey))
        {
            colNames.TrimRight();
            key.SetValueString(colNames, sRegValue_ColumnNames);
        }
	}
}

void CP4ListCtrl::DeleteColumnNames(LPCTSTR subkey)
{
	// Save the column names
	if(!m_SubKey.IsEmpty())
	{
		CString theKey = sRegKey + m_SubKey;
        CRegKeyEx key;
        if(ERROR_SUCCESS == key.Create(HKEY_CURRENT_USER, theKey))
        {
            key.DeleteValue(sRegValue_ColumnNames);
        }
	}
}

/*
	_________________________________________________________________
	
	hitting enter should do what the left click button does (mainly
	call ondescribe, although some lists call onedit for some reason)

	do delete too, when i get the owners of the clients
	etc. and deletes are allowed. also, when i know who is a super
	user, allow inserts.
	_________________________________________________________________
*/

void CP4ListCtrl::OnKeydown( NMHDR *pNMHDR, LRESULT *pResult) 
{
	LV_KEYDOWN *pLVKeyDow = ( LV_KEYDOWN *)pNMHDR;

	MainFrame()->SetGotUserInput( );

	switch ( pLVKeyDow->wVKey )
	{
	case VK_RETURN:	
        OnDescribe( );	
        break;
	case VK_DELETE:	
        OnDelete( GetDeleteType( ) );	
        break;
	case VK_TAB:
		if ( ::GetKeyState(VK_CONTROL) & 0x8000 )	// Ctrl+TAB switches to opposite pane
			MainFrame()->SwitchPanes(DYNAMIC_DOWNCAST(CView, GetParent()), 
					( ::GetKeyState(VK_SHIFT) & 0x8000 ));	// Shift+Ctrl+TAB switches to status pane
		break;
	}
	*pResult = 0;
}


/*
	_________________________________________________________________
*/

int CP4ListCtrl::GetDeleteType( )
{
	return ( m_viewType == P4BRANCH_SPEC ) ? P4BRANCH_DEL
		: ( m_viewType == P4CLIENT_SPEC ) ? P4CLIENT_DEL
		: ( m_viewType == P4JOB_SPEC ) ? P4JOB_DEL
		: ( m_viewType == P4LABEL_SPEC ) ? P4LABEL_DEL
		: ( m_viewType == P4USER_SPEC ) ? P4USER_DEL
		: 0;
}


/*	_________________________________________________________________

	put all this in one place to reduce code bloat.
	_________________________________________________________________
*/

void CP4ListCtrl::InsertColumnHeaders( const CStringArray &colnames, int * widths )
{
	// Delete all of the old columns.
	int nColumnCount = GetHeaderCtrl()->GetItemCount();
	for (int i=-1; ++i < nColumnCount; )
		DeleteColumn(0);

	SetImageList( TheApp()->GetImageList(), LVSIL_SMALL );
	
	for( int subItem = 0; subItem < colnames.GetSize(); subItem++ )
	{
		LVCOLUMN lvCol;
		lvCol.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvCol.fmt = LVCFMT_LEFT;
		lvCol.pszText = ( LPTSTR )( LPCTSTR ) colnames.GetAt( subItem );
		lvCol.iSubItem = lvCol.iOrder = subItem;
		lvCol.cx = widths[ subItem ];
		
		static	BOOL	bErrDispalyed=FALSE;
		int i = InsertColumn( subItem, &lvCol );
		if (i==-1 && !bErrDispalyed)
		{
			AfxMessageBox(_T("Column header insert failure"), MB_ICONSTOP);
			bErrDispalyed = TRUE;
		}
	}
    m_ColsInited = TRUE;
}


/*
	_________________________________________________________________

	get the string of the item. return it. if nothing is selected
	return an empty string.
	_________________________________________________________________
*/

CString CP4ListCtrl::GetSelectedItemText( )
{
	TCHAR str[ LISTVIEWNAMEBUFSIZE + 1 ];

	int index = GetSelectedItem ( );

	CString name = "";
	if( index != -1 )
	{
		GetItemText( index, 0, str, LISTVIEWNAMEBUFSIZE );
		name = str;
	}

	return name;
}

CString CP4ListCtrl::GetSelectedItemOwner(int ownerColumnNumber)
{
	TCHAR str[ LISTVIEWNAMEBUFSIZE + 1 ];

	int index = GetSelectedItem ( );

	CString owner = "";
	if( index != -1 )
	{
		GetItemText( index, ownerColumnNumber, str, LISTVIEWNAMEBUFSIZE );
		owner = str;
	}

	return owner;
}

BOOL CP4ListCtrl::SelectedItemIsLocked(int optionsColumnNumber )
{
	TCHAR str[ LISTVIEWNAMEBUFSIZE + 1 ];

	int index = GetSelectedItem ( );

	if( index != -1 )
	{
		GetItemText( index, optionsColumnNumber, str, LISTVIEWNAMEBUFSIZE );
		if (str[0] == _T('l'))
			return TRUE;
	}

	return FALSE;
}

int CP4ListCtrl::GetSelectedItem()
{
	if ( m_LastSelIx != -1 && GetItemState( m_LastSelIx, LVIS_SELECTED ) == LVIS_SELECTED )
		return m_LastSelIx;

	int cnt = GetItemCount();
	int start = max(GetTopIndex() - 100, -1);
	for( m_LastSelIx = start; ++m_LastSelIx < cnt;  )
	{
		if( GetItemState( m_LastSelIx, LVIS_SELECTED ) == LVIS_SELECTED )
			return m_LastSelIx;
	}

	for( m_LastSelIx = start+1; --m_LastSelIx >= 0;  )
	{
		if( GetItemState( m_LastSelIx, LVIS_SELECTED ) == LVIS_SELECTED )
			return m_LastSelIx;
	}

	return m_LastSelIx = -1;
}


int CP4ListCtrl::GetHitItem( const CPoint &point, BOOL bScreenCoords /* = FALSE */)
{
	CPoint local = point;

	if ( bScreenCoords )
		ScreenToClient( &local );

 	LV_HITTESTINFO ht;
	ht.pt = local;
	ht.flags = LVHT_ONITEMICON | LVHT_ONITEMLABEL;
	return HitTest( &ht );
}


int CP4ListCtrl::FindInList( const CString &name )
{
	TCHAR str[ LISTVIEWNAMEBUFSIZE + 1 ];

	for( int i = 0; i < GetItemCount(); i++ )
	{
		GetItemText( i, 0, str, LISTVIEWNAMEBUFSIZE );
		if( !Compare(name,str) )
			return i;
	}  

	return -1;
}

int CP4ListCtrl::FindInListAll( const CString &name )
{
	INT_PTR cnt = m_ListAll.column[0].GetCount();

	for( int i = 0; i < cnt; i++ )
	{
		if( !Compare(name, m_ListAll.column[0].GetAt(i)) )
			return i;
	}  

	return -1;
}


int CP4ListCtrl::FindInListNoCase( const CString &name )
{
	TCHAR str[ LISTVIEWNAMEBUFSIZE + 1 ];

	for( int i = 0; i < GetItemCount(); i++ )
	{
		GetItemText( i, 0, str, LISTVIEWNAMEBUFSIZE );
		if( !name.CompareNoCase(str) )
			return i;
	}  

	return -1;
}

void CP4ListCtrl::OnDescribe( void )
{
	m_Active = GetSelectedItemText();

	if ( m_Active.IsEmpty( ) )
		return;

	CCmd_Describe *pCmd = new CCmd_Describe;
	pCmd->Init( m_hWnd, RUN_ASYNC );
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

LRESULT CP4ListCtrl::OnP4Describe( WPARAM wParam, LPARAM lParam )
{
	MSG  msg;
	BOOL ret = FALSE;

	CCmd_Describe *pCmd = ( CCmd_Describe * )wParam;

	if(!pCmd->GetError() && !MainFrame()->IsQuitting())
	{
		CString ref = pCmd->GetReference();
		ASSERT(!ref.IsEmpty());
		if (!ref.IsEmpty())
			m_Describing = ref;

		int i, j = -2;
        CString specblankdesc = CCmd_EditSpec::g_blankDesc;
		CString desc = MakeCRs( pCmd->GetDescription( ) );
		if ((i = desc.Find(specblankdesc)) > 0)
		{
			j = lstrlen(_T("\r\nDescription:\r\n\t"));
			j = desc.Find(_T("\r\nDescription:\r\n\t"), i - j*2) + j;
		}
		if ((i == j) 
		 && (desc.GetAt(i-1) == _T('\t'))
		 && ((i+specblankdesc.GetLength() >= desc.GetLength()) 
		  || (desc.GetAt(i+specblankdesc.GetLength()) < _T(' '))))
		{
			CString msg;
			msg.FormatMessage(IDS_THERE_IS_NO_s_TO_DESCRIBE, m_Describing);
			AddToStatus(msg, SV_WARNING);
		}
		else
		{
			int key;
			CSpecDescDlg *dlg = new CSpecDescDlg(this);
			dlg->SetIsModeless(TRUE);
			dlg->SetKey(key = pCmd->HaveServerLock()? pCmd->GetServerKey() : 0);
			if (m_viewType == P4USER_SPEC)
			{
				if ((desc.Find(_T("\r\n\r\nUpdate:\t")) == -1)
				 && (desc.Find(_T("\r\n\r\nAccess:\t")) == -1))
				{
					desc += _T("\n\n\tTHIS USER DOES NOT APPEAR TO EXIST - has it been deleted?");
				}
			}
			dlg->SetDescription( desc );
			dlg->SetItemName( m_Describing );
			if (m_viewType == P4JOB_SPEC)
				dlg->SetReportedByTitle( m_ReportedByTitle );
			CString caption;
			caption.FormatMessage(IDS_PERFORCE_DESCRIPTION_FOR_s, m_Describing);
			dlg->SetCaption( caption );
			dlg->SetShowNextPrev(m_UpdateState == LIST_UPDATED ? TRUE : FALSE);
			dlg->SetShowShowFixes(m_viewType == P4JOB_SPEC);
			dlg->SetShowShowFiles(m_viewType == P4LABEL_SPEC);
			dlg->SetViewType(m_viewType);
			dlg->SetCaller(pCmd->GetCaller());
			if (pCmd->GetListCtrl() && pCmd->GetListCtrl() != this)
			{
				dlg->SetShowEditBtn(FALSE);
				dlg->SetListCtrl(pCmd->GetListCtrl());
			}
			else
			{
				dlg->SetListCtrl(this);
				switch(m_viewType)
				{
				case P4CLIENT_SPEC:
					dlg->SetShowEditBtn(!key && GET_P4REGPTR()->GetP4Client() == m_Describing);
					break;
				case P4USER_SPEC:
					dlg->SetShowEditBtn(!key && GET_P4REGPTR()->GetP4User() == m_Describing);
					break;
				default:
					dlg->SetShowEditBtn(!key ? TRUE : FALSE);
					break;
				}
			}
			if (m_EditInProgress)						// overrides any previous possibility
				dlg->SetShowEditBtn(FALSE);

			while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )	// clear the message queue
			{
				if ( msg.message == WM_QUIT )	//	get out if app is terminating
					break;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if (!dlg->Create(IDD_SPECDESC, this))	// display the description dialog box
			{
				EnumChildWindows(AfxGetMainWnd()->m_hWnd, ChildSetRedraw, TRUE);
				dlg->DestroyWindow();	// some error! clean up
				delete dlg;
			}
			else
				ret = TRUE;
		}
	}
	else	// had an error - need to turn painting back on
	{
		EnumChildWindows(AfxGetMainWnd()->m_hWnd, ChildSetRedraw, TRUE);
	}
	
	delete pCmd;
	MainFrame()->ClearStatus();
	return ret;
}

LRESULT CP4ListCtrl::OnP4EndDescribe( WPARAM wParam, LPARAM lParam )
{
	CSpecDescDlg *dlg = (CSpecDescDlg *)lParam;
	CString ref = dlg->GetItemName();
	ASSERT(!ref.IsEmpty());
	if (!ref.IsEmpty())
		m_Describing = ref;

	switch(wParam)				// which button did they click to close the box?
	{
	case IDC_NEXTITEM:
	case IDC_PREVITEM:
	{
		CListCtrl *plc = (CListCtrl *)dlg->GetListCtrl();
		if (SetToNextPrevItem(m_Describing, wParam == IDC_NEXTITEM ? 1 : -1, plc))
		{
			if (plc == this)
				OnDescribe();	// display the next/prev in the list on the screen
			else
				dlg->GetCaller()->SendMessage(WM_COMMAND, IDC_DESCRIBE, 0);
			break;
		}
	}
	case IDC_EDITIT:
		if (wParam == IDC_EDITIT)	// note fall-thru from above!
			EditTheSpec(&m_Describing);
	default:	// clicked OK, pressed ESC or ENTER - need to turn painting back on
		EnumChildWindows(AfxGetMainWnd()->m_hWnd, ChildSetRedraw, TRUE);
		break;
	}
	dlg->DestroyWindow();
	return TRUE;
}

// The plc points to the list control that initially fired off
// the Describe - it may not be the same as 'this'
BOOL CP4ListCtrl::SetToNextPrevItem(CString& name, int np, CListCtrl *plc)
{
	// Rummage through the list for the desired item
	int index = -1;
	int count = plc->GetItemCount();
	TCHAR str[ LISTVIEWNAMEBUFSIZE + 1 ];

	BOOL b = FALSE;
	if (plc == this && m_LastSelIx != -1)
	{
		plc->GetItemText( m_LastSelIx, 0, str, LISTVIEWNAMEBUFSIZE );
		if( !Compare(name,str) )
		{
			index = m_LastSelIx;
			b = TRUE;
		}
	}

	if (!b)
	{
		for( int i = -1; ++i < count; )
		{
			plc->GetItemText( i, 0, str, LISTVIEWNAMEBUFSIZE );
			if( !Compare(name,str) )
			{
				index = i;
				break;
			}
		}
	}

	if((index + np < 0) || (index == -1) || (index + np >= count))
		return FALSE;

	plc->SetItemState( index + np, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
	if (b)
		m_LastSelIx = index + np;
	return TRUE;
}

void CP4ListCtrl::OnDelete( int type )
{
	m_Active = GetSelectedItemText( ) ;

	if ( m_Active.IsEmpty( ) )
		return;

	if ( !OKToDelete( ) )
		return;
	
	CString msg;
	msg.FormatMessage ( IDS_DELETEIT_s, m_Active );
	if( AfxMessageBox( msg, MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION ) != IDYES)
		return;	

	CCmd_Delete *pCmd = new CCmd_Delete;
	pCmd->Init( m_hWnd, RUN_ASYNC );
	if( pCmd->Run( type, m_Active ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_DELETING) );
	else
		delete pCmd;
}

LRESULT CP4ListCtrl::OnP4Delete( WPARAM wParam, LPARAM lParam )
{
	CCmd_Delete *pCmd = ( CCmd_Delete * )wParam;

	if(!pCmd->GetError())
	{
		AddToStatus( pCmd->GetCompletionMessage()
					, SV_COMPLETION );
		int index = FindInList( m_Active );
		if ( index > -1 )
			DeleteItem( index );
		index = FindInListAll( m_Active );
		if ( index > -1 )
			m_ListAll.column[0].SetAt(index, _T("@@"));	// Removes from list of all items
	}

	// the following code is only used if a new user is created, 
	// but then the edit of the new user is canceled.
	// in that case the newly created user is deleted
	// so we then want to switch back to the previous user.
	if ( m_viewType == P4USER_SPEC )
	{
		CString sw2user = pCmd->GetSwitch2User();
		if (!sw2user.IsEmpty())
		{
			// switch to the previous user
			if( !GET_P4REGPTR()->SetP4User( sw2user, TRUE, FALSE, FALSE ) )
			{
				AfxMessageBox( IDS_UNABLE_TO_WRITE_P4USER_TO_THE_REGISTRY, MB_ICONEXCLAMATION);
				m_Active  = GET_P4REGPTR()->GetP4User();
			}
			MainFrame()->UpdateCaption( ) ;	
		}
	}
	delete pCmd;
	MainFrame()->ClearStatus();
    return 0;
}


/*
	_________________________________________________________________
*/

BOOL CP4ListCtrl::OnUpdateShowMenuItem( CCmdUI* pCmdUI, UINT idString )
{
    CString str;
	CString txt = GetSelectedItemText();
    str.FormatMessage(idString, TruncateString(txt, 50));
	pCmdUI->SetText ( str );
	return( !SERVER_BUSY( ) && !txt.IsEmpty( ) );	
}

int CP4ListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	LRESULT dwStyle = ::SendMessage(m_hWnd,LVM_GETEXTENDEDLISTVIEWSTYLE,0,0);
	dwStyle |= LVS_EX_FULLROWSELECT;
	::SendMessage(m_hWnd,LVM_SETEXTENDEDLISTVIEWSTYLE,0,dwStyle);
	
	// In any event, subclass the stinkin header so we can owner-draw
	// sort arrows to indicate how its sorted
	m_headerctrl.SubclassWindow(GetHeaderCtrl()->m_hWnd);

	return 0;
}

void CP4ListCtrl::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetItemCount() ? TRUE : FALSE);
}

void CP4ListCtrl::OnEditCopy() 
{
	CString txt = GetSelectedItemText();
	CopyTextToClipboard(txt);
}

void CP4ListCtrl::OnEditPaste( const CString &name ) 
{
	int index;
	if (m_viewType == P4JOB_SPEC 
	 && _istdigit(name.GetAt(0))
	 && GET_P4REGPTR()->GetConvertJobNbr())
	{
		BOOL b = TRUE;
		CString jobStr = name;
		for (int i=0; ++i < jobStr.GetLength(); )
		{
			if (!_istdigit(jobStr.GetAt(i)))
			{
				b = FALSE;
				break;
			}
		}
		if (b)
			jobStr.FormatMessage(IDS_JOBNAME_FORMAT_n, _tstoi(jobStr));
		index = FindInList( jobStr );
	}
	else
		index = FindInList( name );

	if ( index > -1 )
	{
		SetItemState( index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
		EnsureVisible(index, FALSE);
	}
	else
		MessageBeep(0);
}

// Save sort columns in array of signed, 1-relative numbers;
// negative columns numbers are for descending, positive for ascending.
// Note that incoming 'colnbr' is a non-negative 0-relative number and must be incremented
void CP4ListCtrl::AddSortColumn(int colnbr, BOOL sortAscending)
{
	int i;
	BOOL lastAsc = m_SortColumns[0] >= 0 ? TRUE : FALSE;

	// first, check for just changing ascending/descending (after making 1 relative)
	if (++colnbr == abs(m_SortColumns[0]))
	{
		if (sortAscending != lastAsc)
			m_SortColumns[0] = 0 - m_SortColumns[0];
		return;
	}

	// if first column, clear the saved columns since the 1st column is unique
	if (colnbr == 1)
	{
		for (i = -1; ++i < MAX_SORT_COLUMNS; )
			m_SortColumns[i] = 0;
	}

	// descending columns are stored as a negative number
	if (!sortAscending)
		colnbr = 0 - colnbr;

	// move older columns up one, then save new signed, 1-relative column number
	for (i = MAX_SORT_COLUMNS; --i; )
		m_SortColumns[i] = m_SortColumns[i-1];
	m_SortColumns[0] = colnbr;

	// remove any older references to this column since they are no longer valid
	for (i = 0; ++i < MAX_SORT_COLUMNS; )
	{
		if (!m_SortColumns[i])
			break;
		if (abs(m_SortColumns[i]) == abs(colnbr))
		{
			for (int j = i; ++j < MAX_SORT_COLUMNS; )
				m_SortColumns[j-1] = m_SortColumns[j];
			m_SortColumns[MAX_SORT_COLUMNS-1] = 0;
		}
	}
}

// Since a sort column can only appear once in our saved column array,
// given the current column, we can determine the next column.
// Note that incoming 'colnbr' is a non-negative 0-relative number and must be incremented
// The return value is a signed 1-relative number; negative for descending, positive for ascending
int CP4ListCtrl::NextSortColumn(int lastcol)
{
	if (lastcol == 0)
		return 0;		// 0 => no next column
	lastcol++;
	for (int i = -1; ++i < (MAX_SORT_COLUMNS-1); )
	{
		if (abs(m_SortColumns[i]) == lastcol)
			return m_SortColumns[i+1];
	}
	return 0;			// 0 => no next column
}

LRESULT CP4ListCtrl::OnFindPattern(WPARAM wParam, LPARAM lParam)
{
	TCHAR str[ 1024 ];
	CString text;
	CString what = (TCHAR *)lParam;
	int flags = (int)wParam;

	if (!(flags & FR_MATCHCASE))
		what.MakeLower();
	int i = GetSelectedItem( );
	int j;
	if ( i == -1 )
		SetItemState( 0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
	int columns = GetHeaderCtrl()->GetItemCount();
	if (flags & FR_DOWN)
	{
		if (flags < 0)
			i++;
		while( i < GetItemCount() )
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
	return 0;
}

void CP4ListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
    // find out what was hit
	LVHITTESTINFO ht;
	ht.pt=point;
	int index = HitTest( &ht );

	// if it wasn't on an item, return
	if( index == -1 || !(ht.flags & LVHT_ONITEM) )
		return;

	// We can't call CListView::OnLButtonDown(nFlags, point) like we want to
	// because doing so screws up the double click (you have to double click slowly
	// for it to be detected(!)), so we have to set the selection ourselves.
	SetItemState(index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	EnsureVisible(index, FALSE);

	TCHAR str[ LISTVIEWNAMEBUFSIZE + 1 ];
	GetItemText( index, 0, str, LISTVIEWNAMEBUFSIZE );
	m_DragFromItemName = str;

	// Create a suitably small drag rect around the cursor 
	CPoint pt= point;
	ClientToScreen(&pt);
	m_DragSourceRect.SetRect( max(0, pt.x - 2), max(0, pt.y - 2), 
							  max(0, pt.x + 2), max(0, pt.y + 2) );
	TryDragDrop( );
}

BOOL CP4ListCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style|=LVS_SINGLESEL | LVS_SHAREIMAGELISTS | LVS_ALIGNLEFT | LVS_REPORT;
	if (GET_P4REGPTR( )->AlwaysShowFocus())
		cs.style|=LVS_SHOWSELALWAYS;
	return CListCtrl::PreCreateWindow(cs);
}

void CP4ListCtrl::GetListItems(CStringArray *list)
{
	for(int i = 0; i < GetItemCount(); i++)
		list->Add(GetItemText(i, 0));
}

LRESULT CP4ListCtrl::OnP4ObjectFetch( WPARAM wParam, LPARAM lParam )
{
    if( GetItemCount() > 0 )
        SendMessage( WM_GETOBJECTLIST, wParam, lParam);
    else
	{
		m_PostViewUpdateMsg = WM_GETOBJECTLIST;
		m_PostViewUpdateWParam = wParam;
		m_PostViewUpdateLParam = lParam;
		ViewUpdate();
	}

    return 0;
}

LRESULT CP4ListCtrl::OnP4IntegObjectFetch( WPARAM wParam, LPARAM lParam )
{
    if( GetItemCount() > 0 )
        SendMessage( WM_INTEGGETOBJECTLIST, wParam, lParam);
    else
	{
		m_PostViewUpdateMsg = WM_INTEGGETOBJECTLIST;
		m_PostViewUpdateWParam = wParam;
		m_PostViewUpdateLParam = lParam;
		ViewUpdate();
	}

    return 0;
}

LRESULT CP4ListCtrl::OnP4WizObjectFetch( WPARAM wParam, LPARAM lParam )
{
    if( GetItemCount() > 0 )
        SendMessage( WM_WIZGETOBJECTLIST, wParam, lParam);
    else
	{
		// Wizard doesn't work if there are not any clients in the client pane
		// So simulate pressing the Back button
		CString txt;
		CP4PaneView * pv = DYNAMIC_DOWNCAST(CP4PaneView, GetWnd()->GetParent());
		CString caption = pv ? pv->GetContent()->GetCaption() : _T("Perforce Objects");
		txt.FormatMessage(IDS_NO_s_AVAILABLE, caption);
        AfxMessageBox(txt, MB_ICONEXCLAMATION);
		HWND hWnd = (HWND)wParam;
		UINT msg  = (UINT)lParam;
		::SendMessage(hWnd, msg, IDC_BACK, 0);
	}

    return 0;
}

int CP4ListCtrl::GetColNamesAndCount(CStringArray &cols)
{
	TCHAR txt[100];
	CString str;
	int nbrcols;
	LVCOLUMN column;

	column.mask = LVCF_TEXT | LVCF_SUBITEM;
	column.pszText = txt;
	column.cchTextMax = sizeof(txt)/sizeof(TCHAR)-1;
	for (nbrcols = -1; ++nbrcols < MAX_P4OBJECTS_COLUMNS; )
	{
		if (GetColumn(nbrcols, &column))
		{
			str = column.pszText;
			cols.Add(str);
		}
		else
			break;
	}
	return nbrcols;
}


LRESULT CP4ListCtrl::OnSelectThis(WPARAM wParam, LPARAM lParam)
{
	CString *name = (CString *)lParam;
	int index = FindInList( *name );

	if ( index > -1 )
	{
		SetItemState( index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
		EnsureVisible(index, FALSE);
	}
	return 0;
}
	
LRESULT CP4ListCtrl::OnP4ObjectList(WPARAM wParam, LPARAM lParam)
{
	return P4ObjectList(wParam, lParam, FALSE, FALSE);
}

LRESULT CP4ListCtrl::OnP4WizObjectList(WPARAM wParam, LPARAM lParam)
{
	return P4ObjectList(wParam, lParam, TRUE, FALSE);
}

LRESULT CP4ListCtrl::OnP4IntegObjectList(WPARAM wParam, LPARAM lParam)
{
	return P4ObjectList(wParam, lParam, FALSE, TRUE);
}

LRESULT CP4ListCtrl::P4ObjectList(WPARAM wParam, LPARAM lParam, 
								  BOOL bWiz, BOOL bInteg, BOOL bFiltered)
{
	int i;
	CString str;
	CString caption;

	CP4PaneView * pv = DYNAMIC_DOWNCAST(CP4PaneView, GetWnd()->GetParent());
	if (bWiz && pv)
		caption = pv->GetContent()->GetPlainCaption() + LoadStringResource(IDS__WIZARD);
	else
		caption = pv ? pv->GetContent()->GetCaption() : _T("Perforce Objects");

	// Get the column names and count
	CStringArray cols;
	int nbrcols = GetColNamesAndCount(cols);

	// Get the list of objects
	SET_APP_HALTED(TRUE);
    CObList *objs = new CObList;
	if (bFiltered)
	{
		for(i = 0; i < GetItemCount(); i++ )
		{
			int subitem;
			CP4Object *newObj= new CP4Object();
			for (subitem = -1; ++subitem < nbrcols; )
			{
				str = GetItemText(i, subitem);
				if (!subitem)
					newObj->Create(str);
				else
					newObj->AddField(str);
			}
			objs->AddHead(newObj);
		}
	}
	else
	{
		if (pv)
		{
			caption = pv->GetContent()->GetPlainCaption();
			if (bWiz)
				caption += LoadStringResource(IDS__WIZARD);
		}
		for(i = 0; i < m_ListAll.column[0].GetCount(); i++ )
		{
			int subitem;
			CP4Object *newObj= new CP4Object();
			for (subitem = -1; ++subitem < nbrcols; )
			{
				str = m_ListAll.column[subitem].GetAt(i);
				if (!subitem)
					newObj->Create(str);
				else
					newObj->AddField(str);
			}
			objs->AddHead(newObj);
		}
	}

    if( objs->GetCount() == 0 )
    {
		CString txt;
		txt.FormatMessage(IDS_NO_s_AVAILABLE, caption);
        AfxMessageBox(txt, MB_ICONEXCLAMATION);
        SET_APP_HALTED(FALSE);
		delete objs;
        return 0;
    }

	// Get the currently selected item's name
	i = GetSelectedItem();
	CString curr = GetItemText(i, 0);

	// Display the dialog box.
	CP4ListBrowse dlg(this, bWiz, bInteg);
	dlg.SetP4ObjectFont(GetFont());
	dlg.SetP4ObjectType(m_viewType);
	dlg.SetP4ObjectList(objs);
	dlg.SetP4ObjectCols(&cols);
	dlg.SetP4ObjectCurr(&curr);
	dlg.SetP4ObjectSKey(&m_SubKey);
	dlg.SetP4ObjectCaption(&caption);
	dlg.SetP4ObjectImage(m_iImage);
	if (bFiltered && (GetItemCount() < m_ListAll.column[0].GetCount()))
		dlg.SetP4ObjectIsFiltered(TRUE);

	INT_PTR retcode= dlg.DoModal();
    SET_APP_HALTED(FALSE);

	// Delete the object list
	for(POSITION pos=objs->GetHeadPosition(); pos!=NULL; )
		delete (CP4Object *) objs->GetNext(pos);
    delete objs;

	CString *objname= dlg.GetSelectedP4Object();
	if(retcode == IDOK && !objname->IsEmpty())
	{
		HWND hWnd = (HWND)wParam;
		UINT msg  = (UINT)lParam;
		retcode = ::SendMessage(hWnd, msg, 0, (LPARAM)objname);
    }
	else if (retcode == IDC_REFRESH)
	{
		if (bFiltered && (GetItemCount() < m_ListAll.column[0].GetCount()))
		{
			retcode = P4ObjectList(wParam, lParam, bWiz, bInteg, FALSE);
		}
		else
		{
			m_PostViewUpdateMsg = dlg.IsBranchInteg() ? WM_INTEGFETCHOBJECTLIST : WM_GETOBJECTLIST;
			m_PostViewUpdateWParam = wParam;
			m_PostViewUpdateLParam = lParam;
			ViewUpdate();
			retcode = 0;
		}
	}
	else if (retcode == IDC_BACK)
	{
		HWND hWnd = (HWND)wParam;
		UINT msg  = (UINT)lParam;
		retcode = ::SendMessage(hWnd, msg, IDC_BACK, 0);
	}
	MainFrame()->ClearStatus();
    return retcode;
}

void CP4ListCtrl::CantEditRightNow(int type)
{
	CString msg;
	msg.FormatMessage(IDS_CANTEDIT_INPROGRESS, LoadStringResource(type), LoadStringResource(type));
	AddToStatus( msg, SV_WARNING );
}
