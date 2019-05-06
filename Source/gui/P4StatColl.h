//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// P4StatColl.h

#ifndef __P4STATCOLL__
#define __P4STATCOLL__

#include "P4FileStats.h"

#ifdef _DEBUG
	#define BLOCK_ROWS 500       // Force it to use more than one block 
	#define MAX_BLOCKS   1000
#else
	#define BLOCK_ROWS 5000
	#define MAX_BLOCKS   100
#endif

class CP4StatColl
{
public:
	CP4StatColl();
	~CP4StatColl();

protected:
	CP4FileStats **m_fs[MAX_BLOCKS];
	long m_BlockCount;

public:
	void SetStats(LONG_PTR rowIndex, CP4FileStats *fs);
	CP4FileStats *GetStats(LONG_PTR rowIndex);
	void DestroyAll();
	
};

#endif //__P4TATCOLL__
