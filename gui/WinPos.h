/*
 * Copyright 1997, 1999 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */

#ifndef __WINPOS__
#define __WINPOS__

// WinPos.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWinPos

class CWinPos 
{
// Construction
public:
	CWinPos(bool noSize = false);   // standard constructor

protected:
	CString m_WindowName;
	CWnd *m_pWnd;
	CRect m_DefPos;
	CSize m_MinSize;
	bool m_noSize;

	bool ReadRegistry( RECT *rect );
	void WriteRegistry( RECT *rect );

// Implementation
public:
	void SetWindow( CWnd *pWnd, LPCTSTR windowName );
	void SetWindowPtr( CWnd *pWnd ) { m_pWnd = pWnd; }
	void SaveWindowPosition();
	void SetMinSize( CSize minSize ) { m_MinSize= minSize; }
	void SetDefaultPos( CRect &rect ) { m_DefPos= rect; }
	bool RestoreWindowPosition();

protected:

};

#endif // __WINPOS__