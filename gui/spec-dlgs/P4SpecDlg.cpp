/*
 * Copyright 1997, 1999 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */

#include "stdafx.h"
#include "p4win.h"
#include "P4SpecDlg.h"
#include "cmd_editspec.h"
#include "TokenString.h"
#include "SpecDescDlg.h"
#include "Cmd_SendSpec.h"
#include "DeltaView.h"
#include "LabelView.h"
#include "BranchView.h"
#include "ClientView.h"
#include "JobView.h"
#include "UserView.h"
#include "MainFrm.h"
#include "MsgBox.h"
#include "newclientdlg.h"

#include "p4client.h"
#include "p4job.h"
#include "p4branch.h"
#include "p4user.h"
#include "p4label.h"

#include "cmd_diff.h"

#include <afxdisp.h>
#include "hlp\p4win.hh"


#define P4BRANCH_SPEC	2
#define P4CHANGE_SPEC	3
#define P4CLIENT_SPEC	4
#define P4DEPOT_SPEC	5
#define P4JOB_SPEC		6
#define P4LABEL_SPEC	7
#define P4PROTECT_SPEC	8
#define P4USER_SPEC		9

#define	MINHEIGHT	  240
#define	EXTRAHEIGHT		2

const CString g_tagView = _T("View");
const CString g_tagFile = _T("Files");
const CString g_tagJob  = _T("Job");
const CString g_tagJobStatus  = _T("JobStatus");
const CString g_tagRoot = _T("Root");
const CString g_tagAltRoots = _T("AltRoots");
const CString g_tagReviews = _T("Reviews");
const CString g_tagStatus = _T("Status");
const CString g_tagUser = _T("User");
const CString g_tagDescription = _T("Description") ;
const CString g_tagFullName =  _T("FullName");
const CString g_tagPassword = _T("Password");


const int JOB_CODE_NAME = 101;
const int JOB_CODE_STATUS = 102;
const int JOB_CODE_USER = 103;
const int JOB_CODE_DATE = 104;
const int JOB_CODE_DESC = 105;


//		combo box stuff. i'm ashamed of this code, so don't look at it.
//
const CString g_SelectionSeparator = _T(":");
const CString g_PresetSeparator = _T("/");

const int g_DropListSize= 10;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CP4SpecDlg dialog


IMPLEMENT_DYNCREATE(CP4SpecDlg, CPropertyPage)

CP4SpecDlg::CP4SpecDlg() : CPropertyPage(CP4SpecDlg::IDD)
{
	//{{AFX_DATA_INIT(CP4SpecDlg)
	//}}AFX_DATA_INIT
	m_bIsModal = FALSE;
	m_NumMultiLineChildWnds = 0;
	m_jobList = m_fileList = 0;
	m_SpecType=0;
	m_pFirstControl=NULL;
	m_pFocusControl=NULL;
	m_AllowSubmit=FALSE;
	m_SendingSpec=FALSE;
	m_AddFilesControl=TRUE;
	m_WindowShown =FALSE;
	m_EditorBtnDisabled = m_ChangesHaveBeenMade = FALSE;
	m_SetFocusHere = FALSE;
	m_AutomaticallyUpdate = FALSE;
	m_HasRequired = FALSE;
	m_BrowseShown = FALSE;
	m_BrowseBtnCtrlID = -1;
	m_PrevCBPmt = "";
	m_Root2Use = _T("");
	m_MinSize= CSize(0,MINHEIGHT);
	m_VscrollMax = 1000;
	m_VscrollPos = 0;
	m_pDeltaView = MainFrame()->GetDeltaView();
	m_pLastFilesList = 0;
	m_MinLi = _tstoi(GET_P4REGPTR()->GetMinMultiLineSize());
	if (m_MinLi < 2)
		m_MinLi = 3;	// set back to default

	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	m_ScreenHeight = rect.bottom - rect.top;

	HDC hDC = ::GetDC(NULL);
	m_LogPixelsX = GetDeviceCaps(hDC, LOGPIXELSX);
	::ReleaseDC(NULL, hDC);
}

CP4SpecDlg::~CP4SpecDlg()
{
	// can't use MainFrame()-> construct
	// because mainfram might have closed.
	CMainFrame * mainWnd = MainFrame();
	if (mainWnd)
		mainWnd->SetGotUserInput( );
}

void CP4SpecDlg::SetCallingCommand( CCmd_EditSpec *pCmd )
{
	ASSERT_KINDOF( CCmd_EditSpec, pCmd );
	m_pCallingCommand= pCmd;
}

BOOL CP4SpecDlg::SetSpec(LPCTSTR spec, LPCTSTR specDefStr
						 , int specType, BOOL allowSubmit)
{
	// Initialize CWinPos registry class
	switch(specType)
	{
        case P4BRANCH_SPEC:
			m_WinPos.SetWindow( this, _T("BranchSpecDlg") );
            break;
		case P4CHANGE_SPEC:
			m_WinPos.SetWindow( this, _T("ChangeSpecDlg") );
            break;
        case P4CLIENT_SPEC:
            m_WinPos.SetWindow( this, _T("ClientSpecDlg") );
            break;
		case P4JOB_SPEC:
            m_WinPos.SetWindow( this, _T("JobSpecDlg") );
            break;
        case P4LABEL_SPEC:
            m_WinPos.SetWindow( this, _T("LabelSpecDlg") );
            break;
        case P4USER_SPEC:
            m_WinPos.SetWindow( this, _T("UserSpecDlg") );
            break;
		default:
			ASSERT(0);
    }

	m_OldForm = spec;
    m_SpecDefStr= specDefStr;
	m_OrigPassword.Empty();
	m_OrigRoot.Empty();

    // At least some 97.3 servers seem to omit the Labelspec 'Options' tag
    // and the Userspec 'Jobview' tag, yet send specs that include these 
    // values.  Since the servers are already in circulation, fix this at
    // the client side
    if( GET_SERVERLEVEL() < 4 )
    {
        switch(specType)
        {
        case P4BRANCH_SPEC:
            m_SpecDefStr= BRANCH_SPEC;
            break;
        case P4CLIENT_SPEC:
            m_SpecDefStr= CLIENT_SPEC;
            break;
        case P4LABEL_SPEC:
            m_SpecDefStr= LABEL_SPEC;
            break;
        case P4USER_SPEC:
            m_SpecDefStr= USER_SPEC;
            break;
        }
    }
	
	m_SpecType = specType;
	m_AllowSubmit = allowSubmit;

	//		the big kahuna: get the existing spec info.
	//		this code used to be in initdialog but
	//		if there was an error in the spec, it would
	//		put up an empty dialog. very ugly. this is
	//		called before DoModal, so things look better.
	//
	return ParseSpecIntoForm( );
}


/*
	_________________________________________________________________

	Parse( ) calls pMyspecData's Set( ) function
	for as many times as there are elements in the spec
	_________________________________________________________________
*/

BOOL CP4SpecDlg::ParseSpecIntoForm( )
{
	Error e;
	StrRef sSpecDefStr;

	try
	{
		if( GET_SERVERLEVEL() >= 16)
			ReorderSpecDefString( m_OldForm, m_SpecDefStr );

		if ((m_FoundLineElemWithValues = FindLineElemWithValues( m_SpecDefStr )) != -1)
			HandleLineElemWithValues( m_OldForm, m_SpecDefStr );

		m_SpecData.SetSpecElems( m_SpecDefStr );
        m_SpecDefStrA = CharFromCString(m_SpecDefStr);
        sSpecDefStr.Set( const_cast<char*>((const char*)m_SpecDefStrA) );
		m_Spec.Decode( &sSpecDefStr, &e );
		ASSERT( ! e.Test ( ) );

		e.Clear( );
        m_OldFormA = CharFromCString(m_OldForm);
        m_Spec.ParseNoValid( const_cast<char*>((const char*)m_OldFormA), &m_SpecData, &e );

		if( e.Test( ) )
		{
			CString msg= FormatError(&e);

			// A missing field is not a fatal error!
			if( msg.Find(_T("Missing required field")) == -1 )
			{
				CString txt;
				txt.FormatMessage(IDS_ERROR_IN_SPECIFICATION_s, msg);
				AddToStatus( txt, SV_WARNING, true );
				return FALSE;
			}
		}

		//		all fields have to be in the dialogue, even those
		//		that are empty. if there's an empty one at the
		//		end, the ParseNoValid didn't call specdata's Get
		//		enough number of times. so make sure all fields are there
		//		except, of course, Jobs and Files from new changelists! damn.
		//
		m_SpecData.CheckForStragglers( );
	}

	catch( ... )
	{
		CString msg;
		msg.LoadString(IDS_BAD_SPEC);
		AddToStatus(msg, SV_WARNING, true );
		return FALSE;
	}

    // Store any available instruction text for tooltip support.  This text
    // will be rummaged during OnNotifyToolTip() to see if we have a helpful
    // hint to offer the user.  To get the comments, find the first blank line
    // and then grab all text before that line.
    
    int i, j = 0;
    m_InstructionText.Empty();
        
    for( i=0; i < m_OldForm.GetLength()-2; i++)
    {
        if( m_OldForm[i] == _T('\n') )
        {
            m_InstructionText+= g_CRLF;
            BOOL found = FALSE;

            // Look for another '\n' before non-white-space
            for( j=i+1; !found && j<m_OldForm.GetLength()-1; j++ )
            {
                switch( m_OldForm[j])
                {
                case _T('\t'):
                case _T(' '):
                case _T('\r'):
                    break;

                case _T('\n'):
                default:
                    found=TRUE;
                    break;
                }
            }
             
            // If the first non-white-space we found was another \n,
            // we are at the end of comments (it's j-1, because the for
            // loop increments j before the loop test)
            if( m_OldForm[j-1] == _T('\n') )
            {
                break;  // out of for
            }
        }
        else
            m_InstructionText+= m_OldForm[i];
    }
    
    // If we didnt find a blank line, we dont know where the comments end.  
    // In this case, clear the instruction text rather than allow a botched display.
    if( i > j )
        m_InstructionText.Empty();

	return TRUE;
}

void CP4SpecDlg::ReorderSpecDefString( CString &form, CString &specDefStr )
{
	if ((specDefStr.Find(_T(";seq:")) == -1) 
	 && (specDefStr.Find(_T(";type:wlist")) == -1) 
	 && (specDefStr.Find(_T(";type:llist")) == -1))
		return;

	CString oldspec = specDefStr;
	CString oldform = form;
	CString elm;
	CString fld;
	CStringList nonseqelms;
	CStringList nonseqlists;
	CStringArray seqelms;
	CStringArray fields;

	seqelms.SetSize(20, 10);

	// split the data in 2 parts:
	//	1)	'form' contains the header
	//	2)	'oldform' contains the data
	int f = oldform.Find(_T("\n\n"));
	if (f == -1)
		return;
	form = form.Left(f);
	oldform = oldform.Mid(f);

	// loop thru the spec, getting each element
	// save elements with "seq:" in their corresponding slot in the seqelms array
	// save elements without "seq:" in the nonseqelms list
	int n;
	int nbrflds=0;
	int nbrelms=0;
	int m=0;
	int i=0;
	while ((i = oldspec.Find(_T(";;"))) != -1)
	{
		// isolate the next spec element
		i += 2;
		elm = oldspec.Left(i);
		oldspec = oldspec.Mid(i);

		// if this element has "seq:", put it in the 'seqelms' array
		// otherwise put it at the end of the 'nonseqelms' list if not a list
		// or at the end of the 'nonseqlists' list if is a 'wlist' or 'llist'
		int j;
		if ((j = elm.Find(_T(";seq:"))) != -1)
		{
			CString seq = elm.Mid(j+sizeof(_T(";seq"))/sizeof(TCHAR));
			n = _tstoi(seq);
			if (n)
			{
				seqelms.SetAtGrow(n, elm);
				m = max(n, m);
			}
			else	// special case - seq# of 0 means no seq number provided
				nonseqelms.AddTail(elm);
		}
		else if ((elm.Find(_T(";type:wlist")) != -1)
			  || (elm.Find(_T(";type:llist")) != -1))
		{
			nonseqlists.AddTail(elm);
		}
		else
		{
			nonseqelms.AddTail(elm);
		}
		nbrelms++;

		// we can do the fields in the same loop 
		// since there are always at least as many
		// spec elms as fields (maybe more)
		f = oldform.Find(_T("\n\n"), 2);
		if (f == -1)
			f = oldform.GetLength();
		fld = oldform.Left(f);
		oldform = oldform.Mid(f);
		if (!fld.IsEmpty())
		{
			fields.Add(fld);
			nbrflds++;
		}
	}

	// if the greatest seq# is larger than the count of elements
	// we are in trouble - bail and use the order of the spec
	if (m > nbrelms)
	{
		// we have to restore the form data before bailing
		for (int j = -1; ++j < nbrflds; )
		{
			fld = fields.GetAt(j);
				form += fld;
		}
		CString txt;
		txt.FormatMessage(IDS_SEQGTRNBRELMS_d_d, m, nbrelms);
		AfxMessageBox(txt, MB_ICONEXCLAMATION);
		return;
	}

	// if we found any elements without "seq:",
	// put them in the gaps in the seqelms array
	if (!nonseqelms.IsEmpty())
	{
		for (n=0; ++n <= nbrelms; )
		{
			if (n >= seqelms.GetSize() || seqelms[n].IsEmpty())
			{
				seqelms.SetAtGrow(n, nonseqelms.GetHead());
				nonseqelms.RemoveHead();
				if (nonseqelms.IsEmpty())
					break;
			}
		}
	}
	// do the same for any lists
	if (!nonseqlists.IsEmpty())
	{
		for (n=0; ++n <= nbrelms; )
		{
			if (n >= seqelms.GetSize() || seqelms[n].IsEmpty())
			{
				seqelms.SetAtGrow(n, nonseqlists.GetHead());
				nonseqlists.RemoveHead();
				if (nonseqlists.IsEmpty())
					break;
			}
		}
	}

	// Rebuild the spec and the data form
	specDefStr.Empty();
	for (n=0; ++n <= nbrelms; )
	{
		// the spec elements are in order in the seqelms array
		// so just concatenate them together
		elm = seqelms.GetAt(n);
		specDefStr += elm;
		// for the data form, we need to find the data
		// that corresponds to the current spec element,
		// so get the name of the current spec element
		i = elm.Find(_T(';'));
		elm = _T('\n') + elm.Left(i) + _T(':');
		// search thru the data fields for that name
		for (int j = -1; ++j < nbrflds; )
		{
			fld = fields.GetAt(j);
			if (fld.Find(elm) != -1)
			{
				// if (there might not be any data for a field) we find it
				// concatenate that field to the form
				form += fld;
				break;
			}
		}
	}
}

int CP4SpecDlg::FindLineElemWithValues( CString specDefStr, int offset /*= 0*/ )
{
	int typeline, values, dblsemicolon;
	while ((typeline = specDefStr.Find(_T(";type:line;"), offset)) != -1)
	{
		if ((dblsemicolon = specDefStr.Find(_T(";;"), typeline)) == -1)
			return -1;
		if (((values = specDefStr.Find(_T(";val:"), typeline)) < dblsemicolon)
		  && (values > typeline))
			return typeline;
		offset = dblsemicolon;
	}
	return -1;
}

void CP4SpecDlg::HandleLineElemWithValues( CString form, CString specDefStr )
{
	int i, j;
	int offset;
	int values;
	int counter;
	TCHAR next;
	CString newform;
	CString newspec;
	CString newelem;
	CString fldname;
	CString dspname;
	CString txt;

	m_OrigSpecDefStr = specDefStr;
	for (offset = 0; (offset = FindLineElemWithValues(specDefStr, offset)) != -1; 
								offset += sizeof(_T(";type:line;"))/sizeof(TCHAR))
	{
		newspec = specDefStr.Left(offset);
		while ((i = newspec.ReverseFind(_T(';'))) != -1)
		{
			if (newspec.GetAt(i-1) == _T(';'))
				break;
			newspec = newspec.Left(i);
		}
		if (i == -1)	// spec doesn't match what we expect!
			return;
		dspname = fldname = newspec.Right(newspec.GetLength() - i - 1);
		dspname += CString((TCHAR)0x10);
		newspec = newspec.Left(i+1);
		values = specDefStr.Find(_T(";val:"), offset) + sizeof(_T(";val:"))/sizeof(TCHAR)-1;
		counter = 1;
   		do
   		{
			// Set the format for the 1st of the group to "L";
			// set the format for the rest of the group to "R".
			// This will be fixed up by CSpecData::AddElem()
			// which converts from the new L, R, I codes back
			// to the internal BH, MH, IH and RH codes
			CString fmt = (counter == 1) ? _T("L") : _T("R");

 			i = specDefStr.Find(_T('/'), values);
 			if (i == -1)
 			{
				newelem.Format(_T("%s%d;type:select;len:20;fmt:%s;pre:"), 
					dspname, counter, fmt);
 				newspec += newelem + _T(";val:") + _T(" /");
 			}
 			else
 			{
	   			newelem.Format(_T("%s%d;type:select;rq;len:20;fmt:%s;pre:"), 
					dspname, counter, fmt);
   				newspec += newelem + specDefStr.Mid(values, i-values) + _T(";val:");
 			}
			counter++;
			i = specDefStr.Find(_T(','), values);
			j = specDefStr.Find(_T(';'), values);
			if ((i == -1) || (j < i))
				 i = j;
			newspec += specDefStr.Mid(values, i-values) + _T(";;");
			next = specDefStr.GetAt(i);
			specDefStr = specDefStr.Mid(i+1);
			values = 0;
		} while (next == _T(','));
		newspec += specDefStr.Mid(1);

		txt.Format(_T("\n\n%s:\t"), fldname);
		i = form.Find(txt);
		if (i == -1)	// data doesn't match spec!
		{
			m_FoundLineElemWithValues = -1;
			return;
		}
		newform = form.Left(i);
		values  = i + txt.GetLength();
		counter = 1;
		do
		{
			newelem.Format(_T("\n\n%s%d:\t"), dspname, counter++);
			i = form.Find(_T(' '), values);
			j = form.Find(_T('\n'), values);
			if ((i == -1) || (j < i))
				 i = j;
			newform += newelem + form.Mid(values, i-values);
			next = form.GetAt(i);
			form = form.Mid(i+1);
			values = 0;
		} while (next == _T(' '));
		newform += _T("\n\n") + form.Mid(1);
		specDefStr = newspec;
		form = newform;
	}
	m_SpecDefStr = newspec;
	m_OldForm = newform;
}

BOOL CP4SpecDlg::RestoreLineElemWithValues( CString form, CString specDefStr )
{
	int i;
	int offset;
	int values;
	int counter;
	CString newform;
	CString fldname;
	CString dspname;
	CString temp;
	CString txt;

	for (offset = 0; (offset = FindLineElemWithValues(specDefStr, offset)) != -1; 
								offset += sizeof(_T(";type:line;"))/sizeof(TCHAR))
	{
		counter = 0;
		temp = specDefStr.Left(offset);
		i = temp.ReverseFind(_T(';'));
		while ((i = temp.ReverseFind(_T(';'))) != -1)
		{
			if (temp.GetAt(i-1) == _T(';'))
				break;
			temp = temp.Left(i);
		}
		if (i == -1)	// data doesn't match spec!
			return FALSE;
		dspname = fldname = temp.Right(temp.GetLength() - i - 1);
		dspname += CString((TCHAR)0x10);
		txt.Format(_T("\n\n%s%d:\t"), dspname, ++counter);
		i = form.Find(txt);
		if (i == -1)	// data doesn't match spec!
			return FALSE;
		newform = form.Left(i);
		values  = i + txt.GetLength();
		txt.Format(_T("\n\n%s:\t"), fldname, counter);
		newform += txt;
		do
		{
			if (counter > 1)
				newform += _T(' ');
			i = form.Find(_T('\n'), values);
			if (i == -1)	// data doesn't match spec!
				return FALSE;
			newform += form.Mid(values, i-values);
			form = form.Mid(i);
			txt.Format(_T("\n\n%s%d:\t"), dspname, ++counter);
			i = form.Find(txt);
			values = i + txt.GetLength();
		} while (i != -1);
		newform += form;
		form = newform;
	}
	m_NewForm = newform;
	m_SpecDefStr = specDefStr;
	return TRUE;
}


/*
	_________________________________________________________________
*/

void CP4SpecDlg::SetChangeParms(BOOL checkAllJobs, 
								BOOL allowSubmit, 
								BOOL checkOnlyChgedFiles/*=FALSE*/,
								BOOL addFilesControl/*=TRUE*/,
								BOOL submitOnlySelected/*=FALSE*/,
								BOOL automaticallyUpdate/*=FALSE*/)
{
	m_CheckAllJobs= checkAllJobs;
	m_AllowSubmit=allowSubmit;
	m_CheckOnlyChgedFiles=checkOnlyChgedFiles;
	m_AddFilesControl=addFilesControl;
	m_SubmitOnlySelected=submitOnlySelected;
	m_AutomaticallyUpdate=automaticallyUpdate;
}


void CP4SpecDlg::SetClientParms(LPCTSTR root, BOOL automaticallyUpdate/*=FALSE*/)
{
	m_Root2Use= root;
	m_AutomaticallyUpdate=automaticallyUpdate;
}


LPCTSTR CP4SpecDlg::GetSpec()
{
	return m_NewForm;
}


void CP4SpecDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CP4SpecDlg)
	DDX_Control(pDX, IDC_BUSYMESSAGE, m_BusyMessage);
	DDX_Control(pDX, IDC_REQSTATIC, m_ReqStatic);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CP4SpecDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CP4SpecDlg)
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_SHOWWINDOW()
	ON_WM_VSCROLL()
	ON_WM_GETMINMAXINFO()
	ON_WM_HELPINFO()
	ON_COMMAND(ID_FILE_CANCEL, OnCancelButton)
	ON_COMMAND(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
    ON_NOTIFY_EX(TTN_NEEDTEXT,0,OnToolTipNotify)
	ON_MESSAGE(WM_P4SENDSPEC, OnP4SendSpec)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CP4SpecDlg message handlers

BOOL CP4SpecDlg::OnInitDialog() 
{
	ASSERT( m_specControls.GetSize() == 0 );
	
	//		create the m_hWnd.
	//
	CPropertyPage::OnInitDialog();

    EnableToolTips(TRUE);
	CreateUserFonts();

    SetScrollRange(SB_VERT, 0, m_VscrollMax, FALSE);
    SetScrollPos(SB_VERT, m_VscrollPos = 0, TRUE);
	
	//		Calculate some dimensions for control placement
	//
	RECT rectdlg, rect1, rect2, rect3, rectlist;
	CWnd *static1 = GetDlgItem( IDC_STATIC1 );  // invisible static in the dlg resource
	CWnd *static2 = GetDlgItem( IDC_STATIC2 );	// invisible static in the dlg resource
	CWnd *static3 = GetDlgItem( IDC_STATIC3 );	// invisible static in the dlg resource
	CWnd *droplist= GetDlgItem( IDC_COMBO1 );   // invisible combo in the dlg resource

	//		Store the dialog width
	//
	GetClientRect( &rectdlg );
	m_Width = rectdlg.right;       
	
	//		store the standard and maximum dimensions of the controls
	//
	static1->GetWindowRect( &rect1 );
	static2->GetWindowRect( &rect2 );
	static3->GetWindowRect( &rect3 );
	droplist->GetWindowRect( &rectlist );
	ScreenToClient( &rect1 );
	ScreenToClient( &rect2 );
	ScreenToClient( &rect3 );

	int lenlongestprompt = GetLengthLongestPrompt();
	m_StdHeight = rect1.bottom - rect1.top;		// standard static, edit dimension
	m_StdWidth  = min(rect1.right - rect1.left, lenlongestprompt); // prompt width
	// Adjust for scaling that will be done in SetUserFont()
	if (m_OldAveCharWidth != m_NewAveCharWidth)
		m_StdWidth = (m_StdWidth*m_OldAveCharWidth + m_NewAveCharWidth*2) / m_NewAveCharWidth;
	m_MaxWidth  = rect3.right - rect3.left;		// max width that fits dlg
	m_StdSpaceV = rect2.top - rect1.bottom;		// vertical spacing
	m_StdSpaceH = 5;							// horizontal spacing

	int adjamt = GetSystemMetrics(SM_CXVSCROLL);
	if (m_LogPixelsX < 105)
		adjamt += adjamt/2;
	m_MaxWidth -= adjamt;

	m_X = m_Y = rect1.top;

	m_ComboWidth = m_MaxWidth - m_StdWidth;
	m_HlfWidth   = m_MaxWidth/2 - m_StdWidth - m_StdSpaceH*2;

	//		okay, add controls, buttons, caption, size the window to the
	//		controls and slap it onto the window!
	//
	if ( SetControls( ) )
	{
		CDialog *pParent = (CDialog *)GetParent();
		pParent->SetWindowText( GetDialogueCaption( ) );

		if ( m_pFocusControl != NULL )
		{
			GotoDlgCtrl(m_pFocusControl);
			if( m_pFocusControl->IsKindOf( RUNTIME_CLASS( CP4EditBox ) ) )
			{
				int i;
				CP4EditBox *pEdit = (CP4EditBox *)m_pFocusControl;
				CString txt;
				pEdit->GetWindowText(txt);
				if ((i = txt.Find(_T("\r\n\r\n"))) != -1)
				{
					pEdit->SetSel(i+2, txt.GetLength());
					pEdit->LineScroll(pEdit->LineFromChar(i));
				}
			}
		}
		else if ( m_pFirstControl != NULL )
			GotoDlgCtrl( m_pFirstControl );

		SetWindowPos( NULL, 0, 0, m_Width, m_Y , SWP_NOMOVE | SWP_NOZORDER );
		
		CenterWindow();
	}

	int thebottom = SetUserFont();
	if (m_MinSize.cy < thebottom)
		m_MinSize.cy = thebottom;

	if (m_AutomaticallyUpdate)
		On_OK();
	else
		m_pCallingCommand->ReleaseServerLock();

	return FALSE;  // return TRUE unless you set the focus to a control
				   //fanny: and i do, so i'm returning FALSE
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CP4SpecDlg::CreateUserFonts()
{
	// Create the user font 
	//
    LOGFONT logFont;
 
	GetFont()->GetLogFont(&logFont);
	logFont.lfHeight= -abs(GET_P4REGPTR()->GetFontSize());
    m_Font.CreateFontIndirect( &logFont );

	logFont.lfWeight += 200;
    m_FontBold.CreateFontIndirect( &logFont );

	lstrcpy(logFont.lfFaceName, GET_P4REGPTR()->GetFontFace());
	logFont.lfWeight -= 200;
	logFont.lfPitchAndFamily= FIXED_PITCH | FF_DONTCARE;
	logFont.lfCharSet = DEFAULT_CHARSET;
    m_FontFixed.CreateFontIndirect( &logFont );

	// Get text metrics for old font ( dialog template ) and new font
	//
	TEXTMETRIC tmOld, tmNew;
	CDC *pDC= GetDC();
	CFont *pOldFont= pDC->SelectObject( GetFont() );
	pDC->GetTextMetrics( &tmOld );
	pDC->SelectObject( &m_Font );
	pDC->GetTextMetrics( &tmNew );
	pDC->SelectObject( pOldFont );
	ReleaseDC( pDC );

	m_OldHeight= tmOld.tmHeight + tmOld.tmExternalLeading;
	m_NewHeight= tmNew.tmHeight + tmNew.tmExternalLeading;
	m_OldAveCharWidth = tmOld.tmAveCharWidth;
	m_NewAveCharWidth = tmNew.tmAveCharWidth;
}

int CP4SpecDlg::SetUserFont()
{
	// Resize the dialog, holding the topleft corner position
	//
	CRect clientRect, newClientRect, windowRect, newWindowRect;
	GetWindowRect( windowRect );
	GetClientRect( clientRect );

	int xDiff= windowRect.Width() - clientRect.Width();   // borders
	int yDiff= windowRect.Height() - clientRect.Height(); // borders

	newClientRect.left= newClientRect.top= 0;
	newClientRect.right= clientRect.right * m_NewAveCharWidth / m_OldAveCharWidth;
	newClientRect.bottom= clientRect.bottom * m_NewHeight / m_OldHeight;

	newWindowRect.left = windowRect.left;
	newWindowRect.top = windowRect.top;
	newWindowRect.right= windowRect.left + newClientRect.right + xDiff;
	newWindowRect.bottom= windowRect.top + newClientRect.bottom + yDiff;
	MoveWindow( newWindowRect );

	m_MinSize= CSize( newWindowRect.Width() - xDiff, max(MINHEIGHT,newWindowRect.Height() - yDiff));

	// And then set the new font
	//
 	SetFont(&m_Font);

	// Iterate thru all child windows, changing fonts and rescaling
	//
	int thebottom = m_Y - m_StdSpaceV;
	for( int i=0; i < m_childControls.GetSize(); i++ )
	{
		CWnd *pChildWnd= m_childControls[i].GetWindow( );

		pChildWnd->SetFont( (pChildWnd->IsKindOf( RUNTIME_CLASS(CP4EditBox) )
			&& m_childControls[i].GetType()==CHILD_MULTILINEEDIT) ? &m_FontFixed : &m_Font );
		pChildWnd->GetWindowRect( windowRect );
		ScreenToClient( windowRect );
		windowRect.left= windowRect.left * m_NewAveCharWidth / m_OldAveCharWidth;
		windowRect.right= windowRect.right * m_NewAveCharWidth / m_OldAveCharWidth;
		windowRect.top= windowRect.top * m_NewHeight / m_OldHeight;

		if( pChildWnd->IsKindOf( RUNTIME_CLASS( CComboBox ) ) )
		{
			windowRect.bottom= windowRect.top + ( m_StdHeight * g_DropListSize * m_NewHeight / m_OldHeight );
			thebottom = windowRect.top + m_StdHeight;
		}
		else
		{
			windowRect.bottom = thebottom = windowRect.bottom * m_NewHeight / m_OldHeight + 1;
		}
		
		pChildWnd->MoveWindow( windowRect );
		m_childControls[i].SetOrigRect( &windowRect );
	}
	thebottom += m_StdSpaceV*2;

	return thebottom;
}

BOOL CP4SpecDlg::OnToolTipNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{   
    BOOL foundTip=FALSE;
    int i;

    TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;    
    UINT nID =pNMHDR->idFrom;

    if (pTTT->uFlags & TTF_IDISHWND)    
    {
        // idFrom is actually the HWND of the tool
        HWND hWnd= (HWND) nID;
        nID = ::GetDlgCtrlID((HWND)nID);        
        switch(nID)
        {
        case 0:
        case IDCANCEL:
        case IDOK:
        case IDALTERNATE:
            break;
        case ID_HELPNOTES:
			// can't just take a pointer to the CString returned by LoadStringResource
			// because it will be gone once we return, so make a copy of the first 80
			// bytes of it and hope the tooltip isn't any longer
			_tcsncpy(pTTT->szText, (LPCTSTR)LoadStringResource(IDS_SPEC_TOOLTIP), sizeof(pTTT->szText)/sizeof(TCHAR));
            pTTT->hinst = NULL;
            foundTip=TRUE;
            break;
        default:
			for( i=0; i<m_specControls.GetSize(); i++ )
            {
				SpecControl const & sc = m_specControls[i];
                CWnd *pWnd= sc.control;
				if(pWnd && hWnd == pWnd->m_hWnd && sc.tip.GetLength())
                {
                    pTTT->lpszText = (LPTSTR) LPCTSTR(sc.tip);
                    pTTT->hinst = NULL;
                    foundTip=TRUE;
                }
            }
            break;
        }    
    }
    return(foundTip);
}


/*
	_________________________________________________________________
*/

static DWORD SetBasicWinStyles( BOOL readOnly )
{
	DWORD style = WS_CHILD | WS_VISIBLE | ES_LEFT ;

	if( readOnly )
		style |= ES_READONLY; 
	else
		style |= WS_TABSTOP;

	return style;
}


/*
	_________________________________________________________________
*/

CStatic * SpecControl::CreateLabel ( CWnd *parent, LPCRECT rect, LPCTSTR prompt  )
{
	ASSERT(!label);
	label = new CStatic;
	ASSERT(label);
	label->Create( prompt, WS_CHILD | WS_VISIBLE, *rect, parent );
	return label;
}
CStatic * SpecControl::CreateLabel ( CWnd *parent )
{
	// create a hidden label

	ASSERT(!label);
	label = new CStatic;
	ASSERT(label);
	label->Create( _T("hidden"), WS_CHILD, CRect(0,0,1,1), parent );
	return label;
}

CComboBox * SpecControl::CreateCombo ( CWnd *parent, LPCRECT rect, DWORD style, HMENU menu )
{
	ASSERT(!control);
	CComboBox * pCombo = new CComboBox;
	ASSERT( pCombo );

	if ( !pCombo->CreateEx( WS_EX_CLIENTEDGE, _T("ComboBox")
		, _T(""), style, rect->left, rect->top
		, rect->right - rect->left + 1
		, ( rect->bottom - rect->top + 1 ) * g_DropListSize
		, parent->m_hWnd
		, menu ) )
	{
		delete pCombo;
		pCombo = 0;
	}
	control = pCombo;
	return pCombo;
}

CButton * SpecControl::CreateCheckBox(CWnd *parent, LPCRECT rect, 
									  DWORD style, int id, LPCTSTR prompt)
{
	ASSERT(!control);
	CButton * pButton = new CButton;
	ASSERT( pButton );

	CRect crect = rect;

	if ( !pButton->Create( prompt, style, crect, parent, id ) )
	{
		delete pButton;
		pButton = 0;
	}
	control = pButton;
	return pButton;
}

CReviewList * SpecControl::CreateList ( CWnd *parent, LPCRECT rect, DWORD style, HMENU menu, int code )
{
	ASSERT(!control);
	this->code = code;
	CReviewList * pList = new CReviewList;
	ASSERT( pList );

	if(!pList->CreateEx(WS_EX_CLIENTEDGE, MainFrame()->m_ReviewListClass
		, _T(""), style, rect->left, rect->top
		, rect->right - rect->left
		, rect->bottom - rect->top
		, parent->m_hWnd
		, menu))
	{
		delete pList;
		pList = 0;
	}
	control = pList;
	isChkList = TRUE;
	return pList;
}

CP4EditBox * SpecControl::CreateEdit(CWnd *parent,LPCRECT rect,DWORD style, HMENU menu,int code, 
									 BOOL allowDD/*=FALSE*/, int specType/*=0*/)
{
	ASSERT(!control);
	this->code = code;
	CP4EditBox * pEdit = new CP4EditBox(parent);
	ASSERT( pEdit );

	DWORD exStyle = WS_EX_NOPARENTNOTIFY;
	if(!(style & ES_READONLY))
		exStyle |= WS_EX_CLIENTEDGE;
	if(!pEdit->CreateEx(exStyle, _T("EDIT")
		, _T(""), style, rect->left, rect->top
		, rect->right-rect->left+1
		, rect->bottom-rect->top+1
		, parent->m_hWnd
		, menu))
	{
		delete pEdit;
		pEdit = 0;
	}
	control = pEdit;
	if (allowDD)
	{
		pEdit->m_pDropTgt = new CEBDropTarget();
		pEdit->m_pDropTgt->Register(pEdit);
		pEdit->m_pDropTgt->m_Owner = pEdit;
		pEdit->m_SpecType = specType;
	}
	return pEdit;
}

CP4EditBox * SpecControl::CreateEdit ( CWnd *parent, int code )
{
	// create a hidden edit control

	ASSERT(!control);
	this->code = code;
	CP4EditBox * pEdit = new CP4EditBox(parent);
	ASSERT( pEdit );

	if(!pEdit->CreateEx(WS_EX_CLIENTEDGE, _T("EDIT")
		, _T(""), WS_CHILD, 0, 0
		, 1
		, 1
		, parent->m_hWnd
		, NULL))
	{
		delete pEdit;
		pEdit = 0;
	}
	control = pEdit;
	return pEdit;
}

void SpecControl::AddToolTip(LPCTSTR prompt, CString const & instructionText)
{
	// See if tooltip text can be found
	tip=_T("");
	CString searchStr;
	searchStr.Format(_T("#  %s:"), prompt);
	int offset= instructionText.Find(searchStr);
	if( offset == -1)
	{
		searchStr.Format(_T("# %s:"), prompt);
		offset= instructionText.Find(searchStr);
	}
	if( offset != -1)
	{
		tip= instructionText.Mid(offset + searchStr.GetLength());
		offset=tip.Find(_T('\n'));
		if(offset != -1)
			tip=tip.Left(offset);
		tip.TrimLeft();
		tip.TrimRight();
	}
}

/*
	_________________________________________________________________
*/

void CP4SpecDlg::AddDummy()
{
	SpecControl sc;
	m_specControls.Add(sc);
}

void CP4SpecDlg::AddComboBox( const CString &prompt, const CStringArray &values
						 , const CString &editText 
						 , int specCode, BOOL readOnly
						 , int height, int width, int required
						 , const CString &indent, const CString &wCode, int lioff)
{
	int i;
	CString pmt;
	BOOL bCvt2CkhBox;
	// we stuck a 0x10 char after the prompt on all internally
	// generated fileds (that we made from a single edit line
	// with the appropriate attributes).
	if ((i = prompt.Find((TCHAR)0x10)) != -1)
	{
		pmt = prompt.Left(i) + _T(":");
		if (pmt != m_PrevCBPmt)
			m_PrevCBPmt = pmt;
		else
			pmt.Empty();
		bCvt2CkhBox = values.GetSize() == 2;
	}
	else
	{
		pmt = prompt + ((required && !readOnly) ? _T(":*") : _T(":"));
		bCvt2CkhBox = values.GetSize() == 2 
					&& (m_SpecType != P4JOB_SPEC || GET_P4REGPTR()->Cvt2ValComboToChkBx());
		m_PrevCBPmt = "";
	}

	if (lioff < 1)
		m_Y = m_LiY[m_LiY.GetCount()-1 + lioff];

	CRect rect( m_X, m_Y+3, m_X+m_StdWidth, m_Y+3 + m_StdHeight );

	if (indent == _T('M'))
	{
		rect.left += m_MaxWidth/2;
		rect.right += m_MaxWidth/2;
	}
	else if (indent == _T('I'))
	{
		rect.left += m_StdWidth + m_StdSpaceH;
		rect.right += m_StdWidth + m_StdSpaceH;
	}

	SpecControl sc;
	if (!pmt.IsEmpty())
	{
		sc.CreateLabel(this, rect, pmt);
		sc.AddToolTip(pmt, m_InstructionText);
		m_childControls.Add(CChildWindow(sc.label, CHILD_STATIC, &rect,
							TRUE, 0, wCode == _T('H'), indent == _T('M'), indent == _T('R')));
		if (!m_HasRequired)
		{
			if (pmt.Find(_T('*')) != -1)
				m_HasRequired = TRUE;
		}
	}

	if (wCode == _T("H") && (width > m_HlfWidth))
		width = m_HlfWidth;

	rect.SetRect( m_X + m_StdWidth + m_StdSpaceH
		, m_Y, m_X + m_StdWidth + width, m_Y + height );

	if (indent == _T('M'))
	{
		rect.left += m_MaxWidth/2;
		rect.right = m_X + m_MaxWidth;
	}
	else if (indent == _T('I'))
	{
		rect.left += m_StdWidth + m_StdSpaceH;
		rect.right += m_StdWidth + m_StdSpaceH;
	}

	m_LiY.Add(m_Y);

	// if we are to create a checkbox rather than a combobox
	// go do that and return;
	if (bCvt2CkhBox && GET_SERVERLEVEL() >= 11)
	{
		AddCheckBox( prompt, values, editText, specCode, 
			readOnly, height, width, required, indent, wCode, lioff, rect, sc);
		return;
	}

	// Combo's are taller than statics, so put a little more
	// whitespace between them and next lower control
	m_Y += 2 * m_StdSpaceV + height;

	DWORD style = SetBasicWinStyles( readOnly );
	style |= CBS_DROPDOWNLIST | WS_VSCROLL ;
	CComboBox * pCombo = sc.CreateCombo(this, rect, style, 
		(HMENU)IDC( m_SpecType, specCode ));
	if(!pCombo)
	{
		AddToStatus( LoadStringResource(IDS_UNABLE_TO_ADD_CHILD_WINDOW_TO_SPECIFICATION_DIALOG), 
            SV_WARNING, true );
		return;
	}
	m_childControls.Add(CChildWindow(sc.control, CHILD_DROPLIST, &rect, 
						TRUE, 0, wCode == _T('H'), indent == _T('M'), indent == _T('R')));

	//		set the selections in the combo box and
	//		pick which one will show when not pulled down.
	//		(the selection is right only if the combo box is not sorted)
	//
	BOOL newAdded = FALSE;
	if( editText == _T("new") )
	{
		pCombo->AddString( _T("new") );
		newAdded = TRUE;
	}
	CString value;
	int selected = -1;
	for ( int j = 0; j < values.GetSize ( ); j++ )
	{
		value = values.GetAt ( j );
		if ( !newAdded || (value != _T("new")) )
			pCombo->AddString( value );
		if ( value == editText )
			selected = j;
	}
	pCombo->SetCurSel( selected );

	//		okay, store the control away
	//
	m_specControls.Add(sc);

	if ( !m_pFirstControl )
		if ( !readOnly )
			m_pFirstControl = pCombo;

	if ( m_SetFocusHere )
	{
		m_SetFocusHere = FALSE;
		if ( !readOnly )
			m_pFocusControl = pCombo;
	}
}

/*
	This routine converts a 2 item ComboBox to a CheckBox 
	with the label of the 2nd item
*/

void CP4SpecDlg::AddCheckBox( const CString &prompt, const CStringArray &values
						 , const CString &editText 
						 , int specCode, BOOL readOnly
						 , int height, int width, int required
						 , const CString &indent, const CString &wCode, int lioff
						 , CRect &rect, SpecControl &sc)
{
	m_Y += m_StdSpaceV + height;

	// Adjust the width and placement of checkboxes 
	// that start in the middle of the form or at the right side
	if (indent == _T('M') || indent == _T('R'))
	{
		rect.right = rect.left + m_StdWidth + GetSystemMetrics(SM_CXVSCROLL);
		if (indent == _T('M'))
		{
			rect.left  -= m_StdWidth;
			rect.right  = rect.left + max(m_HlfWidth - GetSystemMetrics(SM_CXVSCROLL)*2,
				                          m_StdWidth + GetSystemMetrics(SM_CXVSCROLL));
		}
		else if (indent == _T('R'))
		{
			int w = rect.right - rect.left;
			rect.right = m_X + m_MaxWidth;
			rect.left  = rect.right - w;
		}
	}

	DWORD style = SetBasicWinStyles( readOnly );
	style |= BS_AUTOCHECKBOX;
	CButton * pCheck = sc.CreateCheckBox(this, rect, style, specCode, values[1]);
	if(!pCheck)
	{
		AddToStatus( LoadStringResource(IDS_UNABLE_TO_ADD_CHILD_WINDOW_TO_SPECIFICATION_DIALOG), 
            SV_WARNING, true );
		return;
	}
	m_childControls.Add(CChildWindow(sc.control, CHILD_CHECKBOX, &rect, 
						TRUE, 0, wCode == _T('H'), indent == _T('M'), indent == _T('R')));

	//		set the check in the Check box
	pCheck->SetCheck(values.GetAt(1) == editText ? BST_CHECKED : BST_UNCHECKED);

	//		set the tooltip to the "other" value
	sc.tip = values.GetAt(0);

	//		okay, store the control away
	//
	m_specControls.Add(sc);

	if ( !m_pFirstControl )
		if ( !readOnly )
			m_pFirstControl = pCheck;

	if ( m_SetFocusHere )
	{
		m_SetFocusHere = FALSE;
		if ( !readOnly )
			m_pFocusControl = pCheck;
	}
}


/*
	_________________________________________________________________
*/

void CP4SpecDlg::AddList(const CString &prompt, const CStringArray &list, int specCode, int height, 
                         int width, int scrollWidth)
{
	//		Set the position of the first control
	//
	if (m_specControls.GetSize() == 0)
		m_Y=20;

	CRect rect(m_X, m_Y, m_X+m_StdWidth, m_Y + m_StdHeight);

	SpecControl sc;
	sc.CreateLabel(this, rect, prompt + _T(":"));
	sc.AddToolTip( prompt, m_InstructionText );
	m_childControls.Add(CChildWindow(sc.label, CHILD_STATIC, &rect));

	//		Calc the rect for the list box
	//
	rect.SetRect(m_X, m_Y+m_StdHeight, m_X+width, m_Y+m_StdHeight+height); 
	if (!GET_P4REGPTR()->AllowPromptAbove())
	{
		m_Y -= m_StdHeight + m_StdSpaceV;
		rect.top -= m_StdHeight;
		height = rect.bottom - rect.top - m_StdHeight;
		rect.left += m_StdWidth + m_StdSpaceH;
		width -= m_StdWidth + m_StdSpaceH;
	}
	m_LiY.Add(m_Y);
	
	DWORD style= WS_CHILD | WS_BORDER | WS_VISIBLE | LBS_HASSTRINGS
		 | LBS_EXTENDEDSEL | LBS_OWNERDRAWFIXED | WS_VSCROLL | WS_HSCROLL | WS_TABSTOP;

    if( scrollWidth > width )
        style |= WS_HSCROLL;

	CReviewList *pList = sc.CreateList(this, rect, style, (HMENU) IDC(m_SpecType, specCode), specCode);
	m_childControls.Add(CChildWindow(sc.control, CHILD_CHECKLISTBOX, &rect));
	pList->SetCheckStyle(BS_AUTOCHECKBOX);

	// Because checklistboxes are an integral height
	// don't try to compute the top of the next field;
	// just get the bottom coord relative to the dlg's client area
	// and add the appropriate spaceing to that.
	CRect newRect;
	sc.control->GetWindowRect(newRect);
	ScreenToClient(newRect);
	m_Y = newRect.bottom + m_StdSpaceV;

    if( scrollWidth > width )
        // Set a sufficiently wide horizontal extent for files    
        pList->SetHorizontalExtent( scrollWidth );

	m_specControls.Add(sc);

	int index;
	int iNbrChecked = 0;
	for ( int i = 0; i < list.GetSize( ); i++ )
	{
		CString s = list.GetAt( i );

        if( !s.IsEmpty() )
        {
		    index = pList->AddString( s );

		    //		Files are selected by default, and jobs are not
		    //
			BOOL bSetCheck = FALSE;
		    if( prompt == g_tagFile )
			{
				bSetCheck = !m_SubmitOnlySelected ? TRUE
					      : m_pDeltaView->GetTreeCtrl().IsAMemeberOfSelectionList(s);
				if (bSetCheck && m_CheckOnlyChgedFiles)
					bSetCheck = (m_UnchangedFlag != 2) ? TRUE : IsFileChanged(s);
			}	
			else if ( m_CheckAllJobs )
				bSetCheck = TRUE;
			if (bSetCheck)
			{
			    pList->SetCheck( index, 1 );
				iNbrChecked++;
			}
        }
	}
	if (!iNbrChecked && (prompt == g_tagFile) && m_CheckOnlyChgedFiles)
		AfxMessageBox(IDS_SPEC_NO_FILES_CHANGED, MB_ICONEXCLAMATION);

	m_NumMultiLineChildWnds++;

	if (prompt == g_tagFile)
	{
		m_pLastFilesList = pList;
//		if (iNbrChecked == list.GetSize())
//			GetParent()->SendMessage(WM_ENABLEDISABLE, 0, FALSE);
	}
}


/*
	_________________________________________________________________
*/

BOOL CP4SpecDlg::IsFileChanged(CString filename)
{
	if (!m_pDeltaView->GetTreeCtrl().IsAMemeberOfFileList(filename))
		return TRUE;

	// for servers that support p4 revert -an chg#
	// we are done since the list is definitive
	if (m_pDeltaView->GetTreeCtrl().m_FileListDefinitive) 
		return FALSE;

	// for older servers we have to diff every file Not in the list.
	int i;
	CString fileonly = filename;
	BOOL rc = TRUE;

	m_StringList.RemoveAll();
	m_StringList.AddHead(LPCTSTR(filename));
	if ((i = fileonly.ReverseFind(_T('/'))) != -1)
		fileonly = fileonly.Mid(i+1);
	fileonly = _T("Diffing ") + fileonly;
	MainFrame()->UpdateStatus(fileonly);
	CCmd_Diff *pCmd= new CCmd_Diff;
	pCmd->Init( NULL, RUN_SYNC, HOLD_LOCK, m_pCallingCommand->GetServerKey());
	if ( pCmd->Run( &m_StringList, NULL, _T('r') ) && !pCmd->GetError() )
	{
		rc = ((pCmd->GetDiffNbrFiles() == 0) 
			|| pCmd->GetDiffErrCount()) ? TRUE : FALSE;
	}
	delete pCmd;
	return rc;
}


/*
	_________________________________________________________________
*/

void CP4SpecDlg::AddInput(const CString &prompt, const CString &editText, 
						  int specCode, BOOL readOnly, BOOL multiLine, 
						  BOOL promptAbove, int height, int width, BOOL required,
						  const CString &indent, const CString &wCode, int lioff,
						  BOOL showBrowse, BOOL allowDD)
{
	CRect browseRect;

	if (lioff < 1)
		m_Y = m_LiY[m_LiY.GetCount()-1 + lioff];

	CRect rect(m_X, m_Y, m_X + m_StdWidth, m_Y + m_StdHeight);

	if (indent == _T('M'))
	{
		rect.left += m_MaxWidth/2;
		rect.right += m_MaxWidth/2;
	}
	else if (indent == _T('I'))
	{
		rect.left += m_StdWidth + m_StdSpaceH;
		rect.right += m_StdWidth + m_StdSpaceH;
	}

	SpecControl sc;
	sc.CreateLabel(this, rect, prompt + ((required && !readOnly) ? _T(":*") : _T(":")));
	sc.AddToolTip(prompt, m_InstructionText);
	m_childControls.Add(CChildWindow(sc.label, CHILD_STATIC, &rect, 
						TRUE, 0, wCode == _T('H'), indent == _T('M'), indent == _T('R')));
	if (!m_HasRequired)
	{
		if (required && !readOnly)
			m_HasRequired = TRUE;
	}

	// Create the edit control
	int maxLines = 0;
	DWORD style = SetBasicWinStyles ( readOnly );

	if ( prompt == g_tagPassword )
		style |= ES_PASSWORD;

	if(multiLine)
	{
		if (prompt == _T("View") || prompt == _T("Reviews"))
		{
			style |= ES_MULTILINE | ES_AUTOVSCROLL | WS_HSCROLL | WS_VSCROLL | ES_WANTRETURN;
		}
		else
		{
			style |= ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | ES_WANTRETURN;
			if (prompt == g_tagDescription)
			{
				if (m_SpecType == P4CLIENT_SPEC
				 || m_SpecType == P4LABEL_SPEC
				 || m_SpecType == P4BRANCH_SPEC)
				{
					maxLines = 3;
					int l;
					if ((l = editText.GetLength()) > 25)
					{
						int n;
						if ((n = GetNbrNL(&editText)) > 3)
							maxLines = min(m_MinLi+1, n);
						else if (l > 240)
							maxLines = min(m_MinLi, editText.GetLength()/80) + 1;
					}
				}
			}
			else if (prompt == g_tagAltRoots)
				maxLines = 2;
		}
		if (maxLines)
			height = (m_StdHeight+4) * maxLines + EXTRAHEIGHT;
		else
			m_NumMultiLineChildWnds++;
	}
    else
        style |= ES_AUTOHSCROLL;

	if (showBrowse)
	{
		CWnd *pWnd= GetDlgItem(IDC_BROWSE);
	 	pWnd->EnableWindow();
		pWnd->GetWindowRect(&browseRect);
		height = max(height, browseRect.Height());
		if (width < 0)
			width += browseRect.Width() + m_StdSpaceH;
		else
			width -= browseRect.Width() + m_StdSpaceH;
		m_BrowseFldCtrlID = m_childControls.GetCount();
	}

	if ((m_SpecType != P4CHANGE_SPEC && m_SpecType != P4JOB_SPEC && prompt == g_tagDescription)
	 || (m_SpecType == P4CLIENT_SPEC && prompt == g_tagAltRoots))
	{
		rect.SetRect(m_X + m_StdWidth + m_StdSpaceH
			, m_Y
			, m_X + width
			, m_Y + height );
		m_LiY.Add(m_Y);
		m_Y += m_StdSpaceV + height;
	}
	else if (width < 0)
	{
		rect.SetRect(m_X + m_StdWidth + m_StdSpaceH
			, m_Y
			, m_X - width			// or use this to center: , 0 - width - (m_X + m_StdWidth/2 + m_StdSpaceH)
			, m_Y + height );
		m_LiY.Add(m_Y);
		m_Y += m_StdSpaceV + height;
	}
	else if(promptAbove && GET_P4REGPTR()->AllowPromptAbove())
	{
		rect.SetRect( m_X
			, m_Y + m_StdHeight
			, m_X + width
			, m_Y + m_StdHeight + height ); 
		m_LiY.Add(m_Y);
		m_Y += m_StdHeight + m_StdSpaceV + height;
	}
	else
	{
		rect.SetRect(m_X + m_StdWidth + m_StdSpaceH
			, m_Y
			, m_X + width + (promptAbove ? 0 : m_StdWidth)
			, m_Y + height );
		m_LiY.Add(m_Y);
		m_Y += m_StdSpaceV + height;
	}
	if (indent == _T('M'))
	{
		rect.left += m_MaxWidth/2;
		rect.right = m_X + m_MaxWidth;
	}
	else if (indent == _T('I'))
	{
		rect.left += m_StdWidth + m_StdSpaceH;
		if (wCode == _T('H'))
			rect.right = rect.left + m_HlfWidth;
		else
			rect.right += m_StdWidth + m_StdSpaceH;
	}

	CP4EditBox * pEdit = sc.CreateEdit(this, rect, style, 
        (HMENU) IDC(m_SpecType, specCode), specCode, allowDD, m_SpecType);
	m_childControls.Add(CChildWindow(sc.control, 
		multiLine ? CHILD_MULTILINEEDIT : CHILD_SINGLELINEEDIT, 
		&rect, !readOnly, maxLines, wCode == _T('H'), indent == _T('M'), indent == _T('R')));

	if (m_SpecType == P4CLIENT_SPEC && prompt == g_tagAltRoots)
		pEdit->ShowScrollBar(SB_BOTH, FALSE);

	long len;
	if ((len = editText.GetLength()) > 25000)
		pEdit->SendMessage(EM_LIMITTEXT, len + 5000, 0);

	pEdit->SetWindowText(editText);
	
	// Store the spec code and control pointer
	m_specControls.Add(sc);

	if ( !m_pFirstControl )
		if ( !readOnly )
			m_pFirstControl = pEdit;

	if ( m_SetFocusHere )
	{
		m_SetFocusHere = FALSE;
		if ( !readOnly )
			m_pFocusControl = pEdit;
	}

	if (showBrowse)
	{
		m_BrowseBtnCtrlID = m_childControls.GetCount();
		browseRect.SetRect(rect.right + m_StdSpaceH, rect.top, 
						   rect.right + m_StdSpaceH + browseRect.Width(),
						   rect.top + browseRect.Height());
		GetDlgItem(IDC_BROWSE)->SetWindowPos(pEdit, browseRect.left, browseRect.top,
			browseRect.Width(), browseRect.Height(), SWP_SHOWWINDOW);
		m_childControls.Add(CChildWindow(GetDlgItem(IDC_BROWSE), CHILD_BUTTON, 
			&browseRect, TRUE, 1, FALSE, FALSE, TRUE));
		m_BrowseTag = prompt;
	}
}

/*
	_________________________________________________________________
*/

// Provide support for hiding the password edit box.  This might
// seem clumsy, but it avoids great complexity in SetControls() and
// UpdateSpec(), since those functions would otherwise have to track
// the position of the password in the specdef, skip over input 
// references at that index and decrement subsequent references

void CP4SpecDlg::AddHiddenEditBox( int i )
{
	CString tag = m_SpecData.GetTagOf( i );
	int specCode= m_SpecData.GetCodeOf( i );

	ASSERT( tag == _T("Password") );
    
	// Create a hidden label
	SpecControl sc;
	sc.CreateLabel(this);

	// Create a hidden edit control
	sc.CreateEdit(this, specCode);

	// Store the spec code and control pointer
	m_specControls.Add(sc);
}


/*
	_________________________________________________________________
*/

int CP4SpecDlg::DoCleanup() 
{
	//		Do some cleanup
	//
	for(int i = 0; i < m_specControls.GetSize(); i++)
	{
		SpecControl & sc = m_specControls[i];
		if (sc.isChkList)
		{	// this is needed because modless dialog CCheckListBoxs leak memory
			CReviewList *pList = (CReviewList *)sc.control;
			if (::IsWindow(pList->m_hWnd))
				pList->ResetContent();
		}
		delete sc.label;
		delete sc.control;
	}
	m_jobList = m_fileList = 0;
	m_childControls.RemoveAll();
	return 0;
}


BOOL CP4SpecDlg::SendSpec(LPCTSTR specText, BOOL submit/*=FALSE*/, 
						  BOOL reopen/*=FALSE*/, int unchangedFlag/*=0*/)
{
	m_Submitting= submit;

	// Set up and run SendSpec Asynchronously
	CCmd_SendSpec *pCmd= new CCmd_SendSpec;

	// 2005.1 SendSpec is no longer is HOLD_LOCK, m_pCallingCommand->GetServerKey()
	// unless this is a modal dialog. This chg was required for modless edit dialogs.
	if (m_bIsModal || (m_AutomaticallyUpdate && SERVER_BUSY()))
		pCmd->Init( m_hWnd, RUN_ASYNC, HOLD_LOCK, m_pCallingCommand->GetServerKey() );
	else
		pCmd->Init( m_hWnd, RUN_ASYNC );
	
	m_SendingSpec=FALSE;
	if( pCmd->Run( m_SpecType, specText, submit, m_pCallingCommand->IsForceEdit(), 
						  reopen, unchangedFlag, m_pCallingCommand->IsUFlag() ) )
	{
		DisableControls();
		m_SendingSpec=TRUE;
		return TRUE;
	}
	else
	{
		delete pCmd;
		return FALSE;
	}
}

LRESULT CP4SpecDlg::OnP4SendSpec(WPARAM wParam, LPARAM lParam)
{
	// Because the spec is sent async, we could arrive here before
	// DisableControls() finishes, so wait for it to finish
	// or we will crash in DisableControls() if Destroy() gets
	// called from EndSpecDlg()'s call to PostMessage().
	while (!m_SendingSpec)
		Sleep(100);

	CCmd_SendSpec *pCmd= (CCmd_SendSpec *) wParam;
	m_SendingSpec=FALSE;
    
	if(!pCmd->GetError() && !pCmd->GetTriggerError())
	{
		m_pCallingCommand->SetNewJobName( pCmd->GetNewJobName() );
		if( pCmd->GetNewChangeNum() > 0 )
			m_pCallingCommand->SetNewChangeNum( pCmd->GetNewChangeNum() );

		if( m_SetPermPassword )
			GET_P4REGPTR()->SetP4Password( m_NewPassword, TRUE, TRUE, TRUE ) ;
		else if( m_SetTempPassword )
			GET_P4REGPTR()->SetP4Password( m_NewPassword, TRUE, FALSE, TRUE ) ;

		delete pCmd;
		if (m_bIsModal)
		{
			CDialog *pParent = (CDialog *)GetParent();
			pParent->EndDialog( m_Submitting ? IDALTERNATE : IDOK);
		}
		else
			m_pCallingCommand->EndSpecDlg( m_Submitting ? IDALTERNATE : IDOK);
		return 0;
	}
	else if(pCmd->GetTriggerError())
	{
		if (m_bIsModal)
		{
			CDialog *pParent = (CDialog *)GetParent();
			pParent->EndDialog( IDNEEDTOREFRESH );
		}
		else
			m_pCallingCommand->EndSpecDlg( IDNEEDTOREFRESH );
		EnableControls();
		delete pCmd;
	}
	else
	{
		CString txt = pCmd->GetErrorText();
		if (m_Tag == _T("Job"))
        {
            // not sure what, if anything, will result in failure to overwrite
            // existing job.  But if error says Job field is missing, don't
            // complain about failing to overwrite existing.
            if(txt.Find(_T("Missing required field 'Job'.")) != -1)
			    txt += LoadStringResource(IDS_SPEC_CANT_OVERWRITE_JOB);
        }
		if (txt.IsEmpty())
			txt = LoadStringResource(IDS_FATAL_ERROR_UNABLE_TO_SEND_SPEC);
		AfxMessageBox( txt, MB_ICONHAND);

		// Failed submit of change assumed to be an update
		if( m_Submitting && (txt.Find(_T("WSAECONNABORTED")) == -1)
						 && (txt.Find(_T("WSAECONNREFUSED")) == -1)
						 && (txt.Find(_T("WSAECONNRESET")) == -1)
						 && (txt.Find(_T("WSAETIMEDOUT")) == -1)
						 && (txt.Find(_T("WSAEHOSTUNREACH")) == -1))
		{
			if (m_bIsModal)
			{
				CDialog *pParent = (CDialog *)GetParent();
				pParent->EndDialog( IDNEEDTOREFRESH );
			}
			else
				m_pCallingCommand->EndSpecDlg( IDNEEDTOREFRESH );
		}
		else
			EnableControls();

		delete pCmd;
	}
	
	return 0;
}

void CP4SpecDlg::DisableControls()
{
	for( int i=0; i<m_childControls.GetSize(); i++ )
	{
		// this weird hack is because the CWnd* of the Browse button
		// keeps changing(!), so it must be recalc'ed each time.
		// this is a side effect of making this dialog non-modal.
		if (i == m_BrowseBtnCtrlID)
			m_childControls[i].SetWindow(GetDlgItem(IDC_BROWSE));

		CWnd *pWnd= m_childControls[i].GetWindow();
		m_childControls[i].SetVisible( pWnd->IsWindowVisible() );
		m_childControls[i].SetEnabled( pWnd->IsWindowEnabled() );
		pWnd->ModifyStyle( WS_VISIBLE, WS_DISABLED, 0);
	}
	GetParent()->SendMessage(WM_MODIFYSTYLE, WS_VISIBLE, WS_DISABLED);

	CRect rectWindow;
	CRect rectClient;
	GetParent()->GetWindowRect( &rectWindow );
	GetClientRect( &rectClient );

	CRect rect= CRect( 0, 0, 450, 150 );
	if( m_Submitting )
		m_BusyMessage.SetWindowText(LoadStringResource(IDS_SUBMITTING_CHANGE_PLEASE_WAIT));
	else
		m_BusyMessage.SetWindowText(LoadStringResource(IDS_SENDING_SPECIFICATION_PLEASE_WAIT));

	m_BusyMessage.ModifyStyle( 0, WS_VISIBLE, 0 );
	::SendMessage(m_BusyMessage.m_hWnd, WM_SETFONT, (WPARAM)GetStockObject(SYSTEM_FONT), FALSE);
	m_BusyMessage.MoveWindow( &rect );

	CRect rectCancel;
	CWnd *pWnd= GetDlgItem(ID_FILE_CANCEL);
	pWnd->GetWindowRect(&rectCancel);
	rect.InflateRect(0, 0, rectWindow.Width()- rectClient.Width(), 
						   rectWindow.Height()- rectClient.Height() + rectCancel.Height());
	GetParent()->MoveWindow( &rect );
	GetParent()->CenterWindow();
	GetParent()->RedrawWindow();

	SetScrollRange(SB_VERT, 0, 0, TRUE);
	ShowScrollBar(SB_VERT, FALSE);

	// turn on and position the Cancel button
	pWnd->ShowWindow( SW_SHOWNORMAL );
	pWnd->EnableWindow();
	GetClientRect(&rect);
	rect.top    = rect.bottom - rectCancel.Height();
	rect.left   = (rect.Width() - rectCancel.Width())/2;
	rect.right  = rect.left + rectCancel.Width();
	pWnd->MoveWindow(&rect);
}


void CP4SpecDlg::EnableControls()
{
	// hide the 2 controls related to sending a spec
	GetDlgItem(ID_FILE_CANCEL)->ModifyStyle( WS_VISIBLE, WS_DISABLED, 0);
	m_BusyMessage.ShowWindow( SW_HIDE );

	// un-hide the other controls
	for( int i=0; i<m_childControls.GetSize(); i++ )
	{
		// this weird hack is because the CWnd* of the Browse button
		// keeps changing(!), so it must be recalc'ed each time.
		// this is a side effect of making this dialog non-modal.
		if (i == m_BrowseBtnCtrlID)
			m_childControls[i].SetWindow(GetDlgItem(IDC_BROWSE));

		CWnd *pWnd= m_childControls[i].GetWindow();
		if( m_childControls[i].GetVisible() )
			pWnd->ShowWindow( SW_SHOWNORMAL );
		if( m_childControls[i].GetEnabled() )
			pWnd->EnableWindow();
	}
	GetParent()->SendMessage(WM_MODIFYSTYLE, WS_DISABLED, WS_VISIBLE);

	if (!m_AutomaticallyUpdate)
		m_WinPos.RestoreWindowPosition();
	RedrawWindow();
}

// The user pressed the Enter key and we got a button click for IDOK
void CP4SpecDlg::OnEnter() 
{
	if (m_AllowSubmit)
		OnAlternate();
	else
		On_OK();
}

void CP4SpecDlg::On_OK() 
{
	if (!m_AutomaticallyUpdate)
		m_WinPos.SaveWindowPosition();

	if( UpdateSpec() )
	{
		SendSpec( m_NewForm, FALSE );
	}
}

void CP4SpecDlg::OnAlternate() 
{
	if (m_AllowSubmit)
	{
		if( m_fileList )
		{
			CString value;
			BOOL bChecked = FALSE;
			int count = m_fileList->GetCount();
			for(int i=0; i<count; i++)
			{
				m_fileList->GetText(i, value);
				if(!value.IsEmpty() && m_fileList->GetCheck(i)==1)
				{
					bChecked = TRUE;
					break;
				}
			}
			if (!bChecked)
			{
				AfxMessageBox(IDS_SPEC_EMPTY_SUBMIT, MB_ICONEXCLAMATION);
				return;
			}
		}
		if (!m_AutomaticallyUpdate)
			m_WinPos.SaveWindowPosition();
		if( UpdateSpec() )
		{
			SendSpec( m_NewForm, TRUE, GET_SERVERLEVEL() >= 13 ? m_bReopen : FALSE, 
									   GET_SERVERLEVEL() >= 21 ? m_UnchangedFlag : 0 );
		}
	}
	else MessageBeep(0);
}

void CP4SpecDlg::On_Cancel() 
{
	if (!m_AutomaticallyUpdate)
		m_WinPos.SaveWindowPosition();
	if (m_bIsModal)
	{
		CDialog *pParent = (CDialog *)GetParent();
		pParent->EndDialog(IDCANCEL);
	}
	else
		m_pCallingCommand->EndSpecDlg(IDCANCEL);
}

void CP4SpecDlg::OnCancel() 
{
	// Eat ESC while sending spec, so the server's reply can
	// be properly processed.  Don't confuse this with On_Cancel(),
	// which is called when the cancel button is hit.
	if( ! m_SendingSpec )	
	{
		if (m_ChangesHaveBeenMade)	// Have changes been made? This is our best guess...
		{
			// Warn that they are about to discard changes
			BOOL b;
			if ((b = GET_P4REGPTR()->DontShowDiscardFormChgs()) == FALSE)
			{
				if (MsgBox(IDS_DISCARD_CHGS, MB_ICONQUESTION | MB_DEFBUTTON2, 
							0, this, &b) == IDC_BUTTON2)
					return;
				if (b)
					GET_P4REGPTR()->SetDontShowDiscardFormChgs(b);
			}
		}
		// Cancel the dialog
		if (!m_AutomaticallyUpdate)
			m_WinPos.SaveWindowPosition();
		if (m_bIsModal)
		{
			CDialog *pParent = (CDialog *)GetParent();
			pParent->EndDialog(IDCANCEL);
		}
		else
			m_pCallingCommand->EndSpecDlg(IDCANCEL);
	}
}


/*
	_________________________________________________________________
*/

int CP4SpecDlg::WordCount( const CString& cst, int type )
{
	CTokenString tstr;

	tstr.Create(cst);
	int count=0;
	while( lstrlen( tstr.GetToken(TRUE)) > 0)
		count++;

	return count ;
}


/*
	_________________________________________________________________
*/

BOOL CP4SpecDlg::CheckNumWords ( const CString tag, const CString &cst
						, int type , int words, int required, CString &msg )
{
	msg.Empty ( );

	if ( !required && cst.IsEmpty( ) )
		return TRUE;
	
	int count = words;
	if ( type == SDT_WORD )
	{
		count = WordCount( cst, type );
		
		if( count > words )
		{
            msg.FormatMessage( words > 1 ? IDS_FIELD_s_CAN_HAVE_ONLY_n_WORDS : IDS_FIELD_s_CAN_HAVE_ONLY_ONE_WORD, 
				tag, words);
			return FALSE;
		}
		else if( count < words && ( count > 0 || required != 0))
		{
            msg.FormatMessage( words > 1 ? IDS_FIELD_s_NEEDS_n_WORDS : IDS_FIELD_s_NEEDS_ONE_WORD, 
				tag , words);
			return FALSE;
		}
	}
	else if( type == SDT_WLIST )
	{
		CString r = cst;
		while ( !r.IsEmpty( ) )
		{
			int len = r.Find ( g_CRLF );
            CString l;
            if(len == -1)
            {
                l = r;
                r.Empty();
            }
            else if(len == 0)
            {
                r = r.Mid(2);
                continue;		// found a blank line, go try again
            }
            else
            {
                l = r.Left(len);
                r = r.Mid(len+2);
            }

			count = WordCount( l, type );
		
			if( count > words )
			{
                msg.FormatMessage( words > 1 ? IDS_FIELD_s_CAN_HAVE_ONLY_n_WORDS_PER_LINE : IDS_FIELD_s_CAN_HAVE_ONLY_ONE_WORD_PER_LINE, 
    				tag, words);
				return FALSE;
			}
			else if( count < words && ( count > 0 || required != 0))
			{
                msg.FormatMessage( words > 1 ? IDS_FIELD_s_NEEDS_n_WORDS_PER_LINE : IDS_FIELD_s_NEEDS_ONE_WORD_PER_LINE, 
    				tag, words);
				return FALSE;
			}
		}
	}

	return count == words;
}

/*
	_________________________________________________________________

	Calculate the width of the widest prompt in pixels
	_________________________________________________________________
*/

int CP4SpecDlg::GetLengthLongestPrompt()
{
	CString longest, tag;
	int mx = 0;
    for( int i = 0; i < m_SpecData.GetNumItems( ); i++ ) 
	{
		tag = m_SpecData.GetTagOf( i );
		int lgth = tag.GetLength();
		if (mx < lgth)
		{
			mx = lgth;
			longest = tag;
		}
	}

	CDC *pDC = GetDC();
	CFont *pOldFont= pDC->SelectObject( GetFont() );
	pDC->SelectObject( &m_Font );
	CSize sz = pDC->GetTextExtent(longest + _T(":*"));
	pDC->SelectObject( pOldFont );
	ReleaseDC( pDC );

	return sz.cx;
}


/*
	_________________________________________________________________
*/

CString CP4SpecDlg::GetDialogueCaption ( )
{
    CString types = LoadStringResource(IDS_SPECCAPTION_TYPES);
    for(int i = 0; i < m_SpecType; i++)
        types = types.Mid(types.Find(_T('|'))+1);
    types = types.Left(types.Find(_T('|')));

    CString winCaption;
	winCaption.FormatMessage( IDS_SPECCAPTION_s, types );
	return winCaption;
}


/*
	_________________________________________________________________
*/

BOOL CP4SpecDlg::SetControls() 
{
	ASSERT(m_specControls.GetSize() == 0);
	ASSERT( m_childControls.GetSize() <= 6 );
	ASSERT( m_NumMultiLineChildWnds == 0 );

	int code = -1, type;
	CString value, tag;
	BOOL isReadOnly;
	CStringArray aPresets;
	BOOL bJobsDone = FALSE;

	m_OrigPassword.Empty();
	m_ReadOrigPassword= FALSE;

	m_OrigRoot.Empty();
	m_OrigView.Empty();
	m_ReadOrigRoot = m_ReadOrigView = m_SyncAfter = FALSE;

	RECT rectPos;
	m_ReqStatic.GetWindowRect(&rectPos);
	ScreenToClient(&rectPos);
	int w = rectPos.right - rectPos.left;
	rectPos.right = m_X + m_MaxWidth;
	rectPos.left  = rectPos.right - w;
	m_ReqStatic.MoveWindow(&rectPos);
	m_childControls.Add(CChildWindow(&m_ReqStatic, CHILD_RIGHTSTATIC, &rectPos));
	m_Y = rectPos.bottom + 1;

    for( int i = 0; i < m_SpecData.GetNumItems( ); i++ ) 
	{	
		//		put m_Specdata stuff for this item in local variables
		//		to prevent code litter. manipulate these variables
		//		and then before the call to Add() copy them into the spec data
		//
		code = m_SpecData.GetCodeOf( i );
		type = m_SpecData.GetTypeOf( i );
		isReadOnly = m_SpecData.GetReadOnlyOf( i );
		tag = m_SpecData.GetTagOf( i );
		value = m_SpecData.GetValueOf ( i );

		switch ( type )
		{
			//		for a block of text or a single line of text, strip the tabs
			//
			case SDT_TEXT:
				if (GET_P4REGPTR()->PreserveSpecFormat( ))
				{
					value = MakeCRs( value );
					break;
				}
			case SDT_BULK:	
			case SDT_LINE:
			case SDT_DATE:
				value = RemoveTabs( value );
				break;

			//		for multiple lines, wait until loop ends
			//		(job list for changelist, user reviews list, and views )
			//
			case SDT_WLIST:	
			case SDT_LLIST:	
				if ( tag == g_tagAltRoots )
				{
					// for multiple client roots, we will have both a line type
					// element called "Root" and a llist type element called 
					// "AltRoots".  Create a view control for the AltRoots info.
					CWnd * pEdit = m_pFocusControl;
					AddView( g_tagAltRoots, m_SpecData.m_aList );
					m_pFocusControl = pEdit;
					for (int j = -1; ++j < m_SpecData.m_aList.GetSize(); )
					{
						if (!m_OrigRoot.IsEmpty())
							m_OrigRoot += _T('\r');
						m_OrigRoot += m_SpecData.m_aList.GetAt(j);
					}
					m_ReadOrigRoot= TRUE;
				}
				continue;

			case SDT_WORD:	// single line, N words
				break;

			case SDT_SELECT:
				GetComboBoxValues( value, aPresets );
				break;

			default:
				break;
		}

		// Keep track of original password
		if( tag == _T("Password") )
		{
			if( value.Find(_T("******")) == 0 )
				value= GET_P4REGPTR()->GetP4UserPassword();

			m_OrigPassword= value;
			m_ReadOrigPassword= TRUE;
		}
		// Keep track of original root
		else if( tag == g_tagRoot )
		{
			if (!m_Root2Use.IsEmpty())
				value = m_Root2Use;
			m_OrigRoot= value;
			m_ReadOrigRoot= TRUE;
		}

		//		user is creating a job, label, branch, etc.
		//		make the value blank so user can fill it in.
		//
		if( value == NEWSPECNAME )
		{
			value.Empty( );
			isReadOnly = FALSE; 
		}

		//		Job complications:
		//
		//		1.	The server allows any job to be changed, but that screws
		//			up the gui's refresh of the job window, because it  
		//			copies rather than renames the job
		//
		if( m_SpecType == P4JOB_SPEC && tag == g_tagJob )
			if( value !=  _T("new") )
				isReadOnly = TRUE; 

		//		2.	if a job's status is 'new", the status should be read only 
		//
		if( m_SpecType == P4JOB_SPEC ) 
			if ( tag == g_tagStatus )
				if( value == _T("new") )
					isReadOnly = TRUE; 

		//		Changelist complication:
		//
		//		The Job Status should go after the jobs
		//
		if( m_SpecType == P4CHANGE_SPEC && tag == g_tagJobStatus )
		{
			if ( m_SpecData.m_aJobs.GetSize ( ) )
			{
				if ( ! m_SpecData.m_aJobs.GetAt( 0 ).IsEmpty( ) )
				{
					AddList( _T("Jobs"), m_SpecData.m_aJobs, code, 3 * m_StdHeight, m_MaxWidth, m_MaxWidth );
					m_jobList = dynamic_cast<CReviewList*>(m_specControls[m_specControls.GetUpperBound()].control);
				}
			}
			bJobsDone = TRUE;
			// Add some backwards compatible spec info
			if( GET_SERVERLEVEL() < 16 && GET_SERVERLEVEL() > 7 )
			{
				m_SpecData.SetIndentOf( i, _T("I") );
				m_SpecData.SetwCodeOf ( i, _T("H") );
			}
		}

		//		okay, we've manipulated all we can. set any spec data value
		//		that's changed.
		//
		m_SpecData.SetValueOf ( i, value );
		m_SpecData.SetIsReadOnlyOf( i, isReadOnly );

		//		okay, add either an edit field or a combo box to the dialogue
		//
		if( ( GET_SERVERLEVEL() >= 6 &&  tag == _T("Password") ) )
			AddHiddenEditBox( i );
		else if ( type == SDT_SELECT )
		{
			AddComboBox( tag, aPresets, m_SpecData.GetValueOf( i ), code , isReadOnly, 
						  m_StdHeight , m_ComboWidth, m_SpecData.GetRequiredOf( i ), 
						  m_SpecData.GetIndentOf( i ), m_SpecData.GetwCodeOf( i ),
						  m_SpecData.GetLiOffsetOf( i )); 
			aPresets.RemoveAll( );
		}
		else
			AddEditBox( i );
	}

	//		now add any views, job lists or file lists
	//
	if ( m_SpecData.m_aReview.GetSize ( ) )
		AddView( g_tagReviews, m_SpecData.m_aReview, m_SpecType == P4USER_SPEC );

	if ( m_SpecData.m_aWordList.GetSize( ) )
	{
		BOOL b = m_SpecType == P4LABEL_SPEC || m_SpecType == P4CLIENT_SPEC;
		AddView( g_tagView, m_SpecData.m_aWordList, b );
		CString e;
		for ( int i = 0; i < m_SpecData.m_aWordList.GetSize( ); i++ )
		{
			CObject *o = m_SpecData.m_aWordList.GetAt( i );
			CStringArray *sa = ( CStringArray * )o;
			for( int j = 0; j < sa->GetSize( ) ; j++ )
			{
				e = sa->GetAt( j );
				m_OrigView += e + _T(" ");
			}
			m_OrigView += g_CRLF;
		}
		m_ReadOrigView = TRUE;
	}

	if ( m_SpecData.m_aJobs.GetSize ( ) && !bJobsDone )
	{
		if ( ! m_SpecData.m_aJobs.GetAt( 0 ).IsEmpty( ) )
		{
			AddList( _T("Jobs"), m_SpecData.m_aJobs, code, 3 * m_StdHeight, m_MaxWidth, m_MaxWidth );
			m_jobList = dynamic_cast<CReviewList*>(m_specControls[m_specControls.GetUpperBound()].control);
		}
	}
	if ( m_SpecData.m_aFile.GetSize ( ) && m_AddFilesControl )
	{
		if ( ! m_SpecData.m_aFile.GetAt( 0 ).IsEmpty( ) )
		{
            ASSERT(code > -1);
			AddList( g_tagFile, m_SpecData.m_aFile, code, 6 * m_StdHeight, m_MaxWidth, m_MaxWidth*3 );
			m_fileList = dynamic_cast<CReviewList*>(m_specControls[m_specControls.GetUpperBound()].control);
			MainFrame()->UpdateStatus(_T(" "));
		}
	}
	else m_SpecData.m_aFile.RemoveAll( );

	if (!m_HasRequired)
		m_ReqStatic.ShowWindow(SW_HIDE);

	return TRUE;
}


/*
	_________________________________________________________________

	all possible values and the actual value for this field are in 
	one cstring ( e.g., "SIR:BUG/SIR/unknown" )
	put the real value, which is to the left of the colon, into value.
	put all possible values, to the right of the colon and separated
	by slashes, into a string array for the combo box. 
	_________________________________________________________________
*/

void CP4SpecDlg::GetComboBoxValues( CString &value, CStringArray &aPresets )
{
	int where = value.Find( g_SelectionSeparator );
	if ( where > -1 )
	{
		CString presets = value.Right( value.GetLength( ) 
			- where - g_SelectionSeparator.GetLength( ) );
		CString pre = presets;
		value = value.Left ( where );
		while ( pre.GetLength( ) )
		{
			int sep = presets.Find( g_PresetSeparator );
			if(sep > -1)
			{
				pre = presets.Left( sep );
				aPresets.Add( pre );
			}
			else
			{
				pre.Empty();
				aPresets.Add( presets );
			}
			presets = presets.Right ( presets.GetLength( ) - (sep + 1));
		}
	}
}


/*
	_________________________________________________________________
*/

void CP4SpecDlg::AddEditBox( int i )
{
	int code = m_SpecData.GetCodeOf( i );
	int type = m_SpecData.GetTypeOf( i );
	CString tag = m_SpecData.GetTagOf( i );
	BOOL isReadOnly = m_SpecData.GetReadOnlyOf( i );
	BOOL showBrowse = FALSE;

	BOOL bMultiLine = TRUE; 
	int height = m_StdHeight;
	int width = m_MaxWidth;
	int maxlen = m_SpecData.GetSizeOf( i );

	int lioff = m_SpecData.GetLiOffsetOf( i );
	CString indent = m_SpecData.GetIndentOf( i );
	CString wCode = m_SpecData.GetwCodeOf( i );

	// If the 1st field on the spec is read-only
	// widen it to the full width of the form
	// (only necessary for newer servers tho,
	// older servers serve fullwidth name fields)
	if (!i && isReadOnly && (GET_SERVERLEVEL() >= 16) && (m_SpecType != P4JOB_SPEC))
	{
		wCode = _T("R");
	}
	// Add some backwards compatible spec info
    else if( GET_SERVERLEVEL() < 16 && GET_SERVERLEVEL() > 7 )
    {
		switch(m_SpecType)
        {
		case P4CHANGE_SPEC:
			if (i >= 1 && i <= 4)
			{
				lioff = (i == 2 || i == 4) ? 0 : 1;
				indent = (i == 1 || i == 4) ? _T("M") : _T("B");
				wCode = _T("H");
			}
			break;
        case P4CLIENT_SPEC:
			if (i >= 1 && i <= 4)
			{
				lioff = i == 3 ? -1 : 1;
				indent = i > 2 ? _T("M") : _T("B");
				wCode = _T("H");
			}
			else if (tag == _T("Root") && !m_BrowseShown)
				showBrowse = m_BrowseShown = TRUE;
			break;
        case P4BRANCH_SPEC:
        case P4LABEL_SPEC:
			if (i >= 1 && i <= 3)
			{
				lioff  = i == 3 ? 0 : 1;
				indent = i == 3 ? _T("M") : _T("B");
				wCode = _T("H");
			}
            break;
        case P4USER_SPEC:
			if (i >= 1 && i <= 4)
			{
				wCode = _T("H");
				if ((i == 1) || (i == 4))
				{
					indent = _T("M");
					lioff = (i == 1) ? 1 : 0;
				}
				else if (i == 2)
					lioff = 0;
			}
            break;
        }

    }
	if ( type == SDT_WLIST  || type == SDT_LLIST
		|| ( type == SDT_TEXT &&  maxlen == 0  ) )
	{
		height = 4 * m_StdHeight + EXTRAHEIGHT;
	}
	else if ( type == SDT_TEXT && maxlen > 63  )
	{
		m_MinLi = m_SpecData.GetNumItems( ) > 16 ? m_MinLi : max(m_MinLi, 4);
		height = EXTRAHEIGHT + max( m_MinLi * m_StdHeight,  ( ( maxlen + 63 ) / 64 ) * m_StdHeight );
	}
	else
	{
		width = ((tag == "Root") || (tag == _T("Options"))) ? -1 * m_MaxWidth : m_ComboWidth;
		bMultiLine = FALSE;
	}
	if (wCode.GetAt(0) == _T('H') && (width > m_HlfWidth))
		width = m_HlfWidth;
		
	//		if we have a multiline field, put the tag above the
	//		field instead of along side it.
	//
	BOOL tagAboveValue = bMultiLine;

	// This is temp until the server is smarter
	// should be if( GET_SERVERLEVEL() == 16 && 
	if (tag == _T("Root") && m_SpecType == P4CLIENT_SPEC && !m_BrowseShown)
		showBrowse = m_BrowseShown = TRUE;

	AddInput( tag 
		, m_SpecData.GetValueOf( i )
		, code, isReadOnly
		, bMultiLine, tagAboveValue
		, height, width, m_SpecData.GetRequiredOf ( i )
		, indent, wCode, lioff, showBrowse );
}


/*
	_________________________________________________________________

	string all views together into one string, separate with carriage
	return-line feed, and put into dialogue
	_________________________________________________________________
*/

void CP4SpecDlg::AddView( const CString &tag, const CObArray &oa, BOOL allowDD/*=FALSE*/ )
{
	CString s;
	CString e;

	for ( int i = 0 ; i < oa.GetSize ( ) ; i++ )
	{
		CObject *o = oa.GetAt( i );
		CStringArray *sa = ( CStringArray * )o;
		for( int j = 0; j < sa->GetSize( ) ; j++ )
		{
			e = sa->GetAt( j );
			if (e != _T("@"))		// single @ means put a blank line here
				s += e + _T(" ");
			else
				m_SetFocusHere = TRUE;
		}
		s += g_CRLF;
	}

	AddInput( tag 
			, s 
			, 0		//fanny: what do i need this for?
			, FALSE //		is read only
			, TRUE	//		all views are multiline
			, TRUE	//		tag is above the value, not to the side
			, 4 * m_StdHeight 
			, m_MaxWidth
			, FALSE,_T("B"),_T("A"),1,FALSE,allowDD
			);
}


/*
	_________________________________________________________________
*/

void CP4SpecDlg::AddView( const CString &tag, const CStringArray &sa, BOOL allowDD/*=FALSE*/ )
{
	CString s;
	CString e;
	for ( int i = 0; i < sa.GetSize( ); i++ )
	{
		e = sa.GetAt( i );
		if (e.IsEmpty())
			m_SetFocusHere = TRUE;
		s += e + g_CRLF;
	}

	AddInput( tag 
			, s 
			, 0		//fanny: what do i need this for?
			, FALSE //		is read only
			, TRUE	//		all views are multiline
			, TRUE	//		tag is above the value, not to the side
			, 4 * m_StdHeight 
			, m_MaxWidth
			, FALSE,_T("B"),_T("A"),1,FALSE,allowDD
			);
}


/*
	_________________________________________________________________
*/

// A replacement for CString::Remove that overcomes a bug in the original
// when operating on MBCS strings.
void CStringRemove(CString &str, TCHAR chRemove)
{
	SET_BUSYCURSOR();
	// if the character to be removed is less than a space
	// we can safely use the native (and much faster!) remove
	if ((unsigned)chRemove < (unsigned)_T(' '))
	{
		str.Remove(chRemove);
	}
	else
	{
		for(int pos = 0; (pos = str.Find(chRemove, pos)) != -1; )
		{
			str.Delete(pos);
			if(pos)
				pos--;
		}
    }
}

BOOL CP4SpecDlg::UpdateSpec( )
{
	CString value;
	CString tag;
	int type;
	int words;
	int required;
	CString txt;
	CString newRoot = _T("");
	CString newView = _T(":");
	CWnd *pControl;

	m_SetPermPassword= FALSE;
	m_SetTempPassword= FALSE;
	m_NewPassword= m_OrigPassword;
	m_Tag.Empty();

	int i;
	for( i = 0; i< m_specControls.GetSize(); i++ ) 
	{
		pControl = m_specControls[ i ].control;
		// skip over any dummy controls
		if(!pControl)
			continue;
		value = GetNewValue( pControl, i );
		tag = m_SpecData.GetTagOf( i );
		type = m_SpecData.GetTypeOf( i );
		words = m_SpecData.GetNWordsOf ( i );
		required = m_SpecData.GetRequiredOf ( i );

		//  strip out any leading or trailing whitespace
		//	unless this is a TEXT field and we are to preserve
		//	the whitespace.
		if ((type != SDT_TEXT) || !GET_P4REGPTR()->PreserveSpecFormat( ))
		{
			value.TrimRight();
			value.TrimLeft();
			if (type == SDT_TEXT)
//				value.Remove(_T('\r'));
                CStringRemove(value, _T('\r'));
            else if(type != SDT_BULK)
            {
                // if value is quoted, make sure there's something inside the quotes
                // otherwise, it's the same as empty
                if(value == "\"" || value == "\"\"")
                    value.Empty();

				if (type != SDT_WORD)
				{
					// anything following a '#' will be considered a comment and stripped
					// so do it here to allow better error detection and handling
					int comment = value.Find(_T('#'));
					if(comment != -1)
					{
						if (type == SDT_LINE)
						{
							value.TrimLeft(_T('\"'));
							value.TrimRight(_T('\"'));
							value = _T('\"') + value + _T('\"');
						}
						else
						{
							CString errTxt;
							errTxt.FormatMessage(IDS_NO_POUND_ALLOWED_s_s, tag, value);
							return ShowUserError( errTxt, pControl );
						}
					}
				}
            }
		}

		//		Verify that required field is not empty, that it has
		//		the number of words it's supposed to have, and that
		//		if it's the description, it doesn't have the garbage
		//		field still in it
		//
		if ( m_SpecType == P4JOB_SPEC && tag == _T("Job") )
		{
			value.TrimRight(_T(" \""));
			value.TrimLeft (_T(" \""));
		}
		if ( value.IsEmpty( ) && required )
        {
            CString errTxt;
            errTxt.FormatMessage(IDS_PLEASE_ENTER_A_VALUE_FOR_s, tag);
			return ShowUserError( errTxt, pControl );
        }

		// For the Client View the following is not considered valid:
		// -"//depot/two words/... //myclient/two words/...
		// because the - needs to be inside the quotes for CheckNumWords().
		// So replace all /n-" with /n"-
		if ( type == SDT_WLIST && m_SpecType == P4CLIENT_SPEC && tag == _T("View") )
			value.Replace(_T("\n-\""), _T("\n\"-"));

		if ( !CheckNumWords ( tag, value, type , words, required, txt ) )
			return ShowUserError( txt, pControl );

		if ( value == CCmd_EditSpec::g_blankDesc )
			return ShowUserError( LoadStringResource(IDS_PLEASE_ENTER_A_DESCRIPTION), pControl );

		if ( tag == LoadStringResource(IDS_DESCRIPTION) )
		{
			//		Job description needs to have '\r' chars removed and possibly be wrapped.
			//
			if ( m_SpecType == P4JOB_SPEC )
			{
				if (GET_P4REGPTR()->GetDescWrapSw())
					value = WrapDesc(value, GET_P4REGPTR()->GetDescWrap());
				value = UnMakeCRs( value );
			}
			//		Change descriptions might need to be wrapped
			//
			else if ( m_SpecType == P4CHANGE_SPEC )
			{
				if (GET_P4REGPTR()->GetDescWrapSw())
					value = WrapDesc(value, GET_P4REGPTR()->GetDescWrap());
				m_NewChangeDesc = value;
				m_pCallingCommand->SetChangeDesc(value);
			}
		}
		if( (GET_SERVERLEVEL() >= 6) && (tag == _T("Password")) )
		{
			if( GET_SERVERLEVEL() >= 18)
				value = _T("******");
			// else just ignore the hidden edit field, since value didnt change
		}
		else if ( tag == _T("Password") && value != m_OrigPassword )
		{
			if( AfxMessageBox( IDS_YOU_HAVE_CHANGED_YOUR_USER_PASSWORD, MB_YESNO|MB_ICONQUESTION) == IDYES )
				m_SetPermPassword= TRUE;
			else
				m_SetTempPassword= TRUE;
			
			m_NewPassword= value;
		}

		// check to see it the label name they typed is the same as an existing label name
		// issue warning if they said they were intending to create a new label (rather than
		// intending to update an existing label)
		if( ( ( tag == _T("Label") ) 
			&& m_pCallingCommand->GetIsRequestingNew()
			&& (((CLabelView *)(m_pCallingCommand->GetCaller()))->GetListCtrl().FindInList(value) != -1) )
		 || ( ( tag == _T("Branch") ) 
			&& m_pCallingCommand->GetIsRequestingNew()
			&& (((CBranchView *)(m_pCallingCommand->GetCaller()))->GetListCtrl().FindInList(value) != -1) )
		 || ( ( tag == _T("Client") ) 
			&& m_pCallingCommand->GetIsRequestingNew()
			&& (((CClientView *)(m_pCallingCommand->GetCaller()))->GetListCtrl().FindInListAll(value) != -1) ) 
		 || ( ( tag == _T("User") ) 
			&& m_pCallingCommand->GetIsRequestingNew()
			&& (((CUserView *)(m_pCallingCommand->GetCaller()))->GetListCtrl().FindInList(value) != -1) ) )
		{
			CString txt;
			txt.FormatMessage(IDS_s_s_ALREADY_EXISTS_CONTINUING_WILL_OVERWRITE_s, tag, value, value);
			if(AfxMessageBox(txt, MB_YESNO|MB_ICONEXCLAMATION) != IDYES)
				return FALSE;
		}
		else if ( tag == _T("Job") )
		{
			if (value.GetAt(0) == _T('-'))
			{
				CString txt;
				txt.FormatMessage(IDS_SPEC_INVALID_JOBNAME_s, value);
				AfxMessageBox(txt, MB_ICONEXCLAMATION);
				return FALSE;
			}
			if ( m_pCallingCommand->GetIsRequestingNew() )
			{
				if (((CJobView *)(m_pCallingCommand->GetCaller()))->GetListCtrl().FindInList(value) != -1)
				{
					CString txt;
					txt.FormatMessage(IDS_SPEC_s_s_ALREADY_EXISTS_CANT_OVERWRITE_s_WITH_s, tag, value, value, tag);
					AfxMessageBox(txt, MB_ICONEXCLAMATION);
					return FALSE;
				}
				else m_Tag = tag;
			}
		}

		if ( m_SpecType == P4CLIENT_SPEC && tag == _T("Root") )
		{
			newRoot = value + _T('\r');
			m_SpecData.SetValueOf( i , value ) ;					 //		set the new value
		}
		else if( tag == _T("Reviews") )
			m_SpecData.UpdateReviews( value );
		else if ( tag == _T("Jobs") || tag == _T("Files") )
			;//they are taken care of below
		else if ( type == SDT_LLIST )
			m_SpecData.UpdateLList( m_SpecData.m_aList, value );
		else if ( type == SDT_WLIST ) //views and a possible user defined one
		{
			// remove any occurances of multiple blank lines since the server will reject them
			if ( tag == _T("View") )
				while (value.Replace(_T("\r\n\r\n"), _T("\r\n")))
					;
			m_SpecData.UpdateWordList( words, value );
			if ( m_SpecType == P4CLIENT_SPEC && tag == _T("View") )
				newView = value;
		}
		else if( !( tag == _T("Password"
				 && GET_SERVERLEVEL() < 18 
				 && GET_SERVERLEVEL() >= 6 ) ) ) // if no need to ignore the hidden password field
			m_SpecData.SetValueOf( i , value ) ;					 //		set the new value
	}

	// If this is an existing client and they've changed the Root, issue warning
	if (m_SpecType == P4CLIENT_SPEC && !newRoot.IsEmpty())
	{
		ASSERT(m_ReadOrigRoot);
		int n;
		if ((n = m_SpecData.m_aList.GetSize()) > 0)
		{
			for (int j = -1; ++j < n; )
				newRoot += m_SpecData.m_aList.GetAt(j);
		}
		newRoot.TrimRight();
		m_OrigRoot.TrimRight();
#ifdef _DEBUG
		CString msg;
		msg.Format( _T("Old Root=[%s]"),  m_OrigRoot);
		AddToStatus( msg, SV_DEBUG );
		msg.Format( _T("New Root=[%s]"),  newRoot);
		AddToStatus( msg, SV_DEBUG );
		msg.Format( _T("IsANewClient=[%d]"),  m_pCallingCommand->GetIsNewClient());
		AddToStatus( msg, SV_DEBUG );
#endif
		if (newRoot.CompareNoCase(m_OrigRoot) && !m_pCallingCommand->GetIsNewClient())
		{
			if (IDYES != AfxMessageBox(IDS_YOU_HAVE_CHANGED_THE_ROOT_FOR_THIS_CLIENT_YOU_MUST,
								MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION))
				return FALSE;
		}
	}

	// Have they changed the client View?
	if (m_SpecType == P4CLIENT_SPEC && !newView != CString(_T(":")))
	{
		ASSERT(m_ReadOrigView);
		newView.TrimRight();
		m_OrigView.TrimRight();
#ifdef _DEBUG
		CString msg;
		msg.Format( _T("Old View=[%s]"),  m_OrigView);
		AddToStatus( msg, SV_DEBUG );
		msg.Format( _T("New View=[%s]"),  newView);
		AddToStatus( msg, SV_DEBUG );
		msg.Format( _T("IsANewClient=[%d]"),  m_pCallingCommand->GetIsNewClient());
		AddToStatus( msg, SV_DEBUG );
#endif
		if (newView != m_OrigView)
			m_SyncAfter = TRUE;
	}

	// For numbered changelists, if any job or file is unchecked, then
	// set flag so we know to refresh the pending changelists pane
	m_EditedLists= FALSE;
 
	// Add jobs
	int count= 0;
	m_SpecData.m_aJobs.RemoveAll ( );
	if( m_jobList )
	{
		count = m_jobList->GetCount( );
		
		for( i = 0; i< count; i++ )
		{
			m_jobList->GetText( i, value );
			if( value != "" && m_jobList->GetCheck( i ) == 1 )
			{
				// Jobs are supposed to be one word
				// but we may have fiddled with the spec
				// in order to get the description to show; 
				// therefore grab only the 1st word for each value
				int j;
				if ((j = value.Find(_T(" "))) != -1)
					value = value.Left(j);
				m_SpecData.m_aJobs.Add( value );
			}
			else
				m_EditedLists= TRUE;
		}
	}
	
	// Add files
	m_SpecData.m_aFile.RemoveAll ( );
	if( m_fileList )
	{
		count=m_fileList->GetCount(); 
				
		for(i=0; i<count; i++)
		{
			m_fileList->GetText(i, value);
			if(value!=_T("") && m_fileList->GetCheck(i)==1)
				m_SpecData.m_aFile.Add( value );
			else
				m_EditedLists= TRUE;
		}
	}

	//		okay, m_SpecData is updated with the new values.
	//		call m_Spec's Format( ), which will call m_SpecData's 
	//		Get( ) function
	//
	try
	{
		StrBuf *str = m_Spec.Format( ( SpecData * )&m_SpecData );
		m_NewForm = CharToCString(str->Text());

		// 98.2 server will bungle a blank password, so just remove that line from the spec
		// in cases where password is blank
		if( ( GET_SERVERLEVEL() < 6 && m_NewPassword.IsEmpty()) )
			RemovePasswordFromSpec();
		
        delete str;

		if (m_FoundLineElemWithValues != -1)	// Do we need to put the "line with values" stuff back?
		{
			if (!RestoreLineElemWithValues( m_NewForm, m_OrigSpecDefStr ))
			{
				AfxMessageBox(IDS_SPEC_COULDNT_RESTORE, MB_ICONSTOP );
				return FALSE;
			}
		}

        CString txt;
        // don't try to display the whole thing, cuz if it's very large FormatMessage
        // will thrown a memory exception.
        txt.FormatMessage(IDS_THE_NEW_FORM_IS_s, m_NewForm.Left(1000));
		AddToStatus( txt, SV_DEBUG );
	}
	catch(...)
	{
		AfxMessageBox( IDS_BAD_SPEC, MB_ICONSTOP );
		return FALSE;
	}
	
	return TRUE;
}

/*
	_________________________________________________________________

	When running against a 98.2 server, the 'Password:' tag must not
	be included if the password is blank.
	_________________________________________________________________
*/

void CP4SpecDlg::RemovePasswordFromSpec()
{
	int start= m_NewForm.Find(_T("\nPassword:"));
	int end= start;
	
	if( end > -1 )
	{
		while( ++end < m_NewForm.GetLength() && m_NewForm[end] != _T('\n') )
			;

		if( end >= m_NewForm.GetLength() )
			m_NewForm= m_NewForm.Left( start );
		else
			m_NewForm= m_NewForm.Left( start ) + m_NewForm.Mid( end );
	}
}



/*
	_________________________________________________________________

	get the text of the control.

	this code assumes that there are only edit boxes and combo boxes
	in the dialogue. 
	_________________________________________________________________
*/

CString CP4SpecDlg::GetNewValue( CWnd *pControl, int i )
{
	CString cst;

	if( pControl->IsKindOf( RUNTIME_CLASS( CP4EditBox ) ) )
	{
		pControl->GetWindowText( cst );
	}
	else if ( pControl->IsKindOf( RUNTIME_CLASS( CComboBox ) ) )
	{
		CComboBox *box = ( CComboBox * )pControl;
		int cursel = box->GetCurSel( );
		if (cursel != CB_ERR)
			 box->GetLBText(cursel, cst );
		else cst.Empty();
	}
	else if ( pControl->IsKindOf( RUNTIME_CLASS( CButton ) )
		   && m_SpecData.GetTypeOf( i ) == SDT_SELECT)
	{
		CButton *pButton = (CButton *)pControl;
		if (pButton->GetCheck())
			pControl->GetWindowText( cst );
		else
			cst = m_specControls[i].tip;
	}
	return cst;
}


/*
	_________________________________________________________________
*/

BOOL CP4SpecDlg::ShowUserError( const CString &msg, CWnd *pControl )
{
	AddToStatus( msg, SV_WARNING);
	AfxMessageBox( msg, MB_ICONEXCLAMATION );
	GotoDlgCtrl( pControl );

	return FALSE;
}


/*
	_________________________________________________________________

	this wrapper is used by p4job, etc., for displaying the data in
	the pane's list view. not all fields are displayed, especially for
	jobs, which are user-defined, so return only those things that
	the list view needs
	_________________________________________________________________
*/

void CP4SpecDlg::GetCP4Wrapper(CObject *wrapper)
{
	CString name;
	CString fullname;
	CString email;
	CString root;
	CString desc;
	CString user;
	CString status;
	CString owner;
	CString host;
	CString options;
		
	//		the date just changed, since its a new spec
	//
	CTime t = CTime::GetCurrentTime();
	CString date;
	date = t.Format( _T("%Y/%m/%d") );

	CString s;

	if( wrapper->IsKindOf( RUNTIME_CLASS( CP4Client ) ) ) 
	{
		for( int i = 0; i< m_SpecData.GetNumItems( ); i++ )
		{
			s = m_SpecData.GetTagOf( i );
			if( s == _T("Client"))
				name = m_SpecData.GetValueOf( i ) ;
			else if ( s == g_tagDescription )
				desc = m_SpecData.GetValueOf( i ) ;
			else if ( s == _T("Root") )
				root = m_SpecData.GetValueOf( i ) ;
			else if ( s == _T("Owner") )
				owner = m_SpecData.GetValueOf( i ) ;
			else if ( s == _T("Host") )
				host = m_SpecData.GetValueOf( i ) ;
			else if ( s == _T("Options") )
				options = m_SpecData.GetValueOf( i ) ;
		}
		( ( CP4Client * )wrapper )->Create( name, owner, host, date, root, desc );
	}
	else if( wrapper->IsKindOf( RUNTIME_CLASS( CP4Job ) ) )
	{
		//		jobs are special, because their fields are
		//		user-defined. So pass all values and codes.
		CStringArray values;
		CDWordArray  codes;
		for( int i = 0; i< m_SpecData.GetNumItems( ); i++ )
		{
			values.Add(m_SpecData.GetValueOf( i ));
			codes.Add(m_SpecData.GetCodeOf( i ));
		}
		( ( CP4Job * )wrapper )->Create( values, codes );

	}

	else if(wrapper->IsKindOf(RUNTIME_CLASS(CP4Branch)))
	{
		for( int i = 0; i< m_SpecData.GetNumItems( ); i++ )
		{
			s = m_SpecData.GetTagOf( i );
			if( s == _T("Branch"))
				name = m_SpecData.GetValueOf( i ) ;
			else if ( s == _T("Owner") )
				owner = m_SpecData.GetValueOf( i ) ;
			else if ( s.GetAt(0) == _T('O') && (s.Find(_T("Option")) == 0) )
			{
				if (options.GetLength() == 0)
					options = m_SpecData.GetValueOf( i ) ;
				else options += CString(_T(" ")) + m_SpecData.GetValueOf( i ) ;
			}
			else if ( s == g_tagDescription )
				desc = m_SpecData.GetValueOf( i ) ;
		}
		( ( CP4Branch * )wrapper )->Create( name, owner, options, date, desc );
	}

	else if(wrapper->IsKindOf(RUNTIME_CLASS(CP4Label)))
	{
		for( int i = 0; i< m_SpecData.GetNumItems( ); i++ )
		{
			s = m_SpecData.GetTagOf( i );
			if(  s == _T("Label") ) 
				name = m_SpecData.GetValueOf( i );
			else if ( s == _T("Owner") )
				owner = m_SpecData.GetValueOf( i ) ;
			else if ( s.GetAt(0) == _T('O') && (s.Find(_T("Option")) == 0) )
			{
				if (options.GetLength() == 0)
					options = m_SpecData.GetValueOf( i ) ;
				else options += CString(_T(" ")) + m_SpecData.GetValueOf( i ) ;
			}
			else if(  s == g_tagDescription ) 
				desc = m_SpecData.GetValueOf( i );
		}
		( ( CP4Label * )wrapper )->Create( name, owner, options, date, desc );
	}

	else if(wrapper->IsKindOf(RUNTIME_CLASS(CP4User)))
	{
		for( int i = 0; i< m_SpecData.GetNumItems( ); i++ )
		{
			s = m_SpecData.GetTagOf( i );
			if(  s == g_tagUser  ) 
				name = m_SpecData.GetValueOf( i );
			else if( s == g_tagFullName ) 
				fullname = m_SpecData.GetValueOf( i );
			else if( s == _T("Email") ) 
				email = m_SpecData.GetValueOf( i );
		}
		( ( CP4User * )wrapper )->Create( name, email, fullname, date );
	}
}

void CP4SpecDlg::OnHelpnotes() 
{
	if(m_InstructionText.GetLength())
    {
		CSpecDescDlg *dlg = new CSpecDescDlg(this->GetParent());
		dlg->SetIsModeless(TRUE);
		dlg->SetWinPosName(_T("SpecInfo"));
		dlg->SetDescription( m_InstructionText, FALSE );
		dlg->SetCaption( LoadStringResource(IDS_P4WIN_SPECIFICATION_NOTES) );
		if (!dlg->Create(IDD_SPECDESC, this->GetParent()))	// display the description dialog box
		{
			dlg->DestroyWindow();	// some error! clean up
			delete dlg;
		}
    }
}

LRESULT CP4SpecDlg::OnP4EndHelpnotes(WPARAM wParam, LPARAM lParam)
{
	CSpecDescDlg *dlg = (CSpecDescDlg *)lParam;
	dlg->DestroyWindow();
	return TRUE;
}

void CP4SpecDlg::OnEditor() 
{
	BOOL bForce = FALSE;
	BOOL bStatus = FALSE;
	BOOL need2check = FALSE;
	CString client = GET_P4REGPTR()->GetP4Client();
	CString port = GET_P4REGPTR()->GetP4Port();
	CString user = GET_P4REGPTR()->GetP4User();
	CString type;
	CString txt;
	CWnd *pWnd= m_specControls[0].control;
	pWnd->GetWindowText(txt);
	switch(m_SpecType)
	{
        case P4BRANCH_SPEC:
            type = _T("branch ");
			if (txt.IsEmpty())
			{
				CNewClientDlg newdlg;
				newdlg.SetNew( NEWBRANCH );
				if( newdlg.DoModal( ) == IDCANCEL )
					return;
				txt = newdlg.GetName( ) ;
				need2check = TRUE;
			}
            break;
		case P4CHANGE_SPEC:
		{
			CString value;
			for( int i = 0; i < m_SpecData.GetNumItems( ); i++ ) 
			{	
				CString tag = m_SpecData.GetTagOf( i );
				if (tag == g_tagStatus)
				{
					value = m_SpecData.GetValueOf ( i );
					break;
				}
			}
			bForce = value == _T("submitted") ? TRUE : FALSE;
			bStatus = GET_SERVERLEVEL() >= 10 ? TRUE : FALSE;

			type = _T("change ");
			if (txt == _T("new"))
				txt.Empty();
			if (GET_SERVERLEVEL() >= 10)
				bStatus = TRUE;
            break;
		}
        case P4CLIENT_SPEC:
            type = _T("client ");
            break;
		case P4JOB_SPEC:
            type = _T("job ");
            break;
        case P4LABEL_SPEC:
            type = _T("label ");
			if (txt.IsEmpty())
			{
				CNewClientDlg newdlg;
				newdlg.SetNew( NEWLABEL );
				if( newdlg.DoModal( ) == IDCANCEL )
					return;
				txt = newdlg.GetName( ) ;
				need2check = TRUE;
			}
            break;
        case P4USER_SPEC:
            type = _T("user ");
            break;
		default:
			ASSERT(0);
    }
	if (need2check)
	{
		BOOL bError = FALSE;
		CCmd_Describe *pCmd = new CCmd_Describe;
		pCmd->Init( NULL, RUN_SYNC, TRUE, m_pCallingCommand->GetServerKey() );
		if( pCmd->Run( m_SpecType, txt ) )
			bError = pCmd->GetError();
		delete pCmd;
		if (bError)
			return;	// the name chosen is already in use for a different type of object.
	}

	CString errorText;
	CString password= GET_P4REGPTR()->GetP4UserPassword();

	BOOL rc;
	int i = 0;
	LPCTSTR argptr[8];

	if (password.GetLength() > 0)
	{
		argptr[i++] = _T("-P");
		argptr[i++] = password;
	}
	argptr[i++] = type;
	if (bForce)
	{
		argptr[i++] = _T("-f");
	}
	if (bStatus)
	{
		argptr[i++] = _T("-s");
	}
	if (StrLen(m_pCallingCommand->GetTemplateName()))
	{
		argptr[i++] = _T("-t");
		argptr[i++] = m_pCallingCommand->GetTemplateName();
	}
	argptr[i++] = txt;
	while (i < 8)
		argptr[i++] = NULL;

	rc = TheApp()->RunApp(P4_APP, RA_WAIT, m_hWnd, FALSE, NULL, errorText, 
				    _T("-p"), port, _T("-c"), client, _T("-u"), user, 
					argptr[0], argptr[1], argptr[2], argptr[3], 
					argptr[4], argptr[5], argptr[6], argptr[7]);
	if (rc)
	{
		if (m_bIsModal)
		{
			CDialog *pParent = (CDialog *)GetParent();
			pParent->EndDialog(IDCANCEL);
		}
		else
			m_pCallingCommand->EndSpecDlg(IDCANCEL);
		::PostMessage(MainFrame()->m_hWnd, WM_COMMAND, ID_VIEW_UPDATE, 0);
	}
	else
	{
		txt.FormatMessage(IDS_SPEC_UNABLE_TO_START_P4_s_s, type, errorText);
		AfxMessageBox(txt, MB_ICONSTOP);
	}
}

BOOL CP4SpecDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if( m_SendingSpec )
		return SET_BUSYCURSOR();
	else
		return CPropertyPage::OnSetCursor(pWnd, nHitTest, message);
}


void CP4SpecDlg::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyPage::OnSize(nType, cx, cy);

	if( m_MinSize.cx > 0 )
	{
		int xDiff= cx - m_MinSize.cx;
		int yDiff= (cy - m_MinSize.cy) / max(1, m_NumMultiLineChildWnds);
		int xOffset;
		int yOffset= 0;

		// Iterate thru child windows:
		// 1) buttons slide right by xDiff
		// 2) statics can slide down but not resize
		// 3) edits and combos stretch

		int stdHeight = m_StdHeight * m_MinLi + EXTRAHEIGHT;
		int i;
		for( i=0; i < m_childControls.GetSize(); i++ )
		{
			int oldHeight;
			int xdif = m_childControls[i].IsHalfWidth() ? xDiff/2 : xDiff;
			xOffset  = m_childControls[i].IsIndent2Middle() ? xDiff/2 
					 : m_childControls[i].IsIndent2Right() ? xDiff : 0;
			CRect windowRect= m_childControls[i].GetOrigRect();

			switch( m_childControls[i].GetType() )
			{
			case CHILD_BUTTON:
				if (i == m_BrowseBtnCtrlID)	// CWnd* of Browse btn must be recalc'ed each time
					m_childControls[i].SetWindow(GetDlgItem(IDC_BROWSE));
				windowRect.OffsetRect(xOffset, yOffset);
				m_childControls[i].GetWindow()->MoveWindow( windowRect );
				break;
			case CHILD_RIGHTSTATIC:
				windowRect.OffsetRect(xDiff, 0);
				m_childControls[i].GetWindow()->MoveWindow( windowRect );
				break;
			case CHILD_MULTILINEEDIT:
			case CHILD_CHECKLISTBOX:
				oldHeight = windowRect.Height();
				windowRect.OffsetRect(xOffset, yOffset);
				windowRect.InflateRect( 0, 0, xdif, yDiff );
				if (m_childControls[i].GetMaxLines())
				{
					windowRect.bottom = windowRect.top 
									  + (m_StdHeight+4) * m_childControls[i].GetMaxLines() 
									  + EXTRAHEIGHT;
					yOffset+= windowRect.Height() - oldHeight;
				}
				else if (windowRect.Height() < stdHeight)
				{
					windowRect.bottom = windowRect.top + stdHeight;
					yOffset+= windowRect.Height() - oldHeight;
				}
				else
					yOffset+= yDiff;
				m_childControls[i].GetWindow()->MoveWindow( windowRect );
				if ( m_childControls[i].GetType() == CHILD_CHECKLISTBOX )
				{
					// Because checklistboxes are an integral height
					// we have to ask the system where the bottom
					// actually is, and adjust accordingly.
					CRect newRect;
					m_childControls[i].GetWindow()->GetWindowRect(newRect);
					ScreenToClient(newRect);
					yOffset+= newRect.Height() - windowRect.Height();
				}
				break;
			case CHILD_STATIC:
			case CHILD_CHECKBOX:
				windowRect.OffsetRect(xOffset, yOffset);
				m_childControls[i].GetWindow()->MoveWindow( windowRect );
				break;
			case CHILD_SINGLELINEEDIT:
			case CHILD_DROPLIST:
				windowRect.OffsetRect(xOffset, yOffset);
				windowRect.InflateRect( 0, 0, xdif, 0 );
				m_childControls[i].GetWindow()->MoveWindow( windowRect );
				break;
			default:
				ASSERT(0);
			}
		}

		CRect rectWindow, rectChild;
		GetWindowRect(&rectWindow);
		if (i)
		{
			m_childControls[i-1].GetWindow()->GetWindowRect(&rectChild);
			int l;
			switch(m_childControls[i-1].GetType())
			{
			case CHILD_MULTILINEEDIT:
				l = m_MinLi;
				break;
			case CHILD_CHECKLISTBOX:
				l = 4;
				break;
			default:
				l = 0;
				break;
			}
			if (l)
			{
				int h = m_StdHeight * l;
				if (rectChild.top < rectWindow.bottom - h - m_StdSpaceV)
				{
					rectChild.bottom = rectWindow.bottom - m_StdSpaceV;
					ScreenToClient(&rectChild);
					m_childControls[i-1].GetWindow()->MoveWindow( rectChild );
				}
			}
		}
		int y_delta = rectWindow.bottom - rectWindow.top - cy;
		SetScrollRange(SB_VERT, 0, 
								rectWindow.Height() <= m_MinSize.cy 
									  ? m_VscrollMax = m_MinSize.cy 
													 + y_delta - cy 
													 + (m_MinSize.cy - (cy - y_delta))/8
									  : 0, TRUE);
	}
	RedrawWindow(NULL,NULL,RDW_INVALIDATE);
}

void CP4SpecDlg::OnSizing(UINT fwSide, LPRECT pRect) 
{
	CPropertyPage::OnSizing(fwSide, pRect);
	
	pRect->right= max( pRect->right, pRect->left + m_MinSize.cx );
	pRect->bottom= max( pRect->bottom, pRect->top + MINHEIGHT );
	pRect->bottom= min( pRect->bottom, m_ScreenHeight );

	ScrollWindow(0, -m_VscrollPos, NULL, NULL);
	SetScrollPos(SB_VERT, m_VscrollPos = 0, TRUE);
}

void CP4SpecDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	m_ScreenHeight = rect.bottom - rect.top;

	lpMMI->ptMinTrackSize.y = MINHEIGHT;
	lpMMI->ptMaxTrackSize.y = m_ScreenHeight;
	CPropertyPage::OnGetMinMaxInfo(lpMMI);
}

BOOL CP4SpecDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	BOOL rc = CPropertyPage::OnCommand(wParam, lParam);
	if (!m_ChangesHaveBeenMade && m_WindowShown)
	{
		if ((HIWORD(wParam) == EN_UPDATE)
		 || (HIWORD(wParam) == CBN_SELCHANGE))
		{
			CDialog *pParent = (CDialog *)GetParent();
			pParent->GetDlgItem(ID_EDITOR)->EnableWindow( FALSE );
			m_EditorBtnDisabled = m_ChangesHaveBeenMade = TRUE;
		}
	}
	return rc;
}

void CP4SpecDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
	
	OnSize(SIZE_RESTORED, m_MinSize.cx, m_MinSize.cy);
	int i;
	if ((i = m_childControls.GetSize()) > 0)
	{
		CRect rectWindow, rectChild;
		GetWindowRect(&rectWindow);
		m_childControls[i-1].GetWindow()->GetWindowRect(&rectChild);
		if (rectChild.bottom + m_StdSpaceV >= rectWindow.bottom)
			m_MinSize.cy = max(MINHEIGHT, rectChild.bottom - rectWindow.top + m_StdSpaceV);
	}

	m_WinPos.SetMinSize( m_MinSize );
	if (!m_AutomaticallyUpdate)
		m_WinPos.RestoreWindowPosition( );

	m_WindowShown = TRUE;
}

void CP4SpecDlg::OnHelp() 
{
	DWORD helpID=0;

	switch(m_SpecType)
	{
        case P4BRANCH_SPEC:
            helpID=TASK_MANAGING_BRANCH_SPECIFICATIONS;
            break;
		case P4CHANGE_SPEC:
			helpID=m_AllowSubmit ? ALIAS_20_SUBMIT : TASK_WORKING_WITH_CHANGELISTS;
            break;
        case P4CLIENT_SPEC:
            helpID=TASK_CREATING_CLIENTS;
            break;
		case P4JOB_SPEC:
            helpID=TASK_MANAGING_JOBS;
            break;
        case P4LABEL_SPEC:
            helpID=TASK_MANAGING_LABELS;
            break;
        case P4USER_SPEC:
            helpID=TASK_MANAGING_USERS;
            break;
		default:
			ASSERT(0);
    }
	AfxGetApp()->WinHelp(helpID);
}

BOOL CP4SpecDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	OnHelp();
	return TRUE;
}

void CP4SpecDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int nVscrollInc;

	switch (nSBCode)
	{
	case SB_TOP:
		nVscrollInc = -m_VscrollPos;
		break;

	case SB_BOTTOM:
		nVscrollInc = m_VscrollMax - m_VscrollPos;
		break;

	case SB_LINEUP:
		nVscrollInc = -10;
		break;

	case SB_LINEDOWN:
		nVscrollInc = 10;
		break;

	case SB_PAGEUP:
		nVscrollInc = min(-1, -100);
		break;

	case SB_PAGEDOWN:
		nVscrollInc = max(1, 100);
		break;

	case SB_THUMBTRACK:
		nVscrollInc = nPos - m_VscrollPos;
		break;

	default:
		nVscrollInc = 0;
	}

	if ((nVscrollInc = max(-m_VscrollPos, min(nVscrollInc, m_VscrollMax - m_VscrollPos))) != 0)
	{
		m_VscrollPos += nVscrollInc;
		ScrollWindow(0, -nVscrollInc, NULL, NULL);
		SetScrollPos(SB_VERT, m_VscrollPos, TRUE);
	}

	CPropertyPage::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CP4SpecDlg::OnCancelButton()
{
	if (m_SendingSpec 
	 && IDYES == AfxMessageBox(IDS_CANCEL_AREYOUSURE, 
									MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2)
	 && SERVER_BUSY())
		global_cancel = 1;
}

void CP4SpecDlg::OnBrowse()
{
	UpdateData();
	CString txt;
	txt.FormatMessage(IDS_CHOOSE_FOLDER_FOR, m_BrowseTag);
	CString path;
	m_childControls[m_BrowseFldCtrlID].GetWindow()->GetWindowText(path);
	path = TheApp()->BrowseForFolder(m_hWnd, path, txt, BIF_NEWDIALOGSTYLE);
	if (!path.IsEmpty())
	{
		m_childControls[m_BrowseFldCtrlID].GetWindow()->SetWindowText(path);
		UpdateData(FALSE);
	}
}

void CP4SpecDlg::ResetFileChecks(BOOL bCheck)
{
	ASSERT(m_pLastFilesList);
	if (!m_pLastFilesList)
	{
		AfxMessageBox(_T("Change spec appears to be badly damaged!"), MB_OK|MB_ICONERROR);
		return;
	}

	// if they want to move the unchanged files to the default changelist (i.e. "Leave"),
	//	we must Uncheck the unchanged files in the selection set
	// otherwise
	//	we must Check the unchanged files in the selection set
	// Checks for all other files are not touched
	int cnt = m_pLastFilesList->GetCount( );
	for ( int i = -1; ++i < cnt;  )
	{
		CString s;
		m_pLastFilesList->GetText(i, s);

		if (!m_pDeltaView->GetTreeCtrl().IsAMemeberOfSelectionList(s) || IsFileChanged(s))
			continue;

		if (m_UnchangedFlag == 2)	// leave unchanged files?
			m_pLastFilesList->SetCheck( i, 0 );
		else
			m_pLastFilesList->SetCheck( i, 1 );
	}
}
