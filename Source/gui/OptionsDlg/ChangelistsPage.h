//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// ChangelistsPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChangelistsPage dialog

class CChangelistsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CChangelistsPage)

// Construction
public:
	CChangelistsPage();
	~CChangelistsPage();

// Dialog Data
	//{{AFX_DATA(CChangelistsPage)
	enum { IDD = IDD_PAGE_CHANGELISTS };
	BOOL	m_WarnAlreadyOpened;
	BOOL	m_SubmitOnlyChged;
	BOOL	m_RevertUnchged;
	BOOL	m_AutoReopen;
	CString	m_WarnLimit;
	CString	m_WarnLimitOpen;
	CString	m_WarnLimitDiff;
	CButton	m_ExpandChgLists;
	CButton	m_ReExpandChgs;
	CButton	m_ShowClientPath4Chgs;
	CButton	m_ShowOpenActions;
	CButton	m_ShowChangeDescs;
	CButton m_EnablePendingChgsOtherClients;
	CButton	m_ChangeCountRadio;
	int		m_OldChangeCount;
	int		m_UseLongChglistDesc;
	//}}AFX_DATA

protected:
	int m_ErrorCount;
	BOOL m_Inited;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CChangelistsPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void OnFetchRadio(); 
	void OnShowChgDesc();
	void OnDeselUnchanged();

	// Generated message map functions
	//{{AFX_MSG(CChangelistsPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
};
