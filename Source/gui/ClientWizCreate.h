#pragma once


// CClientWizCreate dialog

class CClientWizCreate : public CPropertyPage
{
	DECLARE_DYNAMIC(CClientWizCreate)

public:
	CClientWizCreate();
	virtual ~CClientWizCreate();

	void SetName(CString& name) { m_Name = name; }
	void SetRoot(CString& root) { m_Root = root; }

// Dialog Data
	//{{AFX_DATA(CClientWizCreate)
	enum { IDD = IDD_CLIENTWIZCREATE };
	CString m_Name;
	CString m_Root;
	BOOL m_bEdit;
	int  m_bTmplate;
	CString m_Tmplate;
	//}}AFX_DATA

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CClientWizCreate)
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	afx_msg void OnBrowse();
	afx_msg void OnTmplateClick();
	afx_msg void OnBrowseTemplates();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	LRESULT OnBrowseClientsCallBack(WPARAM wParam, LPARAM lParam);
};
