#if !defined(AFX_DIFF2DLG_H__D27203CD_1C9C_4CAE_B451_03F46CB17371__INCLUDED_)
#define AFX_DIFF2DLG_H__D27203CD_1C9C_4CAE_B451_03F46CB17371__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Diff2Dlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDiff2Dlg dialog

class CDiff2Dlg : public CDialog
{
// Construction
public:
	CDiff2Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDiff2Dlg)
	enum { IDD = IDD_DIFF2 };
	CString	m_Edit1;
	CString	m_Edit2;
	CString	m_RevNbr1;
	CString	m_RevNbr2;
	CString	m_Symbol1;
	CString	m_Symbol2;
	int		m_RevRadio1;
	int		m_RevRadio2;
	int		m_DefaultRadio1;
	int		m_DefaultRadio2;
	CComboBox m_TypeCombo1;
	CComboBox m_TypeCombo2;
	//}}AFX_DATA

	CRect m_InitRect;
	int m_LastWidth;

	CString m_HaveRev1;
	CString m_HaveRev2;
	int m_HeadRev1;
	int m_HeadRev2;
	BOOL m_IsFolders;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiff2Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDiff2Dlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnUseHeadRev1();
	afx_msg void OnFIsrevnbr1();
	afx_msg void OnFIssymbol1();
	afx_msg void OnUseClientFile1();
	afx_msg void On1ComboValueChg();
	afx_msg void On1Browse();
	afx_msg void OnUseHeadRev2();
	afx_msg void OnFIsrevnbr2();
	afx_msg void OnFIssymbol2();
	afx_msg void OnUseClientFile2();
	afx_msg void On2ComboValueChg();
	afx_msg void On2Browse();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	LRESULT On1BrowseCallBack(WPARAM wParam, LPARAM lParam);
	LRESULT On2BrowseCallBack(WPARAM wParam, LPARAM lParam);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIFF2DLG_H__D27203CD_1C9C_4CAE_B451_03F46CB17371__INCLUDED_)
