//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//
//
// MSTreeCtrl.h : header file
//
// Class CMultiSelTreeCtrl is a TreeCtrl that manages a selection set of items that
// are not actually selected.  The selection set is made to appear selected by
// setting the selected, bold and or cut attributes for all items in the set.
// 
// This class only manages a list and display attributes.  It must be subclassed 
// in order to provide the logic that decides which items to select or unselect


#ifndef __MSTREECTRL
#define __MSTREECTRL

#include "P4PaneContent.h"

/////////////////////////////////////////////////////////////////////////////
// CMultiSelTreeCtrl

class CMultiSelTreeCtrl : public CTreeCtrl, public CP4PaneContent
{
protected:
	CMultiSelTreeCtrl();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMultiSelTreeCtrl)

	enum
	{
		EXPAND_FOLDER = -1,
		FOLDER_ALREADY_EXPANDED = -2,
	};
private:
	CDWordArray m_SelectionSet;
	HTREEITEM m_LastParent;
	HTREEITEM m_LastSelect;
	HTREEITEM m_AnchorItem;
	UINT m_SelectFlags;
	BOOL m_CtrlDown;
	BOOL m_ShiftDown;
	BOOL m_MultiSelect;
	BOOL m_PendingKeyedDeselect;
	
protected:
	enum ContextMenuContext
	{
		KEYSTROKED,
		MOUSEHIT
	} m_ContextContext;
    HTREEITEM m_LastMouseOver;
	HTREEITEM m_LastLButtonDown;
	BOOL m_PendingDeselect;
	HTREEITEM m_DragFromItem;
	CRect m_DragSourceRect;
    BOOL m_ViewIsActive;
	CToolTipCtrl * m_ToolTip;
	int m_ToolState;
	UINT m_Timer;
	BOOL m_SortByFilename;
	BOOL m_SortByAction;
	BOOL m_SortByExtension;
	BOOL m_SortByResolveStat;


// Attributes
public:
	virtual CWnd * GetWnd() { return this; }
	int GetSelectionSetSize() { return m_SelectionSet.GetSize(); }

// Operations
private:
	void ClearSelection();

protected:
	// A faster alternative to CTreeCtrl::DeleteAllItems - faster since it doesnt call
	// this class's OnDeleteItem() for each item in the tree 
	BOOL DeleteAllItems();

	// Manage the list of selected items - any item added to the selection set will
	// get the current display atts for the set.  Items removed will get normal atts
	void DeleteSelectedItems();	// delete selection set from tree and selection set
	void UnselectAll();			// remove all from selection set
	
	// CTreeCtrl has a broken recursive sort capability, so roll our own
	void SortTree(HTREEITEM topNode=NULL,HTREEITEM parentNode=NULL);

	BOOL SetSelectState(HTREEITEM item, BOOL selected);  // one item to or from selection set
	BOOL ToggleSelectState(HTREEITEM item);				 // one item to or from selection set
	BOOL RangeSelect(HTREEITEM secondItem);				 // first item is m_LastSelect
	void DoKeyedDeselect( BOOL scrollingDown );

    void ScrollToFirstItem( HTREEITEM firstItem );

	// Tree scroll support
	BOOL ScrollTree(int linesToScroll); 
	void ExpandSelection( int linesToExpand );
	
    // Utility to support context menus
    void SetItemAndPoint( HTREEITEM &item, CPoint &point );

	// Utilities to update/get item atts
	void SetImage(HTREEITEM curr_item, int imageIndex, int selectedImage=-1);
	void SetLParam(HTREEITEM curr_item, LPARAM lParam);
	void SetItemText(HTREEITEM curr_item, LPCTSTR txt);
	void SetChildCount(HTREEITEM curr_item, int count);
	DWORD GetLParam(HTREEITEM curr_item);
	int GetImage(HTREEITEM curr_item);
	int GetChildCount(HTREEITEM curr_item);
	BOOL HasExpandedChildren(HTREEITEM curr_item);

	// Access the selected items
	int SelectionToIndex(HTREEITEM item);
	BOOL IsSelected(HTREEITEM item) { return SelectionToIndex(item) != -1; }
	inline HTREEITEM GetLastSelectionParent() { return m_LastParent; }
	inline HTREEITEM GetLastSelection() { return m_LastSelect; }

	// Change the appearance of all selected items
	void SetAppearance(BOOL bold, BOOL selected, BOOL cut);
    void SetItemAtt(HTREEITEM item, UINT flags, BOOL set);
    void SetItemFocus(HTREEITEM item) ;
	void ApplySelectAtts(UINT flags);
	UINT GetSelectAtts() { return m_SelectFlags; };
	void SetSelectAtts(UINT flag) { m_SelectFlags = flag; };
	BOOL IsBoldAtt();
	BOOL IsSelectAtt();
	BOOL IsCutAtt();

    // Support for mouse flyover status messages
    virtual void OnSetFlyoverMessage( HTREEITEM item );
    inline void RestoreStatusMessage( );

	// Virt func to attempt drag drop during left mouse button down
	virtual BOOL TryDragDrop( HTREEITEM currentItem ) { return FALSE; }

	// Virt func to verify that adding to the selection set is OK
	virtual BOOL OKToAddSelection( HTREEITEM currentItem ) { return TRUE; }

	// Virt func to handle a left dbl clk after the Item is determined
	virtual void OnLButtonDblClk( HTREEITEM currentItem ) { return; }

// Overrides
protected:

// Implementation
protected:
	virtual ~CMultiSelTreeCtrl();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	virtual BOOL ExpandTree( const HTREEITEM item );
	virtual BOOL CollapseTree( const HTREEITEM item ) { return TRUE; }
	afx_msg void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()

public:
	void SetMultiSelect(BOOL b) { m_MultiSelect = b; }
	HTREEITEM GetSelectedItem(int index);
	CString GetItemText(HTREEITEM curr_item);
	int GetSelectedCount();

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	void ShowNbrSelected();
};

/////////////////////////////////////////////////////////////////////////////
#endif //__MSTREECTRL
