// FlatSplitter.h : 
//

#ifndef __FLATSPLITTER__
#define __FLATSPLITTER__	

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif


class CFlatSplitter : public CSplitterWnd
{
	DECLARE_DYNCREATE(CFlatSplitter)

// Attributes
public:
  
public:
   	CFlatSplitter();
   	
// Operations
public:
    
    
// Implementation
public:
	virtual ~CFlatSplitter();

	virtual void OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rect);

	DECLARE_MESSAGE_MAP()
};

#endif //__FLATSPLITTER__	
/////////////////////////////////////////////////////////////////////////////
