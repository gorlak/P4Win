//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// EditorsPage.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "MainFrm.h"
#include "EditorsPage.h"
#include "EditAssocDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditorsPage property page

IMPLEMENT_DYNCREATE(CEditorsPage, CPropertyPage)

CEditorsPage::CEditorsPage() : CPropertyPage(CEditorsPage::IDD)
{
	//{{AFX_DATA_INIT(CEditorsPage)
	m_DefaultEditApp = _T("");
	m_UseOpenForEditExts = _T("");
	m_UseTempForView = -1;
	m_UseTempForExts = _T("");
	//}}AFX_DATA_INIT
	m_Inited = FALSE;
}

CEditorsPage::~CEditorsPage()
{
}

void CEditorsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditorsPage)
	DDX_Control(pDX, IDC_BROWSE, m_Browse);
	DDX_Control(pDX, IDC_P4ASSAPPS, m_List);
	DDX_Control(pDX, IDC_EDAPP, m_Edit);
	DDX_Control(pDX, IDC_DELAPP, m_Delete);
	DDX_Control(pDX, IDC_ADDAPP, m_Add);
	DDX_Control(pDX, IDC_EDITCONSOLE, m_EditConsole);
	DDX_Control(pDX, IDC_IGNORE_ASSAPPS, m_IgnoreAssoc);
	DDX_Text(pDX, IDC_P4EDITOR, m_DefaultEditApp);
	DDV_MaxChars(pDX, m_DefaultEditApp, 255);
	DDX_Control(pDX, IDC_EDITUSEVIEW, m_UseOpenForEdit);
	DDX_Text(pDX, IDC_EDITUSEVIEWEXTS, m_UseOpenForEditExts);
	DDX_Radio(pDX, IDC_VIEWUSINGLIVE, m_UseTempForView);
	DDX_Text(pDX, IDC_USETEMPFORTHESE, m_UseTempForExts);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditorsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CEditorsPage)
	ON_BN_CLICKED(IDC_ADDAPP, OnAdd)
	ON_BN_CLICKED(IDC_DELAPP, OnDelete)
	ON_BN_CLICKED(IDC_EDAPP, OnEdit)
	ON_LBN_SELCHANGE(IDC_P4ASSAPPS, OnP4AssAppsSelChange)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_EDITUSEVIEW, OnEditUseView)
	ON_BN_CLICKED(IDC_VIEWUSINGLIVE, OnUseTempClick)
	ON_BN_CLICKED(IDC_VIEWUSINGTEMP, OnUseTempClick)
	ON_BN_CLICKED(IDC_VIEWUSINGEXT, OnUseTempClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditorsPage message handlers

void CEditorsPage::OnOK() 
{
	if (!m_Inited)
		return;

	// This bit of silliness is required to ensure all the 
	// fields provide their data to the UpdateData() routine
	GotoDlgCtrl(GetParent()->GetDlgItem(IDC_APPLY));

	int errors=0;

	if(UpdateData(TRUE))
	{
		BOOL ignoreAssoc=TRUE;
		if(m_IgnoreAssoc.GetCheck() == 0)
			ignoreAssoc=FALSE;
		if(ignoreAssoc != GET_P4REGPTR()->GetIgnoreWinAssoc())
			if(!GET_P4REGPTR()->SetIgnoreWinAssoc( ignoreAssoc ) )
				errors++;

		m_DefaultEditApp.TrimLeft( );
		m_DefaultEditApp.TrimRight( );
		if(m_DefaultEditApp != GET_P4REGPTR()->GetEditApp())
			if(!GET_P4REGPTR()->SetEditApp( m_DefaultEditApp ) )
				errors++;

		BOOL editConsole=TRUE;
		if(m_EditConsole.GetCheck() == 0)
			editConsole=FALSE;
		if(editConsole != GET_P4REGPTR()->GetEditAppIsConsole())
			if(!GET_P4REGPTR()->SetEditAppIsConsole( editConsole ) )
				errors++;

		BOOL b = m_UseOpenForEdit.GetCheck();
		if (b != GET_P4REGPTR()->GetUseOpenForEdit())
			if(!GET_P4REGPTR()->SetUseOpenForEdit(b))
				errors++;

		m_UseOpenForEditExts.MakeLower();
		while (m_UseOpenForEditExts.Replace(_T(", "), _T(",")))	;
		while (m_UseOpenForEditExts.Replace(_T(" ,"), _T(","))) ;
		if( m_UseOpenForEditExts != GET_P4REGPTR()->GetUseOpenForEditExts() )
			if(!GET_P4REGPTR()->SetUseOpenForEditExts(m_UseOpenForEditExts))
				errors++;
		
		if( m_UseTempForView != GET_P4REGPTR()->GetUseTempForView() )
			if(!GET_P4REGPTR()->SetUseTempForView(m_UseTempForView))
				errors++;

		m_UseTempForExts.TrimLeft(_T(" \t."));
		m_UseTempForExts.TrimRight(_T(" \t,"));
		while (m_UseTempForExts.Find(_T(", ")) != -1)
			m_UseTempForExts.Replace(_T(", "), _T(","));
		while (m_UseTempForExts.Find(_T(",.")) != -1)
			m_UseTempForExts.Replace(_T(",."), _T(","));
		if(m_UseTempForExts != GET_P4REGPTR()->GetEditApp())
			if(!GET_P4REGPTR()->SetUseTempForExts( m_UseTempForExts ) )
				errors++;

		if(errors)
			AfxMessageBox(IDS_BAD_REGISTRY, MB_ICONSTOP);

	}		
}

BOOL CEditorsPage::ReadAssociationList() 
{
	// Clear the list box
	m_List.ResetContent();

	// Fetch a ptr to the association list
	CStringList *assocList= GET_P4REGPTR()->GetAssociationList();

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


BOOL CEditorsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_List.SetTabStops(40);
	
	// Load up the associated viewer list, making sure we get
	// a non-cached set of registry entries
	GET_P4REGPTR()->ReadAssociationList();
	ReadAssociationList();
	
	// Init all data members from the registry
	m_IgnoreAssoc.SetCheck(GET_P4REGPTR()->GetIgnoreWinAssoc()); 

	m_DefaultEditApp = GET_P4REGPTR()->GetEditApp();
	m_EditConsole.SetCheck(GET_P4REGPTR()->GetEditAppIsConsole()); 

	m_UseOpenForEdit.SetCheck(GET_P4REGPTR()->GetUseOpenForEdit());
	m_UseOpenForEditExts = GET_P4REGPTR()->GetUseOpenForEditExts();

	m_UseTempForView = GET_P4REGPTR()->GetUseTempForView();
	m_UseTempForExts = GET_P4REGPTR()->GetUseTempForExts();

	OnP4AssAppsSelChange();

	// Update input fields
	UpdateData(FALSE);
	
	OnUseTempClick();
	OnEditUseView();
	m_Inited = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditorsPage::OnAdd() 
{
	CEditAssocDlg dlg;
	dlg.m_EditMode=FALSE;
	if(dlg.DoModal() == IDOK)
		ReadAssociationList();
}

void CEditorsPage::OnDelete() 
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
				GET_P4REGPTR()->ClearAssociation(entry);
			}
			else 
				ASSERT(0);
		}
		delete [] selSet;
		ReadAssociationList();
	}
}

void CEditorsPage::OnEdit() 
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

	CEditAssocDlg dlg;
	dlg.m_Application=application;
	dlg.m_Extension=extension;
	dlg.m_EditMode=TRUE;

	if(dlg.DoModal() == IDOK)
		ReadAssociationList();	
	
}

void CEditorsPage::OnP4AssAppsSelChange() 
{
	// Enable buttons according to number of selections
	m_Edit.EnableWindow(m_List.GetSelCount() == 1 && ::GetFocus() == m_List.m_hWnd);
	m_Delete.EnableWindow(m_List.GetSelCount() > 0 && ::GetFocus() == m_List.m_hWnd);
}

void CEditorsPage::OnBrowse() 
{
	UpdateData();
	UpdateData(FALSE);
	CFileDialog fDlg(TRUE, NULL, NULL,  
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NONETWORKBUTTON | OFN_PATHMUSTEXIST,
		LoadStringResource(IDS_TEXT_EDITOR_FILTER), this,
		MainFrame()->m_osVer.dwMajorVersion < 5 ? OPENFILENAME_SIZE_VERSION_400 : sizeof(OPENFILENAME)); 
	
	TCHAR buf[MAX_PATH];
	// Zero 1st char so commdlg knows we aren't providing a default filename
	buf[0]=_T('\0');  

    fDlg.m_ofn.lpstrFile= buf; 
	fDlg.m_ofn.nMaxFile= MAX_PATH; 

	// Set the dlg caption
    CString title = LoadStringResource(IDS_SELECT_DEFAULT_TEXT_EDITOR_CAPTION);
	fDlg.m_ofn.lpstrTitle = title;
	// We dont need no stinking file title
	fDlg.m_ofn.lpstrFileTitle=NULL;
	
	// Set the initial directory
	CString initDir = _T("\\");
	int slashpos;
	if ((slashpos = m_DefaultEditApp.ReverseFind(_T('\\'))) != -1)
		initDir = m_DefaultEditApp.Left(slashpos);
	fDlg.m_ofn.lpstrInitialDir=initDir;

	fDlg.m_ofn.lpstrCustomFilter= NULL;

	// Set filter index
	fDlg.m_ofn.nFilterIndex=1;
	
	INT_PTR retcode=fDlg.DoModal();

	if(retcode == IDOK)
	{
		m_DefaultEditApp = fDlg.GetPathName();
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
	UpdateData(FALSE);
}

void CEditorsPage::OnUseTempClick()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_USETEMPFORTHESE)->EnableWindow( m_UseTempForView == 2 ? TRUE : FALSE );
}

void CEditorsPage::OnEditUseView()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_EDITUSEVIEWEXTS)->EnableWindow(m_UseOpenForEdit.GetCheck() ? TRUE : FALSE );
}