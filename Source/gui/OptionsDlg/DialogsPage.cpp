// OptionsDlg\DialogsPage.cpp : implementation file
//

#include "stdafx.h"
#include "P4win.h"
#include "OptionsDlg\DialogsPage.h"


// CDialogsPage dialog

IMPLEMENT_DYNAMIC(CDialogsPage, CPropertyPage)
CDialogsPage::CDialogsPage()
	: CPropertyPage(CDialogsPage::IDD)
{
	m_Inited = FALSE;
}

CDialogsPage::~CDialogsPage()
{
}

void CDialogsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDepotPage)
	DDX_Control(pDX, IDC_CHECK1, m_ShowDiscard);
	DDX_Control(pDX, IDC_CHECK2, m_ShowNewClient);
	DDX_Control(pDX, IDC_CHECK3, m_ShowSyncClient);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogsPage, CPropertyPage)
END_MESSAGE_MAP()


// CDialogsPage message handlers

BOOL CDialogsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// Init all data members from the registry
	m_ShowDiscard.SetCheck(!(GET_P4REGPTR()->DontShowDiscardFormChgs()));
	m_ShowNewClient.SetCheck(!(GET_P4REGPTR()->DontShowYouHaveCr8NewClient()));
    m_ShowSyncClient.SetCheck(!(GET_P4REGPTR()->DontShowYouHaveChgClientView()));

	// Update input fields
	UpdateData(FALSE);

	m_Inited = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogsPage::OnOK() 
{
	if (!m_Inited)
		return;

	// This bit of silliness is required to ensure all the 
	// fields provide their data to the UpdateData() routine
	GotoDlgCtrl(GetParent()->GetDlgItem(IDC_APPLY));

	int errorCount=0;

	if(UpdateData(TRUE))
	{
		BOOL b = !m_ShowDiscard.GetCheck();
		if (b != GET_P4REGPTR()->DontShowDiscardFormChgs())
		{
			if(!GET_P4REGPTR()->SetDontShowDiscardFormChgs( b ) )
				errorCount++;
		}

		b = !m_ShowNewClient.GetCheck();
		if (b != GET_P4REGPTR()->DontShowYouHaveCr8NewClient())
		{
			if(!GET_P4REGPTR()->SetDontShowYouHaveCr8NewClient( b ) )
				errorCount++;
		}

		b = !m_ShowSyncClient.GetCheck();
		if (b != GET_P4REGPTR()->DontShowYouHaveChgClientView())
		{
			if(!GET_P4REGPTR()->SetDontShowYouHaveChgClientView( b ) )
				errorCount++;
		}

		if(errorCount)
			AfxMessageBox(IDS_BAD_REGISTRY, MB_ICONSTOP);
	}		
}
