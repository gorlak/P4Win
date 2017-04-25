// P4AboutDialog.cpp : implementation file
//

#include "stdafx.h"
#include <afxole.h>
#include "P4AboutDialog.h"
#include "commonres.h"
#include "resource.h"
#include "P4GuiApp.h"
#include "StringUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CP4AboutDialog dialog


CP4AboutDialog::CP4AboutDialog(CWnd *parent) : CDialog(IDD_ABOUT, parent)
{
}

BEGIN_MESSAGE_MAP(CP4AboutDialog, CDialog)
	ON_COMMAND(ID_COPY_ABOUT_BOX, OnEditCopy)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

BOOL CP4AboutDialog::OnInitDialog() 
{
	int i;
    CDialog::OnInitDialog();

    SetWindowText(LoadStringResource(IDS_ABOUTCAPTION));
    
    // Set up a white background color and brush
    m_backgroundColor = RGB(255,255,255);
    m_backgroundBrush.CreateSolidBrush( m_backgroundColor );

    // Load the logo bitmap
	CBitmap bitmap;
	CDC dc;
	dc.CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	int colorDepth = dc.GetDeviceCaps(BITSPIXEL);
	if(colorDepth >= 16)
		bitmap.LoadBitmap(MAKEINTRESOURCE(IDB_LOGO));
	else
		bitmap.LoadBitmap(MAKEINTRESOURCE(IDB_LOGO256));
	BITMAP bMap;
	bitmap.GetBitmap(&bMap);
    m_logo.Create( bMap.bmWidth, bMap.bmHeight, ILC_COLOR24, 1, 1);
    m_logo.Add( &bitmap, (CBitmap*)0);

    // Set the richedit's font to match dialog font
    CRichEditCtrl *pRich = (CRichEditCtrl*)GetDlgItem(IDC_VERSION);
    pRich->SetFont(GetFont(), TRUE);

	// Set the richedit's text color to black
	CHARFORMAT cf;
	pRich->GetDefaultCharFormat(cf);
	cf.crTextColor = RGB(0,0,0);
	cf.dwMask |= CFM_COLOR;
	cf.dwEffects &= 0xFFFFFFFF ^ CFE_AUTOCOLOR;
	pRich->SetDefaultCharFormat(cf);

    // Set the richedit background color to match dialog background
    pRich->SetBackgroundColor(FALSE, m_backgroundColor);

	// move richedit over to leave space for the logo
	CRect r;
	pRich->GetWindowRect(r);
	ScreenToClient(r);
	r.left += 128;
	pRich->SetWindowPos(&wndTop, r.left, r.top, r.Width(), r.Height(), SWP_NOZORDER);

	// leave a 7 DU margin to the left of the logo
	CRect du(0,0,7,0);
	MapDialogRect(du);
	m_logoPos.x = du.Width();
	m_logoPos.y = (r.top + r.bottom - 128) / 2;

    // Fill in the version information
    // formatting it as RTF

	CString info;
	info.Format(_T("{\\rtf1\\ansi\\deff0\\tx300"));	// set indent at 300 twips
	info += "\\b ";
	info += LoadStringResource(IDS_PROGRAM_TITLE);
	info += "\\b0 \\\n\t";

#ifndef UNICODE
#ifdef IDS_NOTUNICODE
    info += LoadStringResource(IDS_NOTUNICODE);
#endif
#endif
    info += LoadStringResource(IDS_VERSION)
		 + ((CP4GuiApp*)AfxGetApp())->GetAppVersionString();
	if ((i = info.ReverseFind(_T('.'))) != -1)
		info.Delete(i);
	info += _T("\\\n\t");
	info += __DATE__;
	info += _T("\\\n\t");
    info += ((CP4GuiApp*)AfxGetApp())->GetAppCopyrightString();
	
	info += _T("\\\n\\\n\t\\b ");
    info += LoadStringResource(IDS_RESOURCEDLL);
	info += "\\b0 \\\n\t";
    info += LoadStringResource(IDS_VERSION) 
		 + ((CP4GuiApp*)AfxGetApp())->GetResVersionString();
	if ((i = info.ReverseFind(_T('.'))) != -1)
		info.Delete(i);
	info += _T("\\\n\t");
    info += ((CP4GuiApp*)AfxGetApp())->GetResLanguageString();
	info += _T("\\\n\t");
    info += ((CP4GuiApp*)AfxGetApp())->GetResCopyrightString();
	info += "\n\\\n}";

	pRich->SetWindowText(info);
    
	return TRUE;    // return TRUE unless you set the focus to a control
                    // EXCEPTION: OCX Property Pages should return FALSE
}

void CP4AboutDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// just draw the logo
	m_logo.Draw( &dc, 0, m_logoPos, ILD_NORMAL );
}

HBRUSH CP4AboutDialog::OnCtlColor(CDC* pDC, CWnd* /*pWnd*/, UINT /*nCtlColor*/) 
{
	pDC->SetBkColor( m_backgroundColor );
	return m_backgroundBrush;
}

void CP4AboutDialog::OnContextMenu(CWnd* pWnd, CPoint point) 
{
    long nStartChar, nEndChar;
    CRichEditCtrl *pRich = (CRichEditCtrl*)GetDlgItem(IDC_VERSION);
    pRich->GetSel(nStartChar, nEndChar);
	if (nStartChar == nEndChar)
		return;

	CMenu popMenu;
    popMenu.LoadMenu(IDR_ABOUTBOX);
	popMenu.GetSubMenu(0)->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}

void CP4AboutDialog::OnEditCopy() 
{
    CRichEditCtrl *pRich = (CRichEditCtrl*)GetDlgItem(IDC_VERSION);
    CString selText= pRich->GetSelText();
	int lgth;
	if( (lgth=selText.GetLength()) > 0 )
	{
		COleDataSource *pSource= new COleDataSource();
		HGLOBAL hText= ::GlobalAlloc(GMEM_SHARE, (lgth+1)*sizeof(TCHAR));
		LPTSTR pStr= (LPTSTR) ::GlobalLock( hText );
		lstrcpy( pStr, selText.GetBuffer(lgth+1) );
		::GlobalUnlock( hText );
	#ifdef UNICODE
		pSource->CacheGlobalData( CF_UNICODETEXT, hText );
	#else
		pSource->CacheGlobalData( CF_TEXT, hText );
	#endif
		pSource->SetClipboard();
	}
}
