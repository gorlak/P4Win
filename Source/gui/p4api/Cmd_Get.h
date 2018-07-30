//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Get.h   
//

#include "P4Command.h"

class CCmd_Get : public CP4Command
{
    // Construction
public:
    CCmd_Get(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_Get)
				    
    BOOL Run(CStringList *files, BOOL whatIf, BOOL bRefresh = FALSE );

    CStringList *GetGetList() { return &m_GetList; }
    CStringList *GetRecover() { return &m_Recover; }
    int ReiterateWarnings();
    CStringList *GetRemoveList() { return &m_RemoveList; }
    BOOL IsWhatIf() const { return m_WhatIf; }
    BOOL IsRunIntegAfterSync() const { return m_bIntegAfterSync; }
    void SetRunIntegAfterSync(BOOL b) { m_bIntegAfterSync = b; }
    BOOL IsOpenAfterSync() const { return m_bOpenAfterSync; }
    void SetOpenAfterSync(BOOL b) { m_bOpenAfterSync = b; }
    BOOL IsOpeningForEdit() const { return m_bOpeningForEdit; }
    void SetOpeningForEdit(BOOL b) { m_bOpeningForEdit = b; }
	void Add2Recover(CString &str) { m_Recover.AddHead(str); }
	int  GetNumberAdded() { return m_AddCount; }
	int  GetWarningsCount() { return m_Warnings.GetCount(); }
	void SetRevHistWnd(HWND hwnd) { m_RevHistWnd = hwnd; }
	HWND GetRevHistWnd() { return m_RevHistWnd; }
	void SetRevReq(int rev) { m_RevReq = rev; }
	int  GetRevReq() { return m_RevReq; }
	void Add2SelSet(HTREEITEM item) { m_SelectionSet.Add((DWORD) item); }
	CDWordArray *GetSelectionSet() { return &m_SelectionSet; }

    // Attributes	
protected:
	CDWordArray m_SelectionSet;	// saves selection set if we want to edit after sync
    CStringList m_GetList;
    CStringList m_RemoveList;
	CStringList m_Recover;
	int m_AddCount;

    // Track rows of output for the command, and store warnings along
    // the way.  
    // ReiterateWarnings() function can re-sprew the warnings or
    // summarize that there were no warnings.
    int m_OutputRows;
    CStringArray m_Warnings;

    BOOL m_WhatIf;
    BOOL m_bRefresh;
    BOOL m_bIntegAfterSync;
    BOOL m_bOpenAfterSync;
    BOOL m_bOpeningForEdit;

	// If sync is requested by Rev Hist dialog, 
	// the following are used to notify 
	// the Rev Hist dialog of a successful sync
	HWND m_RevHistWnd;
	int	 m_RevReq;

    void RemoveLastFromGetList();

    // CP4Command overrides
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
    virtual void OnOutputError(char level, LPCTSTR errBuf, LPCTSTR errMsg);
};









