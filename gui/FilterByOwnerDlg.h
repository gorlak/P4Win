#pragma once


// CFilterByOwnerDlg dialog

class CFilterByOwnerDlg : public CDialog
{
	DECLARE_DYNAMIC(CFilterByOwnerDlg)

public:
	CFilterByOwnerDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFilterByOwnerDlg();

// Dialog Data
	//{{AFX_DATA(CFilterByOwnerDlg)
	enum { IDD = IDD_FILTERBYOWNER };
	int     m_NotUser;
	CString m_Owner;
	BOOL m_IncBlank;
	//}}AFX_DATA

	BOOL m_bShowIncBlanks;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilterByOwnerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFilterByOwnerDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowseUsers();
	afx_msg void OnOwner();
	//}}AFX_MSG
	LRESULT OnBrowseUsersCallBack(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};
