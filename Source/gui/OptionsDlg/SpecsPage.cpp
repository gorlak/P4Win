/*
 * Copyright 1999 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */



// SpecsPage.cpp : implementation file
//

#include "stdafx.h"
#include "..\p4win.h"
#include "SpecsPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpecsPage property page

IMPLEMENT_DYNCREATE(CSpecsPage, CPropertyPage)

CSpecsPage::CSpecsPage() : CPropertyPage(CSpecsPage::IDD)
{
	//{{AFX_DATA_INIT(CSpecsPage)
	m_FontFace = _T("");
	m_FontSize = _T("");
	m_FontStyle = _T("");
	m_MinMultiLineSize = _T("");
	m_DescWrapSw = FALSE;
	m_DescWrap = _T("");
	//}}AFX_DATA_INIT

	m_Inited = FALSE;
}

CSpecsPage::~CSpecsPage()
{
}

void CSpecsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpecsPage)
	DDX_Control(pDX, IDC_AUTOMINEDITDLG, m_AutoMinEditDlg);
	DDX_Control(pDX, IDC_ALLOWPROMPTABOVE, m_AllowPromptAbove);
	DDX_Control(pDX, IDC_PRESERVESPECFORMAT, m_PreserveSpecFormat);
	DDX_Text(pDX, IDC_MINMULTILINESIZE, m_MinMultiLineSize);
	DDV_MaxChars(pDX, m_MinMultiLineSize, 1);
	DDX_Check(pDX, IDC_DESCWRAPSW, m_DescWrapSw);
	DDX_Text(pDX, IDC_DESCWRAP, m_DescWrap);
	DDV_MaxChars(pDX, m_DescWrap, 3);
	DDX_Control(pDX, IDC_FONT, m_FontCombo);
	DDX_Control(pDX, IDC_FONTSIZE, m_SizeCombo);
	DDX_Control(pDX, IDC_STYLE, m_StyleCombo);
	DDX_Control(pDX, IDC_SAMPLE, m_Sample);
	DDX_CBString(pDX, IDC_FONT, m_FontFace);
	DDX_CBString(pDX, IDC_FONTSIZE, m_FontSize);
	DDX_CBString(pDX, IDC_STYLE, m_FontStyle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSpecsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CSpecsPage)
	ON_CBN_SELCHANGE(IDC_FONTSIZE, OnSelChange)
	ON_CBN_SELCHANGE(IDC_STYLE, OnSelChange)
	ON_CBN_KILLFOCUS(IDC_FONT, OnSelchangeFont)
	ON_CBN_KILLFOCUS(IDC_FONTSIZE, OnSelChange)
	ON_CBN_KILLFOCUS(IDC_STYLE, OnSelChange)
	ON_CBN_SELCHANGE(IDC_FONT, OnSelchangeFont)
	ON_BN_CLICKED(IDC_DESCWRAPSW, OnDescWrapSw)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpecsPage message handlers

BOOL CSpecsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// Create the current fixed width font
	CWindowDC dc(GetDesktopWindow() );
	m_cyPixelsPerInch = GetDeviceCaps(dc, LOGPIXELSY);	

	// Get the current dialog font from the registry 
	//
	CString face= GET_P4REGPTR()->GetFontFace();
	int size= - abs(GET_P4REGPTR()->GetFontSize());
	int weight= GET_P4REGPTR()->GetFontWeight();
	BOOL isItalic= GET_P4REGPTR()->GetFontItalic();

	// Attempt to create the font that is referred to in the
	// registry.  Windows may substitute an alternative
	LOGFONT logFont;
	memset( &logFont, 0, sizeof(LOGFONT) );
	lstrcpy( logFont.lfFaceName, face );
	logFont.lfCharSet= DEFAULT_CHARSET;
	logFont.lfHeight= size;
	logFont.lfWeight= weight;
	logFont.lfItalic= (unsigned char) isItalic;
	logFont.lfPitchAndFamily= FIXED_PITCH | FF_DONTCARE;
	m_FontSample.CreateFontIndirect( &logFont );
	m_FontSample.GetLogFont( &logFont );	

	m_FontFace = logFont.lfFaceName;
	m_FontSize.Format( _T("%d"), abs(MulDiv( logFont.lfHeight, 72, m_cyPixelsPerInch)) );
	m_FontStyle = LoadStringResource(IDS_FONT_REGULAR);
	if( logFont.lfWeight >= 700 && logFont.lfItalic)
		m_FontStyle = LoadStringResource(IDS_FONT_BOLD_ITALIC);
	else if( logFont.lfItalic )
		m_FontStyle = LoadStringResource(IDS_FONT_ITALIC);
	else if ( logFont.lfWeight >= 700 )
		m_FontStyle = LoadStringResource(IDS_FONT_BOLD);

	// Now get the names of all the fixed width fonts
	memset( &logFont, 0, sizeof(LOGFONT) );
	logFont.lfPitchAndFamily= FIXED_PITCH | FF_DONTCARE;
	logFont.lfCharSet= DEFAULT_CHARSET; 
	CWindowDC dcthis(this);
	EnumFontFamiliesEx( dcthis.m_hDC, &logFont, (FONTENUMPROC)FontEnumProc, (LPARAM) this, 0 );		

	// Fill Style combobox with "common" styles
	m_StyleCombo.AddString( LoadStringResource(IDS_FONT_REGULAR) );	
	m_StyleCombo.AddString( LoadStringResource(IDS_FONT_BOLD) );
	m_StyleCombo.AddString( LoadStringResource(IDS_FONT_ITALIC) );
	m_StyleCombo.AddString( LoadStringResource(IDS_FONT_BOLD_ITALIC) );

	// Load non-font stuff
	m_AutoMinEditDlg.SetCheck(GET_P4REGPTR()->AutoMinEditDlg());
	m_AllowPromptAbove.SetCheck(GET_P4REGPTR()->AllowPromptAbove());
	m_PreserveSpecFormat.SetCheck(GET_P4REGPTR()->PreserveSpecFormat()); 
	m_MinMultiLineSize = GET_P4REGPTR()->GetMinMultiLineSize();

	m_DescWrapSw = GET_P4REGPTR()->GetDescWrapSw();
	TCHAR str[8];
	_stprintf(str, _T("%d"), GET_P4REGPTR()->GetDescWrap());
	m_DescWrap = str;

	// Update input fields
	UpdateData(FALSE);
	OnSelchangeFont();
	OnDescWrapSw();
		
	m_Inited = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CALLBACK CSpecsPage::FontEnumProc( ENUMLOGFONTEX *enumLogFontEx, NEWTEXTMETRICEX *newTextMetric,
		int fontType, CSpecsPage *specsPage )
{
	BOOL fixedPitch= FALSE;

	if( enumLogFontEx->elfLogFont.lfPitchAndFamily & FIXED_PITCH )
		fixedPitch= TRUE;
	
    if( fixedPitch && (lstrlen((LPCTSTR)enumLogFontEx->elfFullName) < 32)
		&& (specsPage->m_FontCombo.FindStringExact( 0, 
							(LPCTSTR)enumLogFontEx->elfFullName) == CB_ERR ) )
	{
		int index= specsPage->m_FontCombo.AddString( (LPCTSTR)enumLogFontEx->elfFullName );
		specsPage->m_FontCombo.SetItemData( index, (DWORD_PTR) fontType );	
	}
	return 1;
}

void CSpecsPage::OnSelChange() 
{
	// The selection hasn't changed yet, so change it	
	if( IsChild( GetFocus() ) && GetFocus()->GetParent()->IsKindOf( RUNTIME_CLASS( CComboBox ) ) )	
	{
		CComboBox *cb = (CComboBox *)GetFocus()->GetParent();		
		CString text;
		if( cb->GetCurSel() != CB_ERR )		
		{			
			cb->GetLBText( cb->GetCurSel(), text );
			cb->SetWindowText( text );		
		}	
	}
	
	UpdateData(TRUE);		
	UpdateSampleFont();
}

void CSpecsPage::UpdateSampleFont()
{
	LOGFONT logFont;

	memset( &logFont, 0, sizeof(LOGFONT) );
	logFont.lfPitchAndFamily= FIXED_PITCH | FF_DONTCARE;
	lstrcpy(logFont.lfFaceName,m_FontFace.GetBuffer(m_FontFace.GetLength()));
	logFont.lfHeight= -MulDiv(_ttoi(m_FontSize), m_cyPixelsPerInch, 72);	 
	logFont.lfWeight = 400;			//Regular	
	logFont.lfItalic = FALSE;
    logFont.lfCharSet = DEFAULT_CHARSET;
	if( m_FontStyle.Find( LoadStringResource(IDS_FONT_ITALIC) ) != -1 )		
		logFont.lfItalic = TRUE;
	if( m_FontStyle.Find( LoadStringResource(IDS_FONT_BOLD) ) != -1 )		
		logFont.lfWeight = 700;

	m_FontSample.DeleteObject();	
	m_FontSample.CreateFontIndirect( &logFont );
 	m_Sample.SetFont(&m_FontSample);	
}


void CSpecsPage::OnOK() 
{
	if (!m_Inited)
		return;

	// This bit of silliness is required to trick the font edit
	// fields into providing their data to the UpdateData() routine
	GotoDlgCtrl(GetParent()->GetDlgItem(IDC_APPLY));

	m_ErrorCount = 0;
	if( UpdateData(TRUE) )
	{
		int i;
		LOGFONT logFont;

		m_FontSample.GetLogFont( &logFont );

		if( !GET_P4REGPTR()->SetFontFace( logFont.lfFaceName ) ) 
			m_ErrorCount++;
		if( !GET_P4REGPTR()->SetFontWeight( logFont.lfWeight ) ) 
			m_ErrorCount++;
		if( !GET_P4REGPTR()->SetFontSize( abs(logFont.lfHeight) ) ) 
			m_ErrorCount++;
		if( !GET_P4REGPTR()->SetFontItalic( logFont.lfItalic ) ) 
			m_ErrorCount++;

		BOOL autoMinEditDlg = TRUE;
		if( m_AutoMinEditDlg.GetCheck() == 0 )
			autoMinEditDlg = FALSE;
		if( autoMinEditDlg != GET_P4REGPTR()->AutoMinEditDlg( ) )
			if(!GET_P4REGPTR()->SetAutoMinEditDlg( autoMinEditDlg ) )
				m_ErrorCount++;

		BOOL allowPromptAbove = TRUE;
		if( m_AllowPromptAbove.GetCheck() == 0 )
			allowPromptAbove = FALSE;
		if( allowPromptAbove != GET_P4REGPTR()->AllowPromptAbove( ) )
			if(!GET_P4REGPTR()->SetAllowPromptAbove( allowPromptAbove ) )
				m_ErrorCount++;

		BOOL preserveSpecFormat = TRUE;
		if( m_PreserveSpecFormat.GetCheck() == 0 )
			preserveSpecFormat = FALSE;
		if( preserveSpecFormat != GET_P4REGPTR()->PreserveSpecFormat( ) )
			if(!GET_P4REGPTR()->SetPreserveSpecFormat( preserveSpecFormat ) )
				m_ErrorCount++;

		if (!m_ErrorCount)
		{
			m_MinMultiLineSize.TrimRight();
			m_MinMultiLineSize.TrimLeft();
			if (m_MinMultiLineSize < _T("2"))
				m_MinMultiLineSize = _T("2");
			if(!GET_P4REGPTR()->SetMinMultiLineSize( m_MinMultiLineSize ) )
				m_ErrorCount++;
		}

		m_DescWrap.TrimLeft();
		i = _tstoi(m_DescWrap);
		if (m_DescWrapSw && i < 10)
			AfxMessageBox( IDS_DESC_WRAP_VALUE_MUST_BE_GREATER_THAN_9, MB_ICONEXCLAMATION );
		if (!m_ErrorCount)
		{
			if (i < 10)
				m_DescWrapSw = FALSE;
			if(!GET_P4REGPTR()->SetDescWrap( i ) )
				m_ErrorCount++;
		}
		// do this after m_DescWrap because it might set m_DescWrapSw
		if (!m_ErrorCount)
		{
			if(!GET_P4REGPTR()->SetDescWrapSw( m_DescWrapSw ) )
				m_ErrorCount++;
		}
		
		if(m_ErrorCount)
			AfxMessageBox(IDS_BAD_REGISTRY, MB_ICONSTOP);
	}

	if(m_ErrorCount)
		MessageBox(LoadStringResource(IDS_BAD_REGISTRY), LoadStringResource(IDS_P4WIN_ERROR),  MB_ICONSTOP);
}

void CSpecsPage::OnSelchangeFont() 
{
	int	cursel;

	// Change the selection
	if( (cursel = m_FontCombo.GetCurSel()) != CB_ERR )		
	{			
		CString text;
		m_FontCombo.GetLBText( m_FontCombo.GetCurSel(), text );
		m_FontCombo.SetWindowText( text );		
	}

	// Fill in the size list
	m_SizeCombo.ResetContent();
	
	// Fill with "real" sizes
	CString faceName;
	if (cursel >= 0)
		 m_FontCombo.GetLBText(cursel, faceName);
	else m_FontCombo.GetWindowText(faceName);

	if (faceName.GetLength() > 31)
	{
		AfxMessageBox(IDS_MICROSOFT_NO_SUPPORT_FONTNAMES_LONGER_THAN_31_CHAR, MB_OK | MB_ICONEXCLAMATION);
		m_FontCombo.SetWindowText( _T("") );
		m_FontCombo.SetCurSel(-1);
		return;
	}

	CClientDC dc(NULL);

	DWORD_PTR fontType= (cursel >= 0) ? m_FontCombo.GetItemData( m_FontCombo.GetCurSel() ) : 0;

	// Check if we have a font that is either a vector or Truettype font
	if ( fontType != RASTER_FONTTYPE)
	{
		static TCHAR* Defaults[] = 
		{	_T("8"), _T("9"), _T("10"), _T("11"), _T("12"), _T("14"),
			_T("16"), _T("18"), _T("20"), _T("22"), _T("24"), _T("26"), 
			_T("28"), _T("36"), _T("48") 
		};

		// Fill with "common" sizes
		for (int i = 0; i < sizeof(Defaults)/sizeof(Defaults[0]); i++)
			m_SizeCombo.AddString( Defaults[i] );
	}
	else	// Enum the fontname to get all the sizes
		EnumFontFamilies(dc.GetSafeHdc(), faceName, (FONTENUMPROC) CSpecsPage::SizeEnumProc, (LPARAM) this);

	// Set the point size
	if (m_SizeCombo.FindString(-1, m_FontSize) != CB_ERR)
	{
		// Attempt to select proper list item
		m_SizeCombo.SelectString(-1, m_FontSize);
	}
	else
	{
		// Point size is not in the list so just fill the edit box
		// and don't select anything from the list
		m_SizeCombo.SetCurSel(-1);
		m_SizeCombo.SetWindowText(m_FontSize);
	}

	// Redraw the sample
	UpdateData(TRUE);
	UpdateSampleFont();
}

//	This callback is called once for each font on the system that has the same name
//	as the current font in the font name combobox.  It does nothing for non-Raster fonts;
//	for raster fonts, it adds the various font sizes to the font size combobox.
//	Because the fonts come in random order, to be able to get the sizes in ascending
//	order, we have to paw thru those already loaded to figure where to insert this size.
//	We cannot us the CB_SORT style becuase that is an alpha sort and 7 would come after 10.

int CALLBACK CSpecsPage::SizeEnumProc( ENUMLOGFONT *enumLogFont, 
									  NEWTEXTMETRIC *newTextMetric, int fontType, 
									  CSpecsPage *specsPage)
{
	int	i, hy;
	ASSERT(specsPage != NULL);

	if (fontType & RASTER_FONTTYPE)
	{
		int height = newTextMetric->tmHeight - newTextMetric->tmInternalLeading;
		CString h;
		h.Format(_T("%d"), hy = MulDiv(height, 72, specsPage->m_cyPixelsPerInch) );

		// First look to see if this size is already in the combox
		// If so (i.e. != -1), there is nothing to do
		if (specsPage->m_SizeCombo.FindStringExact( -1, h ) == -1)
		{
			CString buf;
			// now look thru the size already loaded and find one bigger than this
			for (i = -1; specsPage->m_SizeCombo.GetLBTextLen(++i) != CB_ERR; )
			{
				specsPage->m_SizeCombo.GetLBText(i, buf);
				int y = _ttoi(buf);
				if (y > hy)	// if this one is bigger than the new one
				{			//	insert the new one in front of it
					specsPage->m_SizeCombo.InsertString(i, h);
					i = -1;	// signal that we did the insert
					break;
				}
			}
			if (i != -1)	// i != -1 means we didn't find a bigger one, so append this one at the end
				specsPage->m_SizeCombo.AddString(h);
		}
	}

	return 1;
}


void CSpecsPage::OnDescWrapSw() 
{
	UpdateData();
	CWnd *pwnd=GetDlgItem(IDC_DESCWRAP);
	if( m_DescWrapSw )
	{
		pwnd->EnableWindow(TRUE);
		if (m_Inited)
			GotoDlgCtrl(pwnd);
	}
	else
		pwnd->EnableWindow(FALSE);
}
