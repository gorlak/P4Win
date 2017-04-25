//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// A simple dialog which displays a list of files to be reverted
// The listbox is intended for DoModal() use. The user can accept 
// the list or hit the Cancel button.  The list of file is set in
// the Init() member

// RevertListDlg.h

#ifndef __REVERTLISTDLG__
#define __REVERTLISTDLG__

#include "WinPos.h"

/////////////////////////////////////////////////////////////////////////////
// CRevertListDlg dialog

class CRevertListDlg : public CDialog
{
// Construction
public:
	CRevertListDlg(CWnd* pParent = NULL);   // standard constructor
	void Init(CStringList *strList);
	BOOL AnyRowsDeleted() { return m_DeletedRows; }

// Dialog Data
	//{{AFX_DATA(CRevertListDlg)
	enum { IDD = IDD_REVERTLIST };
	CButton	m_Delete;
	CStatic	m_StaticRect;
	CButton	m_OK;
	CButton	m_Cancel;
	//}}AFX_DATA

protected:	
	CStringList *m_pStrList;
	int m_SelectedChange;
	BOOL m_Initialized;
	BOOL m_InitDialogDone;
	CP4ListBox m_List;
	BOOL m_DeletedRows;
	CWinPos m_WinPos;
	CRect m_InitRect;
	CRect m_LastRect;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRevertListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	
protected:
	// Generated message map functions
	//{{AFX_MSG(CRevertListDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDeleteselected();
	afx_msg void OnSelchangeList();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //__REVERTLISTDLG__
