// P4ListBox.cpp : implementation file
//

#include "stdafx.h"
#include "P4win.h"


// CP4ListBox

IMPLEMENT_DYNAMIC(CP4ListBox, CListBox)
CP4ListBox::CP4ListBox()
{
	width_ = 0;
	m_RightClkCallback = NULL;
}

CP4ListBox::~CP4ListBox()
{
}


BEGIN_MESSAGE_MAP(CP4ListBox, CListBox)
	ON_WM_RBUTTONUP( )
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// Public methods
//////////////////////////////////////////////////////////////////////
int CP4ListBox::AddString(LPCTSTR s)
{
	int result = CListBox::AddString(s);
	if (result < 0)
	{
		return result;
	}
	update_width(s);
	return result;
}
int CP4ListBox::InsertString(int i, LPCTSTR s)
{
	int result = CListBox::InsertString(i, s);
	if (result < 0)
	{
		return result;
	}
	update_width(s);
	return result;
}
void CP4ListBox::ResetContent()
{
	CListBox::ResetContent();
	width_ = 0;
}
int CP4ListBox::DeleteString(int n)
{
	int result = CListBox::DeleteString(n);
	if (result < 0)
	{
		return result;
	}
	CClientDC dc(this);
	CFont * f = CListBox::GetFont();
	dc.SelectObject(f);
	width_ = 0;
	for(int i = 0; i < CListBox::GetCount(); i++)
	{
		CString s;
		CListBox::GetText(i, s);
		CSize sz = dc.GetTextExtent(s);
		sz.cx += 3*::GetSystemMetrics(SM_CXBORDER) + 20;
		if (sz.cx > width_)
		{
			width_ = sz.cx;
		}
	}
	CListBox::SetHorizontalExtent(width_);
	return result;
}
int CP4ListBox::get_width() const
{
	return width_;
}
//////////////////////////////////////////////////////////////////////
// Private methods
//////////////////////////////////////////////////////////////////////
void CP4ListBox::update_width(LPCTSTR s)
{
	CClientDC dc(this);
	CFont *f = CListBox::GetFont();
	dc.SelectObject(f);
	CSize sz = dc.GetTextExtent(s, _tcslen(s));
	sz.cx += 3*::GetSystemMetrics(SM_CXBORDER) + 20;
	if (sz.cx > width_)
	{
		width_ = sz.cx;
		CListBox::SetHorizontalExtent(width_);
	}
}

void CP4ListBox::OnRButtonUp( UINT flags, CPoint pt )
{
	if (m_RightClkCallback)
		(m_RightClkCallback)(flags, pt);
}
