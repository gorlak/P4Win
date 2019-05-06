//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Diff.cpp

#include "stdafx.h"
#include <io.h>
#include <sys\stat.h>
#include "p4win.h"
#include "cmd_diff.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_Diff, CP4Command)


CCmd_Diff::CCmd_Diff(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4DIFF;
	m_TaskName= _T("Diff");
}

BOOL CCmd_Diff::Run(CStringList *files, LPCTSTR filter, TCHAR sFlag /*= '\0'*/, BOOL bThreadWait)
{
	ASSERT(!files->IsEmpty());

	BOOL bAllowBinary = GET_P4REGPTR()->GetDiffAppIsBinary();
	if (!bAllowBinary)
	{
		CString appName;
		for(POSITION pos=files->GetHeadPosition(); pos != NULL; )
		{
			appName.Empty();
			CString extension = GetFilesExtension(files->GetNext(pos));
			if(!extension.IsEmpty())
				appName= GET_P4REGPTR()->GetAssociatedDiff(extension);
			if (!appName.IsEmpty())
			{
				bAllowBinary = TRUE;
				break;
			}
		}
	}
	
	// Set the base of arg list
	ClearArgs();
	m_BaseArgs=AddArg(_T("diff"));
	if(bAllowBinary && GET_SERVERLEVEL() > 7)
		m_BaseArgs=AddArg(_T("-t"));
	if(filter != NULL)
		m_BaseArgs=AddArg(filter);
	if (sFlag)
	{
		TCHAR sflag[4] = _T("-s");
		sflag[2] = sFlag;
		m_BaseArgs=AddArg(sflag);
	}

	m_posStrListIn=files->GetHeadPosition();
	m_pStrListIn=files;  
	m_DiffRunCount=0;
	m_DiffErrCount = 0;
	m_sFlag = sFlag;
	m_ThreadWait = bThreadWait;

	// Put the first few files into the arg list
	NextListArgs();
		
	return CP4Command::Run();
}

void CCmd_Diff::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	m_StrListOut.AddHead(data);
}

UINT DiffCleanupThread( LPVOID pParam )	// must be a non-member function!
{
	RUNAPPTHREADINFO *lprati = (RUNAPPTHREADINFO *)pParam;
	HGLOBAL hMem = (HGLOBAL)lprati->pParam;
	TWOSTRINGS * p2Strs= (TWOSTRINGS *) ::GlobalLock( hMem );

	WaitForSingleObject( lprati->hProcess, INFINITE );
	CloseHandle( lprati->hProcess );

	if (p2Strs->str1)
	{
		_tchmod(p2Strs->str1, _S_IREAD | _S_IWRITE);
		_tunlink(p2Strs->str1);
	}
	if (p2Strs->str2)
	{
		_tchmod(p2Strs->str2, _S_IREAD | _S_IWRITE);
		_tunlink(p2Strs->str2);
	}
	::GlobalUnlock(hMem);
	::GlobalFree(hMem);
    AfxEndThread( 0 );
	return 0;
}

// Called by CGuiClientUser
void CCmd_Diff::Diff( FileSys *f1, FileSys *f2, int doPage, char * diffFlags, Error *e )
{
	// Check for possible abort request
	if(APP_ABORTING())
	{
		ReleaseServerLock();
		ExitThread(0);
	}
	
	m_DiffRunCount++;
	
	if( !f1->IsTextual( ) || !f2->IsTextual(  ) )
	{
	    if( f1->Compare( f2, e ) )
		{
			CString txt;
			txt.Format(_T("Binary files differ but unable to display diff:\n\n%s,\n%s"), 
				CharToCString(f1->Name()), 
				CharToCString(f2->Name()));
			TheApp()->StatusAdd( txt);
			if (!TheApp()->m_DiffPath.IsEmpty())
				AfxMessageBox(txt, MB_OK);
		}
		else
		{
			ASSERT(0);
			return;
		}
	}
	else
	{
		BOOL isUnicode = f1->IsUnicode() + f2->IsUnicode();
		if (isUnicode && (isUnicode != 2))
		{
			if (IDYES != AfxMessageBox(IDS_UNICODETEXTMIX, MB_ICONQUESTION|MB_DEFBUTTON2|MB_YESNO))
				return;
		}
		if ( isUnicode && ((f1->GetType() & FST_MASK) == FST_UTF16 
						|| (f2->GetType() & FST_MASK) == FST_UTF16 ))
			isUnicode = 16;
		CString fname1 = CharToCString(f1->Name());
		CString fname2 = CharToCString(f2->Name());
		CString errorText;
		RunAppMode mode;
		RUNAPPTHREADINFO *lprati;
		if (f1->IsDeleteOnClose() || f2->IsDeleteOnClose())
		{
			long lgth = 0;
			if (f1->IsDeleteOnClose())
				lgth = lstrlen(fname1) + 1;
			if (f2->IsDeleteOnClose())
				lgth += lstrlen(fname2) + 1;

			CString ext1 = GetFilesExtension(fname1);
			CString ext2 = GetFilesExtension(fname2);
			lgth += ext1.GetLength() + ext2.GetLength() + 2;

			// Alloc memory for both the RUNAPPTHREADINFO and the names of the 2 files
			HGLOBAL hMem = ::GlobalAlloc(GMEM_SHARE, sizeof(TWOSTRINGS) + (lgth + 2)*sizeof(TCHAR));
			TWOSTRINGS * p2Strs= (TWOSTRINGS *) ::GlobalLock( hMem );

			// The file names are stored after the TWOSTRINGS struct
			LPTSTR p = (LPTSTR)((char*)p2Strs + sizeof(TWOSTRINGS));
			if (f1->IsDeleteOnClose())
			{
				if (!ext1.CompareNoCase(_T("tmp")) && !ext2.IsEmpty())
					fname1 = RenameFileExt(fname1, ext1, ext2);
				_tcscpy(p2Strs->str1 = p, fname1);
				f1->ClearDeleteOnClose();
				p += _tcslen(p) + 1;
			}
			else p2Strs->str1 = NULL;
			if (f2->IsDeleteOnClose())
			{
				if (!ext2.CompareNoCase(_T("tmp")) && !ext1.IsEmpty())
					fname2 = RenameFileExt(fname2, ext2, ext1);
				_tcscpy(p2Strs->str2 = p, fname2);
				f2->ClearDeleteOnClose();
			}
			else p2Strs->str2 = NULL;

			// Load the RUNAPPTHREADINFO struct
			p2Strs->rati.pfnThreadProc = DiffCleanupThread;
			p2Strs->rati.pParam = (LPVOID)hMem;	// we need the hMem value in order to do the GlobalFree() in DiffCleanupThread()
			lprati = &(p2Strs->rati);

			// Determine the mode of the spawned diff program
			if (GET_P4REGPTR()->GetDontThreadDiffs())
				mode = RA_WAIT;
			else
				mode = m_ThreadWait ? RA_THREADWAIT : RA_THREAD;
		}
		else
		{
			mode = RA_NOWAIT;
			lprati = NULL;
		}
		// Build the f1 display name (BaseFilename.ext in depot)
		int i;
		CString f1display = fname2;	// Yes f2 - we're going to build f1
		if ((i = f1display.ReverseFind(_T('\\'))) != -1)
			f1display = f1display.Mid(i+1);
		f1display += LoadStringResource(IDS__IN_DEPOT);

		if( !((CP4winApp *) AfxGetApp())->RunApp(DIFF_APP, mode, NULL, isUnicode, 
										lprati, errorText, fname1, fname2, 
										_T("-l"), f1display) )
		{
			if (TheApp()->m_DiffPath.IsEmpty())
				TheApp()->StatusAdd( errorText, SV_ERROR);
			else
				AfxMessageBox(errorText, MB_ICONSTOP);
		}

		if (mode == RA_WAIT)
		{
			HGLOBAL hMem = (HGLOBAL)lprati->pParam;
			TWOSTRINGS * p2Strs= (TWOSTRINGS *) ::GlobalLock( hMem );
			if (p2Strs->str1)
			{
				_tchmod(p2Strs->str1, _S_IREAD | _S_IWRITE);
				_tunlink(p2Strs->str1);
			}
			if (p2Strs->str2)
			{
				_tchmod(p2Strs->str2, _S_IREAD | _S_IWRITE);
				_tunlink(p2Strs->str2);
			}
			::GlobalFree(hMem);	// this will also unlock hMem
		}
	}
	
	return;
}

CString CCmd_Diff::RenameFileExt(CString file1, LPCTSTR ext1, LPCTSTR ext2)
{
	if (!StrCmpI(ext1, ext2))
		return file1;

	CString newname = file1 + _T('.') + ext2;

	return (_trename(file1, newname) == 0) ? newname : file1;
}


int CCmd_Diff::GetDiffNbrFiles() const
{
	return (int) m_StrListOut.GetCount();
}

LPCTSTR CCmd_Diff::GetDiffFileName() const
{
	int i;
	for (i = 0; ++i < GetArgc(); )
	{
		if (!_tcscmp(_T("-f"), GetArgv(i)) || 
            !_tcscmp(_T("-t"), GetArgv(i)) ||
		    !_tcsncmp(GetArgv(i), _T("-ds"), 3))
		{
			continue;
		}
		break;
	}
	return GetArgv(i);
}

BOOL CCmd_Diff::HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg)
{
	m_DiffErrCount++;
	m_DiffErrBuf = errBuf;
	if ((m_sFlag == _T('r')) && (m_DiffErrBuf.Find(_T("not opened for edit")) > 0))
		return TRUE;
	return FALSE;
}
