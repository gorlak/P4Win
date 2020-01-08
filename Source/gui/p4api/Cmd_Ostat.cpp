//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Ostat.cpp

#include "stdafx.h"
#include "p4win.h"
#include "MainFrm.h"
#include "Cmd_Ostat.h"
#include "Cmd_Fstat.h"
#include "cmd_Info.h"
#include "Cmd_Opened.h"
#include "Cmd_Resolved.h"
#include "Cmd_Unresolved.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int compareOpenFiles( const void *arg1, const void *arg2 );
int compareUnresolvedFiles( const void *arg1, const void *arg2 );
	
IMPLEMENT_DYNCREATE(CCmd_Ostat, CP4Command)


CCmd_Ostat::CCmd_Ostat(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4OSTAT;
	m_TaskName= "Ostat";
}


BOOL CCmd_Ostat::Run(BOOL allOpenFiles, int changeNumber/*-1*/, CStringList *files/*=NULL*/)
{
	m_AllOpenFiles=allOpenFiles;
	m_ChangeNumber=changeNumber;
	m_pSpecList=files;
	return CP4Command::Run();
}

void CCmd_Ostat::PreProcess(BOOL& done)
{
	Error e;
	POSITION pos;
	CObList *list = 0;
	CObList *mylist;

    // Set initial array sizes with a reasonable rate of
    // growth, to avoid continual re-alloc operations
    m_OpenArray.SetSize(0,500);
    m_UnresolvedArray.SetSize(0,500);
    m_ResolvedArray.SetSize(0,500);

	BOOL b = FALSE;
	if(GET_SERVERLEVEL() >= 19)			// 2005.1 or later?
	{
		// first make sure the client is valid
	    CCmd_Info cmd(m_pClient);
		cmd.Init( NULL, RUN_SYNC );
		if( cmd.Run( ) && !cmd.GetError() )
		{
			CP4Info const &info = cmd.GetInfo();
			if (info.m_ClientRoot.IsEmpty( ))
			{
				HWND hWnd= AfxGetMainWnd()->m_hWnd;
				if( hWnd != NULL )
				{
					::PostMessage(hWnd, WM_CLIENTERROR, 0, 0);
					m_FatalError= -1;
					done=TRUE;
					return;
				}
			}
		}

     	// Set up and run fstat synchronously
		CStringList strlist;
		CString str;
		str.Format(_T("//%s/..."), GET_P4REGPTR()->GetP4Client());
		strlist.AddHead(str);
	    CCmd_Fstat cmd0(m_pClient);
		cmd0.Init(NULL, RUN_SYNC);
		cmd0.SetIncludeAddedFiles( TRUE );
		if(cmd0.Run( FALSE, &strlist, TRUE, 0, TRUE, m_ChangeNumber ) && !cmd0.GetError())
			b = TRUE;
		cmd0.CloseConn(&e);
		// Copy the results from fstat -W
		if (b)
		{
	        mylist= cmd0.GetFileList();
			for( pos= mylist->GetHeadPosition(); pos!= NULL; )
				m_OpenArray.Add( mylist->GetNext(pos) );
		}
	}

	if (!b || m_AllOpenFiles)
	{
		// Set up and run Opened synchronously
		BOOL bFilter;
		CStringList filterList;
		CStringList *pList;
		CCmd_Opened cmd1(m_pClient);
		cmd1.Init(NULL, RUN_SYNC);
		if ((bFilter = GET_P4REGPTR()->FilterPendChgsByMyClient()) == 2)
		{
			CString filter = GET_P4REGPTR()->FilterPendChgsByPath();
			int i;
			while ((i = filter.Find(_T("//"), 2)) != -1)
			{
				CString str = filter.Left(i);
				str.TrimRight();
				filterList.AddTail(str);
				filter = filter.Mid(i);
			}
			filterList.AddTail(filter);
			pList = &filterList;
		}
		else
			pList = m_pSpecList;
		if(cmd1.Run(m_AllOpenFiles, bFilter, m_ChangeNumber, pList))
		{
			m_FatalError= cmd1.GetError();
			done=TRUE;
		}
		else
		{
			m_ErrorTxt= "Unable to Run Opened";
			m_FatalError=TRUE;
		}
		cmd1.CloseConn(&e);
		list= cmd1.GetList();
		// Copy the results from opened
		if(!m_FatalError && (list && list->GetCount() > 0))
		{
			for( pos= list->GetHeadPosition(); pos!= NULL; )
			{
				CP4FileStats *stats= (CP4FileStats *) list->GetNext(pos);
				if( b && ( stats->IsMyOpen() || stats->IsOtherUserMyClient() ) )
				{
					delete stats;
					continue;
				}
				m_OpenArray.Add( stats );
			}
		}
	}

	if(!m_FatalError && !m_OpenArray.IsEmpty())
	{
		if (list && list->GetCount() > 0)
		{
			for( pos= list->GetHeadPosition(); pos!= NULL; )
			{
				CP4FileStats *stats= (CP4FileStats *) list->GetNext(pos);
				if( b && ( stats->IsMyOpen() || stats->IsOtherUserMyClient() ) )
				{
					delete stats;
					continue;
				}
				m_OpenArray.Add( stats );
			}
		}

     	// Set up and run unresolved (p4 resolve-n)
	    CCmd_Unresolved cmd2(m_pClient);
		cmd2.Init(NULL, RUN_SYNC);
		if(cmd2.Run())
            m_FatalError= cmd2.GetError();
        else
		{
			m_ErrorTxt= "Unable to Run Unresolved";
			m_FatalError=TRUE;
		}

        if( !m_FatalError )
		{
			// Store a pointer to the list of unresolved files
			CObArray const *pArray= cmd2.GetArray();
			for( int i=0; i < pArray->GetSize(); i++)
			{
				CP4FileStats *stats= (CP4FileStats *) pArray->GetAt(i);
				m_UnresolvedArray.Add( stats->GetFullDepotPath());
				delete stats;
			}

			// Set up and run resolved (p4 resolved)
        	CCmd_Resolved cmd3(m_pClient);
			cmd3.Init(NULL, RUN_SYNC);
			if(cmd3.Run())
				m_FatalError= cmd3.GetError();
			else
			{
				m_ErrorTxt= "Unable to Run Resolved";
				m_FatalError=TRUE;
			}
            cmd3.CloseConn(&e);

			if( !m_FatalError )
			{
				// Store a pointer to the list of Resolved files
				CObArray const *pArray= cmd3.GetArray();
				for( int i=0; i < pArray->GetSize(); i++)
				{
					CP4FileStats *stats= (CP4FileStats *) pArray->GetAt(i);
					m_ResolvedArray.Add( stats->GetFullDepotPath());
					delete stats;
				}
			}
        }
	}

	// Look for any changelists with shelved files.
	// p4 changes?
	// user-fstat -Op -Rs -e 4758996 //...'

	if( !m_FatalError && m_OpenArray.GetSize() > 0 )
         // Sort the results from opened
        SortOpened();

    if( !m_FatalError && m_UnresolvedArray.GetSize() > 0 )
        // Sort the results from Unresolved
        SortUnresolved();

    if( !m_FatalError && m_ResolvedArray.GetSize() > 0 )
        // Sort the results from Unresolved
        SortResolved();

    
	///////////////
	// Update files in m_OpenList.  The array of opened files is sorted
    // by myopen+changeno+filename, so search should never look at another
    // user's open files.  Files appearing in the m_Unresolved list
	// must have the unresolved att set; Files appearing in the m_Resolved list
	// must have the resolved att set

	if(!m_FatalError && ((m_UnresolvedArray.GetSize() > 0) || (m_ResolvedArray.GetSize() > 0)))
	{
        INT_PTR maxR= m_ResolvedArray.GetSize();
        INT_PTR maxU= m_UnresolvedArray.GetSize();
        INT_PTR maxO= m_OpenArray.GetSize();

        CString fName;
        CP4FileStats *stats;

		int i;
        for( i=0; i<maxU; i++)
        {
            fName= m_UnresolvedArray.GetAt(i);

            for( int j=0; j<maxO; j++ )
            {
                stats= (CP4FileStats *) m_OpenArray.GetAt(j);
                if(stats->IsMyOpen() && fName == stats->GetFullDepotPath())
                {
					// Update the unresolved flag for file 
					stats->SetUnresolved(TRUE);
					break;
				}
                if( !stats->IsMyOpen() )
                {
                    // It is possible that 'p4 resolve -n' returned unresolved files on
                    // my client that were opened by another user.  In 99.1 we may try to
                    // display these.  For now, do not show these files as unresolved, so
                    // break off the search for opened files as soon as we hit a file that
                    // is not my open.
                    break;
                }
            }
        }
        for( i=0; i<maxR; i++)
        {
            fName= m_ResolvedArray.GetAt(i);

            for( int j=0; j<maxO; j++ )
            {
                stats= (CP4FileStats *) m_OpenArray.GetAt(j);
                if(stats->IsMyOpen() && fName == stats->GetFullDepotPath())
                {
					// Update the resolved flag for file 
					stats->SetResolved(TRUE);
					break;
				}
                if( !stats->IsMyOpen() )
                {
                    // It is possible that 'p4 resolved' returned unresolved files on
                    // my client that were opened by another user.  In 99.1 we may try to
                    // display these.  For now, do not show these files as unresolved, so
                    // break off the search for opened files as soon as we hit a file that
                    // is not my open.
                    break;
                }
            }
        }
    }

    // Provide statistics if requested
    if( GET_P4REGPTR()->ShowCommandTrace( ) )
    {
        CString statistics;
        statistics.Format(_T("     Found %ld opened files with %ld unresolved files and %ld resolved files"), m_OpenArray.GetSize(), m_UnresolvedArray.GetSize(), m_ResolvedArray.GetSize());
        TheApp()->StatusAdd(statistics);
    }
    done=TRUE;
}

// to avoid having to call IS_NOCASE and branch for each compare, set up a 
// compare function pointer:
int (*pCompare)(LPCTSTR str1, LPCTSTR str2) = 0;

void setupCompare()
{
	// If server level not set, we also dont know if server is nocase
	ASSERT(GET_SERVERLEVEL());

	if(IS_NOCASE())
        pCompare = _tcsicmp;
    else
        pCompare = _tcscmp;
}

void CCmd_Ostat::SortOpened()
{
    // Sort the m_OpenArray of CP4FileStats according to:
    // 1) my open files first, other's open files later
    // 2) change number
    // 3) file name

    INT_PTR size= m_OpenArray.GetSize();
    CObject **array= m_OpenArray.GetData();

    setupCompare();
    qsort( (void *) array, size, sizeof( CObject *), compareOpenFiles );
}

void CCmd_Ostat::SortUnresolved()
{
    // Sort the m_UnresolvedArray of CP4FileStats according to:
    // 1) my open files first, other's open files later
    // 2) change number
    // 3) file name

    INT_PTR size= m_UnresolvedArray.GetSize();
    CString *array= m_UnresolvedArray.GetData();

    setupCompare();
    qsort( (void *) array, size, sizeof( CString * ), compareUnresolvedFiles );
}


void CCmd_Ostat::SortResolved()
{
    // Sort the m_ResolvedArray of CP4FileStats according to:
    // 1) my open files first, other's open files later
    // 2) change number
    // 3) file name

    INT_PTR size= m_ResolvedArray.GetSize();
    CString *array= m_ResolvedArray.GetData();

    setupCompare();
    qsort( (void *) array, size, sizeof( CString * ), compareUnresolvedFiles );
}

// return <0 if arg1 < arg2, 0 if arg1=arg2, >0 if arg1 > arg2
int compareOpenFiles( const void *arg1, const void *arg2 )
{
    CP4FileStats *fs1= *((CP4FileStats **) arg1);
    CP4FileStats *fs2= *((CP4FileStats **) arg2);
    
    int result;

    // First compare whose file open it is.  My files sort to the top
    if( (result= fs2->IsMyOpen() - fs1->IsMyOpen()) != 0)
        goto done;

    // Next compare change number
    if( (result= fs1->GetOpenChangeNum() - fs2->GetOpenChangeNum()) != 0)
        goto done;
    
    ASSERT(pCompare);

    // Next compare user@client, for default changes only
    if( fs1->GetOpenChangeNum() == 0 )
        if( (result= pCompare(fs1->GetOtherUsers(), fs2->GetOtherUsers())) != 0)
            goto done;
 
    // Finally compare file name
    result= pCompare(fs1->GetFullDepotPath(), fs2->GetFullDepotPath());

done:
    return result;
}

// return <0 if arg1 < arg2, 0 if arg1=arg2, >0 if arg1 > arg2
int compareUnresolvedFiles( const void *arg1, const void *arg2 )
{
    CString *f1= (CString *) arg1;
    CString *f2= (CString *) arg2;
    ASSERT(pCompare);
    return pCompare(*f1,*f2);
}
