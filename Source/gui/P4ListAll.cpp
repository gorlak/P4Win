// P4ListAll.cpp: implementation of the P4ListAll class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "p4win.h"
#include "P4ListAll.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

P4ListAll::P4ListAll()
{

}

P4ListAll::~P4ListAll()
{

}

void P4ListAll::RemoveAll()
{
	for (int i = -1; ++i < MAX_LISTALL_COL; )
		column[i].RemoveAll();
}
