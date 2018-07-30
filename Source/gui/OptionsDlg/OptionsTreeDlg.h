// OptionsTreeDlg.h : header file
//

#if !defined(AFX_OPTIONSTREEDLG_H__01DD0C28_2C3F_11D2_8085_D9D496965B4A__INCLUDED_)
#define AFX_OPTIONSTREEDLG_H__01DD0C28_2C3F_11D2_8085_D9D496965B4A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ConnectInfoPage.h"
#include "DepotPage.h"
#include "AutoResolvePage.h"
#include "ChangelistsPage.h"
#include "LabelsPage.h"
#include "ClientsPage.h"
#include "JobsPage.h"
#include "PanesPage.h"
#include "DoubleClickPage.h"
#include "EditorsPage.h"
#include "DialogsPage.h"
#include "HistoryPage.h"
#include "..\IntegDlg\OptionsPage.h"	// Integ Options
#include "HelperAppsPage.h"
#include "MergeAppPage.h"
#include "FileUtilitiesPage.h"
#include "TempDirPage.h"
#include "StatusPanePage.h"
#include "SpecsPage.h"
#include "DisplayPage.h"

/////////////////////////////////////////////////////////////////////////////
// COptionsTreeDlg dialog

class COptionsTreeDlg : public CDialog
{
// Construction
public:
	COptionsTreeDlg(CWnd* pParent = NULL);	// standard constructor

  bool m_bDlgCreated;
  int  m_InitialPage;
  CConnectInfoPage m_ConnectInfoPage;
  CPanesPage m_PanesPage;
  CDepotPage m_DepotPage;
  CChangelistsPage m_ChangelistsPage;
  CLabelsPage m_LabelsPage;
  CClientsPage m_ClientsPage;
  CJobsPage m_JobsPage;
  CDoubleClickPage m_DoubleClickPage;
  CEditorsPage m_EditorsPage;
  CDialogsPage m_DialogsPage;
  CHistoryPage m_HistoryPage;
  CIntegOptionsPage m_IntegrationPage;
  CHelperAppsPage m_HelperAppsPage;
  CMergeAppPage m_MergeAppPage;
  CFileUtilitiesPage m_FileUtilitiesPage;
  CTempDirPage m_TempDirPage;
  CStatusPanePage m_StatusPanePage;
  CSpecsPage m_SpecsPage;
  CAutoResolvePage m_AutoRslvPage;
  CDisplayPage m_DisplayPage;

// Dialog Data
	//{{AFX_DATA(COptionsTreeDlg)
	enum { IDD = IDD_OPTIONSTREE_DIALOG };
	CTreeCtrl	m_Tree;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsTreeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CRect GetPropertyPageRect();
	void OnApply();

	// Generated message map functions
	//{{AFX_MSG(COptionsTreeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangingTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnDestroy( );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSTREEDLG_H__01DD0C28_2C3F_11D2_8085_D9D496965B4A__INCLUDED_)
