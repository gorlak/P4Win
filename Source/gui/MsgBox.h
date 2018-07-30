#if !defined(AFX_MSGBOX_H__F2CC8786_3D84_11D4_835F_009027AF6042__INCLUDED_)
#define AFX_MSGBOX_H__F2CC8786_3D84_11D4_835F_009027AF6042__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MsgBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMsgBox dialog

// This class expects a string resource formatted as a series of 1-3 button
// labels, followed by a prompt string.  The string is delimited by tab 
// characters.  The type param is a combination of messagebox style values
// used to specify the icon and default button.  The return value from the
// dialog is one of the IDC_BUTTON# values indicating which button was selected.
// cancelButton indicates which, if any, button is associated with canceling.
// This should be one of the IDC_BUTTON# values, or 0 if there is no cancel button.
// This determines what value will be returned if the user presses the escape key.

class CMsgBox : public CDialog
{
// Construction
public:
	CMsgBox(CString text, UINT nType = 1, int cancelButton = 0, CWnd* pParent = NULL, BOOL *lpBdontShow = NULL, BOOL bShowVscroll = FALSE);   // standard constructor
	BOOL IsDontShowAgain() { return m_DontShowAgain; }

// Dialog Data
	//{{AFX_DATA(CMsgBox)
	enum { IDD = IDD_MSGBOX };
	BOOL m_DontShowAgain;
	//}}AFX_DATA

	CRect m_InitRect;
	CRect m_LastRect;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMsgBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL *m_lpDontShowAgain;

	// Generated message map functions
	//{{AFX_MSG(CMsgBox)
	virtual BOOL OnInitDialog();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnButton3();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CString m_Text;
	CString m_Caption;
	CString m_Button1;
	CString m_Button2;
	CString m_Button3;
	int     m_DefButton;
	LPCTSTR	m_Icon;
    int     m_numButtons;
    int     m_cancelButton;
	BOOL	m_ShowVscroll;
};

inline int MsgBox(UINT nIDText, UINT nType =1, int cancelButton =0, CWnd* pParent =NULL, BOOL *lpBdontShow =NULL, BOOL bShowVscroll =FALSE)
{
	CMsgBox dlg(LoadStringResource(nIDText), nType, cancelButton, pParent, lpBdontShow, bShowVscroll);
	int rc = dlg.DoModal();
	if (lpBdontShow)
	   *lpBdontShow = dlg.IsDontShowAgain();
	return rc;
}

inline int MsgBox(CString text, UINT nType =1, int cancelButton =0, CWnd* pParent =NULL, BOOL *lpBdontShow =NULL, BOOL bShowVscroll =FALSE)
{
	CMsgBox dlg(text, nType, cancelButton, pParent, lpBdontShow, bShowVscroll);
	int rc = dlg.DoModal();
	if (lpBdontShow)
	   *lpBdontShow = dlg.IsDontShowAgain();
	return rc;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSGBOX_H__F2CC8786_3D84_11D4_835F_009027AF6042__INCLUDED_)
