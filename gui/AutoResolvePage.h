#pragma once


// CAutoResolvePage dialog

class CAutoResolvePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CAutoResolvePage)

public:
	CAutoResolvePage();
	virtual ~CAutoResolvePage();

// Dialog Data
	enum { IDD = IDD_PAGE_AUTORESOLVE };
	int		m_ResolveAutoDefault;
	int		m_Resolve2wayDefault;
	int		m_Resolve3wayDefault;

// Implementation
protected:
	BOOL m_Inited;
	BOOL m_Closing;

// Overrides
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CAutoResolveDlg)
public:
	virtual void OnOK();
protected:
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
