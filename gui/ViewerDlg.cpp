// ViewerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "MainFrm.h"
#include "ViewerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewerDlg dialog


CViewerDlg::CViewerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CViewerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CViewerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_NumMRUViewers=0;
}


void CViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewerDlg)
	DDX_Control(pDX, IDC_MRU_LIST, m_MRUList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CViewerDlg, CDialog)
	//{{AFX_MSG_MAP(CViewerDlg)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio)
	ON_BN_CLICKED(IDC_FINDVIEWER, OnFindviewer)
	ON_LBN_DBLCLK(IDC_MRU_LIST, OnDblclkMruList)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewerDlg message handlers

void CViewerDlg::OnFindviewer() 
{
	// Fire up a common dlg to find new file
	CFileDialog fDlg(TRUE, LoadStringResource(IDS_VIEWERDLG_EXTENSION), NULL,  
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NONETWORKBUTTON | OFN_PATHMUSTEXIST,
		LoadStringResource(IDS_VIEWERDLG_FILTER), this,
		MainFrame()->m_osVer.dwMajorVersion < 5 ? OPENFILENAME_SIZE_VERSION_400 : sizeof(OPENFILENAME)); 

	if(fDlg.DoModal() == IDOK)
	{
		m_Viewer= fDlg.GetPathName();
		EndDialog(IDOK);
	}
}

void CViewerDlg::GetViewerFromControls()
{
	if(GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO2)==IDC_RADIO1)
		m_Viewer= _T("SHELLEXEC");
	else 
	{
		int cursel=m_MRUList.GetCurSel();
		if(cursel == LB_ERR)
			m_Viewer.Empty();
		else
			m_MRUList.GetText(m_MRUList.GetCurSel(),m_Viewer);
	}
}

void CViewerDlg::OnRadio() 
{
	if(GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO2) == IDC_RADIO1)
	{
		m_MRUList.EnableWindow(FALSE);
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		GetViewerFromControls();
	}
	else
	{
		if(m_NumMRUViewers > 0)
		{
			m_MRUList.EnableWindow(TRUE);
			GetDlgItem(IDOK)->EnableWindow(TRUE);
		}
		else
			GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
}


void CViewerDlg::OnOK() 
{
    GetViewerFromControls();
    GetViewer();
	EndDialog(IDOK);
}

BOOL CViewerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Load all MRU viewers into list box
	for(int i=0; i < MAX_MRU_VIEWERS; i++)
	{
		if(GET_P4REGPTR()->GetMRUViewer(i).GetLength() > 0)
		{
			m_MRUList.AddString(GET_P4REGPTR()->GetMRUViewer(i));
			m_NumMRUViewers++;
		}
	}

	// Set a current selection
	if(m_NumMRUViewers)
	{
		m_MRUList.SetCurSel(0);
		GetDlgItem(IDC_RADIO2)->EnableWindow(TRUE);
		((CButton *) GetDlgItem(IDC_RADIO2))->SetCheck(1);
	}
	else
	{
		GetDlgItem(IDC_RADIO2)->EnableWindow(FALSE);
		// The default is a Windows associated viewer
		((CButton *) GetDlgItem(IDC_RADIO1))->SetCheck(1);
	}
	OnRadio();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CViewerDlg::OnDblclkMruList() 
{
	OnOK();
}
