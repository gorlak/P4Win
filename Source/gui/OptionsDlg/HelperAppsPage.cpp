//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// HelperAppsPage.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "MainFrm.h"
#include "HelperAppsPage.h"
#include "DiffAssocDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHelperAppsPage property page

IMPLEMENT_DYNCREATE(CHelperAppsPage, CPropertyPage)

CHelperAppsPage::CHelperAppsPage() : CPropertyPage(CHelperAppsPage::IDD)
{
	//{{AFX_DATA_INIT(CHelperAppsPage)
	m_DiffUserApp = _T("");
	//}}AFX_DATA_INIT
	m_Inited = m_Closing = FALSE;
}

CHelperAppsPage::~CHelperAppsPage()
{
}

void CHelperAppsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHelperAppsPage)
	DDX_Control(pDX, IDC_DIFFCLOSE, m_DiffClose);
	DDX_Control(pDX, IDC_DIFFBINARY, m_DiffBinary);
	DDX_Control(pDX, IDC_DIFFMODAL, m_DiffModal);
	DDX_Control(pDX, IDC_DIFFBROWSE, m_DiffBrowse);
	DDX_Control(pDX, IDC_DIFFCONSOLE, m_DiffConsole);
	DDX_Control(pDX, IDC_DIFF_P4_RAD, m_P4DiffRadio);
	DDX_Control(pDX, IDC_DIFF_OTHER_RAD, m_OtherDiffRadio);
	DDX_Control(pDX, IDC_DIFF_P4DIFF_RAD, m_WinDiffRadio);
	DDX_Radio(pDX, IDC_WHTSP_LINEEND, m_P4DiffWhtSp);
	DDX_Text(pDX, IDC_DIFF_USERAPP, m_DiffUserApp);
	DDX_Text(pDX, IDC_TABWIDTH, m_TabSize);
	DDX_Control(pDX, IDC_DIFFOPTARGCHK, m_DiffOptArgChk);
	DDX_Text(pDX, IDC_DIFFOPTARGS, m_DiffOptArgs);
	DDX_Control(pDX, IDC_P4ASSAPPS, m_List);
	DDX_Control(pDX, IDC_EDAPP, m_Edit);
	DDX_Control(pDX, IDC_DELAPP, m_Delete);
	DDX_Control(pDX, IDC_ADDAPP, m_Add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHelperAppsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CHelperAppsPage)
	ON_BN_CLICKED(IDC_DIFF_P4_RAD, OnDiffP4Radio)
	ON_BN_CLICKED(IDC_DIFF_P4DIFF_RAD, OnDiffP4Radio)
	ON_BN_CLICKED(IDC_DIFF_OTHER_RAD, OnDiffP4Radio)
	ON_BN_CLICKED(IDC_DIFFBROWSE, OnDiffbrowse)
	ON_BN_CLICKED(IDC_DIFFCONSOLE, OnDiffconsole)
	ON_BN_CLICKED(IDC_DIFFOPTARGCHK, OnDiffOptArgChk)
	ON_BN_CLICKED(IDC_ADDAPP, OnAdd)
	ON_BN_CLICKED(IDC_DELAPP, OnDelete)
	ON_BN_CLICKED(IDC_EDAPP, OnEdit)
	ON_LBN_SELCHANGE(IDC_P4ASSAPPS, OnP4AssAppsSelChange)
	ON_EN_KILLFOCUS(IDC_DIFFOPTARGS, OnKillfocusArgs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHelperAppsPage message handlers

void CHelperAppsPage::OnDiffP4Radio() 
{
	UpdateData();
	CWnd *pwnd1=GetDlgItem(IDC_DIFF_USERAPP);
	CWnd *pwnd2=GetDlgItem(IDC_DIFFCONSOLE);
	CWnd *pwnd3=GetDlgItem(IDC_DIFFBROWSE);
	CWnd *pwnd4=GetDlgItem(IDC_TABWIDTH_CAPTION);
	CWnd *pwnd6=GetDlgItem(IDC_TABWIDTH);
	CWnd *pwnd7=GetDlgItem(IDC_DIFFCLOSE);
	CWnd *pwnd8=GetDlgItem(IDC_WHTSP_LINEEND);
	CWnd *pwnd9=GetDlgItem(IDC_WHTSP_INCLUDE);
	CWnd *pwnd10=GetDlgItem(IDC_WHTSP_IGNOREDIFFS);
	CWnd *pwnd11=GetDlgItem(IDC_WHTSP_IGNOREALL);
	CWnd *pwnd12=GetDlgItem(IDC_WHTSP_PREFS);
	CWnd *pwnd13=GetDlgItem(IDC_DIFFOPTARGCHK);
	CWnd *pwnd14=GetDlgItem(IDC_DIFFOPTARGS);

	if(m_OtherDiffRadio.GetCheck()==1)		// Other diff
	{
		pwnd1->EnableWindow(TRUE);
		pwnd2->EnableWindow(TRUE);
		pwnd3->EnableWindow(TRUE);
		pwnd4->EnableWindow(FALSE);
		pwnd6->EnableWindow(FALSE);
		pwnd8->EnableWindow(FALSE);
		pwnd9->EnableWindow(FALSE);
		pwnd10->EnableWindow(FALSE);
		pwnd11->EnableWindow(FALSE);
		pwnd12->EnableWindow(FALSE);
		pwnd7->EnableWindow(m_DiffConsole.GetCheck());
		pwnd13->EnableWindow(TRUE);
		pwnd14->EnableWindow(m_DiffOptArgChk.GetCheck());
	}
	else	// P4Merge & P4WinDiff apps
	{
		pwnd1->EnableWindow(FALSE);
		pwnd2->EnableWindow(FALSE);
		pwnd3->EnableWindow(FALSE);
		pwnd4->EnableWindow(TRUE);
		pwnd6->EnableWindow(TRUE);
		pwnd8->EnableWindow(TRUE);
		pwnd10->EnableWindow(TRUE);
		pwnd11->EnableWindow(TRUE);
		if (m_WinDiffRadio.GetCheck() && m_P4DiffWhtSp > 2)
		{
			m_P4DiffWhtSp = 0;
			UpdateData(FALSE);
		}
		pwnd9->EnableWindow(m_P4DiffRadio.GetCheck());
		pwnd12->EnableWindow(m_P4DiffRadio.GetCheck());
		pwnd7->EnableWindow(FALSE);
		pwnd13->EnableWindow(FALSE);
		pwnd14->EnableWindow(FALSE);
	}
}

void CHelperAppsPage::OnOK() 
{
	if (!m_Inited)
		return;

	// This bit of silliness is required to ensure all the 
	// fields provide their data to the UpdateData() routine
	GotoDlgCtrl(GetParent()->GetDlgItem(IDC_APPLY));

	int errors=0;

	if(UpdateData(TRUE))
	{

		// DIFF Application
		int useInternal=1;
		if(m_OtherDiffRadio.GetCheck() == 1)
			useInternal=0;
		else if (m_WinDiffRadio.GetCheck() == 1)
			useInternal=2;
		if( useInternal != GET_P4REGPTR()->GetDiffInternal() )
			if(!GET_P4REGPTR()->SetDiffInternal(useInternal))
				errors++;
		
		if( m_TabSize != GET_P4REGPTR()->GetTabWidth() )
			if(!GET_P4REGPTR()->SetTabWidth( m_TabSize) )
				errors++;

		if( m_P4DiffWhtSp != GET_P4REGPTR()->GetWhtSpFlag() )
			if(!GET_P4REGPTR()->SetWhtSpFlag( m_P4DiffWhtSp) )
				errors++;

		BOOL diffConsole=TRUE;
		if(m_DiffConsole.GetCheck() == 0)
			diffConsole=FALSE;
		if( diffConsole != GET_P4REGPTR()->GetDiffAppIsConsole() )
			if(!GET_P4REGPTR()->SetDiffAppIsConsole( diffConsole) )
				errors++;

		BOOL diffClose=TRUE;
		if(m_DiffClose.GetCheck() == 0)
			diffClose=FALSE;
		if( diffClose != GET_P4REGPTR()->GetDiffAppIsClose() )
			if(!GET_P4REGPTR()->SetDiffAppIsClose( diffClose) )
				errors++;

		BOOL diffBinary=TRUE;
		if(m_DiffBinary.GetCheck() == 0)
			diffBinary=FALSE;
		if( diffBinary != GET_P4REGPTR()->GetDiffAppIsBinary() )
			if(!GET_P4REGPTR()->SetDiffAppIsBinary( diffBinary) )
				errors++;

		BOOL diffModal=TRUE;
		if(m_DiffModal.GetCheck() == 0)
			diffModal=FALSE;
		if( diffModal != GET_P4REGPTR()->GetDontThreadDiffs() )
			if(!GET_P4REGPTR()->SetDontThreadDiffs( diffModal) )
				errors++;

		m_DiffUserApp.TrimRight();
		m_DiffUserApp.TrimLeft();
		if( m_DiffUserApp != GET_P4REGPTR()->GetDiffApp() )
			if(!GET_P4REGPTR()->SetDiffApp( m_DiffUserApp) )
				errors++;

		BOOL diffOptArgChk=TRUE;
		if(m_DiffOptArgChk.GetCheck() == 0)
			diffOptArgChk=FALSE;
		if( diffOptArgChk != GET_P4REGPTR()->GetDiffAppOptArgChk() )
			if(!GET_P4REGPTR()->SetDiffOptArgChk( diffOptArgChk) )
				errors++;

		if( m_DiffOptArgs != GET_P4REGPTR()->GetDiffOptArgs() )
		{
			if (m_DiffOptArgs.Find(_T("%1")) == -1 || m_DiffOptArgs.Find(_T("%2")) == -1)
				m_DiffOptArgs = GET_P4REGPTR()->GetDiffOptArgs();
			else if (!GET_P4REGPTR()->SetDiffOptArgs( m_DiffOptArgs) )
				errors++;
		}

		if(errors)
			AfxMessageBox(IDS_BAD_REGISTRY, MB_ICONSTOP);

	}		
}

BOOL CHelperAppsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// Init all data members from the registry
	m_TabSize = GET_P4REGPTR()->GetTabWidth();
	m_DiffOptArgs = GET_P4REGPTR()->GetDiffOptArgs();
	m_P4DiffWhtSp = GET_P4REGPTR()->GetWhtSpFlag();
	m_DiffUserApp = GET_P4REGPTR()->GetDiffApp();

	int iInternal = GET_P4REGPTR()->GetDiffInternal();
	if (!TheApp()->m_P4Merge)
	{
		if (iInternal == 2)
			iInternal = 1;
		CWnd *pwnd = GetDlgItem(IDC_DIFF_P4DIFF_RAD);
		pwnd->EnableWindow(FALSE);
		pwnd->ShowWindow(SW_HIDE);
	}
	switch(iInternal)
	{
	case 0:
		m_OtherDiffRadio.SetCheck(1);
		break;
	default:
	case 1:
		m_P4DiffRadio.SetCheck(1);
		break;
	case 2:
		m_WinDiffRadio.SetCheck(1);
		break;
	}

	m_DiffConsole.SetCheck( GET_P4REGPTR()->GetDiffAppIsConsole() );
	m_DiffClose.SetCheck( GET_P4REGPTR()->GetDiffAppIsClose() );
	m_DiffBinary.SetCheck( GET_P4REGPTR()->GetDiffAppIsBinary() );
	m_DiffModal.SetCheck( GET_P4REGPTR()->GetDontThreadDiffs() );
	m_DiffOptArgChk.SetCheck( GET_P4REGPTR()->GetDiffAppOptArgChk() );

	// Enable/disable diff widgets per radio button
	UpdateData(FALSE);	// have to initialize the widgets before calling OnDiffP4Radio()
	OnDiffP4Radio();

	// Initialize the per extension diff fields
	m_List.SetTabStops(40);
	
	// Load up the associated diff list, making sure we get
	// a non-cached set of registry entries
	GET_P4REGPTR()->ReadDiffAssocList();
	ReadDiffAssocList();
	OnP4AssAppsSelChange();

	// Update input fields
	UpdateData(FALSE);
	
	m_Inited = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHelperAppsPage::Browse(CString &filename, LPCTSTR title)
{
	CFileDialog fDlg(TRUE, NULL, NULL,  
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NONETWORKBUTTON | OFN_PATHMUSTEXIST,
		LoadStringResource(IDS_HELPER_APPS_FILTER),
		this,
		MainFrame()->m_osVer.dwMajorVersion < 5 ? OPENFILENAME_SIZE_VERSION_400 : sizeof(OPENFILENAME), FALSE); 
	
	TCHAR buf[MAX_PATH+1];
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


void CHelperAppsPage::OnDiffbrowse() 
{
	UpdateData();
	Browse(m_DiffUserApp, LoadStringResource(IDS_SELECT_DIFF_APPLICATION));
	UpdateData(FALSE);
}

void CHelperAppsPage::OnDiffconsole() 
{
	m_DiffClose.EnableWindow( m_DiffConsole.GetCheck() );
}

void CHelperAppsPage::OnDiffOptArgChk() 
{
	GetDlgItem(IDC_DIFFOPTARGS)->EnableWindow( m_DiffOptArgChk.GetCheck() );
}

BOOL CHelperAppsPage::ReadDiffAssocList() 
{
	// Clear the list box
	m_List.ResetContent();

	// Fetch a ptr to the association list
	CStringList *assocList= GET_P4REGPTR()->GetDiffAssocList();

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

void CHelperAppsPage::OnAdd() 
{
	CDiffAssocDlg dlg;
	dlg.m_EditMode=FALSE;
	if(dlg.DoModal() == IDOK)
		ReadDiffAssocList();
}

void CHelperAppsPage::OnDelete() 
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
				GET_P4REGPTR()->ClearDiffAssociation(entry);
			}
			else 
				ASSERT(0);
		}
		delete [] selSet;
		ReadDiffAssocList();
	}
}

void CHelperAppsPage::OnEdit() 
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

	if(dlg.DoModal() == IDOK)
		ReadDiffAssocList();	
}

void CHelperAppsPage::OnP4AssAppsSelChange() 
{
	// Enable buttons according to number of selections
	m_Edit.EnableWindow(m_List.GetSelCount() == 1 && ::GetFocus() == m_List.m_hWnd);
	m_Delete.EnableWindow(m_List.GetSelCount() > 0 && ::GetFocus() == m_List.m_hWnd);
}

void CHelperAppsPage::OnKillfocusArgs()
{
	UpdateData(TRUE);
	if (!m_Closing
	 && (m_DiffOptArgs.Find(_T("%1")) == -1 || m_DiffOptArgs.Find(_T("%2")) == -1))
	{
		AfxMessageBox(IDS_INVALID_USERDIFF_ARGS, MB_ICONSTOP);
		GotoDlgCtrl(GetDlgItem(IDC_DIFFOPTARGS));
	}
}

BOOL CHelperAppsPage::OK2Cancel() 
{
	m_Closing = TRUE;
	return TRUE;
}
