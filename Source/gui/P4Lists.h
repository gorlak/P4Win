//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// P4Lists.h 
//

#ifndef __P4LISTS__
#define __P4LISTS__

#include <afxmt.h>
#include <afxcoll.h>


/////////////////////////////////////////////////////////////////////////////
// CRevision    Represents the info returned by one reply from "P4 filelog -l"

class CRevision : public CObject
{
// Construction
public:
	CRevision();
	DECLARE_DYNCREATE(CRevision)

// Attributes
public:
	int m_RevisionNum;
	int m_ChangeNum;
	int m_FNbr;
	CString m_Date;
	CString m_User;
	CString m_FName;
	CString m_ChangeType;
	CString m_ChangeDescription;

// Implementation
public:
	BOOL Create(LPCTSTR fname, int fnbr, LPCTSTR text, LPCTSTR branchInfo);
	virtual ~CRevision();
};

/////////////////////////////////////////////////////////////////////////////
// CHistory    A collection of CRevisions, to contain all results from "P4 filelog -l"

class CHistory
{
// Construction
public:
	CHistory();
	
// Attributes
protected:
	CObList* m_pList;
	CString m_FileName;
	POSITION m_pos;

// Operations
public:
	BOOL AddRevision(LPCTSTR fname, int fnbr, LPCTSTR desc, LPCTSTR branchInfo);
	CRevision *GetLatestRevision();
	CRevision *GetNextRevision();
	CRevision *GetHeadRevision();
	CRevision *GetPrevRevision();
	INT_PTR GetRevisionCount();
	CString GetFileName();
	void Clear();

public:

// Implementation
public:
	virtual ~CHistory();
	
};

/////////////////////////////////////////////////////////////////////////////
#endif // __P4LISTS__


