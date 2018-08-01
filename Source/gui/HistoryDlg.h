//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// HistoryDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// HistoryDlg dialog

#include "p4lists.h"
#include "sortlistheader.h"
#include "WinPos.h"
#include "cmd_history.h"


/////////////////////////////////////////////////////////////////////////////
// CHistList window

class CHistList : public CListCtrl
{
// Construction
public:
	CHistList();

// Attributes
protected:
	CSortListHeader m_headerctrl;
public:

// Operations
public:
	void Sort( int column, BOOL ascending );
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHistList)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHistList();
	LRESULT OnFindPattern(WPARAM wParam, LPARAM lParam);

	// Generated message map functions
protected:
	//{{AFX_MSG(CHistList)
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};




class CHistoryDlg : public CDialog
{
// Construction
public:
	CHistoryDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CHistoryDlg( );

// Dialog Data
	//{{AFX_DATA(CHistoryDlg)
	enum { IDD = IDD_HISTORY };
	CEdit	m_RevisionAction;
	CEdit	m_RevisionInfo;
	CStatic	m_RevFileType;
	CHistList	m_ListCtl;
	int 	m_CompleteHistI;
	int 	m_CompleteHistIC;
	//}}AFX_DATA

	int  m_CompleteHist;
	BOOL m_SortAscending;
	BOOL m_Rerun;
	BOOL m_More;
	BOOL m_InitiallyNotBusy;
	int  m_RevHistCount;

protected:
	CWinPos m_WinPos;
	BOOL m_IsMinimized;

	CWnd *m_pParent;
	HWND m_CallingWnd;
	CCmd_History *m_pCallingCmd;
	CImageList *m_pImageList;
	CStringList m_FileList;
	CStringList m_Recover;
	CString m_Viewer;
	CString m_ViewFilePath;
	CString m_FileType;
	BOOL m_ViewFileIsText;
	HWND m_DepotWnd;
	BOOL m_Busy;
	int m_Key;
	int m_HaveRev;
	BOOL m_MyOpenFile;
	int m_InitialRev;
	CString m_InitialName;
	CString m_LatestName;
	BOOL m_LatestIsDeleted;
	int m_LastSortColumn;
	CRect m_InitRect;
	int m_listTop;
	int m_listHeight;
	int m_lorgHeight;
	int m_dlgHeight;
	BOOL m_SizeSet;
	int m_SmlWidth;
	BOOL m_EnableShowIntegs;

	CFindReplaceDialog *m_pFRDlg;
	CString m_FindWhatStr;
	int m_FindWhatFlags;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHistoryDlg)
	public:
	virtual INT_PTR DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();   
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	CHistory *m_pHistory;
	void Init(CHistory *hist, CString &ftype, BOOL isText, HWND depotWnd, 
				int haveRev, BOOL myOpenFile, const int initialRev=-1, 
				LPCTSTR initialName=NULL, int key =0);
	BOOL OnInitDialog();
	inline CP4winApp *TheApp() { return (CP4winApp *) AfxGetApp(); }
	void EnableCtrl( CWnd *ctl, BOOL enable );
	void OnContextMenu(CPoint screen, int index);
	int GetSelectedRevision();
	CString GetSelectedFileName();
	int GetSelectedChgNbr();
	void EnableButtons();
	int GetListTop() { return m_listTop; }
	void RePaintMiddleWindows();
	void FileQuickbrowse(BOOL ckifdeleted=TRUE);
	void SetEnableShowIntegs(BOOL b) { m_EnableShowIntegs = b; }
	BOOL GetEnableShowIntegs() { return m_EnableShowIntegs; }
    void SetCallingWnd(HWND hwnd) { m_CallingWnd = hwnd; }
    HWND GetCallingWnd() { return m_CallingWnd; }
	void SetCallingCommand(CCmd_History *pCmd) { m_pCallingCmd = pCmd; }
	CCmd_History *GetCallingCommand() { return m_pCallingCmd; }

protected:
	void OnDescribeChgLong(long changeNumber, int rc = 0);

	// Generated message map functions
	//{{AFX_MSG(CHistoryDlg)
	afx_msg void OnColumnclickRevlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedRevlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateDiff2(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRecover(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGet(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDescribeChg(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePositionDepot(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCopyRows(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileRevisiontree(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileAnnotate(CCmdUI* pCmdUI);
	afx_msg void OnDiff2();
	afx_msg void OnRecover();
	afx_msg void OnGet();
	afx_msg void OnDescribeChg();
	afx_msg void OnPositionDepot();
	afx_msg void OnCopyRows();
	afx_msg void OnFileRevisionTree();
	afx_msg void OnFileTimeLapseView();
	afx_msg void OnFileAnnotate();
	afx_msg void OnFileAnnotateAll();
	afx_msg void OnFileAnnotateChg();
	afx_msg void OnFileAnnotateChgAll();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnDiffvsclient();
	virtual void OnOK();
	afx_msg void OnCompletehist();
	afx_msg void OnHelp();
	afx_msg void OnMore();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnKeydownRevisionlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPerforceOptions();
	afx_msg void OnClose();
 	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnUpdatePositionToPattern(CCmdUI* pCmdUI);
	afx_msg void OnPositionToPattern();
	afx_msg void OnUpdatePositionToPrev(CCmdUI* pCmdUI);
	afx_msg void OnPositionToPrev();
	afx_msg void OnUpdatePositionToNext(CCmdUI* pCmdUI);
	afx_msg void OnPositionToNext();
	//}}AFX_MSG
	void OnFileMRUBrowser(UINT  nID);
	void OnFileNewBrowser();
	void OnFileQuickbrowse();
	void OnFileAutobrowse();
	void FileAnnotate(BOOL bAll, BOOL bChg=FALSE);
	void SaveColumnWidths();
	void RestoreSavedWidths(int *width, int numcols);
	void SizeBottonOfHistory(int x, int y);
	CString WriteTempHistFile();

	DECLARE_MESSAGE_MAP()

	LRESULT OnP4ViewFile(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Diff(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Diff2(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Get(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Describe(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4EndDescribe(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdateHaveRev(WPARAM wParam, LPARAM lParam);
	LRESULT OnQuitting(WPARAM wParam, LPARAM lParam);
	LRESULT OnFindReplace(WPARAM wParam, LPARAM lParam);

};
