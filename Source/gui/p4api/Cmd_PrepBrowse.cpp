//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_PrepBrowse.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_prepbrowse.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_PrepBrowse, CP4Command)


CCmd_PrepBrowse::CCmd_PrepBrowse(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4PREPBROWSE;
	m_TaskName= _T("PrepBrowse");
	m_pOutputFile=NULL;
	m_Annotating = m_bP4a = m_NoFileAtRev = FALSE;
	m_FileRev = -1;
	m_Type = FST_CANTTELL;
}

CCmd_PrepBrowse::~CCmd_PrepBrowse()
{
	if(m_pOutputFile != NULL)
		delete m_pOutputFile;
}

// Normal p4 print of file
BOOL CCmd_PrepBrowse::Run(LPCTSTR fileSpec, CString &fileType, 
						  long fileRev, BOOL bForce2Binary)
{
	ASSERT(fileSpec != NULL);
	
	if(!SetupPrint(fileSpec, fileType, fileRev, bForce2Binary))
		return FALSE;			 
	
	// Assign properties to keep track of temp file retrieval
	m_ByteCount=0;
	
	// Set the arg list
	ClearArgs();
	AddArg(_T("print"));
	if (GET_SERVERLEVEL() >= 10)
	{
		AddArg(_T("-o"));
		AddArg(m_TempName);
	}
	AddArg(_T("-q"));
	CString DepotName;
	if (fileRev < 0)
		DepotName = fileSpec;
	else
		DepotName.Format(_T("%s#%d"), fileSpec, fileRev);
	AddArg(DepotName);

	return CP4Command::Run();
}

// p4 annotate
BOOL CCmd_PrepBrowse::Run(BOOL bUseP4A, LPCTSTR fileSpec, CString &fileType, 
		 BOOL bAll/*=FALSE*/, BOOL bChg/*=FALSE*/, BOOL bNoHead/*=FALSE*/, long fileRev/*=-1*/, 
		 int whtSp/*=0*/, BOOL bIncInteg/*=FALSE*/)
{
	if (GET_SERVERLEVEL() < 14)
		return FALSE;

	ASSERT(fileSpec != NULL);

	m_Annotating = TRUE;
	m_bP4a = bUseP4A;

	if(!SetupPrint(fileSpec, fileType, fileRev, TRUE))
		return FALSE;			 
	
	// Assign properties to keep track of temp file retrieval
	m_ByteCount=0;
	
	// Set the arg list
	ClearArgs();
	AddArg(_T("annotate"));
	if (bAll)
		AddArg(_T("-a"));
	if (bChg)
		AddArg(_T("-c"));
	if (bNoHead)
		AddArg(_T("-q"));
	CString DepotName;
	m_FileRev = fileRev;
	if (fileRev < 0 || m_bP4a)
		DepotName = fileSpec;
	else
		DepotName.Format(_T("%s#%d"), fileSpec, fileRev);
	if (GET_SERVERLEVEL() >= 20)			// 2005.2 or later?
	{
		switch(whtSp)
		{
		case 1:
			AddArg(_T("-db"));
			break;
		case 2:
			AddArg(_T("-dw"));
			break;
		case 0:
		default:
			break;
		}
		if (bIncInteg)
			AddArg(_T("-i"));
	}
	AddArg(DepotName);

	return CP4Command::Run();
}


void CCmd_PrepBrowse::OnOutputText(LPCTSTR data, int length)
{
	// if server is 2000.2 or greater, 
	// -o will take care of everything,
	// so just return unless we are annotating
	if (GET_SERVERLEVEL() >= 10 && !m_Annotating)
		return;

	ASSERT(lstrlen(data) == length);

	CString temp;
	StrBuf sptr;
	Error e;

	if(length > 0)
	{
		// TODO: handle error
		CString line;
		m_ByteCount+= length;
		if (m_Annotating)
		{
			line = data;
			line.TrimRight();
			line += _T("\r\n");
			data = line.GetBuffer();
			length = lstrlen(data);
		}
		sptr.Set(const_cast<char*>((const char*)CharFromCString(data)));
		m_pOutputFile->Write( &sptr, &e);
		if(e.Test())
			return;
	}
		
	if(length == 4096)
	{
		// Update the status text
		//::PostMessage(m_ReplyWnd, WM_P4STATUS, P4_PREPBROWSE, (LPARAM) m_KByteCount());
	}

	if(length == 0)
	{
		// Clear the status box
		//::PostMessage(m_pP4->GetReplyWnd(), WM_P4STATUS, 0, 0);
		if(m_ByteCount > 9999)
			temp.Format(_T("Retrieved %s, %ld KB"), CharToCString(m_pOutputFile->Name()), m_ByteCount/1024);		
		else
			temp.Format(_T("Retrieved %s, %ld bytes"), CharToCString(m_pOutputFile->Name()), m_ByteCount);													


		TheApp()->StatusAdd(temp); 
		// Dont delete the file before the viewer can see it!
		m_pOutputFile->ClearDeleteOnClose();
		m_pOutputFile->Close(&e);
	}
}


// Internal function to set up args for printing a depot file to a temp file
// Used by View() and Diff2()
BOOL CCmd_PrepBrowse::SetupPrint(LPCTSTR fileSpec, CString &fileType, 
								 long fileRev, BOOL bForce2Binary)
{
	// Get the file suffix as "filename.ext"
	CString FileName(fileSpec);
	CString Symbol=_T("");

	int i;
	if ((i = FileName.Find(_T('@'))) != -1)
	{
		Symbol = FileName.Mid(i+1);
		FileName = FileName.Left(i);
		int len = Symbol.GetLength();
		for (i=-1; ++i < len; )
		{
			TCHAR c = Symbol.GetAt(i);
			if (!_istalpha(c) && !_istalnum(c))
				Symbol.SetAt(i, _T('_'));
		}
	}
	else if (fileRev == 0x80000000)
		Symbol = _T("Head-Rev");

	int begName=0;
	for(i = FileName.GetLength()-1; i>0; i--)
	{
		if(FileName[i]==_T('/') || FileName[i]==_T('\\'))
		{
			begName=i+1;
			break;
		}
	}
	ASSERT(i);
	if(i==0)
	{
		m_ErrorTxt.Format(_T("Invalid filespec:\n %s"), FileName);
		TheApp()->StatusAdd(m_ErrorTxt, SV_ERROR);
		m_FatalError=TRUE;
		return FALSE;
	}
	FileName=FileName.Mid(begName);
	
	// Check the file type 
	int fType;
	int plus;
	
	if ((fileType.Find(_T("text")) != -1) && !bForce2Binary)
	{
		if ((plus = fileType.Find(_T("+"))) != -1)
		{
			if (fileType.Find(_T("x"), plus) == -1)
				fType = FST_TEXT;
			else
				fType = FST_XTEXT;
		}
		else
		{
			if ((fileType == _T("text"))  || (fileType == _T("ktext"))
			 || (fileType == _T("ltext")) || (fileType == _T("ctext")))
				fType = FST_TEXT;
			else if ((fileType == _T("xtext"))  || (fileType == _T("kxtext")) || (fileType == _T("cxtext")))
				fType = FST_XTEXT;
			else
				fType=FST_BINARY;
		}
	}
	else if ((fileType.Find(_T("unicode")) != -1) && !bForce2Binary)
	{
		if (fileType.Find(_T("x")) != -1)
			fType=FST_XUNICODE;
		else
			fType=FST_UNICODE;
	}
	else if ((fileType.Find(_T("utf16")) != -1) && !bForce2Binary)
	{
		fType=FST_UTF16;
	}
	else
	{
		if ((fileType.Find(_T("binary")) != -1) && (fileType.Find(_T("x")) != -1))
			fType=FST_XBINARY;
		else if (fileType == _T("xtempobj"))
			fType=FST_XBINARY;
		else
			fType=FST_BINARY;
	}
	
	m_pOutputFile= FileSys::Create( (enum FileSysType) fType );

	CString TempPath= GET_P4REGPTR()->GetTempDir();
	Error e;

	static int ctr = 0;
	for(i=ctr; ++i != ctr; )
	{
		if (i >= 100)
		{
			i = 0;
			if (i == ctr)
				break;
		}
		e.Clear();
		if (Symbol.IsEmpty())
			m_TempName.Format(_T("%s\\ReadOnly-%d-Rev-%d-%s"), TempPath, i, fileRev, FileName);
		else
			m_TempName.Format(_T("%s\\ReadOnly-%d-%s-%s"), TempPath, i, Symbol, FileName);
		int j;
		while ((j = m_TempName.Find(':', 2)) != -1)
			m_TempName.SetAt(j, '_');
		while ((j = m_TempName.FindOneOf(_T("/*?\"<>|"))) != -1)
			m_TempName.SetAt(j, '_');
		m_pOutputFile->Set(CharFromCString(m_TempName));
		
		if( !e.Test() )
			// Prepare write (makes dir as required)
			m_pOutputFile->MkDir( &e );
		if( !e.Test() )
		{
			// Open it
			m_pOutputFile->Perms( m_Annotating && m_bP4a ? FPM_RW : FPM_RO );
			m_pOutputFile->Open( FOM_WRITE, &e );
		}
		if(!e.Test())
			break;
	}
	ctr = i;
	if(e.Test())
	{
		m_ErrorTxt.Format(_T("Error opening temporary file:\n %s"), m_TempName);
		TheApp()->StatusAdd(m_ErrorTxt, SV_ERROR);
		m_FatalError=TRUE;

		delete m_pOutputFile;
		m_pOutputFile=NULL;
		return FALSE;
	}

	// 2000.2 server will use print -o, 
	// so we no longer need the file now that we know it can be created
	// (unless we are annotating)
	if (GET_SERVERLEVEL() >= 10 && !m_Annotating)
		m_pOutputFile->Close(&e);

	return TRUE;
}

void CCmd_PrepBrowse::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	if (m_Annotating && (CString(data)).Find(_T(" change")) > 0)
	{
		if (m_bP4a)
			OnOutputText(data, lstrlen(data));
		else
			TheApp()->StatusAdd(msg);
	}
	else
	{
		if(APP_ABORTING() && m_Asynchronous)
		{
			ReleaseServerLock();
			ExitThread(0);
		}

		TheApp()->StatusAdd(msg, SV_WARNING);
	}
}

BOOL CCmd_PrepBrowse::HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg )
{
	if ( StrStr(errBuf, _T(" - no file(s) at that revision")) 
	 ||	 StrStr(errBuf, _T(" - no such file(s)")) )
		m_NoFileAtRev = TRUE ; 
	return ( FALSE );
}
