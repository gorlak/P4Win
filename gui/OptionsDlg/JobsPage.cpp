//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// JobsPage.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "JobsPage.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJobsPage property page

IMPLEMENT_DYNCREATE(CJobsPage, CPropertyPage)

CJobsPage::CJobsPage() : CPropertyPage(CJobsPage::IDD)
{
	//{{AFX_DATA_INIT(CJobsPage)
	m_JobCount = 0;
	//}}AFX_DATA_INIT
	m_Inited = FALSE;
}

CJobsPage::~CJobsPage()
{
}

void CJobsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJobsPage)
	DDX_Control(pDX, IDC_POLLJOBS, m_PollJobs);
	DDX_Control(pDX, IDC_FETCHALLJOBS_RAD, m_JobCountRadio);
	DDX_Text(pDX, IDC_JOBCOUNT, m_JobCount);
	DDV_MinMaxInt(pDX, m_JobCount, 0, 1000000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CJobsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CJobsPage)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_FETCHALLJOBS_RAD, OnFetchRadio)
	ON_BN_CLICKED(IDC_FETCHXJOBS_RAD, OnFetchRadio)
	ON_BN_CLICKED(ID_JOB_CONFIGURE, OnJobConfigure)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJobsPage message handlers


BOOL CJobsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	CString str;

	// Init all data members from the registry
	m_JobCount = GET_P4REGPTR()->GetFetchJobCount();
	m_JobCountRadio.SetCheck(GET_P4REGPTR()->GetFetchAllJobs());
	if(!GET_P4REGPTR()->GetFetchAllJobs())
	{	
		CButton otherButton;
		otherButton.Attach(GetDlgItem(IDC_FETCHXJOBS_RAD)->m_hWnd);
		otherButton.SetCheck(1);
		otherButton.Detach();
	}

	// Enable/disable fetch change count per radio button
	OnFetchRadio();
	
	// Init the jobs polling flag
	m_PollJobs.SetCheck(GET_P4REGPTR()->GetAutoPollJobs());
	// If we are not polling, we don't allow polling for jobs
	if (!GET_P4REGPTR()->GetAutoPoll())
	{
		CWnd *pwnd=GetDlgItem(IDC_POLLJOBS);
		pwnd->ShowWindow(SW_HIDE);
	}

	// Update input fields
	UpdateData(FALSE);
	
	m_Inited = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CJobsPage::OnFetchRadio() 
{
	CWnd *pwnd=GetDlgItem(IDC_JOBCOUNT);
	if(m_JobCountRadio.GetCheck()==0) 
		pwnd->EnableWindow(TRUE);
	else
		pwnd->EnableWindow(FALSE);
}

void CJobsPage::OnJobConfigure()
{
	MainFrame()->OnJobConfigure();
}

void CJobsPage::OnOK() 
{
	if (!m_Inited)
		return;

	// This bit of silliness is required to ensure all the 
	// fields provide their data to the UpdateData() routine
	GotoDlgCtrl(GetParent()->GetDlgItem(IDC_APPLY));

	m_ErrorCount=0;

	if( UpdateData( TRUE ) )
	{
		BOOL jobsState=TRUE;	
		if(m_PollJobs.GetCheck() == 0)
			jobsState=FALSE;
		if( jobsState != GET_P4REGPTR()->GetAutoPollJobs() )
			if(!GET_P4REGPTR()->SetAutoPollJobs( jobsState) )
				m_ErrorCount++;

		BOOL allState=TRUE;
		if(m_JobCountRadio.GetCheck() == 0)
            allState=FALSE;
        if( allState != GET_P4REGPTR()->GetFetchAllJobs() )
			if(!GET_P4REGPTR()->SetFetchAllJobs( allState) )
				m_ErrorCount++;

		if ((m_JobCount != GET_P4REGPTR()->GetFetchJobCount())
		 || (!m_JobCount && !allState))
		{
			if (!m_JobCount && !allState)
				 m_JobCount = 1;
			if(!GET_P4REGPTR()->SetFetchJobCount(m_JobCount))
                m_ErrorCount++;
		}
		
		if(m_ErrorCount)
			AfxMessageBox(IDS_BAD_REGISTRY, MB_ICONSTOP);
	}		
}
