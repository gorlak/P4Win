//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Revert.h   
//

#include "P4Command.h"

class CCmd_Revert : public CP4Command
{
    // Construction
public:
    CCmd_Revert(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_Revert)
				    
    BOOL Run(CString &Revertspec,BOOL bChgList=FALSE,BOOL bUnChgd=FALSE,BOOL bPreview=FALSE, BOOL bVirtual=FALSE, BOOL bChkChgOnly=FALSE);
	BOOL Run(CStringList *files, BOOL bChgList=FALSE,BOOL bUnChgd=FALSE,BOOL bPreview=FALSE, BOOL bVirtual=FALSE, BOOL bChkChgOnly=FALSE);
    CStringList *GetFileList() { return &m_List; }
	BOOL OnlyUnChgd() { return m_UnChgd; }
	BOOL IsPreview() { return m_Preview; }
	int NbrNonEdits() { return m_NbrNonEdits; }
	void SetNbrNonEdits(int n) { m_NbrNonEdits = n; }
	LPCTSTR GetChgName() { return m_chgname; }
	void SetChgName(LPCTSTR str) { m_chgname = str; }

    // Attributes	
protected:
	BOOL m_UnChgd;
	BOOL m_Preview;
	BOOL m_ChkChgOnly;

	int m_NbrEdits;
	int m_NbrNonEdits;
    CStringList m_List;
    CStringList m_TempList;
	CString m_chgname;

    // CP4Command overrides
	virtual BOOL NextListArgs();
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
    virtual BOOL IsQueueable() const { return TRUE; }
	virtual BOOL HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg);
};
