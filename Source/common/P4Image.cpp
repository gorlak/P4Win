#include "stdafx.h"
#include "P4Image.h"

///////////////////////////////////////////////////////////////////////////
// CP4Image16 - support for 16 color images

bool CP4Image16::Load(int nId)
{
	if(!m_bmp.LoadBitmap(nId))
		return false;
	m_bmp.GetBitmap(&m_bm);
	m_w = m_bm.bmWidth;
	m_h = m_bm.bmHeight;
	return true;
}

bool CP4Image16::Create(CDC &dc, int w, int h)
{
	if(!m_bmp.CreateCompatibleBitmap(&dc, w, h))
		return false;
	m_w = w;
	m_h = h;
	return true;
}

void CP4Image16::StartBlendingSource() 
{
	m_imgList = new CImageList();
	m_imgList->Create(m_cellWidth, m_h, ILC_COLOR4|ILC_MASK, 1, 1);
	m_imgList->Add(&m_bmp, m_chromaKey);
}

void CP4Image16::EndBlendingSource() 
{
	delete m_imgList;
	m_imgList = 0;
}

void CP4Image16::StartBlendingDest(CDC &dc) 
{
	m_dc = new CDC;
	m_dc->CreateCompatibleDC(&dc);
	m_oldBmp = m_dc->SelectObject(&m_bmp);
}

void CP4Image16::EndBlendingDest() 
{
	m_dc->SelectObject(m_oldBmp);
	m_oldBmp = 0;
	delete m_dc;
	m_dc = 0;
}

void CP4Image16::FillImage(COLORREF clr)
{
	CBrush bgbr;
	bgbr.CreateSolidBrush(clr);
	m_dc->FillRect(CRect(0, 0, m_w, m_h), &bgbr);
}

void CP4Image16::BlendImage(int di, CP4Image &src, int si)
{
	// Blending is done using an ImageList rather than just
	// using TransparentBlt, because TransparentBlt isn't
	// available on win95 or nt4
	CP4Image16 * src4 = dynamic_cast<CP4Image16*>(&src);
	ASSERT(src4);
	ASSERT(src4->m_cellWidth == m_cellWidth);
	src4->m_imgList->Draw(m_dc, si, CPoint(di * m_cellWidth, 0), ILD_TRANSPARENT);
}

CBitmap * CP4Image16::CreateDDB(CDC &/*dc*/) 
{ 
	return &m_bmp; 
}

///////////////////////////////////////////////////////////////////////////
// CP4ImageDib - support for 32 bit rgba images

bool CP4ImageDib::Load(int nId)
{
	// You might think it would work to just call LoadImage
	// to load a bitmap image.  But apparently some versions
	// of Windows will be confused and load a 32-bit image
	// incorrectly for some mysterious reason.  So we do it
	// the slightly harder way here:

	HRSRC hRes = FindResource(AfxGetResourceHandle(), 
		MAKEINTRESOURCE(nId), RT_BITMAP);
	if(hRes == NULL)
		return false;
	HGLOBAL hResLoad = LoadResource(AfxGetResourceHandle(), hRes);
	if(hResLoad == NULL)
		return false;
	char * lpResLock = (char*)LockResource(hResLoad); 
	if (lpResLock == NULL) 
		return false;

	BITMAPINFOHEADER * bmih = (BITMAPINFOHEADER*)lpResLock;

	if(bmih->biBitCount != 32 ||
		bmih->biPlanes != 1)
	{
		return false;
	}

	// create a dib using the dib section and copy the bits
	m_bmi.bmiHeader = *bmih;

	m_w = m_bmi.bmiHeader.biWidth;
	m_h = m_bmi.bmiHeader.biHeight;
	m_bmi.bmiHeader.biSizeImage = m_w * m_h * 4;

	m_bits = new unsigned char [m_bmi.bmiHeader.biSizeImage];
	memcpy(m_bits, lpResLock + bmih->biSize, m_bmi.bmiHeader.biSizeImage);

	return true;
}

bool CP4ImageDib::Create(int w, int h)
{
	// set up the DIB
	m_bmi.bmiHeader.biSize = sizeof(m_bmi.bmiHeader);
	m_bmi.bmiHeader.biWidth = w;
	m_bmi.bmiHeader.biHeight = h;
	m_bmi.bmiHeader.biPlanes = 1;
	m_bmi.bmiHeader.biBitCount = 32;
	m_bmi.bmiHeader.biCompression = BI_RGB;
	m_bmi.bmiHeader.biSizeImage = w * h * 4;
	m_bmi.bmiHeader.biXPelsPerMeter = 0;
	m_bmi.bmiHeader.biYPelsPerMeter = 0;
	m_bmi.bmiHeader.biClrUsed = 0;
	m_bmi.bmiHeader.biClrImportant = 0;

	m_w = m_bmi.bmiHeader.biWidth;
	m_h = m_bmi.bmiHeader.biHeight;

	m_bits = new unsigned char [m_bmi.bmiHeader.biSizeImage];
	return true;
}

void CP4ImageDib::FillImage(COLORREF clr)
{
	for(int r = 0; r < m_h; r++)
	{
		unsigned char *pd = m_bits + r * m_w * 4;
		for(int c = 0; c < m_w; c++, pd += 4)
		{
			pd[0] = GetBValue(clr);
			pd[1] = GetGValue(clr);
			pd[2] = GetRValue(clr);
			pd[3] = 0;
		}
	}
}

void CP4ImageDib::BlendImage(int di, CP4Image &src, int si)
{
	CP4ImageDib * dib = dynamic_cast<CP4ImageDib*>(&src);
	ASSERT(dib);
	if(!dib)
		return;
	for(int r = 0; r < m_h; r++)
	{
		unsigned char *ps = dib->m_bits + r * dib->m_w * 4 + si * m_cellWidth * 4;
		unsigned char *pd = m_bits + r * m_w * 4 + di * m_cellWidth * 4;
		for(int c = 0; c < m_cellWidth; c++, ps += 4, pd += 4)
		{
			int alpha = ps[3];
			if(!alpha)
				continue;

			// blend source bitmap with background color using source alpha
			pd[0] = (unsigned char)(ps[0] + ((255 - alpha) * pd[0] >> 8));
			pd[1] = (unsigned char)(ps[1] + ((255 - alpha) * pd[1] >> 8));
			pd[2] = (unsigned char)(ps[2] + ((255 - alpha) * pd[2] >> 8));
			pd[3] = (unsigned char)(ps[3] + ((255 - alpha) * pd[3] >> 8));
		}
	}
}

CBitmap * CP4ImageDib::CreateDDB(CDC &dc)
{
	HBITMAP hBitmap = ::CreateDIBitmap(dc.m_hDC, &m_bmi.bmiHeader,
		CBM_INIT, m_bits, &m_bmi, DIB_RGB_COLORS);
	m_bmp.Attach(hBitmap);
	return &m_bmp;
}

void CP4ImageDib::PreMultiplyAlpha()
{
	for(int r = 0; r < m_h; r++)
	{
		unsigned char *ps = m_bits + r * m_w * 4;
		for(int c = 0; c < m_w; c++, ps += 4)
		{
			// pre-multiply alpha into source bitmap
			int alpha = ps[3];
			ps[0] = (unsigned char)((ps[0] * alpha) >> 8);
			ps[1] = (unsigned char)((ps[1] * alpha) >> 8);
			ps[2] = (unsigned char)((ps[2] * alpha) >> 8);
		}
	}
}

void CP4ImageDib::BlendBackground(COLORREF bg)
{
	// set up some colors
	unsigned char bgb = GetBValue(bg);
	unsigned char bgg = GetGValue(bg);
	unsigned char bgr = GetRValue(bg);
	unsigned ubg = GetBValue(bg) | (GetGValue(bg)<<8) | (GetRValue(bg)<<16);

	for(int r = 0; r < m_h; r++)
	{
		unsigned char *ps = m_bits + r * m_w * 4;
		for(int c = 0; c < m_w; c++, ps += 4)
		{
			int alpha = ps[3];
			if(!alpha)
			{
				// special case for totally transparent parts
				*((unsigned*)ps) = ubg;
				continue;
			}

			// blend source bitmap with background color using source alpha
			ps[0] = (unsigned char)((bgb * (255 - alpha) + ps[0] * alpha) >> 8);
			ps[1] = (unsigned char)((bgg * (255 - alpha) + ps[1] * alpha) >> 8);
			ps[2] = (unsigned char)((bgr * (255 - alpha) + ps[2] * alpha) >> 8);
			ps[3] = 255;
		}
	}
}

bool CP4ImageDib::CreateDisabled(CP4ImageDib &src, COLORREF bg)
{
	if(!Create(src.m_w, src.m_h))
		return false;

	// To create the disabled image, convert to a 'grayscale'
	// using a range from COLOR_3DHILIGHT to COLOR_3DDKSHADOW
	// This produces a good approximation to what winxp does.
	COLORREF shadow = ::GetSysColor(COLOR_3DDKSHADOW);
	COLORREF hilight = ::GetSysColor(COLOR_3DHILIGHT);
	unsigned char bgb = GetBValue(bg);
	unsigned char bgg = GetGValue(bg);
	unsigned char bgr = GetRValue(bg);
	unsigned char sb = GetBValue(shadow);
	unsigned char sg = GetGValue(shadow);
	unsigned char sr = GetRValue(shadow);
	unsigned char hb = GetBValue(hilight);
	unsigned char hg = GetGValue(hilight);
	unsigned char hr = GetRValue(hilight);
	unsigned ubg = GetBValue(bg) | (GetGValue(bg)<<8) | (GetRValue(bg)<<16);

	for(int r = 0; r < m_h; r++)
	{
		unsigned *ps = (unsigned *)src.m_bits + r * m_w;
		unsigned *pd = (unsigned *)m_bits + r * m_w;
		for(int c = 0; c < m_w; c++, ps++, pd++)
		{
			if(*ps == ubg)
			{
				*pd = ubg;
				continue;
			}

			// get source image components
			unsigned nb = *ps & 0xff;
			unsigned ng = (*ps >> 8) & 0xff;
			unsigned nr = (*ps >> 16) & 0xff;
			unsigned alpha = (*ps >> 24) & 0xff;

			// calculate luminance.  This is what is used to determine
			// where in the monochrome range the color maps.
			double luminance = (0.299 * nr + 0.587 * ng + 0.114 * nb)/255; 
			if(luminance > 1.0)
				luminance = 1.0;

			// map from color to monochrome
			unsigned char b = (unsigned char)(sb + luminance * (hb - sb));
			unsigned char g = (unsigned char)(sg + luminance * (hg - sg));
			unsigned char r = (unsigned char)(sr + luminance * (hr - sr));
			// blend with the background color
			b = (unsigned char)((bgb * (255 - alpha) + b * alpha) >> 8);
			g = (unsigned char)((bgg * (255 - alpha) + g * alpha) >> 8);
			r = (unsigned char)((bgr * (255 - alpha) + r * alpha) >> 8);
			// and put it away
			pd[0] = b | (g << 8) | (r << 16) | 0xff000000;
		}
	}
	return true;
}

void CP4ImageDib::SetTransparentColor(COLORREF t)
{
	unsigned clr = GetBValue(t) | (GetGValue(t)<<8) | (GetRValue(t)<<16);

	for(int r = 0; r < m_h; r++)
	{
		unsigned char *ps = m_bits + r * m_w * 4;
		for(int c = 0; c < m_w; c++, ps += 4)
		{
			int alpha = ps[3];
			if(!alpha)
			{
				// replace color only for totally transparent pixels
				*((unsigned*)ps) = clr;
			}
		}
	}
}
