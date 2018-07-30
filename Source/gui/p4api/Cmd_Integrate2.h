//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Integrate2.h  
//
//

#include "P4Command.h"
	

class CCmd_Integrate2 : public CP4Command
{
    // Construction
public:
    CCmd_Integrate2(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_Integrate2)

    BOOL Run( CStringList *source, CStringList *target,
                LPCTSTR branchName, LPCTSTR revRange, LPCTSTR commonPath,
                BOOL isBranch, BOOL isReverse, BOOL isNoCopy,
                BOOL isForced, BOOL isForcedDirect, BOOL isRename, BOOL isPreview,
                int changeNum, BOOL isPermitDelReadd, int delReaddType, 
                BOOL isBaselessMerge, BOOL isIndirectMerge, 
				BOOL isPropagateTypes,BOOL isBaseViaDelReadd,
				int branchFlag, BOOL bDontSync, BOOL bBiDir, int newChangeNbr );
    CObList *GetTargetList() { return &m_TargetList; }
    CStringList *GetPreviewTargetList() { return &m_PreviewTargetList; }
    int GetTargetChange() const { return m_ChangeNum; }
	BOOL HitFatalError() const { return m_HitFatalError; }
    BOOL IsPreview() const { return m_IsPreview; }
    BOOL IsRename() const { return m_IsRename; }
    BOOL IsOK2rename() const { return m_OK2rename; }
    BOOL IsRunSyncAfterPreview() const { return m_RunSyncAfterPreview; }
    void SetRunSyncAfterPreview(BOOL b) { m_RunSyncAfterPreview = b; };
	int  GetActualCount() { return m_ActualCount; }
	int  GetNewChangeNbr() { return m_NewChangeNbr; }

    // Attributes	
protected:
    CObList m_TargetList;
    CStringList m_PreviewTargetList;

    CStringList *m_pSourceList;
    CStringList *m_pTargetList;
    CString m_BranchName;
    CString m_RevRange;
	CString m_CommonPath;
    BOOL m_IsBranch;
    BOOL m_IsReverse;
    BOOL m_IsNoCopy;
    BOOL m_IsForced;
    BOOL m_IsForcedDirect;
    BOOL m_IsRename;
    BOOL m_IsPreview;
    int  m_ChangeNum;
    BOOL m_RunSyncAfterPreview;
    BOOL m_IsPermitDelReadd;
    int  m_DelReaddType;
    BOOL m_IsBaselessMerge;
    BOOL m_IsIndirectMerge;
    BOOL m_IsPropagateTypes;
    BOOL m_IsBaseViaDelReadd;
	int  m_BranchFlag;
	BOOL m_DontSync;
	BOOL m_BiDir;
	int  m_NewChangeNbr;
	int  m_ActualCount;
	BOOL m_OK2rename;
	BOOL m_HitFatalError;

    BOOL ParseIntegResults( CString& line, CString& source, CString& dest );	

    // CP4Command overrides
    virtual void PreProcess(BOOL& done);
};









