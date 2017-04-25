/*
 * Copyright 1997, 1999 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */


// Cmd_EditSpec.h  
//
//	Use to get a spec and then edit it w/ dialog
//

#include "P4Command.h"
#include "p4specsheet.h"

#define	m_SpecDlg	m_SpecSheet->m_P4SpecDlg

class CCmd_EditSpec : public CP4Command
{
    // Construction
public:
    CCmd_EditSpec(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_EditSpec)

    BOOL Run(int descType, LPCTSTR reference=NULL, CObject *item=NULL);
    BOOL Run( int descType, LPCTSTR newName, LPCTSTR tName, CObject *item);
    BOOL Run(long changeNum, BOOL allowSubmit, BOOL force=FALSE, BOOL submitOnlyChged=FALSE, BOOL submitOnlySeled=FALSE, BOOL uFlag=FALSE);

    int GetNewChangeNum() const { return m_NewChangeNum; }
    void SetNewChangeNum( int num ) { m_NewChangeNum= num; }
    LPCTSTR GetItemName() const { return m_ItemName; }
    LPCTSTR	GetChangeDesc() const { return m_ChangeDesc; }
    void SetChangeDesc( LPCTSTR desc ) { m_ChangeDesc= desc; }
    LPCTSTR GetNewJobName() const { return m_NewJobName; }
    void SetNewJobName( LPCTSTR name ) { m_NewJobName= name; }
    LPCTSTR GetOldClient() const { return m_OldClient; }
    void SetOldClient( LPCTSTR name ) { m_OldClient= name; }
	void SetNewClientRoot( LPCTSTR name ) { m_NewRoot= name; }
	void SetAutoUpdateSpec (BOOL b) { m_AutoUpdateSpec = b; }
	BOOL IsAutoUpdateSpec () { return m_AutoUpdateSpec; }
	void SetSpecForceSwitch (BOOL b) { m_SpecForceSwitch = b; }
	BOOL IsSpecForceSwitch () { return m_SpecForceSwitch; }
    BOOL EditedLists() const { return m_SpecDlg.EditedLists(); }
    BOOL IsForceEdit() const { return m_ForceEdit; }
	BOOL IsUFlag() const { return m_uFlag; }
    void SetIsNewClient( BOOL b) { m_IsNewClient = b; };
    BOOL GetIsNewClient() const { return m_IsNewClient; };
    void SetIsNewUser( BOOL b) { m_IsNewUser = b; };
    BOOL GetIsNewUser() const { return m_IsNewUser; };
    void SetIsRequestingNew( BOOL b) { m_IsRequestingNew = b; };
    BOOL GetIsRequestingNew() const { return m_IsRequestingNew; };
    void SetUseLocalDefTmplate( BOOL b) { m_UseLocalDefTmplate = b; };
    BOOL GetUseLocalDefTmplate() const { return m_UseLocalDefTmplate; };
    void SetCaller( CView *lv) { m_Caller = lv; };
    CView *GetCaller() { return m_Caller; };
	int IsSyncAfter() { return m_SyncAfter; }

    int GetSpecDlgExit() const { return m_SpecDlgExit; }
    LPCTSTR GetSpecStr() const { return m_SpecStr; }
    LPCTSTR GetSpecIn() const { return m_SpecIn; }
    LPCTSTR GetSpecOut() const { return m_SpecOut; }
    void SetSpecIn(LPCTSTR specIn) { m_SpecIn = specIn; }
    LPCTSTR GetTemplateName() const { return m_TemplateName; }
	BOOL PreprocessChgSpec();
    BOOL DoSpecDlg(CWnd* caller);
	void EndSpecDlg(int exitCode);
	CP4SpecSheet *GetSpecSheet() { return m_SpecSheet; }


    //		ripped off from dmchange.cc
    //
    static LPCTSTR g_blankDesc; // = "<enter description here>";


protected:
    // Attributes
    CString m_SpecIn;
    CString m_SpecOut;
    CString m_SpecStr;
    CString m_ChangeDesc;
    CString m_TemplateName;

    int m_SpecType;
    CP4SpecSheet *m_SpecSheet;

    BOOL m_IsNewClient;
    BOOL m_IsNewUser;
    BOOL m_IsRequestingNew;
    BOOL m_AllowChangeSubmit;
    BOOL m_SubmitOnlyChged;
	BOOL m_SubmitOnlySeled;
    BOOL m_ForceEdit;
	BOOL m_uFlag;
    BOOL m_CreateFromTemplate;
	BOOL m_UseLocalDefTmplate;
    CString m_ItemName;
    CView* m_Caller;
	HWND m_CallingWnd;

    // For changes and jobs, a new name may be assigned by server
    int m_OldChangeNum;
    int m_NewChangeNum;
    CString m_NewJobName;
	CString m_OldClient;
	CString m_NewRoot;
	BOOL m_AutoUpdateSpec;
	BOOL m_SpecForceSwitch;

    int m_SpecDlgExit;
    CObject *m_pSpecObj;

	// We might have to run a sync after editing a spec
	int m_SyncAfter;
	    
protected:

    // CP4Command overrides
    virtual void PreProcess(BOOL& done);
};
