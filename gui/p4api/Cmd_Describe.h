//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Describe.h  
//
//	Used for any sort of bulk text output, as for a change description,
//	or as for a specification output command (e.g. 'p4 bramch -o branchname')
//

#include "P4Command.h"


class CCmd_Describe : public CP4Command
{
    // Construction
public:
    CCmd_Describe(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_Describe)

    BOOL Run(int descType, LPCTSTR reference, LPCTSTR templateName=NULL, BOOL force=FALSE, int flag=0, BOOL uFlag=FALSE);
    LPCTSTR GetReference() const { return m_Reference; }
    LPCTSTR GetDescription() const { return m_Description; }
	void SetCaller(CWnd *caller) { m_Caller = caller; }
    void SetDescription(CString str) { m_Description= str; }
    void SetSpecStr(LPCTSTR str) { m_SpecStr= str; }
	void SetListCtrl(CWnd *plc) { m_CallingListCtrl = plc; }
	void SetSpecDescDlg(CWnd *pWnd) { m_SpecDescDlg = pWnd; }
	CWnd * GetListCtrl() { return m_CallingListCtrl; }
	CWnd * GetCaller() { return m_Caller; }
	CWnd * GetSpecDescDlg() { return m_SpecDescDlg; }
    LPCTSTR GetSpecStr() const { return m_SpecStr; }
    BOOL IsLongSpec() const { return m_bLong; }
	int GetSpecType() { return m_SpecType; }
	int GetFlag() { return m_Flag; }

    // Attributes	
protected:
	CWnd * m_Caller;
	CWnd * m_CallingListCtrl;
	CWnd * m_SpecDescDlg;
    CString m_Reference;
    CString m_Description;
    CString m_SpecStr;
    int m_SpecType;
	int m_Flag;
    BOOL m_bLong;
	    
    // CP4Command overrides
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
    virtual void OnOutputText(LPCTSTR data, int length);
    virtual void OnOutputStat( StrDict *varList );
    virtual void PostProcess();
    virtual BOOL HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg);
};









