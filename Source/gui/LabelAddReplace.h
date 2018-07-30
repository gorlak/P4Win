#if !defined(AFX_LABELADDREPLACE_H__C8CCA151_C267_11D5_8F56_009027AF6042__INCLUDED_)
#define AFX_LABELADDREPLACE_H__C8CCA151_C267_11D5_8F56_009027AF6042__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LabelAddReplace.h : header file
//

#include "WinPos.h"

/////////////////////////////////////////////////////////////////////////////
// CLabelAddReplace dialog

class CLabelAddReplace : public CDialog
{
// Construction
public:
	CLabelAddReplace(CWnd* pParent = NULL);   // standard constructor

	HWND m_LabelWnd;
	CString m_Active;
	CStringList * m_SelectedList;
	CStringList * m_OutputList;
	CWinPos m_WinPos;
	CRect m_InitRect;
	CRect m_LastRect;
	BOOL m_InitDialogDone;

// Dialog Data
	//{{AFX_DATA(CLabelAddReplace)
	enum { IDD = IDD_LABEL_ADD_REPLACE };
	CP4ListBox	m_List;
	int		m_RevFlag;
	int		m_FilesFlag;
	int		m_DefaultFlag;
	CString	m_RevNbr;
	CString	m_Symbol;
	CComboBox m_TypeCombo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLabelAddReplace)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetActive(LPCTSTR txt) { m_Active= txt; }
	void SetSelected(CStringList *plist) { m_SelectedList = plist; }
	void SetOutputList(CStringList *plist) { m_OutputList = plist; }

protected:

	// Generated message map functions
	//{{AFX_MSG(CLabelAddReplace)
	virtual void OnOK();
	afx_msg void OnPreview();
	afx_msg void OnCancel();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnUseclientrev();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnFIsrevnbr();
	afx_msg void OnFIssymbol();
	afx_msg void OnComboValueChg();
	afx_msg void OnBrowse();
	afx_msg void OnRemove();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	LRESULT OnBrowseCallBack(WPARAM wParam, LPARAM lParam);
	void OnFinish();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LABELADDREPLACE_H__C8CCA151_C267_11D5_8F56_009027AF6042__INCLUDED_)
