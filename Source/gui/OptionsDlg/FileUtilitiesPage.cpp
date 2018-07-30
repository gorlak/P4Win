// FileUtilitiesPage.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "FileUtilitiesPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileUtilitiesPage dialog


IMPLEMENT_DYNCREATE(CFileUtilitiesPage, CPropertyPage)

CFileUtilitiesPage::CFileUtilitiesPage() : CPropertyPage(CFileUtilitiesPage::IDD)
{
	//{{AFX_DATA_INIT(CFileUtilitiesPage)
	m_P4AnnWhtSp = 0;
	m_P4TLVWhtSp = 0;
	//}}AFX_DATA_INIT
	m_Inited = FALSE;
}

CFileUtilitiesPage::~CFileUtilitiesPage()
{
}

void CFileUtilitiesPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileUtilitiesPage)
	DDX_Radio(pDX, IDC_WHTSP_INCLUDE2, m_P4AnnWhtSp);
	DDX_Control(pDX, IDC_INCLUDEINTEGS, m_P4AnnIncInteg);
//	DDX_Radio(pDX, IDC_WHTSP_INCLUDE3, m_P4TLVWhtSp);
	DDX_Control(pDX, IDC_INCLUDEINTEGSTLV, m_P4TLVIncInteg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileUtilitiesPage, CDialog)
	//{{AFX_MSG_MAP(CFileUtilitiesPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileUtilitiesPage message handlers

BOOL CFileUtilitiesPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// Init all data members from the registry
	m_P4AnnWhtSp = GET_P4REGPTR()->GetAnnotateWhtSpace();
	m_P4TLVWhtSp = GET_P4REGPTR()->GetTLVWhtSpace();
	if (!GET_P4REGPTR()->GetEnableSubChgIntegFilter( ))
	{
		GetDlgItem(IDC_INCLUDEINTEGS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_INCLUDEINTEGSTLV)->ShowWindow(SW_HIDE);
	}
	else
	{
		m_P4AnnIncInteg.SetCheck( GET_P4REGPTR()->GetAnnotateIncInteg() );
		m_P4TLVIncInteg.SetCheck( GET_P4REGPTR()->GetTLVIncInteg() );
	}

	UpdateData(FALSE);
	
	m_Inited = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFileUtilitiesPage::OnOK() 
{
	if (!m_Inited)
		return;

	// This bit of silliness is required to ensure all the 
	// fields provide their data to the UpdateData() routine
	GotoDlgCtrl(GetParent()->GetDlgItem(IDC_APPLY));

	int errors=0;

	if(UpdateData(TRUE))
	{
		if( m_P4AnnWhtSp != GET_P4REGPTR()->GetAnnotateWhtSpace() )
			if(!GET_P4REGPTR()->SetAnnotateWhtSpace(m_P4AnnWhtSp))
				errors++;
		
		BOOL incinteg=GET_P4REGPTR()->GetEnableSubChgIntegFilter();
		if(m_P4AnnIncInteg.GetCheck() == 0)
			incinteg=FALSE;
		if( incinteg != GET_P4REGPTR()->GetAnnotateIncInteg() )
			if(!GET_P4REGPTR()->SetAnnotateIncInteg( incinteg ) )
				errors++;

		if( m_P4TLVWhtSp != GET_P4REGPTR()->GetTLVWhtSpace() )
			if(!GET_P4REGPTR()->SetTLVWhtSpace(m_P4TLVWhtSp))
				errors++;
		
		BOOL incintegTLV=GET_P4REGPTR()->GetEnableSubChgIntegFilter();
		if(m_P4TLVIncInteg.GetCheck() == 0)
			incintegTLV=FALSE;
		if( incintegTLV != GET_P4REGPTR()->GetTLVIncInteg() )
			if(!GET_P4REGPTR()->SetTLVIncInteg( incintegTLV ) )
				errors++;

		if(errors)
			AfxMessageBox(IDS_BAD_REGISTRY, MB_ICONSTOP);
	}		
}
