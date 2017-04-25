//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// P4CommandStatus.h   

#ifndef __P4COMMANDSTATUS__
#define __P4COMMANDSTATUS__

class CP4Command;

#include <afxmt.h>
////////////////////////////////////////////////////////////////////////////
// The CP4CommStatus is a very simple class that maintains info re: communication
// with the server from one instance of a CP4Command to the next.  The App object
// must own one of these to ensure that it doesnt go out of scope prematurely
class CP4CommandStatus 
{
public:
	CP4CommandStatus();
			
protected:
	volatile int m_ServerLevel;
	volatile int m_SecurityLevel;
	volatile BOOL m_ServerBusy;
	volatile BOOL m_RequestAbort;
	volatile BOOL m_ServerNoCase;
	volatile BOOL m_ServerUnicode;
    volatile BOOL m_PWDError;
	volatile BOOL m_NoPWDSet;
	volatile BOOL m_PWDnotAllow;
    volatile int m_CommandNumber;

    CObList m_Queue;
	
public:
	inline int GetServerLevel() { return m_ServerLevel; }
	inline void SetServerLevel(int level) { m_ServerLevel=level;}

	inline int GetSecurityLevel() { return m_SecurityLevel; }
	inline void SetSecurityLevel(int level) { m_SecurityLevel=level;}

    inline BOOL GetPWDError() { return m_PWDError; }
    inline void SetPWDError(BOOL error) { m_PWDError = error; }

    inline BOOL GetNoPWDSet() { return m_NoPWDSet; }
    inline void SetNoPWDSet(BOOL error) { m_NoPWDSet = error; }

    inline BOOL GetPWDnotAllow() { return m_PWDnotAllow; }
    inline void SetPWDnotAllow(BOOL error) { m_PWDnotAllow = error; }

	inline BOOL IsServerNoCase() { return m_ServerNoCase; }
	inline void SetServerNoCase(BOOL noCase) { m_ServerNoCase= noCase; }

	inline BOOL IsServerUnicode() { return m_ServerUnicode; }
	inline void SetServerUnicode(BOOL unicode) { m_ServerUnicode= unicode; }

	void RequestAbort();
    inline BOOL IsAppAborting() { return m_RequestAbort; }
	
    BOOL GetServerLock( int &lock );
	void ReleaseServerLock( int &lock );
	void ClearServerBusy() { m_ServerBusy = false; }
	void SetServerBusy() { m_ServerBusy = true; }
    inline BOOL IsServerBusy() { return m_ServerBusy; }

    void QueueCommand( CP4Command *pCmd );
    void PumpQueue( );

	void Reset();
};


#endif //__P4COMMANDSTATUS__