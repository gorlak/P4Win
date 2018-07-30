//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// P4FileStats.h

#ifndef __P4FILESTATS__
#define __P4FILESTATS__

// File actions
enum FileAction
{
	F_NOACTION,
	F_UNKNOWNACTION,
	F_ADD,
	F_EDIT,
	F_DELETE,
	F_BRANCH,
	F_INTEGRATE,
	F_IMPORT,
	
	F_MAXACTION
};

// File types
enum FileType
{
	F_UNKNOWNFILETYPE,
	F_TEXT,
	F_CTEXT,
	F_CXTEXT,
	F_LTEXT,
	F_KTEXT,
	F_TTEXT,
	F_XTEXT,
	F_XLTEXT,
	F_KXTEXT,
	F_BINARY,
	F_TBINARY,
	F_UBINARY,
	F_XBINARY,
	F_SYMLINK,
	F_RESOURCE,
	F_TEMPOBJ,
	F_TEMPXOBJ,
	F_UNICODE,
	F_XUNICODE,
	f_UFT16,
	

	F_MAXTYPE
};


// Flags
#define FILE_MY_LOCK 0x04
#define FILE_OTHER_LOCK 0x08
#define FILE_UNRESOLVED 0x04

// Class CP4FileStats - a simple class to store results of 'P4 fstat'
class CP4FileStats : public CObject
{
public:
	CP4FileStats();
	DECLARE_DYNCREATE(CP4FileStats)

	~CP4FileStats();

	
protected:
	// Storage for all info returned by FSTAT.  
	CString m_DepotPath;
	CString m_ClientPath;
	CString m_OtherUsers;

	BYTE m_MyOpenAction;
	BYTE m_OtherOpenAction;
	BYTE m_HeadAction;
	CString m_HeadType;
	CString m_Type;
	BYTE m_Unresolved;
	BYTE m_Resolved;
	BYTE m_OtherLock;
	BYTE m_MyLock;
	BYTE m_OtherUserMyClient;
	CString m_ActionOwner;
	CString m_Digest;
		
	long m_HeadRev;
	long m_HaveRev;
	int m_OtherOpens;
	unsigned long m_FileSize;
	long m_HeadChangeNum;
	long m_HeadTime;
	long m_OpenChangeNum;

	LPARAM m_UserParam;
	BOOL m_NotInDepot;

public:
// Creation and assignment members
	void Clear();
	void Create( CP4FileStats *stats );
	BOOL Create(StrDict *client);
	BOOL Create(LPCTSTR openRow);  // temporary, till change # in fstat output
	BOOL Create(LPCTSTR depotName, long changeNumber);  // Used for file add
	BOOL Create(LPCTSTR localsyntax, LPCTSTR depotsyntax);	// Used for files not under Perforce control

	//TODO: Set functions are weak.  If a file is set to not open, it should not be locked
	//      etc
	void SetOpenAction(int action, BOOL otherUser);
	void SetLocked(BOOL locked, BOOL otherUser);
	inline void SetHeadRev(long rev) {ASSERT(rev >= m_HaveRev); m_HeadRev=rev; }
	void SetHaveRev(long rev); 
	void SetHeadAction(int action);
	inline void SetUnresolved(BOOL unresolved) {m_Unresolved=(BYTE)unresolved;}
	inline void SetResolved(BOOL resolved) {m_Resolved=(BYTE)resolved;}
	void SetOtherOpens(int num);
	void SetHeadType(int type);
	void SetHeadType(LPCTSTR txttype);
	void SetType(int type);
	void SetType(LPCTSTR txttype);
	void SetDigest(CString *digest) { m_Digest = *digest; }
	inline void SetOpenChangeNum(long change) { ASSERT(change >= 0); m_OpenChangeNum=change; }
	inline void SetUserParam(LPARAM parm) {m_UserParam = parm; }
	inline void SetNotInDepot(BOOL b) {m_NotInDepot = b; }

	void SetClosed();

	void SetDepotPath(LPCTSTR path);
	void SetClientPath(LPCTSTR path);
	void SetOtherUsers(LPCTSTR userlist);

	// Data access members
	inline int GetMyOpenAction() const {return (int) m_MyOpenAction;}
	inline int GetOtherOpenAction() const {return (int) m_OtherOpenAction;}
	inline BOOL IsOtherUserMyClient() const {return m_OtherUserMyClient; }
	inline BOOL IsMyLock() const {return m_MyLock;}
	inline BOOL IsOtherLock() const {return m_OtherLock;}
	inline BOOL IsMyOpen() const {return (m_MyOpenAction > 0) ;}
	inline BOOL IsOtherOpen() const {return (m_OtherOpenAction > 0) ;}
	inline BOOL IsOpen() const {return (m_MyOpenAction > 0 || m_OtherOpenAction > 0); }
	inline long GetHeadRev() const {return m_HeadRev;}
	inline long GetHaveRev() const {return m_HaveRev;}
	inline int GetHeadAction() const {return m_HeadAction;}
	inline CString GetHeadType() const {return m_HeadType;}
	inline CString GetType() const {return m_Type;}
	inline CString GetDigest() const {return m_Digest;}
	inline unsigned long GetFileSize() const {return m_FileSize;}
	inline long GetHeadChangeNum() const {return m_HeadChangeNum;}
	inline long GetHeadTime() const {return m_HeadTime;}
	
	inline long GetOpenChangeNum() const {return m_OpenChangeNum;}
	inline BOOL InClientView() const {return (m_ClientPath.GetLength() > 0); }
	inline BOOL IsUnresolved() const {return m_Unresolved;}
	inline BOOL IsResolved() const {return m_Resolved;}
	inline long GetOtherOpens() const {return m_OtherOpens;}
	LPCTSTR GetOtherUsers() const {return m_OtherUsers;}
	inline LPARAM GetUserParam() const {return m_UserParam; }
	inline BOOL IsNotInDepot() const {return m_NotInDepot; }

	BOOL IsTextFile() const;
	BOOL IsMyOpenExclusive() const;
	BOOL IsOtherOpenExclusive() const;

	CString GetActionStr(int action) const;

	LPCTSTR GetFullDepotPath() const {return m_DepotPath;}
	LPCTSTR GetFullClientPath() const {return m_ClientPath;}
	CString GetClientDir() const;
	CString GetDepotDir() const;
	CString GetDepotFilename() const;
	CString GetClientFilename() const;
	CString GetFormattedChangeFile(BOOL showFileType, BOOL showOpenAction) const;
	CString GetFormattedFilename(BOOL showFileType) const;
	CString GetFormattedHeadTime();

};

#endif //__P4FILESTATS__
