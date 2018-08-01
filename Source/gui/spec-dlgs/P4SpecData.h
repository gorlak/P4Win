//
// 
// Copyright 1999 Perforce Software.  All rights reserved.
//
// This file is part of Perforce - the FAST SCM System.
//
//

#ifndef __P4SPECDATA__
#define __P4SPECDATA__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "spec.h"

//		these are the 98.2 format specs describing 97.3 data.
//		98.2 doesn't need these at all, since the server returns them to me.
//		but pre 98.2 sends either nothing or garbage, so put in the
//		old data in the new format.
//
extern const char * BRANCH_SPEC;
extern const char * CHANGE_SPEC;
extern const char * CLIENT_SPEC;
extern const char * DEPOT_SPEC;
extern const char * LABEL_SPEC;
extern const char * USER_SPEC;
extern const char * JOB_SPEC;
extern const char * GROUP_SPEC;

class CSpecData : public SpecData
{

public:
	CSpecData ( ) { m_PrevWid = _T('A');}
	virtual ~CSpecData ();

    // SpecData overrides
	virtual int  Get( SpecElem *sd, int x, char **wv, char **cmt ) ;
	virtual void Set( SpecElem *sd, int x, char **wv, Error *e ) ;

	int AddElem( const CString &tag, const CString &value
					   , const CString &sInsertAfter, int SDTtype
					   , BOOL bisReadOnly, BOOL bRequired
					   , int maxLen, int code, int nWords
					   , const CString &format );
	int AddElemForEmptyItems( );
	int AddElem( const CString &tag, const CString &value
					   , int iInsertAt, int SDTtype
					   , BOOL bisReadOnly, BOOL bRequired
					   , int maxLen, int code, int nWords
					   , const CString &format );

public:
	INT_PTR GetNumItems( ) 
		{ return m_asSpecElems.GetSize( ) ; }

	CString GetTagOf( int i )
		{ return m_aTags.GetAt( i ) ; }

	int GetSizeOf( int i )
		{ return _ttoi( m_maxLength.GetAt( i ) ); } 

	BOOL GetReadOnlyOf( int i )
		{ return _ttoi( m_isReadOnly.GetAt( i ) ); }

	int GetCodeOf( int i )
		{ return _ttoi( m_code.GetAt( i ) ); }

	int GetNWordsOf( int i )
		{ return _ttoi( m_nWords.GetAt( i ) ); }

	int GetTypeOf( int i )
		{ return _ttoi( m_type.GetAt( i ) ); }

	int GetRequiredOf( int i )
		{ return _ttoi( m_required.GetAt( i ) ); }

	CString GetIndentOf( int i )
		{ return m_indent.GetAt( i ) ; }

	void SetIndentOf( int which, const CString &indent )
		{ m_indent.SetAt( which, indent ) ; }

	CString GetwCodeOf( int i )
		{ return m_wCode.GetAt( i ) ; }

	void SetwCodeOf( int which, const CString &wcode )
		{ m_wCode.SetAt( which, wcode ) ; }

	int GetLiOffsetOf( int i )
		{ return _ttoi( m_liOffset.GetAt( i ) ); }

	void SetIsReadOnlyOf( int which, BOOL isReadOnly )
		{ CString s; s.Format( _T("%d"), ( int ) isReadOnly ); m_isReadOnly.SetAtGrow( which, s ) ; }

	void SetValueOf( int which, const CString &value )
		{ m_aValues.SetAtGrow( which, value ) ; }

	LPCTSTR GetValueOf( int which )
		{ return m_aValues.GetAt( which ) ; }

	int GetListData( const CStringArray &aList, int index );
	int GetListData( const CObArray &aList, int Serverindex, int nWords );

	void UpdateLList( CStringArray &a_list, const CString &s );
	void UpdateReviews( const CString &cst );
	void UpdateWordList( int i, const CString &cst );
	void AddToWordList( int i, const CString &cst );

//fanny: make these private

//change the above to ints. right now i'm too lazy.
//well, why not leave them...

private:
    friend class CP4SpecDlg;

	CStringArray m_aView;
	CStringArray m_aList;
	CStringArray m_aFile;
	CStringArray m_aReview;
	CStringArray m_aJobs;
	CObArray m_aWordList;
	TCHAR m_PrevIndent;
	TCHAR m_PrevWid;

	CString GetSelectedValues( const CString &sSpecElems );
	int GetElemValue( const CString &sSpecElems, const CString &el );
	CString GetPresetValue( const CString &sSpecElems );
	void AddElem( const CString &tag, const CString &value
		, int isReadOnly, int required, int maxLength
		, int code, int type, int nWords, const CString &format );

    // used to hold value, possibly trimmed of quotes, being returned by Get()
    CStringArray m_aStrGet;
#ifdef UNICODE
    CTypedPtrArray<CPtrArray,CharString*> m_aStrGetA;
#endif

	CStringArray m_aValues;
	CStringArray m_aTags;
	CStringArray m_maxLength;
	CStringArray m_code;
	CStringArray m_type;
	CStringArray m_isReadOnly;
	CStringArray m_nWords;
	CStringArray m_required;
	CStringArray m_indent;
	CStringArray m_wCode;
	CStringArray m_liOffset;

	void CleanUpArray( CObArray &oa );
	CStringArray m_asSpecElems;
	int m_numParsed;
	int m_ChkBoxCtr;
	BOOL UnFilledElem( const CString &oldtag, const CString &tag );

public:
	void SetSpecElems( const CString &sSpecElems );
	void CheckForStragglers( );
};

#endif // __P4SPECDATA__
