//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Jobs.h   
//

#include "P4Command.h"

class CCmd_Jobs : public CP4Command
{
    // Construction
public:
    CCmd_Jobs(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_Jobs)
				    
    BOOL Run(LPCTSTR jobStatus=NULL,BOOL bFastJobs=TRUE,CStringList *viewSpec=NULL,BOOL bIncIntegs=FALSE);

    void SetFilter( BOOL set ) { m_bFilter = set; }
    CObList *GetList() { return &m_List; }	

    CStringArray & GetFieldNames() { return m_FieldNames; }
    CDWordArray & GetFieldCodes() { return m_FieldCodes; }
    // Attributes	
protected:
    CObList m_List;
    BOOL m_bFilter;

    CStringArray m_FieldNames;
    CDWordArray  m_FieldCodes;

    // P4Command overrides
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
    virtual void OnOutputStat( StrDict *varList );
};


class CCmd_JobSpec : public CP4Command
{
    // Construction
public:
    CCmd_JobSpec( CGuiClient *client = NULL );
    virtual ~CCmd_JobSpec( ) { if ( m_pSpec ) delete m_pSpec ; }
    DECLARE_DYNCREATE( CCmd_JobSpec )
				    
    BOOL Run(  );

    void GetSpec( CString &spec );

    // Attributes	
protected:
    CString *m_pSpec;

    // CP4Command overrides
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg );
    virtual void PostProcess();
};








