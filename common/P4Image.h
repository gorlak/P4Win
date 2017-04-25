#pragma once

///////////////////////////////////////////////////////////////////////////
// 
class CP4Image
{
protected:
	int m_cellWidth;
	int m_w;
	int m_h;
	CBitmap m_bmp;
public:
	CP4Image(int cellWidth)
		: m_cellWidth(cellWidth)
	{
	}
	
	// fill the image with a solid color
	virtual void FillImage(COLORREF clr) = 0;
	
	// blend an overlay image onto this image
	// this uses transparency or alpha
	virtual	void BlendImage(int di, CP4Image &src, int si) = 0;
	
	// create a device dependent bitmap from this image
	virtual CBitmap * CreateDDB(CDC &dc) = 0;
};

///////////////////////////////////////////////////////////////////////////
// CP4Image4 - support for 16 color images

class CP4Image16 : public CP4Image
{
	CBitmap m_bmp;
	BITMAP m_bm;
	CDC * m_dc;
	CImageList *m_imgList;
	CBitmap * m_oldBmp;
	COLORREF m_chromaKey;
public:
	CP4Image16(COLORREF chromaKey, int cellWidth)
		: CP4Image(cellWidth)
		, m_chromaKey(chromaKey)
	{
		m_dc = 0;
	}
	~CP4Image16()
	{
		delete m_dc;
	}
	
	// initialize from resource
	bool Load(int nId);

	// create a new image with uninitialized pixels
	bool Create(CDC &dc, int w, int h);

	// all calls to BlendImage should be wrapped by
	// pairs of calls to these functions
	void StartBlendingSource(); 
	void EndBlendingSource(); 
	void StartBlendingDest(CDC &dc); 
	void EndBlendingDest(); 
	
	virtual void FillImage(COLORREF clr);
	virtual	void BlendImage(int di, CP4Image &src, int si);
	virtual CBitmap * CreateDDB(CDC &dc);
};

///////////////////////////////////////////////////////////////////////////
// CP4ImageDib - support for 32 bit rgba images

class CP4ImageDib : public CP4Image
{
	BITMAPINFO m_bmi;
	unsigned char * m_bits;
public:
	CP4ImageDib(int cellWidth)
		: CP4Image(cellWidth)
		, m_bits(0)
	{
	}

	~CP4ImageDib()
	{
		delete m_bits;
	}

	// initialize from resource
	bool Load(int nId);

	// create a new image with uninitialized pixels
	bool Create(int w, int h);

	virtual void FillImage(COLORREF clr);
	virtual void BlendImage(int di, CP4Image &src, int si);
	virtual CBitmap * CreateDDB(CDC &dc);

	// premultiply alpha into image in preparation for blending images
	void PreMultiplyAlpha();

	// blend a background color into image using image's alpha
	void BlendBackground(COLORREF bg);

	// create a new disable image from an existing image
	bool CreateDisabled(CP4ImageDib &src, COLORREF bg);

	// replace the color of the transparent pixels
	void SetTransparentColor(COLORREF t);
};
