//
// Copyright 1998 Perforce Software.  All rights reserved.
//
//

// CCmd_ListOpStat.h  
//
//
// This command is required to accomodate the 98.2 api.  
//
// In the 97.3 gui:
// We cached the results of fstat for (at a minimum) all files in the client 
// view.  So the sparse results the server returned for commands like EDIT, 
// DELETE, LOCK, UNLOCK, GET, UNGET, REVERT were sufficient since those results
// could be combined with info cached in CDepotView.  The CDepotView::
// ProcessListResults() handler would combine the data, and use the results
// to update itself as well as the CDeltaView.
//
// In the 98.2 gui:
// We call CCmd_ListOp first.  Then we do a MINIMUM amount of work to pull
// together the info required to update CDepotView and CDeltaView.  This 
// extra work amounts to:
// 1) Nothing for LOCK and UNLOCK, since these files must already be present in
//    the CDeltaView
// 2) Nothing for REVERT, since all we need is the filename to remove it from
//    the CDeltaView
// 3) Nothing for REOPEN, since the involved files are already in the CDeltaView
// 4) For EDIT and DELETE, we always know which of our changes the new CDeltaView
//    entries will be placed under.  So the efficient command is to run OSTAT
//    for the specific change.
//
// In all cases, the extra steps are completed on a single server connection, to
// minimize connection overhead.  As a future enhancement, it may be reasonable 
// to do the extra work only for arg lists that include folders, since all non-folder
// arg list items must already be present in CDepotView.


#include "P4Command.h"
	

class CCmd_ListOpStat : public CP4Command
{
    // Construction
public:
    CCmd_ListOpStat(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_ListOpStat)

    // command is P4EDIT, P4DELETE, P4LOCK, P4UNLOCK, P4REVERT
    BOOL Run(CStringList *files, int command, long changeNum=0, LPCTSTR newType= NULL);

    int GetCommand() { return m_Command; }
    CStringList *GetList() { return &m_StrListOut; }  // Basic text results
    CObList *GetStatList() { return &m_StatList; }	  // the StatList where applicable
	BOOL HitMaxFileStats() { return m_StatList.GetCount() > MAX_FILESTATS; }
    void DeleteStatList();
    BOOL GetOutputErrFlag() { return m_OutputError; }
    BOOL GetOpenAfterDelete() { return m_OpenAfterDelete; }
    int  GetSelectedChange() { return m_SelectedChange; }
    void SetOpenAfterDelete( BOOL b, int selectedChange ) { m_OpenAfterDelete = b; m_SelectedChange = selectedChange;}
    void SetSync2Head( BOOL b ) { m_Sync2Head = b; }
    BOOL GetSync2Head() { return m_Sync2Head; }
    void SetChkForSyncs( BOOL b ) { m_ChkForSyncs = b; }
    BOOL GetChkForSyncs() { return m_ChkForSyncs; }
    void SetRevertUnchgAfter( BOOL b ) { m_RevertUnchgAfter = b; }
    BOOL GetRevertUnchgAfter() { return m_RevertUnchgAfter; }
	void SetNbrChgedFilesReverted( int i ) { m_NbrChgedFilesReverted = i; }
	int  GetNbrChgedFilesReverted() { return m_NbrChgedFilesReverted; }
	void SetWarnIfLocked( BOOL b ) { m_WarnIfLocked = b; }
    CStringList *GetUnsynced() { return &m_Unsynced; }
    CStringList *GetRevertIfCancel() { return &m_RevertIfCancel; }
    CStringList *GetRevertAdds() { return &m_RevertAdds; }
    void Add2RevertList( CObject const * obj, int iAction );

    // Attributes	
protected:
    CStringList *m_pFileSpecs;
    int m_Command;
    long m_ChangeNumber;
    CString m_NewType;
    BOOL m_OutputError;
    BOOL m_OpenAfterDelete;
    BOOL m_ChkForSyncs;
    BOOL m_Sync2Head;
	BOOL m_WarnIfLocked;
	BOOL m_RevertUnchgAfter;
	int m_NbrChgedFilesReverted;
    int m_SelectedChange;

    CObList m_StatList;
    CStringList m_Unresolved;
    CStringList m_Unsynced;
    CStringList m_RevertIfCancel;
    CStringList m_RevertAdds;

    void PrepareStatInfo();

    // CP4Command overrides
    virtual void PreProcess(BOOL& done);
};









