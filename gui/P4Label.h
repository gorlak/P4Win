//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// P4Label.h

#ifndef __P4LABEL__
#define __P4LABEL__

// Class CP4Label - a simple wrapper to parse and store info returned by 'P4 labels'

class CP4Label : public CObject
{
public:
	CP4Label();
	DECLARE_DYNCREATE(CP4Label)
	
protected:
	CString m_LabelName;
	CString m_Owner;
	CString m_Options;
	CString m_Date;
	CString m_Description;
	
	BOOL m_Initialized;
	
public:
	void Create(LPCTSTR name, LPCTSTR owner, LPCTSTR options, LPCTSTR date, LPCTSTR desc);
	BOOL Create(LPCTSTR labelRow);
	BOOL Create(StrDict *varlist);

	LPCTSTR GetLabelName() const {ASSERT(m_Initialized); return m_LabelName; }
 	LPCTSTR GetOwner() const {ASSERT(m_Initialized); return m_Owner; }
 	LPCTSTR GetOptions() const {ASSERT(m_Initialized); return m_Options; }
	LPCTSTR GetDate() const {ASSERT(m_Initialized); return m_Date; }
	LPCTSTR GetDescription() const {ASSERT(m_Initialized); return m_Description; }
		
	~CP4Label();

};

#endif //__P4LABEL__


