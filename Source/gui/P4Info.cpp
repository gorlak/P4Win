//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// P4Info.cpp
#include "stdafx.h"
#include "P4Info.h"
#include "StringUtil.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Class CP4info -  a simple class to parse and hold info returned  by 'P4 info'

CP4Info::CP4Info()
{
}

void CP4Info::SetPort(LPCTSTR port)
{
	m_ServerAddress=port;
}

void CP4Info::SetClient(LPCTSTR client)
{
	m_ClientName=client;
}

void CP4Info::SetUser(LPCTSTR user)
{
	m_UserName=user;
}
