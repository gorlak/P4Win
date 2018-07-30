// SortListHeader.h : header file
//

#ifndef __SORTLISTHDR__
#define __SORTLISTHDR__

/////////////////////////////////////////////////////////////////////////////
// CSortListHeader window

class CSortListHeader : public CHeaderCtrl
{
// Construction
public:
	CSortListHeader();

// Attributes
protected:
	int	m_SortColumn;
	BOOL m_SortAscending;
public:

// Operations
public:
	int SetSortImage( int Column, BOOL Ascending );
	void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSortListHeader)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSortListHeader();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSortListHeader)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif //__SORTLISTHDR__
/////////////////////////////////////////////////////////////////////////////
