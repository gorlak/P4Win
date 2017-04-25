// P4PaneView.h : header file
//

#ifndef __P4PANEVIEW__
#define __P4PANEVIEW__

//		shortcut for append flags -- they're all over the place
//		and it's hard to read.
//
const UINT stringsON = MF_ENABLED | MF_STRING ;



// A subclass of CButton to make a transparent control that displays
// a string, shortening it with ellipsis if it's too long, and
// displaying the full text in a tooltip in any case.
// This is used to put the caption text on the rebar with the
// rebar background showing through.
class CCaptionTextControl : public CButton
{
	DECLARE_DYNCREATE(CCaptionTextControl)
	virtual void DrawItem(LPDRAWITEMSTRUCT);
	int OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	bool m_textTruncated;
public:
	DECLARE_MESSAGE_MAP()
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
};

// a subclass of COleDropTarget to get autoscroll regions
// to be inset from content window rather than view window
class CP4OleDropTarget : public COleDropTarget
{
	DROPEFFECT OnDragScroll(CWnd* pWnd, DWORD dwKeyState,
		CPoint point);
};

class CP4PaneContent;

class CP4PaneView : public CView
{
	DECLARE_DYNCREATE(CP4PaneView)

public:
	CP4PaneView();
	virtual ~CP4PaneView();
	virtual void LButtonClk();
	virtual void LButtonDblClk();

// Attributes
protected:
	CP4PaneContent *m_content;
	CReBar m_reBar;				// the rebar containing the caption and toolbar
	CCaptionTextControl m_captionCtrl;	// for the caption band
	CString m_captionTxt;		// a place to store the caption tooltip
	CToolBar m_toolBar;			// for the toolbar band
	CFont m_activeFont;			// rebar font when view is active
	CFont m_inactiveFont;		// rebar font when view is inactive
	int m_barHeight;			// height of rebar window
	int m_contentTop;			// top of content window

	// OLE drag drop support
	CP4OleDropTarget m_DropTarget;

// Overrides
	virtual void OnDraw(CDC* pDC);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
							AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void OnInitialUpdate();

// Implementation
protected:
	virtual bool CreateContent() { return false; }
	virtual void SetToolBarButtons() {}

public:
	CP4PaneContent* GetContent() { return m_content; }
	virtual CWnd * GetContentWnd() { return 0; }
	void SetCaption();

	// list type views
	virtual int GetSelectedItem();
	virtual void OnEditPaste( const CString &Name );
	virtual CString GetSelectedItemText( );

	// tree type views
	HTREEITEM GetSelectedItem(int index);
	CString GetItemText(HTREEITEM curr_item);

	// Operations
public:
	void SetBranchWnd(CWnd *wnd);
	void SetChangeWnd(CWnd *wnd);
	void SetClientWnd(CWnd *wnd);
	void SetDepotWnd(CWnd *wnd);
	void SetJobWnd(CWnd *wnd);
	void SetLabelWnd(CWnd *wnd);
	void SetOldChgWnd(CWnd *wnd);
	void SetUserWnd(CWnd *wnd);

	void SwapButtonPosition();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult );
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnP4Cmd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	afx_msg virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	afx_msg virtual void OnDragLeave();
	afx_msg virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	afx_msg virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	afx_msg LRESULT OnFindPattern(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnSysColorChange();

	LRESULT OnActivateModeless(WPARAM wParam, LPARAM lParam);
};


#endif // __P4PANEVIEW__
/////////////////////////////////////////////////////////////////////////////
