// CoolButton.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "CoolBtn.h"
#if _MFC_VER >= 0x0700
#define USE_THEMES
#include <uxtheme.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int nDropBtnWidth =  16;
/////////////////////////////////////////////////////////////////////////////
// CCoolButton
// ========
// 
// To Use:
// 1.	In the RC file, create the button as BS_OWNERDRAW.
//		Optionally, create a Bitmap resource 16x15 Pixels
//		- normal default size for a toolbar bitmap.
// 
// 2.	in the header for the dialog class:
//			#include "CoolBtn.h"
//		then create the necessary button widgets in the //{{AFX_DATA() section:
//			CCoolButton  m_DblBtn;
//
// 3.	In the InitDialog() for the dialog:
//
//	a)	Optionally, Call SetButtonImage specificing the
//		Transparency color reference.
//			(Usally RGB(255, 0, 255) magenta)
//		m_DblBtn.SetButtonImage(IDC_BTNIMAGE, RGB(255,0,255));
//
//	b)	Either call CCoolButton's SetIDs function:
//			m_DblBtn.SetIDs(IDC_BTNPUSH, IDC_BTNMENU);
//		the first ID must be the same as the ID in the RC file!
//			--or--
//		Add menu item(s) to the button:
//			m_DblBtn.AddMenuItem(nMenuId1, strMenu1, nMenuFlags1);
//			m_DblBtn.AddMenuItem(nMenuId2, strMenu2, nMenuFlags2);
//
//		If you do both SetIDs() and AddMenuItem()s, only the AddMenuItem
//		will be called and the second ID will be ignored.
//
//	c)	Optionally, call CCoolButton's show split function:
//			m_DblBtn.SetSplit(bSplit);
//		The default is to show the split (TRUE). If set to FALSE, only
//		the AddMenuItem()s or 2nd ID in SetIDs() will be called and the
//		base id (1st ID in SetIDs()==ID in RC file) will be ignored.
//
//	d)	Optionally call m_DblBtn.SetAsDefaultBtn(TRUE) to make the button
//		the default button.
//
// 4.	Add the appropiate ON_COMMAND handlers in the parent windows message map
//
// 5.	You can also call SetIDs() or SetSplit() later to dynamically change the
//		button and you can later add more AddMenuItem().
////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  CCoolButton
//
// DESCRIPTION:	
//
// NOTES:			
//
// MAINTENANCE:
// Name:		  Date:	  Version:	Notes:
// NT ALMOND	210100		1.0		Origin as CCoolBtn
// L Brasington	03/06/06	2.0		Rewritten as CCoolButton
//
////////////////////////////////////////////////////////////////////////////////
CCoolButton::CCoolButton()
{
	m_nID2 = 0;
	m_bPushed = FALSE;
	m_bLoaded = FALSE;
	m_bMenuPushed = FALSE;
	m_bMenuLoaded = FALSE;
	m_bDefaultBtn = m_bActAsDefaultBtn = FALSE;
	m_bSplit = TRUE;

	m_bOverControl = FALSE;
	m_bTracking = FALSE;

	ZeroMemory(&m_bm, sizeof m_bm);
	m_menu.CreatePopupMenu();

	m_themeLib = LoadLibrary(_T("UxTheme.dll"));
}

////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  ~CCoolButton
//
// DESCRIPTION:	
//
// NOTES:			
//
// MAINTENANCE:
// Name:		  Date:	  Version:	Notes:
// NT ALMOND	210100	1.0			  Origin
//
////////////////////////////////////////////////////////////////////////////////
CCoolButton::~CCoolButton()
{
	if(m_themeLib)
		FreeLibrary(m_themeLib);
}


BEGIN_MESSAGE_MAP(CCoolButton, CButton)
	//{{AFX_MSG_MAP(CCoolButton)
	ON_WM_KEYDOWN()
	ON_WM_SYSKEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(BM_SETSTYLE, OnSetStyle)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover )
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCoolButton message handlers

////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  SetIDs
//
// DESCRIPTION:	Set the IDs
//
// NOTES:			
//
// MAINTENANCE:
// Name:		  Date:	  Version:	Notes:
// L Brasington 03/06/06	2.0		Rewritten as CCoolButton
//
////////////////////////////////////////////////////////////////////////////////
BOOL CCoolButton::SetIDs( UINT nID1, UINT nID2 )
{
	m_nID1 = nID1;
	m_nID2 = nID2;
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////

UINT CCoolButton::OnGetDlgCode() 
{
	UINT nCode = CButton::OnGetDlgCode();
	nCode |= (m_bActAsDefaultBtn ? DLGC_DEFPUSHBUTTON : DLGC_UNDEFPUSHBUTTON);
  	return nCode;
}

// mask for control's type
#undef	BS_TYPEMASK
#define BS_TYPEMASK SS_TYPEMASK

LRESULT CCoolButton::OnSetStyle(WPARAM wParam, LPARAM lParam)
{
	m_bActAsDefaultBtn = wParam & BS_DEFPUSHBUTTON;
	// can't change control type after owner-draw is set.
	// let the system process changes to other style bits
	// and redrawing, while keeping owner-draw style
	return DefWindowProc(BM_SETSTYLE,
		(wParam & ~BS_TYPEMASK) | BS_OWNERDRAW, lParam);
}

////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  DrawItem
//
// DESCRIPTION:	Called in response to draw the button
//
// NOTES:			
//
// MAINTENANCE:
// Name:		  Date:	  Version:	Notes:
// NT ALMOND	210100		1.0		Origin
// L Brasington	03/06/06	2.0		Rewritten as CCoolButton, Added XP support
//
////////////////////////////////////////////////////////////////////////////////
#ifdef USE_THEMES
typedef HTHEME(__stdcall *PFNOPENTHEMEDATA)(HWND hwnd, LPCWSTR pszClassList);
typedef HRESULT(__stdcall *PFNCLOSETHEMEDATA)(HTHEME hTheme);
typedef HRESULT(__stdcall *PFNDRAWTHEMEBACKGROUND)(HTHEME hTheme, HDC hdc, 
	int iPartId, int iStateId, const RECT *pRect, OPTIONAL const RECT *pClipRect);
typedef HRESULT(__stdcall *PFNGETTHEMEBACKGROUNDCONTENTRECT)(HTHEME hTheme, HDC hdc, 
	int iPartId, int iStateId, const RECT *pRect, RECT *pContentRect);
#endif

void CCoolButton::DrawItem(DRAWITEMSTRUCT* lpDIS) 
{
	if (lpDIS->CtlType != ODT_BUTTON)
		return;

	CFont *pFont = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));

	CDC dcMem;
	CBitmap bmp;

	CRect btnRect(lpDIS->rcItem);
	CRect trueRect(btnRect);
	CRect rectFocus(btnRect);

	CDC *pDC = CDC::FromHandle(lpDIS->hDC);

	BOOL bDisabled = ODS_DISABLED & lpDIS->itemState;

#ifdef USE_THEMES
	HTHEME hTheme = 0;
	if(m_themeLib)
	{
		PFNOPENTHEMEDATA pOpenThemeData = (PFNOPENTHEMEDATA)
			GetProcAddress(m_themeLib, "OpenThemeData");
		if(pOpenThemeData)
			hTheme = (pOpenThemeData)(m_hWnd, L"Button");
	}
	if(hTheme)
	{
		// attempt to map owner draw states to theme pushbutton states
		int iState = PBS_NORMAL;
		if(m_bOverControl)//lpDIS->itemState & ODS_HOTLIGHT)
			iState = PBS_HOT;
		else if(m_bDefaultBtn || m_bActAsDefaultBtn)//lpDIS->itemState & ODS_DEFAULT)
			iState = PBS_DEFAULTED;
		else if(lpDIS->itemState & ODS_SELECTED)
			iState = PBS_PRESSED;
		HRESULT hr;

		HDC hDC = lpDIS->hDC;
		RECT rc = lpDIS->rcItem;

		// let theme draw background
		PFNDRAWTHEMEBACKGROUND pDrawThemeBackground = (PFNDRAWTHEMEBACKGROUND)
			GetProcAddress(m_themeLib, "DrawThemeBackground");
		if(!pDrawThemeBackground)
			return;
		hr = (pDrawThemeBackground)(hTheme, hDC, 
			BP_PUSHBUTTON, iState, &rc, 0);
		if(hr != S_OK)
			return;

		// get rect to put content in
		RECT rcContent;
		PFNGETTHEMEBACKGROUNDCONTENTRECT pGetThemeBackgroundContentRect = 
			(PFNGETTHEMEBACKGROUNDCONTENTRECT)
			GetProcAddress(m_themeLib, "GetThemeBackgroundContentRect");
		if(!pGetThemeBackgroundContentRect)
			return;
		hr = (pGetThemeBackgroundContentRect)(hTheme, hDC,
			BP_PUSHBUTTON, iState, &rc, &rcContent);
		if(hr != S_OK)
			return;

		rectFocus = rcContent;
		if (!m_bMenuPushed)
			rectFocus.OffsetRect(m_bPushed,m_bPushed);
		if (m_bSplit)
			rectFocus.right -= nDropBtnWidth;
		if (lpDIS->itemState & ODS_FOCUS || m_bPushed)
		{
			if (!m_bMenuPushed)
				::DrawFocusRect(lpDIS->hDC, &rcContent);
		}
	}
	else
#endif
	{
		////////////////////////////////////////
		// Button Background                  //
		////////////////////////////////////////
		CBrush brush(GetSysColor(COLOR_BTNFACE));
		pDC->FillRect(trueRect,&brush);

		if (m_bDefaultBtn)
			btnRect.DeflateRect(1,1);

		rectFocus.DeflateRect(4,4);

		if (!m_bMenuPushed)
			rectFocus.OffsetRect(m_bPushed,m_bPushed);

		if (m_bSplit)
			rectFocus.right -= nDropBtnWidth;


		////////////////////////////////////////
		// Button in a normal state           //
		////////////////////////////////////////
		if (!m_bPushed || m_bMenuPushed)
			pDC->DrawFrameControl(&btnRect,DFC_BUTTON,DFCS_BUTTONPUSH);


		////////////////////////////////////////
		// Default Button State               //
		////////////////////////////////////////
		if ((m_bDefaultBtn || m_bPushed) && !bDisabled)
		{
			pDC->FrameRect(&lpDIS->rcItem,CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
			if (m_bPushed && !m_bMenuPushed)
				pDC->FrameRect(&btnRect,CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH))); 
		}


		////////////////////////////////////////
		// State Focus                        //
		////////////////////////////////////////
		if (lpDIS->itemState & ODS_FOCUS || m_bPushed) 
		{
			pDC->FrameRect(&lpDIS->rcItem,CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
			if (!m_bMenuPushed)
				pDC->DrawFocusRect(&rectFocus);
		}


		////////////////////////////////////////
		// Action Focus                       //
		////////////////////////////////////////
		if ((lpDIS->itemAction & ODA_FOCUS))
			if (!m_bMenuPushed)
				pDC->DrawFocusRect(&rectFocus);

	}
	////////////////////////////////////////
	// Draw out bitmap                    //
	////////////////////////////////////////

	// Draw out bitmap
	if (m_bLoaded)
	{
		if (!bDisabled)
		{
			m_IL.DrawIndirect(pDC,0,CPoint(6+m_bPushed,6+m_bPushed), CSize(m_bm.bmWidth, m_bm.bmHeight), CPoint(0,0),ILD_NORMAL);
		}
		else
		{
  			pDC->DrawState(CPoint(6+m_bPushed,6+m_bPushed), CSize(m_bm.bmWidth, m_bm.bmHeight), m_hbmpDisabled, DST_BITMAP | DSS_DISABLED);
		}
	}


	////////////////////////////////////////
	// Draw out text                      //
	////////////////////////////////////////
	pDC->SelectObject(pFont);
	CRect rectText(rectFocus);
	if (!m_bSplit)
		rectText.right -= nDropBtnWidth;
	rectText.left += m_bm.bmWidth + 2;

	CString strCaption;
	GetWindowText(strCaption);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetBkColor(GetSysColor(COLOR_BTNFACE));

	if (ODS_DISABLED & lpDIS->itemState)
	{
		rectText.OffsetRect(1,1);
		pDC->SetTextColor(GetSysColor(COLOR_WINDOW));
		pDC->DrawText(strCaption,rectText,DT_SINGLELINE|DT_CENTER|DT_VCENTER);

		rectText.OffsetRect(-1,-1);
		pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));
		pDC->DrawText(strCaption,rectText,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
	}
	else
	{
		pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
		pDC->DrawText(strCaption,rectText,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
	}


	CRect rectSplit(btnRect);
	rectSplit.DeflateRect(2,2);
	rectSplit.right -= nDropBtnWidth;


	////////////////////////////////////////
	// Drop down split                    //
	////////////////////////////////////////
	if (m_bSplit)
	{
		CPen brFace(PS_SOLID,1,GetSysColor(COLOR_3DSHADOW));
		pDC->SelectObject(&brFace);
		pDC->MoveTo(rectSplit.right, rectSplit.top);
		pDC->LineTo(rectSplit.right, rectSplit.bottom);


		CPen brLite(PS_SOLID,1,GetSysColor(COLOR_3DHILIGHT));
		pDC->SelectObject(&brLite);
		pDC->MoveTo(rectSplit.right+1 , rectSplit.top);
		pDC->LineTo(rectSplit.right+1, rectSplit.bottom);
	}

	////////////////////////////////////////
	// Draw Arrow                         //
	////////////////////////////////////////
	rectSplit.left = rectSplit.right;
	rectSplit.right += nDropBtnWidth;

	CPoint pt(rectSplit.CenterPoint());
	pt += CPoint(m_bPushed,m_bPushed);

	CPen penBlack(PS_SOLID, 1, bDisabled ? GetSysColor(COLOR_GRAYTEXT) : GetSysColor(COLOR_WINDOWTEXT));
	pDC->SelectObject(&penBlack);
	DrawArrow(pDC,pt);

	////////////////////////////////////////
	// Drop down state                    //
	////////////////////////////////////////
	if (m_bMenuPushed && !bDisabled)
	{    
		rectSplit.InflateRect(1,1);
		pDC->DrawEdge(rectSplit,BDR_SUNKENOUTER, BF_RECT);
	}

	if(hTheme)
	{
		PFNCLOSETHEMEDATA pCloseThemeData = (PFNCLOSETHEMEDATA)GetProcAddress(m_themeLib, "CloseThemeData");
		if(!pCloseThemeData)
			return;
		(pCloseThemeData)(hTheme);
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  DrawArrow
//
// DESCRIPTION:	Draws drop down arrow, we could use DrawFrameControl - a bit too 
//              messy
//
// NOTES:			
//
// MAINTENANCE:
// Name:		  Date:	  Version:	Notes:
// NT ALMOND	210100	1.0			  Origin
//
////////////////////////////////////////////////////////////////////////////////
void CCoolButton::DrawArrow(CDC* pDC,CPoint ArrowTip)
{

  CPoint ptDest;

  pDC->SetPixel(ArrowTip,RGB(0,0,0));

  ArrowTip -= CPoint(1,1);
  pDC->MoveTo(ArrowTip);
  
  ptDest = ArrowTip;
  ptDest += CPoint(3,0);
  pDC->LineTo(ptDest);

  ArrowTip -= CPoint(1,1);
  pDC->MoveTo(ArrowTip);

  ptDest = ArrowTip;
  ptDest += CPoint(5,0);
  pDC->LineTo(ptDest);

  ArrowTip -= CPoint(1,1);
  pDC->MoveTo(ArrowTip);

  ptDest = ArrowTip;
  ptDest += CPoint(7,0);
  pDC->LineTo(ptDest);
}


////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  OnLButtonDown
//
// DESCRIPTION:	handles button pressed state, including drop down menu
//              
//
// NOTES:			
//
// MAINTENANCE:
// Name:		  Date:	  Version:	Notes:
// NT ALMOND	210100		1.0		Origin
// L Brasington	03/06/06	2.0		Rewritten as CCoolButton
//
////////////////////////////////////////////////////////////////////////////////
void CCoolButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetFocus();
	if (m_bMenuPushed)
	{
		m_bMenuPushed = FALSE;
		Invalidate();
		return;
	}

	if (!m_bSplit || HitMenuBtn(point))
	{
		OnMenuBtn();
		return;
	}

	m_bPushed = TRUE;
	Invalidate();
	CButton::OnLButtonDown(nFlags, point);
}

BOOL CCoolButton::OnMenuBtn()
{
	if (!m_bMenuLoaded)
	{
		if (m_nID2)
		{
			GetParent()->PostMessage(WM_COMMAND, m_nID2, 0);
			m_bMenuPushed = TRUE;
			Invalidate();
			return TRUE;
		}
	}
	else
	{
		m_bMenuPushed = TRUE;
		Invalidate();

		CRect rc;
		GetWindowRect(rc);
		int x = rc.left;
		int y = rc.bottom;

		m_menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,x,y,GetParent());

		m_bMenuPushed = FALSE;
		Invalidate();
		return TRUE;
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  OnLButtonUp
//
// DESCRIPTION:	Redraws button in normal state
//              
//
// NOTES:			
//
// MAINTENANCE:
// Name:		  Date:	  Version:	Notes:
// NT ALMOND	210100		1.0		Origin
// L Brasington	03/06/06	2.0		Rewritten as CCoolButton
//
////////////////////////////////////////////////////////////////////////////////
void CCoolButton::ClearButtonPushed()
{
	if (m_bPushed)
	{
		m_bPushed = FALSE;
		Invalidate();
	}
	if (m_bMenuPushed)
	{
		m_bMenuPushed = FALSE;
		Invalidate();
	}
}

void CCoolButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ClearButtonPushed();
	CButton::OnLButtonUp(nFlags, point);
}

////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  OnMouseMove
//
// DESCRIPTION:	Tracks mouse whilst pressed
//              
//
// NOTES:			
//
// MAINTENANCE:
// Name:		  Date:	  Version:	Notes:
// NT ALMOND	210100		1.0		Origin
// L Brasington	03/06/06	2.0		Rewritten as CCoolButton
//
////////////////////////////////////////////////////////////////////////////////
LRESULT CCoolButton::OnMouseHover(WPARAM wparam, LPARAM lparam)
{
	m_bOverControl=TRUE;
	Invalidate();
	return 1;
}

LRESULT CCoolButton::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bTracking = FALSE;
	m_bOverControl = FALSE;
	Invalidate(FALSE);
	return 0;
}

void CCoolButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bPushed)
	{
		ClientToScreen(&point);
		if (WindowFromPoint(point) != this)
		{
  			m_bPushed = FALSE;
			Invalidate();
		}
	}
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE|TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme) != FALSE;		
	}	
	CButton::OnMouseMove(nFlags, point);
}

////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  SetButtonImage
//
// DESCRIPTION:	Sets the button image, COLORREF crMask specifics the transparency
//              color              
//
// NOTES:			
//
// MAINTENANCE:
// Name:		  Date:	  Version:	Notes:
// NT ALMOND	210100	1.0			  Origin
//
////////////////////////////////////////////////////////////////////////////////
BOOL CCoolButton::SetButtonImage(UINT nResourceId, COLORREF crMask)
{
	// The ID must exist also as a bitmap resource!!!
	m_btnImage.LoadBitmap(nResourceId);  
	m_btnImage.GetObject(sizeof m_bm, &m_bm);
	m_IL.Create( nResourceId, m_bm.bmWidth, 1, crMask );
	m_bLoaded = TRUE;
	m_crMask = crMask;

	HBITMAP bmTemp;
	COLORMAP mapColor;
	mapColor.from = crMask;
	mapColor.to  = RGB(255,255,255);

	bmTemp = (HBITMAP)::CreateMappedBitmap(AfxGetApp()->m_hInstance, nResourceId, IMAGE_BITMAP, &mapColor, 1);
	m_hbmpDisabled = (HBITMAP)::CopyImage(bmTemp, IMAGE_BITMAP, m_bm.bmWidth, m_bm.bmHeight, LR_COPYDELETEORG);

	return m_bLoaded;
}

void CCoolButton::OnSetFocus(CWnd* pOldWnd) 
{
	CButton::OnSetFocus(pOldWnd);
	Invalidate();
}

void CCoolButton::OnKillFocus(CWnd* pNewWnd) 
{
	CButton::OnKillFocus(pNewWnd);
}

////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  OnSysColorChange
//
// DESCRIPTION:	Called when system colors change, force a button redraw
//
// NOTES:			
//
// MAINTENANCE:
// Name:		  Date:	  Version:	Notes:
// NT ALMOND	210100	1.0			  Origin
//
////////////////////////////////////////////////////////////////////////////////
void CCoolButton::OnSysColorChange() 
{
	CButton::OnSysColorChange();
	Invalidate();	
}
 
////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  HitMenuBtn
//
// DESCRIPTION:	Helper function to test for menu button hit...
//
// NOTES:			
//
// MAINTENANCE:
// Name:		  Date:	  Version:	Notes:
// NT ALMOND	210100		1.0		Origin
// L Brasington	03/06/06	2.0		Rewritten as CCoolButton
//
////////////////////////////////////////////////////////////////////////////////
BOOL CCoolButton::HitMenuBtn(CPoint point)
{
	if (!m_bMenuLoaded && !m_nID2)
		return FALSE; // Don't allow menu button drop down if no menu items are loaded

	ClientToScreen(&point);
	CRect rect;
	GetWindowRect(rect);
	rect.left = rect.right - nDropBtnWidth;

	return rect.PtInRect(point);
}


////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	  AddMenuItem
//
// DESCRIPTION:	Adds a menu item and id to our menu.
//
// NOTES:			
//
// MAINTENANCE:
// Name:		  Date:	  Version:	Notes:
// NT ALMOND	210100	1.0			  Origin
//
////////////////////////////////////////////////////////////////////////////////
BOOL CCoolButton::AddMenuItem(UINT nMenuId,const CString strMenu, UINT nMenuFlags)
{
	BOOL bRet = m_menu.AppendMenu(nMenuFlags | MF_STRING, nMenuId, (LPCTSTR)strMenu);
	m_bMenuLoaded |= bRet;
	return bRet;
}

////////////////////////////////////////////////////////////////////////////////
void CCoolButton::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == 'F' && GetKeyState(VK_CONTROL) & 0x8000)	// ^F == Find
		GetParent()->PostMessage(WM_COMMAND, ID_POSITIONTOPATTERN, 0);
	else if (nChar == VK_F3)
		GetParent()->PostMessage(WM_COMMAND, GetKeyState(VK_SHIFT) & 0x8000 
											 ? ID_FINDPREV : ID_FINDNEXT, 0);
	else
		CButton::OnChar(nChar, nRepCnt, nFlags);
}

void CCoolButton::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch ( nChar )
	{
	case VK_DOWN:
		OnMenuBtn();
		break;
	}
	
	CWnd::OnSysKeyDown(nChar, nRepCnt, nFlags);
}
