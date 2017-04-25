//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// MSTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
// #define TRACE_HERE
#include "p4win.h"
#include "MSTreeCtrl.h"
#include "P4PaneView.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMultiSelTreeCtrl

IMPLEMENT_DYNCREATE(CMultiSelTreeCtrl, CTreeCtrl)

CMultiSelTreeCtrl::CMultiSelTreeCtrl()
{
	m_Timer=0;
	m_SelectionSet.SetSize(20,5);
	m_SelectFlags=TVIS_SELECTED;
    m_ContextContext= KEYSTROKED;
	m_CtrlDown= m_ShiftDown= m_MultiSelect= FALSE;
	m_PendingKeyedDeselect= FALSE;
	m_ToolTip = NULL;
	m_SortByFilename = m_SortByAction = m_SortByExtension = m_SortByResolveStat = FALSE;
	m_LastLButtonDown = NULL;
	ClearSelection();
}

void CMultiSelTreeCtrl::ClearSelection()
{
	XTRACE(_T("ClearSelection()\n"));
	m_LastSelect = m_LastParent = NULL;
	m_SelectionSet.RemoveAll();
    m_LastMouseOver=NULL;
}

CMultiSelTreeCtrl::~CMultiSelTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CMultiSelTreeCtrl, CTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, OnDeleteitem)
	ON_WM_SETFOCUS()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemExpanding)
	ON_WM_SETCURSOR()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_WM_LBUTTONUP()
	ON_WM_ACTIVATE()
	ON_WM_CHAR()
	ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMultiSelTreeCtrl diagnostics

#ifdef _DEBUG
void CMultiSelTreeCtrl::AssertValid() const
{
	CTreeCtrl::AssertValid();
}

void CMultiSelTreeCtrl::Dump(CDumpContext& dc) const
{
	CTreeCtrl::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMultiSelTreeCtrl message handlers

void CMultiSelTreeCtrl::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult) 
{//do a virtual function for depot view that is called by this to delete the memory that was newed.
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	TV_ITEM ptv=pNMTreeView->itemOld;

	// Get this item out of the selection set, since it was deleted
	SetSelectState(ptv.hItem, FALSE);  

	// Make sure that m_LastParent gets nulled as required
	if(ptv.hItem==m_LastParent || GetCount() == 0)
		m_LastParent=NULL;

	*pResult = 0;
}

BOOL CMultiSelTreeCtrl::DeleteAllItems()
{
	XTRACE(_T("CMultiSelSTreeCtrl::DeleteAllItems()\n"));
	ClearSelection();
	return CTreeCtrl::DeleteAllItems();
}

////////////////////////////////////////////////////////////////////////////////
// Simple utilities for parameter update

void CMultiSelTreeCtrl::SetLParam(HTREEITEM curr_item, LPARAM lParam)
{
	if (curr_item == TVI_ROOT)
		return;

	TV_ITEM item;
	item.hItem = curr_item;
	item.lParam = lParam;
	item.mask = TVIF_PARAM |TVIF_HANDLE;
	SetItem( &item );	
}

DWORD CMultiSelTreeCtrl::GetLParam(HTREEITEM curr_item)
{
	TV_ITEM item;
	item.hItem=curr_item;
	item.mask=TVIF_PARAM | TVIF_HANDLE;
	GetItem(&item );	
	return(item.lParam);
}

void CMultiSelTreeCtrl::SetItemText(HTREEITEM curr_item, LPCTSTR txt)
{
	TV_ITEM item;
	item.hItem = curr_item;
	item.mask = TVIF_TEXT | TVIF_HANDLE;
	item.pszText = ( LPTSTR ) txt;         
    SetItem(&item );	
}

CString CMultiSelTreeCtrl::GetItemText( HTREEITEM curr_item )
{
	TCHAR buf[ LONGPATH + 1 ];

	TV_ITEM item;
	item.hItem = curr_item;
	item.mask = TVIF_TEXT | TVIF_HANDLE;
	item.pszText = buf;         
    item.cchTextMax = LONGPATH ;  
	GetItem( &item );	
	return( CString( item.pszText ) );
}

void CMultiSelTreeCtrl::SetChildCount(HTREEITEM curr_item, int count)
{
	TV_ITEM item;
	item.hItem=curr_item;
	item.cChildren=count ;
	item.mask=TVIF_CHILDREN | TVIF_HANDLE;
	SetItem(&item );	
}

int CMultiSelTreeCtrl::GetChildCount(HTREEITEM curr_item)
{
	TV_ITEM item;
	item.hItem=curr_item;
	item.mask=TVIF_CHILDREN | TVIF_HANDLE;
	GetItem(&item );	
	return( item.cChildren );
}


void CMultiSelTreeCtrl::SetImage(HTREEITEM curr_item, int imageIndex, int selectedImage)
{
	TV_ITEM item;
	item.hItem=curr_item;
	item.iImage=imageIndex;
	if(selectedImage==-1)
		item.iSelectedImage=imageIndex;
	else
		item.iSelectedImage=selectedImage;
	item.mask=TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_HANDLE;
	SetItem(&item );	
}

int CMultiSelTreeCtrl::GetImage(HTREEITEM curr_item)
{
	TV_ITEM item;
	item.hItem=curr_item;
	
	item.mask=TVIF_IMAGE | TVIF_HANDLE;
	GetItem(&item );	

	return item.iImage;
}

BOOL CMultiSelTreeCtrl::HasExpandedChildren(HTREEITEM curr_item)
{
	TV_ITEM item;
	item.hItem=curr_item;
	item.mask=TVIF_CHILDREN | TVIF_STATE | TVIF_HANDLE;
	GetItem(&item );	
	if(item.cChildren > 0 && item.state & TVIS_EXPANDED)
		return TRUE;
	else
		return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// Selection Set management

// Manage the list of selected items - any item added to the selection set will
// get the current display atts for the set.  Items removed will get normal atts

// delete selection set from tree and selection set
void CMultiSelTreeCtrl::DeleteSelectedItems()	
{
	for(int i= m_SelectionSet.GetSize()-1; i >= 0; i++)
	{
		DeleteItem((HTREEITEM) m_SelectionSet.GetAt(i));	
	}
	ClearSelection();
}

// remove all from selection set and redisplay with normal atts
void CMultiSelTreeCtrl::UnselectAll()
{
	HTREEITEM item;
	for(int i= m_SelectionSet.GetSize()-1; i >= 0; i--)
	{
		item= (HTREEITEM) m_SelectionSet.GetAt(i);	
		// Undo any display atts
		SetItemState(item, 0, TVIS_CUT | TVIS_BOLD | TVIS_SELECTED);
	}
	ClearSelection();

	MainFrame()->SetMessageText(LoadStringResource(IDS_FOR_HELP_PRESS_F1));
}

static int CALLBACK SortTreeCB(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	if ((lParam1 < 1) || (lParam2 < 1))
		return -1;
	int i = 0;
	CP4FileStats *stats1 = (CP4FileStats *)lParam1;
	CP4FileStats *stats2 = (CP4FileStats *)lParam2;
	CString f1 = stats1->GetFormattedChangeFile(GET_P4REGPTR()->ShowFileType(), GET_P4REGPTR()->ShowOpenAction());
	CString f2 = stats2->GetFormattedChangeFile(GET_P4REGPTR()->ShowFileType(), GET_P4REGPTR()->ShowOpenAction());
	if ((i = f1.ReverseFind(_T('#'))) < 1)
		return f1.CompareNoCase(f2);
	f1 = f1.Left(i);
	CString e1 = ((i = f1.ReverseFind(_T('.'))) == -1) ? _T("") : f1.Right(f1.GetLength() - i - 1);
	if ((i = f1.ReverseFind(_T('/'))) == -1)
		return f1.CompareNoCase(f2);
	CString n1 = f1.Right(f1.GetLength() - i - 1);
	if ((i = f2.ReverseFind(_T('#'))) < 1)
		return f1.CompareNoCase(f2);
	f2 = f2.Left(i);
	CString e2 = ((i = f2.ReverseFind(_T('.'))) == -1) ? _T("") : f2.Right(f2.GetLength() - i - 1);
	if ((i = f2.ReverseFind(_T('/'))) == -1)
		return f1.CompareNoCase(f2);
	CString n2 = f2.Right(f2.GetLength() - i - 1);
	int sortByFilename        = lParamSort & 0x8;
	int sortByAction      = lParamSort & 0x4;
	int sortByResolveStat = lParamSort & 0x2;
	int sortByExtension   = lParamSort & 0x1;
	i = 0;
	if (sortByResolveStat)
	{
		int r1, r2;
		
		if (stats1->IsUnresolved())		r1 = 1;
		else if (stats1->IsResolved())	r1 = 2;
		else							r1 = 3;
		
		if (stats2->IsUnresolved())		r2 = 1;
		else if (stats2->IsResolved())	r2 = 2;
		else							r2 = 3;
		
		i = r1 - r2;
	}
	if (i)
		return i;

	if (sortByAction)
	{
		int a1, a2;
		
		a1 = stats1->IsMyOpen() ? stats1->GetMyOpenAction() : stats1->GetOtherOpenAction();
		a2 = stats2->IsMyOpen() ? stats2->GetMyOpenAction() : stats2->GetOtherOpenAction();
		i = a1 - a2;
	}
	if (i)
		return i;

	if (sortByExtension)
		i = e1.CompareNoCase(e2);
	if (i)
		return i;

	if (sortByFilename)
		i = n1.CompareNoCase(n2);
	if (i)
		return i;

	return f1.CompareNoCase(f2);

}

void CMultiSelTreeCtrl::OnTimer(UINT nIDEvent) 
{
	CTreeCtrl::OnTimer(nIDEvent);
	if (nIDEvent == SORT_TIMER)
	{
		m_Timer = 0;
		::KillTimer(m_hWnd, SORT_TIMER);
		SortTree();
	}
}

// Recursively sort the entire tree
void CMultiSelTreeCtrl::SortTree(HTREEITEM topNode/*=NULL*/, HTREEITEM parentNode/*=NULL*/)
{
	HTREEITEM item;

	// Sort things at the this level
	if (parentNode && (m_SortByExtension || m_SortByResolveStat 
					|| m_SortByAction    || m_SortByFilename))
	{
		TVSORTCB tvsortcb;
		tvsortcb.hParent = topNode;
		tvsortcb.lParam = (m_SortByResolveStat ? 2 : 0) + (m_SortByExtension ? 1 : 0) 
						+ (m_SortByFilename ? 8 : 0)    + (m_SortByAction ? 4 : 0);
		tvsortcb.lpfnCompare = SortTreeCB;
		SortChildrenCB(&tvsortcb);
	}
	else 
		SortChildren(topNode);

	// Get the first item at this level
	if(topNode == NULL)
		item=GetNextItem(TVI_ROOT, TVGN_ROOT);
	else
		item=GetChildItem(topNode);   // Get first child

	// Recurse all items that have children
	while(item != NULL)
	{
		if(ItemHasChildren(item))
			SortTree(item, topNode);
		item=GetNextSiblingItem(item);
	}
}


// add one item to selection set, or remove it	
BOOL CMultiSelTreeCtrl::SetSelectState(HTREEITEM item, BOOL selected)
{
//	ASSERT(item != NULL);	// Can't ASSERT here! When called from context menu click below any changes, there is no 'item'
	if(item==NULL)
		return FALSE;

	HTREEITEM parent=GetParentItem(item);
	int index = SelectionToIndex(item);
	BOOL found = index != -1;
	BOOL success=TRUE;

	if(found && !selected)  
	{
		// Clear the selected appearance for this item
		success=SetItemState(item, 0, TVIS_CUT | TVIS_BOLD | TVIS_SELECTED);

		// and remove item from selection set
		if(success)
		{
			m_SelectionSet.RemoveAt(index);
			if(m_SelectionSet.GetSize()==0)
				SelectItem(NULL);
		}
	}
	else if(!found && selected)
	{
		// Set the appearance for this item
		success=SetItemState(item, m_SelectFlags, TVIS_CUT | TVIS_BOLD | TVIS_SELECTED);

		// and add it to selection set
		if(success)
		{
			m_SelectionSet.Add((DWORD) item);
			if (!m_ShiftDown && !m_MultiSelect)
				SelectItem(item);
		}
	}
	// !found && !selected --> select atts already correct
	// found && selected --> select atts already correct
	
	if(selected)
	{
		m_LastSelect=item;
		m_LastParent=parent;
	}
	else
	{
		if(m_SelectionSet.GetSize()==0)
			m_LastParent=m_LastSelect=NULL;
		else
		{
			if(found)
				// Only change m_LastSelect if we deleted a selected item
				m_LastSelect= (HTREEITEM) m_SelectionSet.GetAt(0);

			// m_LastParent must still be valid
			m_LastParent=GetParentItem(m_LastSelect);
		}
	}

	ShowNbrSelected();
	return success;
}

// add one item to selection set, or remove it	
BOOL CMultiSelTreeCtrl::ToggleSelectState(HTREEITEM item)
{
	ASSERT(item != NULL);
	if(item==NULL)
		return FALSE;

	// All selections required to be under same parent
	HTREEITEM parent=GetParentItem(item);
	if(parent != m_LastParent && m_LastParent != NULL)
		return FALSE;

	int index = SelectionToIndex(item);
	BOOL found = index != -1;
	BOOL success=TRUE;

	if(found)  
	{
		// Clear the selected appearance for this item
		success=SetItemState(item, 0, TVIS_CUT | TVIS_BOLD | TVIS_SELECTED);

		// and remove item from selection set
		if(success)
			m_SelectionSet.RemoveAt(index);
	}
	else 
	{
		// Set the appearance for this item
		success=SetItemState(item, m_SelectFlags, TVIS_CUT | TVIS_BOLD | TVIS_SELECTED);

		// and add it to selection set
		if(success)
			m_SelectionSet.Add((DWORD) item);
	}
	
	m_LastSelect=item;
	if(m_SelectionSet.GetSize()==0)
		m_LastParent=NULL;
	else
		m_LastParent=parent;

	ShowNbrSelected();
	return success;
}

 
// first item is m_LastSelect
BOOL CMultiSelTreeCtrl::RangeSelect(HTREEITEM secondItem)
{
	ASSERT(secondItem != NULL);
	if(secondItem == NULL)
		return FALSE;

	// Special case #1 - anchor point and current point the same
	if(m_LastSelect == secondItem)
		return TRUE;  // nothing to do

	// Special case #2 - no anchor point, just a regular selection
	if(m_LastSelect == NULL)
		return SetSelectState(secondItem, TRUE);

	// Do both items have same parent - an arbitrary restriction to avoid boggling select combos
	// Look up last parent, since m_LastParent will be null if no items currently selected
	HTREEITEM parent=GetParentItem(secondItem);
	HTREEITEM lastParent=GetParentItem(m_LastSelect);
	if(lastParent != parent)
		return FALSE;

	// Allow subclass to veto the operation
	if( !OKToAddSelection( secondItem ) )
		return FALSE;

	BOOL success=TRUE;
	// Find out which one is higher in the tree by comparing item rects - this avoids getting
	// stung by some possible sort order we dont know about
	RECT lastRect, thisRect;
	GetItemRect(m_LastSelect, &lastRect, TRUE);
	GetItemRect(secondItem, &thisRect, TRUE);

	HTREEITEM topItem, bottomItem;
	if(lastRect.top < thisRect.top)   // current selection is below anchor
	{
		topItem=m_LastSelect;
		bottomItem=secondItem;
	}
	else
	{
		topItem=secondItem;
		bottomItem=m_LastSelect;
	}
	
	// Select the items
	while(topItem != bottomItem)
	{
		if(!SetSelectState(topItem, TRUE))
			success=FALSE;
		topItem=GetNextSiblingItem(topItem);
	}

	if(!SetSelectState(topItem, TRUE))
		success=FALSE;

	return success;
}

// Access the selected items
inline int CMultiSelTreeCtrl::SelectionToIndex(HTREEITEM item)
{
	// First, see if item is in list
	for(int index=m_SelectionSet.GetSize()-1; index >= 0; index--)
	{
		if(item == (HTREEITEM) m_SelectionSet.GetAt(index))
		{
			return index;
		}
	}

	return -1;
}

// Utility to support context menues.  Finds the treeitem for a
// mouseclick, and useable screen coords for a shift+f10 key hit
void CMultiSelTreeCtrl::SetItemAndPoint( HTREEITEM &item, CPoint &point )
{
    CString text ;
    if( m_ContextContext == MOUSEHIT )
    {
        ScreenToClient( &point );
        TV_HITTESTINFO ht;
	    ht.pt = point;
	    item = HitTest( &ht );
	    m_ContextContext= KEYSTROKED;
    }
    else
    {
        CRect rect;
        GetClientRect(&rect);   
        point= rect.CenterPoint();
        if(GetSelectedCount() > 0)
            item = GetSelectedItem(0);
        else
            item=NULL;
    }
}

HTREEITEM CMultiSelTreeCtrl::GetSelectedItem(int index)
{
	if (index < m_SelectionSet.GetSize())
		return (HTREEITEM) m_SelectionSet.GetAt(index);
	return NULL;
}

int CMultiSelTreeCtrl::GetSelectedCount()
{
	return m_SelectionSet.GetSize();
}

// Change the appearance of all selected items
void CMultiSelTreeCtrl::SetAppearance(BOOL bold, BOOL selected, BOOL cut)
{
	m_SelectFlags=0;

	if(bold)
		m_SelectFlags= TVIS_BOLD;

	if(selected)
		m_SelectFlags |= TVIS_SELECTED;

	if(cut)
		m_SelectFlags |= TVIS_CUT;

	ApplySelectAtts(m_SelectFlags);
}

void CMultiSelTreeCtrl::ApplySelectAtts(UINT flags)
{
	HTREEITEM item;
	for(int i= m_SelectionSet.GetSize()-1; i >= 0; i--)
	{
		item= (HTREEITEM) m_SelectionSet.GetAt(i);	
		// Undo any display atts
		SetItemState(item, flags, TVIS_CUT | TVIS_BOLD | TVIS_SELECTED);
	}

	ShowNbrSelected();
}

void CMultiSelTreeCtrl::SetItemAtt(HTREEITEM item, UINT flags, BOOL set)
{
    ASSERT( item != NULL );

    if( set )
	    SetItemState(item, flags, flags);
    else
        SetItemState(item, 0, flags);

	ShowNbrSelected();
}

BOOL CMultiSelTreeCtrl::IsBoldAtt()
{
	return ((m_SelectFlags & TVIS_BOLD)==TVIS_BOLD);
}

BOOL CMultiSelTreeCtrl::IsSelectAtt()
{
	return ((m_SelectFlags & TVIS_SELECTED)==TVIS_SELECTED);
}

BOOL CMultiSelTreeCtrl::IsCutAtt()
{
	return ((m_SelectFlags & TVIS_CUT)==TVIS_CUT);
}

void CMultiSelTreeCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	// When activated, CTreeView will highlight first tree item by default, so 
	// undo that highlight and then redisplay all selected items
	CTreeCtrl::OnSetFocus(pOldWnd);

	m_ViewIsActive = TRUE;

	// Update our Shift and Control keys states
	m_CtrlDown = ::GetKeyState(VK_CONTROL) & 0x8000 ? TRUE : FALSE;
	m_ShiftDown= ::GetKeyState(VK_SHIFT) & 0x8000 ? TRUE : FALSE;

	Select(NULL, TVGN_CARET);
	ApplySelectAtts(m_SelectFlags);
	if (GET_P4REGPTR( )->AlwaysShowFocus())
		InvalidateRect(NULL);
}


/*
	_________________________________________________________________
*/

void CMultiSelTreeCtrl::OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	HTREEITEM parent=m_LastParent;

	while( 1 )
	{
		//		If expanding/collapsing node is a parent of selected items, 
		//		unselect all.
		//		No need to check pNMTreeView->action, since no items will 
		//		be selected under parent if expanding, and all must be 
		//		cleared while deleting
		//
		if(pNMTreeView->itemNew.hItem == parent)
		{
			UnselectAll();
			break;
		}
		
		if(parent == NULL || parent == TVI_ROOT)
			break;

		parent=GetParentItem(parent);
	}

	//		whenever we expand a folder, we have to call both p4 dirs 
	//		and p4 fstat for that subdirectory. ExpandTree is a 
	//		virtual function in depotview that does this.
	//
	if ( pNMTreeView->action == TVE_EXPAND )
		ExpandTree( pNMTreeView->itemNew.hItem );
	else if ( pNMTreeView->action == TVE_COLLAPSE )
		CollapseTree( pNMTreeView->itemNew.hItem );

	// wait at least 20 secs before autopolling for updates
	MainFrame()->WaitAWhileToPoll();

	*pResult = 0;
}

void CMultiSelTreeCtrl::ScrollToFirstItem( HTREEITEM firstItem )
{
    ASSERT(firstItem != NULL);
    HTREEITEM lastItem= firstItem;

    EnsureVisible( firstItem );
    int c=GetVisibleCount();

    for( int i=1; i < c && lastItem != NULL; i++ )
        lastItem= GetNextVisibleItem( lastItem );
            
    // Make the last item visible, then scroll up if required to make sure the
    // first item is still visible (user may have changed window height while
    // we were running the refresh)
    if( lastItem != NULL )
    {
        EnsureVisible( lastItem );
        EnsureVisible( firstItem );
    }
}


/*
	_________________________________________________________________

	virtual class function just so i can call the proper commands in depotview.cpp
	_________________________________________________________________
*/

BOOL CMultiSelTreeCtrl::ExpandTree( const HTREEITEM item )
{
	return TRUE;
}


BOOL CMultiSelTreeCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(SERVER_BUSY())
		return SET_BUSYCURSOR();
	else
		return CTreeCtrl::OnSetCursor(pWnd, nHitTest, message);
}


////////////////////////////////////////////////////////////////////
//	
// Functions to support mouse flyover status messages.  The status message
// will remain the default message unless OnSetFlyoverMessage() is over-ridden
// by the subclass.  That over-ride can just set message text, aand can optionally
// call SetItemFocus() to draw a focus rect arount the flyover item text.
//
////////////////////////////////////////////////////////////////////


void CMultiSelTreeCtrl::SetItemFocus(HTREEITEM item) 
{
    if( item == m_LastMouseOver )
        return;

    if( m_LastMouseOver != NULL )
	{
		CRect rect;
		GetItemRect( m_LastMouseOver, &rect, TRUE );
		rect.DeflateRect(0,1,0,1);
	
		CDC *pDC= GetDC();
		CBrush brush;
		if( IsSelected( m_LastMouseOver ) )
		{
			if( GetTextColor() != -1 )
				brush.CreateSolidBrush( GetTextColor() );
			else
				brush.CreateSolidBrush( GetSysColor( COLOR_WINDOWTEXT ) );
		}
		else
			brush.CreateSolidBrush( GetSysColor( COLOR_WINDOW ) );
		pDC->FrameRect( &rect, &brush );

		ReleaseDC( pDC );
	}
    	
    m_LastMouseOver= item;

    if( m_LastMouseOver != NULL )
    {
		CRect rect;
		GetItemRect( item, &rect, TRUE );
		rect.DeflateRect(0,1,0,1);

		CDC *pDC= GetDC();
		pDC->DrawFocusRect(&rect);

		ReleaseDC( pDC );
	}
		
}

void CMultiSelTreeCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	CTreeCtrl::OnKillFocus(pNewWnd);
	
	m_ViewIsActive = FALSE;
	m_ToolState = -1;	// So that we set the colors correctly next time we get a mouse move
    RestoreStatusMessage();
	if (GET_P4REGPTR( )->AlwaysShowFocus())
	{
		InvalidateRect(NULL);
	}
	else
	{
		ApplySelectAtts(0);
		SetItemFocus(NULL);
	}
}

inline void CMultiSelTreeCtrl::RestoreStatusMessage( )
{
    MainFrame()->SetMessageText(LoadStringResource(IDS_FOR_HELP_PRESS_F1));
}

void CMultiSelTreeCtrl::OnSetFlyoverMessage( HTREEITEM item )
{
    RestoreStatusMessage();
    SetItemFocus(NULL);
}

void CMultiSelTreeCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	// update shift and control flags
	m_CtrlDown= nFlags & MK_CONTROL ? TRUE : FALSE;
	m_ShiftDown= nFlags & MK_SHIFT  ? TRUE : FALSE;

	if( !m_ViewIsActive )
        return;

    // find out what mouse is over
	TV_HITTESTINFO ht;
	ht.pt=point;
    ht.flags=TVHT_ONITEMLABEL | TVHT_ONITEMICON | TVHT_ONITEMBUTTON;
	HTREEITEM currentItem=HitTest( &ht	);

    if(currentItem != m_LastMouseOver)
    {
        // see if subclassed views want to display something as a status
        // message for the item mouse is over
        if(currentItem == NULL)
            OnSetFlyoverMessage( NULL );
        else
            OnSetFlyoverMessage( currentItem );
    }

	CTreeCtrl::OnMouseMove(nFlags, point);
}

/*
	_________________________________________________________________

	Curious observation:  on a Lbutton click, point is client coords
    on a right button click, its screen coords
	_________________________________________________________________
*/

void CMultiSelTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// update shift and control flags
	m_CtrlDown= nFlags & MK_CONTROL ? TRUE : FALSE;
	m_ShiftDown= nFlags & MK_SHIFT  ? TRUE : FALSE;

    // find out what was hit
	TV_HITTESTINFO ht;
	ht.pt=point;
	m_LastLButtonDown=HitTest( &ht	);
	if(m_LastLButtonDown==NULL)
		return;
    
    BOOL success;
	m_PendingDeselect=FALSE;

	if(m_LastLButtonDown != TVI_ROOT && (ht.flags & TVHT_ONITEM ))
	{
		// Select nothing so there is no focus rect
		SelectItem(NULL);

		// Add the item to the selection set
		if(nFlags &	MK_CONTROL)
		{
			// Make sure its a valid selection
			if( !OKToAddSelection( m_LastLButtonDown ) )
				return;
			// If not in set, add it
			if(!IsSelected(m_LastLButtonDown))	
				success=SetSelectState(m_LastLButtonDown, TRUE);
			else
				// removing from set, or possibly re-clicking for drag
				success=m_PendingDeselect=TRUE;
		}
		else if(nFlags & MK_SHIFT)
		{
			success=RangeSelect(m_LastLButtonDown);
		}
		else
		{
			if(!IsSelected(m_LastLButtonDown))
			{
				UnselectAll();
				success=SetSelectState(m_LastLButtonDown, TRUE);
				ASSERT(GetSelectedCount());
			}
			else 
			{
				success= TRUE;
				m_PendingDeselect= TRUE;
			}
		}

		// wait at least 20 secs before autopolling for updates
		MainFrame()->WaitAWhileToPoll();

		if(!success)
			return;

		// Store the clicked item
		m_DragFromItem=m_LastLButtonDown;
		
		// Force the stinking item to repaint, because new select atts seem to
		// be lost in commctl32.dll occasionally
		SetAppearance(FALSE, TRUE, FALSE);
		GetItemRect(m_LastLButtonDown, &m_DragSourceRect, TRUE);
		RedrawWindow( m_DragSourceRect, NULL, RDW_UPDATENOW );

		// Then create a suitably small drag rect around the cursor 
		CPoint pt= point;
		ClientToScreen(&pt);
		m_DragSourceRect.SetRect(	max(0, pt.x - 2), max(0, pt.y - 2), 
									max(0, pt.x + 2), max(0, pt.y + 2) );

		// The drag drop attempt will clear m_PendingDeselect if a drag is attempted
		TryDragDrop( m_LastLButtonDown );
			
		if( m_PendingDeselect )
		{
			if( nFlags & MK_CONTROL )
				SetSelectState(m_LastLButtonDown, FALSE);
			else
			{
				UnselectAll();
				success=SetSelectState(m_LastLButtonDown, TRUE);
			}
		}
		
		// Make sure selection set is properly displayed
		SetAppearance(FALSE, TRUE, FALSE);
	}
	else
	{
		if(ht.flags & TVHT_ONITEM && (nFlags &	MK_CONTROL || nFlags & MK_SHIFT) )
			return;
		else 
		{
			if (ht.flags & TVHT_ONITEM && m_ViewIsActive )
			{
				// Select just the one item
				UnselectAll();
				SetSelectState(m_LastLButtonDown, TRUE);
			}
				
			// Clicked on something other than a bimap or item text, so just call
			// the default hanlder and make sure nothing gets selected
			CTreeCtrl::OnLButtonDown(nFlags, point);
			SelectItem( NULL);	
		}
	}
}

void CMultiSelTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CTreeCtrl::OnLButtonUp(nFlags, point);
	ApplySelectAtts(m_SelectFlags);
}

#define VK_PGUP 33
#define VK_PGDN 34

void CMultiSelTreeCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	MainFrame()->SetGotUserInput( );

	if (nChar == VK_RETURN || nChar == VK_APPS)
		return;

	if (nChar == VK_TAB && m_CtrlDown)	// Ctrl+TAB switches to opposite pane
	{
		BOOL bShift = m_ShiftDown;
		m_CtrlDown= m_ShiftDown= FALSE;	// clear these because we may miss the Up
		m_PendingKeyedDeselect = FALSE;	//	in the other pane
		MainFrame()->SwitchPanes(DYNAMIC_DOWNCAST(CView, GetParent()), bShift);
		return;
	}

	if( nChar == VK_CONTROL )
		m_CtrlDown= TRUE;
	else if( nChar == VK_SHIFT )
	{
		m_ShiftDown= TRUE;
		if( m_SelectionSet.GetSize() > 0 )
		{
			m_AnchorItem= m_LastSelect;
			m_PendingKeyedDeselect= TRUE;
		}
	}
	
	if( m_ShiftDown && m_SelectionSet.GetSize() > 0 )
	{
		// Try to range select, and scroll the tree only as necessary
		// to ensure that the last item selected is visible
		switch( nChar )
		{
		case VK_DOWN:
			ExpandSelection( -1 );
			break;
		case VK_UP:
			ExpandSelection( 1 );
			break;
		case VK_END:
			ExpandSelection( - (int) GetCount() );
			break;
		case VK_HOME:
			ExpandSelection( (int) GetCount() );
			break;
		case VK_PGDN:
			ExpandSelection( - (int) GetVisibleCount() +1 );
			break;
		case VK_PGUP:
			ExpandSelection( (int) GetVisibleCount() -1 );
			break;
		default:
			break;
		}
	}
	else if( m_CtrlDown || m_SelectionSet.GetSize() == 0)
	{
		// Scroll the tree, but do not fool with the selection set
		// or move a focus rect around
		switch( nChar )
		{
		case VK_DOWN:
			ScrollTree( -1 );
			break;
		case VK_UP:
			ScrollTree( 1 );
			break;
		case VK_END:
			ScrollTree( - (int) GetCount() );
			break;
		case VK_HOME:
			ScrollTree( GetCount() );
			break;
		case VK_PGDN:
			ScrollTree( - (int) GetVisibleCount() +1 );
			break;
		case VK_PGUP:
			ScrollTree( GetVisibleCount() -1 );
			break;
		default:
			break;
		}
	}
	else
	{
		// Deselect all, reposition at m_LastSelect + offset, reselect the new
		// item, and ensure visible
		HTREEITEM item= m_LastSelect;
		ASSERT( item != NULL );
		HTREEITEM newItem;
		UnselectAll();
#if 0
		HTREEITEM lastItem;
		int i, count;
#endif
		switch( nChar )
		{
		case VK_END:
			ScrollTree( - (int) GetCount() );
			newItem= GetNextItem(TVI_ROOT, TVGN_LASTVISIBLE);
			break;
		case VK_HOME:
			ScrollTree( GetCount() );
			newItem= GetNextItem(TVI_ROOT, TVGN_ROOT );
			break;
#if 0	// This code was removed to fix job004105 - removing it causes the DepotView to behave like a normal TreeView when page-up and page-down are pressed
		case VK_PGDN:
			count=GetVisibleCount() - 1;
			newItem= GetFirstVisible();

			// First, find the last visible item on the screen
			for( i=0; i< count; i++)
			{
				lastItem= newItem;
				newItem= GetNextVisible(newItem );
				if( newItem == NULL )
				{
					newItem= lastItem;
					break;
				}
			}

			// If current item is the last one on the screen, move down
			// a page beyond the end of current page
			if( newItem == item )
			{
				for( i=0; i< count; i++)
				{
					lastItem= newItem;
					newItem= GetNextVisible( newItem );
					if( newItem==NULL)
					{
						newItem= lastItem;
						break;
					}
				}
			}
			break;
		case VK_PGUP:
			if( item == GetFirstVisible() )
			{
				count=GetVisibleCount() - 1;
				newItem= GetFirstVisible();
				for( i=0; i< count; i++)
				{
					lastItem= newItem;
					newItem= GetPrevVisible( newItem );
					if( newItem==NULL)
					{
						newItem= lastItem;
						break;
					}
				}
			}
			else
				newItem= GetFirstVisible();
			break;
#endif
		default:
			newItem= NULL;
			break;
		}

		// If something went wrong, like hitting top or bottom,
		// use our saved copy of m_LastSelect
		if( newItem == NULL )
			newItem= item;

		if(newItem != NULL )
		{
			SetSelectState( newItem, TRUE );
			EnsureVisible( newItem );

			// Redraw w/out erase to avoid slow video update
			RedrawWindow( NULL, NULL, RDW_UPDATENOW );
		}
	}
	if (!m_CtrlDown && !m_ShiftDown)
		CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMultiSelTreeCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_RETURN)
	{
		HTREEITEM item = GetSelectedItem(0);
		if (item)
			OnLButtonDblClk(item);
		return;
	}
	CTreeCtrl::OnChar(nChar, nRepCnt, nFlags);
}

void CMultiSelTreeCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if( nChar == VK_CONTROL )
		m_CtrlDown= FALSE;
	else if( nChar == VK_SHIFT )
	{
		m_ShiftDown= FALSE;
		m_PendingKeyedDeselect= FALSE;
	}
	
	CTreeCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}

// User mouse-keyed a request to expand the current selection.  Note that
// the selection set can actually shrink if the already selected items are
// not contiguous - per Exploder convention
void CMultiSelTreeCtrl::ExpandSelection( int linesToExpand )
{
	ASSERT( m_SelectionSet.GetSize() > 0 );
	if( m_AnchorItem == NULL )
	{
		ASSERT(0);
		return;
	}

	MainFrame()->WaitAWhileToPoll();	// wait at least 20 secs before autopolling for updates

	HTREEITEM currentItem= m_AnchorItem;

	if( linesToExpand > 0 )
	{
		// Scrolling up
		for( int i=0; i < linesToExpand; i++ )
		{
			HTREEITEM lastGoodItem= currentItem;
			currentItem= GetPrevSiblingItem(currentItem);
			if( currentItem == NULL )
			{
				currentItem= lastGoodItem;
				break;
			}
			else
			{
				if( m_PendingKeyedDeselect )
					DoKeyedDeselect( FALSE );
				if( IsSelected( currentItem ) )
					SetSelectState( lastGoodItem, FALSE );  // Selection shrinking
				else if( OKToAddSelection( currentItem ) )
					SetSelectState( currentItem, TRUE );  // Selection growing
				else
					break;
			}
		}
	}
	else
	{
		// Scrolling down
		for( int i=0; i < (-linesToExpand); i++ )
		{
			HTREEITEM lastGoodItem= currentItem;
			currentItem= GetNextSiblingItem(currentItem);
			if( currentItem == NULL )
			{
				currentItem= lastGoodItem;
				break;
			}
			else
			{
				if( m_PendingKeyedDeselect )
					DoKeyedDeselect( TRUE );

				if( IsSelected( currentItem ) )
					SetSelectState( lastGoodItem, FALSE );  // Selection shrinking
				else if( OKToAddSelection( currentItem ) )
					SetSelectState( currentItem, TRUE );  // Selection growing
				else
					break;
			}

		}
	}

	m_AnchorItem= currentItem;
	EnsureVisible( m_AnchorItem );
}

// When starting a mouse-key select, the anchor point, and all contiguous 
// selections that are not in the path of the current item can remain 
// selected.  All other selections are tossed, like Exploder does.
void CMultiSelTreeCtrl::DoKeyedDeselect( BOOL scrollingDown )
{
	CDWordArray keepSet;
	HTREEITEM currentItem= m_AnchorItem;
	
	// Record the contiguous selection's we're keeping
	keepSet.Add( (DWORD) m_AnchorItem );
	while(1)
	{
		if( scrollingDown )
			currentItem= GetPrevSiblingItem( currentItem);
		else
			currentItem= GetNextSiblingItem( currentItem);

		if( currentItem == NULL || !IsSelected( currentItem ) )
			break;

		keepSet.Add( (DWORD) currentItem );
	}

	// Unselect everything
	//
	int i;
	for( i= m_SelectionSet.GetSize()-1; i >= 0; i-- )
	{
		currentItem= (HTREEITEM) m_SelectionSet.GetAt(i);	
		// Undo any display atts
		SetItemState(currentItem, 0, TVIS_CUT | TVIS_BOLD | TVIS_SELECTED);
	}

	// Then select everything in the keepset
	//
	m_SelectionSet.RemoveAll();
	for( i= keepSet.GetSize()-1; i>=0; i-- )
		SetSelectState( (HTREEITEM) keepSet.GetAt(i), TRUE );

	m_PendingKeyedDeselect= FALSE;
	ShowNbrSelected();
}

BOOL CMultiSelTreeCtrl::ScrollTree( int linesToScroll ) 
{
	BOOL moved= FALSE;
	HTREEITEM firstItem;
	HTREEITEM currentItem;

	if( linesToScroll < 0 )
	{
		// Scrolling down
		firstItem=GetFirstVisibleItem();
		long visible=GetVisibleCount();	
		int count=0;

		for(int i=0; i < visible - linesToScroll; i++)
		{
			currentItem= firstItem;
			firstItem= GetNextVisibleItem(currentItem);
			if( firstItem == NULL )
			{
				firstItem= currentItem;
				break;
			}
			else
				count++;
		}
		if( count >= visible )
			moved= TRUE;
	}
	else if( linesToScroll > 0 )
	{
		// Scrolling up
		firstItem=GetFirstVisibleItem();
		
		for(int i=0; i < linesToScroll; i++)
		{
			currentItem= firstItem;
			firstItem= GetPrevVisibleItem(currentItem);
			if( firstItem == NULL )
			{
				firstItem= currentItem;
				break;
			}
			else
				moved= TRUE;
		}
	}
    else
    {
        ASSERT(0);
        return moved;
    }

	// Turn on redraws
	if(moved)
	{
		EnsureVisible(firstItem);

		// Redraw w/out erase to avoid slow video update
		RedrawWindow( NULL, NULL, RDW_UPDATENOW );
	}

	return moved;
}

void CMultiSelTreeCtrl::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	if (pNMTreeView->action == TVC_BYKEYBOARD)
	{
		HTREEITEM currentItem = CTreeCtrl::GetSelectedItem();
		if ((currentItem != NULL) && !IsSelected(currentItem))
		{
			UnselectAll();
			SetSelectState(currentItem, TRUE);
			ASSERT(GetSelectedCount());
		}
	}
	*pResult = 0;
}

BOOL CMultiSelTreeCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style|=TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;
	if (GET_P4REGPTR( )->AlwaysShowFocus())
		cs.style|=TVS_SHOWSELALWAYS;
	return CTreeCtrl::PreCreateWindow(cs);
}

void CMultiSelTreeCtrl::ShowNbrSelected()
{
	if (m_MultiSelect)
		return;
	CString msg;
	int n = m_SelectionSet.GetSize();
	if (n < 2)
		msg = LoadStringResource(IDS_FOR_HELP_PRESS_F1);
	else
		msg.FormatMessage(IDS_NBR_n_ITEMSEL, n);
	MainFrame()->SetMessageText(msg);
}

