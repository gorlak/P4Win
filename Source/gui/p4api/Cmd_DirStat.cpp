//
// Copyright 1998 Perforce Software.  All rights reserved.
//
//

// Cmd_DirStat.cpp

#include "stdafx.h"
#include "p4win.h"
#include "MainFrm.h"
#include "Cmd_Dirstat.h"
#include "Cmd_Fstat.h"
#include "Cmd_Dirs.h"
#include "Cmd_Where.h"
#include "strops.h"

#pragma warning (disable:4786)
#include <list>
#include <string>

using namespace std;

#ifdef UNICODE
typedef list<wstring> LISTSTR;
#else
typedef list<string> LISTSTR;
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CCmd_DirStat, CP4Command)


CCmd_DirStat::CCmd_DirStat(CGuiClient *client) : CP4Command(client)
{
	m_ReplyMsg= WM_P4DIRSTAT;
	m_TaskName= _T("DirStat");
	m_ShowOnlyNotInDepot = MainFrame()->m_ShowOnlyNotInDepot;
}

BOOL CCmd_DirStat::Run( CStringList *specs, BOOL entireDepot )
{
	m_EntireDepot= entireDepot;
	m_pSpecList=specs;
	return CP4Command::Run();
}

void CCmd_DirStat::PreProcess(BOOL& done)
{
	BOOL bSpecListHasWild = FALSE;
	BOOL bFileListHasWild = FALSE;
	CStringList files;
    POSITION pos;
	Error e;
	
	if (GET_P4REGPTR( )->ShowEntireDepot( ) <= SDF_DEPOT)
	{
		// Make a copy of the specList, since Cmd_Dirs will 
		// delete the list as it is processed
		CStringList specListCopy;
		for( pos= m_pSpecList->GetHeadPosition(); pos!= NULL; )
			specListCopy.AddHead( m_pSpecList->GetNext(pos) );

		// Set up and run Dirs synchronously
		CCmd_Dirs cmd1(m_pClient);
		cmd1.Init(NULL, RUN_SYNC);
		if(cmd1.Run( &specListCopy, m_EntireDepot ))
		{
			m_FatalError= cmd1.GetError();
			done=TRUE;
		}
		else
		{
			m_ErrorTxt= "Unable to Run Dirs";
			done= m_FatalError=TRUE;
		}
		cmd1.CloseConn(&e);

		CStringList *list= cmd1.GetList();
    
		// Copy the results from dirs
		if(!m_FatalError && list->GetCount() > 0)
		{
			for( pos= list->GetHeadPosition(); pos!= NULL; )
				m_Dirs.AddHead( list->GetNext(pos) );
		}
	}
	else
	{
		CString theroot = TheApp()->m_ClientRoot;
		if (GetFileAttributes(theroot) == -1)
		{
			done = TRUE;
			return;
		}
		TCHAR curdir[MAX_PATH+1];
		if ((theroot.GetAt(0) == _T('\\')) && GetCurrentDirectory(sizeof(curdir)/sizeof(TCHAR), curdir))
		{
			curdir[2] = _T('\0');
			theroot = curdir + theroot;
			TheApp()->m_ClientRoot = theroot;
		}
		if (theroot.GetLength() > 3)
			theroot += _T('\\');
		int therootlgth = theroot.GetLength();

		CFileFind finder;

		// find the directories
		for( pos= m_pSpecList->GetHeadPosition(); pos!= NULL; )
		{
			CString dirname = m_pSpecList->GetNext(pos);
			if (dirname.FindOneOf(_T("@#%")) != -1)
				bSpecListHasWild = TRUE;
			if (dirname.GetAt(0) == _T('/'))
				continue;	// we don't handle depot syntax here
			else
				dirname.Replace(_T('/'), _T('\\'));

			// check to make sure the desired directory
			// is actually under the client's root dir
			if (_tcsnicmp(dirname, theroot, therootlgth))
				continue;

			// now find all the dirs and files in the directory
			BOOL bWorking = finder.FindFile(dirname);

			while (bWorking)
			{
				bWorking = finder.FindNextFile();

				// skip . and .. files
				if (finder.IsDots())
					continue;

				CString str = finder.GetFilePath();

				// we can't deal with files or directories 
				// with * in their name, so skip 'em
				if (str.Find(_T('*')) != -1)
				{
					CString txt;
					txt.FormatMessage(IDS_CANTDEALWITH_ASTERISK, str);
					TheApp()->StatusAdd(txt, SV_WARNING);
					continue;
				}

				// if it's a directory, add it to the command's dirs list
				if (finder.IsDirectory())
				{
					m_Dirs.AddHead( str );
				}
				// if it's a regular file, save it for later processing
				// in the fstat part of this routine below
				else if ((GET_P4REGPTR( )->ShowEntireDepot( ) == SDF_LOCALTREE)
					&& (!finder.IsHidden() || GET_P4REGPTR( )->ShowHiddenFilesNotInDepot())
					&& !finder.IsSystem())
				{
					if (str.FindOneOf(_T("@#%")) != -1)
						bFileListHasWild = TRUE;
					files.AddTail( str );
				}
			}
		}

		finder.Close();

		// if we only want to see Perforce files we now need to eliminate
		// all the directories that don't have Perforce files - so run p4 dirs
		if (m_Dirs.GetCount())
		{
			// Save a copy of the m_Dirs, since Cmd_Dirs will 
			// delete the list as it is processed
			BOOL bWild = FALSE;
			BOOL b = GET_P4REGPTR( )->ShowEntireDepot( ) > SDF_DEPOT;	// Local View?
			CStringList saveDirs;
			for( pos= m_Dirs.GetHeadPosition(); pos!= NULL; )
			{
				CString str = m_Dirs.GetNext(pos);
				if (b && str.FindOneOf(_T("@#%")) != -1)
					bWild = TRUE;
				saveDirs.AddHead( str );
			}
			if (bWild)
			{
				m_Dirs.RemoveAll();
				for( pos= saveDirs.GetHeadPosition(); pos!= NULL; )
				{
					CString str =  saveDirs.GetNext(pos);
					if (b && str.FindOneOf(_T("@#%")) != -1)
					{
						int star = str.Find(_T('*'));
						StrBuf b;
						StrBuf f;
						f << CharFromCString(str);
						StrPtr *p = &f;
						StrOps::WildToStr(*p, b);
						str = CharToCString(b.Value());
						if (star != -1)
						{
							while ((star = str.Find(_T("%2A"))) != -1)
							{
								CString s1 = str.Left(star);
								CString s2 = str.Mid(star + sizeof(_T("%2A")));
								str = s1 + _T('*') + s2;
							}
						}
					}
					m_Dirs.AddHead( str );
				}
			}

			// Set up and run Dirs synchronously
			CCmd_Dirs cmd1(m_pClient);
			cmd1.Init(NULL, RUN_SYNC);
			if(cmd1.Run( &m_Dirs, m_EntireDepot ))
			{
				m_FatalError= cmd1.GetError();
				done=TRUE;
			}
			else
			{
				m_ErrorTxt= "Unable to Run Dirs";
				done= m_FatalError=TRUE;
			}
			cmd1.CloseConn(&e);

			// all we care about is the error listing
			CStringList *list= cmd1.GetErrors();
    
			// Make sure m_Dirs was emptied
			m_Dirs.RemoveAll();

			// Look thru the directories and remove any
			// mentioned in an error message
			if(!m_FatalError && list->GetCount() > 0)
			{
				int lgth = TheApp()->m_ClientRoot.GetLength();
				CStringList fstatlist;
				CStringList fdirslist;
				POSITION pos2;
				for( pos = saveDirs.GetHeadPosition(); pos!= NULL; )
				{
					CString dir = saveDirs.GetNext(pos);
					BOOL bAdd = TRUE;
					for( pos2 = list->GetHeadPosition(); pos2 != NULL; )
					{
						CString errmsg = list->GetNext(pos2);
						if ((tolower(dir.GetAt(lgth)) == tolower(errmsg.GetAt(lgth)))
						  && errmsg.Find(dir) == 0)
						{
							if (errmsg.Find(dir + _T(" - file(s) not in client view")) == 0)
							{
								if (GET_SERVERLEVEL() >= 25)	// 2008.1 or later?
								{
									CString dirdotdotdot = dir + _T("\\...");
									fstatlist.AddTail(dirdotdotdot);
									fdirslist.AddTail(dir);
								}
								bAdd = FALSE;
							}
							else if (errmsg.Find(dir + _T(" - no mappings in client view")) == 0)
								m_NotMapped.AddHead(dir);
							break;
						}
					}
					if (bAdd)
						m_Dirs.AddHead(dir);
				}

				if (fstatlist.GetCount())
				{
					CCmd_Fstat cmd2(m_pClient);
					cmd2.Init(NULL, RUN_SYNC);
					if(cmd2.Run( FALSE, &fstatlist, m_EntireDepot, 0, FALSE, -1, 1 ))
					{
						if (cmd2.GetFileList()->GetCount())
						{
							CObList *fstatList= cmd2.GetFileList();
							for( pos= fstatList->GetHeadPosition(); pos!= NULL; )
							{
								CP4FileStats *stats = ( CP4FileStats * )fstatList->GetNext( pos );
								ASSERT_KINDOF( CP4FileStats, stats );
								CString clientPath = stats->GetFullClientPath( );
								for( pos2= fdirslist.GetHeadPosition(); pos2!= NULL; )
								{
									CString dir = fdirslist.GetNext(pos2);
									CString dirslash = dir + _T("\\");
									CString path = clientPath.Left(dirslash.GetLength());
									if (path == dirslash)
									{
										m_Dirs.AddHead(dir);
										break;
									}
								}
								delete stats;
							}
						}
					}
					else
					{
						m_ErrorTxt= _T("Unable to Run Fstat");
						m_FatalError=TRUE;
					}
				}
			}
			else	// we have to put the saved dirs back in m_Dirs
			{
				for( pos = saveDirs.GetHeadPosition(); pos!= NULL; )
					m_Dirs.AddHead(saveDirs.GetNext(pos));
			}
		}
	}

	if(!m_FatalError)
	{
     	// Set up and run fstat
		// Fisrt convert any wild syntax if there is any
		CStringList *pstrlist = m_pSpecList;
		CStringList strlist;
		if (bSpecListHasWild)
		{
			for( pos= m_pSpecList->GetHeadPosition(); pos!= NULL; )
			{
				CString str = m_pSpecList->GetNext(pos);
				if (str.FindOneOf(_T("@#%")) != -1)
				{
					int star = str.Find(_T('*'));
					StrBuf b;
					StrBuf f;
					f << CharFromCString(str);
					StrPtr *p = &f;
					StrOps::WildToStr(*p, b);
					str = CharToCString(b.Value());
					if (star != -1)
					{
						while ((star = str.Find(_T("%2A"))) != -1)
						{
							CString s1 = str.Left(star);
							CString s2 = str.Mid(star + sizeof(_T("%2A")));
							str = s1 + _T('*') + s2;
						}
					}
				}
				strlist.AddTail( str );
			}
			pstrlist = &strlist;
		}
		// Now run the Fstat
	    CCmd_Fstat cmd2(m_pClient);
		cmd2.Init(NULL, RUN_SYNC);
		if(cmd2.Run( FALSE, pstrlist, m_EntireDepot, 0, FALSE, -1, -1 ))
            m_FatalError= cmd2.GetError();
        else
		{
			m_ErrorTxt= _T("Unable to Run Fstat");
			m_FatalError=TRUE;
		}

        CObList *fstatList= cmd2.GetFileList();
		    
	    // Copy the results from fstat
	    if(!m_FatalError && fstatList->GetCount() > 0)
	    {
			if (GET_P4REGPTR( )->ShowEntireDepot( ) == SDF_LOCALTREE)
			{
				// We need to generate 2 sorted lists:
				LISTSTR filesarray;		// 1) the 'files' names
				LISTSTR fstatarray;		// 2) the pFileStats->GetFullClientPath() names
				LISTSTR::iterator ifi;	// iterator for filesarray
				LISTSTR::iterator ifs;	// iterator for fstatarray

				for (pos = files.GetHeadPosition(); pos != NULL; )
				{
					filesarray.insert(filesarray.end(), 
						(TCHAR *)(files.GetNext( pos ).GetBuffer()));
				}
				filesarray.sort();

			    for( pos= fstatList->GetHeadPosition(); pos!= NULL; )
				{
				    CObject *cobject = fstatList->GetNext(pos);
				    m_Files.AddHead( cobject );
					if (((CP4FileStats *)cobject)->GetHeadAction() != F_DELETE)
						fstatarray.insert(fstatarray.end(), 
							(TCHAR *)(((CP4FileStats *)cobject)->GetFullClientPath()));
				}
				fstatarray.sort();

				// now walk both arrays at the same time
				// add any file not in the fstatarray to the 'files' list
				files.RemoveAll();
				ifi = filesarray.begin();
				ifs = fstatarray.begin();
				while (ifi != filesarray.end())
				{
#ifdef _DEBUG
					if (ifs != fstatarray.end())
					{
						CString ifistr = (*ifi).c_str();
						CString ifsstr = (*ifs).c_str();
						int i = fstatarray.size();
						i--;	// to shutup the compiler
					}
#endif
					if (ifs != fstatarray.end() 
					 && Compare((*ifi).c_str(), (*ifs).c_str())==0)
					{
						ifi++;						// found it in the fstats
						fstatarray.remove((*ifs).c_str());// remove it from the fstat list
						ifs = fstatarray.begin();	// setup for next item
						continue;
					}
					if (ifs != fstatarray.end())	// keep checking if not at end of fstat list
					{
						ifs++;						// next fstat item
						continue;
					}
					files.AddHead( (*ifi++).c_str() );	// not in fstat list; add to not in depot list
					ifs = fstatarray.begin();			// setup for next item
				}
			}
			else
			{
				for( pos= fstatList->GetHeadPosition(); pos!= NULL; )
				{
					CObject *cobject = fstatList->GetNext(pos);
					m_Files.AddHead( cobject );
				}
			}
        }

		if (GET_P4REGPTR( )->ShowEntireDepot( ) > SDF_DEPOT)
		{
			// Now see if there were any " - no mappings in client view" errors
			// if so, add those directories to the 'm_NotMapped' list
			CStringList *list= cmd2.GetErrors();
			if (list->GetCount())
			{
				for(POSITION pos = list->GetHeadPosition(); pos != NULL; )
				{
					int i;
					CString errmsg = list->GetNext(pos);
					if ((i = errmsg.Find(_T('*'))) > 0)
					{
						errmsg = errmsg.Left(i);
						if (errmsg.GetLength() != 3)	// handle C:\ == root
							TrimRightMBCS(errmsg, _T("\\"));
						m_NotMapped.AddHead(errmsg);
					}
				}
			}
		}

		if (GET_P4REGPTR( )->ShowEntireDepot( ) == SDF_LOCALTREE)
		{
			// 'files' contains a list of files that are 
			// under the root but not under Perforce control.
			// 'm_NotMapped' contains a list of directories
			// that are not in the client
			// Now remove any files that cannot be added to the client
			POSITION pos;
			POSITION pos1, pos2;
			if (files.GetCount() && m_NotMapped.GetCount())
			{
				int i;
				for (pos = m_NotMapped.GetHeadPosition(); pos != NULL; )
				{
					CString dir = m_NotMapped.GetNext( pos );
					for (pos1 = files.GetHeadPosition(); ( pos2 = pos1 ) != NULL; )
					{
						CString file = files.GetNext( pos1 );
						if ((i = ReverseFindMBCS(file, _T('\\'))) == -1)
							continue;
						file = file.Left(i);
						if (file == dir)
						{
							// Don't mess with pos1!
							files.RemoveAt( pos2 );
						}
					}
				}
			}
			if (files.GetCount())
			{
				cmd2.CloseConn(&e);

				// if any addable files contain @ # or %, we have to convert them
				if (bFileListHasWild)
				{
					BOOL bWild = FALSE;
					CStringList strlist;
					for (POSITION pos = files.GetHeadPosition(); pos != NULL; )
					{
						CString file = files.GetNext( pos );
						if (file.FindOneOf(_T("@#%")) != -1)
							bWild = TRUE;
						strlist.AddHead( file );
					}
					if (bWild)
					{
						files.RemoveAll();
						for (POSITION pos = strlist.GetHeadPosition(); pos != NULL; )
						{
							CString file = strlist.GetNext( pos );
							if (file.FindOneOf(_T("@#%")) != -1)
							{
								int star = file.Find(_T('*'));
								StrBuf b;
								StrBuf f;
								f << CharFromCString(file);
								StrPtr *p = &f;
								StrOps::WildToStr(*p, b);
								file = CharToCString(b.Value());
								if (star != -1)
								{
									while ((star = file.Find(_T("%2A"))) != -1)
									{
										CString s1 = file.Left(star);
										CString s2 = file.Mid(star + sizeof(_T("%2A")));
										file = s1 + _T('*') + s2;
									}
								}
							}
							files.AddHead( file );
						}
					}
				}

				// Now create FileStats for all the addable files
				// First run 'p4 where' on all the files to
				// get both the local and the depot syntax
				CCmd_Where cmd3(m_pClient);
				cmd3.Init(NULL, RUN_SYNC);
				if ( cmd3.Run(&files) && !cmd3.GetError() )
				{
					CStringList *locals = cmd3.GetLocalFiles();
					CStringList *depots = cmd3.GetDepotFiles();
					ASSERT(locals->GetCount() == depots->GetCount());

					// get the fstat of files opened for add
					CObList *list = (CObList *)::SendMessage(m_ReplyWnd, WM_GETADDFSTATS, 0, 0);
					ASSERT_KINDOF(CObList, list);
					int listcount = list->GetCount();

					// now walk the 'p4 where' output
					CString localprev = _T("");
					for (pos1 = locals->GetHeadPosition(), 
						 pos2 = depots->GetHeadPosition(); pos1 != NULL; )
					{
						CString localsyntax = locals->GetNext( pos1 );
						CString depotsyntax = depots->GetNext( pos2 );

						// check for duplicates (due to a + mapping in clinet's view)
						if (localprev == localsyntax)
							continue;
						localprev = localsyntax;

						// do we have an unmap record?
						if (depotsyntax.GetAt(0) == _T('-'))
						{
							depotsyntax = depotsyntax.Mid(1);
							POSITION pos= m_Files.GetHeadPosition();
							while(pos != NULL)
							{
								// Get the filestats info for files opened for add
								CP4FileStats *stats = (CP4FileStats *) m_Files.GetNext(pos);
								if ( depotsyntax == stats->GetFullDepotPath() )
								{
									// since the obvious m_Files.RemoveAt(pos) doesn't work
									// we just mark the record as deleted 
									// by setting the depotpat to the empty string
									stats->SetDepotPath(_T(""));
									break;
								}
							}
							// clear the 'localprev' since we might
							// subsequently map this same file back in
							localprev = _T("-");
							// we certainly don't want to add
							// a new fstat record for an unmap
							continue;
						}

						CP4FileStats *newStats= new CP4FileStats;
						BOOL b = FALSE;
						// if there are files opened for add, 
						// we need to check that list before creating a
						// whole new fstat record from the bare 'p4 where' data
						if (listcount)
						{
							POSITION pos= list->GetHeadPosition();
							while(pos != NULL)
							{
								// Get the filestats info for files opened for add
								CP4FileStats *stats = (CP4FileStats *) list->GetNext(pos);
								ASSERT_KINDOF(CP4FileStats, stats);

								// is this file which has been added the same as 
								// the file we are examining from the 'p4 where' list?
								if ( depotsyntax == stats->GetFullDepotPath() )
								{
									// Create a copy of the fstat info
									newStats->Create(stats);
									newStats->SetClientPath(localsyntax);
									newStats->SetNotInDepot(TRUE);
									newStats->SetHaveRev(0);
									listcount--;
									b = TRUE;
									break;
								}
							}
						}
						// If we didn't create a new fstat because it wasn't open for add
						// we have to create a sparse fstat record from the 'p4 where' data
						if(!b && !newStats->Create(localsyntax, depotsyntax))
							delete newStats;
						else
						{
							// now add out newly created fstat record to the list
							CObject *cobject = (CP4FileStats *)newStats;
							m_Files.AddHead( cobject );
						}
					}
				}
				else
					TheApp()->StatusAdd( LoadStringResource(IDS_UNABLE_TO_SHOW_LOCAL_FILES), SV_WARNING );  
			}
		}
	}
	
    done=TRUE;
}
