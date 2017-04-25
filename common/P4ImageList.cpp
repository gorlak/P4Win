#include "stdafx.h"
#include "P4ImageList.h"
#include "P4Image.h"

///////////////////////////////////////////////////////////////////////////
// CP4ImageList 

CP4ImageList::CP4ImageList()
	: m_disabled(0)
	, m_normalBmp(0)
	, m_disabledBmp(0)
	, m_disabledmenu(0)
	, m_disabledmenuBmp(0)
	, m_use256ColorIcons(false)
{
}

CP4ImageList::~CP4ImageList()
{
	if(m_disabledmenu)
		m_disabledmenu->Detach();
	delete m_disabledmenu;
	delete m_disabledmenuBmp;

	if(m_disabled)
		m_disabled->Detach();
	delete m_disabled;
	delete m_disabledBmp;

	Detach();
	delete m_normalBmp;
}

void CP4ImageList::OnSysColorChange(COLORREF bg)
{
	CDC dc;
	dc.CreateIC(_T("DISPLAY"), NULL, NULL, NULL);
	int newColorDepth = dc.GetDeviceCaps(BITSPIXEL);

	// if 16 colors, this is easy: just set the background color
	if(m_colorDepth == 4 && newColorDepth == m_colorDepth)
	{
		SetBkColor(bg);
		return;
	}

	// if xp and 32-bit color, nothing to do
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx (&osvi);

	if(osvi.dwMajorVersion >= 5 && osvi.dwMinorVersion >= 1 
		&& m_colorDepth == 32 && newColorDepth == 32)
		return;

	// all other cases require rebuilding images
	Detach();
	if(m_disabled)
		m_disabled->Detach();
	Create();
}

///////////////////////////////////////////////////////////////////////////
// CP4ToolBarImageList 

bool CP4ToolBarImageList::Create(int cx, int cy, int idNormal4, int idDisabled4, 
								 int idNormal32, COLORREF transparent4)
{
	CDC dc;
	dc.CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	m_colorDepth = dc.GetDeviceCaps(BITSPIXEL);

	// First create the disabled menu icons from the 4-bit bitmap
	if(idDisabled4)
	{
		CBitmap bmp;
		bmp.LoadBitmap(idDisabled4);
		if(!m_disabledmenu)
			m_disabledmenu = new CImageList();
		if(m_disabledmenu->Create(cx, cy, ILC_COLOR4|ILC_MASK, 5, 0))
			m_disabledmenu->Add(&bmp, transparent4);
	}

	if (m_colorDepth >= 16 && !m_use256ColorIcons)
	{
		OSVERSIONINFO osvi;
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx (&osvi);

		if(osvi.dwMajorVersion >= 5 && osvi.dwMinorVersion >= 1 && m_colorDepth == 32)
		{
			// xp can do it all for us; just add the bitmap with alpha
			// and toolbar will draw using the alpha, and create the
			// disabled image as well
			if(!CImageList::Create(cx, cy, ILC_COLOR32, 5, 0))
				return false;
			CP4ImageDib src(24);
			src.Load(idNormal32);
			CBitmap *bmp = src.CreateDDB(dc);
			if(bmp)
				Add(bmp, (CBitmap*)0);
			if(m_disabled)
			{
				delete m_disabled;
				m_disabled = 0;
			}
			return true;
		}

		if(!CImageList::Create(cx, cy, ILC_COLOR24|ILC_MASK, 5, 0))
			return false;
		// We can display enough colors, but the toolbar won't draw using
		// the alpha, and it can't produce a good disabled image, so we
		// 1. blend in the background color
		// 2. create a disabled image
		if(!m_disabled)
			m_disabled = new CImageList();
		if(!m_disabled->Create(cx, cy, ILC_COLOR24|ILC_MASK, 5, 0))
			return false;
		CP4ImageDib normal(cx);
		CP4ImageDib disabled(cx);
		if(normal.Load(idNormal32))
		{
			normal.BlendBackground(::GetSysColor(COLOR_3DFACE));
			if(disabled.CreateDisabled(normal, ::GetSysColor(COLOR_3DFACE)))
			{
				// replace transparent color with magenta because
				// ME & 98 have trouble with gray in 16-bit color

				COLORREF transparent = RGB(255,0,255);
				normal.SetTransparentColor(transparent);
				disabled.SetTransparentColor(transparent);

				CBitmap *normalBmp = normal.CreateDDB(dc);
				CBitmap *disabledBmp = disabled.CreateDDB(dc);
				if(normalBmp && disabledBmp)
				{
					Add(normalBmp, transparent);
					m_disabled->Add(disabledBmp, transparent);
					return true;
				}
			}
		}
		// something didn't work out, so fall through and use 16 colors
	}

	if(m_disabled)
	{
		delete m_disabled;
		m_disabled = 0;
	}

	// 256 or fewer colors device, so use 16 color bitmap
	if(!CImageList::Create(cx, cy, ILC_COLOR4 | ILC_MASK, 5, 0))
		return false;

	CBitmap bmp;
	bmp.Detach();
	bmp.LoadBitmap(idNormal4);
	Add(&bmp, transparent4);
	m_colorDepth = 4;
	return true;
}
