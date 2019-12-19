#include "stdafx.h"
//#include "p4win.h"
#include "ImageList.h"
#include "P4Image.h"
#include "resource.h"

///////////////////////////////////////////////////////////////////////////
// CP4ToolBarImageList 

bool CP4WinToolBarImageList::Create()
{
	return CP4ToolBarImageList::Create(18, 16, 
		IDB_TOOLBAR4, IDB_BITMAPDIS, IDB_TOOLBAR32,
		RGB(255, 0, 255));
}

///////////////////////////////////////////////////////////////////////////
// CP4ViewIconsImageList 

// SrcImages represents the actual images in TreeIcons?.bmp
// it must be kept in sync with the bitmap.

// note: for built up images, base images are mixed in with badges, 
// but must precede badge images so that they will be drawn first 
// and badges drawn on top of them

enum SrcImages
{
	SI_LAST_SIMPLE = 19,

	// base images
	SI_CLIENT,
	SI_USER,
	SI_GHOST,
	SI_TEXT,
	SI_BINARY,

	// badge images
	SI_RED_DOC_BORDER,
	SI_YOUR_EDIT,
	SI_YOUR_DELETE,
	SI_YOUR_ADD,
	SI_THEIR_ADD,
	SI_THEIR_EDIT,
	SI_THEIR_DELETE,
	SI_YOUR_LOCK,
	SI_THEIR_LOCK,
	SI_SYNCED,
	SI_NOT_SYNCED,
	SI_CUR,
	SI_DEF,
	SI_BLANK,

	// more simple images
	SI_OPENFOLDER,
	SI_SHELVED,
};

// convert from CUSB enum flag to image index
static int CUSBToImage(int badge)
{
	static int map[][2] =
	{
		{CP4ViewImageList::CUSB_USER, SI_USER},
		{CP4ViewImageList::CUSB_CLIENT, SI_CLIENT},
		{CP4ViewImageList::CUSB_DEF,	SI_DEF},
		{CP4ViewImageList::CUSB_CUR, SI_CUR},
		{-1,-1}
	};

	for(int i = 0; map[i][0] != -1; i++)
		if(map[i][0] == badge)
			return map[i][1];
	ASSERT(0);
	return 0;
}

// convert from FSB enum flag to image index
static int FSBToImage(int badge)
{
	static int map[][2] =
	{
		{CP4ViewImageList::FSB_GHOST,		SI_GHOST},
		{CP4ViewImageList::FSB_TEXT,		SI_TEXT},
		{CP4ViewImageList::FSB_BINARY,		SI_BINARY},
		{CP4ViewImageList::FSB_YOUR_EDIT,	SI_YOUR_EDIT},
		{CP4ViewImageList::FSB_YOUR_DELETE,	SI_YOUR_DELETE},
		{CP4ViewImageList::FSB_YOUR_ADD,	SI_YOUR_ADD},
		{CP4ViewImageList::FSB_THEIR_EDIT,	SI_THEIR_EDIT},
		{CP4ViewImageList::FSB_THEIR_DELETE,SI_THEIR_DELETE},
		{CP4ViewImageList::FSB_THEIR_ADD,	SI_THEIR_ADD},
		{CP4ViewImageList::FSB_YOUR_LOCK,	SI_YOUR_LOCK},
		{CP4ViewImageList::FSB_THEIR_LOCK,	SI_THEIR_LOCK},
		{CP4ViewImageList::FSB_SYNCED,		SI_SYNCED},
		{CP4ViewImageList::FSB_NOT_SYNCED,	SI_NOT_SYNCED},
		{-1,-1}
	};

	for(int i = 0; map[i][0] != -1; i++)
		if(map[i][0] == badge)
			return map[i][1];
	ASSERT(0);
	return 0;
}

// lookup tables for finding an icon given it's state mask
static unsigned short fileMap[2 << CP4ViewImageList::FSB_NUM_BITS];
static unsigned short cuMap[2 << CP4ViewImageList::CUSB_NUM_BITS];

// return composite image index for client/user (CUSB) state flags
int CP4ViewImageList::GetCUIndex(int state)
{
	int result = cuMap[state];
	ASSERT(result);	// illegal combination state bits
	return result;
}

// return image index for client with specified attributes
int CP4ViewImageList::GetClientIndex(bool isCurrent, bool isDefault)
{
	return GetCUIndex(CUSB_CLIENT |
		(isDefault ? CUSB_DEF : 0) |
		(isCurrent ? CUSB_CUR : 0));
}

// return image index for user with specified attributes
int CP4ViewImageList::GetUserIndex(bool isCurrent, bool isDefault)
{
	return GetCUIndex(CUSB_USER |
		(isDefault ? CUSB_DEF : 0) |
		(isCurrent ? CUSB_CUR : 0));
}

// return composite image index for file state (FSB) flags
int CP4ViewImageList::GetFileIndex(int state)
{
	int result = fileMap[state];
	ASSERT(result);	// illegal combination state bits
	return result;
}

// create a composite client or user image
static void AddCUImage(CP4Image &dst, CP4Image &src, int &x, int layers)
{
	ASSERT(layers);
	ASSERT(x/24 < 0x10000);
	cuMap[layers] = (unsigned short)(x / 24);
	int mask = 1;
	while(layers)
	{
		if(layers & 1)
		{
			int img = CUSBToImage(mask);
			dst.BlendImage(x/24, src, img);
		}
		layers >>= 1;
		mask <<= 1;
	}
	x += 24;
}

// create a composite file image
static void AddFileImage(CP4Image &dst, CP4Image &src, int &x, int layers)
{
	ASSERT(layers);
	ASSERT(x/24 < 0x10000);
	fileMap[layers] = (unsigned short)(x / 24);
	bool red = (layers & CP4ViewImageList::FSB_YOUR_ADD) && 
		(layers & CP4ViewImageList::FSB_THEIR_ADD);
	int mask = 1;
	while(layers)
	{
		if(layers & 1)
		{
			int img = FSBToImage(mask);
			dst.BlendImage(x/24, src, img);
			// special case to get red border when you're not the
			// only one to have a file open for add
			if(red)
				dst.BlendImage(x/24, src, SI_RED_DOC_BORDER);
			red = false;
		}
		layers >>= 1;
		mask <<= 1;
	}
	x += 24;
}

// add a group of file images with non-add actions

static void AddFileImageSync(CP4Image &dst, CP4Image &src, int &x, int base)
{
	AddFileImage(dst, src, x, base);
	AddFileImage(dst, src, x, base | CP4ViewImageList::FSB_SYNCED);
	AddFileImage(dst, src, x, base | CP4ViewImageList::FSB_NOT_SYNCED);
}

static void AddFileImageLock(CP4Image &dst, CP4Image &src, int &x, int base)
{
	AddFileImageSync(dst, src, x, base);
	AddFileImageSync(dst, src, x, base | CP4ViewImageList::FSB_YOUR_LOCK);
	AddFileImageSync(dst, src, x, base | CP4ViewImageList::FSB_THEIR_LOCK);
}

static void AddFileImageTheirAction(CP4Image &dst, CP4Image &src, int &x, int base)
{
	AddFileImageLock(dst, src, x, base);
	AddFileImageLock(dst, src, x, base | CP4ViewImageList::FSB_THEIR_DELETE);
	AddFileImageLock(dst, src, x, base | CP4ViewImageList::FSB_THEIR_ADD);
	AddFileImageLock(dst, src, x, base | CP4ViewImageList::FSB_THEIR_EDIT);
}

static void AddFileImageYourAction(CP4Image &dst, CP4Image &src, int &x, int base)
{
	AddFileImageTheirAction(dst, src, x, base);
	AddFileImageTheirAction(dst, src, x, base | CP4ViewImageList::FSB_YOUR_DELETE);
	AddFileImageTheirAction(dst, src, x, base | CP4ViewImageList::FSB_YOUR_ADD);
	AddFileImageTheirAction(dst, src, x, base | CP4ViewImageList::FSB_YOUR_EDIT);
}

static void AddFileImageType(CP4Image &dst, CP4Image &src, int &x, int base)
{
	AddFileImageYourAction(dst, src, x, base | CP4ViewImageList::FSB_BINARY);
	AddFileImageYourAction(dst, src, x, base | CP4ViewImageList::FSB_TEXT);
}

// create the bitmap containing both simple and composite images
void CP4ViewImageList::MakeBitmap(CP4Image &dst, CP4Image &src, COLORREF bg)
{
	// first, set the whole thing to the background color
	dst.FillImage(bg);

	// next, just copy over the first segment of simple images
	int i;
	for(i = 0; i <= SI_LAST_SIMPLE; i++)
		dst.BlendImage(i, src, i);

	// add open folder icon
	dst.BlendImage(i++, src, SI_OPENFOLDER);

	// create the unresolved pending chglist ones
	dst.BlendImage(i,   src, CP4ViewImageList::VI_YOURCHANGE);
	dst.BlendImage(i++, src, SI_NOT_SYNCED);
	dst.BlendImage(i,   src, CP4ViewImageList::VI_YOUROTHERCHANGE);
	dst.BlendImage(i++, src, SI_NOT_SYNCED);

	// create the pending with shelved files chglist ones
	dst.BlendImage(i,   src, CP4ViewImageList::VI_YOURCHANGE);
	dst.BlendImage(i++, src, SI_SHELVED);
	dst.BlendImage(i,   src, CP4ViewImageList::VI_THEIRCHANGE);
	dst.BlendImage(i++, src, SI_SHELVED);

	// same, but for shelved + not synced case
	dst.BlendImage(i,   src, CP4ViewImageList::VI_YOURCHANGE);
	dst.BlendImage(i,   src, SI_SHELVED);
	dst.BlendImage(i++, src, SI_NOT_SYNCED);
	dst.BlendImage(i,   src, CP4ViewImageList::VI_THEIRCHANGE);
	dst.BlendImage(i,   src, SI_SHELVED);
	dst.BlendImage(i++, src, SI_NOT_SYNCED);

	int x = i * 24;

	// copy client, plus badges
	AddCUImage(dst, src, x, CUSB_CLIENT);
	AddCUImage(dst, src, x, CUSB_CLIENT | CUSB_CUR);
	AddCUImage(dst, src, x, CUSB_CLIENT | CUSB_DEF);
	AddCUImage(dst, src, x, CUSB_CLIENT | CUSB_CUR | CUSB_DEF);

	// copy user, plus badges
	AddCUImage(dst, src, x, CUSB_USER);
	AddCUImage(dst, src, x, CUSB_USER | CUSB_CUR);
	AddCUImage(dst, src, x, CUSB_USER | CUSB_DEF);
	AddCUImage(dst, src, x, CUSB_USER | CUSB_CUR | CUSB_DEF);

	// create all file images except the wierd one
	AddFileImageType(dst, src, x, 0);

	// create the wierd file one
	AddFileImage(dst, src, x, FSB_GHOST | FSB_SYNCED);
}

bool CP4ViewImageList::Create()
{
	// determine width required for bitmap
	int w = 24 * 
		(SI_LAST_SIMPLE + 1 +	// simple (non-composite) images
		4 +			// user + cur/def
		4 +			// client + cur/def
		2 *			// file types: binary, text
		4 *			// your action: none, add, edit, delete
		4 *			// their action: none, add, edit, delete
		3 *			// lock states: none, yours, theirs
		3 +			// sync states: none, synced, not synced
		1 +			// wierd ghost combo
		1 +			// open folder
		2 +			// 2 unresolved pending chglists
		2 +			// 2 unresolved + shelved pending changelists
		2 );		// your/their change with shelved files

	CDC dc;
	dc.CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	m_colorDepth = dc.GetDeviceCaps(BITSPIXEL);
	if (m_colorDepth >= 16 && !m_use256ColorIcons)
	{
		// at least 16-bit color, so create a device dependent bitmap
		// using 32-bit rgba source bitmap
		if(!CImageList::Create(24, 16, ILC_COLOR32|ILC_MASK, 5, 0))
			return false;


		CP4ImageDib src(24);
		CP4ImageDib dst(24);
		if(src.Load(IDB_VIEWICONS32))
		{
			src.PreMultiplyAlpha();
			dst.Create(w, 16);

			MakeBitmap(dst, src, GetSysColor(COLOR_WINDOW));
			CBitmap *bmp = dst.CreateDDB(dc);
			if(bmp)
				Add(bmp, GetSysColor(COLOR_WINDOW));
			return true;
		}
		// something didn't work out, so fall through and use 16 colors
	}

	// 256 or fewer colors device, so use 16 color bitmap
	if(!CImageList::Create(24, 16, ILC_COLOR4|ILC_MASK, 5, 0))
		return false;

	// use magenta for background; it will be transparent
	// (using non-transparent COLOR_WINDOW background fails 
	// sometimes in 256 color mode)
	COLORREF magenta = RGB(255,0,255);
	CP4Image16 src(magenta, 24);
	CP4Image16 dst(magenta, 24);
	if(src.Load(IDB_VIEWICONS4) && dst.Create(dc, w, 16))
	{
		src.StartBlendingSource();
		dst.StartBlendingDest(dc);

		MakeBitmap(dst, src, magenta);

		dst.EndBlendingDest();
		src.EndBlendingSource();

		CBitmap *bmp = dst.CreateDDB(dc);
		if(bmp)
			Add(bmp, magenta);
		m_colorDepth = 4;
		return true;
	}
	return false;
}