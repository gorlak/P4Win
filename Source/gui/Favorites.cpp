// Favorites.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "MainFrm.h"
#include "Favorites.h"
#include "FavEditDlg.h"
#include "ToolsAdd.h"
#include "hlp\p4win.hh"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFavorites dialog


CFavorites::CFavorites(CWnd* pParent /*=NULL*/)
	: CDialog(CFavorites::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFavorites)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_CurSel = -1;
	m_NbrSubMenus = 0;
	m_1stSubmenu = MAX_FAVORITES+1;

	int n;
	CString txt;
	CString desc;
	CString path;
	int i, j;
	for (i = j = -1; ++i < MAX_FAVORITES; )
	{
		m_FavIsSubMenu[i] = FALSE;				// init the array of submenu flags
		txt = GET_P4REGPTR()->GetFavMenuName(i);
		if (txt.IsEmpty())
			continue;
		if ((n = txt.Find(_T('#'))) != -1)
		{
			desc = txt.Mid(n+1);
			txt  = txt.Left(n);
		}
		else desc.Empty();
		if ((n = txt.Find(_T('@'))) != -1)
		{
			path = txt.Mid(n+1);
			txt  = txt.Left(n);
		}
		else path.Empty();
		m_FavMenuName[++j] = txt;
		if (!desc.IsEmpty())
			m_FavMenuName[j] += _T('\t') + desc;
		m_FavMenuPath[j] = path;
		m_NbrSubMenus += m_FavIsSubMenu[j] = GET_P4REGPTR()->GetFavIsSubMenu(i);
		if (m_FavIsSubMenu[j] && (j < m_1stSubmenu))
			m_1stSubmenu = j;
	}

	m_Canceling = FALSE;
}


void CFavorites::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFavorites)
	DDX_Control(pDX, IDC_MENULIST, m_MenuItemList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFavorites, CDialog)
	//{{AFX_MSG_MAP(CFavorites)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_CHANGE, OnChangeSubmenu)
	ON_BN_CLICKED(IDC_MOVE_UP, OnMoveUp)
	ON_BN_CLICKED(IDC_MOVE_DOWN, OnMoveDown)
	ON_BN_CLICKED(IDC_IMPORT, OnImport)
	ON_BN_CLICKED(IDC_EXPORT, OnExport)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_LBN_SELCHANGE(IDC_MENULIST, OnSelchangeMenulist)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFavorites message handlers

BOOL CFavorites::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	LoadMenuItemList();

	if (m_FavMenuName[0].GetLength())
		m_MenuItemList.SetCurSel(m_CurSel = 0);

	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFavorites::LoadMenuItemList()
{
	while (m_MenuItemList.DeleteString(0) > 0)
		;

	for (int i = -1; m_FavMenuName[++i].GetLength(); )
	{
	    CString txt = m_FavMenuName[i];
		if (m_FavIsSubMenu[i])
			txt += LoadStringResource(IDS_A_SUBMENU);
		else if (i > m_1stSubmenu)
			txt = _T("    ") + txt;
		m_MenuItemList.AddString(txt);
		if (i >= MAX_FAVORITES-1)
		{
			GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
			break;
		}
	}
}

void CFavorites::OnAdd() 
{
	if (m_NbrSubMenus < MAX_FAVORITE_SUBMENUS)
	{
		CFavEditDlg dlg;
		dlg.SetTitle(LoadStringResource(IDS_ADD_FAVORITE));
		dlg.SetRadioShow(0);
		dlg.SetNewMenuName(_T(""));
		dlg.SetIsSubMenu(TRUE);
		dlg.SetCanCr8SubMenu(TRUE);
		dlg.SetFocusDesc(FALSE);
		if ((dlg.DoModal() == IDOK) && ((dlg.GetNewMenuName()).GetLength()))
		{
			UpdateData();
			CString txt = dlg.GetNewMenuName();
			if ((m_CurSel = m_MenuItemList.AddString(txt)) >= 0)
			{
				m_FavMenuName[m_CurSel] = txt;
				if (dlg.GetIsSubMenu())
				{
					txt += LoadStringResource(IDS_A_SUBMENU);
					m_FavIsSubMenu[m_CurSel] = TRUE;
					if (m_CurSel < m_1stSubmenu)
						m_1stSubmenu = m_CurSel;
					m_NbrSubMenus++;
				}
				else if (m_CurSel > m_1stSubmenu)
					txt = _T("    ") + txt;
				m_MenuItemList.DeleteString(m_CurSel);
				m_MenuItemList.InsertString(m_CurSel, txt);
				UpdateData( FALSE );
				m_MenuItemList.SetCurSel(m_CurSel);
				if (m_CurSel >= MAX_FAVORITES-1)
					GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
				UpdateData( FALSE );
			}
		}
	}
}

void CFavorites::OnChangeSubmenu() 
{
	int i;

	if (m_CurSel == -1)
	{
		MessageBeep(0);
		return;
	}
	CFavEditDlg dlg;
	dlg.SetTitle(LoadStringResource(IDS_CHANGE_FAVORITE));
	dlg.SetNewMenuName(m_FavMenuName[m_CurSel]);
	dlg.SetIsSubMenu(m_FavIsSubMenu[m_CurSel]);
	dlg.SetRadioShow(m_FavIsSubMenu[m_CurSel] ? 1 : 2);
	if ((m_NbrSubMenus < MAX_FAVORITE_SUBMENUS) || m_FavIsSubMenu[m_CurSel])
		dlg.SetCanCr8SubMenu(TRUE);
	if ((dlg.DoModal() == IDOK) && ((dlg.GetNewMenuName()).GetLength()))
	{
		m_FavMenuName[m_CurSel] = dlg.GetNewMenuName();
		CString txt = dlg.GetNewMenuName();
		if (dlg.GetIsSubMenu())
			txt += LoadStringResource(IDS_A_SUBMENU);
		else if (m_CurSel > m_1stSubmenu)
			txt = _T("    ") + txt;
		m_MenuItemList.DeleteString(m_CurSel);
		m_MenuItemList.InsertString(m_CurSel, txt);
		if (m_FavIsSubMenu[m_CurSel])
			--m_NbrSubMenus;
		m_NbrSubMenus += m_FavIsSubMenu[m_CurSel] = dlg.GetIsSubMenu();
		if (m_FavIsSubMenu[m_CurSel])
		{
			if (m_CurSel < m_1stSubmenu)
			{
				m_1stSubmenu = m_CurSel;
				LoadMenuItemList();
			}
		}
		else if (m_1stSubmenu == m_CurSel)
		{
			m_1stSubmenu = MAX_FAVORITES+1;
			for (i = m_CurSel; ++i < MAX_FAVORITES; )
			{
				if (m_FavIsSubMenu[i])
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

void CFavorites::OnMoveDown() 
{
	if (m_CurSel >= 0 && (m_CurSel < (m_MenuItemList.GetCount()-1)))
	{
		UpdateData();
		CString FavMenuName = m_FavMenuName[m_CurSel+1];
		CString FavMenuPath = m_FavMenuPath[m_CurSel+1];
		BOOL FavIsSubMenu = m_FavIsSubMenu[m_CurSel+1];

		m_FavMenuName[m_CurSel+1] = m_FavMenuName[m_CurSel];
		m_FavMenuPath[m_CurSel+1] = m_FavMenuPath[m_CurSel];
		m_FavIsSubMenu[m_CurSel+1] = m_FavIsSubMenu[m_CurSel];

		m_FavMenuName[m_CurSel] = FavMenuName;
		m_FavMenuPath[m_CurSel] = FavMenuPath;
		m_FavIsSubMenu[m_CurSel] = FavIsSubMenu;

		if (m_FavIsSubMenu[m_CurSel] && (m_CurSel < m_1stSubmenu))
			m_1stSubmenu = m_CurSel;
		else if (m_1stSubmenu < MAX_FAVORITES+1 && !m_FavIsSubMenu[m_1stSubmenu])
			m_1stSubmenu = m_CurSel+1;

		m_MenuItemList.DeleteString(m_CurSel);
		CString txt = m_FavMenuName[m_CurSel];
		if (m_FavIsSubMenu[m_CurSel])
			txt += LoadStringResource(IDS_A_SUBMENU);
		else if (m_CurSel > m_1stSubmenu)
			txt = _T("    ") + txt;
		m_MenuItemList.InsertString(m_CurSel, txt);

		m_CurSel++;
		m_MenuItemList.DeleteString(m_CurSel);
		txt = m_FavMenuName[m_CurSel];
		if (m_FavIsSubMenu[m_CurSel])
			txt += LoadStringResource(IDS_A_SUBMENU);
		else if (m_CurSel > m_1stSubmenu)
			txt = _T("    ") + txt;
		m_MenuItemList.InsertString(m_CurSel, txt);

		UpdateData( FALSE );
		GotoDlgCtrl(GetDlgItem(IDC_MENULIST));
		m_MenuItemList.SetCurSel(m_CurSel);
	}
	else MessageBeep(0);
}

void CFavorites::OnMoveUp() 
{
	if (m_CurSel > 0)
	{
		UpdateData();
		CString FavMenuName = m_FavMenuName[m_CurSel-1];
		CString FavMenuPath = m_FavMenuPath[m_CurSel-1];
		BOOL FavIsSubMenu = m_FavIsSubMenu[m_CurSel-1];

		m_FavMenuName[m_CurSel-1] = m_FavMenuName[m_CurSel];
		m_FavMenuPath[m_CurSel-1] = m_FavMenuPath[m_CurSel];
		m_FavIsSubMenu[m_CurSel-1] = m_FavIsSubMenu[m_CurSel];

		m_FavMenuName[m_CurSel] = FavMenuName;
		m_FavMenuPath[m_CurSel] = FavMenuPath;
		m_FavIsSubMenu[m_CurSel] = FavIsSubMenu;

		if (m_FavIsSubMenu[m_CurSel-1] && (m_CurSel-1 < m_1stSubmenu))
			m_1stSubmenu = m_CurSel-1;
		else if (m_1stSubmenu < MAX_FAVORITES+1 && !m_FavIsSubMenu[m_1stSubmenu])
			m_1stSubmenu = m_CurSel;

		m_MenuItemList.DeleteString(m_CurSel);
		CString txt = m_FavMenuName[m_CurSel];
		if (m_FavIsSubMenu[m_CurSel])
			txt += LoadStringResource(IDS_A_SUBMENU);
		else if (m_CurSel > m_1stSubmenu)
			txt = _T("    ") + txt;
		m_MenuItemList.InsertString(m_CurSel, txt);

		m_CurSel--;
		m_MenuItemList.DeleteString(m_CurSel);
		txt = m_FavMenuName[m_CurSel];
		if (m_FavIsSubMenu[m_CurSel])
			txt += LoadStringResource(IDS_A_SUBMENU);
		else if (m_CurSel > m_1stSubmenu)
			txt = _T("    ") + txt;
		m_MenuItemList.InsertString(m_CurSel, txt);

		UpdateData( FALSE );
		GotoDlgCtrl(GetDlgItem(IDC_MENULIST));
		m_MenuItemList.SetCurSel(m_CurSel);
	}
	else MessageBeep(0);
}

void CFavorites::OnRemove() 
{
	if (m_CurSel >= 0)
	{
		int old1stSubmenu = m_1stSubmenu;
		UpdateData( );
		if (m_FavIsSubMenu[m_CurSel])
		{
			--m_NbrSubMenus;
			GetDlgItem(IDC_ADD)->EnableWindow(TRUE);
		}
		if (m_1stSubmenu == m_CurSel)
			m_1stSubmenu =  MAX_FAVORITES+1;
		int i;
		for (i = m_CurSel; ++i < m_MenuItemList.GetCount(); )
		{
			m_FavMenuName[i-1] = m_FavMenuName[i];
			m_FavMenuPath[i-1] = m_FavMenuPath[i];
			m_FavIsSubMenu[i-1] = m_FavIsSubMenu[i];
			if (m_FavIsSubMenu[i-1] && ((i-1) < m_1stSubmenu))
				m_1stSubmenu = i-1;
		}
		m_FavMenuName[i-1].Empty();
		m_FavMenuPath[i-1].Empty();
		m_MenuItemList.DeleteString(m_CurSel);
		if (m_CurSel >= m_MenuItemList.GetCount())
			m_CurSel  = m_MenuItemList.GetCount() - 1;
		if (old1stSubmenu != m_1stSubmenu)
			LoadMenuItemList();
		UpdateData( FALSE );
		GotoDlgCtrl(GetDlgItem(IDC_MENULIST));
		m_MenuItemList.SetCurSel(m_CurSel);
	}
	else MessageBeep(0);
}

void CFavorites::OnHelp() 
{
	AfxGetApp()->WinHelp(TASK_MANAGING_FAVORITES);
}

BOOL CFavorites::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}

void CFavorites::OnOK() 
{
	int i, n;
	CString txt;
	CString desc;

	UpdateData();
	for (i = -1; ++i < MAX_FAVORITES; )
	{
		if (m_FavMenuName[i].GetLength() == 0)
		{
			txt.Empty();
			m_FavIsSubMenu[i] = FALSE;
		}
		else if ((n = m_FavMenuName[i].Find(_T('\t'))) != -1)
		{
			txt = m_FavMenuName[i].Left(n);
			desc = m_FavMenuName[i].Mid(n+1);
			if (!m_FavMenuPath[i].IsEmpty())
				txt += _T('@') + m_FavMenuPath[i];
			if (!desc.IsEmpty())
				txt += _T('#') + desc;
		}
		else
		{
			txt = m_FavMenuName[i];
			if (!m_FavMenuPath[i].IsEmpty())
				txt += _T('@') + m_FavMenuPath[i];
		}
		GET_P4REGPTR()->SetFavMenuName(i, txt);
		GET_P4REGPTR()->SetFavIsSubMenu(i, m_FavIsSubMenu[i]);
	}

	CDialog::OnOK();
}

void CFavorites::OnSelchangeMenulist() 
{
	UpdateData();
	m_CurSel = m_MenuItemList.GetCurSel();
	UpdateData( FALSE );
}

void CFavorites::OnCancel() 
{
	m_Canceling = TRUE;
	CDialog::OnCancel();
}

void CFavorites::OnImport() 
{
	BOOL foundHdr = FALSE;
	int nbradded = 0;

	CFileDialog fDlg(TRUE, NULL, NULL,  
		OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_NONETWORKBUTTON,
		LoadStringResource(IDS_TOOLSDLG_READ_CUSTOM_TOOLS_FILTER), this,
		MainFrame()->m_osVer.dwMajorVersion < 5 ? OPENFILENAME_SIZE_VERSION_400 : sizeof(OPENFILENAME)); 
	
	TCHAR buf[LONGPATH+1];

	lstrcpy(buf, LoadStringResource(IDS_FAV_EXPORT_DEFAULT_FILE));  

    fDlg.m_ofn.lpstrFile= buf; 
	fDlg.m_ofn.nMaxFile= LONGPATH; 

	// Set the dlg caption
    CString title = LoadStringResource(IDS_FAVSDLG_READ_FAVS_TITLE);
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
			int favnbr;
			for (favnbr=MAX_FAVORITES; favnbr--; )
			{
				if (!m_FavMenuName[favnbr].IsEmpty())
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
				while (*pStr)
				{
					if (++favnbr >= MAX_FAVORITES)
					{
						errorTxt.FormatMessage(IDS_FAVSDLG_READ_FAVS_TOO_MANY_FAVS_s, pStr);
						break;
					}
					if (*pStr == _T('#'))
					{
						m_FavIsSubMenu[favnbr] = foundHdr = TRUE;
						p = ++pStr;
						while (*pStr != _T('\r'))
						{
							if ((unsigned)*pStr < (unsigned)_T(' '))
								*pStr = _T(' ');
							pStr++;
						}
						if (m_1stSubmenu > favnbr)
							m_1stSubmenu = favnbr;
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
						m_FavIsSubMenu[favnbr] = FALSE;
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
						m_FavMenuName[favnbr] = p;
					m_FavMenuPath[favnbr].Empty();
					while (*++pStr != _T('\n'))
						;
					++pStr;
					if (((n = m_FavMenuName[favnbr].FindOneOf(_T("@#"))) != -1)
					 && (((i = m_FavMenuName[favnbr].Find(_T('\t'))) == -1) || (n < i)))
					{
						CString txt;
						txt.FormatMessage(IDS_FAV_INVALID_CHAR_s, m_FavMenuName[favnbr]);
						MainFrame()->AddToStatusLog(txt, SV_WARNING);
						m_FavMenuName[favnbr--].Empty();
					}
					else if (*p)
						++nbradded;
				}
				if (nbradded)
				{
					CString txt;
					txt.FormatMessage(IDS_FAV_READ_FAVS_n_FAVS_ADDED_FROM_s, nbradded, filename);
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

void CFavorites::OnExport() 
{
	CFileDialog fDlg(FALSE, NULL, NULL,  
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NONETWORKBUTTON,
		LoadStringResource(IDS_TOOLS_EXPORT_FILTER), this,
		MainFrame()->m_osVer.dwMajorVersion < 5 ? OPENFILENAME_SIZE_VERSION_400 : sizeof(OPENFILENAME)); 
	
	TCHAR buf[LONGPATH+1];

	lstrcpy(buf, LoadStringResource(IDS_FAV_EXPORT_DEFAULT_FILE));  

    fDlg.m_ofn.lpstrFile= buf; 
	fDlg.m_ofn.nMaxFile= LONGPATH; 

	// Set the dlg caption
    CString title = LoadStringResource(IDS_FAV_EXPORT_TITLE);
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
			CString recd;
			recd.FormatMessage(IDS_FAV_IMPORTED_FROM_USER, GET_P4REGPTR()->GetP4User());
			WriteFile(hExportFile, recd, recd.GetLength()*sizeof(TCHAR), &NumberOfBytesWritten, NULL);

			int i, n;
			CString txt;
			CString desc;

			UpdateData();
			for (i = -1; ++i < MAX_FAVORITES; )
			{
				if (m_FavMenuName[i].GetLength() == 0)
					continue;
				if ((n = m_FavMenuName[i].Find(_T('\t'))) != -1)
				{
					txt = m_FavMenuName[i].Left(n);
					desc = m_FavMenuName[i].Mid(n+1);
					if (!desc.IsEmpty())
						txt += _T(" #") + desc;
				}
				else
				{
					txt = m_FavMenuName[i];
				}
				if (m_FavIsSubMenu[i])
					recd.FormatMessage(IDS_EXPORT_SUBMENU, txt);
				else
					recd.FormatMessage(IDS_EXPORT_ITEM, txt);
				WriteFile(hExportFile, recd, recd.GetLength()*sizeof(TCHAR), &NumberOfBytesWritten, NULL);
			}
			CloseHandle(hExportFile);
			recd.FormatMessage(IDS_FAV_EXPORT_TOOLS_EXPORTED_TO_s, filename);
			MainFrame()->AddToStatusLog(recd, SV_COMPLETION);
		}
	}
}
