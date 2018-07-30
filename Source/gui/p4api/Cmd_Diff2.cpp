//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Diff2.cpp

#include "stdafx.h"
#include "p4win.h"
#include "cmd_diff2.h"
#include "cmd_prepbrowse.h"
#include "cmd_where.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_Diff2, CP4Command)


CCmd_Diff2::CCmd_Diff2(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4DIFF2;
	m_TaskName= _T("Diff2");
}


BOOL CCmd_Diff2::Run(LPCTSTR file1, LPCTSTR file2, 
					 int rev1, int rev2, 
					 CString &fileType1, CString &fileType2,
					 BOOL bLocal1, BOOL bLocal2, BOOL bDoIt/*=FALSE*/)
{
	// Record the args
	m_FileName[0]=file1;
	m_FileName[1]=file2;
	m_FileRev[0]=rev1;
	m_FileRev[1]=rev2;
	m_FileType[0]=fileType1;
	m_FileType[1]=fileType2;
	m_LocalFlag[0]=bLocal1;
	m_LocalFlag[1]=bLocal2;
	m_DoIt=bDoIt;

	if (bDoIt)
	{
		ClearArgs();
		AddArg(_T("diff2"));
		AddArg(_T("-q"));
		AddArg(m_FileName[0]);
		AddArg(m_FileName[1]);
	}
	
	return CP4Command::Run();
}

void CCmd_Diff2::PreProcess(BOOL& done)
{
	m_InfoText.Empty();

	// If we are going to actually run the command on the server
	// then there is no preprocessing to do.
	if (m_DoIt)
	{
		done=FALSE;
		return;
	}

	int i;
	Error e;
	CString fn1;
	CString fn2;
	BOOL isTextual1;
	BOOL isTextual2;
	BOOL isUnicode1;
	BOOL isUnicode2;
	BOOL isUtf161;
	BOOL isUtf162;
	BOOL success;

	// Get the first file
	if (m_FileRev[0] >= 0 && (i = m_FileName[0].FindOneOf(_T("@#"))) != -1)
		m_FileName[0] = m_FileName[0].Left(i);
	if (!m_LocalFlag[0])
	{
		CCmd_PrepBrowse cmd1(m_pClient);
		cmd1.Init(NULL, RUN_SYNC, HOLD_LOCK);
		success= cmd1.Run(m_FileName[0], m_FileType[0], m_FileRev[0]);
		cmd1.CloseConn(&e);
		m_FatalError= cmd1.GetError();
		m_ErrorTxt= cmd1.GetErrorText();
		if(	!success || m_FatalError || cmd1.NoFileAtThatRev() )
		{
			done=TRUE;
			return;
		}
		fn1 = cmd1.GetTempName();
		isTextual1 = cmd1.GetTempFile()->IsTextual();
		isUnicode1 = cmd1.GetTempFile()->IsUnicode();
		isUtf161 = (cmd1.GetTempFile()->GetType() & FST_MASK) == FST_UTF16;
	}
	else
	{
		if (m_FileName[0].GetAt(1) != _T(':'))
		{
			success = FALSE;
			CCmd_Where cmd(m_pClient);
			cmd.Init(NULL, RUN_SYNC, HOLD_LOCK);
			if ( cmd.Run(m_FileName[0]) && !cmd.GetError() 
			  && cmd.GetDepotFiles()->GetCount() )
			{
				m_FileName[0] = cmd.GetLocalSyntax();
				success = TRUE;
			}
			cmd.CloseConn(&e);
			if(	!success )
			{
				m_FatalError = TRUE;
				m_ErrorTxt.FormatMessage(IDS_UNABLE_TO_CONVERT_s_TO_LOCAL_SYNTAX, m_FileName[0]);
				done=TRUE;
				return;
			}
		}
		fn1 = m_FileName[0].GetBuffer(m_FileName[0].GetLength()+1);
		isUtf161 = (m_FileType[0].Find(_T("utf16")) != -1);
		isUnicode1 =  isUtf161 || (m_FileType[0].Find(_T("unicode")) != -1);
		isTextual1 = isUnicode1 || (m_FileType[0].Find(_T("text")) != -1);
	}
    
	// Get the second file
	if (m_FileRev[1] >= 0 && (i = m_FileName[1].FindOneOf(_T("@#"))) != -1)
		m_FileName[1] = m_FileName[1].Left(i);
	if (!m_LocalFlag[1])
	{
		CCmd_PrepBrowse cmd2(m_pClient);
		cmd2.Init(NULL, RUN_SYNC, HOLD_LOCK);
		success= cmd2.Run(m_FileName[1], m_FileType[1], m_FileRev[1]);
		cmd2.CloseConn(&e);
		m_FatalError= cmd2.GetError();
		m_ErrorTxt= cmd2.GetErrorText();
		if(	!success || m_FatalError  || cmd2.NoFileAtThatRev() )
		{
			done=TRUE;
			return;
		}
		fn2 = cmd2.GetTempName();
		isTextual2 = cmd2.GetTempFile()->IsTextual();
		isUnicode2 = cmd2.GetTempFile()->IsUnicode();
		isUtf162 = (cmd2.GetTempFile()->GetType() & FST_MASK) == FST_UTF16;
	}
	else
	{
		if (m_FileName[1].GetAt(1) != _T(':'))
		{
			success = FALSE;
			CCmd_Where cmd(m_pClient);
			cmd.Init(NULL, RUN_SYNC, HOLD_LOCK);
			if ( cmd.Run(m_FileName[1]) && !cmd.GetError() 
			  && cmd.GetDepotFiles()->GetCount() )
			{
				m_FileName[1] = cmd.GetLocalSyntax();
				success = TRUE;
			}
			cmd.CloseConn(&e);
			if(	!success )
			{
				m_FatalError = TRUE;
				m_ErrorTxt.FormatMessage(IDS_UNABLE_TO_CONVERT_s_TO_LOCAL_SYNTAX, m_FileName[1]);
				done=TRUE;
				return;
			}
		}
		fn2 = m_FileName[1].GetBuffer(m_FileName[1].GetLength()+1);
		isUtf162 = (m_FileType[1].Find(_T("utf16")) != -1);
		isUnicode2 =  isUtf161 || (m_FileType[1].Find(_T("unicode")) != -1);
		isTextual2 = isUnicode2 || (m_FileType[1].Find(_T("text")) != -1);
	}

	// check to see if we have a binary filetype for either one of the files
	// and if so, has the user specified their own diff program and does it handle binary files
	// if using p4diff or their diff doesn't handle binary, just use internal Compare function
	//
	// If dealing with binary files, get the file extension, if any,
	// and find out if it has an associated diff for that extension
	CString appName;
	appName.Empty();
	BOOL b = (!isTextual1 || !isTextual2) && !GET_P4REGPTR()->GetDiffAppIsBinary();
	if ( b )
	{
		CString extension = GetFilesExtension(fn2);
		if(!extension.IsEmpty())
			appName= GET_P4REGPTR()->GetAssociatedDiff(extension);
	}
	if ( b && appName.IsEmpty() )
	{
		FileSys *f1 = FileSys::Create( FST_BINARY );
		FileSys *f2 = FileSys::Create( FST_BINARY );
		f1->Set( CharFromCString(fn1) );
		f2->Set( CharFromCString(fn2) );
	    if( f1->Compare( f2, &e ) )
		{
			m_InfoText.Format(_T("Binary files differ but unable to display diff:\n\n%s,\n%s"), 
				m_FileName[0], m_FileName[1]);
			TheApp()->StatusAdd(m_InfoText);
			if (m_Output2Dlg)
				m_InfoText.Empty();
		}
		else
		{
			m_InfoText.Format(_T("Binary files identical:\n\n%s,\n%s"), 
				m_FileName[0], m_FileName[1]);
			TheApp()->StatusAdd(m_InfoText);
			if (m_Output2Dlg)
				m_InfoText.Empty();
		}
		delete f1;
		delete f2;
	}
	else	// both are text  or  user's diff can handle binary files
	{
		CString errorText;
		CString buf1;
		CString buf2;
		BOOL isUnicode = isUnicode1 + isUnicode2;
		if (isUnicode)
		{
			if (isUnicode != 2)
			{
				if (IDYES != AfxMessageBox(IDS_UNICODETEXTMIX, MB_ICONQUESTION|MB_DEFBUTTON2|MB_YESNO))
				{
					done=TRUE;
					return;
				}
			}
			if (isUtf161 || isUtf162)
				isUnicode = 16;
		}
		if (m_FileRev[0] > 0)
			buf1.Format(_T("%s#%ld"), m_FileName[0], m_FileRev[0]);
		else
			buf1.Format(_T("%s"), m_FileName[0]);
		if (m_FileRev[1] > 0)
			buf2.Format(_T("%s#%ld"), m_FileName[1], m_FileRev[1]);
		else
			buf2.Format(_T("%s"), m_FileName[1]);
		if( !TheApp()->RunApp(DIFF_APP, RA_NOWAIT, NULL, isUnicode, NULL, 
								errorText, fn1, fn2, _T("-l"), buf1, _T("-r"), buf2) )
		TheApp()->StatusAdd(errorText, SV_ERROR );
	}

	// Note:
	// By setting done=TRUE, we prevent the main ExecCommand loop from running,
	// so this command will consist only of the subcommands that were invoked
	// in PreProcess()
	done=TRUE;
}

void CCmd_Diff2::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	if (m_DoIt && level == _T('0'))
	{
		if (m_Output2Dlg)
			m_InfoText += CString(msg) + _T('\n');
		else
			TheApp()->StatusAdd(msg);
	}
	else
		CP4Command::OnOutputInfo(level, data, msg);
}

BOOL CCmd_Diff2::HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg )
{
	if ( StrStr(errBuf, _T("No file(s) to diff")) )
	{
		TheApp()->StatusAdd(m_Output2Dlg ? LoadStringResource(IDS_NOFILESDIFFER) 
										 : errMsg, SV_COMPLETION);
		return TRUE ; 
	}
	return ( FALSE );
}
