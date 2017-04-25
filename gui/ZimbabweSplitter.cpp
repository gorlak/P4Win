//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// ZimbabweSplitter.cpp : implementation file
//

#include "stdafx.h"
#include "zimbabwesplitter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CZimbabweSplitter

IMPLEMENT_DYNCREATE(CZimbabweSplitter, CFlatSplitter)

BEGIN_MESSAGE_MAP(CZimbabweSplitter, CFlatSplitter)
END_MESSAGE_MAP()

CZimbabweSplitter::CZimbabweSplitter()
{ 
	m_Sizing=FALSE;
}

CZimbabweSplitter::~CZimbabweSplitter()
{                 
 
}

// RecalcLayout is called once during construction.  After that, there are two ways
// it can be called:
// 1) if the splitter is adjusted by mouse action, m_Sizing will not be true, and we 
//	  will record the size of the lower pane that the user has selected.  
// 2) if the entire splitter is resized, in CMainFrame::OnSize() it is possible to
//    call SetSizing() for the splitter so we know not to record the dimensions
//    of the lower splitter pane
// 
// This isnt the slickest way to remember the relative sizes size of the bottom pane
// and the whole window, but OnSize() is not a virtual function
void CZimbabweSplitter::RecalcLayout()
{
	ASSERT(GetRowCount()==2); // This is not a generic class - see header file

	CFlatSplitter::RecalcLayout();

	if(!m_Sizing)
	{
		// Should only wind up in this version of recalc if user moved splitter
		int min;
		GetRowInfo(0, m_cyCur0, min);
		GetRowInfo(1, m_cyCur1, min);
		GetClientRect(&m_ZimbabweRect);
		m_BarAndBorders = m_ZimbabweRect.bottom - m_cyCur0 - m_cyCur1;    
    }
}
   

// After the splitter window has been resized, calculate what size the upper pane must be
// in order to display the lower pane as a percentage of the last recorded user-set height
void CZimbabweSplitter::Resized()
{
	RECT rect;
	GetClientRect(&rect);

	// Set minimum row heights for main horizontal splitter
	SetRowInfo(0, max(0, rect.bottom - m_cyCur1 - m_BarAndBorders), 0);
	SetRowInfo(1, m_cyCur1, 0);

	// Call the base class for recalc, since we dont want to store a pane size set
	// via overal splitter window size
	CFlatSplitter::RecalcLayout();
}
