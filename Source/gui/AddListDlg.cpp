//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// AddListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "AddListDlg.h"
#include "AddFilterDlg.h"
#include "MainFrm.h"
#include "hlp\p4win.hh"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddListDlg dialog


CAddListDlg::CAddListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddListDlg)
	m_Action = m_Default = 0;
	//}}AFX_DATA_INIT
	m_Initialized = m_bEditAndAdd = FALSE;
	m_InitRect.SetRect(0,0,100,100);
	m_WinPos.SetWindow( this, _T("AddListDlg") );
	m_WarnLimit = _ttoi(GET_P4REGPTR()->GetWarnLimit());
	m_Need2Refresh = FALSE;
}


void CAddListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddListDlg)
	DDX_Control(pDX, IDC_ADDCHANGNUM, m_ChangeCombo);
	DDX_Control(pDX, ID_DELETESELECTED, m_Delete);
	DDX_Control(pDX, IDC_STATICRECT, m_StaticRect);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Radio(pDX, IDC_ADD, m_Action);
	DDX_Radio(pDX, IDC_DEFAULT1, m_Default);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddListDlg, CDialog)
	//{{AFX_MSG_MAP(CAddListDlg)
	ON_BN_CLICKED(ID_DELETESELECTED, OnDeleteselected)
	ON_LBN_SELCHANGE(IDC_THELIST, OnSelchangeList)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDC_FILTERFILES, OnFilterFiles)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

	
void CAddListDlg::Init(CStringList *strList, CStringList *changeList, 
					   LPCTSTR selChange, BOOL bEditAndAdd, int key)
{
	ASSERT_KINDOF(CStringList, strList);
	m_pStrList=strList;
	m_pChangeList=changeList;
	m_SelChange= selChange;
	m_bEditAndAdd= bEditAndAdd;
	m_Initialized=TRUE;
	m_DeletedRows=FALSE;
    m_UserTerminated=FALSE;
	m_Key=key;

	/////////////////////////////////
	// Enumerate the files before the dialog window is crafted
	EnumerateFiles();
}


/////////////////////////////////////////////////////////////////////////////
// CAddListDlg message handlers

BOOL CAddListDlg::OnInitDialog() 
{
	ASSERT(m_Initialized);

    if(m_UserTerminated)
        return FALSE;

	CDialog::OnInitDialog();
	// Record the initial window size, then see if there is a registry preference
	GetWindowRect(&m_InitRect);
	m_LastRect = m_InitRect;
	
	CRect rect;
	m_StaticRect.GetWindowRect(&rect);
	ScreenToClient(rect);
	DWORD style= WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | 
				 LBS_SORT | LBS_STANDARD | LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT;

	style |= LBS_EXTENDEDSEL;
		
	m_Delete.ShowWindow(SW_SHOWNOACTIVATE);
	m_Delete.EnableWindow(FALSE);


	/////////////////////////////
	// Fill in the change list, if present, and select the suggested
	// change number
	POSITION pos;

	if(m_pChangeList != NULL)
	{
		ASSERT(m_pChangeList->GetCount() > 0);
		
		for(pos=m_pChangeList->GetHeadPosition(); pos != NULL; )
		{
			m_ChangeCombo.AddString(m_pChangeList->GetNext(pos));
		}	
	}

	int index;
	ASSERT(!m_SelChange.IsEmpty());
	if( (index=m_ChangeCombo.FindString(-1, m_SelChange))==CB_ERR)
	{
		ASSERT(0);  //Why wasnt default value in list?
		index=0;
	}
	m_ChangeCombo.SetCurSel(index);

	if (m_bEditAndAdd)
	{
		if(m_EnumeratedList.GetCount()==1)
			m_Caption=LoadStringResource(IDS_CAPTIONADDONEFILE);
		else
			m_Caption.FormatMessage(IDS_CAPTIONADDSEVERALFILES_n, m_EnumeratedList.GetCount());
		GetDlgItem(IDC_STATIC1)->ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_ADD )->ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_EDIT)->ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_BOTH)->ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_DELETE)->ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_ADD )->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BOTH)->EnableWindow(TRUE);
		GetDlgItem(IDC_DELETE)->EnableWindow(TRUE);
#if 0	// Setting the default for these radio buttons
		// has been moved to Settings > Options > Files :-(
		GetDlgItem(IDC_DEFAULT1)->ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_DEFAULT2)->ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_DEFAULT3)->ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_DEFAULT4)->ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_DEFAULT1)->EnableWindow(TRUE);
		GetDlgItem(IDC_DEFAULT2)->EnableWindow(TRUE);
		GetDlgItem(IDC_DEFAULT3)->EnableWindow(TRUE);
		GetDlgItem(IDC_DEFAULT4)->EnableWindow(TRUE);
#endif
		m_Action = m_Default = GET_P4REGPTR()->GetDefaultDnDfromExp();
		m_OK.SetWindowText(LoadStringResource(IDS_OK));
	}
	else
	{
		if(m_EnumeratedList.GetCount()==1)
			m_Caption=LoadStringResource(IDS_FOUND_ONE_FILE_FOR_ADD);
		else
			m_Caption.FormatMessage(IDS_FOUND_n_FILES_FOR_ADD, m_EnumeratedList.GetCount());
	}
	SetWindowText(m_Caption);
		
	/////////////////////////////////
	// Fill the file list
	m_List.CreateEx(WS_EX_CLIENTEDGE, _T("ListBox"), _T(""),style,
                 rect.left, rect.top, rect.Width(), rect.Height(),
				 this->m_hWnd, (HMENU) IDC_THELIST);
	
	m_List.SetFont(GetFont());
	
	for(pos=m_EnumeratedList.GetHeadPosition(); pos != NULL; )
	{
		m_List.AddString(m_EnumeratedList.GetNext(pos));
	}

	UpdateData(FALSE);	
	m_WinPos.RestoreWindowPosition();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAddListDlg::OnDeleteselected() 
{
	for(int i=m_List.GetCount()-1; i>=0; i--)
	{
		if(m_List.GetSel(i))
		{
			m_List.DeleteString(i);
			m_DeletedRows=TRUE;
		}
	}
	m_Delete.EnableWindow(FALSE);
}

void CAddListDlg::OnSelchangeList() 
{
	if(m_List.GetSelCount() > 0)
		m_Delete.EnableWindow(TRUE);
	else
		m_Delete.EnableWindow(FALSE);
}

void CAddListDlg::OnOK() 
{
	UpdateData();
	m_WinPos.SaveWindowPosition();

	// some files were deleted from the listbox, so
    // rebuild the enumerated list from the listbox
	if(m_DeletedRows)
	{
		CString txt;
		m_EnumeratedList.RemoveAll();

		for(int i=0; i< m_List.GetCount(); i++)
		{
			m_List.GetText(i, txt);
			m_EnumeratedList.AddTail(txt);
		}
	}

    // determine which changelist to add files to
	int index=m_ChangeCombo.GetCurSel();
	CString txt;
	m_ChangeCombo.GetLBText(index, txt);
	if(txt.Compare(LoadStringResource(IDS_DEFAULTCHANGELISTNAME)) == 0)
		m_SelectedChange= 0;
	else if(txt.Compare(LoadStringResource(IDS_NEWCHANGELISTNAME)) == 0)
	{
		m_SelectedChange= MainFrame()->CreateNewChangeList(m_Key);
		if (m_SelectedChange == -1)	// the user bailed
			return;
		m_Need2Refresh = TRUE;
	}
	else
		m_SelectedChange=_ttol(txt);

	GET_P4REGPTR()->SetDefaultDnDfromExp(m_Default);
	
	CDialog::OnOK();
}


BOOL CAddListDlg::EnumerateFiles()
{
	m_AddFileCount=0;
	BOOL success=TRUE;

	// Recurse any directories in files list, placing enumerated files into 
	// the 'files' list
	POSITION pos=m_pStrList->GetHeadPosition();
	for( int i=0; pos != NULL && success; i++)
		success=AddPath(&m_EnumeratedList, m_pStrList->GetNext(pos));

	return success;
}


// Recursive routine to get all subdirs added
BOOL CAddListDlg::AddPath(CStringList *pList, LPCTSTR path)
{
	BOOL success=TRUE;
    	
	// Process any accumulated messages
	if (!MainFrame()->IsQuitting())
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				// App was just terminated
				return FALSE;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// Is path a directory or a file?
	WIN32_FIND_DATA fileInfo;
	HANDLE hFile=FindFirstFile(path, &fileInfo);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		success= FALSE;
		goto ReportErrors;
	}

	FindClose(hFile);
	
	if( (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
	{
		// Its a file, so just add the one file
		pList->AddHead(path);
		m_AddFileCount++;

        // Every m_WarnLimit, check with user to see if we are out of 
        // control.
		if(m_WarnLimit && (m_AddFileCount%m_WarnLimit) == 0 )
		{
            if(VerifyOKToContinue() != IDYES)
            {
			    success=FALSE;
                m_UserTerminated=TRUE;
			    goto ReportErrors;
            }
			if (m_WarnLimit < 10)
				m_WarnLimit = 5000;
        }
	}
	else
	{
		// Its a directory, so enumerate its contents
		CString subPath(path);
		subPath += _T("\\*.*");
		
		hFile=FindFirstFile(subPath, &fileInfo);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			success= FALSE;
			goto ReportErrors;
		}
		
		CString fullName;
		while(1)
		{
			if(lstrcmp(fileInfo.cFileName, _T(".")) !=0 && lstrcmp(fileInfo.cFileName, _T("..")) !=0)
			{
				fullName=path;
				fullName += _T("\\");
				fullName += fileInfo.cFileName;
				if( (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
				{
					// Add files directly
					pList->AddHead(fullName);
					m_AddFileCount++;
					if( (m_AddFileCount%m_WarnLimit) == 0 &&
                        VerifyOKToContinue() != IDYES )
					{	
						success=FALSE;
                        m_UserTerminated=TRUE;
						break;
					}
				}
				else
				{
					// Make a recursive call to AddPath for directories
					if(!AddPath(pList, fullName))
					{
						success=FALSE;
						break;
					}
				}
			}
			if( !FindNextFile(hFile, &fileInfo) )
				break;
		}	
		FindClose(hFile);
	}

ReportErrors:

	if(!success && !m_UserTerminated)
	{
		DWORD errNo=GetLastError();
		if(errNo != 0)  // Avoid showing same message too many times
		{
		    LPVOID lpMsgBuf;
			FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,  errNo, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf, 0, NULL );
			// Display the string.
			CString msg = (LPCTSTR)lpMsgBuf;
			if (errNo == 0x7B)
			{
				CString p = path;
				if (p.ReverseFind(_T(':')) != 1)
					msg += LoadStringResource(IDS_CANTADDLNKVIAFILEADD);
			}
			AfxMessageBox(msg + _T('\n') + path, MB_OK|MB_ICONERROR );

            // Free the buffer.
			LocalFree( lpMsgBuf );
			SetLastError(0);
		}
	}

	return success;
}

UINT CAddListDlg::VerifyOKToContinue()
{
    CString warning;
    if(m_AddFileCount == m_WarnLimit)
        warning.FormatMessage(IDS_THIS_FILE_ADD_OPERATION_HAS_ALREADY_FOUND_OVER_n, m_AddFileCount); 
    else
        warning.FormatMessage(IDS_THIS_FILE_ADD_OPERATION_HAS_NOW_ADDED_n, m_AddFileCount); 

    return AfxMessageBox(warning, MB_YESNO|MB_ICONEXCLAMATION);
}

INT_PTR CAddListDlg::DoModal()
{ 
    if(m_UserTerminated) 
        return IDCANCEL; 
    else 
        return CDialog::DoModal();
}

void CAddListDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	if (!m_List || !IsWindow(m_List.m_hWnd))
		return;

	// Compute the change in width (dx)
	CRect rect;
	GetWindowRect(&rect);
	int dx = rect.Width() - m_LastRect.Width();
	int dy = rect.Height() - m_LastRect.Height();
	// Save the new size
	m_LastRect = rect;

	// Widen the list and group boxes by the change in width (dx) and
	// heighten the list and group boxes by the change in height (dy)
	m_List.GetWindowRect(&rect);
	m_List.SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								    rect.bottom - rect.top + dy, SWP_NOMOVE | SWP_NOZORDER);
	CWnd *pWnd = GetDlgItem(IDC_STATIC3);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								   rect.bottom - rect.top + dy, SWP_NOMOVE | SWP_NOZORDER);

	// Slide other controls down by change in height
	pWnd = GetDlgItem(IDC_ADDPROMPT);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(IDC_ADDCHANGNUM);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(IDC_STATIC1);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(IDC_ADD);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(IDC_EDIT);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(IDC_BOTH);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(IDC_DELETE);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(IDC_DEFAULT1);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(IDC_DEFAULT2);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(IDC_DEFAULT3);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(IDC_DEFAULT4);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// Slide the buttons to the right by the change in width
	// and down by the change in height
	pWnd = GetDlgItem(IDC_FILTERFILES);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(ID_DELETESELECTED);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(IDOK);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top +dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(IDCANCEL);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top +dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(IDHELP);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	InvalidateRect(NULL, TRUE);
}

void CAddListDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	if (m_InitRect.Height())
	{
		lpMMI->ptMinTrackSize.x= m_InitRect.Width();
		lpMMI->ptMinTrackSize.y= m_InitRect.Height();
	}
}

void CAddListDlg::OnFilterFiles() 
{
	CAddFilterDlg dlg;
	CString itemtext;
	CString ext;
	BOOL bFound;
	POSITION pos;

	dlg.m_Extensions = GET_P4REGPTR()->GetAddFileExtFilter();
	if (dlg.DoModal( ) == IDOK)
	{
		GET_P4REGPTR()->SetAddFileExtFilter(dlg.m_Extensions);
		for (int i = m_List.GetCount(); i--; )
		{
			m_List.GetText( i, itemtext );
			for(bFound = FALSE, pos=dlg.m_Exts.GetHeadPosition(); pos != NULL; )
			{
				ext = '.' + dlg.m_Exts.GetNext(pos);
				if (!ext.CompareNoCase(itemtext.Right(ext.GetLength())))
				{
					bFound = TRUE;
					if (dlg.m_Exclude)
					{
						m_List.DeleteString(i);
						m_DeletedRows=TRUE;
					}
					break;
				}
			}
			if (!dlg.m_Exclude && !bFound)
			{
				m_List.DeleteString(i);
				m_DeletedRows=TRUE;
			}
		}

	};
}

void CAddListDlg::OnHelp() 
{
	AfxGetApp()->WinHelp(TASK_ADDING_FILES_TO_DEPOT);
}

BOOL CAddListDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}
