//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// OldChgView.h : header file
//

#ifndef __OLDCHGVIEW__
#define __OLDCHGVIEW__

#include "P4PaneView.h"
#include "OldChgListCtrl.h"

class COldChgView : public CP4PaneView
{
	DECLARE_DYNCREATE(COldChgView)

	COldChgListCtrl m_oldChgListCtrl;

public:
	COldChgView();
	virtual ~COldChgView();

// Attributes
public:
	COldChgListCtrl& GetListCtrl() { return m_oldChgListCtrl; }

// Overrides

// Implementation
protected:
	virtual bool CreateContent();
	virtual CWnd * GetContentWnd() { return &m_oldChgListCtrl; }
	virtual void SetToolBarButtons();
	void LButtonDblClk();

// Operations
public:
	DECLARE_MESSAGE_MAP()
	virtual void OnInitialUpdate();
};

#endif //__OLDCHGVIEW__
/////////////////////////////////////////////////////////////////////////////
