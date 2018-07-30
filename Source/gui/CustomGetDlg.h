// CustomGetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCustomGetDlg dialog

#ifndef	CCUSTOMGETDLG
#define CCUSTOMGETDLG	1
class CCustomGetDlg : public CDialog
{
// Construction
public:
	CCustomGetDlg(CWnd* pParent = NULL);   // standard constructor
	~CCustomGetDlg();
	LPCTSTR GetQualifier() { return m_LabelText; }
// Dialog Data
	//{{AFX_DATA(CCustomGetDlg)
	enum { IDD = IDD_CUSTOMGET };
	CComboBox m_TypeCombo;
	CString	m_LabelText;
	CString	m_RevText;
	int		m_Radio;
	BOOL	m_Only;
	BOOL	m_Force;
	//}}AFX_DATA

	int m_NbrSel;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomGetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	HWND m_DepotWnd;
	BOOL m_IsMinimized;

	// Generated message map functions
	//{{AFX_MSG(CCustomGetDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnGetpreview();
	afx_msg void OnGet();
	afx_msg void OnRadioClick();
	afx_msg void OnComboValueChg();
	afx_msg void OnBrowse();
	virtual void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	LRESULT OnBrowseCallBack(WPARAM wParam, LPARAM lParam);
};
#endif
