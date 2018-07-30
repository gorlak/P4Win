// FindFilesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "DepotView.h"
#include "FindFilesDlg.h"
#include "MainFrm.h"
#include "P4Command.h"
#include "hlp\p4win.hh"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindFilesDlg dialog


CFindFilesDlg::CFindFilesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindFilesDlg::IDD, pParent)
{
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CFindFilesDlg)
	//}}AFX_DATA_INIT

	m_ExitCode = 0;
	m_IsMinimized = FALSE;
	m_InitRect.SetRect(0,0,0,0);
	m_InitDialogDone=FALSE;
	m_WinPos.SetWindow( this, _T("FindFilesDlg") );
	m_CloseAfterFind = GET_P4REGPTR()->GetCloseAfterFind();
}


void CFindFilesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindFilesDlg)
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Control(pDX, IDC_EDIT1, m_EditCombo);
	DDX_Check(pDX, IDC_CHECK1, m_CloseAfterFind);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindFilesDlg, CDialog)
	//{{AFX_MSG_MAP(CFindFilesDlg)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_CBN_SETFOCUS(IDC_EDIT1, OnSetfocusEdit1)
	ON_LBN_SETFOCUS(IDC_LIST1, OnSetfocusList1)
	ON_LBN_DBLCLK(IDC_LIST1, OnDblclkList)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDD_DEPOT_FILTER, OnDepotFilter)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_WM_HELPINFO()
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CFindFilesDlg::Init(CString &filespec, CStringList *filelist, 
						 CDepotTreeCtrl *depotView, BOOL bDeselect)
{
	m_Edit = filespec;
	m_pList = filelist;
	m_pDepotView = depotView;
	m_Deselect = bDeselect;
}


/////////////////////////////////////////////////////////////////////////////
// CFindFilesDlg message handlers

BOOL CFindFilesDlg::OnInitDialog() 
{
	POSITION pos;

	CDialog::OnInitDialog();

	if (m_pParent)
		MainFrame()->SetModelessWnd(this);

	GetWindowRect(&m_InitRect);
	m_LastRect = m_InitRect;

	// Make sure the combo is empty
	m_EditCombo.Clear();
	m_EditCombo.ResetContent();

	// If we have a string passed in, put it in the combo's list box
	int limit;
	if (!m_Edit.IsEmpty())
	{
		m_EditCombo.AddString(m_Edit);
		limit = MAX_MRU_FINDSTRS - 1;
	}
	else
		limit = MAX_MRU_FINDSTRS;

	// Load all MRU find strings into combo's list box
	int i;
	for(i=0; i < limit; i++)
	{
		CString str = GET_P4REGPTR()->GetMRUFindStr(i);
		if ((str.GetLength() > 0) && (str != m_Edit))
			m_EditCombo.AddString(str);
	}

	m_EditCombo.SetWindowText( m_Edit );

	m_List.ResetContent();
	for( pos = m_pList->GetHeadPosition(); pos != NULL; )
		m_List.AddString(m_pList->GetNext( pos ));

	// We also need to refrsh *m_pList to ensure
	// it is in the same no-case order as the depot
	// so that Find Next & Prev will work correctly.
	// This can be done by reloading *m_pList with
	// the contents of m_List since it's been sorted.
	if (m_List.GetCount())
	{
		m_pList->RemoveAll();
		CString str;
		for (i = -1; ++i < m_List.GetCount(); )
		{
			m_List.GetText(i, str);
			m_pList->AddTail(str);
		}
	}

	// Restore window position
	m_InitDialogDone = TRUE;
	m_WinPos.RestoreWindowPosition();

	if (m_Deselect)
	{
		GotoDlgCtrl(GetDlgItem(IDC_EDIT1));
		GetDlgItem(IDC_EDIT1)->PostMessage(WM_KEYDOWN, VK_END, 0);
		return FALSE;
	}	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFindFilesDlg::CheckEditField()
{
	UpdateData( );
	if(m_EditCombo.GetCurSel() != CB_ERR)
		m_EditCombo.GetLBText(m_EditCombo.GetCurSel(), m_Edit);
	else
		m_EditCombo.GetWindowText( m_Edit );
	if (m_Edit.GetLength() == 0)
	{
		MessageBeep(0);
		return;
	}
	if ((m_Edit.Find('/') == -1) && (m_Edit.Find('\\') == -1))
	{
		CString client = GET_P4REGPTR()->GetP4Client();
		m_Edit = "//" + client + "/..." + m_Edit + "...";
		UpdateData(FALSE);
	}
	m_Edit.Replace(_T("...*"), _T("..."));
	GET_P4REGPTR()->AddMRUFindStr( m_Edit );
}

void CFindFilesDlg::OnOK() 
{
	m_WinPos.SaveWindowPosition();
	if(UpdateData(TRUE))
	{
		if (m_CloseAfterFind != GET_P4REGPTR()->GetCloseAfterFind())
			GET_P4REGPTR()->SetCloseAfterFind( m_CloseAfterFind );
	}
	if (m_ExitCode == 1)
	{
		CheckEditField();
	}
	else
	{
		int i, cursel;
		UpdateData( );
		m_SelStr.Empty();
		if ((cursel = m_List.GetCurSel()) != LB_ERR)
		{
			m_List.GetText(cursel, m_SelStr);
			if ((i = m_SelStr.Find('#')) != -1)
				m_SelStr = m_SelStr.Left(i);
		}
	}
	if (m_pParent)
		m_pParent->PostMessage(WM_P4ENDFINDFILES, 0, (LPARAM)this);
	if (m_CloseAfterFind || m_ExitCode != 2)
		CDialog::OnOK();
}

void CFindFilesDlg::OnHelp() 
{
	AfxGetApp()->WinHelp(TASK_FIND_FILES_DEPOT);
}

BOOL CFindFilesDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}

void CFindFilesDlg::OnCancel() 
{
	m_ExitCode = 0;
	if (m_pParent)
		m_pParent->PostMessage(WM_P4ENDFINDFILES, 0, (LPARAM)this);
	CDialog::OnCancel();
}

void CFindFilesDlg::OnSetfocusEdit1() 
{
	m_ExitCode = 1;
}

void CFindFilesDlg::OnSetfocusList1() 
{
	m_ExitCode = 2;
}

void CFindFilesDlg::OnDblclkList() 
{
	OnOK();	
}

void CFindFilesDlg::OnSize(UINT nType, int cx, int cy) 
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

	// Widen the edit field by the change in width (dx)
	CWnd *pWnd = GetDlgItem( IDC_EDIT1 );
	pWnd->GetWindowRect(&rect);
	pWnd->GetWindowRect(&rect);
	pWnd->SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								   rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);

	// Widen the list box by the change in width (dx) and
	// heighten the list box by the change in height (dy)
	m_List.GetWindowRect(&rect);
	m_List.SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								    rect.bottom - rect.top + dy, SWP_NOMOVE | SWP_NOZORDER);

	// Slide the checkbox down by the change in height
	pWnd = GetDlgItem(IDC_CHECK1);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// Slide the buttons to the right by the change in width
	// and down by the change in height
	pWnd = GetDlgItem(IDOK);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);

	pWnd = GetDlgItem(IDD_DEPOT_FILTER);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);

	pWnd = GetDlgItem(IDC_COPY);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);

	pWnd = GetDlgItem(IDCANCEL);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);

	pWnd = GetDlgItem(IDHELP);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	pWnd->InvalidateRect(NULL, TRUE);
}

void CFindFilesDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	if (m_InitRect.Height())
	{
		lpMMI->ptMinTrackSize.x= m_InitRect.Width();
		lpMMI->ptMinTrackSize.y= m_InitRect.Height();
	}
}

void CFindFilesDlg::OnDepotFilter() 
{
	m_WinPos.SaveWindowPosition();
	m_pDepotView->EmptyDepotFilter();
	if ((m_ExitCode += 2) == 3)
		CheckEditField();
	else
	{
		CStringList list;
		CString str;
		int count = m_List.GetCount();
		for (int i = -1; ++i < count; )
		{
			m_List.GetText(i, str);
			list.AddHead( str );
		}
		m_pDepotView->LoadDepotFilterList(&list);
	}
	m_pDepotView->SetDepotFilterType(DFT_FIND);
	if (m_pParent)
		m_pParent->PostMessage(WM_P4ENDFINDFILES, 0, (LPARAM)this);
	CDialog::OnOK();
}

void CFindFilesDlg::OnCopy() 
{
	if (m_ExitCode == 1)
	{
		GotoDlgCtrl(GetDlgItem(IDC_EDIT1));
		::SendMessage(GetDlgItem(IDC_EDIT1)->m_hWnd, WM_COPY, 0, 0);
	}
	else
	{
		int cursel;
		UpdateData( );
		if ((cursel = m_List.GetCurSel()) != LB_ERR)
		{
			CString selStr;
			m_List.GetText(cursel, selStr);
			CopyTextToClipboard(selStr);
		}
		else MessageBeep(0);
		GotoDlgCtrl(GetDlgItem(IDC_LIST1));
	}
}

// This signals the closing of a modeless dialog
// to MainFrame which will delete the 'this' object
void CFindFilesDlg::OnDestroy()
{
	if (m_pParent)
		::PostMessage(MainFrame()->m_hWnd, WM_P4DLGDESTROY, 0, (LPARAM)this);
}

void CFindFilesDlg::OnSysCommand(UINT nID, LPARAM lParam) 
{
	switch(nID)
	{
	case SC_MINIMIZE:
		GetDesktopWindow()->ArrangeIconicWindows();
		break;
	}

	CDialog::OnSysCommand(nID, lParam);
}
