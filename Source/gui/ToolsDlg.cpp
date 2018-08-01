// ToolsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "ToolsDlg.h"
#include "MainFrm.h"
#include "p4registry.h"
#include "hlp\p4win.hh"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolsDlg dialog


CToolsDlg::CToolsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CToolsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CToolsDlg)
	//}}AFX_DATA_INIT

	Clear();
	m_CurSel = -1;
	m_NbrSubMenus = 0;
	m_1stSubmenu = MAX_TOOLS+1;
	m_IsOK2Cr8SubMenu = FALSE;
	m_Finished = m_bEdit = FALSE;
}


void CToolsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolsDlg)
	DDX_Control(pDX, IDC_MENULIST, m_MenuItemList);
	DDX_Text(pDX, IDC_COMMAND, m_Command);
	DDX_Check(pDX, IDC_CONSOLE, m_IsConsole);
	DDX_Text(pDX, IDC_INITDIR, m_InitDir);
	DDX_Text(pDX, IDC_ARGS, m_Args);
	DDX_Check(pDX, IDC_PROMPT, m_IsPrompt);
	DDX_Text(pDX, IDC_PROMPT_EDIT, m_PromptText);
	DDX_Check(pDX, IDC_OUTPUT2STATUSPANE, m_IsOutput2Status);
	DDX_Check(pDX, IDC_CLOSEWINDOW, m_IsCloseOnExit);
	DDX_Check(pDX, IDC_SHOWBROWSE, m_IsShowBrowse);
	DDX_Check(pDX, IDC_REFRESH, m_IsRefresh);
	DDX_Check(pDX, IDC_ADD2CONTEXT, m_OnContext);
	DDX_Text(pDX, IDC_EDIT_NAME, m_Name);
	DDX_Radio(pDX, IDC_RADIO_TOOL, m_MenuType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CToolsDlg, CDialog)
	//{{AFX_MSG_MAP(CToolsDlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_BROWSE_DIR, OnBrowseDir)
	ON_BN_CLICKED(IDC_CONSOLE, OnIsConsole)
	ON_BN_CLICKED(IDC_MOVE_DOWN, OnMoveDown)
	ON_BN_CLICKED(IDC_MOVE_UP, OnMoveUp)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_CHANGE, OnEdit)
	ON_BN_CLICKED(IDC_RADIO_TOOL, OnMenuTypeClick)
	ON_BN_CLICKED(IDC_RADIO_SUBMENU, OnMenuTypeClick)
	ON_LBN_SELCHANGE(IDC_MENULIST, OnSelchangeMenulist)
	ON_LBN_DBLCLK(IDC_MENULIST, OnDblclkMenuList)
	ON_EN_KILLFOCUS(IDC_ARGS, OnKillfocusArgs)
	ON_EN_KILLFOCUS(IDC_INITDIR, OnKillfocusInitDir)
	ON_BN_CLICKED(IDC_PROMPT, OnIsPrompt)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_STATUSADD, LoadVariables )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolsDlg message handlers

BOOL CToolsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CRect rect;
	m_CurSel = 0;
	m_OldbEdit = !m_bEdit;
	if (m_bEdit)
	{
		SetWindowText(m_Caption);
		GetDlgItem(IDC_STATIC_1)->SetWindowText(LoadStringResource(IDS_MENU_ITEM));
		if (!m_IsOK2Cr8SubMenu)
			GetDlgItem(IDC_SUBMENU)->EnableWindow(FALSE);
		GetVariables();
		UpdateData(FALSE);
		GetWindowRect(&rect);
		OffsetRect(&rect, GetSystemMetrics(SM_CYMENUSIZE), 0);
		MoveWindow(&rect, FALSE);
	}
	else
	{
		for (int i = -1; ++i < MAX_TOOLS; )
		{
			m_ToolMenuName[i] = GET_P4REGPTR()->GetToolMenuName(i);
			m_ToolCommand[i] = GET_P4REGPTR()->GetToolCommand(i);
			m_ToolArgs[i] = GET_P4REGPTR()->GetToolArgs(i);
			m_ToolInitDir[i] = GET_P4REGPTR()->GetToolInitDir(i);
			m_ToolPromptText[i] = GET_P4REGPTR()->GetToolPromptText(i);
			m_ToolIsConsole[i] = GET_P4REGPTR()->GetToolIsConsole(i);
			m_ToolIsPrompt[i] = GET_P4REGPTR()->GetToolIsPrompt(i);
			m_ToolIsOutput2Status[i] = GET_P4REGPTR()->GetToolIsOutput2Status(i);
			m_ToolIsCloseOnExit[i] = GET_P4REGPTR()->GetToolIsCloseOnExit(i);
			m_ToolIsShowBrowse[i] = GET_P4REGPTR()->GetToolIsShowBrowse(i);
			m_ToolIsRefresh[i] = GET_P4REGPTR()->GetToolIsRefresh(i);
			m_ToolOnContext[i] = GET_P4REGPTR()->GetToolOnContext(i);
			m_NbrSubMenus += m_ToolIsSubMenu[i] = GET_P4REGPTR()->GetToolIsSubMenu(i);
			if (m_ToolIsSubMenu[i] && (i < m_1stSubmenu))
				m_1stSubmenu = i;
		}

		LoadMenuItemList();

		if (m_ToolMenuName[0].GetLength())
			m_MenuItemList.SetCurSel(m_CurSel = 0);
		else
			GetDlgItem(IDC_MOVE_DOWN)->EnableWindow(FALSE);

		GetDlgItem(IDC_ARGS)->GetWindowRect(&rect);
		int w = rect.Width();
		int h = rect.Height();
		GetDlgItem(IDC_COMMAND)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		GetDlgItem(IDC_COMMAND)->MoveWindow(rect.left, rect.top, w, h);
		GetDlgItem(IDC_INITDIR)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		GetDlgItem(IDC_INITDIR)->MoveWindow(rect.left, rect.top, w, h);
	}
	LoadVariables();
	GetDlgItem(IDC_MOVE_UP)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CToolsDlg::LoadMenuItemList()
{
	while (m_MenuItemList.DeleteString(0) > 0)
		;

	for (int i = -1; m_ToolMenuName[++i].GetLength(); )
	{
	    CString txt = m_ToolMenuName[i];
		if (m_ToolIsSubMenu[i])
			txt += LoadStringResource(IDS_A_SUBMENU);
		else if (i > m_1stSubmenu)
			txt = _T("    ") + txt;
		m_MenuItemList.AddString(txt);
		if (i >= MAX_TOOLS-1)
		{
			GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
			break;
		}
	}
}

void CToolsDlg::OnSelchangeMenulist() 
{
	GetVariables();
	m_CurSel = m_MenuItemList.GetCurSel();
	GetDlgItem(IDC_MOVE_UP)->EnableWindow(m_CurSel > 0 ? TRUE : FALSE);
	GetDlgItem(IDC_MOVE_DOWN)->EnableWindow((m_CurSel < (m_MenuItemList.GetCount()-1)) ? TRUE : FALSE);
	LoadVariables();
}

void CToolsDlg::OnAdd() 
{
	if (m_bEdit)
	{
		ASSERT(0);
		return;
	}

	CToolsDlg dlg;
	dlg.m_bEdit = TRUE;
	dlg.m_Caption = LoadStringResource(IDS_ADD_TOOLS_MENU_ITEM);
	dlg.m_Name = _T("");
	dlg.m_MenuType = 0;
	dlg.m_Command = _T("");
	dlg.m_Args    = _T("");
	dlg.m_InitDir = _T("");
	dlg.m_PromptText = _T("");
	dlg.m_IsConsole  = FALSE;
	dlg.m_IsPrompt     = FALSE;
	dlg.m_IsOutput2Status= FALSE;
	dlg.m_IsCloseOnExit= FALSE;
	dlg.m_IsShowBrowse = FALSE;
	dlg.m_IsRefresh = FALSE;
	dlg.m_OnContext = FALSE;
	if (m_NbrSubMenus < MAX_TOOLS_SUBMENUS)
		dlg.SetCanCr8SubMenu(TRUE);
	if ((dlg.DoModal() == IDOK) && (dlg.m_Name.GetLength()))
	{
		CString txt = dlg.m_Name;
		if ((m_CurSel = m_MenuItemList.AddString(txt)) >= 0)
		{
			if (dlg.GetIsSubMenu())
				txt += LoadStringResource(IDS_A_SUBMENU);
			else if (m_CurSel > m_1stSubmenu)
				txt = _T("    ") + txt;
			m_MenuItemList.DeleteString(m_CurSel);
			m_MenuItemList.InsertString(m_CurSel, txt);
			m_ToolMenuName[m_CurSel] = dlg.m_Name;
			m_NbrSubMenus += m_ToolIsSubMenu[m_CurSel] = dlg.GetIsSubMenu();
			if (m_ToolIsSubMenu[m_CurSel] && (m_CurSel < m_1stSubmenu))
					m_1stSubmenu = m_CurSel;
			m_ToolCommand[m_CurSel] = dlg.m_Command;
			m_ToolArgs[m_CurSel] = dlg.m_Args;
			m_ToolInitDir[m_CurSel] = dlg.m_InitDir;
			m_ToolPromptText[m_CurSel] = dlg.m_PromptText;
			m_ToolIsConsole[m_CurSel] = dlg.m_IsConsole;
			m_ToolIsPrompt[m_CurSel] = dlg.m_IsPrompt;
			m_ToolIsOutput2Status[m_CurSel] = dlg.m_IsOutput2Status;
			m_ToolIsCloseOnExit[m_CurSel] = dlg.m_IsCloseOnExit;
			m_ToolIsShowBrowse[m_CurSel] = dlg.m_IsShowBrowse;
			m_ToolIsRefresh[m_CurSel] = dlg.m_IsRefresh;
			m_ToolOnContext[m_CurSel] = dlg.m_OnContext;
			LoadVariables();
			GotoDlgCtrl(GetDlgItem(IDC_MENULIST));
			m_MenuItemList.SetCurSel(m_CurSel);
			if (m_CurSel >= MAX_TOOLS-1)
				GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
		}
	}
	GetDlgItem(IDC_MOVE_UP)->EnableWindow(m_CurSel > 0 ? TRUE : FALSE);
	GetDlgItem(IDC_MOVE_DOWN)->EnableWindow((m_CurSel < (m_MenuItemList.GetCount()-1)) ? TRUE : FALSE);
}

void CToolsDlg::OnEdit() 
{
	int i;

	if (m_bEdit)
	{
		ASSERT(0);
		return;
	}
	if (m_CurSel == -1)
	{
		MessageBeep(0);
		return;
	}

	BOOL bIsSubMenu;
	CToolsDlg dlg;
	dlg.m_bEdit = TRUE;
	dlg.m_Caption = LoadStringResource(IDS_EDIT_TOOLS_MENU_ITEM);
	dlg.m_Name = m_ToolMenuName[m_CurSel];
	dlg.m_MenuType = bIsSubMenu = m_ToolIsSubMenu[m_CurSel];
	dlg.m_Command = bIsSubMenu ? _T("") : m_ToolCommand[m_CurSel];
	dlg.m_Args    = bIsSubMenu ? _T("") : m_ToolArgs[m_CurSel];
	dlg.m_InitDir = bIsSubMenu ? _T("") : m_ToolInitDir[m_CurSel];
	dlg.m_PromptText = bIsSubMenu ? _T("") : m_ToolPromptText[m_CurSel];
	dlg.m_IsConsole  = bIsSubMenu ? FALSE : m_ToolIsConsole[m_CurSel];
	dlg.m_IsPrompt     = bIsSubMenu ? FALSE : m_ToolIsPrompt[m_CurSel];
	dlg.m_IsOutput2Status= bIsSubMenu ? FALSE : m_ToolIsOutput2Status[m_CurSel];
	dlg.m_IsCloseOnExit= bIsSubMenu ? FALSE : m_ToolIsCloseOnExit[m_CurSel];
	dlg.m_IsShowBrowse = bIsSubMenu ? FALSE : m_ToolIsShowBrowse[m_CurSel];
	dlg.m_IsRefresh = bIsSubMenu ? FALSE : m_ToolIsRefresh[m_CurSel];
	dlg.m_OnContext = m_ToolOnContext[m_CurSel];
	if ((m_NbrSubMenus < MAX_TOOLS_SUBMENUS) || m_ToolIsSubMenu[m_CurSel])
		dlg.SetCanCr8SubMenu(TRUE);
	if ((dlg.DoModal() == IDOK) && (dlg.m_Name.GetLength()))
	{
		m_ToolMenuName[m_CurSel] = dlg.m_Name;
		CString txt = dlg.m_Name;
		if (dlg.GetIsSubMenu())
			txt += LoadStringResource(IDS_A_SUBMENU);
		else if (m_CurSel > m_1stSubmenu)
			txt = _T("    ") + txt;
		m_MenuItemList.DeleteString(m_CurSel);
		m_MenuItemList.InsertString(m_CurSel, txt);
		if (m_ToolIsSubMenu[m_CurSel])
			--m_NbrSubMenus;
		m_NbrSubMenus += m_ToolIsSubMenu[m_CurSel] = dlg.GetIsSubMenu();
		if (m_ToolIsSubMenu[m_CurSel])
		{
			if (m_CurSel < m_1stSubmenu)
			{
				m_1stSubmenu = m_CurSel;
				LoadMenuItemList();
			}
		}
		else if (m_1stSubmenu == m_CurSel)
		{
			m_1stSubmenu = MAX_TOOLS+1;
			for (i = m_CurSel; ++i < MAX_TOOLS; )
			{
				if (m_ToolIsSubMenu[i])
				{
					m_1stSubmenu = i;
					break;
				}
			}
			LoadMenuItemList();
		}
		m_ToolCommand[m_CurSel] = dlg.m_Command;
		m_ToolArgs[m_CurSel] = dlg.m_Args;
		m_ToolInitDir[m_CurSel] = dlg.m_InitDir;
		m_ToolPromptText[m_CurSel] = dlg.m_PromptText;
		m_ToolIsConsole[m_CurSel] = dlg.m_IsConsole;
		m_ToolIsPrompt[m_CurSel] = dlg.m_IsPrompt;
		m_ToolIsOutput2Status[m_CurSel] = dlg.m_IsOutput2Status;
		m_ToolIsCloseOnExit[m_CurSel] = dlg.m_IsCloseOnExit;
		m_ToolIsShowBrowse[m_CurSel] = dlg.m_IsShowBrowse;
		m_ToolIsRefresh[m_CurSel] = dlg.m_IsRefresh;
		m_ToolOnContext[m_CurSel] = dlg.m_OnContext;
		LoadVariables();
		GotoDlgCtrl(GetDlgItem(IDC_MENULIST));
		m_MenuItemList.SetCurSel(m_CurSel);
	}
}

void CToolsDlg::OnMoveDown() 
{
	if (m_CurSel >= 0 && (m_CurSel < (m_MenuItemList.GetCount()-1)))
	{
		GetVariables();
		CString toolMenuName = m_ToolMenuName[m_CurSel+1];
		CString toolCommand = m_ToolCommand[m_CurSel+1];
		CString toolArgs = m_ToolArgs[m_CurSel+1];
		CString toolInitDir = m_ToolInitDir[m_CurSel+1];
		CString toolPromptText = m_ToolPromptText[m_CurSel+1];
		BOOL toolIsConsole = m_ToolIsConsole[m_CurSel+1];
		BOOL toolIsPrompt = m_ToolIsPrompt[m_CurSel+1];
		BOOL toolIsOutput2Status = m_ToolIsOutput2Status[m_CurSel+1];
		BOOL toolIsCloseOnExit = m_ToolIsCloseOnExit[m_CurSel+1];
		BOOL toolIsShowBrowse = m_ToolIsShowBrowse[m_CurSel+1];
		BOOL toolIsRefresh = m_ToolIsRefresh[m_CurSel+1];
		BOOL toolOnContext = m_ToolOnContext[m_CurSel+1];
		BOOL toolIsSubMenu = m_ToolIsSubMenu[m_CurSel+1];

		m_ToolMenuName[m_CurSel+1] = m_ToolMenuName[m_CurSel];
		m_ToolCommand[m_CurSel+1] = m_ToolCommand[m_CurSel];
		m_ToolArgs[m_CurSel+1] = m_ToolArgs[m_CurSel];
		m_ToolInitDir[m_CurSel+1] = m_ToolInitDir[m_CurSel];
		m_ToolPromptText[m_CurSel+1] = m_ToolPromptText[m_CurSel];
		m_ToolIsConsole[m_CurSel+1] = m_ToolIsConsole[m_CurSel];
		m_ToolIsPrompt[m_CurSel+1] = m_ToolIsPrompt[m_CurSel];
		m_ToolIsOutput2Status[m_CurSel+1] = m_ToolIsOutput2Status[m_CurSel];
		m_ToolIsCloseOnExit[m_CurSel+1] = m_ToolIsCloseOnExit[m_CurSel];
		m_ToolIsShowBrowse[m_CurSel+1] = m_ToolIsShowBrowse[m_CurSel];
		m_ToolIsRefresh[m_CurSel+1] = m_ToolIsRefresh[m_CurSel];
		m_ToolOnContext[m_CurSel+1] = m_ToolOnContext[m_CurSel];
		m_ToolIsSubMenu[m_CurSel+1] = m_ToolIsSubMenu[m_CurSel];

		m_ToolMenuName[m_CurSel] = toolMenuName;
		m_ToolCommand[m_CurSel] = toolCommand;
		m_ToolArgs[m_CurSel] = toolArgs;
		m_ToolInitDir[m_CurSel] = toolInitDir;
		m_ToolPromptText[m_CurSel] = toolPromptText;
		m_ToolIsConsole[m_CurSel] = toolIsConsole;
		m_ToolIsPrompt[m_CurSel] = toolIsPrompt;
		m_ToolIsOutput2Status[m_CurSel] = toolIsOutput2Status;
		m_ToolIsCloseOnExit[m_CurSel] = toolIsCloseOnExit;
		m_ToolIsShowBrowse[m_CurSel] = toolIsShowBrowse;
		m_ToolIsRefresh[m_CurSel] = toolIsRefresh;
		m_ToolOnContext[m_CurSel] = toolOnContext;
		m_ToolIsSubMenu[m_CurSel] = toolIsSubMenu;

		if (m_ToolIsSubMenu[m_CurSel] && (m_CurSel < m_1stSubmenu))
			m_1stSubmenu = m_CurSel;
		else if (m_1stSubmenu < MAX_TOOLS+1 && !m_ToolIsSubMenu[m_1stSubmenu])
			m_1stSubmenu = m_CurSel+1;

		m_MenuItemList.DeleteString(m_CurSel);
		CString txt = m_ToolMenuName[m_CurSel];
		if (m_ToolIsSubMenu[m_CurSel])
			txt += LoadStringResource(IDS_A_SUBMENU);
		else if (m_CurSel > m_1stSubmenu)
			txt = _T("    ") + txt;
		m_MenuItemList.InsertString(m_CurSel, txt);

		m_CurSel++;
		m_MenuItemList.DeleteString(m_CurSel);
		txt = m_ToolMenuName[m_CurSel];
		if (m_ToolIsSubMenu[m_CurSel])
			txt += LoadStringResource(IDS_A_SUBMENU);
		else if (m_CurSel > m_1stSubmenu)
			txt = _T("    ") + txt;
		m_MenuItemList.InsertString(m_CurSel, txt);

		LoadVariables();
		GotoDlgCtrl(GetDlgItem(IDC_MENULIST));
		m_MenuItemList.SetCurSel(m_CurSel);
		GetDlgItem(IDC_MOVE_UP)->EnableWindow(m_CurSel > 0 ? TRUE : FALSE);
		GetDlgItem(IDC_MOVE_DOWN)->EnableWindow((m_CurSel < (m_MenuItemList.GetCount()-1)) ? TRUE : FALSE);
	}
	else MessageBeep(0);
}

void CToolsDlg::OnMoveUp() 
{
	if (m_CurSel > 0)
	{
		GetVariables();
		CString toolMenuName = m_ToolMenuName[m_CurSel-1];
		CString toolCommand = m_ToolCommand[m_CurSel-1];
		CString toolArgs = m_ToolArgs[m_CurSel-1];
		CString toolInitDir = m_ToolInitDir[m_CurSel-1];
		CString toolPromptText = m_ToolPromptText[m_CurSel-1];
		BOOL toolIsConsole = m_ToolIsConsole[m_CurSel-1];
		BOOL toolIsPrompt = m_ToolIsPrompt[m_CurSel-1];
		BOOL toolIsOutput2Status = m_ToolIsOutput2Status[m_CurSel-1];
		BOOL toolIsCloseOnExit = m_ToolIsCloseOnExit[m_CurSel-1];
		BOOL toolIsShowBrowse = m_ToolIsShowBrowse[m_CurSel-1];
		BOOL toolIsRefresh = m_ToolIsRefresh[m_CurSel-1];
		BOOL toolOnContext = m_ToolOnContext[m_CurSel-1];
		BOOL toolIsSubMenu = m_ToolIsSubMenu[m_CurSel-1];

		m_ToolMenuName[m_CurSel-1] = m_ToolMenuName[m_CurSel];
		m_ToolCommand[m_CurSel-1] = m_ToolCommand[m_CurSel];
		m_ToolArgs[m_CurSel-1] = m_ToolArgs[m_CurSel];
		m_ToolInitDir[m_CurSel-1] = m_ToolInitDir[m_CurSel];
		m_ToolPromptText[m_CurSel-1] = m_ToolPromptText[m_CurSel];
		m_ToolIsConsole[m_CurSel-1] = m_ToolIsConsole[m_CurSel];
		m_ToolIsPrompt[m_CurSel-1] = m_ToolIsPrompt[m_CurSel];
		m_ToolIsOutput2Status[m_CurSel-1] = m_ToolIsOutput2Status[m_CurSel];
		m_ToolIsCloseOnExit[m_CurSel-1] = m_ToolIsCloseOnExit[m_CurSel];
		m_ToolIsShowBrowse[m_CurSel-1] = m_ToolIsShowBrowse[m_CurSel];
		m_ToolIsRefresh[m_CurSel-1] = m_ToolIsRefresh[m_CurSel];
		m_ToolOnContext[m_CurSel-1] = m_ToolOnContext[m_CurSel];
		m_ToolIsSubMenu[m_CurSel-1] = m_ToolIsSubMenu[m_CurSel];

		m_ToolMenuName[m_CurSel] = toolMenuName;
		m_ToolCommand[m_CurSel] = toolCommand;
		m_ToolArgs[m_CurSel] = toolArgs;
		m_ToolInitDir[m_CurSel] = toolInitDir;
		m_ToolPromptText[m_CurSel] = toolPromptText;
		m_ToolIsConsole[m_CurSel] = toolIsConsole;
		m_ToolIsPrompt[m_CurSel] = toolIsPrompt;
		m_ToolIsOutput2Status[m_CurSel] = toolIsOutput2Status;
		m_ToolIsCloseOnExit[m_CurSel] = toolIsCloseOnExit;
		m_ToolIsShowBrowse[m_CurSel] = toolIsShowBrowse;
		m_ToolIsRefresh[m_CurSel] = toolIsRefresh;
		m_ToolOnContext[m_CurSel] = toolOnContext;
		m_ToolIsSubMenu[m_CurSel] = toolIsSubMenu;

		if (m_ToolIsSubMenu[m_CurSel-1] && (m_CurSel-1 < m_1stSubmenu))
			m_1stSubmenu = m_CurSel-1;
		else if (m_1stSubmenu < MAX_TOOLS+1 && !m_ToolIsSubMenu[m_1stSubmenu])
			m_1stSubmenu = m_CurSel;

		m_MenuItemList.DeleteString(m_CurSel);
		CString txt = m_ToolMenuName[m_CurSel];
		if (m_ToolIsSubMenu[m_CurSel])
			txt += LoadStringResource(IDS_A_SUBMENU);
		else if (m_CurSel > m_1stSubmenu)
			txt = _T("    ") + txt;
		m_MenuItemList.InsertString(m_CurSel, txt);

		m_CurSel--;
		m_MenuItemList.DeleteString(m_CurSel);
		txt = m_ToolMenuName[m_CurSel];
		if (m_ToolIsSubMenu[m_CurSel])
			txt += LoadStringResource(IDS_A_SUBMENU);
		else if (m_CurSel > m_1stSubmenu)
			txt = _T("    ") + txt;
		m_MenuItemList.InsertString(m_CurSel, txt);

		LoadVariables();
		GotoDlgCtrl(GetDlgItem(IDC_MENULIST));
		m_MenuItemList.SetCurSel(m_CurSel);
		GetDlgItem(IDC_MOVE_UP)->EnableWindow(m_CurSel > 0 ? TRUE : FALSE);
		GetDlgItem(IDC_MOVE_DOWN)->EnableWindow((m_CurSel < (m_MenuItemList.GetCount()-1)) ? TRUE : FALSE);
	}
	else MessageBeep(0);
}

void CToolsDlg::OnRemove() 
{
	if (m_CurSel >= 0)
	{
		int old1stSubmenu = m_1stSubmenu;
		GetVariables();
		if (m_ToolIsSubMenu[m_CurSel])
			--m_NbrSubMenus;
		if (m_1stSubmenu == m_CurSel)
			m_1stSubmenu =  MAX_TOOLS+1;
		int i;
		for (i = m_CurSel; ++i < m_MenuItemList.GetCount(); )
		{
			m_ToolMenuName[i-1] = m_ToolMenuName[i];
			m_ToolCommand[i-1] = m_ToolCommand[i];
			m_ToolArgs[i-1] = m_ToolArgs[i];
			m_ToolInitDir[i-1] = m_ToolInitDir[i];
			m_ToolPromptText[i-1] = m_ToolPromptText[i];
			m_ToolIsConsole[i-1] = m_ToolIsConsole[i];
			m_ToolIsPrompt[i-1] = m_ToolIsPrompt[i];
			m_ToolIsOutput2Status[i-1] = m_ToolIsOutput2Status[i];
			m_ToolIsCloseOnExit[i-1] = m_ToolIsCloseOnExit[i];
			m_ToolIsShowBrowse[i-1] = m_ToolIsShowBrowse[i];
			m_ToolIsRefresh[i-1] = m_ToolIsRefresh[i];
			m_ToolOnContext[i-1] = m_ToolOnContext[i];
			m_ToolIsSubMenu[i-1] = m_ToolIsSubMenu[i];
			if (m_ToolIsSubMenu[i-1] && ((i-1) < m_1stSubmenu))
				m_1stSubmenu = i-1;
		}
		m_ToolMenuName[i-1].Empty();
		m_MenuItemList.DeleteString(m_CurSel);
		if (m_CurSel >= m_MenuItemList.GetCount())
			m_CurSel  = m_MenuItemList.GetCount() - 1;
		if (old1stSubmenu != m_1stSubmenu)
			LoadMenuItemList();
		LoadVariables();
		GotoDlgCtrl(GetDlgItem(IDC_MENULIST));
		m_MenuItemList.SetCurSel(m_CurSel);
		GetDlgItem(IDC_ADD)->EnableWindow(TRUE);
		GetDlgItem(IDC_MOVE_UP)->EnableWindow(m_CurSel > 0 ? TRUE : FALSE);
		GetDlgItem(IDC_MOVE_DOWN)->EnableWindow((m_CurSel < (m_MenuItemList.GetCount()-1)) ? TRUE : FALSE);
	}
	else MessageBeep(0);
}

void CToolsDlg::OnBrowse() 
{
	UpdateData();

	CFileDialog fDlg(TRUE, NULL, NULL,  
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NONETWORKBUTTON | OFN_PATHMUSTEXIST,
		LoadStringResource(IDS_TOOLS_DLG_FILTER), this,
		MainFrame()->m_osVer.dwMajorVersion < 5 ? OPENFILENAME_SIZE_VERSION_400 : sizeof(OPENFILENAME)); 
	
	TCHAR buf[MAX_PATH];
	// Zero 1st char so commdlg knows we aren't providing a default filename
	buf[0]=_T('\0');  

    fDlg.m_ofn.lpstrFile= buf; 
	fDlg.m_ofn.nMaxFile= MAX_PATH; 

	// Set the dlg caption
    CString title = LoadStringResource(IDS_SELECT_THE_COMMAND);
	fDlg.m_ofn.lpstrTitle = title;
	// We dont need no stinking file title
	fDlg.m_ofn.lpstrFileTitle=NULL;
	
	// Set the initial directory
	CString initDir = _T("\\");
	int slashpos;
	if ((slashpos = m_Command.ReverseFind(_T('\\'))) != -1)
		initDir = m_Command.Left(slashpos);
	fDlg.m_ofn.lpstrInitialDir=initDir;

	fDlg.m_ofn.lpstrCustomFilter= NULL;

	// Set filter index
	fDlg.m_ofn.nFilterIndex=1;
	
	INT_PTR retcode=fDlg.DoModal();

	if(retcode == IDOK)
	{
		m_Command = fDlg.GetPathName();
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

void CToolsDlg::OnBrowseDir() 
{
	UpdateData();
	CString newfolder = TheApp()->BrowseForFolder(m_hWnd, 
							m_InitDir.IsEmpty() ? TheApp()->m_ClientRoot : m_InitDir, 
							LoadStringResource(IDS_CHOOSE_TEMP_DIR), BIF_NEWDIALOGSTYLE);
	if (!newfolder.IsEmpty())
	{
		m_InitDir = newfolder;
		UpdateData(FALSE);
	}
}
	
void CToolsDlg::OnIsConsole() 
{
	if (m_CurSel >= 0)
	{
		GetVariables();
		if (!m_IsConsole)
		{
			m_ToolIsOutput2Status[m_CurSel] = m_IsOutput2Status = FALSE;
			m_ToolIsCloseOnExit[m_CurSel] = m_IsCloseOnExit = FALSE;
		}
		LoadVariables();
	}
}

void CToolsDlg::OnIsPrompt() 
{
	if (m_CurSel >= 0)
	{
		GetVariables();
		if (!m_IsPrompt)
			m_ToolIsShowBrowse[m_CurSel] = m_IsShowBrowse = FALSE;
		LoadVariables();
	}
}

void CToolsDlg::OnHelp() 
{
	AfxGetApp()->WinHelp(TASK_ADDING_COMMANDS_TO_THE_TOOLS_MENU);
}

BOOL CToolsDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}

void CToolsDlg::OnOK() 
{
	int i;

	GetVariables();
	if (!m_bEdit)
	{
		for (i = -1; ++i < MAX_TOOLS; )
		{
			if (m_ToolMenuName[i].GetLength() == 0)
			{
				m_ToolCommand[i].Empty();
				m_ToolArgs[i].Empty();
				m_ToolInitDir[i].Empty();
				m_ToolPromptText[i].Empty();
				m_ToolIsConsole[i] = FALSE;
				m_ToolIsPrompt[i] = FALSE;
				m_ToolIsOutput2Status[i] = FALSE;
				m_ToolIsCloseOnExit[i] = FALSE;
				m_ToolIsShowBrowse[i] = FALSE;
				m_ToolIsRefresh[i] = FALSE;
				m_ToolOnContext[i] = FALSE;
				m_ToolIsSubMenu[i] = FALSE;
			}
			else if (m_ToolCommand[i].IsEmpty() 
				&& m_ToolArgs[i].IsEmpty() 
				&& m_ToolInitDir[i].IsEmpty()
				&& m_ToolIsSubMenu[i] == FALSE)
			{
				CString txt;
				txt.FormatMessage(IDS_YOU_MUST_ENTER_SOMETHING_IN_FIELD_FOR_TOOL_s,
								m_ToolMenuName[i]);
				AfxMessageBox(txt, MB_ICONERROR);
				m_MenuItemList.SetCurSel(m_CurSel = i);
				LoadVariables();
				GotoDlgCtrl(GetDlgItem(IDC_COMMAND));
				return;
			}
			if (!m_ToolIsConsole[i])
			{
				m_ToolIsOutput2Status[i] = FALSE;
				m_ToolIsCloseOnExit[i] = FALSE;
			}
			if (!m_ToolIsPrompt[i])
				m_ToolIsShowBrowse[i] = FALSE;
			GET_P4REGPTR()->SetToolMenuName(i, m_ToolMenuName[i]);
			GET_P4REGPTR()->SetToolCommand(i, m_ToolCommand[i]);
			GET_P4REGPTR()->SetToolArgs(i, m_ToolArgs[i]);
			GET_P4REGPTR()->SetToolInitDir(i, m_ToolInitDir[i]);
			GET_P4REGPTR()->SetToolPromptText(i, m_ToolPromptText[i]);
			GET_P4REGPTR()->SetToolIsConsole(i, m_ToolIsConsole[i]);
			GET_P4REGPTR()->SetToolIsPrompt(i, m_ToolIsPrompt[i]);
			GET_P4REGPTR()->SetToolIsOutput2Status(i, m_ToolIsOutput2Status[i]);
			GET_P4REGPTR()->SetToolIsCloseOnExit(i, m_ToolIsCloseOnExit[i]);
			GET_P4REGPTR()->SetToolIsShowBrowse(i, m_ToolIsShowBrowse[i]);
			GET_P4REGPTR()->SetToolIsRefresh(i, m_ToolIsRefresh[i]);
			GET_P4REGPTR()->SetToolOnContext(i, m_ToolOnContext[i]);
			GET_P4REGPTR()->SetToolIsSubMenu(i, m_ToolIsSubMenu[i]);
		}
	}
	m_Finished = TRUE;
	CDialog::OnOK();
}

LRESULT CToolsDlg::LoadVariables(WPARAM, LPARAM)
{
	if (m_CurSel >= 0)
	{
		BOOL bIsSubMenu = m_MenuType = m_ToolIsSubMenu[m_CurSel];
		m_Command = bIsSubMenu ? _T("") : m_ToolCommand[m_CurSel];
		m_Args    = bIsSubMenu ? _T("") : m_ToolArgs[m_CurSel];
		m_InitDir = bIsSubMenu ? _T("") : m_ToolInitDir[m_CurSel];
		m_PromptText = bIsSubMenu ? _T("") : m_ToolPromptText[m_CurSel];
		m_IsConsole  = bIsSubMenu ? FALSE : m_ToolIsConsole[m_CurSel];
		m_IsPrompt     = bIsSubMenu ? FALSE : m_ToolIsPrompt[m_CurSel];
		m_IsOutput2Status= bIsSubMenu ? FALSE : m_ToolIsOutput2Status[m_CurSel];
		m_IsCloseOnExit= bIsSubMenu ? FALSE : m_ToolIsCloseOnExit[m_CurSel];
		m_IsShowBrowse = bIsSubMenu ? FALSE : m_ToolIsShowBrowse[m_CurSel];
		m_IsRefresh = bIsSubMenu ? FALSE : m_ToolIsRefresh[m_CurSel];
		m_OnContext = m_ToolOnContext[m_CurSel];
		m_Name = m_ToolMenuName[m_CurSel];
		UpdateData( FALSE );
		GetDlgItem(IDC_COMMAND)->EnableWindow(!bIsSubMenu);
		GetDlgItem(IDC_ARGS)->EnableWindow(!bIsSubMenu);
		GetDlgItem(IDC_INITDIR)->EnableWindow(!bIsSubMenu);
		GetDlgItem(IDC_CONSOLE)->EnableWindow(!bIsSubMenu);
		GetDlgItem(IDC_PROMPT)->EnableWindow(!bIsSubMenu);
		GetDlgItem(IDC_OUTPUT2STATUSPANE)->EnableWindow(m_IsConsole);
		GetDlgItem(IDC_CLOSEWINDOW)->EnableWindow(m_IsConsole);
		GetDlgItem(IDC_SHOWBROWSE)->EnableWindow(m_IsPrompt);
		GetDlgItem(IDC_PROMPT_EDIT)->EnableWindow(m_IsPrompt);
		GetDlgItem(IDC_PROMPT_LABEL)->EnableWindow(m_IsPrompt);
		GetDlgItem(IDC_REFRESH)->EnableWindow(!bIsSubMenu);
		if (m_bEdit)
		{
			if (m_OldbEdit != m_bEdit)
			{
				GetDlgItem(IDC_STATIC_2)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_EDIT_NAME)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_EDIT_NAME)->EnableWindow(TRUE);
				GetDlgItem(IDC_STATIC_3)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_RADIO_TOOL)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_RADIO_TOOL)->EnableWindow(TRUE);
				GetDlgItem(IDC_RADIO_SUBMENU)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_RADIO_SUBMENU)->EnableWindow(TRUE);
				GotoDlgCtrl(GetDlgItem(IDC_EDIT_NAME));
				GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHANGE)->EnableWindow(FALSE);
				GetDlgItem(IDC_REMOVE)->EnableWindow(FALSE);
				GetDlgItem(IDC_MOVE_UP)->EnableWindow(FALSE);
				GetDlgItem(IDC_MOVE_DOWN)->EnableWindow(FALSE);
				GetDlgItem(IDC_MENULIST)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_ADD)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_CHANGE)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_REMOVE)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_MOVE_UP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_MOVE_DOWN)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_COMMAND)->SendMessage(EM_SETREADONLY, FALSE, 0);
				GetDlgItem(IDC_COMMAND)->ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);
				GetDlgItem(IDC_ARGS)->SendMessage(EM_SETREADONLY, FALSE, 0);
				GetDlgItem(IDC_ARGS)->ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);
				GetDlgItem(IDC_INITDIR)->SendMessage(EM_SETREADONLY, FALSE, 0);
				GetDlgItem(IDC_INITDIR)->ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);
				GetDlgItem(IDC_PROMPT_EDIT)->SendMessage(EM_SETREADONLY, FALSE, 0);
				GetDlgItem(IDC_PROMPT_EDIT)->ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);
				m_OldbEdit = m_bEdit;
			}
			GetDlgItem(IDC_BROWSE)->EnableWindow(!bIsSubMenu);
			GetDlgItem(IDC_BROWSE_DIR)->EnableWindow(!bIsSubMenu);
			GetDlgItem(IDC_BROWSE)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BROWSE_DIR)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_CONSOLE)->ModifyStyle(BS_CHECKBOX|BS_FLAT, BS_AUTOCHECKBOX, 0);
			GetDlgItem(IDC_PROMPT)->ModifyStyle(BS_CHECKBOX|BS_FLAT, BS_AUTOCHECKBOX, 0);
			GetDlgItem(IDC_OUTPUT2STATUSPANE)->ModifyStyle(BS_CHECKBOX|BS_FLAT, BS_AUTOCHECKBOX, 0);
			GetDlgItem(IDC_CLOSEWINDOW)->ModifyStyle(BS_CHECKBOX|BS_FLAT, BS_AUTOCHECKBOX, 0);
			GetDlgItem(IDC_SHOWBROWSE)->ModifyStyle(BS_CHECKBOX|BS_FLAT, BS_AUTOCHECKBOX, 0);
			GetDlgItem(IDC_REFRESH)->ModifyStyle(BS_CHECKBOX|BS_FLAT, BS_AUTOCHECKBOX, 0);
			GetDlgItem(IDC_ADD2CONTEXT)->ModifyStyle(BS_CHECKBOX|BS_FLAT, BS_AUTOCHECKBOX, 0);
		}
		else
		{
			if (m_OldbEdit != m_bEdit)
			{
				GetDlgItem(IDC_STATIC_2)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_NAME)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_NAME)->EnableWindow(FALSE);
				GetDlgItem(IDC_STATIC_3)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_RADIO_TOOL)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_RADIO_TOOL)->EnableWindow(FALSE);
				GetDlgItem(IDC_RADIO_SUBMENU)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_RADIO_SUBMENU)->EnableWindow(FALSE);
				GetDlgItem(IDC_MENULIST)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_ADD)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_CHANGE)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_REMOVE)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_MOVE_UP)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_MOVE_DOWN)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_ADD)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHANGE)->EnableWindow(TRUE);
				GetDlgItem(IDC_REMOVE)->EnableWindow(TRUE);
				GetDlgItem(IDC_MOVE_UP)->EnableWindow(TRUE);
				GetDlgItem(IDC_MOVE_DOWN)->EnableWindow(TRUE);
				GotoDlgCtrl(GetDlgItem(IDC_MENULIST));
				GetDlgItem(IDC_COMMAND)->SendMessage(EM_SETREADONLY, TRUE, 0);
				GetDlgItem(IDC_COMMAND)->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
				GetDlgItem(IDC_ARGS)->SendMessage(EM_SETREADONLY, TRUE, 0);
				GetDlgItem(IDC_ARGS)->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
				GetDlgItem(IDC_INITDIR)->SendMessage(EM_SETREADONLY, TRUE, 0);
				GetDlgItem(IDC_INITDIR)->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
				GetDlgItem(IDC_PROMPT_EDIT)->SendMessage(EM_SETREADONLY, TRUE, 0);
				GetDlgItem(IDC_PROMPT_EDIT)->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
				GetDlgItem(IDC_BROWSE)->EnableWindow(FALSE);
				GetDlgItem(IDC_BROWSE_DIR)->EnableWindow(FALSE);
				GetDlgItem(IDC_BROWSE)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BROWSE_DIR)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_CONSOLE)->ModifyStyle(BS_AUTOCHECKBOX, BS_CHECKBOX|BS_FLAT, 0);
				GetDlgItem(IDC_PROMPT)->ModifyStyle(BS_AUTOCHECKBOX, BS_CHECKBOX|BS_FLAT, 0);
				GetDlgItem(IDC_CLOSEWINDOW)->ModifyStyle(BS_AUTOCHECKBOX, BS_CHECKBOX|BS_FLAT, 0);
				GetDlgItem(IDC_OUTPUT2STATUSPANE)->ModifyStyle(BS_AUTOCHECKBOX, BS_CHECKBOX|BS_FLAT, 0);
				GetDlgItem(IDC_SHOWBROWSE)->ModifyStyle(BS_AUTOCHECKBOX, BS_CHECKBOX|BS_FLAT, 0);
				GetDlgItem(IDC_REFRESH)->ModifyStyle(BS_AUTOCHECKBOX, BS_CHECKBOX|BS_FLAT, 0);
				GetDlgItem(IDC_ADD2CONTEXT)->ModifyStyle(BS_AUTOCHECKBOX, BS_CHECKBOX|BS_FLAT, 0);
				m_OldbEdit = m_bEdit;
			}
		}
	}
	return 0;
}

void CToolsDlg::GetVariables()
{
	UpdateData( );
	if (m_CurSel >= 0)
	{
		m_ToolMenuName[m_CurSel] = m_Name;
		m_ToolIsSubMenu[m_CurSel] = m_MenuType;
		m_ToolCommand[m_CurSel] = m_Command;
		m_ToolArgs[m_CurSel] = m_Args;
		m_ToolInitDir[m_CurSel] = m_InitDir;
		m_ToolPromptText[m_CurSel] = m_PromptText;
		m_ToolIsConsole[m_CurSel] = m_IsConsole;
		m_ToolIsPrompt[m_CurSel] = m_IsPrompt;
		m_ToolIsOutput2Status[m_CurSel] = m_IsOutput2Status;
		m_ToolIsCloseOnExit[m_CurSel] = m_IsCloseOnExit;
		m_ToolIsShowBrowse[m_CurSel] = m_IsShowBrowse;
		m_ToolIsRefresh[m_CurSel] = m_IsRefresh;
		m_ToolOnContext[m_CurSel] = m_OnContext;
	}
}

void CToolsDlg::OnMenuTypeClick()
{
	GetVariables();
	LoadVariables();
}

void CToolsDlg::Clear()
{
	m_Command.Empty();
	m_IsConsole = FALSE;
	m_InitDir.Empty();
	m_PromptText.Empty();
	m_Args.Empty();
	m_IsPrompt = FALSE;
	m_IsOutput2Status = FALSE;
	m_IsCloseOnExit = FALSE;
	m_IsShowBrowse = FALSE;
	m_IsRefresh = FALSE;
	m_OnContext = FALSE;
	if (::IsWindow(m_hWnd))
	{
		GetDlgItem(IDC_OUTPUT2STATUSPANE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CLOSEWINDOW)->EnableWindow(FALSE);
		GetDlgItem(IDC_SHOWBROWSE)->EnableWindow(FALSE);
		UpdateData( FALSE );
	}
}

void CToolsDlg::OnDblclkMenuList() 
{
	OnEdit();
}

void CToolsDlg::OnKillfocusArgs() 
{
	if (!m_Finished)
	{
		UpdateData( );
		CString cmdline = m_Command + _T(' ') + m_Args;
		int i = MainFrame()->FindPercentArg(cmdline, 0);
		if ((i != -1) && (MainFrame()->FindPercentArg(cmdline, i+1) != -1))
		{
			CString txt = LoadStringResource(IDS_MORE_THAN_ONE_REPLACEABLE_FILE_ARG_NOT_ALLOWED) + cmdline;
			AfxMessageBox(txt, MB_ICONSTOP);
			GotoDlgCtrl(GetDlgItem(IDC_ARGS));
		}
	}
}

void CToolsDlg::OnKillfocusInitDir() 
{
	if (!m_Finished)
	{
		UpdateData( );
		int i = m_InitDir.Find(_T('%'));
		if ((i == 0) && (m_InitDir.GetAt(1) == _T('F')))
		{
			m_InitDir.SetAt(1, _T('f'));
			UpdateData(FALSE);
		}
		if ((i == 0) && (m_InitDir != _T("%f")))
		{
			CString txt = LoadStringResource(IDS_ONLY_PERCENT_F_MAY_BE_USED) + m_Args;
			AfxMessageBox(txt, MB_ICONSTOP);
			GotoDlgCtrl(GetDlgItem(IDC_INITDIR));
		}
	}
}

void CToolsDlg::OnCancel() 
{
	m_Finished = TRUE;
	CDialog::OnCancel();
}
