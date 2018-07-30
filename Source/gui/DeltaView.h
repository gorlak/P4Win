//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// DeltaView.h : header file
//
#ifndef __DELTAVIEW
#define __DELTAVIEW

#include "P4PaneView.h"
#include "DeltaTreeCtrl.h"
#include "P4filestats.h"

class CDeltaView : public CP4PaneView
{
	DECLARE_DYNCREATE(CDeltaView)

	CDeltaTreeCtrl m_deltaTreeCtrl;

public:
	CDeltaView();
	virtual ~CDeltaView();

// Attributes
protected:
	CBitmap m_SortMenuBitmap;

public:
	CDeltaTreeCtrl& GetTreeCtrl() { return m_deltaTreeCtrl; }

// Overrides
	virtual BOOL OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll = TRUE);

// Implementation
protected:
	virtual bool CreateContent();
	virtual CWnd * GetContentWnd() { return &m_deltaTreeCtrl; }
	virtual void SetToolBarButtons();

// Operations
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDropDown(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnUpdateViewDelta(CCmdUI* pCmdUI);
};

#endif // __DELTAVIEW
/////////////////////////////////////////////////////////////////////////////
