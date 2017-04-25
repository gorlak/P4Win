// P4ListBrowse.h : header file
//

#include "WinPos.h"
#include "P4ListCtrl.h"	// for MAX_SORT_COLUMNS & MAX_P4OBJECTS_COLUMNS

/////////////////////////////////////////////////////////////////////////////
// CP4ListBrowse dialog

class CP4ListBrowse : public CDialog
{
// Construction
public:
	CP4ListBrowse(CWnd* pParent, BOOL bWiz=FALSE, BOOL bBranchInteg=FALSE);
	
// Dialog Data
	//{{AFX_DATA(CP4ListBrowse)
	enum { IDD = IDD_LISTBROWSE };
	CListCtrl	m_P4ListCtrl;
	//}}AFX_DATA

	void SetP4ObjectList(CObList *P4list);
	void SetP4ObjectCols(CStringArray *P4Cols);
	void SetP4ObjectCurr(CString *P4Name);
	void SetP4ObjectFont(CFont* font) { m_Font = font; }
	void SetP4ObjectSKey(CString *subkey) { m_SubKey = *subkey; }
	void SetP4ObjectCaption(CString *caption) { m_caption = *caption; }
	void SetP4ObjectImage(int iImage) { m_iImage = iImage; }
	void SetP4ObjectType(int viewType) { m_viewType = viewType; }
	void SetP4ObjectIsFiltered(BOOL b) { m_IsFiltered = b; }
	CString *GetSelectedP4Object() { return &m_SelectedP4Object; }
	BOOL IsBranchInteg() { return m_BranchInteg; }
	BOOL IsSortAscending() { return m_SortAscending; }
	void SetSortAscending(BOOL b) { m_SortAscending = b; }
	int NextSortColumn(int lastcol);

protected:
	CWnd* m_pParent;
	BOOL m_Wiz;
	BOOL m_BranchInteg;
	BOOL m_FilterByHost;
	CWinPos m_WinPos;
	CFont*  m_Font;
	CString m_caption;
	CString m_Hostname;
	int m_iImage;
	int m_viewType;
	BOOL m_IsFiltered;

	// List of all P4Objects (in) and selected P4Objects (out)
	CObList *m_pP4List;
	CString *m_CurrP4Object;
	CStringArray *m_ColNames;
	CString  m_SelectedP4Object;

	// Column and Sort settings
	CString m_SubKey;
	BOOL m_SortAscending;
	int m_LastSortColumn;
	int m_SortColumns[MAX_SORT_COLUMNS];

	// Resize support
	CRect m_InitRect;
	CRect m_LastRect;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CP4ListBrowse)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	inline CP4winApp *TheApp() { return (CP4winApp *) AfxGetApp(); }
	void RestoreSavedWidths(int *width, int numcols);
	void AddTheListData();

	// Generated message map functions
	//{{AFX_MSG(CP4ListBrowse)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	virtual void OnCancel();
	afx_msg void OnColumnclickP4list(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclickP4list(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRefresh();
	afx_msg void OnBack();
	afx_msg void OnDescribe();
//	afx_msg void OnFilter();
//	afx_msg void OnHelp();
//	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
