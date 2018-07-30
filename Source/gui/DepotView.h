//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// DepotView.h : header file
//

#ifndef __DEPOTVIEW
#define __DEPOTVIEW

#include "P4PaneView.h"
#include "DepotTreeCtrl.h"

class CDepotView : public CP4PaneView
{
	DECLARE_DYNCREATE(CDepotView)

	CDepotTreeCtrl m_depotTreeCtrl;

public:
	CDepotView();
	virtual ~CDepotView();

// Attributes
public:
	CDepotTreeCtrl& GetTreeCtrl() { return m_depotTreeCtrl; }

// Overrides

// Implementation
protected:
	virtual bool CreateContent();
	virtual CWnd * GetContentWnd() { return &m_depotTreeCtrl; }
	virtual void SetToolBarButtons();

	void LButtonDblClk();

// Operations
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDropDown(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnUpdateViewDepot(CCmdUI* pCmdUI);
};

/////////////////////////////////////////////////////////////////////////////
#endif //DEPOTVIEW
