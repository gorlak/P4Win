#if !defined(AFX_CHGDESCRIBE_H__59977B63_BC93_11D3_830E_009027AF6042__INCLUDED_)
#define AFX_CHGDESCRIBE_H__59977B63_BC93_11D3_830E_009027AF6042__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChgDescribe.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChgDescribe dialog

class CChgDescribe : public CDialog
{
// Construction
public:
	CChgDescribe(CWnd* pParent = NULL);   // standard constructor
	CString GetNbr();
	void SetNbr(CString &nbr) { m_Nbr = nbr; }

// Dialog Data
	//{{AFX_DATA(CChgDescribe)
	enum { IDD = IDD_CHANGE_DESCRIBE };
	CString	m_Nbr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChgDescribe)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChgDescribe)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHGDESCRIBE_H__59977B63_BC93_11D3_830E_009027AF6042__INCLUDED_)
