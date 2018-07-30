//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// MoveFiles.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "MoveFiles.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMoveFiles dialog


CMoveFiles::CMoveFiles(CWnd* pParent /*=NULL*/)
	: CDialog(CMoveFiles::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMoveFiles)
	//}}AFX_DATA_INIT
	m_SelectedChange = -1;
	m_Need2Refresh = FALSE;
}


void CMoveFiles::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMoveFiles)
	DDX_Control(pDX, IDC_ADDCHANGNUM, m_ChangeCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMoveFiles, CDialog)
	//{{AFX_MSG_MAP(CMoveFiles)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

	
/////////////////////////////////////////////////////////////////////////////
// CMoveFiles message handlers

BOOL CMoveFiles::OnInitDialog() 
{
	CDialog::OnInitDialog();

	/////////////////////////////
	// Fill in the change list
	POSITION pos;

	ASSERT(m_ChangeList.GetCount() > 0);
	for(pos=m_ChangeList.GetHeadPosition(); pos != NULL; )
	{
		m_ChangeCombo.AddString(m_ChangeList.GetNext(pos));
	}
	m_ChangeCombo.SetCurSel(0);

	UpdateData(FALSE);	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMoveFiles::OnOK() 
{
    // determine which changelist to move the files to
	int index=m_ChangeCombo.GetCurSel();
	CString txt;
	m_ChangeCombo.GetLBText(index, txt);
	if(txt.Compare(LoadStringResource(IDS_DEFAULTCHANGELISTNAME)) == 0)
		m_SelectedChange= 0;
	else if(txt.Compare(LoadStringResource(IDS_NEWCHANGELISTNAME)) == 0)
	{
		m_SelectedChange= MainFrame()->CreateNewChangeList(0);
		if (m_SelectedChange == -1)	// the user bailed
			return;
		m_Need2Refresh = TRUE;
	}
	else
		m_SelectedChange=_ttol(txt);
	
	CDialog::OnOK();
}
