//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// GuiClientMerge.h   
//
// CGuiClientMerge is a ClientMerge wrapper
// that add a synchronization mechanism
//

#ifndef __GUICLIENTMERGE__
#define __GUICLIENTMERGE__

class CGuiClientMerge
{
public:
    CGuiClientMerge(ClientMerge *pMerge)
        : m_pMerge(pMerge)
    {
	    m_hEvent= CreateEvent(NULL, FALSE, FALSE, NULL);
    }
	void WaitForSignal() { WaitForSingleObject(m_hEvent, INFINITE); }
	BOOL Signal() { return SetEvent(m_hEvent); }

	FileSys *YourFile() { return m_pMerge->GetYourFile(); }
	FileSys *TheirFile() { return m_pMerge->GetTheirFile(); }
	FileSys *BaseFile() { return m_pMerge->GetBaseFile(); }
	FileSys *ResultFile() { return m_pMerge->GetResultFile(); }

	int GetBothChunks() const { return m_pMerge->GetBothChunks(); }
	int GetYourChunks() const { return m_pMerge->GetYourChunks(); }
	int GetConflictChunks() const { return m_pMerge->GetConflictChunks(); }
	int GetTheirChunks() const { return m_pMerge->GetTheirChunks(); }

    bool IsAcceptable() const { return m_pMerge->IsAcceptable() == 1; }

    void SetFilespec(LPCTSTR filespec) { m_filespec = filespec; }
    CString GetFilespec() { return m_filespec; } 
    void SetBaseFileName(LPCTSTR baseFileName) { m_baseFileName = baseFileName; }
    CString BaseFileName() { return m_baseFileName; } 
    void SetYourFileName(LPCTSTR yourFileName) { m_yourFileName = yourFileName; }
    CString YourFileName() { return m_yourFileName; } 
    void SetTheirFileName(LPCTSTR theirFileName) { m_theirFileName = theirFileName; }
	CString TheirFileName() { return m_theirFileName; }
	void SetHeadIsText(BOOL b) { m_bHeadIsText = b; }
	BOOL GetHeadIsText() { return m_bHeadIsText; }

    void SetStatus(MergeStatus status) { m_status = status; }
    MergeStatus GetStatus() const { return m_status; }

    void CheckResultFile()
    {
        m_pMerge->IsAcceptable();
    }
protected:
    ClientMerge *m_pMerge;
	HANDLE m_hEvent;
    MergeStatus m_status;
	CString m_filespec;
    CString m_baseFileName;
    CString m_yourFileName;
    CString m_theirFileName;
	BOOL m_bHeadIsText;
};

/////////////////////////////////////////////////////////////////////////////
#endif //__GUICLIENTMERGE__






