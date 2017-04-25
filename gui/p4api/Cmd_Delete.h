//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Delete.h  
//
//	Used for any sort of bulk text output, as for a change description,
//	or as for a specification output command (e.g. 'p4 bramch -o branchname')
//

#include "P4Command.h"

#define P4BRANCH_DEL	1
#define P4CHANGE_DEL	2
#define P4CLIENT_DEL	3
#define P4JOB_DEL		4
#define P4LABEL_DEL		5
#define P4USER_DEL		6
		

class CCmd_Delete : public CP4Command
{
    // Construction
public:
    CCmd_Delete(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_Delete)

    BOOL Run(int delType, LPCTSTR reference);
    LPCTSTR GetCompletionData() const { return m_CompletionData; }
    LPCTSTR GetCompletionMessage() const { return m_CompletionMsg; }
	BOOL IgnoreActiveItem() { return m_IgnoreActiveItem; }
	void SetIgnoreActiveItem(BOOL b) { m_IgnoreActiveItem = b; }
	void SetSwitch2User(LPCTSTR user) { m_Switch2User = user; }
	LPCTSTR GetSwitch2User() { return m_Switch2User; }

protected:
    // Attributes	
    CString m_CompletionData;
    CString m_CompletionMsg;
	BOOL m_IgnoreActiveItem;
	CString m_Switch2User;
	    
    // CP4Command overrides
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
    virtual BOOL HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg);
};









