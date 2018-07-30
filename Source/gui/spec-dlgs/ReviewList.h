//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

//
// ReviewList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CReviewList window

#ifndef	CREVIEWLIST_ISDEFINED
#define CREVIEWLIST_ISDEFINED 1

class CP4CheckListBox : public CCheckListBox
{
// Construction
public:
	CP4CheckListBox();

// Attributes
protected:

// Operations
public:
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CP4CheckListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CP4CheckListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CP4CheckListBox)
	
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

class CReviewList : public CWnd
{
// Construction
public:
	CReviewList();

// Attributes
protected:
	int m_BtnHeight;
	int m_BtnWidth;
	BOOL m_InitDone;
	CRect m_InitRect;
	CRect m_LastRect;
	CP4CheckListBox *m_List;
	CButton *m_SelAll;
	CButton *m_UnSelAll;

	// counters for enabling/disabling the Select/Unselect All buttons
	int m_nbrItems;
	int m_nbrChked;
	int m_nbrUnCked;
	int m_nbrIndetr;

// Operations
public:
	int  AddString(LPCTSTR lpszItem);
	BOOL GetCheck(int nItem) { return m_List->GetCheck(nItem); }
	int  GetCount( ) { return m_List->GetCount( ); }
	int  GetCurSel( ) { return m_List->GetCurSel( ); }
	int  GetText(int nIndex, LPTSTR lpszBuffer) { return m_List->GetText(nIndex, lpszBuffer); }
	void GetText(int nIndex, CString& rString) { m_List->GetText(nIndex, rString); }
	void ResetContent( );
	void SetCheck(int nItem, BOOL fCheck);
	void SetCheckStyle(UINT nStyle) { m_List->SetCheckStyle(nStyle); }
	void SetFont(CFont* pFont, BOOL bRedraw=TRUE) { m_List->SetFont(pFont, bRedraw); }
	void SetHorizontalExtent(int cxExtent) { m_List->SetHorizontalExtent(cxExtent); }
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReviewList)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CReviewList();

	// Generated message map functions
	//{{AFX_MSG(CReviewList)
	afx_msg void OnSelectAll();
	afx_msg void OnUnselectAll();
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnChkChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif
