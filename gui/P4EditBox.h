// P4EditBox.h: interface for the CP4EditBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_P4EditBox_H__36E8FE16_A6BC_11D4_8399_009027AF6042__INCLUDED_)
#define AFX_P4EditBox_H__36E8FE16_A6BC_11D4_8399_009027AF6042__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEBDropTarget : public COleDropTarget
{
// Construction
public:
    CEBDropTarget();

// Implementation
public:
    virtual ~CEBDropTarget();  
    
   //
   // These members MUST be overridden for an OLE drop target
   // See DRAG and DROP section of OLE classes reference
   //
   DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD 
                                                dwKeyState, CPoint point );
   DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD 
                                               dwKeyState, CPoint point );
   void OnDragLeave(CWnd* pWnd);               
   
   BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT 
                                          dropEffect, CPoint point );    
#ifdef USE_NATIVE
   void SetHWnd(HWND hwnd) { m_hWnd = hwnd;};
#endif

   // Owner Window
	CWnd* m_Owner;

	// Internal clipboard format
	CLIPFORMAT m_CF_DEPOT;

   // Drag info stored in OnDragEnter
	UINT m_DragDataFormat;   // clipboard data format (also indicates source)
	DROPEFFECT m_DropEffect;

	// List of depot files dropped onto CP4EditBox
	CStringList m_FileList;
};


class CP4EditBox : public CEdit  
{

public:
	CP4EditBox(CWnd* pParent=0);   // standard constructor
	virtual ~CP4EditBox();
	void SetParent(CWnd* pParent) { m_Parent = pParent; }

	CWnd* m_Parent;
	int m_SpecType;		// spec type of parent

	//////////////////////
	// OLE drag drop support
	CEBDropTarget *m_pDropTgt;

	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	LRESULT OnDropTarget(WPARAM wParam, LPARAM lParam);

// Operations
public:
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_P4EditBox_H__36E8FE16_A6BC_11D4_8399_009027AF6042__INCLUDED_)
