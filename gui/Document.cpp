//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Document.cpp : implementation of the CP4winDoc class
//

#include "stdafx.h"
#include "P4win.h"

#include "Document.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CP4winDoc

IMPLEMENT_DYNCREATE(CP4winDoc, CDocument)

BEGIN_MESSAGE_MAP(CP4winDoc, CDocument)
	//{{AFX_MSG_MAP(CP4winDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CP4winDoc construction/destruction

CP4winDoc::CP4winDoc()
{
	// TODO: add one-time construction code here

}

CP4winDoc::~CP4winDoc()
{
}

BOOL CP4winDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CP4winDoc serialization

void CP4winDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CP4winDoc diagnostics

#ifdef _DEBUG
void CP4winDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CP4winDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CP4winDoc commands
