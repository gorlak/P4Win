// OptionsTreeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "P4Win.h"
#include "OptionsTreeDlg.h"
#include "hlp\p4win.hh"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsTreeDlg dialog

COptionsTreeDlg::COptionsTreeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptionsTreeDlg::IDD, pParent)
{
	m_bDlgCreated = false;

	//{{AFX_DATA_INIT(COptionsTreeDlg)
	m_InitialPage = IDS_PAGE_CONNECTION;
	//}}AFX_DATA_INIT
}

void COptionsTreeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsTreeDlg)
	DDX_Control(pDX, IDC_TREE, m_Tree);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COptionsTreeDlg, CDialog)
	//{{AFX_MSG_MAP(COptionsTreeDlg)
	ON_NOTIFY(TVN_SELCHANGING, IDC_TREE, OnSelchangingTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	ON_COMMAND(IDC_APPLY, OnApply)
	ON_COMMAND(IDCANCEL, OnCancel)
	ON_COMMAND(IDHELP, OnHelp)
	ON_WM_HELPINFO()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsTreeDlg message handlers

BOOL COptionsTreeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	HTREEITEM hTreeSelect = 0;

	HTREEITEM hTreePanes = m_Tree.InsertItem(LoadStringResource(IDS_PAGE_PANES),
		0, 1, TVI_ROOT, TVI_LAST);
	m_Tree.SetItemData(hTreePanes, (DWORD)&m_PanesPage);
	if (m_InitialPage == IDS_PAGE_PANES)
		hTreeSelect = hTreePanes;

	HTREEITEM hTreeConnectInfo = m_Tree.InsertItem(LoadStringResource(IDS_PAGE_CONNECTION),
		0, 1, hTreePanes, TVI_LAST);
	m_Tree.SetItemData(hTreeConnectInfo, (DWORD)&m_ConnectInfoPage);
	if (m_InitialPage == IDS_PAGE_CONNECTION)
		hTreeSelect = hTreeConnectInfo;

	HTREEITEM hTreeDepot = m_Tree.InsertItem(LoadStringResource(IDS_PAGE_DEPOT),
		0, 1, hTreePanes, TVI_LAST);
	m_Tree.SetItemData(hTreeDepot, (DWORD)&m_DepotPage);
	if (m_InitialPage == IDS_PAGE_DEPOT)
		hTreeSelect = hTreeDepot;

	HTREEITEM hTreeChangelists = m_Tree.InsertItem(LoadStringResource(IDS_PAGE_CHANGELIST),
		0, 1, hTreePanes, TVI_LAST);
	m_Tree.SetItemData(hTreeChangelists, (DWORD)&m_ChangelistsPage);
	if (m_InitialPage == IDS_PAGE_CHANGELIST)
		hTreeSelect = hTreeChangelists;

	HTREEITEM hTreeLabels = m_Tree.InsertItem(LoadStringResource(IDS_PAGE_LABEL),
		0, 1, hTreePanes, TVI_LAST);
	m_Tree.SetItemData(hTreeLabels, (DWORD)&m_LabelsPage);
	if (m_InitialPage == IDS_PAGE_LABEL)
		hTreeSelect = hTreeLabels;

	HTREEITEM hTreeClients = m_Tree.InsertItem(LoadStringResource(IDS_PAGE_CLIENT),
		0, 1, hTreePanes, TVI_LAST);
	m_Tree.SetItemData(hTreeClients, (DWORD)&m_ClientsPage);
	if (m_InitialPage == IDS_PAGE_CLIENT)
		hTreeSelect = hTreeClients;

	HTREEITEM hTreeJobs = m_Tree.InsertItem(LoadStringResource(IDS_PAGE_JOB),
		0, 1, hTreePanes, TVI_LAST);
	m_Tree.SetItemData(hTreeJobs, (DWORD)&m_JobsPage);
	if (m_InitialPage == IDS_PAGE_JOB)
		hTreeSelect = hTreeJobs;

	HTREEITEM hTreeStatusPane = m_Tree.InsertItem(LoadStringResource(IDS_PAGE_STATUS),
		0, 1, hTreePanes, TVI_LAST);
	m_Tree.SetItemData(hTreeStatusPane, (DWORD)&m_StatusPanePage);
	if (m_InitialPage == IDS_PAGE_STATUS)
		hTreeSelect = hTreeStatusPane;

	HTREEITEM hTreeFiles = m_Tree.InsertItem(LoadStringResource(IDS_PAGE_FILES),
		0, 1, TVI_ROOT, TVI_LAST);
	m_Tree.SetItemData(hTreeFiles, (DWORD)&m_TempDirPage);
	if (m_InitialPage == IDS_PAGE_FILES)
		hTreeSelect = hTreeFiles;

	HTREEITEM hTreeEditors = m_Tree.InsertItem(LoadStringResource(IDS_PAGE_VIEWEDIT),
		0, 1, hTreeFiles, TVI_LAST);
	m_Tree.SetItemData(hTreeEditors, (DWORD)&m_EditorsPage);
	if (m_InitialPage == IDS_PAGE_VIEWEDIT)
		hTreeSelect = hTreeEditors;

	HTREEITEM hTreeHelperApps = m_Tree.InsertItem(LoadStringResource(IDS_PAGE_DIFF),
		0, 1, hTreeFiles, TVI_LAST);
	m_Tree.SetItemData(hTreeHelperApps, (DWORD)&m_HelperAppsPage);
	if (m_InitialPage == IDS_PAGE_DIFF)
		hTreeSelect = hTreeHelperApps;

	HTREEITEM hTreeMergeApp = m_Tree.InsertItem(LoadStringResource(IDS_PAGE_MERGE),
		0, 1, hTreeFiles, TVI_LAST);
	m_Tree.SetItemData(hTreeMergeApp, (DWORD)&m_MergeAppPage);
	if (m_InitialPage == IDS_PAGE_MERGE)
		hTreeSelect = hTreeMergeApp;

	HTREEITEM hTreeFileUtilities = m_Tree.InsertItem(LoadStringResource(IDS_PAGE_FILEUTIL),
		0, 1, hTreeFiles, TVI_LAST);
	m_Tree.SetItemData(hTreeFileUtilities, (DWORD)&m_FileUtilitiesPage);
	if (m_InitialPage == IDS_PAGE_FILEUTIL)
		hTreeSelect = hTreeFileUtilities;

	HTREEITEM hTreeDoubleClick = m_Tree.InsertItem(LoadStringResource(IDS_PAGE_DOUBLECLICK),
		0, 1, hTreeFiles, TVI_LAST);
	m_Tree.SetItemData(hTreeDoubleClick, (DWORD)&m_DoubleClickPage);
	if (m_InitialPage == IDS_PAGE_DOUBLECLICK)
		hTreeSelect = hTreeDoubleClick;

	HTREEITEM hTreeDialogs = m_Tree.InsertItem(LoadStringResource(IDS_PAGE_DIALOGS),
		0, 1, TVI_ROOT, TVI_LAST);
	m_Tree.SetItemData(hTreeDialogs, (DWORD)&m_DialogsPage);
	if (m_InitialPage == IDS_PAGE_DIALOGS)
		hTreeSelect = hTreeDialogs;

	HTREEITEM hTreeHist = m_Tree.InsertItem(LoadStringResource(IDS_PAGE_REVISIONHISTORY),
		0, 1, hTreeDialogs, TVI_LAST);
	m_Tree.SetItemData(hTreeHist, (DWORD)&m_HistoryPage);
	if (m_InitialPage == IDS_PAGE_REVISIONHISTORY)
		hTreeSelect = hTreeHist;

	HTREEITEM hTreeInteg = m_Tree.InsertItem(LoadStringResource(IDS_PAGE_INTEGRATION),
		0, 1, hTreeDialogs, TVI_LAST);
	m_Tree.SetItemData(hTreeInteg, (DWORD)&m_IntegrationPage);
	if (m_InitialPage == IDS_PAGE_INTEGRATION)
		hTreeSelect = hTreeInteg;

	HTREEITEM hTreeSpecs = m_Tree.InsertItem(LoadStringResource(IDS_PAGE_SPECIFICATIONS),
		0, 1, hTreeDialogs, TVI_LAST);
	m_Tree.SetItemData(hTreeSpecs, (DWORD)&m_SpecsPage);
	if (m_InitialPage == IDS_PAGE_SPECIFICATIONS)
		hTreeSelect = hTreeSpecs;

	HTREEITEM hTreeAutoR = m_Tree.InsertItem(LoadStringResource(IDS_PAGE_AUTORESOLVE),
		0, 1, hTreeDialogs, TVI_LAST);
	m_Tree.SetItemData(hTreeAutoR, (DWORD)&m_AutoRslvPage);
	if (m_InitialPage == IDS_PAGE_AUTORESOLVE)
		hTreeSelect = hTreeAutoR;

	HTREEITEM hTreeDisplay = m_Tree.InsertItem(LoadStringResource(IDS_PAGE_OPTIONS),
		0, 1, hTreeDialogs, TVI_LAST);
	m_Tree.SetItemData(hTreeDisplay, (DWORD)&m_DisplayPage);
	if (m_InitialPage == IDS_PAGE_OPTIONS)
		hTreeSelect = hTreeDisplay;

	if (GET_P4REGPTR()->AutoExpandOptions())
	{
		m_Tree.Expand(hTreePanes,  TVE_EXPAND);
		m_Tree.Expand(hTreeFiles,  TVE_EXPAND);
		m_Tree.Expand(hTreeDialogs,TVE_EXPAND);
	}

	if (hTreeSelect)
		m_Tree.Select(hTreeSelect, TVGN_CARET);

	m_bDlgCreated = true;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////

void COptionsTreeDlg::OnSelchangingTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(m_bDlgCreated)
	{
		HTREEITEM hCurrent = m_Tree.GetSelectedItem();
		if(hCurrent)
			((CPropertyPage*)m_Tree.GetItemData(hCurrent))->ShowWindow(SW_HIDE);
	}

	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////

void COptionsTreeDlg::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if (hItem)
	{
		CPropertyPage* pPage = (CPropertyPage*)m_Tree.GetItemData(hItem);
		if (IsWindow(pPage->m_hWnd) == 0)
		{
			pPage->Create(pPage->m_psp.pszTemplate, this);

			CRect rectPage(GetPropertyPageRect());
			pPage->SetWindowPos(&m_Tree, rectPage.left, rectPage.top, rectPage.Width(), rectPage.Height(), 0);
		}
		pPage->ShowWindow(SW_SHOW);
	}
	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////

CRect COptionsTreeDlg::GetPropertyPageRect()
{
	CRect rcDlgs;
	GetDlgItem(IDC_DLG_AREA)->GetWindowRect(rcDlgs);
	ScreenToClient(rcDlgs);
//	rcDlgs.left -= 1;
	return rcDlgs;
} 

void COptionsTreeDlg::OnApply() 
{
	m_ConnectInfoPage.OnOK();
	m_DepotPage.OnOK();
	m_ChangelistsPage.OnOK();
	m_LabelsPage.OnOK();
	m_ClientsPage.OnOK();
	m_JobsPage.OnOK();
	m_PanesPage.OnOK();
	m_DoubleClickPage.OnOK();
	m_EditorsPage.OnOK();
	m_DialogsPage.OnOK();
	m_HistoryPage.OnOK();
	m_IntegrationPage.OnOK();
	m_HelperAppsPage.OnOK();
	m_MergeAppPage.OnOK();
	m_FileUtilitiesPage.OnOK();
	m_TempDirPage.OnOK();
	m_StatusPanePage.OnOK();
	m_SpecsPage.OnOK();
	m_AutoRslvPage.OnOK();
	m_DisplayPage.OnOK();
}

void COptionsTreeDlg::OnOK() 
{
	OnApply();
	CDialog::OnOK();
}

void COptionsTreeDlg::OnCancel() 
{
	if (!m_HelperAppsPage.OK2Cancel()
	 || !m_MergeAppPage.OK2Cancel())
		return;
	CDialog::OnCancel();
}

///////////////////////////////////////
// Handler for help button
void COptionsTreeDlg::OnHelp() 
{
	DWORD helpID=0;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if (hItem)
	{
		DWORD pPage = (DWORD)m_Tree.GetItemData(hItem);

		if      (pPage == (DWORD)&m_ConnectInfoPage)
			helpID= DLG_OPT_CONNECTION;
		else if (pPage == (DWORD)&m_DepotPage)
			helpID= DLG_OPT_PANES_DEPOT;
		else if (pPage == (DWORD)&m_ChangelistsPage)
			helpID= DLG_OPT_PANES_CHANGELISTS;
		else if (pPage == (DWORD)&m_LabelsPage)
			helpID= DLG_OPT_PANES_LABELS;
		else if (pPage == (DWORD)&m_ClientsPage)
			helpID= DLG_OPT_PANES_CLIENTS;
		else if (pPage == (DWORD)&m_JobsPage)
			helpID= DLG_OPT_PANES_JOBS;
		else if (pPage == (DWORD)&m_EditorsPage)
			helpID= DLG_OPT_FILES_VIEW;
		else if (pPage == (DWORD)&m_HelperAppsPage)
			helpID= DLG_OPT_FILES_DIFF;
		else if (pPage == (DWORD)&m_MergeAppPage)
			helpID= DLG_OPT_FILES_MERGE;
		else if (pPage == (DWORD)&m_FileUtilitiesPage)
			helpID= DLG_OPT_FILES_FILEUTIL;
		else if (pPage == (DWORD)&m_PanesPage)
			helpID= DLG_OPT_PANES;
		else if (pPage == (DWORD)&m_SpecsPage)
			helpID= DLG_OPT_DIALOGS_SECIFICATION;
		else if (pPage == (DWORD)&m_AutoRslvPage)
			helpID= TASK_RESOLVING_FILES;
		else if (pPage == (DWORD)&m_DisplayPage)
			helpID= DLG_OPT_DIALOGS_OPTIONS;
		else if (pPage == (DWORD)&m_DialogsPage)
			helpID= DLG_OPT_DIALOGS_ADVANCED;
		else if (pPage == (DWORD)&m_HistoryPage)
			helpID= DLG_OPT_DIALOGS_REVHIST ;
		else if (pPage == (DWORD)&m_IntegrationPage)
			helpID= DLG_OPT_DIALOGS_INTEGRATION ;
		else if (pPage == (DWORD)&m_DoubleClickPage)
			helpID= DLG_OPT_DOUBLE_CLICK ;
		else if (pPage == (DWORD)&m_TempDirPage)
			helpID= DLG_OPT_FILES;
		else if (pPage == (DWORD)&m_StatusPanePage)
			helpID= DLG_OPT_PANES_STATUS;
	}
	ASSERT(helpID);
	if(helpID)
		AfxGetApp()->WinHelp(helpID);
}

BOOL COptionsTreeDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}

void COptionsTreeDlg::OnDestroy( )
{
	switch(GET_P4REGPTR()->GetOptionStartUpOption())
	{
	default:
		break;
	case 1:
	  {
		CString curTxt = m_Tree.GetItemText(m_Tree.GetSelectedItem( ));
		for (int i = IDS_PAGE_1ST-1; ++i <= IDS_PAGE_LAST; )
		{
			if (curTxt == LoadStringResource(i))
			{
				GET_P4REGPTR()->SetOptionStartUpPage(i);
				break;
			}
		}
		break;
	  }
	case 2:
		GET_P4REGPTR()->SetOptionStartUpPage(IDS_PAGE_CONNECTION);
		break;
	}
}
