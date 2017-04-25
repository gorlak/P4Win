// BranchView.h : header file
//

#ifndef __BRANCHVIEW__
#define __BRANCHVIEW__

#include "BranchListCtrl.h"
#include "P4PaneView.h"

class CBranchView : public CP4PaneView
{
	DECLARE_DYNCREATE(CBranchView)

	CBranchListCtrl m_branchListCtrl;

public:
	CBranchView();
	virtual ~CBranchView();

// Attributes
public:
	CBranchListCtrl& GetListCtrl() { return m_branchListCtrl; }

// Overrides

// Implementation
protected:
	virtual bool CreateContent();
	virtual CWnd * GetContentWnd() { return &m_branchListCtrl; }
	virtual void SetToolBarButtons();

// Operations
public:
	DECLARE_MESSAGE_MAP()
};

#endif //__BRANCHVIEW__
/////////////////////////////////////////////////////////////////////////////
