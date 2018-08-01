//
// 
// Copyright 1999 Perforce Software.  All rights reserved.
//
// This file is part of Perforce - the FAST SCM System.
//
//


#if !defined(AFX_REVRANGE_H__4E198783_7B68_11D3_A36A_00105AC64526__INCLUDED_)
#define AFX_REVRANGE_H__4E198783_7B68_11D3_A36A_00105AC64526__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RevRange.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIntegRevRange dialog

class CIntegRevRange : public CPropertyPage
{
	DECLARE_DYNCREATE(CIntegRevRange)

// Construction
public:
	CIntegRevRange();
	~CIntegRevRange();

// Dialog Data
	//{{AFX_DATA(CIntegRevRange)
	enum { IDD = IDD_PAGE_INTEGREVRNG };
	CString	m_f_revnbr;
	CString	m_f_symbol;
	CString	m_t_revnbr;
	CString	m_t_symbol;
	int		m_from_flag;
	int		m_to_flag;
	CComboBox m_f_TypeCombo;
	CComboBox m_t_TypeCombo;
	//}}AFX_DATA

	BOOL m_Initialized;
	CString	m_RevRangeFrom;
	CString	m_RevRangeTo;

public:
	LPCTSTR GetRevRangeFrom() { return LPCTSTR(m_RevRangeFrom); }
	LPCTSTR GetRevRangeTo() { return LPCTSTR(m_RevRangeTo); }
	int GetRevRangeFromFlag() { return(m_from_flag); }
	int GetRevRangeToFlag() { return(m_to_flag); }
	int GetRevRangeFromType() { return(m_f_TypeVal); }
	int GetRevRangeToType() { return(m_t_TypeVal); }

	void SetRevRangeFrom( CString RevRangeFrom ) { m_RevRangeFrom = RevRangeFrom; }
	void SetRevRangeTo( CString RevRangeTo ) { m_RevRangeTo = RevRangeTo; }
	void SetRevRangeFromFlag( int from_flag ) { m_from_flag = from_flag; }
	void SetRevRangeToFlag( int to_flag ) { m_to_flag = to_flag; }
	void SetRevRangeFromType( int type ) { m_f_TypeVal = type; }
	void SetRevRangeToType( int type ) { m_t_TypeVal = type; }

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CIntegRevRange)
	public:
	virtual INT_PTR DoModal();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_f_TypeVal;
	int m_t_TypeVal;

	// Generated message map functions
	//{{AFX_MSG(CIntegRevRange)
	virtual BOOL OnInitDialog();
	afx_msg void OnFromClick();
	afx_msg void OnFromComboValueChg();
	afx_msg void OnFromBrowse();
	afx_msg void OnToClick();
	afx_msg void OnToComboValueChg();
	afx_msg void OnToBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	LRESULT OnFromBrowseCallBack(WPARAM wParam, LPARAM lParam);
	LRESULT OnToBrowseCallBack(WPARAM wParam, LPARAM lParam);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REVRANGE_H__4E198783_7B68_11D3_A36A_00105AC64526__INCLUDED_)
