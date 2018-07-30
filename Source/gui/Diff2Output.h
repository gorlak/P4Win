// Diff2Output.h : header file
//

#ifndef __DIFF2OUTPUT_DIALOG_HDR
#define __DIFF2OUTPUT_DIALOG_HDR

#include "WinPos.h"

/////////////////////////////////////////////////////////////////////////////
// CDiff2Output dialog

class CDiff2Output : public CDialog
{
// Construction
public:
	CDiff2Output(CWnd* pParent);
	~CDiff2Output();
	
// Dialog Data
	//{{AFX_DATA(CDiff2Output)
	enum { IDD = IDD_DIFF2OUTPUT };
	CListCtrl	m_ListCtrl;
	//}}AFX_DATA

	void SetFont(CFont* font) { m_Font = font; }
	void SetNames(CStringArray * names) { m_ColNames = names; }
	void SetCaption(CString caption) { m_caption = caption; }
	void SetMsg(CString msg) { m_Msg = msg; }
	void SetKey(int key) { m_Key = key; }

protected:
	CWnd* m_pParent;
	CWinPos m_WinPos;
	CFont*  m_Font;
	CString m_caption;
	CString m_Msg;
	int m_Key;
	int m_SubItem;

	CString m_ItemStr;
	CStringList m_StringList;
	CString m_Viewer;
	BOOL m_ViewFileIsText;

	// Folders being compare - with and without trailing "..."
	CStringArray *m_ColNames;
	CString m_Hdr1;
	CString m_Hdr2;
	CString m_OrigHdr1;
	CString m_OrigHdr2;

	// Resize support
	CRect m_InitRect;
	CRect m_LastRect;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiff2Output)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	inline CP4winApp *TheApp() { return (CP4winApp *) AfxGetApp(); }
	void RestoreSavedWidths(int *width, int numcols);
	void SaveColumnWidths();
	void AddTheListData(int lgthHdr1, int lgthHdr2);
	CString ParseFileInfo(CString *itemStr, int *rev=NULL, CString *filetype=NULL);
	BOOL PumpMessages( );

	// Generated message map functions
	//{{AFX_MSG(CDiff2Output)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	virtual void OnCancel();
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDblclickP4list(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedP4List(NMHDR* pNMHDR, LRESULT* pResult);
//	afx_msg void OnHelp();
//	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDiff2();
	afx_msg void OnEditCopy();
	afx_msg void OnPositionDepot();
	afx_msg void OnFileAutobrowse();
	afx_msg void OnFileInformation();
	afx_msg void OnFileRevisionTree();
	afx_msg void OnFileAnnotate();
	afx_msg void OnFileRevisionhistory();
	afx_msg void OnSinglePaneView();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	LRESULT OnP4Diff2(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4ViewFile(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4FileInformation(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4EndFileInformation(WPARAM wParam, LPARAM lParam);
};
#endif	// __DIFF2OUTPUT_DIALOG_HDR
