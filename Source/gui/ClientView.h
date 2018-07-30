// ClientView.h : header file
//

#ifndef __CLIENTVIEW__
#define __CLIENTVIEW__

#include "P4PaneView.h"
#include "ClientListCtrl.h"

class CClientView : public CP4PaneView
{
	DECLARE_DYNCREATE(CClientView)

	CClientListCtrl m_clientListCtrl;

public:
	CClientView();
	virtual ~CClientView();

// Attributes
public:
	CClientListCtrl& GetListCtrl() { return m_clientListCtrl; }

// Overrides

// Implementation
protected:
	virtual bool CreateContent();
	virtual CWnd * GetContentWnd() { return &m_clientListCtrl; }
	virtual void SetToolBarButtons();
	void LButtonDblClk();

// Operations
public:
	DECLARE_MESSAGE_MAP()
};

#endif //__CLIENTVIEW__
/////////////////////////////////////////////////////////////////////////////
