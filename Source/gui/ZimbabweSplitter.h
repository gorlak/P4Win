//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// ZimbabweSplitter.h : 

// Splitter to remember the height of the lowest splitter pane, even if
// the splitter is sized smaller than that pane.  This class is called
// a Zimbabwe splitter because it is not in any way generic.  It is intended
// only for use with a horizontal two pane splitter.  Making this class
// generic is left as an exercise for someone with spare time.


#ifndef __ZIMBABWESPLITTER__
#define __ZIMBABWESPLITTER__	
/////////////////////////////////////////////////////////////////////////////
// CSplitter frame with splitter

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "FlatSplitter.h"

class CZimbabweSplitter : public CFlatSplitter
{
	DECLARE_DYNCREATE(CZimbabweSplitter)

public:
   	CZimbabweSplitter();
	
	RECT m_ZimbabweRect;
	int m_BarAndBorders;
	int m_cyCur0;
	int m_cyCur1;
	BOOL m_Sizing;

// Operations
public:
    void RecalcLayout();
	void Resized();
	void SetSizing(BOOL sizing) { m_Sizing= sizing; }

// Implementation
public:
	virtual ~CZimbabweSplitter();

	DECLARE_MESSAGE_MAP()
};

#endif //__ZIMBABWESPLITTER__	
/////////////////////////////////////////////////////////////////////////////
