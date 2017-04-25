// LabelListCtrl.h : header file
//

#ifndef __LABELLISTCTRL__
#define __LABELLISTCTRL__

#include "P4ListCtrl.h"
#include "P4Label.h"
#include "ReviewList.h"
#include "LabelAddReplace.h"
#include "LabelDelSync.h"

class CLabelListCtrl : public CP4ListCtrl
{
public:
	CLabelListCtrl();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CLabelListCtrl)

// Attributes
protected:
	CString m_SpecText;
	CStringList m_StringList;

	CString m_ActiveLabel;  // Name of label being edited or deleted
	CString m_LabelFiles;
	int m_LabelFileCount;
	BOOL m_LabelFilesInDialog;
	BOOL m_NewLabel;        // Does an edit operation refer to a new label
	BOOL m_Need2DoNew;		// New Menuitem click still needs to be handled
	CP4Label *m_pNewSpec;
	inline CP4winApp *TheApp() { return (CP4winApp *) AfxGetApp(); }
	virtual BOOL OKToDelete( ) { return TRUE; }

	CStringList m_LabelFilterView;
	CStringList m_StrList;

	CString m_User;
	CString m_FilterOwner;
	int m_FilterOwnerFlag;
	BOOL m_FilterIncBlank;
	BOOL m_AnyBlankOwner;

	// List of selected Depot files
	CStringList m_AddToViewList;

	// Internal clipboard format
	UINT m_CF_DEPOT;

	// Drag info stored in OnDragEnter
	UINT m_DragDataFormat;   // clipboard data format (also indicates source)
	DROPEFFECT m_DropEffect;

	// Labelsync dialog
	CLabelAddReplace *m_AddReplaceDlg;
	CStringList m_AddReplaceList;
	CString m_AddReplaceName;

	// Label Delete/sync dialog
	CLabelDelSync *m_DelSyncDlg;
	CStringList m_DelSyncList;
	CString m_DelSyncName;

// Operations
public:
	void Clear();
	void EditTheSpec(CString *name);
	void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	void ClearLabelFilter();
	void OnUpdateClearFilterLabels(CCmdUI* pCmdUI);

// Overrides

// Implementation
public:
	virtual int OnCompareItems(LPARAM lParam1, LPARAM lParam2, int subItem);
protected:
	DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point); 
	DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point); 
	BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	void OnDragLeave( );
	CString GetSelectedLabel();
	int FindLabel(LPCTSTR labelName);
	void InsertLabel(CP4Label *label, int index);
	void UpdateLabel(CP4Label *label, int index);
	void ViewUpdate() { OnViewUpdate(); }

public:
	virtual ~CLabelListCtrl();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
public:
	afx_msg void OnViewUpdate();
protected:
	afx_msg void OnLabelDelete();
	afx_msg void OnUpdateLabelDelete(CCmdUI* pCmdUI);
	afx_msg void OnLabelEditspec();
	afx_msg void OnUpdateLabelEditspec(CCmdUI* pCmdUI);
	afx_msg void OnLabelNew();
	afx_msg void OnUpdateLabelNew(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewUpdate(CCmdUI* pCmdUI);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnUpdateLabelSync(CCmdUI* pCmdUI);
	afx_msg void OnLabelSync();
	afx_msg void OnUpdateLabelSyncClient(CCmdUI* pCmdUI);
	afx_msg void OnLabelSyncClient();
	afx_msg void OnUpdateAddToLabelView(CCmdUI* pCmdUI);
	afx_msg void OnAddToLabelView();
	afx_msg void OnUpdateLabelDescribe(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLabelListfiles();
	afx_msg void OnUpdateLabelListfiles(CCmdUI* pCmdUI);
	afx_msg void OnLabelDeletefiles();
	afx_msg void OnUpdateLabelDeletefiles(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLabelEditSpec(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLabelTemplate(CCmdUI* pCmdUI);
	afx_msg void OnLabelTemplate();
	afx_msg void OnLabelFilterClear();
	afx_msg void OnLabelFilterSetview();
	afx_msg void OnLabelFilterSetviewRev();
	afx_msg void OnUpdateLabelFilterClearview(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLabelFilterSetview(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLabelFilterSetviewRev(CCmdUI* pCmdUI);
	afx_msg void OnLabelFilterClearview();
	afx_msg void OnViewReloadall();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLabelDDsetSync();
	afx_msg void OnLabelDDsetDeleteFiles();
	afx_msg void OnLabelDDsetSyncClient();
	afx_msg void OnLabelDDAddToLabelView();
	afx_msg void OnLabelDDsetFilterSetview();
	afx_msg void OnLabelDDsetFilterSetviewRev();
	afx_msg void OnLabelDDsetShowMenu();
	afx_msg void OnUpdateLabelDDsetShowMenu(CCmdUI* pCmdUI);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnPerforceOptions();
	afx_msg void OnUpdateFilterByOwner(CCmdUI* pCmdUI);
	afx_msg void OnUpdateClearFilterByOwner(CCmdUI* pCmdUI);
	afx_msg void OnFilterByOwner();
	afx_msg void OnClearFilterByOwner();
	afx_msg void OnUpdateDiff2Labels(CCmdUI* pCmdUI);
	afx_msg void OnDiff2Labels();
	LRESULT OnP4LabelList(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4LabelContents(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4LabelSpec(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4LabelSync(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4EndSpecEdit(WPARAM wParam, LPARAM lParam);
	LRESULT OnLabelDropMenu(WPARAM wParam, LPARAM lParam);
	LRESULT OnDoLabelSync(WPARAM wParam, LPARAM lParam);
	LRESULT OnDoLabelDelFiles(WPARAM wParam, LPARAM lParam);
	LRESULT OnDoLabelSyncCli(WPARAM wParam, LPARAM lParam);
	LRESULT CallOnUpdateLabelFilterClearview(WPARAM wParam, LPARAM lParam);
	LRESULT NewUser(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
private:
	void EditSpec( const CString &);
public:
};

#endif //__LABELLISTCTRL__
/////////////////////////////////////////////////////////////////////////////
