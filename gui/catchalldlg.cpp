#include "stdafx.h"
#include "p4win.h"
#include "viewerdlg.h"
#include "UserView.h"
#include "Mainfrm.h"
#include "catchalldlg.h"
#include "..\SpecDescDlg.h"
#include "hlp\p4win.hh"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CJobFilter dialog


CJobFilter::CJobFilter(CWnd* pParent /*=NULL*/)
	: CDialog(CJobFilter::IDD, pParent)
{
	//{{AFX_DATA_INIT(CJobFilter)
	m_Filter = _T("");
	//}}AFX_DATA_INIT
	m_InitRect.SetRect(0,0,0,0);
}


void CJobFilter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJobFilter)
	DDX_Control(pDX, ID_JOBVIEW, m_JobViewBtn);
	DDX_Control(pDX, ID_RMVJOBFILTER, m_RmvFilter);
	DDX_Control(pDX, ID_HELPJOBFILTER, m_Help);
	DDX_Control(pDX, IDC_JOB_FILTER, m_ChangeCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CJobFilter, CDialog)
	//{{AFX_MSG_MAP(CJobFilter)
	ON_BN_CLICKED(ID_HELPJOBFILTER, OnHelpJobFilter)
	ON_BN_CLICKED(ID_RMVJOBFILTER, OnRmvJobFilter)
	ON_BN_CLICKED(ID_JOBVIEW, OnJobView)
	ON_BN_CLICKED(ID_FORMINFO, OnFormInfo)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_P4DESCRIBE, OnP4Describe )
    ON_MESSAGE(WM_P4ENDDESCRIBE, OnP4EndDescribe )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJobFilter message handlers


BOOL CJobFilter::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// We want to position the job filter dialog in the upper left corner 
	// of the job view pane which has provided it's screen coords in m_top, 
	// m_left & m_right.  However if the dialog box is longer than the job
	// view pane, we need to slide it to the left so that the right edges
	// line up, so get our screen coords for calc'ing the width of our box.
	// We also want to resize the dialogbox to its previous width;
	// so get the previous width, and resize and reposition the dialogbox.

	int w = GET_P4REGPTR()->GetJobFilterWidth();
	m_left = min(m_left, m_right - w);	// calc horiz position
	GetWindowRect(&m_InitRect);

	SetWindowPos( NULL, m_left, m_top, w, m_InitRect.bottom - m_InitRect.top, SWP_NOZORDER );
	
	LoadFilterComboBox();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CJobFilter::LoadFilterComboBox()
{
	// Make sure the combo is empty
	m_ChangeCombo.ResetContent();
	m_ChangeCombo.Clear();

	// Load all MRU filters into list box
	for(int i=0; i < MAX_MRU_FILTERS; i++)
	{
		if(GET_P4REGPTR()->GetMRUFilter(i).GetLength() > 0)
			m_ChangeCombo.AddString(GET_P4REGPTR()->GetMRUFilter(i));
	}

	// Select the first item
	m_ChangeCombo.SetCurSel(0);
}

void CJobFilter::OnOK() 
{
	UpdateData( );
	if(m_ChangeCombo.GetCurSel() != CB_ERR)
		m_ChangeCombo.GetLBText(m_ChangeCombo.GetCurSel(), m_Filter);
	else
		m_ChangeCombo.GetWindowText( m_Filter );
	if (m_Filter.GetLength() > 0)					// if new filter is not blank
		GET_P4REGPTR()->AddMRUFilter( m_Filter );	// save as most recently used in Reg

	CRect rect;
	GetWindowRect(&rect);
	if(!GET_P4REGPTR()->SetJobFilterWidth( rect.Width() ) )
		AfxMessageBox( IDS_BAD_REGISTRY,  MB_ICONSTOP );
	CDialog::OnOK();
}


void CJobFilter::OnHelpJobFilter() 
{
	AfxGetApp()->WinHelp(TASK_SPECIFYING_JOB_FILTERS);
}

BOOL CJobFilter::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelpJobFilter();
	return TRUE;
}

void CJobFilter::OnRmvJobFilter() 
{
	CString rmvFilter;

	UpdateData( );

	if(m_ChangeCombo.GetCurSel() != CB_ERR)
		m_ChangeCombo.GetLBText(m_ChangeCombo.GetCurSel(), rmvFilter);
	else
		m_ChangeCombo.GetWindowText( rmvFilter );
	if (rmvFilter.GetLength() > 0)					// if filter is not blank
		GET_P4REGPTR()->RmvMRUFilter( rmvFilter );	// remove it from Reg

	// Reload combobox
	LoadFilterComboBox();

	UpdateData(FALSE);
	GotoDlgCtrl(GetDlgItem(IDC_JOB_FILTER));
}

void CJobFilter::OnJobView() 
{
	// Run p4 user to get the server version
	CCmd_Describe *pCmd = new CCmd_Describe;
	pCmd->Init( NULL, RUN_SYNC );
	BOOL cmdStarted= pCmd->Run( P4USER_SPEC, GET_P4REGPTR()->GetP4User() );
	if(cmdStarted && !pCmd->GetError())
	{
		CString jobView = TheApp()->GetClientSpecField( _T("JobView"), pCmd->GetDescription());

		m_ChangeCombo.SetCurSel(-1);
		m_ChangeCombo.SetWindowText( jobView );
		UpdateData(FALSE);
		GotoDlgCtrl(GetDlgItem(IDC_JOB_FILTER));
	}
	else MessageBeep(0);
	delete pCmd;
}

void CJobFilter::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	CWnd *pList=GetDlgItem(IDC_JOB_FILTER);
	if (!pList || !IsWindow(pList->m_hWnd))
		return;

	int w;
	CRect rect;

	GetClientRect(&rect);
	int x = rect.Width();
	ClientToScreen(&rect);
	int edge = rect.left;
	
	pList->GetClientRect(&rect);
	pList->ClientToScreen(&rect);
	pList->SetWindowPos(NULL, 0, 0, 
		w = x - ((rect.left - edge)*2), rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);

	POINT pt;
	pt.x = rect.left + w;
	pt.y = rect.top;
	ScreenToClient(&pt);
	int end = pt.x;

	CWnd *pGbox=GetDlgItem(IDC_GROUPBOX);
	pGbox->GetClientRect(&rect);
	pGbox->ClientToScreen(&rect);
	pGbox->SetWindowPos(NULL, 0, 0, 
		x - ((rect.left - edge)*2), rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);

	CWnd *pButton=GetDlgItem(ID_RMVJOBFILTER);
	pButton->InvalidateRect(NULL, TRUE);
	pButton->GetWindowRect(&rect);
	w=rect.Width();
	int xtra = w/10;
	pt.x = rect.left;
	pt.y = rect.top;
	ScreenToClient(&pt);
	int y = pt.y;
	pButton->SetWindowPos(NULL, end - w, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pButton=GetDlgItem(ID_FORMINFO);
	pButton->InvalidateRect(NULL, TRUE);
	pButton->SetWindowPos(NULL, end - (w*2 + xtra), y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pButton=GetDlgItem(ID_JOBVIEW);
	pButton->InvalidateRect(NULL, TRUE);
	pButton->SetWindowPos(NULL, end - (w*3 + xtra*2), y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pButton=GetDlgItem(ID_HELPJOBFILTER);
	pButton->GetWindowRect(&rect);
	pt.x = rect.left;
	pt.y = rect.top;
	ScreenToClient(&pt);
	y = pt.y;
	pButton->InvalidateRect(NULL, TRUE);
	pButton->SetWindowPos(NULL, end - w, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pButton=GetDlgItem(IDCANCEL);
	pButton->InvalidateRect(NULL, TRUE);
	pButton->SetWindowPos(NULL, end - (w*2 + xtra), y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pButton=GetDlgItem(IDOK);
	pButton->InvalidateRect(NULL, TRUE);
	pButton->SetWindowPos(NULL, end - (w*3 + xtra*2), y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void CJobFilter::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	if (m_InitRect.Height())
	{
		lpMMI->ptMinTrackSize.x= m_InitRect.Width();
		lpMMI->ptMinTrackSize.y= lpMMI->ptMaxTrackSize.y= m_InitRect.Height();
	}
}

void CJobFilter::OnClearfilter() 
{
	m_Filter.Empty();
	CDialog::OnOK();
}

void CJobFilter::OnFormInfo()
{
	CCmd_Describe *pCmd = new CCmd_Describe;
	pCmd->Init( m_hWnd, RUN_ASYNC );
	if( pCmd->Run( P4JOB_SPEC, _T("0") ) )
		MainFrame()->UpdateStatus( LoadStringResource(IDS_FETCHING_SPEC) );	
	else
		delete pCmd;
	GotoDlgCtrl(GetDlgItem(IDC_JOB_FILTER));
}

LRESULT CJobFilter::OnP4Describe( WPARAM wParam, LPARAM lParam )
{
	CCmd_Describe *pCmd = ( CCmd_Describe * )wParam;

	MainFrame()->ClearStatus();
	if(!pCmd->GetError())
	{
		int i;
		CString desc = MakeCRs( pCmd->GetDescription( ) );
		if ((i = desc.ReverseFind(_T('#'))) != -1)
		{
			if ((i = desc.Find(_T('\n'), i)) != -1)
				desc = desc.Left(i+1);
		}
		int key;
		CSpecDescDlg *dlg = new CSpecDescDlg(this);
		dlg->SetIsModeless(TRUE);
		dlg->SetKey(key = pCmd->HaveServerLock()? pCmd->GetServerKey() : 0);
		dlg->SetDescription( desc, FALSE );
		dlg->SetCaption( LoadStringResource(IDS_P4WIN_SPECIFICATION_NOTES) );
		dlg->SetWinPosName(_T("JobSpecInfo"));
		CRect rect;
		GetWindowRect(&rect);
		rect.top += rect.Height() - 10;
		rect.bottom = GetSystemMetrics(SM_CYFULLSCREEN);
		rect.right  = GetSystemMetrics(SM_CXFULLSCREEN);
		dlg->SetWinPosDefault(rect);
		if (!dlg->Create(IDD_SPECDESC, this))	// display the description dialog box
		{
			dlg->DestroyWindow();	// some error! clean up
			delete dlg;
		}
	}
	delete pCmd;
	GotoDlgCtrl(GetDlgItem(IDC_JOB_FILTER));
	return 0;
}

LRESULT CJobFilter::OnP4EndDescribe(WPARAM wParam, LPARAM lParam)
{
	CSpecDescDlg *dlg = (CSpecDescDlg *)lParam;
	dlg->DestroyWindow();
	return TRUE;
}
