//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// P4CommandStatus.h 
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "p4win.h"
#include "p4command.h"

CCriticalSection g_cSection;



CP4CommandStatus::CP4CommandStatus()
{ 
    m_CommandNumber=0;
    m_ServerBusy=FALSE;
	m_ServerUnicode=TRUE;
	Reset();
} 

void CP4CommandStatus::Reset()
{
	m_ServerLevel=0;
	m_SecurityLevel=0;
    m_RequestAbort=FALSE;
	m_ServerNoCase=FALSE;
}

void CP4CommandStatus::RequestAbort() 
{ 
	ASSERT(AfxGetThread() == (CWinThread*) AfxGetApp()); 
	m_RequestAbort=TRUE; 
}

BOOL CP4CommandStatus::GetServerLock( int &lock )
{
    BOOL success= FALSE;
    lock=0;

    g_cSection.Lock();
    if( !m_ServerBusy )
    {
        lock= ++m_CommandNumber;
        success= TRUE;
        m_ServerBusy=TRUE;
    }
    g_cSection.Unlock();

    return success;
}

void CP4CommandStatus::ReleaseServerLock( int &lock )
{
    ASSERT( lock == m_CommandNumber );
    ASSERT( m_ServerBusy );

    lock= 0;
    g_cSection.Lock();
    m_ServerBusy=FALSE;
    PumpQueue();
    g_cSection.Unlock();
}

// When a server lock is released, the next item in the queue is 
// started by spawning a new thread
void CP4CommandStatus::PumpQueue( )
{
    g_cSection.Lock();
    POSITION pos= m_Queue.GetHeadPosition();
    if( pos != NULL )
    {
        // Get the top command out of the queue, and give it a key
        CP4Command *pCmd= (CP4Command *) m_Queue.GetAt(pos);
        m_Queue.RemoveAt(pos);
        ASSERT_KINDOF(CP4Command, pCmd);
        pCmd->SetServerKey(++m_CommandNumber);

        // And start the command
        pCmd->AsyncExecCommand();
	    
        // Server will be busy till the just-spawned command is done
        m_ServerBusy= TRUE;
    }
    g_cSection.Unlock();
}

void CP4CommandStatus::QueueCommand( CP4Command *pCmd )
{
    ASSERT_KINDOF(CP4Command, pCmd);

    g_cSection.Lock();
    m_Queue.AddTail(pCmd);
    if( !m_ServerBusy )
        PumpQueue();    
    g_cSection.Unlock();
}