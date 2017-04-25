// P4EditBox.cpp: implementation of the CP4EditBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "p4win.h"
#include "MainFrm.h"
#include "P4EditBox.h"
#include "P4SpecDlg.h"

#define P4BRANCH_SPEC	2
#define P4CHANGE_SPEC	3
#define P4CLIENT_SPEC	4
#define P4DEPOT_SPEC	5
#define P4JOB_SPEC		6
#define P4LABEL_SPEC	7
#define P4PROTECT_SPEC	8
#define P4USER_SPEC		9

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CP4EditBox, CEdit)
	ON_WM_CHAR()
	ON_MESSAGE(WM_DROPTARGET, OnDropTarget)
END_MESSAGE_MAP()

CP4EditBox::CP4EditBox(CWnd* pParent)
{
	m_Parent = pParent;
	m_pDropTgt = 0;
}

CP4EditBox::~CP4EditBox()
{
	if (m_pDropTgt)
	{
		m_pDropTgt->Revoke();
		delete m_pDropTgt;
	}
}

void CP4EditBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == 0x0A)	// 0x0A == Ctrl+VK_RETURN, but there is no #define
		((CP4SpecDlg *)m_Parent)->OnEnter();
	else
		CEdit::OnChar(nChar, nRepCnt, nFlags);
}

LRESULT CP4EditBox::OnDropTarget(WPARAM wParam, LPARAM lParam)
{
//	m_DropTargetPt.x = LOWORD(lParam);
//	m_DropTargetPt.y = HIWORD(lParam);
	CStringList *filelist = (CStringList *)wParam;
	CString txt;
	CString filename;
	CString localname;
	BOOL b = GET_P4REGPTR()->UseDepotInClientView();
	int i;

	GetWindowText(txt);
	txt.TrimRight();
	if (txt.Find(_T("\r\n\r\n")) == -1)
		txt += _T("\r\n\r\n");
	else
		txt += _T("\r\n");

	for(POSITION pos = filelist->GetHeadPosition(); pos != NULL; )
	{
		filename = filelist->GetNext( pos );
		switch(m_SpecType)
		{
		case 0:		// Single filename only
			SetWindowText(filename);
			return 0;

		case P4LABEL_SPEC:
		case P4USER_SPEC:
			if (filename.Find(_T(' ')) != -1)
				filename = _T('\"') + filename + _T('\"');
			break;

		case P4CLIENT_SPEC:
			if ((i = filename.Find(_T("<contains no files or folders>"))) != -1)
				filename = filename.Left(i-1) + _T("/...");
			localname = CString ( _T("//") ) + GET_P4REGPTR()->GetP4Client();
			if (!b && ((i = filename.Find(_T('/'), 2)) > 2))
				localname += filename.Right(filename.GetLength() - i);
			else
				localname += filename.Right(filename.GetLength() - 1);
			if (filename.Find(_T(' ')))
				filename = _T('\"') + filename + _T('\"');
			if (localname.Find(_T(' ')))
				localname = _T('\"') + localname + _T('\"');
			filename = filename + _T(' ') + localname;
			break;

		default:
			ASSERT(0);
			return 0;
		}
		txt += filename + _T("\r\n");
	}
	SetWindowText(txt);

	// Set caret at end of view/review and scroll into view.
	int s, e;
	SetSel(0, -1);
	GetSel(s, e);
	SetSel(e, e, TRUE);
	return 0;
}

/////////////////////////////////////////////////////////////////////
// OLE drag-drop support, to accept depot files or folders
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CEBDropTarget

CEBDropTarget::CEBDropTarget()
{
    m_CF_DEPOT = static_cast<CLIPFORMAT>(RegisterClipboardFormat(LoadStringResource(IDS_DRAGFROMDEPOT)));
}

CEBDropTarget::~CEBDropTarget() {}  

// OnDragEnter is called by OLE dll's when drag cursor enters
// a window that is REGISTERed with the OLE dll's
DROPEFFECT CEBDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* 
                           pDataObject, DWORD dwKeyState, CPoint point )
{
	m_DropEffect=DROPEFFECT_NONE;
	m_DragDataFormat=0;

	// Dont allow a drop if the server is busy, since a drop immediately attempts to
	// invoke a server command
	// Also don't allow a drop if we are in local syntax because
	// the reviews should be specified in depot syntax
	if(SERVER_BUSY() || GET_P4REGPTR( )->ShowEntireDepot( ) > SDF_DEPOT)
		return DROPEFFECT_NONE;
		
	if(pDataObject->IsDataAvailable(m_CF_DEPOT))
	{
		m_DropEffect=DROPEFFECT_COPY;
		m_DragDataFormat=m_CF_DEPOT;
	}
	
	return m_DropEffect;
} 

// OnDragLeave is called by OLE dll's when drag cursor leaves
// a window that is REGISTERed with the OLE dll's
void CEBDropTarget::OnDragLeave(CWnd* pWnd)
{
    // Call base class implementation
    COleDropTarget:: OnDragLeave(pWnd);
}
 
// OnDragOver is called by OLE dll's when cursor is dragged over 
// a window that is REGISTERed with the OLE dll's
DROPEFFECT CEBDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* 
           pDataObject, DWORD dwKeyState, CPoint point )
{
	// Dont allow a drop if the server is busy, since a drop immediately attempts to
	// invoke a server command
	if(SERVER_BUSY())
		m_DropEffect= DROPEFFECT_NONE;
		
	return m_DropEffect;
}

// OnDrop is called by OLE dll's when item is dropped in a window
// that is REGISTERed with the OLE dll's
BOOL CEBDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, 
                 DROPEFFECT dropEffect, CPoint point )
{ 
	if(SERVER_BUSY())
	{
		// OnDragEnter() and OnDragOver() should avoid a drop at 
		// the wrong time!
		ASSERT(0);
		return FALSE;
	}
	
	if(m_DragDataFormat == m_CF_DEPOT)
	{
		::SendMessage(MainFrame()->GetDepotWnd(), WM_GETSELLIST, (WPARAM) &m_FileList, 0);
	    if(m_FileList.GetCount() > 0)
			m_Owner->SendMessage(WM_DROPTARGET, (WPARAM)&m_FileList, (LPARAM)&point);
		return TRUE;
	}
	// Return false, so depot window doesnt start a file-open operation
	return FALSE;
}
