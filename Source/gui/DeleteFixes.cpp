// DeleteFixes.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "reviewlist.h"
#include "DeleteFixes.h"
#include "MainFrm.h"
#include "P4Command.h"
#include "cmd_describe.h"
#include "hlp\p4win.hh"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeleteFixes dialog


CDeleteFixes::CDeleteFixes(CWnd* pParent /*=NULL*/)
	: CDialog(CDeleteFixes::IDD, pParent)
{
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CDeleteFixes)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_OldChgWnd = pParent->m_hWnd;
	m_InitRect.SetRect(0,0,0,0);
	m_InitDialogDone = m_IsMinimized = FALSE;
	m_WinPos.SetWindow( this, _T("DeleteFixes") );
	m_SortFlag = 0;
}

CDeleteFixes::~CDeleteFixes()
{
	delete m_List;
}


void CDeleteFixes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeleteFixes)
	DDX_Control(pDX, IDC_JOBSTATUS, m_JobStatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDeleteFixes, CDialog)
	//{{AFX_MSG_MAP(CDeleteFixes)
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_DESCRIPTION, OnDescribeJob)
	ON_BN_CLICKED(ID_HELP, OnHelp)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeleteFixes message handlers


BOOL CDeleteFixes::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (m_pParent)
		MainFrame()->SetModelessWnd(this);

	GetWindowRect(&m_InitRect);
	m_LastRect = m_InitRect;

	SetWindowText(m_Caption);

	CRect rect;
	CWnd *pCtrl=GetDlgItem(IDC_LISTRECT);
	pCtrl->GetWindowRect(&rect);
	ScreenToClient(&rect);

	DWORD style= WS_CHILD | WS_BORDER | WS_VISIBLE | LBS_HASSTRINGS | WS_HSCROLL
		 | LBS_EXTENDEDSEL | LBS_OWNERDRAWFIXED | WS_VSCROLL | WS_TABSTOP | LBS_NOINTEGRALHEIGHT;

	m_List = new CReviewList;
	
	m_List->CreateEx(WS_EX_CLIENTEDGE, MainFrame()->m_ReviewListClass, _T(""),style,
                 rect.left, rect.top, rect.Width(), rect.Height(),
				 this->m_hWnd, (HMENU) IDC_LIST);

	LoadFixes();

	if (!LoadJobStatusComboBox())
	{
		GetDlgItem(IDC_JOBSTATUSPROMPT)->ShowWindow( SW_HIDE );
		GetDlgItem(IDC_JOBSTATUS)->ShowWindow( SW_HIDE );
	}
	OnSelchangeList();

	// Restore window position
	m_InitDialogDone = TRUE;
	m_WinPos.RestoreWindowPosition();

	ShowWindow(SW_SHOW);
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDeleteFixes::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	m_List->SetFocus();
}

void CDeleteFixes::LoadFixes()
{
	CString s;
	int maxlgth = 0;
	for (POSITION pos = m_SelectedList->GetHeadPosition(); pos != NULL; )
	{
		s = m_SelectedList->GetNext(pos);
		if( !s.IsEmpty() )
		{
			maxlgth = max(maxlgth, s.GetLength());
			m_List->AddString( s );
			m_SortFlag |= 2;
		}
		// Get text metrics for font so we can figure scroll extent
		//
		TEXTMETRIC tm;
		CDC *pDC= GetDC();
		pDC->GetTextMetrics( &tm );
		ReleaseDC( pDC );

		m_List->SetHorizontalExtent( tm.tmAveCharWidth * maxlgth + 40 );
	}
}

BOOL CDeleteFixes::LoadJobStatusComboBox()
{
	int i;
	m_JobStatus.AddString(LoadStringResource(IDS_PAREN_NOCHANGE_PAREN));
	m_JobStatus.SetCurSel(0);
	if (GET_SERVERLEVEL() < 10)
		return FALSE;

	i = m_pJobSpec->Find(_T("\nFields:\n\t"));
	if (i == -1)
		return FALSE;
	i = m_pJobSpec->Find(_T("\n\t102 "), i);
	if (i == -1)
		return FALSE;
	i += sizeof(_T("\n\t102 "))/sizeof(TCHAR) - 1;
	int j = m_pJobSpec->Find(_T(' '), i);
	if (j == -1)
		return FALSE;
	CString name = m_pJobSpec->Mid(i, j-i);
	i = m_pJobSpec->Find(_T("\nValues:"), j);
	if (i == -1)
	{
		CString dashed = _T("\nValues");
		dashed += _T("-") + name;
		i = m_pJobSpec->Find(dashed, j);
		if (i == -1)
			return FALSE;
	}
	i = m_pJobSpec->Find(name, i);
	if (i == -1)
		return FALSE;
	i += name.GetLength();
	while (1)
	{
		TCHAR c = m_pJobSpec->GetAt(++i);
		if ((c != _T(':')) && (c != _T(' ')) && (c != _T('\t')))
			break;
	}
	j = m_pJobSpec->Find(_T('\n'), i);
	if (j == -1)
		return FALSE;
	CString values = m_pJobSpec->Mid(i, j-i);

	while((i = values.Find(_T('/'))) != -1)
	{
		CString value = values.Mid(0,i);
		m_JobStatus.AddString(value);
		values = values.Mid(i+1);
	}
	m_JobStatus.AddString(values);

	// at this point we have loaded the dropdown; now figure out the initial value
	i = m_pJobSpec->Find(_T("\nPresets:"));
	if (i == -1)
		return TRUE;
	i = m_pJobSpec->Find(name, i);
	if (i == -1)
		return TRUE;
	i += name.GetLength();
	while (1)
	{
		TCHAR c = m_pJobSpec->GetAt(++i);
		if ((c != _T(':')) && (c != _T(' ')) && (c != _T('\t')))
			break;
	}
	j = m_pJobSpec->Find(_T('\n'), i);
	if (j == -1)
		return TRUE;
	CString preset = m_pJobSpec->Mid(i, j-i);
	if ((i = m_JobStatus.FindStringExact(-1, preset)) != CB_ERR)
		m_JobStatus.SetCurSel(i);

	return TRUE;
}

void CDeleteFixes::OnSelectAll() 
{
	for ( int i = 0; i < m_List->GetCount ( ); i++ )
		m_List->SetCheck( i, 1 );
}

void CDeleteFixes::OnUnselectAll() 
{
	for ( int i = 0; i < m_List->GetCount ( ); i++ )
		m_List->SetCheck( i, 0 );
}

void CDeleteFixes::OnCancel() 
{
	::PostMessage(m_OldChgWnd, m_ReturnMsg, (WPARAM)IDCANCEL, (LPARAM)0);
}

void CDeleteFixes::OnOK() 
{
	CString txt;

	m_OutputList->RemoveAll();
	for ( int i = 0; i < m_List->GetCount( ); i++ )
	{
		if (m_List->GetCheck(i))
		{
			m_List->GetText( i, txt );
			m_OutputList->AddTail( txt );
		}
	}

	if (m_OutputList->GetCount() == 0)
	{
		if (IDYES == AfxMessageBox(IDS_NOTHINGSELECTED_CLOSE, MB_YESNO|MB_DEFBUTTON2))
			OnCancel();
		return;
	}

	m_WinPos.SaveWindowPosition();

	int cursel = m_JobStatus.GetCurSel();
	if (cursel != CB_ERR)
		m_JobStatus.GetLBText(cursel, *m_JobStatusValue);
	if (*m_JobStatusValue == LoadStringResource(IDS_PAREN_NOCHANGE_PAREN))
		(*m_JobStatusValue).Empty();
	::PostMessage(m_OldChgWnd, m_ReturnMsg, (WPARAM)IDOK, (LPARAM)FALSE);
}

void CDeleteFixes::OnHelp() 
{
	AfxGetApp()->WinHelp(TASK_MANAGING_LABELS);
}

BOOL CDeleteFixes::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}

void CDeleteFixes::OnDescribeJob()
{
	int i;
	CString str;

	if ((i = m_List->GetCurSel()) != LB_ERR)
	{
		m_List->GetText(i, str);
		if ((i = str.Find(_T(' '))) != -1)
		{
			str = str.Left(i);
			CCmd_Describe *pCmd= new CCmd_Describe;
			pCmd->Init( MainFrame()->JobWnd(), RUN_ASYNC );
			if( pCmd->Run( P4JOB_SPEC, str ) )
				MainFrame()->UpdateStatus(LoadStringResource(IDS_FETCHING_JOB_SPEC));
			else
				delete pCmd;
		}
	}
}

void CDeleteFixes::OnSelchangeList() 
{
	GetDlgItem(IDC_DESCRIPTION)->EnableWindow(m_List->GetCurSel() != LB_ERR);
}

BOOL CDeleteFixes::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(SERVER_BUSY())
		return SET_BUSYCURSOR();
	else
		return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CDeleteFixes::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	if (!m_InitDialogDone)
		return;

	if (nType == SIZE_MINIMIZED)
	{
		m_IsMinimized = TRUE;
		return;
	}
	else if (m_IsMinimized)
	{
		m_IsMinimized = FALSE;
		return;
	}

	// Compute the change in width (dx)
	CRect rect;
	GetWindowRect(&rect);
	int dx = rect.Width() - m_LastRect.Width();
	int dy = rect.Height() - m_LastRect.Height();
	// Save the new size
	m_LastRect = rect;

	// Widen the list box by the change in width (dx) and
	// heighten the list box by the change in height (dy)
	CWnd *pWnd=GetDlgItem(IDC_LIST);
	pWnd->GetWindowRect(&rect);
	pWnd->SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								    rect.bottom - rect.top + dy, SWP_NOMOVE | SWP_NOZORDER);
	// Slide the Describe button down by the change in height
	pWnd = GetDlgItem(IDC_DESCRIPTION);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// Slide the Status label down by the change in height
	pWnd = GetDlgItem(IDC_JOBSTATUSPROMPT);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// Slide the Status field down by the change in height
	pWnd = GetDlgItem(IDC_JOBSTATUS);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// Slide the other buttons to the right by the change in width
	// and down by the change in height
	pWnd = GetDlgItem(IDOK);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(IDCANCEL);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(ID_HELP);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	InvalidateRect(NULL, TRUE);
}

void CDeleteFixes::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	if (m_InitRect.Height())
	{
		lpMMI->ptMinTrackSize.x= m_InitRect.Width();
		lpMMI->ptMinTrackSize.y= m_InitRect.Height();
	}
}

// This signals the closing of a modeless dialog
// to MainFrame which will delete the 'this' object
void CDeleteFixes::OnDestroy()
{
	::PostMessage(MainFrame()->m_hWnd, WM_P4DLGDESTROY, 0, (LPARAM)this);
}

void CDeleteFixes::OnSysCommand(UINT nID, LPARAM lParam) 
{
	switch(nID)
	{
	case SC_MINIMIZE:
		GetDesktopWindow()->ArrangeIconicWindows();
		break;
	}

	CDialog::OnSysCommand(nID, lParam);
}
