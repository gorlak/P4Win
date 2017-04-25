//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// A simple dialog which displays a list of files to be added in a
// listbox.  The listbox is intended for DoModal() use.  Files can 
// be deleted from the list, and the target change can be updated
// The file list, change list, and selected change are set in the Init() 
// member function prior to calling DoModal()

// MoveFiles.h

#ifndef __MOVEFILES__
#define __MOVEFILES__

#include "WinPos.h"

/////////////////////////////////////////////////////////////////////////////
// CMoveFiles dialog

class CMoveFiles : public CDialog
{
// Construction
public:
	CMoveFiles(CWnd* pParent = NULL);   // standard constructor
					
// Dialog Data
	//{{AFX_DATA(CMoveFiles)
	enum { IDD = IDD_MOVEFILES };
	CComboBox	m_ChangeCombo;
	//}}AFX_DATA

protected:	
	CStringList m_StrList;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMoveFiles)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	CStringList m_ChangeList;
	long m_SelectedChange;
	BOOL m_Need2Refresh;

protected:

	// Generated message map functions
	//{{AFX_MSG(CMoveFiles)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //__MOVEFILES__
