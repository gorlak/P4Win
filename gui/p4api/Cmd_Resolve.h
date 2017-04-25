//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Resolve.h  
//
//	Used for any sort of bulk text output, as for a change description,
//	or as for a specification output command (e.g. 'p4 bramch -o branchname')
//

#include "P4Command.h"


class CCmd_Resolve : public CP4Command
{
    // Construction
public:
    CCmd_Resolve(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_Resolve)

    BOOL Run(LPCTSTR filespec, BOOL bForce=FALSE, BOOL bTextualMerge=FALSE);
    LPCTSTR GetBaseMergeFileName() const { return m_BaseMergeFileName; }
    LPCTSTR GetYourMergeFileName() const { return m_YourMergeFileName; }
    LPCTSTR GetTheirMergeFileName() const { return m_TheirMergeFileName; }
	void SetHeadIsText(BOOL b) { m_bHeadIsText = b; }
	BOOL GetHeadIsText() { return m_bHeadIsText; }

    BOOL GetResolved() const { return m_Resolved; }
protected:
    // Attributes	
    BOOL m_Resolved;
	BOOL m_bHeadIsText;
	CString m_filespec;
    CString m_BaseMergeFileName;
    CString m_YourMergeFileName;
    CString m_TheirMergeFileName;

    BOOL IsValidMergeMessage(CString const& data);
    BOOL HandleBinaryMergeMessage( CString const& data );

    // CP4Command overrides
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
    virtual BOOL HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg);
    virtual int OnResolve(ClientMerge *m, Error *e);
};









