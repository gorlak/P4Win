//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Changes.h   
//

#include "P4Command.h"
#define ALL_CHANGES			1
#define PENDING_CHANGES		2
#define SUBMITTED_CHANGES	3


class CCmd_Changes : public CP4Command
{
    // Construction
public:
    CCmd_Changes(CGuiClient *client=NULL);
    ~CCmd_Changes();
    DECLARE_DYNCREATE(CCmd_Changes)

    BOOL Run(int filter, int loquatious, CStringList *viewSpec=NULL, long numToFetch=0, BOOL inclInteg=FALSE, CString *user=NULL, CString *client=NULL);
	CObArray *GetChanges() { return &m_Changes; }

    // Attributes	
protected:
    CObList *m_pBatch;
    CObArray m_Changes;
	CString m_User;
	CString m_Client;

    // CP4Command overrides
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
    virtual void PostProcess();
};









