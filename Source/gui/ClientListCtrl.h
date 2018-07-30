// ClientListCtrl.h : header file
//

#ifndef __CLIENTLISTCTRL__
#define __CLIENTLISTCTRL__


#include "P4ListCtrl.h"
#include "P4Client.h"

class CClientListCtrl : public CP4ListCtrl
{
public:
	CClientListCtrl();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CClientListCtrl)

// Attributes
public:
	CString m_SpecText;

protected:
	CString m_ActiveClient;  // Name of client selected in the list
	CString m_Template;      // Name of client to be used as a template
	CString m_OldClient;	 // Active client name at the start of the operation
	CString m_ClientNew;	 // Name of freshly created client
	CString m_saveclientnew; // temp copy of name of freshly created client
	BOOL	m_isNew;		 // Client name is not in client list
	BOOL	m_NewClicked;	 // User started by clicking the New menuitem
	BOOL	m_NewClient;	 // User started by clicking the New menuitem and entered a name that is not the current client name
	int		m_PrevNbrCli;	 // Nbr clients in listctrl before creating this one

	inline CP4winApp *TheApp() { return (CP4winApp *) AfxGetApp(); }
	CP4Client *m_pNewSpec;
	void OnEditSpec( LPCTSTR clientName, BOOL bUseDefTemplate=FALSE );
	void OnEditTemplate( LPCTSTR templateName );
    BOOL SetP4Client( BOOL bAlways );
	BOOL OKToDelete( );

	//////////////////////
	// OLE drag drop support

	// Internal clipboard format
	UINT m_CF_DEPOT;
	UINT m_CF_CLIENT;

	// Drag info stored in OnDragEnter
	UINT m_DragDataFormat;   // clipboard data format (also indicates source)
	DROPEFFECT m_DropEffect;

	// List of selected Depot files
	CStringList m_AddToViewList;

	// previous current & default clients
	CString m_OldDefClient;
	CString m_OldCurClient;

	// Operations
public:
	void ClientspecNew( );
	void Clear();
	void EditTheSpec(CString *name);
	void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	void OnClientEditmy();
	BOOL ClientSpecSwitch(CString switchTo, BOOL bAlways = FALSE, BOOL portWasChanged = FALSE); 
	void OnNewClient(WPARAM wParam, LPARAM lParam);

// Overrides

// Implementation
public:
	virtual int OnCompareItems(LPARAM lParam1, LPARAM lParam2, int subItem);
	BOOL AutoCreateClientSpec( LPCTSTR clientName, LPCTSTR clientRoot, BOOL bEdit, BOOL bTmpl, LPCTSTR tmplate );
	void DoClientspecNew(BOOL bUseDefTemplate, LPCTSTR defName);
protected:
	BOOL TryDragDrop( );
	DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point); 
	DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point); 
	BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point); 
	void InsertClient(CP4Client *client, int index, CString *curcli, CString *defcli, CString *user=0);
	void UpdateClient(CP4Client *client, int index);
	void UpdateClientAll(CP4Client *client, int index);
	BOOL SyncAfter(int key, int syncAfter);
	void ViewUpdate() { OnViewUpdate(); }
	CString SetCaption();

public:
	virtual ~CClientListCtrl();
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
public:
	afx_msg void OnViewUpdate();
	afx_msg void OnFilterClients();
protected:
	afx_msg void OnClientDelete();
	afx_msg void OnUpdateClientDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewUpdate(CCmdUI* pCmdUI);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnClientTemplate();
	afx_msg void OnUpdateClientTemplate(CCmdUI* pCmdUI);
	afx_msg void OnUseAsLocalTemplate();
	afx_msg void OnUpdateUseAsLocalTemplate(CCmdUI* pCmdUI);
	afx_msg void OnClearLocalTemplate();
	afx_msg void OnUpdateClearLocalTemplate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateClientDescribe(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnClientspecSwitch();
	afx_msg void OnUpdateClientspecSwitch(CCmdUI* pCmdUI);
	afx_msg void OnSetDefClient();
	afx_msg void OnUpdateSetDefClient(CCmdUI* pCmdUI);
	afx_msg void OnClientspecNew();
	afx_msg void OnUpdateClientspecNew(CCmdUI* pCmdUI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateAddToClientView(CCmdUI* pCmdUI);
	afx_msg void OnAddToClientView();
	afx_msg void OnPerforceOptions();
	afx_msg void OnUpdateDiff2Clients(CCmdUI* pCmdUI);
	afx_msg void OnDiff2Clients();
	afx_msg void OnUpdateFilterClients(CCmdUI* pCmdUI);
	afx_msg void OnUpdateClearClientFilter(CCmdUI* pCmdUI);
	afx_msg void OnClearClientFilter();
	LRESULT OnP4ClientList(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4ClientSpec(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4EndSpecEdit(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Sync(WPARAM wParam, LPARAM lParam);
	LRESULT NewUser(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4UseAsLocalTemplate(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
};

#endif //__CLIENTLISTCTRL__
/////////////////////////////////////////////////////////////////////////////
