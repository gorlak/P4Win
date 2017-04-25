// UserListCtrl.h : header file
//

#ifndef __USERLISTCTRL__
#define __USERLISTCTRL__


#include "P4ListCtrl.h"
#include "P4User.h"

class CUserListCtrl : public CP4ListCtrl
{
	friend class CUserView;

public:
	CUserListCtrl();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CUserListCtrl)

// Attributes
public:
	CString m_SpecText;

protected:
	BOOL SetP4User( );
	CString m_olduser;
	CString m_oldJobView;
	void EditSpec( );
	CP4User *m_pNewSpec;
	void OnEditSpec( LPCTSTR sItem, BOOL bNew=FALSE );
	BOOL OKToDelete( );

	//////////////////////
	// OLE drag drop support

	// Internal clipboard format
	CLIPFORMAT m_CF_DEPOT;
	CLIPFORMAT m_CF_USER;

	// Drag info stored in OnDragEnter
	UINT m_DragDataFormat;   // clipboard data format (also indicates source)
	DROPEFFECT m_DropEffect;

	// List of selected Depot files
	CStringList m_ReviewsList;

	// previous current & default users
	CString m_OldDefUser;
	CString m_OldCurUser;

// Operations
public:
	void Clear();
	void EditTheSpec(CString *name);
	void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	void OnUserEditmy();
	void OnUpdateUserPassword(CCmdUI* pCmdUI, LPCTSTR userName);
	void OnNewUser(WPARAM wParam, LPARAM lParam);

// Overrides

// Implementation
public:
	virtual int OnCompareItems(LPARAM lParam1, LPARAM lParam2, int subItem);
protected:
	BOOL TryDragDrop( );
	DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point); 
	DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point); 
	BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point); 
	void InsertUser(CP4User *user, int index);
	void UpdateUser(CP4User *user, int index);
	void ViewUpdate() { OnViewUpdate(); }

public:
	virtual ~CUserListCtrl();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
public:
	afx_msg void OnViewUpdate();
protected:
	afx_msg void OnUserDelete();
	afx_msg void OnUpdateUserDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewUpdate(CCmdUI* pCmdUI);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnUpdateUserDescribe(CCmdUI* pCmdUI);
	afx_msg void OnUserDescribe();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnUserCreatenewuser();
	afx_msg void OnUpdateUserSwitchtouser(CCmdUI* pCmdUI);
	afx_msg void OnUserSwitchtouser();
	afx_msg void OnUpdateSetDefUser(CCmdUI* pCmdUI);
	afx_msg void OnSetDefUser();
	afx_msg void OnUpdateUserPassword(CCmdUI* pCmdUI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateAddReviews(CCmdUI* pCmdUI);
	afx_msg void OnAddReviews();
	LRESULT OnP4UserList(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4UserSpec(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4EndSpecEdit(WPARAM wParam, LPARAM lParam);
public:
	void OnUserPassword();
	void NewUser(LPCTSTR olduser, LPCTSTR newuser);
	int  OnUserPasswordDlg(int lvl, int key);

	DECLARE_MESSAGE_MAP()
};

#endif //__USERLISTCTRL__
/////////////////////////////////////////////////////////////////////////////
