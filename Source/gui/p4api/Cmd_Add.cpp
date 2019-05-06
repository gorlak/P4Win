//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Add.cpp

#include "stdafx.h"
#include "p4win.h"
#include "Cmd_Add.h"
#include "Cmd_Fstat.h"
#include "Cmd_Opened.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


	
IMPLEMENT_DYNCREATE(CCmd_Add, CP4Command)


CCmd_Add::CCmd_Add(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4ADD;
	m_TaskName= _T("Add");
	m_OpenAction = 0;
}


BOOL CCmd_Add::Run(int changeNum, CStringList *files, CStringList *files2edit)
{
	if (files2edit)		// Save off any files that are to be edited
	{
		for (POSITION pos= files2edit->GetHeadPosition(); pos!=NULL; )
			m_Str2Edit.AddTail(files2edit->GetNext(pos));
	}

	m_pStrListIn= files;
	m_posStrListIn= m_pStrListIn->GetHeadPosition();
	m_ChangeNum= changeNum;

	ClearArgs();	
	m_BaseArgs=AddArg(_T("add"));

	if (GET_SERVERLEVEL() >= 18)	// always use -f for 2004.2 or later servers
		m_BaseArgs=AddArg(_T("-f"));

	if(changeNum)
	{
		AddArg(_T("-c"));
        m_BaseArgs=AddArg(m_ChangeNum);
	}
	NextListArgs();

	return CP4Command::Run();
}

void CCmd_Add::PostProcess()
{
	if(m_StrListOut.GetCount() > MAX_FILESEEKS)
	{
		// If there are too many files, do not waste time
		// gathering info for an incremental window update
		m_HitMaxFileSeeks= TRUE;
		TheApp()->StatusAdd(_T("CCmd_Add hit MAX_FILESEEKS - blowing off incremental update"), SV_DEBUG);
	}
	else if(GET_SERVERLEVEL() >= 19)			// 2005.1 or later?
	{
     	// Set up and run fstat synchronously
		CStringList strList;
		strList.AddHead(_T("//..."));
		CCmd_Fstat cmd0(m_pClient);
		cmd0.Init(NULL, RUN_SYNC);
		cmd0.SetIncludeAddedFiles( TRUE );
		if(cmd0.Run( FALSE, &strList, TRUE, 0, TRUE, m_ChangeNum ))
		{
			m_FatalError= cmd0.GetError();
			MergeOpenInfo(cmd0.GetFileList());
		}
	}
	else
	{
		// Set up and run Opened synchronously
		CCmd_Opened cmd(m_pClient);
		cmd.Init(NULL, RUN_SYNC);
		if(cmd.Run(FALSE))
		{
			m_FatalError= cmd.GetError();
			MergeOpenInfo(cmd.GetList());
		}
		else
		{
			m_ErrorTxt= _T("Unable to Run Opened");
			m_FatalError=TRUE;
		}
	}
}

void CCmd_Add::MergeOpenInfo(CObList *pOpenList)
{
	// The names of added files are in m_StrListOut
	// Rummage through the list of my open files, pOpenList,
	// and remove all files that are not on the added files list.
	// TODO: revisit this procedure.  both lists are alphabetical, so
	// there should be a performance gain if we walk the lists together	
	// instead of comparing each item in the first list w/ each item in the
	// second list

	CP4FileStats *stats;
	POSITION posStrList, lastPos;
	POSITION posOpenList= pOpenList->GetHeadPosition();
	CString openName;
	CString addName;
	int compLen;
	BOOL found;

	// Go top to bottom thru openlist
	while( posOpenList != NULL )
	{
		lastPos=posOpenList;
		// Get the depot filename
		stats= (CP4FileStats *) pOpenList->GetNext(posOpenList);
		openName=stats->GetFullDepotPath();
		openName+= _T('#');
		compLen=openName.GetLength();
		found=FALSE;

		posStrList= m_StrListOut.GetHeadPosition();
		while(posStrList != NULL)
		{
			addName=m_StrListOut.GetNext(posStrList);
			if(_tcsncmp(openName, addName, compLen)==0)
			{
				found=TRUE;
				break;
			}
		}
		if(!found)
		{
			delete stats;
			pOpenList->RemoveAt(lastPos);
		}

	} // while posOpenList


	// Finally, transfer the pOpenList pointers to our own ObList,
	// because pOpenList will dissappear when its command goes out of scope
	posOpenList= pOpenList->GetHeadPosition();
	while( posOpenList != NULL )
		m_List.AddHead( pOpenList->GetNext(posOpenList) );
}

void CCmd_Add::OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg)
{
	if(StrStr(data, _T("opened for add")))
	{
		if(!StrStr(data, _T("currently opened for add")))
		{
			m_StrListOut.AddHead(data);
			return;
		}
	}
	else if (m_OpenAction)	// 1 == Edit Only; 2 == Add & Edit
	{
        LPCTSTR pos;
		if ((pos = StrStr(data, _T(" - can't add existing file"))) != 0)
		{
			m_Str2Edit.AddHead(CString(data, (int)(pos-data)));
			return;
		}
		else if ((pos = StrStr(data, _T("can't add (already opened for edit)"))) != 0)
		{
            // remove "can't add(" and ")", leaving only "already opened for edit"
			CString txt(data, (int)(pos-data-1));
			txt += _T("already opened for edit");
            CP4Command::OnOutputInfo(level, txt, data == msg ? txt : msg);
            return;
		}
		else if (StrStr(data, _T(" - warning: add of existing file")) != 0)
			return;
	}

	CP4Command::OnOutputInfo(level, data, msg);
}


