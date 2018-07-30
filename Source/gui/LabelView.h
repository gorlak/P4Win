// LabelView.h : header file
//

#ifndef __LABELVIEW__
#define __LABELVIEW__

#include "P4PaneView.h"
#include "LabelListCtrl.h"

class CLabelView : public CP4PaneView
{
	DECLARE_DYNCREATE(CLabelView)

	CLabelListCtrl m_labelListCtrl;

public:
	CLabelView();
	virtual ~CLabelView();

// Attributes
public:
	CLabelListCtrl& GetListCtrl() { return m_labelListCtrl; }

// Overrides

// Implementation
protected:
	virtual bool CreateContent();
	virtual CWnd * GetContentWnd() { return &m_labelListCtrl; }
	virtual void SetToolBarButtons();

// Operations
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDropDown(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnUpdateFilterLabels(CCmdUI* pCmdUI);
	afx_msg void OnUpdateClearFilterLabels(CCmdUI* pCmdUI);
};

#endif //__LABELVIEW__
/////////////////////////////////////////////////////////////////////////////
