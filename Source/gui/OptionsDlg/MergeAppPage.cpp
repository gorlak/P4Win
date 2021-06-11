//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// MergeAppPage.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "MainFrm.h"
#include "MergeAppPage.h"
#include "DiffAssocDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMergeAppPage property page

IMPLEMENT_DYNCREATE(CMergeAppPage, CPropertyPage)

CMergeAppPage::CMergeAppPage() : CPropertyPage(CMergeAppPage::IDD)
{
	//{{AFX_DATA_INIT(CMergeAppPage)
	m_MergeUserApp = _T("");
	//}}AFX_DATA_INIT
	m_Inited = m_Closing = FALSE;
}

CMergeAppPage::~CMergeAppPage()
{
}

void CMergeAppPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMergeAppPage)
	DDX_Control(pDX, IDC_NSF, m_MergeNSF);
	DDX_Control(pDX, IDC_MRG_P4_RAD, m_P4MrgRadio);
	DDX_Control(pDX, IDC_MRG_OTHER_RAD, m_OtherMrgRadio);
	DDX_Control(pDX, IDC_MRG_P4DIFF_RAD, m_WinMrgRadio);
	DDX_Control(pDX, IDC_MERGECLOSE, m_MergeClose);
	DDX_Control(pDX, IDC_MERGEBROWSE, m_MergeBrowse);
	DDX_Control(pDX, IDC_MERGECONSOLE, m_MergeConsole);
	DDX_Radio(pDX, IDC_WHTSP_LINEEND2, m_P4MrgWhtSp);
	DDX_Text(pDX, IDC_MERGE_USERAPP, m_MergeUserApp);
	DDX_Text(pDX, IDC_MRGTABWIDTH, m_MrgTabSize);
	DDX_Control(pDX, IDC_MERGEOPTARGCHK, m_MergeOptArgChk);
	DDX_Text(pDX, IDC_MERGEOPTARGS, m_MergeOptArgs);
	DDX_Control(pDX, IDC_P4ASSAPPS2, m_List);
	DDX_Control(pDX, IDC_EDAPP, m_Edit);
	DDX_Control(pDX, IDC_DELAPP, m_Delete);
	DDX_Control(pDX, IDC_ADDAPP, m_Add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMergeAppPage, CPropertyPage)
	//{{AFX_MSG_MAP(CMergeAppPage)
	ON_BN_CLICKED(IDC_MRG_P4_RAD, OnMrgP4Radio)
	ON_BN_CLICKED(IDC_MRG_OTHER_RAD, OnMrgP4Radio)
	ON_BN_CLICKED(IDC_MRG_P4DIFF_RAD, OnMrgP4Radio)
	ON_BN_CLICKED(IDC_MERGEBROWSE, OnMergebrowse)
	ON_BN_CLICKED(IDC_MERGECONSOLE, OnMergeconsole)
	ON_BN_CLICKED(IDC_MERGEOPTARGCHK, OnMergeOptArgChk)
	ON_BN_CLICKED(IDC_ADDAPP, OnAdd)
	ON_BN_CLICKED(IDC_DELAPP, OnDelete)
	ON_BN_CLICKED(IDC_EDAPP, OnEdit)
	ON_LBN_SELCHANGE(IDC_P4ASSAPPS2, OnP4AssAppsSelChange)
	ON_EN_KILLFOCUS(IDC_MERGEOPTARGS, OnKillfocusArgs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMergeAppPage message handlers

void CMergeAppPage::OnMrgP4Radio() 
{
	UpdateData();
	CWnd *pwnd1=GetDlgItem(IDC_MERGE_USERAPP);
	CWnd *pwnd2=GetDlgItem(IDC_MERGECONSOLE);
	CWnd *pwnd3=GetDlgItem(IDC_MERGEBROWSE);
	CWnd *pwnd4=GetDlgItem(IDC_NSF);
	CWnd *pwnd6=GetDlgItem(IDC_MERGECLOSE);
	CWnd *pwnd8=GetDlgItem(IDC_WHTSP_LINEEND2);
	CWnd *pwnd9=GetDlgItem(IDC_WHTSP_INCLUDE2);
	CWnd *pwnd10=GetDlgItem(IDC_WHTSP_IGNOREDIFFS2);
	CWnd *pwnd11=GetDlgItem(IDC_WHTSP_IGNOREALL2);
	CWnd *pwnd12=GetDlgItem(IDC_WHTSP_PREFS2);
	CWnd *pwnd13=GetDlgItem(IDC_MRGTABWIDTH_CAPTION);
	CWnd *pwnd14=GetDlgItem(IDC_MRGTABWIDTH);
	CWnd *pwnd15=GetDlgItem(IDC_MERGEOPTARGCHK);
	CWnd *pwnd16=GetDlgItem(IDC_MERGEOPTARGS);

	if(m_OtherMrgRadio.GetCheck()==1)	// Other Merge app
	{
		pwnd1->EnableWindow(TRUE);
		pwnd2->EnableWindow(TRUE);
		pwnd3->EnableWindow(TRUE);
		pwnd4->EnableWindow(FALSE);
		pwnd8->EnableWindow(FALSE);
		pwnd9->EnableWindow(FALSE);
		pwnd10->EnableWindow(FALSE);
		pwnd11->EnableWindow(FALSE);
		pwnd12->EnableWindow(FALSE);
		pwnd13->EnableWindow(FALSE);
		pwnd14->EnableWindow(FALSE);
		pwnd6->EnableWindow(m_MergeConsole.GetCheck());
		pwnd15->EnableWindow(TRUE);
		pwnd16->EnableWindow(m_MergeOptArgChk.GetCheck());
	}
	else if(m_WinMrgRadio.GetCheck()==1)	// P4WinMerge app
	{
		pwnd1->EnableWindow(FALSE);
		pwnd2->EnableWindow(FALSE);
		pwnd3->EnableWindow(FALSE);
		pwnd4->EnableWindow(TRUE);
		pwnd8->EnableWindow(TRUE);
		pwnd10->EnableWindow(TRUE);
		pwnd11->EnableWindow(TRUE);
		if (m_P4MrgWhtSp > 2)
		{
			m_P4MrgWhtSp = 0;
			UpdateData(FALSE);
		}
		pwnd9->EnableWindow(FALSE);
		pwnd12->EnableWindow(FALSE);
		pwnd6->EnableWindow(FALSE);
		pwnd13->EnableWindow(TRUE);
		pwnd14->EnableWindow(TRUE);
		pwnd15->EnableWindow(FALSE);
		pwnd16->EnableWindow(FALSE);
	}
	else
	{
		pwnd1->EnableWindow(FALSE);
		pwnd2->EnableWindow(FALSE);
		pwnd3->EnableWindow(FALSE);
		pwnd4->EnableWindow(TheApp()->m_P4Merge ? FALSE : TRUE);
		pwnd8->EnableWindow(TRUE);
		pwnd9->EnableWindow(TRUE);
		pwnd10->EnableWindow(TRUE);
		pwnd11->EnableWindow(TRUE);
		pwnd12->EnableWindow(TRUE);
		pwnd6->EnableWindow(FALSE);
		pwnd13->EnableWindow(TRUE);
		pwnd14->EnableWindow(TRUE);
		pwnd15->EnableWindow(FALSE);
		pwnd16->EnableWindow(FALSE);
	}
}

void CMergeAppPage::OnOK() 
{
	if (!m_Inited)
		return;

	// This bit of silliness is required to ensure all the 
	// fields provide their data to the UpdateData() routine
	GotoDlgCtrl(GetParent()->GetDlgItem(IDC_APPLY));

	int errors=0;

	if(UpdateData(TRUE))
	{
		// MERGE Application
		int useInternal = TheApp()->m_P4Merge ? 2 : 1;
		if(m_OtherMrgRadio.GetCheck() == 1)
			useInternal = 0;
		else if (m_WinMrgRadio.GetCheck() == 1)
			useInternal = 1;
		if( useInternal != GET_P4REGPTR()->GetMergeInternal() )
			if(!GET_P4REGPTR()->SetMergeInternal(useInternal))
				errors++;
		
		if( m_MrgTabSize != GET_P4REGPTR()->GetMrgTabWidth() )
			if(!GET_P4REGPTR()->SetMrgTabWidth( m_MrgTabSize) )
				errors++;

		BOOL nsf=TRUE;
		if(m_MergeNSF.GetCheck() == 0)
			nsf=FALSE;
		if( nsf != GET_P4REGPTR()->GetMergeNSF() )
			if(!GET_P4REGPTR()->SetMergeNSF( nsf ) )
				errors++;

		if( m_P4MrgWhtSp != GET_P4REGPTR()->GetMrgWhtSpFlag() )
			if(!GET_P4REGPTR()->SetMrgWhtSpFlag( m_P4MrgWhtSp) )
				errors++;
			else
				if(!GET_P4REGPTR()->SetResolveWhtSp( m_P4MrgWhtSp) )
					errors++;

		BOOL mergeConsole=TRUE;
		if(m_MergeConsole.GetCheck() == 0)
			mergeConsole=FALSE;
		if( mergeConsole != GET_P4REGPTR()->GetMergeAppIsConsole() )
			if(!GET_P4REGPTR()->SetMergeAppIsConsole( mergeConsole) )
				errors++;

		BOOL mergeClose=TRUE;
		if(m_MergeClose.GetCheck() == 0)
			mergeClose=FALSE;
		if( mergeClose != GET_P4REGPTR()->GetMergeAppIsClose() )
			if(!GET_P4REGPTR()->SetMergeAppIsClose( mergeClose) )
				errors++;

		m_MergeUserApp.TrimRight();
		m_MergeUserApp.TrimLeft();
		if( m_MergeUserApp != GET_P4REGPTR()->GetMergeApp() )
			if(!GET_P4REGPTR()->SetMergeApp( m_MergeUserApp) )
				errors++;

		BOOL mrgOptArgChk=TRUE;
		if(m_MergeOptArgChk.GetCheck() == 0)
			mrgOptArgChk=FALSE;
		if( mrgOptArgChk != GET_P4REGPTR()->GetMergeAppOptArgChk() )
			if(!GET_P4REGPTR()->SetMergeOptArgChk( mrgOptArgChk) )
				errors++;

		if( m_MergeOptArgs != GET_P4REGPTR()->GetMergeOptArgs() )
		{
			if (m_MergeOptArgs.Find(_T("%1")) == -1 || m_MergeOptArgs.Find(_T("%2")) == -1
			 || m_MergeOptArgs.Find(_T("%3")) == -1 || m_MergeOptArgs.Find(_T("%4")) == -1)
				m_MergeOptArgs = GET_P4REGPTR()->GetMergeOptArgs();
			else if (!GET_P4REGPTR()->SetMergeOptArgs( m_MergeOptArgs) )
				errors++;
		}

		if(errors)
			AfxMessageBox(IDS_BAD_REGISTRY, MB_ICONSTOP);

	}		
}

BOOL CMergeAppPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// Init all data members from the registry
	m_MrgTabSize = GET_P4REGPTR()->GetMrgTabWidth();
	m_MergeOptArgs = GET_P4REGPTR()->GetMergeOptArgs();
	m_P4MrgWhtSp = GET_P4REGPTR()->GetMrgWhtSpFlag();
	m_MergeUserApp = GET_P4REGPTR()->GetMergeApp();

	int iInternal = GET_P4REGPTR()->GetMergeInternal();
	if (!TheApp()->m_P4Merge)
	{
		if (iInternal == 1)
			iInternal = 2;
		CWnd *pwnd = GetDlgItem(IDC_MRG_P4DIFF_RAD);
		pwnd->EnableWindow(FALSE);
		pwnd->ShowWindow(SW_HIDE);
	}
	switch(iInternal)
	{
	case 0:
		m_OtherMrgRadio.SetCheck(1);
		break;
	default:
	case 1:
		m_WinMrgRadio.SetCheck(1);
		break;
	case 2:
		m_P4MrgRadio.SetCheck(1);
		break;
	}
	m_MergeNSF.SetCheck( GET_P4REGPTR()->GetMergeNSF() );
	m_MergeConsole.SetCheck( GET_P4REGPTR()->GetMergeAppIsConsole() );
	m_MergeClose.SetCheck( GET_P4REGPTR()->GetMergeAppIsClose() );
	m_MergeOptArgChk.SetCheck( GET_P4REGPTR()->GetMergeAppOptArgChk() );

	// Enable/disable merge widgets per radio button
	UpdateData(FALSE);	// have to initialize the widgets before calling OnMrgP4Radio()
	OnMrgP4Radio();

	// Load up the associated Merge list, making sure we get
	// a non-cached set of registry entries
	GET_P4REGPTR()->ReadMergeAssocList();
	ReadMergeAssocList();
	OnP4AssAppsSelChange();

	// Update input fields
	UpdateData(FALSE);
	
	m_Inited = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMergeAppPage::Browse(CString &filename, LPCTSTR title)
{
	CFileDialog fDlg(TRUE, NULL, NULL,  
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NONETWORKBUTTON | OFN_PATHMUSTEXIST,
		LoadStringResource(IDS_HELPER_APPS_FILTER), this,
		MainFrame()->m_osVer.dwMajorVersion < 5 ? OPENFILENAME_SIZE_VERSION_400 : sizeof(OPENFILENAME), FALSE); 
	
	TCHAR buf[MAX_PATH];
	// Zero 1st char so commdlg knows we aren't providing a default filename
	buf[0]=_T('\0');  

    fDlg.m_ofn.lpstrFile= buf; 
	fDlg.m_ofn.nMaxFile= MAX_PATH; 

	// Set the dlg caption
	fDlg.m_ofn.lpstrTitle=title;
	// We dont need no stinking file title
	fDlg.m_ofn.lpstrFileTitle=NULL;
	
	// Set the initial directory
	CString initDir = _T("\\");
	int slashpos;
	if ((slashpos = filename.ReverseFind(_T('\\'))) != -1)
		initDir = filename.Left(slashpos);
	fDlg.m_ofn.lpstrInitialDir=initDir;

	fDlg.m_ofn.lpstrCustomFilter= NULL;

	// Set filter index
	fDlg.m_ofn.nFilterIndex=1;
	
	INT_PTR retcode=fDlg.DoModal();

	if(retcode == IDOK)
	{
		filename = fDlg.GetPathName();
	}
	else
	{
		CString errorTxt;
		if(retcode == IDCANCEL)  // an error
		{
			DWORD exError=CommDlgExtendedError();
			if(exError != 0)
				errorTxt.FormatMessage(IDS_EXTENDED_ERROR_n_IN_FILEDIALOG, exError);
		}
		else
		{
			DWORD error=GetLastError();
			if(error)
			{
				LPVOID lpMsgBuf;
				FormatMessage( 
					FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					FORMAT_MESSAGE_FROM_SYSTEM | 
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					error,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
					(LPTSTR) &lpMsgBuf,
					0,
					NULL 
				);
				errorTxt = (TCHAR *)lpMsgBuf;
			}
			else
				errorTxt=LoadStringResource(IDS_UNKNOWN_FILEDIALOG_ERROR);
		}
		if(errorTxt.GetLength() > 0)
			AfxMessageBox(errorTxt, MB_ICONSTOP);
	}
}

void CMergeAppPage::OnMergebrowse() 
{
	UpdateData();
	Browse(m_MergeUserApp, LoadStringResource(IDS_SELECT_MERGE_APPLICATION));
	UpdateData(FALSE);
}

void CMergeAppPage::OnMergeconsole() 
{
	m_MergeClose.EnableWindow( m_MergeConsole.GetCheck() );
}

void CMergeAppPage::OnMergeOptArgChk() 
{
	GetDlgItem(IDC_MERGEOPTARGS)->EnableWindow( m_MergeOptArgChk.GetCheck() );
}

void CMergeAppPage::OnKillfocusArgs()
{
	UpdateData(TRUE);
	if (!m_Closing
	 && (m_MergeOptArgs.Find(_T("%1")) == -1 || m_MergeOptArgs.Find(_T("%2")) == -1
	  || m_MergeOptArgs.Find(_T("%3")) == -1 || m_MergeOptArgs.Find(_T("%4")) == -1))
	{
		AfxMessageBox(IDS_INVALID_USERMERGE_ARGS, MB_ICONSTOP);
		GotoDlgCtrl(GetDlgItem(IDC_MERGEOPTARGS));
	}
}

BOOL CMergeAppPage::ReadMergeAssocList() 
{
	// Clear the list box
	m_List.ResetContent();

	// Fetch a ptr to the association list
	CStringList *assocList= GET_P4REGPTR()->GetMergeAssocList();

	// Load the list box from the file entries
	POSITION pos= assocList->GetHeadPosition();
	while(pos != NULL)
	{
		CString entry= assocList->GetNext(pos);
		int x= entry.Find(_T('|'));
		if(x != -1)
			entry.SetAt(x,_T('\t'));
		m_List.AddString(entry);
	}

	return TRUE;
}

void CMergeAppPage::OnAdd() 
{
	CDiffAssocDlg dlg;
	dlg.m_EditMode=FALSE;
	dlg.m_bMerge=TRUE;
	if(dlg.DoModal() == IDOK)
		ReadMergeAssocList();
}

void CMergeAppPage::OnDelete() 
{
	CString entry;

	// Delete all selected items
	int n= m_List.GetSelCount();
	if(n)
	{
		int *selSet= new int[n];
		int c=m_List.GetSelItems( n, selSet );
		for(int i=0; i<c; i++)
		{
			m_List.GetText(selSet[i], entry);
			int x=entry.Find('\t');
			if(x != -1)
			{
				entry=entry.Left(x);
				GET_P4REGPTR()->ClearMergeAssociation(entry);
			}
			else 
				ASSERT(0);
		}
		delete [] selSet;
		ReadMergeAssocList();
	}
}

void CMergeAppPage::OnEdit() 
{
	CString entry, extension, application;

	// Get the one and only one selection
	// TODO: add support for adding multiple items at once?
	int n= m_List.GetSelCount();
	if(n==1)
	{
		int *selSet= new int[n];
		int c=m_List.GetSelItems( n, selSet );
		for(int i=0; i<c; i++)
		{
			m_List.GetText(selSet[i], entry);
			int x=entry.Find(_T('\t'));
			if(x != -1)
			{
				extension=_T(".");
				extension+=entry.Left(x);
				application=entry.Mid(x+1);
			}
			else 
				ASSERT(0);
		}
		delete [] selSet;
	}

	CDiffAssocDlg dlg;
	dlg.m_Application=application;
	dlg.m_Extension=extension;
	dlg.m_EditMode=TRUE;
	dlg.m_bMerge=TRUE;

	if(dlg.DoModal() == IDOK)
		ReadMergeAssocList();	
}

void CMergeAppPage::OnP4AssAppsSelChange() 
{
	// Enable buttons according to number of selections
	m_Edit.EnableWindow(m_List.GetSelCount() == 1 && ::GetFocus() == m_List.m_hWnd);
	m_Delete.EnableWindow(m_List.GetSelCount() > 0 && ::GetFocus() == m_List.m_hWnd);
}

BOOL CMergeAppPage::OK2Cancel() 
{
	m_Closing = TRUE;
	return TRUE;
}
