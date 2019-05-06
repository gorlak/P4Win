//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Integrate2.cpp

#include "stdafx.h"
#include "p4win.h"
#include "Cmd_Integrate2.h"
#include "Cmd_Fstat.h"
#include "Cmd_ListOp.h"
#include "Cmd_Opened.h"
#include "Cmd_Integrate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


	
IMPLEMENT_DYNCREATE(CCmd_Integrate2, CP4Command)


CCmd_Integrate2::CCmd_Integrate2(CGuiClient *client) : CP4Command(client)
{
	m_ErrorTxt.Empty();
	m_ReplyMsg= WM_P4INTEGRATE2;
	m_TaskName= _T("Integrate2");
	m_RunSyncAfterPreview = m_HitFatalError = FALSE;
	m_OK2rename = TRUE;
	m_ActualCount = 0;
}


BOOL CCmd_Integrate2::Run( CStringList *source, CStringList *target,
				LPCTSTR branchName, LPCTSTR revRange, LPCTSTR commonPath,
				BOOL isBranch, BOOL isReverse, BOOL isNoCopy,
				BOOL isForced, BOOL isForcedDirect, BOOL isRename, BOOL isPreview,
				int changeNum, BOOL isPermitDelReadd, int delReaddType,
				BOOL isBaselessMerge,  BOOL isIndirectMerge, 
				BOOL isPropagateTypes, BOOL isBaseViaDelReadd, 
				int branchFlag, BOOL bDontSync, BOOL bBiDir, int newChangeNbr )
{
	ASSERT_KINDOF(CStringList, source);
	
	if(branchName==NULL || _tcslen(branchName)==0)
	{
		ASSERT(0);
		return FALSE;
	}

	m_pSourceList= source;
	m_pTargetList= target;
	m_BranchName= branchName;
	m_RevRange= revRange;
	m_CommonPath = commonPath;
	m_IsBranch= isBranch;
	m_IsReverse= isReverse;
	m_IsNoCopy= isNoCopy;
	m_IsForced= isForced;
	m_IsForcedDirect= isForcedDirect;
	m_IsRename= isRename;
	m_IsPreview= isPreview;
	m_ChangeNum= changeNum;
	m_IsPermitDelReadd= isPermitDelReadd;
	m_DelReaddType= delReaddType;
	m_IsBaselessMerge= isBaselessMerge;
	m_IsIndirectMerge= isIndirectMerge;
	m_IsPropagateTypes= isPropagateTypes;
	m_IsBaseViaDelReadd= isBaseViaDelReadd;
	m_BranchFlag = branchFlag;
	m_DontSync = bDontSync;
	m_BiDir = bBiDir;
	m_NewChangeNbr = newChangeNbr;

	return CP4Command::Run();
}

void CCmd_Integrate2::PreProcess(BOOL& done)
{
	Error e;
	POSITION pos;
	CStringList *list = 0, deletedList, saveSource;
	BOOL bIsCaseRename;

	if (m_IsRename && !m_IsPreview && m_pTargetList->GetCount() == 1)
	{
		CString tstr = m_pTargetList->GetHead();
		tstr.TrimRight(_T(".*"));
		int lgth = tstr.GetLength();
		POSITION pos;
		for (bIsCaseRename=TRUE, pos=m_pSourceList->GetHeadPosition(); bIsCaseRename && pos; )
		{
			CString sstr = m_pSourceList->GetNext(pos);
			saveSource.AddTail(sstr);
			sstr = sstr.Left(lgth);
			bIsCaseRename = sstr.CompareNoCase(tstr) ? FALSE : TRUE;
		}
	}
	else
		bIsCaseRename = FALSE;
	BOOL bIsPreview = bIsCaseRename ? TRUE : m_IsPreview;

	done=FALSE;

	////////////////
	// Set up and run Integrate synchronously
	////////////////
	CCmd_Integrate cmd1(m_pClient);
	cmd1.Init(NULL, RUN_SYNC);

	if(cmd1.Run(m_pSourceList, m_pTargetList, m_BranchName, m_RevRange, m_CommonPath,
								m_IsBranch, m_IsReverse, m_IsNoCopy, m_IsForced,  m_IsForcedDirect, 
								m_IsRename, bIsPreview, m_ChangeNum, m_IsPermitDelReadd, 
								m_DelReaddType, m_IsBaselessMerge, m_IsIndirectMerge, 
								m_IsPropagateTypes, m_IsBaseViaDelReadd, 
								m_BranchFlag, m_DontSync, m_BiDir) )
		list= cmd1.GetList();
	else
	{
		m_ErrorTxt= _T("Unable to Run Integrate");
		m_FatalError=TRUE;
		done=TRUE;
	}
	m_HitFatalError = cmd1.GetError() | cmd1.GetErrorCleared();
    cmd1.CloseConn(&e);

	
	///////////////
	// Save the lists of source files and target files
	///////////////
	CStringList deleteList, targetList;

	if(!m_FatalError && list->GetCount() > 0)
	{
		for( pos= list->GetHeadPosition(); pos!= NULL; )
		{
			CString line= list->GetNext(pos);
			CString source, dest;
			
			if( ParseIntegResults( line, source, dest) )
			{
				targetList.AddHead( dest );
				deleteList.AddHead( source );
			}
		}
	}

	///////////////
	// Run 'delete' to nuke source files if requested
	///////////////
	if(!m_FatalError && deleteList.GetCount() > 0 && m_IsRename)
	{
		if(m_IsPreview)
		{
			// Just save a copy of delete list and spew text to the
			// status window
			for( pos=deleteList.GetHeadPosition(); pos!=NULL; )
			{
				CString tmp= deleteList.GetNext(pos);
				// If we are renaming, have to check for file on client
				if (m_IsRename)
				{
					Error e2;
					CCmd_Fstat cmd2(m_pClient);
					cmd2.Init(NULL, RUN_SYNC);
					if(cmd2.Run( FALSE, tmp, FALSE ))
					{
						int haveRev = 0;
						if ((m_FatalError= cmd2.GetError()) == FALSE)
						{
					        CObList *fstatList= cmd2.GetFileList();
							if (fstatList->GetCount() > 0)
							{
								CObject *cobject = fstatList->GetHead();
								CP4FileStats *pFileStats = (CP4FileStats *)cobject;
								haveRev = pFileStats->GetHaveRev();
								delete pFileStats;
							}
							else
								haveRev = 0;
						}
						if (!haveRev)
						{
							CString txt;
							txt.FormatMessage(IDS_FILE_NOT_ON_CLIENT_NO_RENAME, tmp);
							m_ErrorTxt += txt;
							m_FatalError=TRUE;
						}
					}
					else
					{
						m_ErrorTxt += _T("\n\nUnable to Run Fstat for ") + tmp;
						m_FatalError=TRUE;
					}
					cmd2.CloseConn(&e2);

					if (m_FatalError)
					{
						m_OK2rename=FALSE;
						done=TRUE;
						break;
					}
				}
				CString tmp2;
				tmp2.Format(_T("Integrate preview: %s - deleted"), tmp);
				TheApp()->StatusAdd(tmp2);
			}
		}
		else
		{
	        CCmd_ListOp cmd3(m_pClient); 
			cmd3.Init(NULL, RUN_SYNC);
			if(cmd3.Run(&deleteList, P4DELETE, m_ChangeNum))
			{
				// Save the results of the delete operation
				list= cmd3.GetList();
				for( pos= list->GetHeadPosition(); pos!= NULL; )
					deletedList.AddHead( list->GetNext(pos) );
			}
			else
			{
				m_ErrorTxt= _T("Unable to Run Delete");
				m_FatalError=TRUE;
				done=TRUE;
			}
			cmd3.CloseConn(&e);

			if (bIsCaseRename)
			{
				CCmd_Integrate cmd4(m_pClient);
				cmd4.Init(NULL, RUN_SYNC);

				if(cmd4.Run(&saveSource, m_pTargetList, m_BranchName, m_RevRange, m_CommonPath,
											m_IsBranch, m_IsReverse, m_IsNoCopy, m_IsForced,  m_IsForcedDirect, 
											m_IsRename, m_IsPreview, m_ChangeNum, m_IsPermitDelReadd, 
											m_DelReaddType, m_IsBaselessMerge, m_IsIndirectMerge, 
											m_IsPropagateTypes, m_IsBaseViaDelReadd, 
											m_BranchFlag, m_DontSync, m_BiDir) )
					list= cmd4.GetList();
				else
				{
					m_ErrorTxt= _T("Unable to Run Integrate");
					m_FatalError=TRUE;
					done=TRUE;
				}
				cmd4.CloseConn(&e);

				targetList.RemoveAll();
				if(!m_FatalError && list->GetCount() > 0)
				{
					for( pos= list->GetHeadPosition(); pos!= NULL; )
					{
						CString line= list->GetNext(pos);
						CString source, dest;
						
						if( ParseIntegResults( line, source, dest) )
							targetList.AddHead( dest );
					}
				}
			}
		}
	}

	m_ActualCount = int(targetList.GetCount());

	///////////////
	// Run 'opened' for the change number, because this will usually be
	// faster than running it for a long filespec list (targetlist).
	// Filter the results to include files open for branch, integrate and delete.
	//
	// Performance fix:  We use a heuristic ( or magic number) approach to 
	// avoid going compute-bound during the Depot and Changelist pane updates.  After
	// a goodly number of files that need to be hunted down for attribute updates, it 
	// is faster to simply redraw the whole stinkin window. 
	///////////////

	if(!m_FatalError && !m_IsPreview && targetList.GetCount() > MAX_FILESEEKS )
	{
		m_HitMaxFileSeeks= TRUE;
		TheApp()->StatusAdd(_T("CCmd_Integrate2 hit MAX_FILESEEKS - blowing off incremental update"), SV_DEBUG);
	}
	else if(!m_FatalError && !m_IsPreview && targetList.GetCount() > 0 )
	{
		CObList openlist;
		if (GET_SERVERLEVEL() >= 19)			// 2005.1 or later?
		{
     		// Set up and run fstat synchronously
			CStringList strList;
			strList.AddHead(_T("//..."));
			CCmd_Fstat cmd0(m_pClient);
			cmd0.Init(NULL, RUN_SYNC);
			cmd0.SetIncludeAddedFiles( TRUE );
			if(cmd0.Run( FALSE, &strList, TRUE, 0, TRUE, m_ChangeNum ))
			{
				CObList *pOpenList = cmd0.GetFileList();
				POSITION posOpenList = pOpenList->GetHeadPosition();
				while( posOpenList != NULL )
					openlist.AddHead( pOpenList->GetNext(posOpenList) );
			}
		}
		else
		{
			CCmd_Opened cmd2(m_pClient);
			cmd2.Init(NULL, RUN_SYNC);
			if(cmd2.Run(FALSE, FALSE, m_ChangeNum))
			{
				CObList *pOpenList = cmd2.GetList();
				POSITION posOpenList = pOpenList->GetHeadPosition();
				while( posOpenList != NULL )
					openlist.AddHead( pOpenList->GetNext(posOpenList) );
			}
		}
		if(openlist.GetCount() > 0)
		{
			// Filter the list to m_TargetList, ensuring that unresolved
			// gets set for all integrated files
			CP4FileStats *stats;
			for( pos=openlist.GetHeadPosition(); pos != NULL; )
			{
				stats= (CP4FileStats *) openlist.GetNext(pos) ;
				switch(stats->GetMyOpenAction())
				{
				case F_INTEGRATE:
					stats->SetUnresolved(TRUE); // fall thru
				case F_BRANCH:
				case F_IMPORT:
				case F_DELETE:
					m_TargetList.AddHead( stats );
					break;
				case F_EDIT:
				{
					// run thru the target list to find out if we are
					// integrating into a file that is open for Edit
					BOOL b = FALSE;
					for (POSITION pos2=targetList.GetHeadPosition(); pos2 != NULL; )
					{
						if (targetList.GetNext(pos2) == stats->GetFullDepotPath())
						{
							stats->SetUnresolved(TRUE);
							m_TargetList.AddHead( stats );
							b = TRUE;
							break;
						}
					}
					if (b)
						break;
				}
				default:
					delete stats;
				}
			}
		}
		else
		{
			m_ErrorTxt= _T("Unable to Run Opened");
			m_FatalError=TRUE;
		}

		// if we didn't find at least as many files in the target changelist
		// as there are target files, we better refresh when we are done.
		// we also have to delete all the stats in m_TargetList since they won't be seen,
		// and we better notify the user since files could be scattered in multiple changelists
		if(openlist.GetCount() < targetList.GetCount())
		{
			m_HitMaxFileSeeks = TRUE;	// set flag to trigger refresh

			// delete stats
			CP4FileStats *stats;
			for( pos=m_TargetList.GetHeadPosition(); pos != NULL; )
			{
				stats= (CP4FileStats *)m_TargetList.GetNext(pos);
				delete stats;
			}

			// notify user
			CString text;
			CString chgnbr;
			if (m_ChangeNum)
				chgnbr.Format(_T("Changelist %d"), m_ChangeNum);
			else
				chgnbr = _T("Default Changelist");
			text.FormatMessage(IDS_CHG_s_HASTOOFEWFILES, chgnbr);
			TheApp()->StatusAdd(text, SV_WARNING, TRUE);
		}
	}
	else if(!m_FatalError && m_IsPreview)
	{
		m_PreviewTargetList.RemoveAll();
		if ( targetList.GetCount() > 0 )
		{
			for (pos=targetList.GetHeadPosition(); pos != NULL; )
				m_PreviewTargetList.AddHead(targetList.GetNext(pos));
			m_ActualCount = int(m_PreviewTargetList.GetCount());
		}
	}

	done=TRUE;
}


BOOL CCmd_Integrate2::ParseIntegResults( CString& line, CString& source, CString& dest)
{
	//////// No results looks looks like
	//depot/main/dps/bmp.txt - all revision(s) already integrated in pending changelist.

	if(line.Find(_T(" - all revision(s) already integrated")) != -1)
	{
		m_ErrorTxt += _T('\n') + line;
		m_OK2rename = FALSE;
		return FALSE;
	}

	BOOL gotBranchSync = FALSE;

	//////// Successful line looks like, with the target file on the left:
	//depot/main/dps/bmp.txt#1 - branch/sync from //depot/main/cps/bmp.txt#1,#3
	//depot/notmain/cps/bmp.txt#3 - sync/integrate from //depot/main/cps/bmp.txt#2,#3
	//depot/notmain/cps/bmp2.txt#3 - delete from //depot/main/cps/bmp2.txt#2
	int x,y;         
	if( (x=line.Find(_T(" - branch/sync from "))) != -1)
	{
		dest=line.Left(x);
		source=line.Mid(x+int(_tcslen(_T(" - branch/sync from "))));
		gotBranchSync = TRUE;
	}
	else if( (x=line.Find(_T(" - import/sync from "))) != -1)
	{
		dest=line.Left(x);
		source=line.Mid(x+int(_tcslen(_T(" - import/sync from "))));
	}
	else if( (x=line.Find(_T(" - sync/integrate from "))) != -1)
	{
		dest=line.Left(x);
		source=line.Mid(x+int(_tcslen(_T(" - sync/integrate from "))));
	}
	else if( (x=line.Find(_T(" - sync/delete from "))) != -1)
	{
		dest=line.Left(x);
		source=line.Mid(x+int(_tcslen(_T(" - sync/delete from "))));
	}
	else if( (x=line.Find(_T(" - sync/edit from "))) != -1)
	{
		dest=line.Left(x);
		source=line.Mid(x+int(_tcslen(_T(" - sync/edit from "))));
	}
	else if( (x=line.Find(_T(" - branch from "))) != -1)
	{
		dest=line.Left(x);
		source=line.Mid(x+int(_tcslen(_T(" - branch from "))));
		if (m_IsNoCopy && m_IsRename)
			gotBranchSync = TRUE;
	}
	else if( (x=line.Find(_T(" - add from "))) != -1)
	{
		dest=line.Left(x);
		source=line.Mid(x+int(_tcslen(_T(" - add from "))));
		if (m_IsNoCopy && m_IsRename)
			gotBranchSync = TRUE;
	}
	else if( (x=line.Find(_T(" - integrate from "))) != -1)
	{
		dest=line.Left(x);
		source=line.Mid(x+int(_tcslen(_T(" - integrate from "))));
	}
	else if( (x=line.Find(_T(" - delete from "))) != -1)
	{
		dest=line.Left(x);
		source=line.Mid(x+int(_tcslen(_T(" - delete from "))));
	}
	else if( (x=line.Find(_T(" - import from "))) != -1)
	{
		dest=line.Left(x);
		source=line.Mid(x+int(_tcslen(_T(" - import from "))));
	}
	else if( (x=line.Find(_T(" - edit from "))) != -1)
	{
		dest=line.Left(x);
		source=line.Mid(x+int(_tcslen(_T(" - edit from "))));
	}
	else if (( line.Find(_T(" - can't ")) != -1)
		  || ( line.Find(_T(" - is already opened by ")) != -1)
		  || ( line.Find(_T("must sync before integrating")) != -1))
	{
		TheApp()->StatusAdd( line, SV_WARNING );
		m_ErrorTxt += _T('\n') + line;
		m_OK2rename = FALSE;
		return FALSE;
	}
	else
	{
		CString txt;
		txt.Format( _T("Error: Unable to parse results:\n%s"), line );
		TheApp()->StatusAdd( txt, SV_WARNING );
		m_ErrorTxt += _T('\n') + line;
		m_OK2rename = FALSE;
		return FALSE;
	}

	if (!gotBranchSync)
	{
		if (m_ErrorTxt.IsEmpty())
			m_ErrorTxt = LoadStringResource(IDS_ALL_TARGETS_MUST_NOT_EXIST_OR_BE_DELETED);
		CString fname = (x != -1) ? line.Left(x) : line;
		m_ErrorTxt += _T('\n') + fname;
		m_OK2rename = FALSE;
	}

	// lose the revision info
	BOOL foundSourceRev=FALSE;
	BOOL foundDestRev=FALSE;
		
	while( (x= source.ReverseFind(_T('#'))) != -1)
	{
		source= source.Left(x);
		foundSourceRev=TRUE;
	}
	
	while( (y= dest.ReverseFind(_T('#'))) != -1)
	{
		dest= dest.Left(y);
		foundDestRev=TRUE;
	}

	if(foundSourceRev && foundDestRev)
		return TRUE;
	else
	{
		CString txt;
		txt.Format( _T("Error: Unable to parse results:\n%s"), line );
		TheApp()->StatusAdd(	txt, SV_WARNING);
		return FALSE;
	}
}
