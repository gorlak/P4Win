// SortListHeader.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "SortListHeader.h"
#include "WindowsVersion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSortListHeader

CSortListHeader::CSortListHeader()
{
	m_SortColumn=-1;
}

CSortListHeader::~CSortListHeader()
{
}


BEGIN_MESSAGE_MAP(CSortListHeader, CHeaderCtrl)
	//{{AFX_MSG_MAP(CSortListHeader)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSortListHeader message handlers

int CSortListHeader::SetSortImage( int Column, BOOL Ascending )
{
	int PreviousColumn = m_SortColumn;

	m_SortColumn = Column;
	m_SortAscending = Ascending;

	// Change the previous item to not owner draw or sort
	HD_ITEM hditem;
	hditem.mask = HDI_FORMAT;
	GetItem(PreviousColumn, &hditem);
#if _MFC_VER >= 0x0700
	hditem.fmt &= ~(HDF_OWNERDRAW | HDF_SORTUP | HDF_SORTDOWN);
#else
	hditem.fmt &= ~(HDF_OWNERDRAW);
#endif
	SetItem(PreviousColumn, &hditem);

	// Change the new item to owner draw or sort
	GetItem(Column, &hditem);
#if _MFC_VER >= 0x0700
	if(TheApp()->GetWindowsVersion()->GetComCtl32Version() < PACKVERSION(6,00))
	{
		hditem.fmt |= HDF_OWNERDRAW;
	}
	else
	{
		if(Ascending)
			hditem.fmt |= HDF_SORTUP;
		else
			hditem.fmt |= HDF_SORTDOWN;
	}
#else
	hditem.fmt |= HDF_OWNERDRAW;
#endif
	SetItem( Column, &hditem );

	// Invalidate header control so that it gets redrawn
	Invalidate();
	return PreviousColumn;
}

void CSortListHeader::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	CDC dc;

	dc.Attach( lpDrawItemStruct->hDC );

	// Get the column rect
	CRect rcLabel( lpDrawItemStruct->rcItem );

	// Save DC
	int nSavedDC = dc.SaveDC();

	// Set clipping region to limit drawing within column
	CRgn rgn;
	rgn.CreateRectRgnIndirect( &rcLabel );
	dc.SelectObject( &rgn );
	rgn.DeleteObject();

    // Draw the background
    CBrush backBrush(::GetSysColor(COLOR_3DFACE));
    dc.FillRect(rcLabel, &backBrush);
	
	// The Labels are offset by a fixed number of pixels
	int offset = 6; 
	

	// Get the column text and format
	TCHAR buf[256];
	HD_ITEM hditem;
	
	hditem.mask = HDI_TEXT | HDI_FORMAT;
	hditem.pszText = buf;
	hditem.cchTextMax = 255;

	GetItem( lpDrawItemStruct->itemID, &hditem );

	// Determine format for drawing column label
	UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP 
						| DT_VCENTER | DT_END_ELLIPSIS ;

	if( hditem.fmt & HDF_CENTER)
		uFormat |= DT_CENTER;
	else if( hditem.fmt & HDF_RIGHT)
		uFormat |= DT_RIGHT;
	else
		uFormat |= DT_LEFT;

	// Adjust the rect if the mouse button is pressed on it
	if( lpDrawItemStruct->itemState == ODS_SELECTED )
	{
		rcLabel.left++;
		rcLabel.top += 2;
		rcLabel.right++;
	}

	// Adjust the rect further if Sort arrow is to be displayed
	if( lpDrawItemStruct->itemID == (UINT) m_SortColumn )
	{
		rcLabel.right -= 3 * offset;
	}

	rcLabel.left += offset;
	
	// Draw column label
	if( rcLabel.left < rcLabel.right )
		dc.DrawText(buf,-1,rcLabel, uFormat);

	// Draw the Sort arrow
	if( lpDrawItemStruct->itemID == (UINT) m_SortColumn )
	{
		CRect rcIcon( lpDrawItemStruct->rcItem );

		// Set up pens to use for drawing the triangle
		CPen penLight(PS_SOLID, 1, RGB(0xff, 0xff, 0xff));//GetSysColor(COLOR_3DHILIGHT));
		CPen penShadow(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
		CPen *pOldPen = dc.SelectObject( &penLight );

		CPoint v[3];

		// this drawing code is pretty ugly, but it seems to faithfully reproduce
		// the look of the arrows used by Explorer and Outlook.  The better way
		// might be to grab the bitmaps from shell32.dll (id# 133 & 134) and draw
		// the arrows using them.
		if( m_SortAscending )
		{
			v[0]= CPoint(rcIcon.right - (3+1)*offset/2, (rcIcon.bottom - offset)/2);
			v[1]= CPoint(rcIcon.right - (3+0)*offset/2, (rcIcon.bottom + offset)/2);
			v[2]= CPoint(rcIcon.right - (3+2)*offset/2, (rcIcon.bottom + offset)/2);

	
			// Draw triangle pointing upwards
			dc.MoveTo( v[2].x-1, v[2].y );
			dc.LineTo( v[1].x, v[1].y );
			dc.LineTo( v[0].x-1, v[0].y-1 );
			// draw again to fatten up the line
			dc.MoveTo( v[0].x, v[0].y );
			dc.LineTo( v[1].x+1, v[1].y+1 );
			
			dc.SelectObject( &penShadow );
			dc.MoveTo( v[2].x-1, v[2].y-1 );
			dc.LineTo( v[0].x, v[0].y-1 );
			// draw again to fatten up the line
			dc.MoveTo( v[0].x-2, v[0].y+1 );
			dc.LineTo( v[2].x, v[2].y );
		}
		else
		{
			v[0]= CPoint(rcIcon.right - (3+1)*offset/2, (rcIcon.bottom + offset)/2);
			v[1]= CPoint(rcIcon.right - (3+2)*offset/2, (rcIcon.bottom - offset)/2);
			v[2]= CPoint(rcIcon.right - (3+0)*offset/2, (rcIcon.bottom - offset)/2);

			// Draw triangle pointing downwards
						
			dc.SelectObject( &penShadow );
			dc.MoveTo( v[2].x, v[2].y );
			dc.LineTo( v[1].x-1, v[1].y );
			dc.LineTo( v[0].x, v[0].y+1 );
			// draw again to fatten up the line
			dc.MoveTo( v[0].x-1, v[0].y );
			dc.LineTo( v[1].x-2, v[1].y-1 );

			dc.SelectObject( &penLight );
			dc.MoveTo( v[2].x, v[2].y+1 );
			dc.LineTo( v[0].x-1, v[0].y+1 );
			// draw again to fatten up the line
			dc.MoveTo( v[0].x+1, v[0].y-1 );
			dc.LineTo( v[2].x-1, v[2].y );
		}


		// Restore the pen
		dc.SelectObject( pOldPen );
	}

	// Restore dc
	dc.RestoreDC( nSavedDC );

	// Detach the dc before returning
	dc.Detach();
}