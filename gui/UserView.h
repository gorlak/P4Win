// UserView.h : header file
//

#ifndef __USERVIEW__
#define __USERVIEW__

#include "P4PaneView.h"
#include "UserListCtrl.h"

class CUserView : public CP4PaneView
{
	DECLARE_DYNCREATE(CUserView)

	CUserListCtrl m_userListCtrl;

public:
	CUserView();
	virtual ~CUserView();

// Attributes
public:
	CUserListCtrl& GetListCtrl() { return m_userListCtrl; }

// Overrides

// Implementation
protected:
	virtual bool CreateContent();
	virtual CWnd * GetContentWnd() { return &m_userListCtrl; }
	virtual void SetToolBarButtons();

// Operations
public:
	DECLARE_MESSAGE_MAP()
};

#endif //__USERVIEW__
/////////////////////////////////////////////////////////////////////////////
