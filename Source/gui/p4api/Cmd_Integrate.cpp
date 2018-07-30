//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Integrate.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_integrate.h"
#include "cmd_delete.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_Integrate, CP4Command)


CCmd_Integrate::CCmd_Integrate(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4INTEGRATE;
	m_TaskName= _T("Integrate");
	m_ClobberFailed = m_IsRename = m_DeleteChg = FALSE;
	m_ChangeNbr = 0;
}

CCmd_Integrate::~CCmd_Integrate()
{
	Error e;
	if (m_DeleteChg || (m_FatalError && m_IsRename && m_ChangeNbr))
	{
		CString txt;
		txt.Format(_T("%ld"), m_ChangeNbr);
		CCmd_Delete cmd1(m_pClient);
		cmd1.Init(NULL, RUN_SYNC);
		cmd1.SetIgnoreActiveItem(TRUE);
		cmd1.Run( P4CHANGE_DEL, txt );
		cmd1.CloseConn(&e);
	}
}

BOOL CCmd_Integrate::Run(BOOL reverse, LPCTSTR branchName)
{
	m_Reverse= reverse;

	ClearArgs();
	AddArg(_T("integ"));
	if (GET_SERVERLEVEL() >= 18)	// 2004.1 server or later?
		AddArg(_T("-o"));
	
	if(m_Reverse)
		AddArg(_T("-r"));
	
	if(branchName == NULL)
	{
		ASSERT(0);
		return FALSE;
	}
	
	AddArg(_T("-b"));
	AddArg(branchName);

	m_MultipleItems=FALSE;
	m_IsBranch= TRUE;
	m_IsPreview= FALSE;
	
	return CP4Command::Run();
}

BOOL CCmd_Integrate::Run( CStringList *source, CStringList *target,
				LPCTSTR reference, 
				LPCTSTR revRange,
				LPCTSTR commonPath,
				BOOL isBranch, BOOL isReverse, BOOL isNoCopy,
				BOOL isForced, BOOL isForcedDirect, BOOL isRename, BOOL isPreview,
				int changeNum, BOOL isPermitDelReadd, int delReaddType, 
				BOOL isBaselessMerge,  BOOL isIndirectMerge, 
				BOOL isPropagateTypes, BOOL isBaseViaDelReadd, 
				int branchFlag, BOOL bDontSync, BOOL bBiDir )
{
	ASSERT( source != NULL && source->IsKindOf( RUNTIME_CLASS(CStringList) ) );
	ASSERT( target != NULL && target->IsKindOf( RUNTIME_CLASS(CStringList) ) );
	if (!isBranch)
	{
		ASSERT( source->GetCount() == 1 || target->GetCount() == 1 );
		ASSERT( source->GetCount() > 0 );
		ASSERT( target->GetCount() > 0 );
	}
	else
		ASSERT( source->GetCount() <= 1 || target->GetCount() <= 1 );

	// With pre-99.1 server, can only spec source if not a branch
	if( GET_SERVERLEVEL() < LEVEL_NEWINTEG )
		ASSERT( !isBranch || ( source->GetCount() == 1 && source->GetHead() == _T("//...") ) );

	if( GET_SERVERLEVEL() >= LEVEL_NEWINTEG 
		&& isBranch && (branchFlag == 1) 
		&& source->GetCount() > 0 )
		m_NewBranchSyntax= TRUE;
	else
		m_NewBranchSyntax= FALSE;
		
	ClearArgs();
	m_BaseArgs= AddArg(_T("integ"));
	if (GET_SERVERLEVEL() >= 18)	// 2004.1 server or later?
		AddArg(_T("-o"));
	if(changeNum > 0)
	{
		AddArg(_T("-c"));
		m_BaseArgs= AddArg(m_ChangeNbr = changeNum);
	}
	if(isReverse)
	{
		ASSERT(!m_NewBranchSyntax);
		m_BaseArgs= AddArg(_T("-r"));
	}
	if(isNoCopy)
		m_BaseArgs= AddArg(_T("-v"));
	if(isPermitDelReadd && ( GET_SERVERLEVEL() > 7 ))
	{
		if (delReaddType != 2 && GET_SERVERLEVEL() > 16)
			m_BaseArgs= AddArg(delReaddType == 1 ? _T("-Dt") : _T("-Ds"));
		else
			m_BaseArgs= AddArg(_T("-d"));
	}
	if(isIndirectMerge && ( GET_SERVERLEVEL() > 14 ))
		m_BaseArgs= AddArg(_T("-I"));
	else if(isBaselessMerge && ( GET_SERVERLEVEL() > 7 ))
		m_BaseArgs= AddArg(_T("-i"));
	if(isPropagateTypes && ( GET_SERVERLEVEL() > 9 ))
		m_BaseArgs= AddArg(_T("-t"));
	if(isBaseViaDelReadd && ( GET_SERVERLEVEL() > 17 ))
		m_BaseArgs= AddArg(_T("-Di"));
	if(isForced)
		m_BaseArgs= AddArg(_T("-f"));
	if(isForcedDirect&& ( GET_SERVERLEVEL() > 17 ))
		m_BaseArgs= AddArg(_T("-1"));
	if(isPreview)
		m_BaseArgs= AddArg(_T("-n"));
	if(bDontSync && ( GET_SERVERLEVEL() >= 13 ))
		m_BaseArgs= AddArg(_T("-h"));

	m_IsBranch= isBranch;
	m_Reference= reference;
	m_IsPreview= isPreview;
	m_IsRename = isRename;
	m_RevRange= revRange;
	m_CommonPath = commonPath;
	m_BiDir = bBiDir;

	if(isBranch)
	{
		AddArg(_T("-b"));
		m_BaseArgs= AddArg(reference);
	}
	else
	{
		m_Reference= reference;
	}

	if( m_NewBranchSyntax || m_BiDir )
		m_BaseArgs= AddArg(_T("-s"));

	// Initialize last message time
	m_LastMessage= GetTickCount();
	m_pStatusArray= new CStringArray();
	
	if( !source->GetCount() && !m_NewBranchSyntax)
	{
		// We are integrating from a branch
		if ( target->GetCount() == 1 )
		{
			CString tgt = target->GetHead();
			tgt += m_RevRange;
			AddArg( tgt );
		}
		else if ( target->GetCount() > 1 )
		{
			// I can't figure out any way to make this code execute
			// But if it does, it'll work fine - unless a rev range is given
			// Unfortunately, the rev range will be ignored
			ASSERT(m_RevRange.IsEmpty());
			m_SourceProvided= FALSE;
			m_posStrListIn= target->GetHeadPosition();
			m_pStrListIn= target;  
			m_MultipleItems= (m_pStrListIn->GetCount() > 1);
		
			// Put the first few files into the arg list
			NextListArgs();
		}
		else if (!m_RevRange.IsEmpty())
			AddArg( m_RevRange );
		return CP4Command::Run();
	}
	else if( ( source->GetCount() == 1 && source->GetHead() == _T("//...") ) &&
			 ( target->GetCount() == 1 && target->GetHead() == _T("//...") ) )
	{
		// If we are integrating an entire branch, there are no list args
		CString src= source->GetHead();
		src+= m_RevRange;
		AddArg( src );
		AddArg( target->GetHead() );
		return CP4Command::Run();
	}
	else if ( source->GetCount() == 1 && source->GetHead() == _T("//...") 
		   && target->GetCount() == 0 && !m_RevRange.IsEmpty() )
	{
		// If we are integrating an entire branch, there are no list args
		CString src= m_RevRange;
		AddArg( src );
		return CP4Command::Run();
	}
	else
	{
		// Set the arg list tracking info
		if( source->GetHead() != _T("//...") )
		{
			m_SourceProvided= TRUE;
			m_posStrListIn= source->GetHeadPosition();
			m_pStrListIn= source;  
		}
		else
		{
			m_SourceProvided= FALSE;
			m_posStrListIn= target->GetHeadPosition();
			m_pStrListIn= target;  
		}
		m_MultipleItems= (m_pStrListIn->GetCount() > 1);
	
		// Put the first few files into the arg list
		NextListArgs();
	
		return CP4Command::Run();
	}
}

BOOL CCmd_Integrate::NextListArgs()
{
    ASSERT(m_BaseArgs <= GetArgc());

	ClearArgs(m_BaseArgs);  // Clear all but the base args
	if(m_NewBranchSyntax && m_SourceProvided)
	{
		// Add the source side, with revision range
		CString src=m_pStrListIn->GetNext(m_posStrListIn);
		src+= m_RevRange;
		AddArg(src);

		// Add the target side
		AddArg(_T("//..."));
	}
	else if(m_NewBranchSyntax)
	{
		// Add the source side, with revision range
		CString src= _T("//...");
		src+= m_RevRange;
		AddArg( src );

		// Add the target side
		if (m_pStrListIn->GetCount() > 0)
		{
			CString targ=m_pStrListIn->GetNext(m_posStrListIn);
			AddArg( targ );
		}
	}
	else if(m_IsBranch)
	{
		// Add the target, with revision range
		if (m_pStrListIn->GetCount() > 0)
		{
			CString targ=m_pStrListIn->GetNext(m_posStrListIn);
			targ+= m_RevRange;
			AddArg(targ);
		}
	}
	else
	{
		// Pull a single arg off the list
		if( m_posStrListIn != NULL )	
		{
			// Add the source side
			CString src=m_pStrListIn->GetNext(m_posStrListIn);
			AddArg(src + m_RevRange);

			// Make a dest side
			CString dest=m_Reference;

			// figure out the slash char (src) and the wildcard string (dest)
			TCHAR   slashChr = GET_P4REGPTR( )->ShowEntireDepot( ) > SDF_DEPOT ? _T('\\') : _T('/');
			TCHAR   wildChr  = slashChr;
			if (src.Find(slashChr) == -1)
			{
				if (src.Find(_T('/')) != -1)
					slashChr = _T('/');
				else if (src.Find(_T('\\')) != -1)
					slashChr = _T('\\');
			}
			if (dest.Find(wildChr) == -1)
			{
				if (dest.Find(_T('/')) != -1)
					wildChr = _T('/');
				else if (dest.Find(_T('\\')) != -1)
					wildChr = _T('\\');
			}
			ASSERT(slashChr == wildChr);
			CString wildcard = CString(wildChr) + _T("...");

			int lastpart;
			int wildDest= dest.Find(wildcard);
			int wildSrc= src.Find(wildcard);

			if(wildSrc != -1 && wildDest != -1 && m_MultipleItems)
			{
				/////////////////////
				// the source directory will be created as a subdir under dest

				// 1) cut off wildcards
				src= src.Left(wildSrc);
				dest= dest.Left(wildDest);
				// 2) find what comes after the common path in src
				if ((FindNoCase(src, m_CommonPath) == 0)
				 && (src.GetLength() == m_CommonPath.GetLength() 
				  || src.GetAt(m_CommonPath.GetLength()) == slashChr))
					 lastpart= m_CommonPath.GetLength();
				else lastpart= src.ReverseFind(slashChr);
				// 3) add subdirname to dest
				dest+= src.Mid(lastpart);
				// 4) put wildcard back on
				dest+= wildcard;
			}
			else if(wildSrc != -1 && wildDest != -1)
			{
				/////////////////////
				// the source directory is the only selected item, so its files
				// are integed directly to dest dir, without creating a subdir
			
				// Nothing to do, since wildcards already match
				
			}
			else if(wildSrc == -1 && wildDest != -1)
			{
				/////////////////////
				// the source item is a file (no wildcard) but the dest
				// item has a wildcard
				
				// 1) cut off the dest wildcard
				dest= dest.Left(wildDest);
				// 2) find what comes after the common path in src
				if ((FindNoCase(src, m_CommonPath) == 0)
				 && (src.GetLength() == m_CommonPath.GetLength() 
				  || src.GetAt(m_CommonPath.GetLength()) == slashChr))
					 lastpart= m_CommonPath.GetLength();
				else lastpart= src.ReverseFind(slashChr);
				// 3) add filename to dest
				dest+= src.Mid(lastpart);
			}
			else if(wildSrc == -1 && wildDest == -1)
			{
				/////////////////////
				// both src and dest are discreet files

				// Nothing to do, since wildcards already match
			}
			else
			{
				/////////////////////
				// We should have already addressed all legal possibilities for
				// wildcard levels.  We were given bad args!
				ASSERT(0);
			}
			
			// Make the target arg
			AddArg(dest);
		}
	}

	// Caller knows not to call again when the list is empty
	if(m_posStrListIn == NULL)
		m_pStrListIn->RemoveAll();
	return FALSE;
}


void CCmd_Integrate::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	BOOL	bDoStatusAdd = TRUE;

    CString msgD = data;
	CString msgM = msg;

	if (StrStr(data, _T(" - can't ")))
	{
		if ( StrStr(data, _T("can't integrate")) 
          || StrStr(data, _T("can't branch"))
          || StrStr(data, _T("can't delete")) )
		{
			LPCTSTR i;

			if ((i = StrStr(data, _T(" without -i flag"))) != 0)
			{
				msgD = CString(data, i - data) + LoadStringResource(IDS_ENABLE_BASELESS_MERGES);
				if (msg != data)
					msgM += LoadStringResource(IDS_ENABLE_BASELESS_MERGES);
			}
			else if ((i = StrStr(data, _T(" without -d flag"))) != 0)
			{
				msgD = CString(data, i - data) + LoadStringResource(IDS_ENABLE_DELETE_READD);
				if (msg != data)
					msgM += LoadStringResource(IDS_ENABLE_DELETE_READD);
			}
		}
		bDoStatusAdd = FALSE;
	}
	if (msg == data)
		msgM = msgD;
	
	if (bDoStatusAdd)
	{
		CString txt;
		if(m_IsPreview)
			txt = LoadStringResource(IDS_INTEGRATE_PREVIEW);

		txt += msgM;

		// Dont go compute bound by passing one message per row
		if (m_ClobberFailed)
			txt= LoadStringResource(IDS_FAILED) + txt;
		m_pStatusArray->Add(txt);
		if( GetTickCount() - m_LastMessage > 250)
		{
			TheApp()->StatusAdd( m_pStatusArray);
			m_pStatusArray= new CStringArray;
			m_LastMessage= GetTickCount();
		}
	}
	if (m_ClobberFailed)
		m_ClobberFailed = FALSE;
	else
		m_List.AddHead(msgD);
}

void CCmd_Integrate::PostProcess()
{
	if( m_pStatusArray->GetSize() )
		TheApp()->StatusAdd( m_pStatusArray);
	else
		delete m_pStatusArray;
}

BOOL CCmd_Integrate::HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg)
{
	BOOL handledError=FALSE;
	if (StrStr(errBuf, _T("Can't clobber writable file")) == errBuf)
	{
		if (m_pStatusArray->GetSize() > 0)
		{
			TheApp()->StatusAdd( m_pStatusArray);
			m_pStatusArray= new CStringArray;
			m_LastMessage= GetTickCount();
		}
		m_ClobberFailed = TRUE;
	}
	else if (StrStr(errBuf, 
		_T("The filename, directory name, or volume label syntax is incorrect")) != NULL)
	{
		if (m_pStatusArray->GetSize() > 0)
		{
			TheApp()->StatusAdd( m_pStatusArray);
			m_pStatusArray= new CStringArray;
			m_LastMessage= GetTickCount();
		}
		m_ClobberFailed = TRUE;
	}

	if (m_IsRename && m_ChangeNbr)
		m_DeleteChg = TRUE;
	
	return handledError;
}

void CCmd_Integrate::OnOutputError(char level, LPCTSTR errBuf, LPCTSTR errMsg)
{
	CP4Command::OnOutputError(level, errBuf, errMsg);
	if (m_FatalError)
	{
		if (level < 0x50)
		{
			m_FatalError = FALSE;
			m_FatalErrorCleared = TRUE;
		}
		else
			delete m_pStatusArray;
	}
}
