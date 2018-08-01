// StatusView.h : header file
//

#ifndef __STATUSVIEW__
#define __STATUSVIEW__


/////////////////////////////////////////////////////////////////////////////
// CStatusView view

// note: this enum must agree with the status icon values in both
// number and ordering.  That is, the first status icon in the imagelist
// should correspond to the first member of this enum, and so on.

enum StatusView
{
	SV_MSG,
	SV_COMPLETION,
	SV_WARNING,
	SV_ERROR,
	SV_DEBUG,
	SV_WARNSUMMARY,
	SV_BLANK,
	SV_TOOL,
	SV_MAX
};

class CP4winApp;

class CStatusView : public CListView
{
public:
	CStatusView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CStatusView)

// Attributes
protected:
	int m_MaxStatusLines;
	int m_MaxSaveLines;
	int m_HeadIndex;
	BOOL m_ErrFound;
	BOOL m_RowAdded;
	BOOL m_ShowStatusMsgs;
	HANDLE m_hErrFile;
	CString m_ErrFile;
	CStringList m_StatusRows;
	CString m_ToolTipText;
	
// Operations
protected:
	inline CP4winApp *TheApp() { return (CP4winApp *) AfxGetApp(); }
public:
	void Clear();
	void AddItem(LPCTSTR text, StatusView level, bool showDialog, BOOL ensureVisible=TRUE);
	void CallOnUpdateViewWarnAndErrs(CCmdUI* pCmdUI);
	void CallOnViewWarnAndErrs();
	void OnMaxStatusLines();
	void SetShowStatusMsgs(BOOL b) { m_ShowStatusMsgs = b; }

protected:
	void AddOneRow(LPCTSTR text, StatusView level, BOOL ensureVisible, BOOL write2file, BOOL bSave=TRUE);
	int GetSelectedCount();
	void GetPaneText( CString &txt, BOOL onlySelectedText); 
	CString Extract1stFilename(CString &str);
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatusView)
	public:
	virtual void OnInitialUpdate();
	INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	virtual ~CStatusView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CStatusView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnWindowClear();
	afx_msg void OnShowCommandTrace();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnUpdateViewWarnAndErrs(CCmdUI* pCmdUI);
	afx_msg void OnViewWarnAndErrs();
	afx_msg void OnUpdateWindowCopyselected(CCmdUI* pCmdUI);
	afx_msg void OnWindowCopyselected();
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnEditSelectAll();
	afx_msg void OnShowTimestamp();
	afx_msg void OnShowStatusMsgs();
	afx_msg void OnUpdateFindInDepot(CCmdUI* pCmdUI);
	afx_msg void OnFindInDepot();
	afx_msg void OnUpdateFindInChgs(CCmdUI* pCmdUI);
	afx_msg void OnFindInChgs();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysColorChange();
	afx_msg void OnPerforceOptions();
	afx_msg BOOL OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult );
	//}}AFX_MSG
	LRESULT OnFindPattern(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

#endif //__STATUSVIEW__
/////////////////////////////////////////////////////////////////////////////
