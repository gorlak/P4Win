// NewWindowDlg.cpp : implementation file
//
// This not only handles the creation of a new window with different (or even the same)
// port-client-user [PCU] (which was its first function, hence the name of the file and class),
// but it now also provides an easy method of switching the port and/or client and/or user
// for the current window.  In addition it allows the deletion of unwanted PCUs from the
// list of Most Recently Used [MRU] PCUs.

#include "stdafx.h"
#include "p4win.h"
#include "NewWindowDlg.h"
#include "MainFrm.h"
#include "P4Object.h"
#include "P4ListBrowse.h"
#include "ImageList.h"
#include "cmd_clients.h"
#include "cmd_users.h"
#include "hlp\p4win.hh"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewWindowDlg dialog


CNewWindowDlg::CNewWindowDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewWindowDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewWindowDlg)
	m_client = GET_P4REGPTR()->GetP4Client();
	m_port = GET_P4REGPTR()->GetP4Port();
	m_user = GET_P4REGPTR()->GetP4User();
	m_SetDefClient = FALSE;
	m_SetDefPort = FALSE;
	m_SetDefUser = FALSE;
	m_StartWith = GET_P4REGPTR()->GetP4StartWith();
	//}}AFX_DATA_INIT

	m_bSamePort = TRUE;
}


void CNewWindowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewWindowDlg)
	DDX_Control(pDX, IDC_PORT, m_PortCombo);
	DDX_Text(pDX, IDC_CLIENT, m_client);
	DDV_MaxChars(pDX, m_client, 1024);
	DDX_Text(pDX, IDC_USER, m_user);
	DDX_Control(pDX, IDC_PORTCLIENTUSER, m_PCUcombo);
	DDX_Check(pDX, IDC_SETDEFCLIENT, m_SetDefClient);
	DDX_Check(pDX, IDC_SETDEFPORT, m_SetDefPort);
	DDX_Check(pDX, IDC_SETDEFUSER, m_SetDefUser);
	DDX_Radio(pDX, IDC_STARTWITHMRUPCU, m_StartWith);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewWindowDlg, CDialog)
	//{{AFX_MSG_MAP(CNewWindowDlg)
	ON_CBN_SELCHANGE(IDC_PORTCLIENTUSER, OnSelchangePortclientuser)
	ON_CBN_EDITCHANGE(IDC_PORTCLIENTUSER, OnEditchangePortclientuser)
	ON_EN_SETFOCUS(IDC_CLIENT, OnSetfocusOther)
	ON_BN_CLICKED(IDC_NEW_WINDOW, OnNewWindow)
	ON_BN_CLICKED(IDC_SETDEFALL, OnSetdefall)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_OPEN, OnOK)
	ON_CBN_SELCHANGE(IDC_PORT, OnSelchangePort)
	ON_CBN_EDITCHANGE(IDC_PORT, OnEditchangePort)
	ON_CBN_SETFOCUS(IDC_PORT, OnSetfocusOther)
	ON_EN_SETFOCUS(IDC_USER, OnSetfocusOther)
	ON_BN_CLICKED(IDC_STARTWITHDEFAULTS, OnStartwithdefaults)
	ON_BN_CLICKED(IDC_STARTWITHMRUPCU, OnStartwithmrupcu)
	ON_BN_CLICKED(IDC_BROWSE_CLIENTS, OnBrowseClients)
	ON_BN_CLICKED(IDC_BROWSE_USERS, OnBrowseUsers)
	ON_WM_HELPINFO()
	ON_MESSAGE(WM_P4CLIENTS, OnP4ClientList )
	ON_MESSAGE(WM_BROWSECALLBACK1, OnBrowseClientsCallBack)
	ON_MESSAGE(WM_P4USERS, OnP4UserList )
	ON_MESSAGE(WM_BROWSECALLBACK2, OnBrowseUsersCallBack)
	ON_MESSAGE(WM_BROWSECLIENTS, OnDoBrowseClients)
	ON_MESSAGE(WM_BROWSEUSERS, OnDoBrowseUsers)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewWindowDlg message handlers


BOOL CNewWindowDlg::OnInitDialog() 
{
	CString str;
	int j;

	CDialog::OnInitDialog();

	if (m_StartWith == 0)
		 OnStartwithmrupcu();


    // Get the current port-client-user setting
	CString m_OrigPcu = GET_P4REGPTR()->GetMRUPcu(0);
	if ((j = m_OrigPcu.Find(_T('@'))) > 0)
		m_OrigPcu = m_OrigPcu.Left(j);

	// Make sure the combo is empty
	m_PCUcombo.Clear();

	// Load all MRU pcus into list box
	for(int i=0; i < MAX_MRU_PCUS; i++)
	{
		if(GET_P4REGPTR()->GetMRUPcu(i).GetLength() > 0)
		{
			str = GET_P4REGPTR()->GetMRUPcu(i);
			if ((j = str.Find(_T('@'))) > 0)
				str = str.Left(j);
			m_PCUcombo.AddString(str);
		}
	}

	// Select the current PCU item
	m_PCUcombo.SetCurSel(m_PCUcombo.FindStringExact(-1, m_OrigPcu));

	LoadPortCombo();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// This routine loads m_port as the 1st item in the m_PortCombo
void CNewWindowDlg::LoadPortCombo()
{
	// Make sure we have the current port setting first in the combo box
	if (m_port.GetLength() > 0)
		GET_P4REGPTR()->AddMRUPort( m_port, GET_P4REGPTR()->GetP4Charset() );

	// Make sure the combo is empty
	m_PortCombo.ResetContent();

	// Load all MRU ports into list box
	for(int i=0; i < MAX_MRU_PORTS; i++)
	{
		if(GET_P4REGPTR()->GetMRUPort(i).GetLength() > 0)
		{
			CString str = GET_P4REGPTR()->GetMRUPort(i);
			if( str.Find(_T(" ")) != -1 )
				str = _T("\"") + str + _T("\"");
			m_PortCombo.AddString(str);
		}
	}

	// Select the first item
	m_PortCombo.SetCurSel(0);
	OnSelchangePort();
}

void CNewWindowDlg::OnSelchangePortclientuser() 
{
	if(m_PCUcombo.GetCurSel() != CB_ERR)
	{
		CString pcuStr;
		m_PCUcombo.GetLBText(m_PCUcombo.GetCurSel(), pcuStr);
		m_PCUcombo.SetWindowText( pcuStr );
	}
	else
		m_PCUcombo.SetWindowText(_T(""));
	OnEditchangePortclientuser();
}

void CNewWindowDlg::OnEditchangePortclientuser() 
{
	int i;
	int firstsep;
	int secondsep;
	UpdateData();

	CString pcuStr;
	// Get the current text in the combobox's edit control
	if ((i = m_PCUcombo.GetCurSel()) >= 0)
		 m_PCUcombo.GetLBText(i, pcuStr);
	else m_PCUcombo.GetWindowText(pcuStr);

	// find the separators (spaces or quotes)
	if (pcuStr.GetAt(0) == _T('\"'))
		firstsep = pcuStr.Find(_T('\"'),1) + 1;
	else
		firstsep = pcuStr.Find(_T(' '));
	secondsep= pcuStr.Find(_T(' '), firstsep+1);

	// set the 3 port, client and user edit controls
	if (firstsep > 0 && secondsep > 0)
	{
		m_port  = pcuStr.Left(firstsep);
		m_port.TrimLeft(_T('\"'));
		m_port.TrimRight(_T('\"'));
		m_client= pcuStr.Mid(firstsep+1, secondsep - firstsep - 1);
		m_user  = pcuStr.Right(pcuStr.GetLength() - secondsep - 1);
		LoadPortCombo();
		UpdateData(FALSE);
	}
}

void CNewWindowDlg::OnSelchangePort() 
{
	if(m_PortCombo.GetCurSel() != CB_ERR)
	{
		CString port;
		m_PortCombo.GetLBText(m_PortCombo.GetCurSel(), port);
		m_PortCombo.SetWindowText( port );
	}
	OnEditchangePort();
}

void CNewWindowDlg::OnEditchangePort() 
{
	CString port;
	m_PortCombo.GetWindowText( port );

	port.TrimRight();
	port.TrimLeft();

	if( port.GetAt(0) == _T('\"'))
	{
		port.TrimRight(_T('\"'));
		port.TrimLeft(_T('\"'));
	}

	BOOL b = port.GetLength() > 0;
	m_bSamePort = b && port == GET_P4REGPTR()->GetP4Port();
	if (!m_bSamePort)
	{
		if (b)
		{
			b = _istdigit(port.GetAt(port.GetLength()-1)) && port.Find(_T(' ')) == -1;
			if (b && !_istdigit(port.GetAt(0)))
				b = port.Find(_T(':')) > 0;
		}
	}
	GetDlgItem(IDC_BROWSE_CLIENTS)->EnableWindow(b);
	GetDlgItem(IDC_BROWSE_USERS)->EnableWindow(b);
}

void CNewWindowDlg::OnSetfocusOther() 
{
	OnSelchangePort();
	m_PCUcombo.SetWindowText(_T(""));	// clear the combobox's edit control if the
										// combobox loses focus
	CEdit *wnd = (CEdit *)GetFocus();
	wnd->SetSel(0, -1);					// select the entire field that is receiving focus
}

void CNewWindowDlg::OnOK()	// OK pressed => set this window to new PCU values
{							//	and/or remove any strings requested to be removed
	BOOL	bUpdateCapt;
	CString old_client = GET_P4REGPTR()->GetP4Client();
	CString old_port = GET_P4REGPTR()->GetP4Port();
	CString old_user = GET_P4REGPTR()->GetP4User();

	UpdateData();

	if ( EditPort( ) == FALSE )
		return;

	GET_P4REGPTR()->SetP4StartWith(m_StartWith);

	if ( !m_RmvMRUPcu.IsEmpty() )	// if they wanted to remove some PCU strings
	{
		 ReloadMRUPcu();			//	now actually do the remove
		 bUpdateCapt = TRUE;
	}
	else bUpdateCapt = FALSE;

	if (!m_client.IsEmpty() || !m_port.IsEmpty() || !m_user.IsEmpty()
	  || m_SetDefPort || m_SetDefClient || m_SetDefUser)	// Anything to change?
	{
		// if any of port. client or user is not given, use the current setting
		if (m_port.IsEmpty())
			m_port = old_port;
		if (m_client.IsEmpty())
			m_client = old_client;
		if (m_user.IsEmpty())
			m_user = old_user;
		m_port.TrimLeft(_T('\"'));
		m_port.TrimRight(_T('\"'));

		if (GET_P4REGPTR()->GetExpandFlag() == 1)
			GET_P4REGPTR()->AddMRUPcuPath(((CMainFrame *) AfxGetMainWnd())->GetCurrentItemPath());

		// change only the ones that are different
		if ((m_port != old_port) || m_SetDefPort)
			GET_P4REGPTR()->SetP4Port(m_port, TRUE, m_SetDefPort, m_SetDefPort);

		if ((m_user != old_user) || m_SetDefUser)
			if (!GET_P4REGPTR()->SetP4User(m_user, TRUE, m_SetDefUser, m_SetDefUser))
				AfxMessageBox( IDS_UNABLE_TO_WRITE_P4USER_TO_THE_REGISTRY, MB_ICONEXCLAMATION);

		if ((m_client != old_client) || m_SetDefClient)
		{
			if(!MainFrame()->ClientSpecSwitch(m_client, TRUE, old_port != m_port))// calls OnPerforceOptions()
			{	// put the old stuff back for port and user if client switch failed
				if (m_port != old_port)
					GET_P4REGPTR()->SetP4Port(old_port, TRUE, m_SetDefPort, m_SetDefPort);
				if (m_user != old_user)
					GET_P4REGPTR()->SetP4User(old_user, TRUE, m_SetDefUser, m_SetDefUser);
				return;
			}
			else // switch worked - check if need to set default also
			{
				if (m_SetDefClient)
					if (!GET_P4REGPTR()->SetP4Client(m_client, TRUE, TRUE, TRUE))
						AfxMessageBox(IDS_UNABLE_TO_WRITE_P4CLIENT_TO_THE_REGISTRY, 
										MB_ICONEXCLAMATION);
			}
		}
		else 
			MainFrame()->OnPerforceOptions(FALSE, old_port != m_port);// if not changing client, need to call this for port/user chg
	}
	else if (bUpdateCapt)
		MainFrame()->UpdateCaption();	// updates Settings menu items

	CDialog::OnOK();
}

void CNewWindowDlg::OnNewWindow()	// Spawn a new P4win with the requested PCU
{
	HKEY	hKey = NULL;
	CString sKey = _T("Software\\Perforce\\environment\\");
	TCHAR	path[_MAX_PATH+1];
	DWORD	disposition;

	UpdateData();

	if ( EditPort( ) == FALSE )
		return;

	GET_P4REGPTR()->SetP4StartWith(m_StartWith);

	if ( !m_RmvMRUPcu.IsEmpty() )	// if they wanted to remove some PCU strings
		ReloadMRUPcu();				//	now actually go do the remove

	// if any of port. client or user is not given, use the current setting
	if (m_port.IsEmpty())
		m_port = GET_P4REGPTR()->GetP4Port();
	if (m_client.IsEmpty())
		m_client = GET_P4REGPTR()->GetP4Client();
	if (m_user.IsEmpty())
		m_user = GET_P4REGPTR()->GetP4User();
	m_port.TrimLeft(_T('\"'));
	m_port.TrimRight(_T('\"'));

	// Fudge the location of the new window by telling it we last shutdown +10, +10 
	// from where we actually did
	// The registry will be rewritten by both P4wins when they shutdown.
	CRect rect= GET_P4REGPTR()->GetWindowPosition();
	rect.SetRect( rect.left+10, rect.top+10, rect.right+10, rect.bottom+10 );
	GET_P4REGPTR()->SetWindowPosition(rect);

	// Set default registry entries if requested
	if ( m_SetDefPort || m_SetDefClient || m_SetDefUser ) 
	{
		LONG rc = RegCreateKeyEx( HKEY_CURRENT_USER, sKey,
								0, NULL,
								REG_OPTION_NON_VOLATILE,
								KEY_WRITE, NULL,
								&hKey, &disposition );
		if (rc == ERROR_SUCCESS )
		{
			if (m_SetDefPort)
				rc = RegSetValueEx( hKey, _T("P4PORT"), NULL, REG_SZ,
					(LPBYTE)(LPCTSTR) m_port, m_port.GetLength( ) * sizeof(TCHAR) + 1);
			if (m_SetDefClient)
				rc = RegSetValueEx( hKey, _T("P4CLIENT"), NULL, REG_SZ,
					(LPBYTE)(LPCTSTR) m_client, m_client.GetLength( ) * sizeof(TCHAR) + 1);
			if (m_SetDefUser)
				rc = RegSetValueEx( hKey, _T("P4USER"), NULL, REG_SZ,
					(LPBYTE)(LPCTSTR) m_user, m_user.GetLength( ) * sizeof(TCHAR) + 1);
			RegCloseKey( hKey );
		}
		if (rc != ERROR_SUCCESS )
		{
			LPVOID lpMsgBuf;
			FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				rc,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf,
				0,
				NULL 
			);
			CString msg;
			msg.FormatMessage(IDS_FAILEDREGWRITE, (TCHAR *)lpMsgBuf);
			AfxMessageBox(msg, MB_OK|MB_ICONERROR );
		}
	}
	if (GET_P4REGPTR()->GetExpandFlag() == 1)
		GET_P4REGPTR()->AddMRUPcuPath(MainFrame()->GetCurrentItemPath());

	// and finally spawn a new P4win
    STARTUPINFO si;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;
    if( !GetModuleFileName(GetModuleHandle(NULL), path, sizeof(path)/sizeof(TCHAR)-1) ||
        !_stprintf(path + lstrlen(path), _T(" -q -p %s -c %s -u %s"), m_port, m_client, m_user) ||
        !CreateProcess(NULL, path, 
            NULL, NULL, 
#ifdef UNICODE
            FALSE, DETACHED_PROCESS | NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT, 
#else
            FALSE, DETACHED_PROCESS | NORMAL_PRIORITY_CLASS, 
#endif
            NULL, NULL, &si, &pi))
	{
		AfxMessageBox(IDS_UNABLE_TO_START_NEW_P4WIN_WINDOW, MB_OK | MB_ICONSTOP);
	}
	CDialog::OnOK();
}

BOOL CNewWindowDlg::EditPort()
{
	CString origPort = m_port;
	m_PortCombo.GetWindowText( m_port );

	m_port.TrimRight();
	m_port.TrimLeft();

	if (!m_port.GetLength())	// if the port field is empty, put back the original port
		m_port = origPort;

	if( m_port.GetAt(0) == _T('\"'))
	{
		m_port.TrimRight(_T('\"'));
		m_port.TrimLeft(_T('\"'));
	}
	if (m_port.GetLength() > 0)
		GET_P4REGPTR()->AddMRUPort( m_port, GET_P4REGPTR()->GetP4Charset() );
	return TRUE;
}

void CNewWindowDlg::OnSetdefall() // they pressed the "set all to defualt" button
{
	UpdateData();
	m_SetDefClient = TRUE;
	m_SetDefPort = TRUE;
	m_SetDefUser = TRUE;
	UpdateData(FALSE);
}

void CNewWindowDlg::OnRemove() // they want to remove a PCU string from the MRU list
{
	int i;
	UpdateData();

	CString txtStr;
	m_PCUcombo.GetWindowText(txtStr);
	CString pcuStr;
	if ((i = m_PCUcombo.GetCurSel()) >= 0)
	{
		m_PCUcombo.GetLBText(i, pcuStr);// get the current selection in the combobox
		if (pcuStr == txtStr)
		{
			m_PCUcombo.DeleteString(i);		// remove that string from the combobox
			m_PCUcombo.SetCurSel(-1);		// make nothing selected in the combobox
			m_client.Empty();
			m_port.Empty();					// clear the 3 p c u edit controls
			m_user.Empty();
			m_PortCombo.SetWindowText(_T(""));
			UpdateData(FALSE);
			m_RmvMRUPcu.AddHead(pcuStr);	// add this string to the list to be actually removed
		}									// when they press OK or NewWindow (can't actually
		else								// remove now cuz they might press Cancel!)
			MessageBeep(0);
	}
	else
	{
		MessageBeep(0);
	}
	m_PCUcombo.SetFocus();
}

void CNewWindowDlg::ReloadMRUPcu()	// reload the Reg's MRU strings and remove ones to be removed
{
	POSITION pos = m_RmvMRUPcu.GetHeadPosition();

	CString pcuStr;
	while (pos)		// while there are strings that are to be removed, ask P4Registry.cpp to remove them
	{
		pcuStr = m_RmvMRUPcu.GetNext(pos);
		GET_P4REGPTR()->RmvMRUPcu(pcuStr);
	}
}

void CNewWindowDlg::OnHelp() 
{
	AfxGetApp()->WinHelp(TASK_SWITCHING_CLIENT_WORKSPACE);
}

BOOL CNewWindowDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}

void CNewWindowDlg::OnStartwithmrupcu() 
{
	GetDlgItem(IDC_SETDEFPORT)->EnableWindow(FALSE);
	GetDlgItem(IDC_SETDEFCLIENT)->EnableWindow(FALSE);
	GetDlgItem(IDC_SETDEFUSER)->EnableWindow(FALSE);
	GetDlgItem(IDC_SETDEFALL)->EnableWindow(FALSE);
}

void CNewWindowDlg::OnStartwithdefaults() 
{
	GetDlgItem(IDC_SETDEFPORT)->EnableWindow(TRUE);
	GetDlgItem(IDC_SETDEFCLIENT)->EnableWindow(TRUE);
	GetDlgItem(IDC_SETDEFUSER)->EnableWindow(TRUE);
	GetDlgItem(IDC_SETDEFALL)->EnableWindow(TRUE);
}

LRESULT CNewWindowDlg::OnDoBrowseClients(WPARAM wParam, LPARAM lParam)
{
	OnBrowseClients();
	return 0;
}

void CNewWindowDlg::OnBrowseClients()
{
	if (m_bSamePort)
	{
		::SendMessage(MainFrame()->ClientWnd(), WM_FETCHOBJECTLIST, 
						(WPARAM)(this->m_hWnd), WM_BROWSECALLBACK1);
	}
	else if (EditPort())
	{
		CString old_port = GET_P4REGPTR()->GetP4Port();
		CString old_user = GET_P4REGPTR()->GetP4User();
		UpdateData(TRUE);
		GET_P4REGPTR()->SetP4Port(m_port, TRUE, FALSE, FALSE);
		GET_P4REGPTR()->SetP4User(m_user, TRUE, FALSE, FALSE);
		CCmd_Clients *pCmd= new CCmd_Clients;
		pCmd->Init( m_hWnd, RUN_ASYNC );
		if( pCmd->Run( ) )
			MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_CLIENT_LISTING) );
		else
			delete pCmd;
		SET_BUSYCURSOR();
		do
		{
			if ( !MainFrame()->PumpMessages( ) )
				break;
			Sleep(100);
		} while (SERVER_BUSY());
		GET_P4REGPTR()->SetP4Port(old_port, TRUE, FALSE, FALSE);
		GET_P4REGPTR()->SetP4User(old_user, TRUE, FALSE, FALSE);
	}
}

LRESULT CNewWindowDlg::OnP4ClientList(WPARAM wParam, LPARAM lParam)
{
	CCmd_Clients *pCmd= (CCmd_Clients *) wParam;

	if(!pCmd->GetError())
	{
		SET_APP_HALTED(TRUE);

		// Get the column names and count
		CStringArray cols;
		int nbrcols = MainFrame()->GetClientColNamesAndCount(cols);
		CString subkey  = _T("Client List");

		CObList const *clients = pCmd->GetList();
	    CObList *objs = new CObList;

		int index = 0;		// initialized only to make the compiler shutup
		BOOL isFiltered = GET_P4REGPTR()->GetClientFilteredFlags();

		if ( isFiltered )
			index = LoadClientList(clients, objs, nbrcols, TRUE);

		if( !isFiltered || objs->GetCount() == 0 )
			 index = LoadClientList(clients, objs, nbrcols, isFiltered = FALSE);

		if( objs->GetCount() == 0 )
		{
			CString txt;
			txt.FormatMessage(IDS_NO_s_AVAILABLE, LoadStringResource(IDS_PERFORCE_CLIENTS));
			AfxMessageBox(txt, MB_ICONEXCLAMATION);
			SET_APP_HALTED(FALSE);
		}
		else while (1)
		{
			CString msg;
			msg.FormatMessage( IDS_NUMBER_OF_CLIENTS_n, index );
			MainFrame()->UpdateStatus( msg );

			CString caption = LoadStringResource(IDS_PERFORCE_CLIENTS);
			if (isFiltered)
				caption += LoadStringResource(IDS_FILTERED_INPAREN);

			// Display the dialog box.
			CP4ListBrowse dlg(&(MainFrame()->GetClientView()->GetListCtrl()));
			dlg.SetP4ObjectFont(GetFont());
			dlg.SetP4ObjectType(P4CLIENT_SPEC);
			dlg.SetP4ObjectList(objs);
			dlg.SetP4ObjectCols(&cols);
			dlg.SetP4ObjectCurr(&m_client);
			dlg.SetP4ObjectSKey(&subkey);
			dlg.SetP4ObjectCaption(&caption);
			dlg.SetP4ObjectImage(CP4ViewImageList::VI_CLIENT);
			dlg.SetP4ObjectIsFiltered(isFiltered);

			CString old_port;
			CString old_user;
			if (!m_bSamePort)
			{
				old_port = GET_P4REGPTR()->GetP4Port();
				old_user = GET_P4REGPTR()->GetP4User();
				GET_P4REGPTR()->SetP4Port(m_port, TRUE, FALSE, FALSE);
				GET_P4REGPTR()->SetP4User(_T(""), TRUE, FALSE, FALSE);
			}
			int retcode= dlg.DoModal();
			if (!m_bSamePort)
			{
				GET_P4REGPTR()->SetP4Port(old_port, TRUE, FALSE, FALSE);
				GET_P4REGPTR()->SetP4User(old_user, TRUE, FALSE, FALSE);
			}
			SET_APP_HALTED(FALSE);

			CString *objname= dlg.GetSelectedP4Object();
			if(retcode == IDOK && !objname->IsEmpty())
				::SendMessage(m_hWnd, WM_BROWSECALLBACK1, 0, (LPARAM)objname);
			else if (retcode == IDC_REFRESH)
			{
				if (isFiltered)
				{
					isFiltered = FALSE;
					index = LoadClientList(clients, objs, nbrcols, FALSE);
					continue;
				}
				else
					PostMessage(WM_BROWSECLIENTS, 0, 0);
			}

			// Delete the object list
			for(POSITION pos=objs->GetHeadPosition(); pos!=NULL; )
				delete (CP4Object *) objs->GetNext(pos);
			break;
		}
		delete objs;
		for(POSITION pos= clients->GetHeadPosition(); pos != NULL; )
		{
       		CP4Client *client = (CP4Client *) clients->GetNext(pos);
			delete client;
		}
	}
	delete pCmd;
	MainFrame()->ClearStatus();
	return 0;
}

int CNewWindowDlg::LoadClientList(CObList const *clients, CObList *objs, int nbrcols, BOOL bFilter)
{
	CString str;
	int index = 0;
	for(POSITION pos= clients->GetHeadPosition(); pos != NULL; index++)
	{
        CP4Client *client = (CP4Client *) clients->GetNext(pos);
		if (bFilter)
		{
			if (MainFrame()->IsClientFilteredOut(client, &m_user))
				continue;
		}
		int subitem;
		CP4Object *newObj= new CP4Object();
		for (subitem = -1; ++subitem < nbrcols; )
		{
			switch(subitem)
			{
			case 0:
				str = client->GetClientName();
				break;
			case 1:
				str = client->GetOwner();
				break;
			case 2:
				str = client->GetHost();
				break;
			case 3:
				str = client->GetDate();
				break;
			case 4:
				str = client->GetRoot();
				break;
			case 5:
				str = client->GetDescription();
				break;
			default:
				ASSERT(0);
				continue;
			}
			if (!subitem)
				newObj->Create(str);
			else
				newObj->AddField(str);
		}
		objs->AddHead(newObj);
	}
	return index;
}

LRESULT CNewWindowDlg::OnBrowseClientsCallBack(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
	CString *str = (CString *)lParam;
	m_client = *str;
	UpdateData(FALSE);
	return 0;
}

LRESULT CNewWindowDlg::OnDoBrowseUsers(WPARAM wParam, LPARAM lParam)
{
	OnBrowseUsers();
	return 0;
}

void CNewWindowDlg::OnBrowseUsers()
{
	if (m_bSamePort)
	{
		::SendMessage(MainFrame()->UserWnd(), WM_FETCHOBJECTLIST, 
						(WPARAM)(this->m_hWnd), WM_BROWSECALLBACK2);
	}
	else if (EditPort())
	{
		CString old_port = GET_P4REGPTR()->GetP4Port();
		CString old_user = GET_P4REGPTR()->GetP4User();
		GET_P4REGPTR()->SetP4Port(m_port, TRUE, FALSE, FALSE);
		GET_P4REGPTR()->SetP4User(_T(""), TRUE, FALSE, FALSE);
		CCmd_Users *pCmd= new CCmd_Users;
		pCmd->Init( m_hWnd, RUN_ASYNC );
		if( pCmd->Run( ) )
			MainFrame()->UpdateStatus( LoadStringResource(IDS_REQUESTING_USER_LISTING) );
		else
			delete pCmd;
		SET_BUSYCURSOR();
		int i = 0;
		do
		{
			Sleep(200);
		} while (SERVER_BUSY() && ++i < 10);
		GET_P4REGPTR()->SetP4Port(old_port, TRUE, FALSE, FALSE);
		GET_P4REGPTR()->SetP4User(old_user, TRUE, FALSE, FALSE);
	}
}

LRESULT CNewWindowDlg::OnP4UserList(WPARAM wParam, LPARAM lParam)
{
	CCmd_Users *pCmd= (CCmd_Users *) wParam;

	if(!pCmd->GetError())
	{
		SET_APP_HALTED(TRUE);

		// Get the column names and count
		CStringArray cols;
		int nbrcols = MainFrame()->GetUserColNamesAndCount(cols);

		CString str;
		CString caption = LoadStringResource(IDS_PERFORCE_USERS);
		CString subkey  = _T("User List");

		CObList const *users = pCmd->GetList();
	    CObList *objs = new CObList;

		int index = 0;
		for(POSITION pos= users->GetHeadPosition(); pos != NULL; index++)
		{
        	CP4User *user = (CP4User *) users->GetNext(pos);
			int subitem;
			CP4Object *newObj= new CP4Object();
			for (subitem = -1; ++subitem < nbrcols; )
			{
				switch(subitem)
				{
				case 0:
					str = user->GetUserName();
					break;
				case 1:
					str = user->GetEmail();
					break;
				case 2:
					str = user->GetFullName();
					break;
				case 3:
					str = user->GetLastAccess();
					break;
				default:
					ASSERT(0);
					continue;
				}
				if (!subitem)
					newObj->Create(str);
				else
					newObj->AddField(str);
			}
			objs->AddHead(newObj);
			delete user;
		}

		if( objs->GetCount() == 0 )
		{
			CString txt;
			txt.FormatMessage(IDS_NO_s_AVAILABLE, caption);
			AfxMessageBox(txt, MB_ICONEXCLAMATION);
			SET_APP_HALTED(FALSE);
			delete objs;
			return 0;
		}

		CString msg;
		msg.FormatMessage( IDS_NUMBER_OF_USERS_n, index );
		MainFrame()->UpdateStatus( msg );

		// Display the dialog box.
		CP4ListBrowse dlg(&(MainFrame()->GetUserView()->GetListCtrl()));
		dlg.SetP4ObjectFont(GetFont());
		dlg.SetP4ObjectType(P4USER_SPEC);
		dlg.SetP4ObjectList(objs);
		dlg.SetP4ObjectCols(&cols);
		dlg.SetP4ObjectCurr(&m_user);
		dlg.SetP4ObjectSKey(&subkey);
		dlg.SetP4ObjectCaption(&caption);
		dlg.SetP4ObjectImage(CP4ViewImageList::VI_USER);

		CString old_port;
		CString old_user;
		if (!m_bSamePort)
		{
			old_port = GET_P4REGPTR()->GetP4Port();
			old_user = GET_P4REGPTR()->GetP4User();
			GET_P4REGPTR()->SetP4Port(m_port, TRUE, FALSE, FALSE);
			GET_P4REGPTR()->SetP4User(_T(""), TRUE, FALSE, FALSE);
		}
		int retcode= dlg.DoModal();
		if (!m_bSamePort)
		{
			GET_P4REGPTR()->SetP4Port(old_port, TRUE, FALSE, FALSE);
			GET_P4REGPTR()->SetP4User(old_user, TRUE, FALSE, FALSE);
		}
		SET_APP_HALTED(FALSE);

		// Delete the object list
		for(POSITION pos=objs->GetHeadPosition(); pos!=NULL; )
			delete (CP4Object *) objs->GetNext(pos);
		delete objs;

		CString *objname= dlg.GetSelectedP4Object();
		if(retcode == IDOK && !objname->IsEmpty())
			::SendMessage(m_hWnd, WM_BROWSECALLBACK2, 0, (LPARAM)objname);
		else if (retcode == IDC_REFRESH)
			PostMessage(WM_BROWSEUSERS, 0, 0);
	}
	delete pCmd;
	MainFrame()->ClearStatus();
	return 0;
}

LRESULT CNewWindowDlg::OnBrowseUsersCallBack(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
	CString *str = (CString *)lParam;
	m_user = *str;
	UpdateData(FALSE);
	return 0;
}
