//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_History.h   
//

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "P4Command.h"
#include "P4Lists.h"


class CCmd_History : public CP4Command
{
    // Construction
public:
    CCmd_History(CGuiClient *client=NULL);
	~CCmd_History();
    DECLARE_DYNCREATE(CCmd_History)

    BOOL Run(LPCTSTR fileSpec);
    CP4FileStats *GetFileStats() { ASSERT( m_pFileStats != NULL ); return m_pFileStats; }
    void SetCallingWnd(HWND hwnd) { m_CallingWnd = hwnd; }
    HWND GetCallingWnd() { return m_CallingWnd; }

	void SetIsAFile(BOOL b) { m_IsAFile = b; }
	BOOL IsAFile() { return m_IsAFile; }
    void SetInitialRev(int initialRev, CString initialName) { m_InitialRev = initialRev; m_InitialName = initialName; }
    int  GetInitialRev() const { return m_InitialRev; }
    LPCTSTR GetInitialName() const { return m_InitialName; }
    LPCTSTR GetFileName() const { return m_FileName; }

    CHistory *GetHistory() { return &m_History; }

	void OverrideFetchCompleteHist(int flag) { m_OverrideDashI = TRUE; m_DashIoverride = flag; }
	void OverrideFetchHistCount(int count) { m_OverrideDashM = TRUE; m_DashMoverride = count; }
	void SetEnableShowIntegs(BOOL b) { m_EnableShowIntegs = b; }
	BOOL GetEnableShowIntegs() { return m_EnableShowIntegs; }

	void SetWriteToTempFile(BOOL b) { m_WriteToTempFile = b; }
    LPCTSTR GetTempName() const { return m_TempName; }
    FileSys *GetTempFile() { return m_pOutputFile; }
	void SetKeyToHold(int k) { m_KeyToHold = k; }
	int GetKeyToHold() { return m_KeyToHold; }

    // Attributes	
protected:
    CHistory m_History;
	BOOL     m_IsAFile;

    CString m_FileName;
    int     m_FileNbr;
    CString m_TextOut;
    CString m_BranchInfo;
    CP4FileStats *m_pFileStats;
    HWND m_CallingWnd;
    int m_InitialRev;
    CString m_InitialName;

	// Commandline flag overrides: 
	// these override the registry values on a one time basis
	//	Set the m_OverrideDashX flag to TRUE to indicate to overide -x
	//	Set m_DashXoverride to the value to use for the -x flag
	BOOL m_OverrideDashI;
	int  m_DashIoverride;	// 0 -> plain; 1 -> -i; 2 -> -h
	BOOL m_OverrideDashM;
	int  m_DashMoverride;	// note 0 -> fetch all

	// Whether to show the include integs checkbox or not
	BOOL m_EnableShowIntegs;

	// Data for when the output is to be written to a temp file
	BOOL m_WriteToTempFile;
    CString m_TempName;
    FileSys *m_pOutputFile;
	int m_KeyToHold;

    // CP4Command overrides
    virtual void PreProcess(BOOL &done);
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
    virtual void PostProcess();
	virtual BOOL HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg);
};









