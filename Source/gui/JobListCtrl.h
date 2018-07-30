//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// JobListCtrl.h : header file
//

#ifndef __JOBLISTCTRL__
#define __JOBLISTCTRL__

#include "P4ListCtrl.h"
#include "P4Job.h"

// Note: this value must be <= MAX_P4OBJECTS_COLUMNS in P4ListCtrl.h
#define	MAX_JOBS_COLUMNS	16

class CJobListCtrl : public CP4ListCtrl
{
public:
	CJobListCtrl();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CJobListCtrl)

// Attributes
public:
	CString m_SpecText;
    int  m_PostListToChangeNum;
    HWND m_PostListToChangeWnd;

protected:
	CString m_ActiveJob;  // Name of job being edited or deleted
	BOOL m_NewJob;        // Does an edit operation refer to a new job
	BOOL m_Need2DoNew;	  // New Menuitem click still needs to be handled
	CString m_Spec;		  // Job Spec - gotten from OnP4JobSpecColumnNames()
	CP4Job *m_pNewSpec;
    CArray<int, int> m_ColCodes;
	CStringArray m_ColNames;	// Column names for the ListView
	CStringArray m_FieldNames;	// Names of the fields as given in the JobSpec
	CStringArray m_DesiredCols;	// Column names or numbers from Registry
	CStringArray m_SpecNames;	// Names of ALL the fields in the JobSpec
	BOOL m_FastJobs;			// True=>p4 jobs; FALSE=p4 -Ztag jobs
	BOOL m_FilterIncIntegs;		// True->p4 jobs -i
	CStringList m_FilterView;	// Files name(s) or path(s) for filtering
	CStringList m_StrList;		// Temp string list
	BOOL m_Need2CallOnJobConfigure;	// Need to call OnJobConfigure() after getting spec

	// Internal clipboard formats
	CLIPFORMAT m_CF_JOB;
	CLIPFORMAT m_CF_DEPOT;

	// Drag info stored in OnDragEnter
	UINT m_DragDataFormat;   // clipboard data format (also indicates source)
	DROPEFFECT m_DropEffect;

// Operations
public:
	void Clear();
	void EditTheSpec(CString *name);
	CString SetCaption( );

private:
	CString m_sFilter;
	void EditSpec();
	DWORD GetNumCols( void );
	BOOL GetFldNames( CStringArray & , const CString &spec );
	int GetFieldNbr( CString str, const CString &spec );
	LRESULT OnP4JobSpecColumnNames( WPARAM wParam, LPARAM lParam );
	BOOL m_bAlreadyGotColumns;
	void GetJobs( );
	void GetJobSpec( );


protected:
	BOOL TryDragDrop( );
	DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point); 
	DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point); 
	BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point); 
	void InsertJob(CP4Job *job, int index);
	void UpdateJob(CP4Job *job, int index);
	int FindJob(LPCTSTR jobName);
	CString GetSelectedJob();
	void OnJobEditspec(CString *jobname);
	inline CP4winApp *TheApp() { return (CP4winApp *) AfxGetApp(); }
	virtual BOOL OKToDelete( ) { return TRUE; }

	void PersistentJobFilter( REGSAM );

// Overrides

// Implementation
public:
	virtual ~CJobListCtrl();
	virtual int OnCompareItems(LPARAM lParam1, LPARAM lParam2, int subItem);
	void ViewUpdate() { OnViewUpdate(); }
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
public:
	afx_msg void OnViewUpdate();
	afx_msg void OnUpdateSetFilterJobs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateClearFilterJobs(CCmdUI* pCmdUI);
protected:
	afx_msg void OnUpdateViewUpdate(CCmdUI* pCmdUI);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnUpdateJobDelete(CCmdUI* pCmdUI);
	afx_msg void OnJobDelete();
	afx_msg void OnUpdateJobEditspec(CCmdUI* pCmdUI);
	afx_msg void OnJobEditspec();
	afx_msg void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateJobDescribe(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	afx_msg void OnUpdateJobSetFilter(CCmdUI* pCmdUI);
	afx_msg void OnJobSetFilter();
	afx_msg void OnUpdateJobRemovefilter(CCmdUI* pCmdUI);
	afx_msg void OnJobRemovefilter();
	afx_msg void OnUpdateJobSetFileFilter(CCmdUI* pCmdUI);
	afx_msg void OnJobSetFileFilter();
	afx_msg void OnJobSetFileFilterInteg();
	afx_msg void OnUpdateJobRemoveFileFilter(CCmdUI* pCmdUI);
	afx_msg void OnJobRemoveFileFilter();

	afx_msg void OnUpdateJobNew(CCmdUI* pCmdUI);
	afx_msg void OnJobNew() ;
	afx_msg void OnUpdateJobConfigure(CCmdUI* pCmdUI);
	afx_msg void OnJobConfigure();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPerforceOptions();
	LRESULT OnP4JobSpec(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4EndSpecEdit(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4JobList(WPARAM wParam, LPARAM lParam);
    LRESULT OnQueryJobs( WPARAM wParam, LPARAM lParam );
    LRESULT OnQueryJobSpec( WPARAM wParam, LPARAM lParam );
    LRESULT OnQueryJobFields( WPARAM wParam, LPARAM lParam );
	LRESULT OnQueryJobColumns( WPARAM wParam, LPARAM lParam );
	LRESULT OnQueryJobSelection( WPARAM wParam, LPARAM lParam );
    LRESULT OnFetchJobs( WPARAM wParam, LPARAM lParam );
	LRESULT OnJobFilter2( WPARAM wParam, LPARAM lParam );
	LRESULT OnClear( WPARAM wParam, LPARAM lParam );

public:
	void OnDescribeJob();
	void OnJobFilter3() { OnJobSetFilter(); }

	DECLARE_MESSAGE_MAP()
};

#endif //__JOBLISTCTRL__
/////////////////////////////////////////////////////////////////////////////
