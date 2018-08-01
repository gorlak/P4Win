// bookmarks.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "Bookmarks.h"
#include "ToolsAdd.h"
#include "BookmarkAdd.h"
#include "MainFrm.h"
#include "hlp\p4win.hh"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBookmarks dialog


CBookmarks::CBookmarks(CWnd* pParent /*=NULL*/)
	: CDialog(CBookmarks::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBookmarks)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_CurSel = -1;
	m_NbrSubMenus = 0;
	m_1stSubmenu = MAX_BOOKMARKS+1;

	int i,j, k;
	for (i = j = -1; ++i < MAX_BOOKMARKS; )
	{
		m_BkMkMenuName[++j] = GET_P4REGPTR()->GetBkMkMenuName(i);
		if (m_BkMkMenuName[j].IsEmpty())
		{
			j--;
			continue;
		}
		m_NbrSubMenus += m_BkMkIsSubMenu[j] = GET_P4REGPTR()->GetBkMkIsSubMenu(i);
		if (m_BkMkIsSubMenu[j])
		{
			if (j < m_1stSubmenu)
				m_1stSubmenu = j;
		}
		else if ((k = m_BkMkMenuName[j].Find(_T('#'))) != -1)
			m_BkMkMenuName[j].SetAt(k, _T('\t'));
	}

	m_Canceling = FALSE;
}


void CBookmarks::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBookmarks)
	DDX_Control(pDX, IDC_MENULIST, m_MenuItemList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBookmarks, CDialog)
	//{{AFX_MSG_MAP(CBookmarks)
	ON_BN_CLICKED(IDC_ADD, OnAddSubmenu)
	ON_BN_CLICKED(IDC_CHANGE, OnChangeItem)
	ON_BN_CLICKED(IDC_MOVE_DOWN, OnMoveDown)
	ON_BN_CLICKED(IDC_MOVE_UP, OnMoveUp)
	ON_BN_CLICKED(IDC_IMPORT, OnImport)
	ON_BN_CLICKED(IDC_EXPORT, OnExport)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_LBN_SELCHANGE(IDC_MENULIST, OnSelchangeMenulist)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	//}}AFX_MSG_MAP
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBookmarks message handlers

BOOL CBookmarks::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	LoadMenuItemList();

	if (m_BkMkMenuName[0].GetLength())
		m_MenuItemList.SetCurSel(m_CurSel = 0);
	else
	{
		GetDlgItem(IDC_CHANGE)->EnableWindow(FALSE);
		GetDlgItem(IDC_REMOVE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EXPORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_MOVE_DOWN)->EnableWindow(FALSE);
	}

	GetDlgItem(IDC_MOVE_UP)->EnableWindow(FALSE);

	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBookmarks::LoadMenuItemList()
{
	while (m_MenuItemList.DeleteString(0) > 0)
		;

	for (int i = -1; m_BkMkMenuName[++i].GetLength(); )
	{
	    CString txt = m_BkMkMenuName[i];
		if (m_BkMkIsSubMenu[i])
			txt += LoadStringResource(IDS_A_SUBMENU);
		else if (i > m_1stSubmenu)
			txt = _T("    ") + txt;
		m_MenuItemList.AddString(txt);
		if (i >= MAX_BOOKMARKS-1)
		{
			GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
			break;
		}
	}
}

void CBookmarks::OnAddSubmenu() 
{
	if (m_NbrSubMenus < MAX_BOOKMARK_SUBMENUS)
	{
		CToolsAdd dlg;
		dlg.SetTitle(LoadStringResource(IDS_ADD_BOOKMARK_SUBMENU));
		dlg.SetRadioShow(3);
		if ((dlg.DoModal() == IDOK) && ((dlg.GetNewMenuName()).GetLength()))
		{
			UpdateData();
			CString txt = dlg.GetNewMenuName();
			if ((m_CurSel = m_MenuItemList.AddString(txt)) >= 0)
			{
				txt += LoadStringResource(IDS_A_SUBMENU);
				m_MenuItemList.DeleteString(m_CurSel);
				m_MenuItemList.InsertString(m_CurSel, txt);
				m_BkMkMenuName[m_CurSel] = dlg.GetNewMenuName();
				m_NbrSubMenus++;
				m_BkMkIsSubMenu[m_CurSel] = TRUE;
				if (m_CurSel < m_1stSubmenu)
					m_1stSubmenu = m_CurSel;
				UpdateData( FALSE );
				m_MenuItemList.SetCurSel(m_CurSel);
				if (m_CurSel >= MAX_BOOKMARKS-1)
					GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHANGE)->EnableWindow(TRUE);
				GetDlgItem(IDC_REMOVE)->EnableWindow(TRUE);
				GetDlgItem(IDC_EXPORT)->EnableWindow(TRUE);
				UpdateData( FALSE );
			}
		}
	}
}

void CBookmarks::OnChangeItem() 
{
	if (m_CurSel == -1)
	{
		MessageBeep(0);
		return;
	}
	if (m_BkMkIsSubMenu[m_CurSel])
		OnChangeSubmenu();
	else
		OnChangeBkMkItem();
}

void CBookmarks::OnChangeSubmenu() 
{
	if (m_CurSel == -1)
	{
		MessageBeep(0);
		return;
	}
	CToolsAdd dlg;
	dlg.SetTitle(LoadStringResource(IDS_EDIT_BOOKMARK_SUBMENU));
	dlg.SetNewMenuName(m_BkMkMenuName[m_CurSel]);
	dlg.SetIsSubMenu(TRUE);
	dlg.SetRadioShow(3);
	if ((m_NbrSubMenus < MAX_BOOKMARK_SUBMENUS) || m_BkMkIsSubMenu[m_CurSel])
		dlg.SetCanCr8SubMenu(TRUE);
	if ((dlg.DoModal() == IDOK) && ((dlg.GetNewMenuName()).GetLength()))
	{
		m_BkMkMenuName[m_CurSel] = dlg.GetNewMenuName();
		CString txt = dlg.GetNewMenuName() + LoadStringResource(IDS_A_SUBMENU);
		m_MenuItemList.DeleteString(m_CurSel);
		m_MenuItemList.InsertString(m_CurSel, txt);
		if (m_CurSel < m_1stSubmenu)
		{
			m_1stSubmenu = m_CurSel;
			LoadMenuItemList();
		}
		UpdateData( FALSE );
		m_MenuItemList.SetCurSel(m_CurSel);
	}
}

void CBookmarks::OnChangeBkMkItem() 
{
	int i, k;

	if (m_CurSel == -1)
	{
		MessageBeep(0);
		return;
	}
	CBookMarkAdd dlg;
	dlg.SetTitle(LoadStringResource(IDS_EDIT_BOOKMARK));
	dlg.SetLabelText(LoadStringResource(IDS_PATH));
	CString txt = m_BkMkMenuName[m_CurSel];
	if ((k = txt.Find(_T('\t'))) != -1)
		txt.SetAt(k, _T('#'));
	dlg.SetNewMenuName(txt);
	dlg.SetIsSubMenu(FALSE);
	dlg.SetRadioShow(3);
	dlg.SetCanCr8SubMenu(FALSE);
	if ((dlg.DoModal() == IDOK) && ((dlg.GetNewMenuName()).GetLength()))
	{
		m_BkMkMenuName[m_CurSel] = dlg.GetNewMenuName();
		txt = dlg.GetNewMenuName();
		if ((k = txt.Find(_T('#'))) != -1)
			txt.SetAt(k, _T('\t'));
		if (m_CurSel > m_1stSubmenu)
			txt = _T("    ") + txt;
		m_MenuItemList.DeleteString(m_CurSel);
		m_MenuItemList.InsertString(m_CurSel, txt);
		if (m_1stSubmenu == m_CurSel)
		{
			m_1stSubmenu = MAX_BOOKMARKS+1;
			for (i = m_CurSel; ++i < MAX_BOOKMARKS; )
			{
				if (m_BkMkIsSubMenu[i])
				{
					m_1stSubmenu = i;
					break;
				}
			}
			LoadMenuItemList();
		}
		UpdateData( FALSE );
		m_MenuItemList.SetCurSel(m_CurSel);
	}
}

void CBookmarks::OnMoveDown() 
{
	if (m_CurSel >= 0 && (m_CurSel < (m_MenuItemList.GetCount()-1)))
	{
		UpdateData();
		CString bkmkMenuName = m_BkMkMenuName[m_CurSel+1];
		BOOL bkmkIsSubMenu = m_BkMkIsSubMenu[m_CurSel+1];

		m_BkMkMenuName[m_CurSel+1] = m_BkMkMenuName[m_CurSel];
		m_BkMkIsSubMenu[m_CurSel+1] = m_BkMkIsSubMenu[m_CurSel];

		m_BkMkMenuName[m_CurSel] = bkmkMenuName;
		m_BkMkIsSubMenu[m_CurSel] = bkmkIsSubMenu;

		if (m_BkMkIsSubMenu[m_CurSel] && (m_CurSel < m_1stSubmenu))
			m_1stSubmenu = m_CurSel;
		else if (m_1stSubmenu < MAX_BOOKMARKS+1 && !m_BkMkIsSubMenu[m_1stSubmenu])
			m_1stSubmenu = m_CurSel+1;

		m_MenuItemList.DeleteString(m_CurSel);
		CString txt = m_BkMkMenuName[m_CurSel];
		if (m_BkMkIsSubMenu[m_CurSel])
			txt += LoadStringResource(IDS_A_SUBMENU);
		else if (m_CurSel > m_1stSubmenu)
			txt = _T("    ") + txt;
		m_MenuItemList.InsertString(m_CurSel, txt);

		m_CurSel++;
		m_MenuItemList.DeleteString(m_CurSel);
		txt = m_BkMkMenuName[m_CurSel];
		if (m_BkMkIsSubMenu[m_CurSel])
			txt += LoadStringResource(IDS_A_SUBMENU);
		else if (m_CurSel > m_1stSubmenu)
			txt = _T("    ") + txt;
		m_MenuItemList.InsertString(m_CurSel, txt);

		UpdateData( FALSE );
		GotoDlgCtrl(GetDlgItem(IDC_MENULIST));
		m_MenuItemList.SetCurSel(m_CurSel);
		GetDlgItem(IDC_MOVE_UP)->EnableWindow(m_CurSel > 0 ? TRUE : FALSE);
		GetDlgItem(IDC_MOVE_DOWN)->EnableWindow((m_CurSel < (m_MenuItemList.GetCount()-1)) ? TRUE : FALSE);
	}
	else MessageBeep(0);
}

void CBookmarks::OnMoveUp() 
{
	if (m_CurSel > 0)
	{
		UpdateData();
		CString bkmkMenuName = m_BkMkMenuName[m_CurSel-1];
		BOOL bkmkIsSubMenu = m_BkMkIsSubMenu[m_CurSel-1];

		m_BkMkMenuName[m_CurSel-1] = m_BkMkMenuName[m_CurSel];
		m_BkMkIsSubMenu[m_CurSel-1] = m_BkMkIsSubMenu[m_CurSel];

		m_BkMkMenuName[m_CurSel] = bkmkMenuName;
		m_BkMkIsSubMenu[m_CurSel] = bkmkIsSubMenu;

		if (m_BkMkIsSubMenu[m_CurSel-1] && (m_CurSel-1 < m_1stSubmenu))
			m_1stSubmenu = m_CurSel-1;
		else if (m_1stSubmenu < MAX_BOOKMARKS+1 && !m_BkMkIsSubMenu[m_1stSubmenu])
			m_1stSubmenu = m_CurSel;

		m_MenuItemList.DeleteString(m_CurSel);
		CString txt = m_BkMkMenuName[m_CurSel];
		if (m_BkMkIsSubMenu[m_CurSel])
			txt += LoadStringResource(IDS_A_SUBMENU);
		else if (m_CurSel > m_1stSubmenu)
			txt = _T("    ") + txt;
		m_MenuItemList.InsertString(m_CurSel, txt);

		m_CurSel--;
		m_MenuItemList.DeleteString(m_CurSel);
		txt = m_BkMkMenuName[m_CurSel];
		if (m_BkMkIsSubMenu[m_CurSel])
			txt += LoadStringResource(IDS_A_SUBMENU);
		else if (m_CurSel > m_1stSubmenu)
			txt = _T("    ") + txt;
		m_MenuItemList.InsertString(m_CurSel, txt);

		UpdateData( FALSE );
		GotoDlgCtrl(GetDlgItem(IDC_MENULIST));
		m_MenuItemList.SetCurSel(m_CurSel);
		GetDlgItem(IDC_MOVE_UP)->EnableWindow(m_CurSel > 0 ? TRUE : FALSE);
		GetDlgItem(IDC_MOVE_DOWN)->EnableWindow((m_CurSel < (m_MenuItemList.GetCount()-1)) ? TRUE : FALSE);
	}
	else MessageBeep(0);
}

void CBookmarks::OnRemove() 
{
	if (m_CurSel >= 0)
	{
		int old1stSubmenu = m_1stSubmenu;
		UpdateData( );
		if (m_BkMkIsSubMenu[m_CurSel])
		{
			--m_NbrSubMenus;
			GetDlgItem(IDC_ADD)->EnableWindow(TRUE);
		}
		if (m_1stSubmenu == m_CurSel)
			m_1stSubmenu =  MAX_BOOKMARKS+1;
		int i;
		for (i = m_CurSel; ++i < m_MenuItemList.GetCount(); )
		{
			m_BkMkMenuName[i-1] = m_BkMkMenuName[i];
			m_BkMkIsSubMenu[i-1] = m_BkMkIsSubMenu[i];
			if (m_BkMkIsSubMenu[i-1] && ((i-1) < m_1stSubmenu))
				m_1stSubmenu = i-1;
		}
		m_BkMkMenuName[i-1].Empty();
		m_MenuItemList.DeleteString(m_CurSel);
		if (m_CurSel >= m_MenuItemList.GetCount())
			m_CurSel  = m_MenuItemList.GetCount() - 1;
		if (old1stSubmenu != m_1stSubmenu)
			LoadMenuItemList();
		UpdateData( FALSE );
		GotoDlgCtrl(GetDlgItem(IDC_MENULIST));
		m_MenuItemList.SetCurSel(m_CurSel);
		GetDlgItem(IDC_MOVE_UP)->EnableWindow(m_CurSel > 0 ? TRUE : FALSE);
		GetDlgItem(IDC_MOVE_DOWN)->EnableWindow((m_CurSel < (m_MenuItemList.GetCount()-1)) ? TRUE : FALSE);
		if (!m_BkMkMenuName[0].GetLength())
		{
			GetDlgItem(IDC_CHANGE)->EnableWindow(FALSE);
			GetDlgItem(IDC_REMOVE)->EnableWindow(FALSE);
			GetDlgItem(IDC_EXPORT)->EnableWindow(FALSE);
		}
	}
	else MessageBeep(0);
}

void CBookmarks::OnHelp() 
{
	AfxGetApp()->WinHelp(TASK_MANAGING_BOOKMARKS);
}

BOOL CBookmarks::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}

void CBookmarks::OnOK() 
{
	int i, k;

	UpdateData();
	for (i = -1; ++i < MAX_BOOKMARKS; )
	{
		if (m_BkMkMenuName[i].GetLength() == 0)
			m_BkMkIsSubMenu[i] = FALSE;
		else if (!m_BkMkIsSubMenu[i])
		{
			if ((k = m_BkMkMenuName[i].Find(_T('\t'))) != -1)
				m_BkMkMenuName[i].SetAt(k, _T('#'));
		}
		GET_P4REGPTR()->SetBkMkMenuName(i, m_BkMkMenuName[i]);
		GET_P4REGPTR()->SetBkMkIsSubMenu(i, m_BkMkIsSubMenu[i]);
	}

	CDialog::OnOK();
}

void CBookmarks::OnSelchangeMenulist() 
{
	UpdateData();
	m_CurSel = m_MenuItemList.GetCurSel();
	GetDlgItem(IDC_MOVE_UP)->EnableWindow(m_CurSel > 0 ? TRUE : FALSE);
	GetDlgItem(IDC_MOVE_DOWN)->EnableWindow((m_CurSel < (m_MenuItemList.GetCount()-1)) ? TRUE : FALSE);
	UpdateData( FALSE );
}

void CBookmarks::OnCancel() 
{
	m_Canceling = TRUE;
	CDialog::OnCancel();
}

void CBookmarks::OnImport() 
{
	BOOL foundHdr = FALSE;
	int nbradded = 0;

	CFileDialog fDlg(TRUE, NULL, NULL,  
		OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_NONETWORKBUTTON,
		LoadStringResource(IDS_TOOLSDLG_READ_CUSTOM_TOOLS_FILTER), this,
		MainFrame()->m_osVer.dwMajorVersion < 5 ? OPENFILENAME_SIZE_VERSION_400 : sizeof(OPENFILENAME)); 
	
	TCHAR buf[LONGPATH+1];

	lstrcpy(buf, LoadStringResource(IDS_BKMK_EXPORT_DEFAULT_FILE));  

    fDlg.m_ofn.lpstrFile= buf; 
	fDlg.m_ofn.nMaxFile= LONGPATH; 

	// Set the dlg caption
    CString title = LoadStringResource(IDS_BKMKSDLG_READ_BKMKS_TITLE);
	fDlg.m_ofn.lpstrTitle = title;
	// We dont need no stinking file title
	fDlg.m_ofn.lpstrFileTitle=NULL;
	
	// Set the initial directory
	fDlg.m_ofn.lpstrInitialDir=GET_P4REGPTR()->GetTempDir();

	// Set the user defined filter
	TCHAR customFilterBuf[512] = _T("");
	fDlg.m_ofn.lpstrCustomFilter= customFilterBuf;
	fDlg.m_ofn.nMaxCustFilter=512;

	CString errorTxt;
	INT_PTR retcode=fDlg.DoModal();

	if(retcode == IDOK)
	{
		CString filename = fDlg.GetPathName();
		HANDLE hImportFile;

		if ((hImportFile=CreateFile(filename, GENERIC_READ, 
					FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0)) == INVALID_HANDLE_VALUE)
		{
			errorTxt.FormatMessage(IDS_TOOLSDLG_READ_CUSTOM_TOOLS_READ_ERROR_s, filename);
		}
		else
		{
			int bkmknbr;
			for (bkmknbr=MAX_BOOKMARKS; bkmknbr--; )
			{
				if (!m_BkMkMenuName[bkmknbr].IsEmpty())
					break;
			}
			DWORD NumberOfBytesRead;
			DWORD fsize = GetFileSize(hImportFile, NULL);
			HGLOBAL hText= ::GlobalAlloc(GMEM_SHARE | GMEM_ZEROINIT, fsize+4);
			LPTSTR pStr= (LPTSTR) ::GlobalLock( hText );
			LPTSTR p;

			if (ReadFile(hImportFile, pStr, fsize, &NumberOfBytesRead, NULL))
			{
				int i, n;
#ifdef UNICODE
				if (*pStr == 0xFEFF)
					 pStr++;
				else
				{
					HGLOBAL h= ::GlobalAlloc(GMEM_SHARE | GMEM_ZEROINIT, fsize*2+4);
					LPWSTR pUTF= (LPWSTR) ::GlobalLock( h );
					MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pStr, -1, pUTF, fsize*2+4);
					pStr = pUTF;
					::GlobalUnlock( hText );
					::GlobalFree( hText );
					hText = h;
				}
#else
				if ((unsigned char)*pStr == (unsigned char)0xFF 
				 && (unsigned char)*(pStr+1) == (unsigned char)0xFE)
				{
					AfxMessageBox(IDS_UNABLE_TO_IMPORT_UNICODE_FILE);
					::GlobalUnlock( hText );
					::GlobalFree( hText );
					CloseHandle(hImportFile);
					return;
				}
#endif
				TCHAR header[] = _T("<P4Win Bookmarks>");
				if (_tcsncmp(header, pStr, (sizeof(header)-1)/sizeof(TCHAR)))
				{
					CString msg;
					msg.FormatMessage(IDS_MISSING_HDR_ON_IMPORT, CString(pStr).Left(60));
					if (IDYES != AfxMessageBox(msg, MB_YESNO|MB_ICONQUESTION))
					{
						::GlobalUnlock( hText );
						::GlobalFree( hText );
						CloseHandle(hImportFile);
						return;
					}
				}
				else
				{
					pStr += sizeof(header)/sizeof(TCHAR);
					while (*pStr < ' ' && *pStr)
						*pStr++;
				}
				while (*pStr)
				{
					if (++bkmknbr >= MAX_BOOKMARKS)
					{
						errorTxt.FormatMessage(IDS_BKMKSDLG_READ_BKMKS_TOO_MANY_BKMKS_s, pStr);
						break;
					}
					if (*pStr == _T('#'))
					{
						m_BkMkIsSubMenu[bkmknbr] = foundHdr = TRUE;
						p = ++pStr;
						while (*pStr != _T('\r'))
						{
							if ((unsigned)*pStr < (unsigned)_T(' '))
								*pStr = _T(' ');
							pStr++;
						}
						if (m_1stSubmenu > bkmknbr)
							m_1stSubmenu = bkmknbr;
					}
					else
					{
						if (!foundHdr)
						{
							CString msg;
							msg.FormatMessage(IDS_MISSING_HDR_ON_IMPORT, CString(pStr).Left(60));
							if (IDYES != AfxMessageBox(msg, MB_YESNO|MB_ICONQUESTION))
							{
								::GlobalUnlock( hText );
								::GlobalFree( hText );
								CloseHandle(hImportFile);
								return;
							}
							foundHdr = TRUE;
						}
						m_BkMkIsSubMenu[bkmknbr] = FALSE;
						p = pStr;
						BOOL bGotHash = FALSE;
						BOOL bInQuote = *pStr == _T('\"') ? TRUE : FALSE;
						if (bInQuote)
							*pStr++;
						while (*pStr != _T('\r'))
						{
							if ((unsigned)*pStr < (unsigned)_T(' '))
								*pStr = _T(' ');
							if (!bInQuote)
							{
								if (!bGotHash && (*pStr == _T(' ')) && (*(pStr+1) == _T('#')))
									lstrcpy(pStr, pStr+1);
								if (!bGotHash && (*pStr == _T('#')))
								{
									*pStr = _T('\t');
									bGotHash = TRUE;
								}
							}
							else if (*pStr == _T('\"'))
								bInQuote = FALSE;
							pStr++;
						}
					}
					*pStr = _T('\0');
					if (*p)
						m_BkMkMenuName[bkmknbr] = p;
					while (*++pStr != _T('\n'))
						;
					++pStr;
					if (((n = m_BkMkMenuName[bkmknbr].FindOneOf(_T("@#"))) != -1)
					 && (((i = m_BkMkMenuName[bkmknbr].Find(_T('\t'))) == -1) || (n < i)))
					{
						CString txt;
						txt.FormatMessage(IDS_BKMK_INVALID_CHAR_s, m_BkMkMenuName[bkmknbr]);
						MainFrame()->AddToStatusLog(txt, SV_WARNING);
						m_BkMkMenuName[bkmknbr--].Empty();
					}
					else if (*p)
						++nbradded;
				}
				if (nbradded)
				{
					CString txt;
					txt.FormatMessage(IDS_BKMK_READ_BKMKS_n_BKMKS_ADDED_FROM_s, nbradded, filename);
					MainFrame()->AddToStatusLog(txt, SV_COMPLETION);
					LoadMenuItemList();
				}
			}
			else
				errorTxt.FormatMessage(IDS_TOOLSDLG_READ_CUSTOM_TOOLS_READ_ERROR_s, filename);
			::GlobalUnlock( hText );
			::GlobalFree( hText );
			CloseHandle(hImportFile);
		}
	}
	else if(retcode == IDCANCEL)  // an error
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

void CBookmarks::OnExport() 
{
	CFileDialog fDlg(FALSE, NULL, NULL,  
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NONETWORKBUTTON,
		LoadStringResource(IDS_TOOLS_EXPORT_FILTER), this,
		MainFrame()->m_osVer.dwMajorVersion < 5 ? OPENFILENAME_SIZE_VERSION_400 : sizeof(OPENFILENAME)); 
	
	TCHAR buf[LONGPATH+1];

	lstrcpy(buf, LoadStringResource(IDS_BKMK_EXPORT_DEFAULT_FILE));  

    fDlg.m_ofn.lpstrFile= buf; 
	fDlg.m_ofn.nMaxFile= LONGPATH; 

	// Set the dlg caption
    CString title = LoadStringResource(IDS_BKMK_EXPORT_TITLE);
	fDlg.m_ofn.lpstrTitle=title;
	// We dont need no stinking file title
	fDlg.m_ofn.lpstrFileTitle=NULL;
	
	// Set the initial directory
	fDlg.m_ofn.lpstrInitialDir=GET_P4REGPTR()->GetTempDir();

	// Set the user defined filter
	TCHAR customFilterBuf[512] = _T("");
	fDlg.m_ofn.lpstrCustomFilter= customFilterBuf;
	fDlg.m_ofn.nMaxCustFilter=512;

	CString errorTxt;
	INT_PTR retcode=fDlg.DoModal();

	if(retcode == IDOK)
	{
		CString filename = fDlg.GetPathName();
		HANDLE hExportFile;

		if ((hExportFile=CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 
					FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0)) == INVALID_HANDLE_VALUE)
		{
			DWORD exError=GetLastError();
			errorTxt.FormatMessage(IDS_TOOLS_EXPORT_WRITE_ERROR_s_n, filename, exError);
		}
		else
		{
			DWORD NumberOfBytesWritten;

#ifdef UNICODE
			TCHAR uhdr[] = {0xFEFF};
			WriteFile(hExportFile, uhdr, 2, &NumberOfBytesWritten, NULL);
#endif
			CString recd = _T("<P4Win Bookmarks>\r\n");
			WriteFile(hExportFile, recd, recd.GetLength()*sizeof(TCHAR), &NumberOfBytesWritten, NULL);
			recd.FormatMessage(IDS_BKMK_IMPORTED_FROM_USER, GET_P4REGPTR()->GetP4User());
			WriteFile(hExportFile, recd, recd.GetLength()*sizeof(TCHAR), &NumberOfBytesWritten, NULL);

			int i, n;
			CString txt;
			CString desc;

			UpdateData();
			for (i = -1; ++i < MAX_BOOKMARKS; )
			{
				if (m_BkMkMenuName[i].GetLength() == 0)
					continue;
				if ((n = m_BkMkMenuName[i].Find(_T('\t'))) != -1)
				{
					txt = m_BkMkMenuName[i].Left(n);
					desc = m_BkMkMenuName[i].Mid(n+1);
					if (!desc.IsEmpty())
						txt += _T(" #") + desc;
				}
				else
				{
					txt = m_BkMkMenuName[i];
				}
				if (m_BkMkIsSubMenu[i])
					recd.FormatMessage(IDS_EXPORT_SUBMENU, txt);
				else
					recd.FormatMessage(IDS_EXPORT_ITEM, txt);
				WriteFile(hExportFile, recd, recd.GetLength()*sizeof(TCHAR), &NumberOfBytesWritten, NULL);
			}
			CloseHandle(hExportFile);
			recd.FormatMessage(IDS_BKMK_EXPORT_TOOLS_EXPORTED_TO_s, filename);
			MainFrame()->AddToStatusLog(recd, SV_COMPLETION);
		}
	}
}
