//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// A simple dialog which displays a list of files to be added in a
// listbox.  The listbox is intended for DoModal() use.  Files can 
// be deleted from the list, and the target change can be updated
// The file list, change list, and selected change are set in the Init() 
// member function prior to calling DoModal()

// AddListDlg.h

#ifndef __ADDLISTDLG__
#define __ADDLISTDLG__

#include "WinPos.h"

/////////////////////////////////////////////////////////////////////////////
// CAddListDlg dialog

class CAddListDlg : public CDialog
{
// Construction
public:
	CAddListDlg(CWnd* pParent = NULL);   // standard constructor
	void Init(CStringList *strList, CStringList *changeList, 
				LPCTSTR selChange, BOOL bEditAndAdd, int key);
					
// Dialog Data
	//{{AFX_DATA(CAddListDlg)
	enum { IDD = IDD_ADDLIST };
	CComboBox	m_ChangeCombo;
	CButton	m_Delete;
	CStatic	m_StaticRect;
	CButton	m_OK;
	CButton	m_Cancel;
	int		m_Action;
	int		m_Default;
	//}}AFX_DATA

protected:	
	CStringList *m_pStrList;
	CStringList *m_pChangeList;
	CString m_SelChange;
	CStringList m_EnumeratedList;
	int m_SelectedChange;
	int m_AddFileCount;
	int m_Key;
    BOOL m_UserTerminated;
	CString m_Caption;
	BOOL m_Initialized;
	BOOL m_bEditAndAdd;
	CP4ListBox m_List;
	CWinPos m_WinPos;
	CRect m_InitRect;
	CRect m_LastRect;
	long m_WarnLimit;
	BOOL m_Need2Refresh;

    BOOL m_DeletedRows;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
    INT_PTR DoModal();
	CStringList *GetEnumeratedList() { ASSERT(m_Initialized); return &m_EnumeratedList; }
	int GetSelectedChange() { ASSERT(m_Initialized); return m_SelectedChange; }
	int GetAction() { return m_Action; }
	int GetDefault() { return m_Default; }
	BOOL GetNeed2Refresh() { return m_Need2Refresh; }

protected:
	BOOL EnumerateFiles();
	BOOL AddPath(CStringList *pList, LPCTSTR path);
    UINT VerifyOKToContinue();

	// Generated message map functions
	//{{AFX_MSG(CAddListDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDeleteselected();
	afx_msg void OnSelchangeList();
	virtual void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnFilterFiles();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //__ADDLISTDLG__
