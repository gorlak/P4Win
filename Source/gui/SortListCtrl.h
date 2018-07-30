// SortListCtrl.h : header file
//

#ifndef __SORTLISTCTRL__
#define __SORTLISTCTRL__

#include "SortListHeader.h"

/////////////////////////////////////////////////////////////////////////////
// CSortListCtrl window

class CSortListCtrl : public CListCtrl
{
// Construction
public:
	CSortListCtrl();

// Attributes
protected:
	CSortListHeader m_headerctrl;
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSortListCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSortListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSortListCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif //__SORTLISTCTRL__
/////////////////////////////////////////////////////////////////////////////
