// RemoveViewer.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "RemoveViewer.h"
#include "hlp\p4win.hh"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRemoveViewer dialog


CRemoveViewer::CRemoveViewer(CWnd* pParent /*=NULL*/)
	: CDialog(CRemoveViewer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRemoveViewer)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CRemoveViewer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRemoveViewer)
	DDX_Control(pDX, IDC_LIST, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRemoveViewer, CDialog)
	//{{AFX_MSG_MAP(CRemoveViewer)
	ON_BN_CLICKED(ID_HELP, OnHelp)
	//}}AFX_MSG_MAP
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRemoveViewer message handlers

BOOL CRemoveViewer::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	for(int i=0; i < MAX_MRU_VIEWERS; i++)
	{
		if( GET_P4REGPTR()->GetMRUViewerName(i).GetLength() > 0 )
		{
			m_List.AddString(GET_P4REGPTR()->GetMRUViewerName(i));
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRemoveViewer::OnOK() 
{
	UpdateData(TRUE);

	int cursel = m_List.GetCurSel();
	if (cursel >= 0)
	{
		CString txt;
		m_List.GetText(cursel, txt);
		//	txt now contain the viewer name; but we must convert it to the viewer path
		if (GET_P4REGPTR()->GetMRUViewerName(cursel) == txt)
			txt = GET_P4REGPTR()->GetMRUViewer(cursel);
		else
		{
			for(int i=0; i < MAX_MRU_VIEWERS; i++)
			{
				if( GET_P4REGPTR()->GetMRUViewerName(i) == txt )
				{
					txt = GET_P4REGPTR()->GetMRUViewer(cursel);
					break;
				}
			}
		}
		GET_P4REGPTR()->RmvMRUViewer(txt);
		CDialog::OnOK();
	}
}

void CRemoveViewer::OnHelp() 
{
	AfxGetApp()->WinHelp(TASK_VIEWING_FILES);
}

BOOL CRemoveViewer::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
