#if !defined(AFX_FILEUTILITIESPAGE_H__07B46521_7AE5_11D5_8F35_009027AF6042__INCLUDED_)
#define AFX_FILEUTILITIESPAGE_H__07B46521_7AE5_11D5_8F35_009027AF6042__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileUtilitiesPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileUtilitiesPage dialog

class CFileUtilitiesPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CFileUtilitiesPage)
// Construction
public:
public:
	CFileUtilitiesPage();
	~CFileUtilitiesPage();

// Dialog Data
	//{{AFX_DATA(CFileUtilitiesPage)
	enum { IDD = IDD_PAGE_FILEUTIL };
	int		m_P4AnnWhtSp;
	CButton	m_P4AnnIncInteg;
	int		m_P4TLVWhtSp;
	CButton	m_P4TLVIncInteg;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileUtilitiesPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_Inited;

	// Generated message map functions
	//{{AFX_MSG(CFileUtilitiesPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEUTILITIESPAGE_H__07B46521_7AE5_11D5_8F35_009027AF6042__INCLUDED_)
