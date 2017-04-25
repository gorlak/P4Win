//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// P4Object.cpp

#include "stdafx.h"
#include "p4win.h"
#include "P4Object.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CP4Object, CObject)


CP4Object::CP4Object()
{
	m_Initialized=FALSE;
}

CP4Object::~CP4Object()
{
	
}

void CP4Object::Create(LPCTSTR name)
{
	m_Field.Add(name);
	m_Count = 1;
	m_Initialized=TRUE;
}
