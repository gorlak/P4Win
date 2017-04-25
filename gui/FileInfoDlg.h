// FileInfoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileInfoDlg dialog

class CFileInfoDlg : public CDialog
{
// Construction
public:
	CFileInfoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFileInfoDlg)
	enum { IDD = IDD_FILE_INFORMATION };
	CButton	m_Describe;
	CP4ListBox	m_OpenedList;
	CString	m_ClientPath;
	CString	m_DepotPath;
	CString	m_HaveRev;
	CString	m_HeadAction;
	CString	m_HeadChange;
	CString	m_HeadRev;
	CString	m_HeadType;
	CString	m_LockedBy;
	CString	m_ModTime;
	CString m_FileSizeFld;
	//}}AFX_DATA

	CStringList m_StrList;
	CString m_Chg;
	int m_Key;
	unsigned long m_FileSize;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWnd* m_pParent;
	CBrush m_Brush;
	BOOL m_IsMinimized;
	CString m_Caption;
	CString m_MinCaption;

	// Generated message map functions
	//{{AFX_MSG(CFileInfoDlg)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDescribeChg();
	afx_msg void OnSelchangeOpenedlist();
	afx_msg void OnDblclkOpenedlist();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	LRESULT OnP4Describe(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4EndDescribe( WPARAM wParam, LPARAM lParam );
};
