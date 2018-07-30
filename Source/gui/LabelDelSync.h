#if !defined(AFX_LABELDELSYNC_H__CAE3D423_BCE4_11D5_8F53_009027AF6042__INCLUDED_)
#define AFX_LABELDELSYNC_H__CAE3D423_BCE4_11D5_8F53_009027AF6042__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LabelDelSync.h : header file
//

#include "WinPos.h"

/////////////////////////////////////////////////////////////////////////////
// CLabelDelSync dialog

class CLabelDelSync : public CDialog
{
// Construction
public:
	CLabelDelSync(CWnd* pParent = NULL);   // standard constructor

protected:
	HWND m_LabelWnd;
	CString m_Active;
	CString m_Caption;
	CString m_BtnText;
	CReviewList *m_List;
	CFont m_Font;
	CStringArray m_LabelFiles;
	int m_LabelFileCount;
	int m_SortFlag;				// Bits: 1=>some files checked; 2=>some file not checked
	CStringList * m_SelectedList;
	CStringList * m_OutputList;
	CWinPos m_WinPos;
	CRect m_InitRect;
	CRect m_LastRect;
	BOOL m_InitDialogDone;
	UINT m_ReturnMsg;

// Dialog Data
	//{{AFX_DATA(CLabelDelSync)
	enum { IDD = IDD_LABEL_DEL_SYNC };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLabelDelSync)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLabelDelSync();
	void SetActive(LPCTSTR txt) { m_Active= txt; }
	void SetCaption(LPCTSTR txt) { m_Caption= txt; }
	void SetBtnText(LPCTSTR txt) { m_BtnText= txt; }
	void SetSelected(CStringList *plist) { m_SelectedList = plist; }
	void SetOutputList(CStringList *plist) { m_OutputList = plist; }
	void SetReturnMsg(UINT msg) { m_ReturnMsg = msg; }

protected:

	void ReloadList();

	// Generated message map functions
	//{{AFX_MSG(CLabelDelSync)
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnPreview();
	afx_msg void OnCancel();
	afx_msg void OnSelectAll();
	afx_msg void OnUnselectAll();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	LRESULT OnP4LabelContents(WPARAM wParam, LPARAM lParam);
	void OnFinish();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LABELDELSYNC_H__CAE3D423_BCE4_11D5_8F53_009027AF6042__INCLUDED_)
