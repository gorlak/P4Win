/*
 * Copyright 1998 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */

// TemplateNameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "TemplateNameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// These defines wrenched out of P4Command.h.  It would be a
// good idea to consolidate all the defines into a single
// header so that huge headers dont need to be included just
// to get at a couple defines, or maybe worse, defines duplicated 
// as below.
#define P4CLIENT_SPEC	4
#define P4LABEL_SPEC	7

/////////////////////////////////////////////////////////////////////////////
// CTemplateNameDlg dialog


CTemplateNameDlg::CTemplateNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTemplateNameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTemplateNameDlg)
	m_NewSpecName = _T("");
	m_TemplateSpecName = _T("");
	//}}AFX_DATA_INIT
	m_SpecType = -1;
}


void CTemplateNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplateNameDlg)
	DDX_Text(pDX, IDC_SPECNAME_NEW, m_NewSpecName);
	DDV_MaxChars(pDX, m_NewSpecName, 1024);
	DDX_Text(pDX, IDC_TEMPLATENAME, m_TemplateSpecName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTemplateNameDlg, CDialog)
	//{{AFX_MSG_MAP(CTemplateNameDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTemplateNameDlg message handlers

void CTemplateNameDlg::OnOK() 
{
    CString errorTxt;
	UpdateData();
    m_NewSpecName.TrimLeft();
    m_NewSpecName.TrimRight();
        
    // Check for illegal characters
    if( m_NewSpecName.FindOneOf(_T(" #@\'")) != -1)
        errorTxt= LoadStringResource(IDS_ERROR_EMBEDDED_SPACE_OR_ILLEGAL_CHARACTER_IN_NAME);
    else
    {
        // See if the user entered a bad specname
        BOOL badName=FALSE;
        POSITION pos= m_pVerbotenSpecNames->GetHeadPosition();
        while(pos!=NULL)
        {
            CString existingName= m_pVerbotenSpecNames->GetNext(pos);

            // Compare spec name with due regard to server case sensitivity
            if( !Compare(m_NewSpecName, existingName) )
            {
                badName=TRUE;
                break;
            }
        }
	
		if(badName)
        {
			CString specTypeName;
			if( m_SpecType == P4CLIENT_SPEC )
				specTypeName= _T("client");
			else if( m_SpecType == P4LABEL_SPEC )
				specTypeName= _T("label");
			else
				ASSERT(0);

			if( GET_SERVERLEVEL() < 6 )
			{
				// We got a bad client name, so inform the user
				errorTxt.FormatMessage(IDS_ERROR_YOU_PROVIDED_THE_NAME_OF_AN_EXISTING_s_s_s, 
								specTypeName, specTypeName, specTypeName);
			}
			else
			{
				errorTxt.FormatMessage(IDS_ERROR_TO_CREATE_A_NEW_s_FROM_A_TEMPLATE_YOU_NEED_s, 
								specTypeName, specTypeName);
			}
        }
    }

    if( !errorTxt.IsEmpty() )
    {
        AfxMessageBox( errorTxt, MB_ICONEXCLAMATION);
        return;
    }
    else
	    CDialog::OnOK();
}

BOOL CTemplateNameDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if( m_SpecType == P4CLIENT_SPEC )
	{
		SetWindowText(LoadStringResource(IDS_CREATE_OR_UPDATE_CLIENT_FROM_TEMPLATE));
		SetDlgItemText(IDC_TARGETNAME, LoadStringResource(IDS_CLIENT_NAME));
	}
	else if( m_SpecType == P4LABEL_SPEC )
	{
		SetWindowText(LoadStringResource(IDS_CREATE_OR_UPDATE_LABEL_FROM_TEMPLATE));
		SetDlgItemText(IDC_TARGETNAME, LoadStringResource(IDS_LABEL_NAME));
	}
	else
		ASSERT(0);

	GotoDlgCtrl(GetDlgItem(IDC_SPECNAME_NEW));
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
