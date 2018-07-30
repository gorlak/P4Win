//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// A simple dialog which displays a list of files to be added in a
// listbox.  The listbox is intended for DoModal() use.  Files can 
// be deleted from the list, and the target change can be updated
// The file list, change list, and selected change are set in the Init() 
// member function prior to calling DoModal()

// FirstTimeDlg.h

#ifndef __FIRSTTIMEDLG__
#define __FIRSTTIMEDLG__

#include "WinPos.h"

/////////////////////////////////////////////////////////////////////////////
// CFirstTimeDlg dialog

class CFirstTimeDlg : public CDialog
{
// Construction
public:
	CFirstTimeDlg(CWnd* pParent = NULL);   // standard constructor
					
// Dialog Data
	//{{AFX_DATA(CFirstTimeDlg)
	enum { IDD = IDD_FIRSTTIMEDLG };
	//}}AFX_DATA

protected:	
	CStringList m_StrList;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFirstTimeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:

protected:
	CBitmap m_Bitmap;

	// Generated message map functions
	//{{AFX_MSG(CFirstTimeDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnGetAdobe();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //__FIRSTTIMEDLG__
