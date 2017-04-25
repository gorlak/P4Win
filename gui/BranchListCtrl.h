// BranchListCtrl.h : header file
//

#ifndef __BRANCHLISTCTRL__
#define __BRANCHLISTCTRL__

#include "P4Branch.h"
#include "P4ListCtrl.h"

class CBranchListCtrl : public CP4ListCtrl
{
public:
	CBranchListCtrl();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CBranchListCtrl)

// Attributes
protected:
	CObArray m_BranchesList;
	CWnd *m_pNotifyWnd;
	CString m_SpecText;

	CString m_ActiveBranch;  // Name of branch being edited or deleted
	BOOL m_NewBranch;        // Does an edit operation refer to a new branch
	BOOL m_Need2DoNew;		 // New Menuitem click still needs to be handled
	CP4Branch *m_pNewSpec;
	inline CP4winApp *TheApp() { return (CP4winApp *) AfxGetApp(); }
	void OnEditSpec( LPCTSTR sItem );
	virtual BOOL OKToDelete( ) { return TRUE; }
	CString SetCaption();

// Operations
public:
	void Clear();
	void EditTheSpec(CString *name);
	void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);

// Overrides

// Implementation
public:
	virtual ~CBranchListCtrl();
	virtual int OnCompareItems(LPARAM lParam1, LPARAM lParam2, int subItem);
protected:
	CString GetSelectedBranch();
	LRESULT OnRequestBranchesList(WPARAM wParam, LPARAM lParam);
	void FillBranchesList();
	void NotifyNewBranchesList();
	int FindBranch(LPCTSTR branchName);
	void InsertBranch(CP4Branch *branch, int index);
	void UpdateBranch(CP4Branch *branch, int index);
	void UpdateBranchAll(CP4Branch *branch, int index);
	void ViewUpdate() { OnViewUpdate(); }


#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
public:
	afx_msg void OnViewUpdate();
protected:
	afx_msg void OnBranchDelete();
	afx_msg void OnUpdateBranchDelete(CCmdUI* pCmdUI);
	afx_msg void OnBranchEditspec();
	afx_msg void OnUpdateBranchEditspec(CCmdUI* pCmdUI);
	afx_msg void OnBranchNew();
	afx_msg void OnUpdateBranchNew(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewUpdate(CCmdUI* pCmdUI);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnBranchInteg();
	afx_msg void OnUpdateBranchInteg(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBranchDescribe(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateFilterByOwner(CCmdUI* pCmdUI);
	afx_msg void OnUpdateClearFilterByOwner(CCmdUI* pCmdUI);
	afx_msg void OnFilterByOwner();
	afx_msg void OnClearFilterByOwner();
	LRESULT OnP4BranchList(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4BranchSpec(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4EndSpecEdit(WPARAM wParam, LPARAM lParam);
	LRESULT NewUser(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};

#endif //__BRANCHLISTCTRL__
/////////////////////////////////////////////////////////////////////////////
