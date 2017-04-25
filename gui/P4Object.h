//
// Copyright 2003 Perforce Softwares.  All rights reserved.
//
//


// P4Object.h

#ifndef __P4OBJECT__
#define __P4OBJECT__

// Class CP4Object - a simple wrapper to store a P4 Object (Client, User, Label, Branch, etc)

class CP4Object : public CObject
{
public:
	CP4Object();
	DECLARE_DYNCREATE(CP4Object)
	
protected:
	CStringArray m_Field;
	int m_Count;
	BOOL m_Initialized;
	
public:
	void Create(LPCTSTR name);
	void AddField(LPCTSTR data) { ASSERT(m_Initialized); m_Field.Add(data); m_Count++; }
	LPCTSTR GetField(int i) const { ASSERT(m_Initialized); return m_Field[i]; }
	int GetCount() const { ASSERT(m_Initialized); return m_Count; }
		
	~CP4Object();

};

#endif //__P4OBJECT__


