//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// P4StatColl.cpp
#include "stdafx.h"
#include "P4Win.h"
#include "P4StatColl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CP4StatColl::CP4StatColl()
{
	m_BlockCount=0;
	m_fs[m_BlockCount]= new CP4FileStats *[BLOCK_ROWS];
	memset(m_fs[m_BlockCount], 0, sizeof(CP4FileStats *) * BLOCK_ROWS);
	m_BlockCount++;
}

CP4StatColl::~CP4StatColl()
{
	DestroyAll();
}

void CP4StatColl::SetStats(long rowIndex, CP4FileStats *fs)
{
	while(rowIndex >= m_BlockCount * BLOCK_ROWS)
	{
		m_fs[m_BlockCount]= new CP4FileStats *[BLOCK_ROWS];
		memset(m_fs[m_BlockCount], 0, sizeof(CP4FileStats *) * BLOCK_ROWS);
		m_BlockCount++;
	}

	int block=rowIndex/BLOCK_ROWS;
	int offset=rowIndex%BLOCK_ROWS;

	if(m_fs[block][offset] != 0)
		delete m_fs[block][offset];
	
	m_fs[block][offset]= fs;
	
}

CP4FileStats *CP4StatColl::GetStats(long rowIndex)
{
	int block=rowIndex/BLOCK_ROWS;
	int offset=rowIndex%BLOCK_ROWS;

	return m_fs[block][offset];
}

void CP4StatColl::DestroyAll()
{
	int offset;
	for(int block =0; block < m_BlockCount; block++)
	{
		for(offset=0; offset < BLOCK_ROWS; offset++)
		{
			if(m_fs[block][offset] != 0)
				delete m_fs[block][offset];
		}
		delete [] m_fs[block];
	}
	m_BlockCount=0;
}