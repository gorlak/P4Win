//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// LabelsPage.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "LabelsPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLabelsPage property page

IMPLEMENT_DYNCREATE(CLabelsPage, CPropertyPage)

CLabelsPage::CLabelsPage() : CPropertyPage(CLabelsPage::IDD)
{
	//{{AFX_DATA_INIT(CLabelsPage)
	m_LabelDragDropOption = -1;
	//}}AFX_DATA_INIT
	m_Inited = FALSE;
}

CLabelsPage::~CLabelsPage()
{
}

void CLabelsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLabelsPage)
	DDX_Control(pDX, IDC_LABEL_FILES_DIALOGBOX, m_LabelFilesInDialog);
	DDX_Control(pDX, IDC_PREVIEWSHOWSDETAILS, m_LabelShowPreviewDetail);
	DDX_Radio(pDX, ID_LABELDD_SHOWMENU, m_LabelDragDropOption);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLabelsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CLabelsPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLabelsPage message handlers

void CLabelsPage::OnOK() 
{
	if (!m_Inited)
		return;

	// This bit of silliness is required to ensure all the 
	// fields provide their data to the UpdateData() routine
	GotoDlgCtrl(GetParent()->GetDlgItem(IDC_APPLY));

	if( UpdateData( TRUE ) )
	{
		int i;
		int errors=0;

		BOOL labelFilesInDialog = TRUE;
		if( m_LabelFilesInDialog.GetCheck() == 0 )
			labelFilesInDialog = FALSE;
		if( labelFilesInDialog != GET_P4REGPTR()->LabelFilesInDialog( ) )
			if(!GET_P4REGPTR()->SetLabelFilesInDialog( labelFilesInDialog ) )
				errors++;

		BOOL labelShowPreviewDetail = TRUE;
		if( m_LabelShowPreviewDetail.GetCheck() == 0 )
			labelShowPreviewDetail = FALSE;
		if( labelShowPreviewDetail != GET_P4REGPTR()->LabelShowPreviewDetail( ) )
			if(!GET_P4REGPTR()->SetLabelShowPreviewDetail( labelShowPreviewDetail ) )
				errors++;

		for (i = -1; m_CvtLabelDragDropOption[++i] != -1; )
		{
			if (m_LabelDragDropOption == m_CvtLabelDragDropOption[i])
			{
				m_LabelDragDropOption = i;
				break;
			}
		}
		if( m_LabelDragDropOption != GET_P4REGPTR()->GetLabelDragDropOption() )
			if(!GET_P4REGPTR()->SetLabelDragDropOption(m_LabelDragDropOption))
				errors++;

		if(errors)
			AfxMessageBox(IDS_BAD_REGISTRY, MB_ICONSTOP);
	}
}

BOOL CLabelsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// Build a table to convert from value in registry to position in dialog
	m_CvtLabelDragDropOption[LDD_MENU]      = 0;
	m_CvtLabelDragDropOption[LDD_ADD]       = 1;
	m_CvtLabelDragDropOption[LDD_DELETE]    = 2;
	m_CvtLabelDragDropOption[LDD_SYNC]      = 3;
	m_CvtLabelDragDropOption[LDD_FILTER]    = 5;
	m_CvtLabelDragDropOption[LDD_FILTERREV] = 6;
	m_CvtLabelDragDropOption[LDD_ADD2VIEW]  = 4;
	m_CvtLabelDragDropOption[LDD_NOTINUSE]  = -1;
	
	m_LabelFilesInDialog.SetCheck(GET_P4REGPTR()->LabelFilesInDialog());
	m_LabelShowPreviewDetail.SetCheck(GET_P4REGPTR()->LabelShowPreviewDetail());
	m_LabelDragDropOption = m_CvtLabelDragDropOption[GET_P4REGPTR()->GetLabelDragDropOption()];

	UpdateData(FALSE);
	
	m_Inited = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
