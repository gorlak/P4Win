// FlatSplitter.cpp : implementation file
//

#include "stdafx.h"
#include "FlatSplitter.h"
#include <..\\src\\mfc\\afximpl.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFlatSplitter

IMPLEMENT_DYNCREATE(CFlatSplitter, CSplitterWnd)

BEGIN_MESSAGE_MAP(CFlatSplitter, CSplitterWnd)
END_MESSAGE_MAP()


CFlatSplitter::CFlatSplitter()
{ 
    
}

CFlatSplitter::~CFlatSplitter()
{                 
 
}

#ifndef CX_BORDER
#define CX_BORDER 1
#endif

#ifndef CY_BORDER
#define CY_BORDER 1
#endif


void CFlatSplitter::OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rectArg)
{
	// This is a modification of CSplitterWnd::OnDrawSplitter()
	// Unfortunately, CSplitterWnd borders aren't configurable, so this is
	// the only way to get a flatter look to the splitters.

	// if pDC == NULL, then just invalidate
	if (pDC == NULL)
	{
		RedrawWindow(rectArg, NULL, RDW_INVALIDATE|RDW_NOCHILDREN);
		return;
	}
	ASSERT_VALID(pDC);

	// otherwise, actually draw
	CRect rect = rectArg;
	switch (nType)
	{
	case splitBorder:
		pDC->Draw3dRect(rect, afxData.clrBtnFace, afxData.clrBtnFace);
		rect.InflateRect(-CX_BORDER, -CY_BORDER);
		pDC->Draw3dRect(rect, afxData.clrBtnShadow, afxData.clrBtnHilite);
		return;

	case splitIntersection:
		break;

	case splitBox:
		pDC->Draw3dRect(rect, afxData.clrBtnShadow, afxData.clrBtnHilite);
		rect.InflateRect(-CX_BORDER, -CY_BORDER);
		pDC->Draw3dRect(rect, afxData.clrBtnFace, afxData.clrBtnFace);
		rect.InflateRect(-CX_BORDER, -CY_BORDER);
		break;

	case splitBar:
		break;

	default:
		ASSERT(FALSE);  // unknown splitter type
	}

	// fill the middle
	COLORREF clr = afxData.clrBtnFace;
	pDC->FillSolidRect(rect, clr);
}
