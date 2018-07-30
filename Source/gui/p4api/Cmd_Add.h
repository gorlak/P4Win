//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Add.h  
//
//	Use to get a spec and then edit it w/ dialog
//

#include "P4Command.h"
	

class CCmd_Add : public CP4Command
{
    // Construction
public:
    CCmd_Add(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_Add)

    BOOL Run(int changeNum, CStringList *files, CStringList *files2edit=NULL);
    void MergeOpenInfo(CObList *pOpenList);
    int GetAddedFileCount() const { return m_StrListOut.GetCount(); }

    CObList const *GetList() const { return &m_List; }			
    int GetChangeNum() const { return m_ChangeNum; }
    CStringList const & GetStr2Edit() const { return m_Str2Edit; }
    int GetOpenAction() const {return m_OpenAction; }

    void SetOpenAction(int action) { m_OpenAction = action; }

protected:
    // Attributes	
    CObList m_List;
    int m_ChangeNum;
    CStringList m_Str2Edit;
    int m_OpenAction;

    // CP4Command overrides
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
    virtual void PostProcess();
};
