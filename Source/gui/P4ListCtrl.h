// P4ListCtrl.h : header file
//

#ifndef __P4LISTCTRL__
#define __P4LISTCTRL__

#define LIST_CLEAR		0
#define LIST_UPDATING	1
#define LIST_UPDATED	2

#define	MAX_SORT_COLUMNS	4
#define	MAX_P4OBJECTS_COLUMNS	16

#include "SortListHeader.h"
#include "P4PaneContent.h"
#include "P4ListAll.h"


/////////////////////////////////////////////////////////////////////////////
// CP4ListCtrl

class CP4ListCtrl : public CListCtrl, public CP4PaneContent
{
protected:
	CP4ListCtrl();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CP4ListCtrl)

// Attributes
	virtual CWnd * GetWnd() { return this; }
	BOOL m_SortAscending;
	BOOL m_ActivatedBefore;
	int m_LastSelIx;
	int m_LastSortCol;
	int m_iImage;
	enum ContextMenuContext
	{
		KEYSTROKED,
		MOUSEHIT
	} m_ContextContext;
    CString m_Active;// Name of job, client, user, branch, etc. being edited, described, deleted, etc.
	CString m_Describing; // Name of job, client, user, branch, etc. being described
	CString m_ReportedByTitle; // Title of Job Owner field (103)
	BOOL m_ReadSavedWidths;
    BOOL m_ColsInited;
	int m_SortColumns[MAX_SORT_COLUMNS];
	CSortListHeader	m_headerctrl;
	P4ListAll	m_ListAll;	 // Holds info for all (unfiltered) list items

	// Drag & Drop data
	COleDataSource m_OLESource;
	CRect m_DragSourceRect;
	BOOL m_bStarting;
	int m_HdrHeight;
	int m_ItemHeight;
	int m_BottomOfPage;

	UINT m_PostViewUpdateMsg;
	WPARAM m_PostViewUpdateWParam;
	LPARAM m_PostViewUpdateLParam;

	// Data for modless edit dialogs
	BOOL m_EditInProgress;
	CWnd *m_EditInProgressWnd;

public:
	int m_UpdateState;
	CString m_DragFromItemName;
	
	LRESULT OnP4Describe( WPARAM wParam, LPARAM lParam );
	LRESULT OnP4EndDescribe( WPARAM wParam, LPARAM lParam );
	LRESULT OnP4Delete( WPARAM wParam, LPARAM lParam );
protected:
	inline CP4winApp *TheApp() { return (CP4winApp *) AfxGetApp(); }
	CString m_SubKey;
	virtual void ViewUpdate() {};
	virtual void OnDescribe( ); 
	virtual void OnDelete( int type );
	virtual BOOL OKToDelete( ) { return FALSE; }
    void SetIndexAndPoint( int &index, CPoint &point );
    int GetContextItem( const CPoint &point );
    int GetHitItem( const CPoint &point, BOOL bContextMenu = FALSE );
	BOOL OnUpdateShowMenuItem( CCmdUI* pCmdUI, UINT idString );
	
	BOOL SetToNextPrevItem(CString& name, int np, CListCtrl *plc);

	void InsertColumnHeaders( const CStringArray &colnames, int *widths );
	void AddSortColumn(int colnbr, BOOL sortAscending);

	// Virt func to attempt drag drop during left mouse button down
	virtual BOOL TryDragDrop( ) { return FALSE; }

	void CantEditRightNow(int type);

public:
	// Last line of Clear() should ALWAYS call this base fn
	virtual void Clear() { m_UpdateState= LIST_CLEAR; m_LastSelIx=-1; m_ListAll.RemoveAll(); }
	virtual void EditTheSpec(CString *name) {}

	void ReSort();
	int NextSortColumn(int lastcol);	
	int GetColNamesAndCount(CStringArray &cols);

	BOOL IsUpdating() { return (m_UpdateState == LIST_UPDATING); }
	BOOL IsClear() { return (m_UpdateState == LIST_CLEAR); }

	BOOL GetSavedColumnNames(CStringArray &colNames, LPCTSTR subkey);
	void SaveColumnNames(CString &colNames, LPCTSTR subkey);
	void DeleteColumnNames(LPCTSTR subkey);
	void RestoreSavedWidths(int *width, int numcols, LPCTSTR subkey);
	void SaveColumnWidths();
	int FindInList( const CString &Name );
	int FindInListAll( const CString &Name );
	int FindInListNoCase( const CString &Name );
	void OnEditPaste( const CString &Name );

	int GetSelectedItem();
	CString GetSelectedItemText( );
	CString GetSelectedItemOwner(int ownerColumnNumber);
	BOOL SelectedItemIsLocked(int optionsColumnNumber);

	void GetListItems(CStringArray *list);

	BOOL IsEditInProgress() { return m_EditInProgress; }

	// After update attempt, call one of the following:
	virtual void SetUpdateDone(); 
    virtual void SetUpdateFailed();
	// Need to lose the DYNCREATE rot if this is to be a pure virt
	virtual void OnContextMenu(CWnd* pWnd, CPoint point) {;}
    
// Overrides
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual int OnCompareItems(LPARAM lParam1, LPARAM lParam2, int subItem) {return 0;}
protected:

	virtual ~CP4ListCtrl();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	//		i  need this for calling the right ondescribe
	//		for on keydown. damn.
	//
	public:
	int m_viewType;

private:
	int GetDeleteType( );

	// Generated message map functions
public:
	LRESULT OnFindPattern(WPARAM wParam, LPARAM lParam);
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnViewUpdate();
    LRESULT OnSelectThis(WPARAM wParam, LPARAM lParam);
    LRESULT OnP4ObjectFetch(WPARAM wParam, LPARAM lParam);
    LRESULT OnP4ObjectList(WPARAM wParam, LPARAM lParam);
    LRESULT OnP4WizObjectFetch(WPARAM wParam, LPARAM lParam);
    LRESULT OnP4WizObjectList(WPARAM wParam, LPARAM lParam);
    LRESULT OnP4IntegObjectFetch(WPARAM wParam, LPARAM lParam);
    LRESULT OnP4IntegObjectList(WPARAM wParam, LPARAM lParam);
	LRESULT OnActivateModeless(WPARAM wParam, LPARAM lParam);
    LRESULT P4ObjectList(WPARAM wParam,LPARAM lParam,BOOL bWiz,BOOL bInteg,BOOL bFiltered=TRUE);
};


#endif // __P4LISTCTRL__
/////////////////////////////////////////////////////////////////////////////
