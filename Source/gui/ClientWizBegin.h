#pragma once


// CClientWizBegin dialog

class CClientWizBegin : public CPropertyPage
{
	DECLARE_DYNAMIC(CClientWizBegin)

public:
	CClientWizBegin();
	virtual ~CClientWizBegin();

	void SetMsg(CString& txt) { m_Message = txt; }
	void SetAllowBrowse(BOOL b) { m_AllowBrowse = b; }
	CBitmap m_Bitmap;

// Dialog Data
	//{{AFX_DATA(CClientWizBegin)
	enum { IDD = IDD_CLIENTWIZBEGIN };
	CString m_Message;
	int m_Radio;
	//}}AFX_DATA

protected:
	BOOL m_AllowBrowse;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CClientWizBegin)
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	afx_msg void OnViewGettingStarted();
	afx_msg void OnGetAdobe();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
