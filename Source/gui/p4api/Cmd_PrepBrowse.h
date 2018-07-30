//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_PrepBrowse.h  
//
//

#include "P4Command.h"


class CCmd_PrepBrowse : public CP4Command
{
    // Construction
public:
    CCmd_PrepBrowse(CGuiClient *client=NULL);
    ~CCmd_PrepBrowse();
    DECLARE_DYNCREATE(CCmd_PrepBrowse)

    BOOL Run(LPCTSTR fileSpec, CString &fileType, long fileRev, BOOL bForce2Binary=FALSE);
    BOOL Run(BOOL bUseP4A, LPCTSTR fileSpec, CString &fileType, BOOL bAll=FALSE, BOOL bChg=FALSE, BOOL bNoHead=FALSE, long fileRev=-1, int whtSp=0, BOOL bIncInteg=FALSE);
	BOOL NoFileAtThatRev() { return m_NoFileAtRev; }
	BOOL IsAnnotating() { return m_Annotating; }
	BOOL UseP4A() { return m_bP4a; }
	int GetFileRev() { return m_FileRev; }

    LPCTSTR GetTempName() const { return m_TempName; }
    FileSys *GetTempFile() { return m_pOutputFile; }

	void SetFileType(FileSysType type) { m_Type = type; }
	FileSysType GetFileType() { return m_Type; }
	void SetTempFilelog(CString &fn) { m_TempFilelogName = fn; }
	CString &GetTempFilelog() { return m_TempFilelogName; }

protected:
    // Attributes
    FileSys *m_pOutputFile;
	FileSysType m_Type;		// only 3 values: FST_TEXT or FST_BINARY or FST_CANTTELL (if not set)
    int m_ByteCount;
	int m_FileRev;
    CString m_TempName;
	CString m_TempFilelogName;
	BOOL m_Annotating;
	BOOL m_bP4a;
	BOOL m_NoFileAtRev;

    BOOL SetupPrint(LPCTSTR fileSpec, CString &fileType, long fileRev, BOOL bForce2Binary);
				     
    // CP4Command overrides
    virtual void OnOutputText(LPCTSTR data, int length);
	virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
	virtual BOOL HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg);
};
