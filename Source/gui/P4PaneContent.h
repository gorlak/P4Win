// P4PaneContent.h : header file
//

#ifndef __P4PANECONTENT__
#define __P4PANECONTENT__

class CP4PaneView;

class CP4PaneContent
{
protected:
		// Where to post messages for changes that affect CDeltaView and COldChgView.
	HWND m_branchWnd;
	HWND m_changeWnd;
	HWND m_clientWnd;
	HWND m_depotWnd;
	HWND m_jobWnd;
	HWND m_labelWnd;
	HWND m_oldChgWnd;
	HWND m_userWnd;
	CString m_caption;
	CString m_captionplain;

public:
	CP4PaneContent();
	virtual CWnd * GetWnd() = 0;
	CP4PaneView * GetView();
	virtual LPCTSTR GetCaption() const { return m_caption; }
	virtual LPCTSTR GetPlainCaption() const { return m_captionplain; }

	void SetBranchWnd(CWnd *wnd);
	void SetChangeWnd(CWnd *wnd);
	void SetClientWnd(CWnd *wnd);
	void SetDepotWnd(CWnd *wnd);
	void SetJobWnd(CWnd *wnd);
	void SetLabelWnd(CWnd *wnd);
	void SetOldChgWnd(CWnd *wnd);
	void SetUserWnd(CWnd *wnd);

	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
};

#endif // __P4PANECONTENT__
/////////////////////////////////////////////////////////////////////////////
