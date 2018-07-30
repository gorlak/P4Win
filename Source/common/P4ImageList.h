#pragma once

class CP4ImageList : public CImageList
{
protected:
	CBitmap *m_normalBmp;
	CBitmap *m_disabledBmp;
	CBitmap *m_disabledmenuBmp;
	CImageList * m_disabled;
	CImageList * m_disabledmenu;
	bool m_blendBackground;
	int m_colorDepth;
	bool m_use256ColorIcons;

	bool BlendBackground(int nId, COLORREF bg);
	bool BlendDisabledOnly(int nId, COLORREF bg);

public:
	CP4ImageList();
	~CP4ImageList();
	virtual bool Create() = 0;

	// force use of 256 color icons; call before Create() to have effect
	void Use256ColorIcons() { m_use256ColorIcons = true; }

	// get the disabled image list, if any
	CImageList * GetDisabled() { return m_disabled; }
	CImageList * GetDisabledMenu() { return m_disabledmenu; }

	// color depth this image was created for
	int ColorDepth() const { return m_colorDepth; }

	void OnSysColorChange(COLORREF bg);
};

class CP4ToolBarImageList : public CP4ImageList
{
protected:
	bool Create(int cx, int cy, 
				int idNormal4, int idDisabled4, int idNormal32, 
				COLORREF transparent4);
};
