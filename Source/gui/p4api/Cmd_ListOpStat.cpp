//
// Copyright 1998 Perforce Software.  All rights reserved.
//
//

// Cmd_ListOpstat.cpp

#include "stdafx.h"
#include "p4win.h"
#include "Cmd_ListOpStat.h"
#include "Cmd_Ostat.h"
#include "Cmd_ListOp.h"
#include "Cmd_Get.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


	
IMPLEMENT_DYNCREATE(CCmd_ListOpStat, CP4Command)


CCmd_ListOpStat::CCmd_ListOpStat(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4LISTOPSTAT;
	m_TaskName= _T("ListOpStat");
	m_OutputError = FALSE;
	m_OpenAfterDelete = FALSE;
	m_ChkForSyncs = m_Sync2Head = m_WarnIfLocked = m_RevertUnchgAfter = FALSE;
	m_NbrChgedFilesReverted = 0;
}

void CCmd_ListOpStat::DeleteStatList()
{
	while(!m_StatList.IsEmpty())
		delete (CP4FileStats *) m_StatList.RemoveHead( );
}

BOOL CCmd_ListOpStat::Run(CStringList *files, int command, long changeNum/*=0*/, LPCTSTR newType/*=NULL*/)
{
	if ((command == P4VIRTREVERT && GET_SERVERLEVEL() < 20)
	 || (command == P4REVERTUNCHG && GET_SERVERLEVEL() < 14))
	{
		ASSERT(0);
		return FALSE;
	}
	// Store the parms that will be passed to CCmd_ListOp
	m_pFileSpecs= files;
	m_Command= command;
	m_ChangeNumber= changeNum;
	m_NewType= newType;

	return CP4Command::Run();
}

void CCmd_ListOpStat::PreProcess(BOOL& done)
{
	int i;
	POSITION pos;

	CCmd_ListOp cmd1(m_pClient);
	
	// Set up and run ListOp synchronously
	cmd1.Init(NULL, RUN_SYNC);
	cmd1.SetChkForSyncs(m_ChkForSyncs);
	cmd1.SetWarnIfLocked(m_WarnIfLocked);
	if(cmd1.Run(m_pFileSpecs, m_Command, m_ChangeNumber, m_NewType))
	{
		m_FatalError= cmd1.GetError();
		done=TRUE;
		if (m_ChkForSyncs)
		{
			CStringList * pSyncList = cmd1.GetSyncList();
			if (!pSyncList->IsEmpty())
			{
				for( pos= pSyncList->GetHeadPosition(); pos!= NULL; )
					m_Unsynced.AddHead( pSyncList->GetNext(pos) );
				
				CStringList * pEditList = cmd1.GetList();
				for( pos= pEditList->GetHeadPosition(); pos!= NULL; )
				{
					CString txt = pEditList->GetNext(pos);
					if ((i = txt.Find(_T('#'))) != -1)
						txt = txt.Left(i);
					m_RevertIfCancel.AddHead( txt );
				}
			}
			else m_ChkForSyncs = FALSE;
		}
	}
	else
	{
		m_ErrorTxt= _T("Unable to Run ListOp");
		m_FatalError=TRUE;
	}
	// Extract the CStringList from ListOp, which has filtered
	// results for the command. Note that Errors, warnings and 
	// gee-whiz info have been thrown to the status window and 
	// are not in the list
	CStringList *strList= cmd1.GetList();

	// Check for huge file sets, where incremental screen update can take
	// a very looong time to complete.  We are more tolerant for repoens,
	// because they involve updates in only one pane
	if(!m_FatalError && m_Command== P4REOPEN && strList->GetCount() > (3 * MAX_FILESEEKS))
	{
		m_HitMaxFileSeeks= TRUE;
		TheApp()->StatusAdd(_T("CCmd_ListOpStat/P4REOPEN hit MAX_FILESEEKS - blowing off incremental update"), SV_DEBUG);
	}
	else if(!m_FatalError && m_Command!= P4REOPEN && strList->GetCount() > MAX_FILESEEKS)
	{
		m_HitMaxFileSeeks= TRUE;
		TheApp()->StatusAdd(_T("CCmd_ListOpStat hit MAX_FILESEEKS - blowing off incremental update"), SV_DEBUG);
	}

	// In the usual case of zero to a few hundred files, gather ostat-like info
	// for each file so that screen updates can be completed
	else if(!m_FatalError && strList->GetCount() > 0)
	{
		// Save a copy of the stringlist.
		for( pos= strList->GetHeadPosition(); pos!= NULL; )
			m_StrListOut.AddHead( strList->GetNext(pos) );

		// See if we need to run Ostat 
		if(m_Command==P4EDIT || m_Command==P4DELETE)
		{
			// Set up and run ostat synchronously
        	CCmd_Ostat cmd2(m_pClient);
			cmd2.Init(NULL, RUN_SYNC);
			if(cmd2.Run(FALSE, m_ChangeNumber))
				m_FatalError= cmd2.GetError();
			else
			{
				m_ErrorTxt= _T("Unable to Run Ostat");
				m_FatalError=TRUE;
			}
			
			CObArray const *array= cmd2.GetArray();	
			if(!m_FatalError && array->GetSize() > 0)
			{
				// Save a copy of the oblist.
				for( int i=0; i < array->GetSize(); i++ )
					m_StatList.AddHead( array->GetAt(i) );
			}
		}
		else
			PrepareStatInfo();
	}

	// Post the completion message
	if(!m_FatalError)
	{
		CString message;
		int already = 0;
		int reopened = 0;
		switch(m_Command)
		{
		case P4EDIT:
			for( pos= strList->GetHeadPosition(); pos!= NULL; )
			{
				CString str = strList->GetNext(pos);
				if (str.Find(_T(" - currently opened ")) != -1)
					already++;
				else if (str.Find(_T(" - reopened ")) != -1)
					reopened++;
			}
			message.FormatMessage(IDS_OPENED_n_FILES_FOR_EDIT, 
									strList->GetCount() - already - reopened);
			if (reopened)
				message.FormatMessage(IDS_s_n_FILES_REOPENED, message, reopened);
			if (already)
				message.FormatMessage(IDS_s_n_FILES_ALREADY_OPENED, message, already);
			break;
		
		case P4REOPEN:
			message.FormatMessage(IDS_REOPENED_n_FILES, strList->GetCount());
			break;

		case P4REVERT:
		case P4VIRTREVERT:
			m_OutputError = cmd1.GetOutputErrFlag();
			message.FormatMessage(m_OutputError 
				? IDS_ERROR_REVERTING_n_FILES : IDS_REVERTED_n_FILES, strList->GetCount());
			break;

		case P4REVERTUNCHG:
			m_OutputError = cmd1.GetOutputErrFlag();
			if (m_OutputError)
				message.FormatMessage(IDS_ERROR_REVERTING_n_FILES, strList->GetCount());
			else if (m_NbrChgedFilesReverted)
				message.FormatMessage(IDS_REVERTED_n_FILES_n_CHG_n_UNCHG, 
					m_NbrChgedFilesReverted + strList->GetCount(), 
					m_NbrChgedFilesReverted, strList->GetCount());
			else
				message.FormatMessage(IDS_REVERTED_n_FILES, strList->GetCount());
			break;

		case P4LOCK:
			message.FormatMessage(IDS_LOCKED_n_FILES, strList->GetCount());
			break;

		case P4UNLOCK:	
			message.FormatMessage(IDS_UNLOCKED_n_FILES, strList->GetCount());
			break;

		case P4DELETE:
			for( pos= strList->GetHeadPosition(); pos!= NULL; )
			{
				CString str = strList->GetNext(pos);
				if (str.Find(_T(" - currently opened ")) != -1)
					already++;
				else if (str.Find(_T(" - reopened ")) != -1)
					reopened++;
			}
			message.FormatMessage(IDS_OPENED_n_FILES_FOR_DELETE,
									strList->GetCount() - already - reopened);
			if (reopened)
				message.FormatMessage(IDS_s_n_FILES_REOPENED, message, reopened);
			if (already)
				message.FormatMessage(IDS_s_n_FILES_ALREADY_OPENED, message, already);
			break;

		case P4ADD:
			message.FormatMessage(IDS_OPENED_n_FILES_FOR_ADD, strList->GetCount());
			break;

		default:
			ASSERT(0);
		}
		if(!message.IsEmpty())
			TheApp()->StatusAdd( message, SV_COMPLETION );
	}
	
	done=TRUE;
}


// PrepareStatInfo()
//
// For calls other than EDIT and DELETE, we can get the gui's
// depot and changes windows updated with the sparse info that
// the server has returned.  But we need to put that sparse info
// into the appropriate CP4FileStat objects, to avoid complexities
// in the list process handlers of the depot and changes windows

void CCmd_ListOpStat::PrepareStatInfo()
{
	POSITION pos;
	CString listRow, fname, fRev;
	int separator;
	int pound = -1;
	int lastSlash;
	int rev = -1;
	BOOL rowError;

	for(pos=m_StrListOut.GetHeadPosition(); pos !=NULL; )
	{
		listRow=fname=m_StrListOut.GetNext(pos);
		rowError=FALSE;

		//////////////
		// Separate the filename from the action description
		// For all operations but lock and unlock, this amounts to a
		// quick search for '#'.  In the case of the lock commands, 
		// there is no revision number, so search for the action text itself

		switch(m_Command)
		{
		case P4LOCK:
			// For Lock and Unlock, server doesnt send rev number
			separator= listRow.Find(_T(" - locking"));   
			if(separator == -1)
				rowError=TRUE;
			break;

		case P4UNLOCK:
			// For Lock and Unlock, server doesnt send rev number
			separator= listRow.Find(_T(" - unlocking"));
			if(separator == -1)
				rowError=TRUE;
			break;

		default:
			pound= listRow.Find(_T('#'));
			if(pound == -1)
			{
				separator = -1;	// this is just to make the compiler shut up
				rowError=TRUE;
				break;
			}

			separator= pound+1;
			int len= listRow.GetLength();
			for( ; separator < len ; separator++)
			{
				if(listRow[separator]==_T(' ') && listRow[separator+1]==_T('-') && listRow[separator+2]==_T(' '))
				break;
			}
			if(separator==len)
				rowError=TRUE;
		}

		if(rowError)
		{ 
			// doesnt look like a valid row, report it and skip it
			ASSERT(0); 
			listRow= _T("Invalid listRow: ") + listRow;
			TheApp()->StatusAdd(listRow, SV_WARNING);
			continue; 
		}	
					
		fname=fname.Left(separator);
		lastSlash=fname.ReverseFind(_T('/'));
		
		// For Lock and Unlock, server doesnt send rev number
		if(m_Command != P4LOCK && m_Command != P4UNLOCK)
		{
			fRev=fname.Mid(pound+1);
			rev=_ttol(fRev);
			fname=fname.Left(pound);		// full name without revision
		}

		CP4FileStats *fs= new CP4FileStats;
		fs->SetDepotPath(fname);

		switch(m_Command)
		{
		case P4EDIT:
			fs->SetOpenAction(F_EDIT, FALSE);
			fs->SetHaveRev(rev);
			fs->SetOpenChangeNum(m_ChangeNumber);
			break;
		case P4DELETE:
			fs->SetOpenAction(F_DELETE, FALSE);
			fs->SetHaveRev(rev);
			fs->SetOpenChangeNum(m_ChangeNumber);
			break;
		case P4LOCK:
			fs->SetLocked(TRUE, FALSE);
			break;
		case P4UNLOCK:
			fs->SetLocked(FALSE, FALSE);
			break;
		case P4REOPEN:
		case P4REVERT:
		case P4VIRTREVERT:
		case P4REVERTUNCHG:
			break;
		case P4ADD:
			fs->SetOpenAction(F_ADD, FALSE);
			fs->SetHaveRev(rev);
			fs->SetOpenChangeNum(m_ChangeNumber);
			break;
		default:
			ASSERT(0);
		}

		m_StatList.AddHead(fs);
	} // for each
}

void CCmd_ListOpStat::Add2RevertList( CObject const * obj, int iAction )
{
	CP4FileStats const *stats= (CP4FileStats const *) obj;
	CString name=stats->GetFullDepotPath();
	m_RevertIfCancel.AddHead(name);
	if (iAction == 1)
		m_RevertAdds.AddHead(name);
}

