// P4Menu.h: interface for the CP4Menu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_P4MENU_H__36E8FE16_A6BC_11D4_8399_009027AF6042__INCLUDED_)
#define AFX_P4MENU_H__36E8FE16_A6BC_11D4_8399_009027AF6042__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CP4Menu : public CMenu  
{

public:
	CP4Menu();
	virtual ~CP4Menu();

	BOOL TrackPopupMenu(UINT nFlags, int x, int y,
						CWnd* pWnd, LPCRECT lpRect = 0);
};

#endif // !defined(AFX_P4MENU_H__36E8FE16_A6BC_11D4_8399_009027AF6042__INCLUDED_)
