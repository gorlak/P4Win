//
// 
// Copyright 1999 Perforce Software.  All rights reserved.
//
// This file is part of Perforce - the FAST SCM System.
//
//


// FileSpecPage.cpp : implementation file
//

#include "stdafx.h"
#include "..\p4win.h"
#include "FileSpecPage.h"
#include "IntegOptSheet.h"
#include "Cmd_Describe.h"
#include "MainFrm.h"
#include "..\hlp\p4win.hh"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define UPDATE_STATUS(x) ((CMainFrame *)AfxGetMainWnd())->UpdateStatus(x)

/////////////////////////////////////////////////////////////////////////////
// CIntegFileSpecPage property page

CIntegFileSpecPage::CIntegFileSpecPage(CWnd* pParent /*=NULL*/)
	: CDialog(CIntegFileSpecPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIntegFileSpecPage)
	m_SourceSpecs = _T("");
	m_TargetSpecs = _T("");
	m_TargetSpecsAlt = _T("");
	m_IsNoCopy = GET_P4REGPTR()->GetIsNoCopy();
	m_IsPermitDelReadd = GET_P4REGPTR()->GetIsPermitDelReadd();
	m_DelReaddType = GET_SERVERLEVEL() > 16 ? GET_P4REGPTR()->GetDelReaddType() : 2;
	m_IsBaselessMerge = GET_P4REGPTR()->GetIsBaselessMerge();
	m_IsIndirectMerge = FALSE;
	m_IsPropagateTypes = GET_P4REGPTR()->GetIsPropagateTypes();
	m_IsBaseViaDelReadd = GET_P4REGPTR()->GetIsBaseViaDelReadd();
	m_IsForceInteg = GET_P4REGPTR()->GetIsForceInteg();;
	m_IsForceDirect = GET_P4REGPTR()->GetIsForceDirect();;
	m_IsDeleteSource = GET_P4REGPTR()->GetIsDeleteSource();
	m_IsAutoSync = GET_P4REGPTR()->GetSyncFirstDefault();
	m_IsMappingReverse = FALSE;
	m_IsChgListInteg = FALSE;
	m_ReverseInteg = FALSE;
	m_BranchSpecs = _T("");
	//}}AFX_DATA_INIT

	m_ChangeNbr = 0;
	m_RevRangeFromFlag = m_RevRangeToFlag = 0;
	m_RevRangeFromType = m_RevRangeToType = 0;
	m_SetBranch= FALSE;
	m_Initialized= FALSE;
	m_IsMinimized= FALSE;
	m_IsSourceSpecified= FALSE;
	m_HaveSwappedAlready= FALSE;
	m_BiDir = FALSE;
	m_ChangeNum= 0;
	m_NewChangeNbr= 0;
	m_CommonPath = _T("");
	m_InitRect.SetRect(0,0,100,100);
	m_WinPos.SetWindow( this, _T("IntegDlg") );
}

CIntegFileSpecPage::~CIntegFileSpecPage()
{
}

void CIntegFileSpecPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIntegFileSpecPage)
	DDX_Text(pDX, IDC_SOURCESPECS, m_SourceSpecs);
	DDX_Text(pDX, IDC_TARGETSPECS, m_TargetSpecs);
	DDX_Text(pDX, IDC_TARGETSPECS_ALT, m_TargetSpecsAlt);
	DDX_Control(pDX, IDC_CHANGECOMBO, m_ChangeCombo);
	DDX_Check(pDX, IDC_REVERSEINTEG, m_ReverseInteg);
	DDX_Text(pDX, IDC_BRANCHSPECS, m_BranchSpecs);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIntegFileSpecPage, CDialog)
	//{{AFX_MSG_MAP(CIntegFileSpecPage)
	ON_BN_CLICKED(IDC_SOURCERADIO, OnSourceradio)
	ON_BN_CLICKED(IDC_TARGETRADIO, OnTargetradio)
	ON_BN_CLICKED(IDC_REVERSEINTEG, OnReverseInteg)
	ON_BN_CLICKED(IDC_ADVOPTS, OnAdvancedOptions)
	ON_BN_CLICKED(IDC_PREVIEW, OnPreview)
	ON_COMMAND(IDC_BACK, OnWizardBack)
	ON_COMMAND(IDC_FINISH, OnOK)
	ON_COMMAND(IDCANCEL, OnWizardCancel)
	ON_COMMAND(IDHELP, OnHelp)
	ON_WM_HELPINFO()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_GOTODLGCTRL, OnGotoDlgCtrl)
	ON_MESSAGE( WM_ENABLEDISABLE, OnEnableDisableButtons)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIntegFileSpecPage message handlers

void CIntegFileSpecPage::SetBranchMode( BOOL isSourceSpecified )
{
	ASSERT( m_IsBranch );

	if( m_Initialized )
	{
		UpdateData();

		if( isSourceSpecified )
		{
			ShowAlternateEditControls( BRANCH_FROMSOURCE );
			GetDlgItem( IDC_SOURCESPECS )->EnableWindow( TRUE );
		}
		else
		{
			ShowAlternateEditControls( BRANCH_FROMTARGET );
			GetDlgItem( IDC_TARGETSPECS_ALT )->EnableWindow( TRUE );
		}

		if( m_HaveSwappedAlready || isSourceSpecified == TRUE )
		{
			if( isSourceSpecified )
			{
				// Target was previously specified
				m_SourceSpecs= m_TargetSpecsAlt;
				m_TargetSpecsAlt= "//...";
			}
			else
			{
				// Source was previously specified
				m_TargetSpecsAlt= m_SourceSpecs;
				m_SourceSpecs= "//...";
			}
			m_HaveSwappedAlready= TRUE;
			UpdateData(FALSE);
		}
	}

	m_IsSourceSpecified= isSourceSpecified;
}

void CIntegFileSpecPage::SetSourceSpecs( CStringList *list )
{
	ASSERT(!m_Initialized);
	ASSERT(m_SetBranch);

	PutListInText( m_SourceSpecs, list );
}

void CIntegFileSpecPage::SetTargetSpecs( CStringList *list )
{
	ASSERT(!m_Initialized);
	ASSERT(m_SetBranch);

	if( m_IsBranch )
		PutListInText( m_TargetSpecsAlt, list );
	else
		PutListInText( m_TargetSpecs, list );
}

CStringList *CIntegFileSpecPage::GetSourceList()
{
	ASSERT(m_Initialized);
	if( IsWindow( m_hWnd ) )
		UpdateData();

	PutTextInList( m_SourceSpecs, &m_SourceSpecList );

	return &m_SourceSpecList;
}

CStringList *CIntegFileSpecPage::GetTargetList()
{
	ASSERT(m_Initialized);
	if( IsWindow( m_hWnd ) )
		UpdateData();

	if( m_IsBranch )
		PutTextInList( m_TargetSpecsAlt, &m_TargetSpecList );
	else
		PutTextInList( m_TargetSpecs, &m_TargetSpecList );

	return &m_TargetSpecList;
}

void CIntegFileSpecPage::PutListInText( CString &text, CStringList *list )
{
	text.Empty();
	for( POSITION pos= list->GetHeadPosition(); pos != NULL; )
	{
		if( !text.IsEmpty() )
			text+= "\r\n";
		text+= list->GetNext( pos );
	}
}

void CIntegFileSpecPage::PutTextInList( CString &text, CStringList *list )
{
	list->RemoveAll();
	if( !text.IsEmpty() )
	{
		CString line;

		for( int i=0; i<text.GetLength(); i++ )
		{
			switch( text[i] )
			{
			case '\r':
				break;
			case '\n':
				list->AddHead( line );
				line.Empty();
				break;
			default:
				line+= text[i];
			}
		}
		if( !line.IsEmpty() )
		{
			if (line.Find(_T("//"), 2) != -1)
				m_2FilesOnLine = line;
			list->AddHead( line );
		}
	}
}

#define MAX_LINE_LEN 1024

void CIntegFileSpecPage::LoadBranchView(void)
{
	CCmd_Describe *pCmd = new CCmd_Describe;
	pCmd->Init( m_hWnd, RUN_SYNC );
	if( pCmd->Run( P4BRANCH_SPEC, m_BranchName ) 
	 && !pCmd->GetError())
	{
		LPTSTR	p;
		LPTSTR	q;
		CString m_Description = pCmd->GetDescription();
		for (p = q = m_Description.GetBuffer( m_Description.GetLength( ) ); *p == _T('#'); ++p)
		{
			while (*++p != _T('\n'))
				;
		}
		p = q + m_Description.Find(_T("\nView:\n"), static_cast<int>(p - q)) + sizeof(_T("\nView:\n"))/sizeof(TCHAR) - 1;
		while (*p)
		{
			if ((TBYTE)(*p) <= _T(' '))
				p++;
			else
				break;
		}
		m_BranchSpecs.Empty();
		while (*p)
		{
			if (*p == _T('\t'))
			{
				++p;
				continue;
			}
			else if (*p == _T(' '))
				m_BranchSpecs += _T(' ');
			else if (*p == _T('\n'))
				m_BranchSpecs += _T('\r');
			m_BranchSpecs += *p++;
		}
    }
	else
	{
	 	m_BranchSpecs.FormatMessage(IDS_UNABLE_TO_OBTAIN_BRANCHSPEC_FOR_s, m_BranchName);
	}
	UpdateData(FALSE);
		
	delete pCmd;
	return;
}

BOOL CIntegFileSpecPage::OnInitDialog() 
{
	CDialog::OnInitDialog();

	MainFrame()->SetModelessWnd(this);

	// Record the initial window size, then see if there is a registry preference
	GetWindowRect(&m_InitRect);
	m_LastRect = m_InitRect;

	UpdateData(FALSE);

	// Record positions of primary controls
	CRect rect;
	GetClientRect( &rect );
	int h = rect.Height();
	GetWindowRect( &rect );
	h = rect.Height() - h;
	GetDlgItem( IDC_LABELSOURCE )->GetWindowRect( &m_RectLabelSource );
	GetDlgItem( IDC_SOURCESPECS )->GetWindowRect( &m_RectSource );
	GetDlgItem( IDC_LABELTARGET )->GetWindowRect( &m_RectLabelTarget );
	GetDlgItem( IDC_TARGETSPECS )->GetWindowRect( &m_RectTarget );
	m_RectLabelSource.OffsetRect( -rect.left, -rect.top-h );
	m_RectSource.OffsetRect( -rect.left, -rect.top-h );
	m_RectLabelTarget.OffsetRect( -rect.left, -rect.top-h );
	m_RectTarget.OffsetRect( -rect.left, -rect.top-h );

	// Computer positions of alternate controls
	if( m_IsBranch )
	{
		// In branch mode, bottom of Source = bottom of Target
		m_RectSource.bottom= m_RectTarget.bottom;
		// Branch View takes same space as Source's top, left, right
		m_RectBranch= m_RectSource;

		if (m_BranchFlag == INTEG_USING_BRANCH)
		{
			// if coming from branch pane, hide source and target controls
			GetDlgItem(  IDC_LABELTARGET  )->ModifyStyle( WS_VISIBLE, 0 );
			GetDlgItem(  IDC_TARGETSPECS  )->ModifyStyle( WS_VISIBLE, 0 );
			GetDlgItem(  IDC_LABELSOURCE  )->ModifyStyle( WS_VISIBLE, 0 );
			GetDlgItem(  IDC_SOURCESPECS  )->ModifyStyle( WS_VISIBLE, 0 );
			GetDlgItem(IDC_TARGETSPECS_ALT)->ModifyStyle( WS_VISIBLE, 0 );

			// And show the Reverse Integ checkbox
			GetDlgItem(IDC_REVERSEINTEG)->ModifyStyle( 0, WS_VISIBLE );
		}
		else
		{
			// Diasable or Show the radio buttons
			if (m_IsChgListInteg)
			{
				GetDlgItem( IDC_SOURCERADIO )->EnableWindow(FALSE);
				GetDlgItem( IDC_TARGETRADIO )->EnableWindow(FALSE);
			}
			else
			{
				GetDlgItem( IDC_SOURCERADIO )->ModifyStyle( 0, WS_VISIBLE );
				GetDlgItem( IDC_TARGETRADIO )->ModifyStyle( 0, WS_VISIBLE );
				GetDlgItem( IDC_SOURCERADIO )->GetWindowRect( &m_RectSourceRadio );
				GetDlgItem( IDC_TARGETRADIO )->GetWindowRect( &m_RectTargetRadio );
				m_RectSourceRadio.OffsetRect( -rect.left, -rect.top-h );
				m_RectTargetRadio.OffsetRect( -rect.left, -rect.top-h );
			}

			// shrink branchview to top half, show target/source controls in bottom half
			if (m_IsChgListInteg)
			{
				GetDlgItem( IDC_SOURCESPECS )->ModifyStyle( WS_VISIBLE, 0 );
				CWnd *pWnd = GetDlgItem(IDC_STATIC_1);
				pWnd->GetWindowRect(&rect);
			}
			else
				m_RectBranch.bottom = m_RectBranch.bottom / 2 + 6;
			m_RectSourceRadio.OffsetRect( 0, m_RectBranch.bottom - m_RectLabelSource.top + 10);
			m_RectTargetRadio.OffsetRect( 0, m_RectBranch.bottom - m_RectLabelSource.top + 10);
			m_RectLabelSource.OffsetRect( 0, m_RectBranch.bottom - m_RectLabelSource.top + 10);
			GetDlgItem( IDC_SOURCERADIO )->MoveWindow( m_RectSourceRadio, FALSE );
			GetDlgItem( IDC_TARGETRADIO )->MoveWindow( m_RectTargetRadio, FALSE );
			GetDlgItem( IDC_LABELSOURCE )->MoveWindow( m_RectLabelSource, FALSE );
			m_RectSource.top += m_RectBranch.Height() + m_RectLabelSource.Height() + 14;

			// In branch mode, both multiline edits take same space
			m_RectTarget= m_RectSource;
			m_RectLabelTarget= m_RectLabelSource;

			// Target is always the default
			((CButton *) GetDlgItem(IDC_TARGETRADIO))->SetCheck( 1 );

			// Relabel the multiline edits
			if (m_IsChgListInteg)
				GetDlgItem( IDC_LABELSOURCE )->ModifyStyle( WS_VISIBLE, 0 );
			else
				SetDlgItemText( IDC_LABELSOURCE, LoadStringResource(IDS_FILE_SPECIFICATIONS__INTEGRATE) );
			SetDlgItemText( IDC_LABELTARGET, LoadStringResource(IDS_FILE_SPECIFICATIONS__INTEGRATE) );
		}
		GetDlgItem( IDC_BRANCHSPECS )->MoveWindow( m_RectBranch, FALSE );
		GetDlgItem( IDC_BRANCHSPECS )->ModifyStyle( 0, WS_VISIBLE );
		GetDlgItem( IDC_LABELBRANCH )->ModifyStyle( 0, WS_VISIBLE );
	}
			
	m_Initialized= TRUE;

	if( m_IsBranch )
	{
		SetBranchMode( m_IsSourceSpecified );
		m_LastSelectedTarget= TRUE;

		if( GET_SERVERLEVEL() < LEVEL_NEWINTEG )
		{
			if (m_BranchFlag != INTEG_USING_BRANCH)
			{
				// Pre-99.1 server doesnt allow source-spec for branch
				GetDlgItem(IDC_SOURCERADIO)->EnableWindow(FALSE);
				GetDlgItem(IDC_TARGETRADIO)->EnableWindow(FALSE);
			}
		}
		else
		{
			::SendMessage(GetDlgItem(IDC_TARGETRADIO)->m_hWnd, BM_SETCHECK, BST_UNCHECKED, 0);
			::SendMessage(GetDlgItem(IDC_SOURCERADIO)->m_hWnd, BM_SETCHECK, BST_CHECKED, 0);
			OnSourceradio();
		}
	}
	else
	{
		// If not branch, set alternate windows visible
		ShowAlternateEditControls( FILESPEC );

		// And make sure both are enabled
		GetDlgItem( IDC_SOURCESPECS )->EnableWindow(TRUE);
		GetDlgItem( IDC_TARGETSPECS_ALT )->EnableWindow(TRUE);

		m_targetspecs.SubclassDlgItem(IDC_TARGETSPECS, this);
		m_targetspecs.SetParent(this);
		m_targetspecs.m_pDropTgt = new CEBDropTarget();
		m_targetspecs.m_pDropTgt->Register(&m_targetspecs);
		m_targetspecs.m_pDropTgt->m_Owner = &m_targetspecs;
		m_targetspecs.m_SpecType = 0;

		m_IsSourceSpecified= TRUE;
	}
	
	if (m_ChangeNbr)	// are we integrating from a changlist?
	{
		m_RevRangeFrom.Format(_T("%d"), m_ChangeNbr);
		m_RevRangeTo.Format(_T("%d"), m_ChangeNbr);
		m_RevRangeFromFlag = m_RevRangeToFlag = 2;
		m_ChangeNbr = 0;	// so we won't overwrite any of the user's changes
	}
	
	UpdateChangesControl();

	if (m_IsRename)
	{
		GetDlgItem(IDC_ADVOPTS)->ShowWindow(SW_HIDE);
		SetDlgItemText( IDC_LABELSOURCE, 
			LoadStringResource(IDS_FILE_SPECIFICATIONS__RENAME__ORIGINAL_NAMES) );
		SetDlgItemText( IDC_LABELTARGET, 
			LoadStringResource(IDS_FILE_SPECIFICATIONS__RENAME__NEW_NAMES) );
	}

	if( m_BranchFlag != INTEG_USING_BRANCH_SPEC )
	{
		GetDlgItem(IDC_BACK)->EnableWindow( FALSE );
		GetDlgItem(IDC_BACK)->ShowWindow( SW_HIDE );
		GetDlgItem(IDC_FINISH)->SetWindowText( LoadStringResource(IDS_OK) );
	}

	if (m_IsBranch)
	{
		// Load the BranchView with the BranchSpecs
		LoadBranchView();

		// Add branch name to dialog box title
        CString title;
        title.FormatMessage(IDS_FILE_INTEGRATE_USING_BRANCHSPEC_s, m_BranchName);
		SetWindowText(title);
	}
	else if (m_IsRename)
	{
		// Change the dialog box title to indicate we are renaming
		SetWindowText(LoadStringResource(IDS_FILE_RENAME_TITLE));
		PostMessage(WM_GOTODLGCTRL, IDC_TARGETSPECS, 0);
	}

	m_WinPos.RestoreWindowPosition();
	ShowWindow(SW_SHOW);

	if (m_IsBranch)
		GotoDlgCtrl( GetDlgItem(IDC_CHANGECOMBO) );
	else
		GotoDlgCtrl( GetDlgItem(IDC_TARGETSPECS) );

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CIntegFileSpecPage::OnGotoDlgCtrl(WPARAM wParam, LPARAM lParam)
{
	GotoDlgCtrl(GetDlgItem(static_cast<int>(wParam)));
	return 0;
}


void CIntegFileSpecPage::ShowAlternateEditControls( ShowMode mode )
{
	// We might have changed the branch flag as a result of a Preview
	// so restore it so this routine will work correctly
	//
	m_BranchFlag = m_SaveBranchFlag;

	switch( mode )
	{
	case BRANCH_FROMSOURCE:
		// Show source edit control and hide target
		if (m_BranchFlag != INTEG_USING_BRANCH)
		{
			if (!m_IsChgListInteg)
			{
				GetDlgItem( IDC_SOURCESPECS )->MoveWindow( m_RectSource, FALSE );
				GetDlgItem( IDC_SOURCESPECS )->ModifyStyle( 0, WS_VISIBLE );
				GetDlgItem( IDC_LABELSOURCE )->ModifyStyle( 0, WS_VISIBLE );
			}
		}

		// Hide target edit controls and label
		GetDlgItem( IDC_LABELTARGET )->ModifyStyle( WS_VISIBLE, 0 );
		GetDlgItem( IDC_TARGETSPECS_ALT )->ModifyStyle( WS_VISIBLE, 0 );
		GetDlgItem( IDC_TARGETSPECS )->ModifyStyle( WS_VISIBLE, 0 );

		break;

	case BRANCH_FROMTARGET:
		// Show alternate target edit control (multiline)
		if (m_BranchFlag != INTEG_USING_BRANCH)
		{
			GetDlgItem( IDC_LABELTARGET )->MoveWindow( m_RectLabelTarget, FALSE );
			GetDlgItem( IDC_LABELTARGET )->ModifyStyle( 0, WS_VISIBLE );

			GetDlgItem( IDC_TARGETSPECS_ALT )->MoveWindow( m_RectTarget, FALSE );
			GetDlgItem( IDC_TARGETSPECS_ALT )->ModifyStyle( 0, WS_VISIBLE );
		
			// Hide source edit control and standard target edit control
			GetDlgItem( IDC_LABELSOURCE )->ModifyStyle( WS_VISIBLE, 0 );
			GetDlgItem( IDC_SOURCESPECS )->ModifyStyle( WS_VISIBLE, 0 );
			GetDlgItem( IDC_TARGETSPECS )->ModifyStyle( WS_VISIBLE, 0 );
		}

		break;

	case FILESPEC:
		// Leave the default edit controls in default positions
		GetDlgItem( IDC_LABELSOURCE )->MoveWindow( m_RectLabelSource, FALSE );
		GetDlgItem( IDC_SOURCESPECS )->MoveWindow( m_RectSource, FALSE );
		GetDlgItem( IDC_SOURCESPECS )->ModifyStyle( 0, WS_VISIBLE );
		
		GetDlgItem( IDC_LABELTARGET )->MoveWindow( m_RectLabelTarget, FALSE );
		GetDlgItem( IDC_TARGETSPECS )->ModifyStyle( 0, WS_VISIBLE );

		// Hide alternate target edit control
		GetDlgItem( IDC_TARGETSPECS_ALT )->ModifyStyle( WS_VISIBLE, 0 );

		break;

	default:
		ASSERT(0);
	}
	RedrawWindow();
}

void CIntegFileSpecPage::OnWizardBack() 
{
	m_WinPos.SaveWindowPosition();

	::PostMessage(m_DepotWnd, WM_DOINTEGRATE1, (WPARAM)ID_WIZBACK, 0);
}

void CIntegFileSpecPage::OnWizardCancel() 
{
	m_WinPos.SaveWindowPosition();

	::PostMessage(m_DepotWnd, WM_DOINTEGRATE1, 0, 0);
}

void CIntegFileSpecPage::OnPreview() 
{
	m_Previewing = TRUE;
	if( SERVER_BUSY() || !OnSetInactive() )
		return;

	m_IsPreview = TRUE;
	::PostMessage(m_DepotWnd, WM_DOINTEGRATE1, (WPARAM)ID_WIZFINISH, 0);
}

void CIntegFileSpecPage::OnOK() 
{
	m_WinPos.SaveWindowPosition();

	m_Previewing = FALSE;
	if( SERVER_BUSY() || !OnSetInactive() )
		return;

	m_IsPreview = FALSE;
	::PostMessage(m_DepotWnd, WM_DOINTEGRATE1, (WPARAM)ID_WIZFINISH, 0);
}

// This signals the closing of a modeless
// dialog to MainFrame which
// will delete the 'this' object
void CIntegFileSpecPage::OnDestroy()
{
	::PostMessage(MainFrame()->m_hWnd, WM_P4DLGDESTROY, 0, (LPARAM)this);
}

BOOL CIntegFileSpecPage::CheckForAllOK()
{
	if ((!m_IsSourceSpecified && (GetTargetList()->GetCount()==1) && (m_TargetSpecsAlt=="//..."))
	  || (m_IsSourceSpecified && (GetSourceList()->GetCount()==1) && (m_SourceSpecs=="//...")))
	{
		if (IDYES != AfxMessageBox(IDS_YOU_HAVE_ENTERED_THE_ALL_ENCOMPASSING_FILESPEC,
								MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2))
			return FALSE;
	}
	return TRUE;
}


BOOL CIntegFileSpecPage::OnSetInactive()
{
	m_2FilesOnLine.Empty();
	UpdateData();

	// We might have changed the branch flag as a result of a Preview
	// so restore it so this will work correctly
	//
	m_BranchFlag = m_SaveBranchFlag;

	m_RevRange.Empty();
	if  (m_RevRangeFromFlag || m_RevRangeToFlag)
	{
		switch(m_RevRangeFromFlag)
		{
		case 0:
			break;
		case 1:
			m_RevRange = _T('#') + m_RevRangeFrom + _T(',');
			break;
		case 2:
			m_RevRange = _T('@') + m_RevRangeFrom + _T(',');
			break;
		default:
			ASSERT(0);
			break;
		}
		switch(m_RevRangeToFlag)
		{
		case 0:
			m_RevRange += _T("#head");
			break;
		case 1:
			m_RevRange += _T('#') + m_RevRangeTo;
			break;
		case 2:
			m_RevRange += _T('@') + m_RevRangeTo;
			break;
		default:
			ASSERT(0);
			break;
		}
	}

	switch( m_BranchFlag )
	{
	case INTEG_USING_FILE_SPEC:
	{
		CString findwhat = m_TargetSpecs.GetAt(0) == _T('/') ? _T("/...") : _T("\\...");
		if( GetSourceList()->GetCount() > 1 && 
			m_TargetSpecs.Find(findwhat) != m_TargetSpecs.GetLength()-4 )
		{
			AfxMessageBox( IDS_TARGET_FILESPEC_MUST_END_IN_WILDCARD, MB_ICONEXCLAMATION);
			return FALSE;
		}
		m_SourceSpecs.Trim();
		m_TargetSpecs.Trim();
		if (m_SourceSpecs == m_TargetSpecs)
		{
			AfxMessageBox( IDS_SOURCE_TARGET_FILESPECS_MAY_NOT_BE_IDENTICAL, MB_ICONEXCLAMATION);
			return FALSE;
		}
		if ((m_SourceSpecs.FindOneOf(_T("@#")) != -1) || (m_TargetSpecs.FindOneOf(_T("@#")) != -1))
		{
			AfxMessageBox(IDS_CANT_USE_REVRANGE_HERE_USE_OPTIONS, MB_ICONEXCLAMATION);
			return FALSE;
		}
		if (m_SourceSpecs.IsEmpty() || m_TargetSpecs.IsEmpty())
		{
			AfxMessageBox(IDS_NEITHER_SRC_OR_TARG_MAY_BE_EMPTY, MB_ICONEXCLAMATION);
			return FALSE;
		}
		if (m_IsRename && IS_NOCASE() && (GetSourceList()->GetCount()==1)
			&& !GetSourceList()->GetHead().CompareNoCase(GetTargetList()->GetHead()))
		{
			AfxMessageBox(IDS_CANT_RENAME_TO_CHANGE_CASE, MB_ICONEXCLAMATION);
			return FALSE;
		}
		if (m_TargetSpecs.GetAt(0) == _T('/') && m_TargetSpecs.Find(_T('\\')) != -1)
		{
			if (AfxMessageBox(IDS_BACKSLASHINDEPOTSYNTAX, MB_YESNO|MB_ICONQUESTION ) != IDYES)
				return FALSE;
		}
		break;
	}
	case INTEG_USING_BRANCH_SPEC:
		m_SaveBranchFlag = m_BranchFlag;	// we will need to restore if this is a Preview
		if( GetSourceList()->GetCount() && !GetTargetList()->GetCount() )
		{
			m_SourceSpecs.Empty();
			m_BranchFlag = INTEG_USING_BRANCH;
			m_IsMappingReverse = m_ReverseInteg = m_IsChgListInteg;
			break;
		}
		if( !GetSourceList()->GetCount() && GetTargetList()->GetCount() )
		{
			m_TargetSpecsAlt.Empty();
			m_BranchFlag = INTEG_USING_BRANCH;
			m_IsMappingReverse = m_ReverseInteg = !m_IsChgListInteg;
			m_BiDir = m_IsChgListInteg;
			break;
		}
		if (!CheckForAllOK())
			return FALSE;
		if( !GetSourceList()->GetCount() && !GetTargetList()->GetCount() )	// shouldn't happen (?)
		{
			AfxMessageBox( IDS_NO_FILES_SPECIFIED_NOTHING_TO_DO, MB_ICONEXCLAMATION);
			return FALSE;
		}
		break;
	case INTEG_USING_BRANCH:
		GetSourceList();		// called only to check m_2FilesOnLine
		m_SourceSpecs.Empty();
		break;
	}

	if (!m_2FilesOnLine.IsEmpty())
	{
		CString txt;
		txt.FormatMessage(IDS_CANTHAVE2FILESON1LINE, m_2FilesOnLine);
		AfxMessageBox(txt, MB_ICONSTOP);
		return FALSE;
	}

	// Looks good, so obtain the change number.
	// We wait until here so that we don't use up
	// a new change number for bad data
	// when the user wants a new changelist.
	m_ChangeNum= GetChangeNum();
	if (m_ChangeNum == -1)
		return FALSE;
	
	return TRUE;
}

void CIntegFileSpecPage::SetChangesList( CStringList *list )
{
	m_pChangeList= list;
}

void CIntegFileSpecPage::UpdateChangesControl()
{
	ASSERT( m_Initialized );
	ASSERT( m_pChangeList != NULL );
	
	// Make sure the combo is empty
	m_ChangeCombo.Clear();

	// Then fill it
	POSITION pos= m_pChangeList->GetHeadPosition();
	while( pos != NULL )
	{
		CString txt= m_pChangeList->GetNext(pos);
		m_ChangeCombo.AddString( txt );
	}

	// Select the first item (but 2nd item for rename or if Option is for New)
	if (m_IsRename)
		m_ChangeCombo.SetCurSel(GET_P4REGPTR()->GetUseNewChglist2() ? 1 : 0);
	else
		m_ChangeCombo.SetCurSel(GET_P4REGPTR()->GetUseNewChglist() ? 1 : 0);
}

int CIntegFileSpecPage::GetChangeNum()
{
	if( IsWindow( m_hWnd ) )
	{
		// Get the target change number
		CString tmp;
		int changeNum=0;
	
		if((m_ChangeCombo.GetCurSel() >= 0) && !m_Previewing)
		{
			m_ChangeCombo.GetLBText(m_ChangeCombo.GetCurSel(), tmp);
			if(tmp.Compare(LoadStringResource(IDS_NEWCHANGELISTNAME)) == 0)
			{
				CString description;
				CString *pDesc = NULL;
				BOOL autoOK=FALSE;

				if (!m_NewChangeNbr && m_IsRename)
				{
					description = LoadStringResource(IDS_RENAME_RENAME)
								+ m_SourceSpecs 
								+ LoadStringResource(IDS_RENAME_TO) 
								+ m_TargetSpecs;
					description.Replace(_T('\n'), _T(' '));
					pDesc = &description;
					autoOK= TRUE;
				}
				changeNum= (m_NewChangeNbr > 0) ? m_NewChangeNbr 
							: m_NewChangeNbr = MainFrame()->CreateNewChangeList(0, pDesc, autoOK);
			}
			else if(tmp.Compare(LoadStringResource(IDS_DEFAULTCHANGELISTNAME)) != 0)
				changeNum= _ttol(tmp);
		}
		return changeNum;
	}
	else
		return m_ChangeNum;
}

void CIntegFileSpecPage::OnSourceradio()
{
	if( m_LastSelectedTarget )
	{
		SetBranchMode( TRUE );
		m_LastSelectedTarget= FALSE;
	}
}

void CIntegFileSpecPage::OnTargetradio()
{
	if( !m_LastSelectedTarget )
	{
		SetBranchMode( FALSE );
		m_LastSelectedTarget= TRUE;
	}
}

void CIntegFileSpecPage::OnReverseInteg() 
{
	UpdateData();
	m_IsMappingReverse = m_ReverseInteg;
}

void CIntegFileSpecPage::OnAdvancedOptions() 
{
	// We might have changed the branch flag as a result of a Preview
	// so restore it so this will work correctly
	//
	m_BranchFlag = m_SaveBranchFlag;

	CIntegOptSheet dlg(LoadStringResource(IDS_FILE_INTEGRATE_ADVANCED_OPTIONS), this, 0);

	dlg.m_OptsPage.SetIsDefOpts( FALSE );
	dlg.m_OptsPage.SetIsBranch( m_IsBranch );
	dlg.m_OptsPage.SetBranchFlag( m_BranchFlag );
	dlg.m_OptsPage.SetIsNoCopy( m_IsNoCopy );
	dlg.m_OptsPage.SetIsSyncFirst( m_IsAutoSync );
	dlg.m_OptsPage.SetIsPermitDelReadd( m_IsPermitDelReadd );
	dlg.m_OptsPage.SetDelReaddType( m_DelReaddType );
	dlg.m_OptsPage.SetIsBaselessMerge( m_IsBaselessMerge );
	dlg.m_OptsPage.SetIsIndirectMerge( m_IsIndirectMerge );
	dlg.m_OptsPage.SetIsPropagateTypes( m_IsPropagateTypes );
	dlg.m_OptsPage.SetIsBaseViaDelReadd( m_IsBaseViaDelReadd );
	dlg.m_OptsPage.SetIsForceInteg( m_IsForceInteg );
	dlg.m_OptsPage.SetIsForceDirect( m_IsForceDirect );
	dlg.m_OptsPage.SetIsDeleteSource( m_IsDeleteSource );
	dlg.m_OptsPage.SetIsMappingReverse( m_IsMappingReverse );
	dlg.m_RevRngPage.SetRevRangeFrom( m_RevRangeFrom );
	dlg.m_RevRngPage.SetRevRangeTo( m_RevRangeTo );
	dlg.m_RevRngPage.SetRevRangeFromFlag( m_RevRangeFromFlag );
	dlg.m_RevRngPage.SetRevRangeToFlag( m_RevRangeToFlag );
	dlg.m_RevRngPage.SetRevRangeFromType( m_RevRangeFromType );
	dlg.m_RevRngPage.SetRevRangeToType( m_RevRangeToType );
	if (m_BranchFlag == INTEG_USING_BRANCH)
	{
		UpdateData();
		dlg.m_OptsPage.SetFileSpecs(m_LastSelectedTarget ? m_TargetSpecsAlt : m_SourceSpecs);
	}
	SET_APP_HALTED(TRUE);
	if( dlg.DoModal() == IDOK )
	{
		m_IsAutoSync       = dlg.m_OptsPage.IsSyncFirst();
		m_IsNoCopy         = dlg.m_OptsPage.IsNoCopy();
		m_IsPermitDelReadd = dlg.m_OptsPage.IsPermitDelReadd();
		m_DelReaddType     = dlg.m_OptsPage.DelReaddType();
		m_IsBaselessMerge  = dlg.m_OptsPage.IsBaselessMerge();
		m_IsIndirectMerge  = dlg.m_OptsPage.IsIndirectMerge();
		m_IsPropagateTypes = dlg.m_OptsPage.IsPropagateTypes();
		m_IsBaseViaDelReadd= dlg.m_OptsPage.IsBaseViaDelReadd();
		m_IsForceInteg     = dlg.m_OptsPage.IsForceInteg();
		m_IsForceDirect     = dlg.m_OptsPage.IsForceDirect();
		m_IsDeleteSource   = dlg.m_OptsPage.IsDeleteSource();
		m_IsMappingReverse = dlg.m_OptsPage.IsMappingReverse();
		m_RevRangeFrom     = dlg.m_RevRngPage.GetRevRangeFrom();
		m_RevRangeTo       = dlg.m_RevRngPage.GetRevRangeTo();
		m_RevRangeFromFlag = dlg.m_RevRngPage.GetRevRangeFromFlag();
		m_RevRangeToFlag   = dlg.m_RevRngPage.GetRevRangeToFlag();
		m_RevRangeFromType = dlg.m_RevRngPage.GetRevRangeFromType();
		m_RevRangeToType   = dlg.m_RevRngPage.GetRevRangeToType();
		if (m_BranchFlag == INTEG_USING_BRANCH)
		{
			if (m_LastSelectedTarget)
				m_TargetSpecsAlt = dlg.m_OptsPage.GetFileSpecs();
			else
				m_SourceSpecs = dlg.m_OptsPage.GetFileSpecs();
			UpdateData(FALSE);
		}
	}
	SET_APP_HALTED(FALSE);
}

void CIntegFileSpecPage::SetIsRename( BOOL isRename ) 
{
	if (isRename)
	{
		m_IsPropagateTypes = m_IsBaseViaDelReadd = m_IsPermitDelReadd = m_IsAutoSync
						   = m_IsForceInteg = m_IsDeleteSource = m_IsRename = TRUE;
		m_IsNoCopy = FALSE;
		m_DelReaddType = 2;
	}
	else
		m_IsRename = FALSE;
}

BOOL CIntegFileSpecPage::SetSpecList( CStringList *list )
{
	// We might have changed the branch flag as a result of a Preview
	// so restore it so this will work correctly
	//
	m_BranchFlag = m_SaveBranchFlag;

	if( m_IsBranch )
	{
		// We always assume that the supplied specs are targets, and only
		// set them to source at the user's request
		// We leave tagrets blank if request is from branch pane
		if (m_BranchFlag != INTEG_USING_BRANCH)
			SetTargetSpecs( list );

		CStringList sourceList;
		sourceList.AddHead( _T("//...") );

		SetSourceSpecs( &sourceList );
	}
	else
	{
		// The supplied list is source specs
		SetSourceSpecs( list );

		// Set the target spec
		CString commonPath = list->GetHead();
		TCHAR slashChar = commonPath.Find(_T('/')) != -1 ? _T('/') : _T('\\');

		// We want a directory (with no wildcard) for the common path
		// What we have is either a directory: //depot/dir1/dir2/...
		//                          or a file: //depot/dir1/dir2/filename.ext
		// In either case we want to remove the last / and everything after it
		int i;
		if( (i=commonPath.ReverseFind(slashChar)) != -1)
			commonPath = commonPath.Left(i);

		// Now walk the list and find the common path for all

		BOOL b = IS_NOCASE() || slashChar == _T('\\');
		for( POSITION pos = list->GetHeadPosition(); pos != NULL; )
		{
			CString item = list->GetNext( pos );
			do
			{
				int len = commonPath.GetLength();
				if (len <= item.GetLength())
				{
					if (b)
					{
						if (item.GetAt(len) == slashChar 
						 && commonPath.CompareNoCase(item.Left(len)) == 0)
							break;
					}
					else
					{
						if (item.GetAt(len) == slashChar 
						 && commonPath.Compare(item.Left(len)) == 0)
							break;
					}
				}
				if( (i=commonPath.ReverseFind(slashChar)) != -1)
					commonPath = commonPath.Left(i);
			} while (commonPath.GetLength() > 2);
		}
		if (commonPath.GetLength() <= 2)
		{
			AfxMessageBox(IDS_CANNTINTEGUNLESSCOMMONPATH, MB_ICONSTOP);
			return FALSE;
		}

		CStringList targetList;
		if (m_IsRename && list->GetCount() == 1)
			targetList.AddHead( list->GetHead() );
		else
			targetList.AddHead( commonPath + slashChar + _T("...") );

		SetTargetSpecs( &targetList );
		SetCommonPath(commonPath);
	}
	return TRUE;
}

LPCTSTR CIntegFileSpecPage::GetReference( )
{
	static CString ref;

	if( m_IsBranch )
		ref= GetBranchName();
	else
	{
		CStringList *list= GetTargetList();
		if( list->IsEmpty() )
			ASSERT(0);
		else
			ref= list->GetHead();		
	}

	return LPCTSTR( ref );
}

///////////////////////////////////////
// Handler for help button

void CIntegFileSpecPage::OnHelp() 
{
	AfxGetApp()->WinHelp(ALIAS_54_INTEG_OPTIONS);
}

BOOL CIntegFileSpecPage::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// Help for this property sheet is only specific down
	// to the property page level
	OnHelp();
	return TRUE;
}

///////////////////////////////////////
// Resize routines

void CIntegFileSpecPage::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	if (m_InitRect.Height())
	{
		lpMMI->ptMinTrackSize.x= m_InitRect.Width();
		lpMMI->ptMinTrackSize.y= m_InitRect.Height();
	}
}

void CIntegFileSpecPage::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

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

	// We might have changed the branch flag as a result of a Preview
	// so restore it so the resize will work correctly
	//
	m_BranchFlag = m_SaveBranchFlag;

	CWnd *pWnd=GetDlgItem(IDC_SOURCESPECS);
	if (!pWnd || !IsWindow(pWnd->m_hWnd))
		return;

	// Compute the change in width (dx)
	CRect rect;
	GetWindowRect(&rect);
	int dx = rect.Width() - m_LastRect.Width();
	int dy = rect.Height() - m_LastRect.Height();
	// Save the new size
	m_LastRect = rect;

	// Widen the source list by the change in width (dx) and
	// heighten the source list by the change in height (dy)
	pWnd->GetWindowRect(&rect);
	pWnd->SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								   rect.bottom - rect.top + dy, SWP_NOMOVE | SWP_NOZORDER);
	m_RectSource.right += dx;
	m_RectSource.bottom += dy;

	// Widen the branchspec list by the change in width (dx) and
	// heighten the branchspec list by the change in height (dy) ONLY if
	// this is an Integ using a Branchspec from the Branches pane
	// or from the Submitted Changelist pane and using a Branchspec.
	pWnd = GetDlgItem(IDC_BRANCHSPECS);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	if (m_BranchFlag == INTEG_USING_BRANCH || m_IsChgListInteg)
		pWnd->SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								   rect.bottom - rect.top + dy, SWP_NOMOVE | SWP_NOZORDER);
	else
		pWnd->SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								   rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);

	pWnd = GetDlgItem(IDC_TARGETSPECS_ALT);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, 0, 0, rect.right - rect.left + dx, 
								   rect.bottom - rect.top + dy, SWP_NOMOVE | SWP_NOZORDER);
	m_RectTarget.right += dx;
	m_RectTarget.bottom += dy;

	// Widen the edit and combo boxes by the change in width (dx)
	// Slide these down by change in height (dy)
	pWnd = GetDlgItem(IDC_TARGETSPECS);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, rect.right - rect.left + dx, 
								   rect.bottom - rect.top, SWP_NOZORDER);

	pWnd = GetDlgItem(IDC_CHANGECOMBO);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, rect.right - rect.left + dx, 
								   rect.bottom - rect.top, SWP_NOZORDER);

	// Slide reverse checkbox by change in width
	pWnd = GetDlgItem(IDC_REVERSEINTEG);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// Slide other controls down by change in height
	if (m_BranchFlag != INTEG_USING_BRANCH_SPEC)
	{
		pWnd = GetDlgItem(IDC_LABELTARGET);
		pWnd->GetWindowRect(&rect);
		ScreenToClient(rect);
		pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}

	pWnd = GetDlgItem(IDC_STATIC_1);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// Slide the buttons to the right by the change in width
	// and down by the change in height
	pWnd = GetDlgItem(IDC_ADVOPTS);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(IDC_PREVIEW);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(IDC_BACK);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	pWnd->SetWindowPos(NULL, rect.left + dx, rect.top +dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	pWnd = GetDlgItem(IDC_FINISH);
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

void CIntegFileSpecPage::OnSysCommand(UINT nID, LPARAM lParam) 
{
	switch(nID)
	{
	case SC_MINIMIZE:
		GetDesktopWindow()->ArrangeIconicWindows();
		break;
	}

	CDialog::OnSysCommand(nID, lParam);
}

LRESULT CIntegFileSpecPage::OnEnableDisableButtons(WPARAM wParam, LPARAM lParam)
{
	if( m_BranchFlag == INTEG_USING_BRANCH_SPEC )
		GetDlgItem(IDC_BACK)->EnableWindow( lParam != 0 );
	GetDlgItem( IDC_ADVOPTS )->EnableWindow( lParam != 0 );
	GetDlgItem( IDC_PREVIEW )->EnableWindow( lParam != 0 );
	GetDlgItem( IDC_FINISH )->EnableWindow( lParam != 0 );
	GetDlgItem( IDHELP )->EnableWindow( lParam != 0 );
	GetDlgItem( IDCANCEL )->SetWindowText( LoadStringResource(lParam ? IDS_CANCEL : IDS_CLOSE) );
	return 0;
}
