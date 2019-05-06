/*
 * Copyright 1997, 1999 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */

#include "stdafx.h"
#include "p4win.h"
#include "P4SpecData.h"
#include "TokenString.h"
#include "Cmd_SendSpec.h"
#include "Cmd_EditSpec.h"

#include "p4client.h"
#include "p4job.h"
#include "p4branch.h"
#include "p4user.h"
#include "p4label.h"

#include <afxdisp.h>

const CString g_SelectionSeparator = _T(":");
const CString g_tagFile = _T("Files");
const CString g_tagRoot = _T("Root");
const CString g_tagReviews = _T("Reviews");
const int DONE_FETCHING = 0;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//		these are the 98.2 format specs describing 97.3 data.
//		98.2 doesn't need these at all, since the server returns them to me.
//		but pre 98.2 sends either nothing or garbage, so put in the
//		old data in the new format.
//
const char * BRANCH_SPEC = 
	"Branch;rq;ro;len:32;;"
	"Date;type:line;ro;len:20;;"
	"Owner;len:32;;"
	"Description;type:text;len:128;;"
	"View;code:6;type:wlist;words:2;len:64;;"
    ;

const char * CHANGE_SPEC =
	"Change;rq;ro;len:10;;"
	"Date;type:line;ro;len:20;;"
	"Client;ro;len:32;;"
	"User;ro;len:32;;"
    "Status;ro;len:10;;"
    "Description;type:text;rq;;"
	"Jobs;type:wlist;len:32;;"
    "Files;type:llist;len:64;;"
	;

const char * CLIENT_SPEC = 
	"Client;rq;ro;len:32;;"
	"Date;type:line;ro;len:20;;"
	"Owner;len:32;;"
	"Description;type:text;len:128;;"
	"Root;type:line;rq;len:64;;"
	"Options;type:line;len:64;;"
	"View;type:wlist;words:2;len:64;;"
    ;

const char * DEPOT_SPEC =
	"Depot;rq;ro;len:32;;"
    "Owner;len:32;;"
    "Date;type:line;ro;len:20;;"
    "Description;type:text;len:128;;"
    "Type;rq;len:10;;" 
    "Address;len:64;;"
	"Map;rq;len:64;;"
	;

const char * LABEL_SPEC = 
	"Label;rq;ro;len:32;;"
	"Date;type:line;ro;len:20;;"
	"Owner;len:32;;"
	"Description;type:text;len:128;;"
	"Options;code:5;type:line;len:64;;"
	"View;code:6;type:wlist;len:64;;"
    ;

const char * USER_SPEC	= 
	"User;rq;ro;len:32;;"
	"Email;rq;len:32;;"
	"Update;type:line;ro;len:20;;"
	"Access;type:line;ro;len:20;;"
	"FullName;type:line;rq;len:32;;"
	"JobView;type:line;len:64;;"
	"Reviews;type:wlist;len:64;;"
    ;

const char * GROUP_SPEC	= 
	"Group;code:401;rq;ro;len:32;;"
	"MaxResults;code:402;type:word;len:12;;"
	"MaxScanRows;code:403;type:word;len:12;;"
	"Timeout;code:406;type:word;len:12;;"
	"Subgroups;code:404;type:wlist;len:32;opt:default;;"
	"Users;code:405;type:wlist;len:32;opt:default;;"
    ;

const char * JOB_SPEC = 
	"Job;code:101;rq;len:32;;" 
    "User;code:103;rq;len:32;pre:$user;;"
    "Status;code:102;type:select;rq;len:10;pre:open;val:open/suspended/closed;;"
    "Date;code:104;type:date;ro;len:20;pre:$now;;"
    "Description;code:105;type:text;rq;pre:$blank;;"
	;


CSpecData::~CSpecData ( ) 
{ 
    CleanUpArray( m_aWordList );
#ifdef UNICODE
   	for ( int i = 0; i < m_aStrGetA.GetSize ( ) ; i++ )
	{
		if ( m_aStrGetA.GetAt( i ) )
			delete m_aStrGetA.GetAt( i );
	}
#endif
}

/*
	_________________________________________________________________

	ParseNoValid doesn't return empty values, but luckily the spec
	def string has the entire spec. so we have to parse it out 
	ourselves. put each element of the spec def str in an element of
	a cstring array and use the array when creating edit boxes for
	fields that are empty.
	_________________________________________________________________
*/

void CSpecData::SetSpecElems( const CString &sSpecDefStr )
{
    m_numParsed = 0;

	CString sd = sSpecDefStr;
	int breakit = 1;

	while ( breakit > 0 )
	{
		breakit = sd.Find ( _T(";;") );
        if ( breakit > 0 )
		{
            m_asSpecElems.Add( sd.Left ( breakit ) );
        //App()->StatusAdd( sd.Left ( breakit ), SV_DEBUG );
       
			sd = sd.Right( sd.GetLength( ) - breakit - 2 );
		}
	}
}


/*
	_________________________________________________________________
*/

int CSpecData::GetElemValue( const CString &sSpecElems, const CString &el )
{
	int where = sSpecElems.Find( el ) ;
	return ( where > 0 )
		? sSpecElems.GetLength( ) - where - el.GetLength( ) 
		: 0;
}


/*
	_________________________________________________________________
*/

CString CSpecData::GetSelectedValues( const CString &sSpecElems )
{
	CString sval = _T(";val:");
	CString val;
	int ival = sSpecElems.Find ( sval );
	if ( ival > 0 )
		val = sSpecElems.Right ( sSpecElems.GetLength( ) - ival - sval.GetLength( ) ); 

	return val;
}


/*
	_________________________________________________________________
*/

CString CSpecData::GetPresetValue( const CString &sSpecElems )
{

	CString spre = _T(";pre:");
	CString pre;

	int ipre = sSpecElems.Find ( spre );
	if ( ipre > 0 )
		pre = sSpecElems.Right ( sSpecElems.GetLength( ) - ipre  - spre.GetLength( ) ); 

	if ( pre == _T("$now") )
	{
		COleDateTime t = COleDateTime::GetCurrentTime();
		pre = t.Format( _T("%Y/%m/%d %H:%M:%S") );
	}
	else if ( pre == _T("$user") )
		pre = GET_P4REGPTR()->GetP4User();
	else if ( pre == _T("$blank") )
		pre = CCmd_EditSpec::g_blankDesc;

	return pre;
}


/*
	_________________________________________________________________
*/

int CSpecData::AddElemForEmptyItems( )
{
    // Avoid over-running array bounds
    if( m_numParsed >= m_asSpecElems.GetSize() )
        return 0;

	CString sSpecElems = m_asSpecElems.GetAt( m_numParsed );
	
	//		tag's easy. it's the first part of the string
	//		fanny: now, why am i returning if it's empty?
	//
	CString tag = sSpecElems.Left( sSpecElems.Find( _T(';') ) );
	if ( tag.IsEmpty( ) )
		return 0;

	//		these are easy too, since all you have to do is find
	//		their tag.
	//
	BOOL bRequired = ( sSpecElems.Find( _T(";rq") ) > 0 ) ? TRUE : FALSE;
	BOOL bisReadOnly = ( sSpecElems.Find( _T(";ro") ) > 0 ) ? TRUE : FALSE;

	//		this assumes that length is the last thing in the string
	//fanny: step through this...
	int maxLen = _ttoi( sSpecElems.Right( 
		sSpecElems.GetLength( ) - sSpecElems.Find( _T(";len:") ) - 5 ) );

	//		get the format code, if present
	CString format = _T(";fmt");
	int pos = sSpecElems.Find ( format );
	if (pos != -1)
	{
		CString s = sSpecElems.Right( sSpecElems.GetLength( ) - pos - 1 - format.GetLength( ) );
        if(s.Find(_T(';')) != -1)
		    format = s.Left ( s.Find ( _T(';') ) );
	}
	else
		format = _T("BA");

	//		just get the code. if it's zero, leave it.
	//
	int code = GetElemValue( sSpecElems, _T(";code:") );
	if ( code > 0 )
		code = _ttoi ( sSpecElems.Right( code ) );

	//		number of words. if it's not there, the default is 1
	//
	const int numWordsDefault = 1;
	int nWords = GetElemValue( sSpecElems, _T(";words:") );
	nWords = nWords > 0 
		?  _ttoi ( sSpecElems.Right( nWords ) )
		: numWordsDefault;

	CString stype = _T(";type");
	pos = sSpecElems.Find ( stype );
	int SDTtype;
	if ( pos == -1 ) 
		SDTtype = SDT_WORD; //which i think is already 0, but you can't be too careful
	else
	{
		CString s = sSpecElems.Right( sSpecElems.GetLength( ) - pos - 1 - stype.GetLength( ) );
        if(s.Find(_T(';')) != -1)
		    s = s.Left ( s.Find ( _T(';') ) );

		SDTtype = ( s == _T("wlist") )  ? SDT_WLIST
				: ( s == _T("text") )   ? SDT_TEXT
				: ( s == _T("line") )   ? SDT_LINE
				: ( s == _T("llist") )  ? SDT_LLIST
				: ( s == _T("date") )   ? SDT_DATE
				: ( s == _T("select") ) ? SDT_SELECT
				: ( s == _T("bulk") )   ? SDT_BULK
				: -1; if ( SDTtype == -1 ) ASSERT ( 0 );
	}

	//		now get the value. if there's anything in the preset field
	//		use it. if it's empty, then value will be empty.
	//		and if there are values to be displayed in a combo box
	//		add them to the value string with a separator between the
	//		preset and the values. the SetControls will divvy them up
	//
	CString selectedvals = GetSelectedValues( sSpecElems );
	CString pre = GetPresetValue( sSpecElems );
	CString Value;
	if ( selectedvals.IsEmpty( ) ||
	        pre == CCmd_EditSpec::g_blankDesc ||
	        SDTtype != SDT_SELECT )
	        Value = pre;
	else if (pre.Find(g_SelectionSeparator) > -1)
	        Value = pre;
	else
	        Value = pre + g_SelectionSeparator + selectedvals;

	if ( SDTtype == SDT_LLIST )
	{
		if ( tag == g_tagFile )
			m_aFile.Add( _T("") );
		else
			m_aList.Add( _T("") );
	}
	else if ( SDTtype == SDT_WLIST )
	{
		if ( tag == g_tagReviews )
			m_aReview.Add( _T("") );
		else if ( tag == _T("Jobs") )
			m_aJobs.Add( _T("") );
		else
		{
			CStringArray *asView = new CStringArray;
			ASSERT ( asView );

			for ( int i = 0; i < nWords; i++ )
				asView->Add( _T("") ); 
			m_aWordList.Add( asView );
		}
	}

	//		that's it. add it
	//
	AddElem( tag, Value, bisReadOnly, bRequired, maxLen, code, SDTtype, nWords, format );
	
	return 0;
}



/*
	_________________________________________________________________
*/

BOOL CSpecData::UnFilledElem( const CString &oldtag, const CString &tag ) 
{
	//		e.g, there can be many jobs, all of which are called one
	//		at a time by parse. so wait until they have all been returned
	//		before taking care of files.
	//
	if ( tag == oldtag )
		return FALSE;

    if( m_numParsed >= m_asSpecElems.GetSize() )
    {
        ASSERT(0);
        return FALSE;
    }

    // This code, and the CheckForStragglers code depend upon the parse
    // order matching the spec definition.  On 98.2, that appears to 
    // always be the case, but the 97.3 servers occasionally sent the
    // spec definition in an alternate order.  Without adding a fair bit
    // more in the way of complexity, the fix is to leave this code be, and
    // include 97.3 spec definitions in this file (see globals at top) which
    // have the spec fields in the correct order

    CString s = m_asSpecElems.GetAt( m_numParsed );
    if ( tag == s.Left ( s.Find ( _T(';') ) ) )
		    return FALSE;
    
	return TRUE;
}


/*
	_________________________________________________________________
*/

void CSpecData::Set( SpecElem *sd, int x, char **wv, Error *e ) 
{
	CString tag = CharToCString(sd->tag.Text( ));
	static CString oldtag;

    while ( UnFilledElem( oldtag, tag ) )
		AddElemForEmptyItems( );

	CString value;
	switch ( sd->type )
	{
	case SDT_WORD:
	case SDT_LINE:
	case SDT_TEXT:
	case SDT_BULK:
	case SDT_DATE:
		value = CharToCString(*wv);
		if ((sd->type == SDT_WORD) && (value.Find(_T(' ')) != -1))
			value = _T("\"") + value + _T("\"");
		break;

	case SDT_SELECT:

		//		add the presets to the actual value so that we can add them
		//		to a combo box. the value will be parsed back later, 
		//		so that the Get() gets the right thing
		//
		value = CharToCString(*wv) + g_SelectionSeparator + CharToCString(sd->values.Text( ));
		break;

	case SDT_LLIST:

		//		the only llists are the Files list for changelist specs
		//		and the AltRoots for Clients
		//		put any other one, like a user-defined one, in a special list
		//	fanny: what if they have more than one?!
		//
		if ( sd->tag == CharFromCString(g_tagFile) )
			m_aFile.Add( CharToCString(*wv) );
		else
			m_aList.Add( CharToCString(*wv) ); 
		break;


	case SDT_WLIST:

		//		the word lists we have are Jobs for changelist specs,
		//		Roots for client specs (as of 2002.2),
		//		Reviews for user specs and Views for lots of other specs.
		//		jobs, roots and reviews are lists with only one word, so keep
		//		them in a simple cstringarray. views, however are lists
		//		with 2 words, so keep them in the wordlist cobarray.
		//		and if a user defined type is one of these, keep it
		//		there too. 
		//fanny: what if they have more than one in a spec?!
		//
		if ( sd->tag == "Jobs" )
			m_aJobs.Add( CharToCString(*wv) );
		else if ( sd->tag == CharFromCString(g_tagReviews) )
		{
			CString str = CharToCString(*wv);		// Reviews have only one word per line
			if (str.Find(_T(' ')) != -1)
				str = _T("\"") + str + _T("\"");
			if (str == _T("#"))			// A review that is a single # means
				str.Empty();		//	put a blank line in the list here.
			m_aReview.Add( str );
		}
		else //		if ( sd->tag == g_tagView )//fanny: take out this test.
		{
			CString str;
			CStringArray *asView = new CStringArray;
			ASSERT ( asView );

			for ( int i = 0; i < sd->nWords; i++ )
			{
				str = CharToCString(wv[i]);
				if (str.Find(_T(' ')) != -1)
					str = _T("\"") + str + _T("\"");
				asView->Add( str );
			}
			m_aWordList.Add( asView );
		}

		break;

	default: 
		TheApp()->StatusAdd( LoadStringResource(IDS_SPEC_WEIRDNESS_IN_SET), SV_DEBUG );
		break;
	}

	if ( tag != oldtag )
		AddElem( tag, value, sd->IsReadOnly(), sd->IsRequired()
					, sd->maxLength, sd->code, sd->type
					, sd->nWords, sd->FmtFmt( ) );
	oldtag = tag;
}


/*
	_________________________________________________________________
*/

void CSpecData::AddElem( const CString &tag, const CString &value, int isReadOnly
						, int required, int maxLength
						, int code, int type, int nWords
						, const CString &format)
{
	// This conversion converts from the new server fomat of L, R or I
	// to the old corresponding BH, MH or IH which is used internally by p4win
	CString fmt = format;
	if      (fmt == _T("L"))
		     fmt =  _T("BH");
	else if (fmt == _T("R"))
		     fmt =  _T("MH");
	else if (fmt == _T("I"))
		     fmt =  _T("IH");

	TCHAR indent = fmt.GetAt(0);
	TCHAR wid = fmt.GetAt(1);

	//		FIXUP FOR CHECK BOXES
	// we stuck a 0x10 char after the prompt (tag) on all internally
	// generated fileds (that we made from a single edit line
	// with the appropriate attributes).
	if (tag.Find((TCHAR)0x10) != -1)
	{
		// Then we have a check box.
		// Adjust the placement of the check boxes:
		// indent == B => 1st of a group; set counter to 1
		// indent != B => not 1st of a group; use counter remainder to place
		if (indent == _T('B'))
		{
			m_ChkBoxCtr = 0;
		}
		else
		{
			switch (++m_ChkBoxCtr % 3)
			{
			case 1:
				indent = _T('M');
				break;
			case 2:
				indent = _T('R');
				break;
			case 0:
				indent = _T('B');
				break;
			}
		}
	}

	// Multiline fields but always be full width
	if (( type == SDT_WLIST ) || ( type == SDT_LLIST )
	 || ( type == SDT_TEXT && maxLength == 0 )
	 || ( type == SDT_TEXT && maxLength > 63 ))
	{
		indent = _T('B');
		wid = _T('A');
	}

	m_aTags.Add( tag );
	m_aValues.Add( value );
	m_indent.Add( indent );
	m_wCode.Add( wid );
	int liOffset = (indent == _T('M') || indent == _T('R')) 
				 ? m_PrevWid == _T('A') || m_PrevIndent == _T('I') || m_PrevIndent == indent
				 : 1;
	m_PrevIndent = indent;
	m_PrevWid = wid;

	CString s;

	s.Format( _T("%d"), isReadOnly );
	m_isReadOnly.Add( s );

	s.Format( _T("%d"), required );
	m_required.Add ( s );

	s.Format( _T("%d"), maxLength );
	m_maxLength.Add ( s );

	s.Format( _T("%d"), code );
	m_code.Add ( s );

	s.Format( _T("%d"), type );
	m_type.Add ( s );

	s.Format( _T("%d"), nWords );
	m_nWords.Add ( s );

	s.Format( _T("%d"), liOffset );
	m_liOffset.Add ( s );

    m_numParsed++;
}


/*
	_________________________________________________________________
*/

void CSpecData::CheckForStragglers( )
{
    INT_PTR numItems = GetNumItems( );
	if ( m_numParsed < numItems )
	{
		INT_PTR missing = numItems - m_numParsed;

		while ( missing-- )
			AddElemForEmptyItems( );
	}
}


/*
	_________________________________________________________________
*/

int CSpecData::Get( SpecElem *sd, int x, char **wv, char **cmt ) 
{
	int ret = DONE_FETCHING;

	INT_PTR numElems = m_aTags.GetSize( );//no, should change this test to numelems
	for ( int i = 0; i < numElems; i++ )
	{
		if ( sd->tag == CharFromCString(m_aTags.GetAt( i )) )
		{
            int nWords = 1;

			switch ( sd->type )
			{
			case SDT_WORD:
			case SDT_LINE:
			case SDT_TEXT:
			case SDT_BULK:
			case SDT_DATE:
                m_aStrGet.SetAtGrow(0, m_aValues.GetAt(i));
				if (sd->type == SDT_WORD)
				{
                    // remove quotes, if present
                    if (!m_aStrGet.GetAt(0).IsEmpty() && (m_aStrGet.GetAt(0).GetAt(0) == _T('\"')))
                    {
                        m_aStrGet.ElementAt(0).TrimLeft(_T("\""));
                        m_aStrGet.ElementAt(0).TrimRight(_T("\""));
                    }
				}
                ret = 1;
                break;

			case SDT_SELECT:
                m_aStrGet.SetAtGrow(0, m_aValues.GetAt(i));
				ret =  (m_aStrGet.GetAt(0).IsEmpty() && !_ttoi(m_required.GetAt( i ))) ? 0 : 1;
                break;

			case SDT_LLIST:
				if ( sd->tag == "Files" )
					ret = GetListData( m_aFile, x );
				else
					ret = GetListData( m_aList, x );
				break;

			case SDT_WLIST:
    			if ( sd->tag == CharFromCString(g_tagReviews) )
	    		{
		    		ret = GetListData( m_aReview, x );
					if(ret != DONE_FETCHING)
					{
                        // remove quotes, CR if present
                        if (!m_aStrGet.GetAt(0).IsEmpty() && (m_aStrGet.GetAt(0).GetAt(0) == _T('\"')))
                        {
                            m_aStrGet.ElementAt(0).TrimLeft(_T("\""));
                            m_aStrGet.ElementAt(0).TrimRight(_T("\r\""));
                        }
		    		}
		    	}
				else if ( sd->tag == "Jobs" )
                    ret = GetListData( m_aJobs, x );
    			else //if ( sd->tag == g_tagView )
	    			ret = GetListData( m_aWordList, x, nWords = _ttoi( m_nWords.GetAt ( i ) ) );
                break;

			default:
				TheApp()->StatusAdd( LoadStringResource(IDS_SPEC_WEIRDNESS_IN_SET), SV_DEBUG );
				break;
			}
            if(ret != DONE_FETCHING)
            {
#ifdef UNICODE
   				for ( int i = 0; i < m_aStrGetA.GetSize ( ) ; i++ )
				{
					delete m_aStrGetA.GetAt( i );
					m_aStrGetA.SetAt( i, 0 );
				}
#endif
                for(int w = 0; w < nWords; w++)
                {
#ifdef UNICODE
                    m_aStrGetA.SetAtGrow(w, new CharString(CharFromCString(m_aStrGet.GetAt(w))));
                    wv[w] = const_cast<char*>((const char*)*m_aStrGetA.GetAt(w));
#else                   
	    		    wv[w] = const_cast<char*>((const char*)m_aStrGet.GetAt(w));
#endif
                }
            }
            return ret;
		}
	}

	return ret;
}


/*
	_________________________________________________________________

	for list data the Spec::Fetch( ) will keep calling us until
	we tell it we are done and it has all the data. it gives us
	an index to tell us which element it wants. when we've hit the
	end of the array, tell the server by returning zero.
	_________________________________________________________________
*/

int CSpecData::GetListData( const CObArray &aList, int Serverindex, int nWords )
{
	if ( Serverindex < aList.GetSize( ) )
	{
		for ( int i = 0; i < nWords; i++ )
		{
			CStringArray *s = ( CStringArray * )aList.GetAt( Serverindex );
            ASSERT_KINDOF(CStringArray,s);
            CString str = s->GetAt( i );
            // remove any enclosing quotes
            str.TrimLeft(_T("\""));
            str.TrimRight(_T("\""));
            m_aStrGet.SetAtGrow(i, str);
		}
		return Serverindex + 1;
	}
	else
		return DONE_FETCHING;
}


/*
	_________________________________________________________________
*/

int CSpecData::GetListData( const CStringArray &aList, int Serverindex )
{
	if ( Serverindex < aList.GetSize( ) )
	{
		m_aStrGet.SetAtGrow(0, aList.GetAt( Serverindex ));
		return Serverindex + 1;
	}
	else
		return DONE_FETCHING;
}


/*
	_________________________________________________________________
*/

void CSpecData::UpdateReviews( const CString &s )
{
	int eol = 0;
	CString cst = s;

	m_aReview.RemoveAll( );

	while( cst.GetLength( ) > 0 )
	{
		eol= cst.Find(_T("\n"));
		if( eol == -1 ) // 1st hit end of text
		{
			m_aReview.Add( cst ) ;
			break;
		}
		else if( eol == 0 ) // 1st char was a \n
			cst = cst.Mid( 1 );
		else
		{
			m_aReview.Add( cst.Left( eol ) ) ;
			cst = cst.Mid( min( eol + 1, cst.GetLength( ) - 1 ) );
		}
	}
}


void CSpecData::UpdateLList( CStringArray &a_list, const CString &s )
{
	int eol = 0;
	CString cst = s;

	a_list.RemoveAll( );

	while( cst.GetLength( ) > 0 )
	{
		eol= cst.Find(_T("\n"));
		if( eol == -1 ) // 1st hit end of text
		{
			a_list.Add( cst ) ;
			break;
		}
		else if( eol == 0 ) // 1st char was a \n
			cst = cst.Mid( 1 );
		else
		{
			a_list.Add( cst.Left( eol ) ) ;
			cst = cst.Mid( min( eol + 1, cst.GetLength( ) - 1 ) );
		}
	}
}


/*
	_________________________________________________________________
*/

void CSpecData::UpdateWordList( int words, const CString &s )
{
	int eol = 0;
	CString cst = s;
	CString t;
	CleanUpArray( m_aWordList );


	while( cst.GetLength() > 0 )
	{
		//		get the view line by line
		//
		eol= cst.Find( g_CRLF );
		if(eol == -1) // 1st hit end of text
		{
			AddToWordList( words, cst );
			break;
		}
		else if(eol==0)
			cst = cst.Mid( 1 );
		else
		{
			t = cst.Left( eol );
			AddToWordList( words, t );
			cst = cst.Right( cst.GetLength( ) - t.GetLength( ) - 2 );
		}
	}
}


/*
	_________________________________________________________________
*/

void CSpecData::AddToWordList( int numWords, const CString &cst )
{
	int i;
	CStringArray *pstringarray ;
	CString s = cst;
	s.TrimLeft( );
	s.TrimRight( );
	s += _T(" ");
	CString t;
	pstringarray = new CStringArray;
	ASSERT ( pstringarray );
	for ( int j = 0; j < numWords; j++ )
	{
		if (s.GetAt(0) == _T('\"'))
		{
			if ((i = s.Find( _T('\"'), 1 )) == -1)
			{
				i = s.GetLength() - 2;
				j = numWords;
			}
			t = s.Left( i + 1 ) ;
			pstringarray->Add( t );
			t = s.Right( s.GetLength( ) - i - 1 );
			s = t;
			s.TrimLeft();
		}
		else
		{
			i = s.FindOneOf( _T(" \t") );
			if(i != -1)
				pstringarray->Add( s.Left(i) );
			else
				pstringarray->Add("");
			s = s.Right( s.GetLength( ) - i - 1 );
			s.TrimLeft();
		}
	}
	
	m_aWordList.Add( pstringarray );
}


/*
	_________________________________________________________________
*/

void CSpecData::CleanUpArray( CObArray &oa )
{
	for ( int i = 0; i < oa.GetSize ( ) ; i++ )
	{
		if ( oa.GetAt( i ) )
			delete oa.GetAt( i );
	}
	m_aWordList.RemoveAll( );
}


