// JobListDlg.h : header file
//

#include "WinPos.h"
#include "P4ListCtrl.h"	// for MAX_SORT_COLUMNS

/////////////////////////////////////////////////////////////////////////////
// CJobListDlg dialog

class CJobListDlg : public CDialog
{
// Construction
public:
	CJobListDlg(CWnd* pParent = NULL);   // standard constructor
	
// Dialog Data
	//{{AFX_DATA(CJobListDlg)
	enum { IDD = IDD_JOBLIST };
	CComboBox	m_JobStatus;
	CListCtrl	m_JobListCtrl;
	CEdit	m_JobDesc;
	//}}AFX_DATA

	CString m_JobStatusValue;

	void SetJobList(CObList *joblist);
	void SetJobSpec(CString *jobSpec);
	void SetJobCols(CStringArray *jobCols);
	void SetJobCurr(CString *jobName);
	void SetJobFont(CFont* font) { m_Font = font; }
	CStringList *GetSelectedJobs() { return &m_SelectedJobs; }
	BOOL IsSortAscending() { return m_SortAscending; }
	void SetSortAscending(BOOL b) { m_SortAscending = b; }
	int NextSortColumn(int lastcol);

protected:
	CWinPos m_WinPos;
	CFont*  m_Font;
	CString m_sFilter;

	// List of all jobs (in) and selected jobs (out)
	CObList *m_pJobList;
	CString *m_pJobSpec;
	CString *m_CurrJob;
	CStringArray *m_ColNames;
	CStringList   m_SelectedJobs;

	// Sort settings
	BOOL m_SortAscending;
	int m_LastSortColumn;
	int m_SortColumns[MAX_SORT_COLUMNS];

	// Resize support
	CRect m_InitRect;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJobListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	inline CP4winApp *TheApp() { return (CP4winApp *) AfxGetApp(); }
	void RestoreSavedWidths(int *width, int numcols);
	BOOL LoadJobStatusComboBox();
	CString PersistentJobFilter(REGSAM accessmask);

	// Generated message map functions
	//{{AFX_MSG(CJobListDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnItemchangedJoblist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickJoblist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclickJoblist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnJobFilter();
	afx_msg void OnClearFilter();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
