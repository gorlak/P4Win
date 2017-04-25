#if !defined(AFX_DOUBLECLICKPAGE_H__07B46521_7AE5_11D5_8F35_009027AF6042__INCLUDED_)
#define AFX_DOUBLECLICKPAGE_H__07B46521_7AE5_11D5_8F35_009027AF6042__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DoubleClickPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDoubleClickPage dialog

class CDoubleClickPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CDoubleClickPage)
// Construction
public:
public:
	CDoubleClickPage();
	~CDoubleClickPage();

// Dialog Data
	//{{AFX_DATA(CDoubleClickPage)
	enum { IDD = IDD_PAGE_DBLCLICK };
	int		m_DblClick;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDoubleClickPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_Inited;

	// Generated message map functions
	//{{AFX_MSG(CDoubleClickPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOUBLECLICKPAGE_H__07B46521_7AE5_11D5_8F35_009027AF6042__INCLUDED_)
