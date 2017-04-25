//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Diff.h   
//

#include "P4Command.h"

typedef	struct _TWOSTRINGS
{
	RUNAPPTHREADINFO rati;
	LPTSTR			 str1;
	LPTSTR			 str2;
}	TWOSTRINGS;

class CCmd_Diff : public CP4Command
{
    // Construction
public:
    CCmd_Diff(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_Diff)
				    
    BOOL Run(CStringList *files, LPCTSTR filter=NULL, TCHAR sFlag='\0', BOOL bThreadWait=FALSE);

    int GetDiffRunCount() const {return m_DiffRunCount; }
    int GetDiffErrCount() const {return m_DiffErrCount; }
    LPCTSTR GetDiffErrBuf() const {return m_DiffErrBuf; }
    void Diff( FileSys *f1, FileSys *f2, int doPage, char *diffFlags, Error *e );
    CStringList *GetList() { return &m_StrListOut; }
    int GetDiffNbrFiles() const;
    LPCTSTR GetDiffFileName() const;
    TCHAR GetSflag() const { return m_sFlag; }
	CString RenameFileExt(CString file1, LPCTSTR ext1, LPCTSTR ext2);

    // Attributes	
protected:
    int m_DiffRunCount;
    int m_DiffErrCount;
    CString m_DiffErrBuf;
    TCHAR m_sFlag;
	BOOL m_ThreadWait;

    // CP4Command overrides
    virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
    virtual BOOL HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg);
};









