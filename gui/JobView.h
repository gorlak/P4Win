//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// JobView.h : header file
//

#ifndef __JOBVIEW__
#define __JOBVIEW__

#include "P4PaneView.h"
#include "JobListCtrl.h"

class CJobView : public CP4PaneView
{
	DECLARE_DYNCREATE(CJobView)

	CJobListCtrl m_jobListCtrl;

public:
	CJobView();
	virtual ~CJobView();

// Attributes
public:
	CJobListCtrl& GetListCtrl() { return m_jobListCtrl; }

// Overrides

// Implementation
protected:
	virtual bool CreateContent();
	virtual CWnd * GetContentWnd() { return &m_jobListCtrl; }
	virtual void SetToolBarButtons();

	void LButtonDblClk();


// Operations
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDropDown(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnUpdateSetFilterJobs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateClearFilterJobs(CCmdUI* pCmdUI);
};

#endif //__JOBVIEW__
/////////////////////////////////////////////////////////////////////////////
