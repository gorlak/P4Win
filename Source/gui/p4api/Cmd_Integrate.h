//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Integrate.h  
//

#include "P4Command.h"


class CCmd_Integrate : public CP4Command
{
    // Construction
public:
    CCmd_Integrate(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_Integrate)
	~CCmd_Integrate();

    CStringList *GetList() { return &m_List; }
    BOOL Run(BOOL reverse, LPCTSTR branchName);
    BOOL Run(CStringList *source, CStringList *target,
            LPCTSTR branchName, 
            LPCTSTR revRange,
			LPCTSTR commonPath,
            BOOL isBranch, BOOL isReverse, BOOL isNoCopy,
            BOOL isForced, BOOL isForcedDirect, BOOL isRename, BOOL isPreview,
            int changeNum, BOOL isPermitDelReadd, int delReaddType,
            BOOL isBaselessMerge,  BOOL isIndirectMerge, 
			BOOL isPropagateTypes, BOOL isBaseViaDelReadd, 
			int branchFlag, BOOL bDontSync, BOOL bBiDir);

protected:
    // Attributes
	int m_ChangeNbr;
    BOOL m_Reverse;
    BOOL m_IsBranch;
    BOOL m_IsPreview;
	BOOL m_IsRename;
    BOOL m_SourceProvided;
    BOOL m_NewBranchSyntax;
    BOOL m_MultipleItems;
	BOOL m_DeleteChg;
	BOOL m_BiDir;
	CString m_CommonPath;
    CString m_Reference;
    CString m_RevRange;
    CStringList m_List;

    DWORD m_LastMessage;
    CStringArray *m_pStatusArray;
    BOOL m_ClobberFailed;	// clobber failed or invalid new file name was given
		    
    // CP4Command overrides
    virtual void PostProcess();
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msgtxt);
    virtual BOOL HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg);
    virtual void OnOutputError(char level, LPCTSTR errBuf, LPCTSTR errMsg);
    virtual BOOL NextListArgs();
};









