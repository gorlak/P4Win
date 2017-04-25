//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// OldChgListCtrl.h : header file
//

#ifndef __OLDCHGLISTCTRL__
#define __OLDCHGLISTCTRL__

class CDeleteFixes;

#include "P4ListCtrl.h"
#include "P4Change.h"
#include "SyncChange.h"

class COldChgListCtrl : public CP4ListCtrl
{
public:
	COldChgListCtrl();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(COldChgListCtrl)

// Operations
public:
	void Clear();
	void EditTheSpec(CString *name, BOOL uFlag);
	void ClearFilter();
	void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	void GetChanges(long numToFetch, int key=0);
	void OnDescribeChg();
	void FilterByUser(CString user);
	void FilterByClient(CString client);
	void SetCaption();
	void PersistentChgFilter( REGSAM accessmask );

protected:

	BOOL m_FilteredByUser;
	BOOL m_FilteredByClient;
	CString m_UserFilter;
	CString m_ClientFilter;

	CString m_JobSpec;
	CString *m_pJobSpec;
	CStringList m_JobList;
	CStringList m_DelFixesList;
	CString m_DelFixesStatus;
	CDeleteFixes *m_DeleteFixesDlg;
	int m_DelFixesChgNbr;

	// Internal clipboard format
	UINT m_CF_DEPOT;
	UINT m_CF_CLIENT;
	UINT m_CF_USER;
	UINT m_CF_JOB;

	// Drag info stored in OnDragEnter
	UINT m_DragDataFormat;   // clipboard data format (also indicates source)
	DROPEFFECT m_DropEffect;

	// Branch flag for integrates
	BOOL m_UseBranch;

private:
	void SetFilterState( ) ;

protected:
	DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point); 
	DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point); 
	BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point); 
	void OnDescribeLong(long changeNumber = 0, int flag = 0);
	void DoFilterSetview();
	void AddJobFixes(CStringList *jobnames, long changeNumber, LPCTSTR jobstatusvalue);

	void InsertChange(CP4Change *change, int index);
	void EditChangeSpec(long changeNumber, BOOL uFlag);
	long GetSelectedChange();
	BOOL GetUserClientForSelectedChg(CString *user);
	inline CP4winApp *TheApp() { return (CP4winApp *) AfxGetApp(); }
	long m_ItemCount;
	long m_MaxChange, m_NewMaxChange;
	long m_LastUpdateTime;
	BOOL m_ForceFocusHere;
	BOOL m_FilterSpecial;
	BOOL m_FilterInteg;
	CString m_LastDescNbr;	// Number of last changelist Described via "Describe Changelist..."
	CStringList m_FilterView;
	CStringList m_StrList;
	CSyncChange * m_SyncChangeDlg;

// Overrides

// Implementation
public:
	virtual ~COldChgListCtrl();
	virtual int OnCompareItems(LPARAM lParam1, LPARAM lParam2, int subItem);
	void ViewUpdate() { OnViewUpdate(); }
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
public:
	afx_msg void OnViewUpdate();
	afx_msg void OnFilterSetview();
protected:
	afx_msg void OnUpdateViewUpdate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSyncChgPreview(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSyncChg(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDescribe(CCmdUI* pCmdUI);
	afx_msg void OnDescribe();
	afx_msg void OnSyncChgPreview();
	afx_msg void OnSyncChg();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnViewReloadall();
	afx_msg void OnUpdateViewReloadall(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnFilterSetviewDrop();
	afx_msg void OnUpdateFilterSetview(CCmdUI* pCmdUI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateFilterClearview(CCmdUI* pCmdUI);
	afx_msg void OnFilterClearview();
	afx_msg void OnUpdateChangeEdspec(CCmdUI* pCmdUI);
	afx_msg void OnChangeEdspec();
	afx_msg void OnUpdateAddjobfix(CCmdUI* pCmdUI);
	afx_msg void OnAddjobfix();
	afx_msg void OnUpdateFileIntegrate(CCmdUI* pCmdUI);
	afx_msg void OnFileIntegrate();
	afx_msg void OnFileIntegspec();
	afx_msg void OnPerforceOptions();
	afx_msg void OnUpdatePositionDepot(CCmdUI* pCmdUI);
	afx_msg void OnPositionDepot();
	afx_msg void OnUpdateRemovefix(CCmdUI* pCmdUI);
	afx_msg void OnRemovefix();
	LRESULT OnP4Fixes(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Fix(WPARAM wParam, LPARAM lParam);
	LRESULT OnDoDeleteFixes(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdateRequest(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Change(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Describe(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4EndDescribe(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4MaxChange(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4ChangeSpec(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4EndSpecEdit(WPARAM wParam, LPARAM lParam);
    LRESULT OnP4JobList(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Get(WPARAM wParam, LPARAM lParam);
	LRESULT OnIntegChg(WPARAM wParam, LPARAM lParam);
	LRESULT OnDoSyncChg(WPARAM wParam, LPARAM lParam);
	LRESULT CallOnUpdateFilterClearview(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4ChgRollback(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4ChgRollbackPreview(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4ChgSelectAll(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4DescribeAlt(WPARAM wParam, LPARAM lParam);
	void OnIntegrate(BOOL useBranch);
	DECLARE_MESSAGE_MAP()
};

#endif //__OLDCHGLISTCTRL__
/////////////////////////////////////////////////////////////////////////////
