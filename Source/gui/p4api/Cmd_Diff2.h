//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// Cmd_Diff2.h  
//
//

#include "P4Command.h"


class CCmd_Diff2 : public CP4Command
{
    // Construction
public:
    CCmd_Diff2(CGuiClient *client=NULL);
    DECLARE_DYNCREATE(CCmd_Diff2)

    BOOL Run(LPCTSTR file1, LPCTSTR file2, 
			    int rev1, int rev2, 
				CString &fileType1, CString &fileType2, 
				BOOL bLocal1=FALSE, BOOL bLocal2=FALSE, BOOL bDoIt=FALSE);

    LPCTSTR GetInfoText() const { return m_InfoText; }
	void SetOutput2Dlg(BOOL b) { m_Output2Dlg = b; }
	BOOL IsOutput2Dlg() { return m_Output2Dlg; }
	CString &GetFileName(int i) { return m_FileName[i ? 1 : 0]; }

protected:
    CString m_InfoText;
    CString m_FileName[2];
    long m_FileRev[2];
    CString m_FileType[2];
	BOOL m_LocalFlag[2];
	BOOL m_DoIt;
	BOOL m_Output2Dlg;
		    
    // CP4Command overrides
    virtual void PreProcess(BOOL& done);
	virtual void OnOutputInfo(char level, LPCTSTR data, LPCTSTR msg);
	virtual BOOL HandledCmdSpecificError(LPCTSTR errBuf, LPCTSTR errMsg);
};









