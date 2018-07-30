// P4ListAll.h: interface for the P4ListAll class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_P4LISTALL_H_INCLUDED_)
#define AFX_P4LISTALL_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define	MAX_LISTALL_COL	8

class P4ListAll  
{
public:
	P4ListAll();
	virtual ~P4ListAll();

	CStringArray column[MAX_LISTALL_COL];

	void RemoveAll();
};

#endif // !defined(AFX_P4LISTALL_H_INCLUDED_)
