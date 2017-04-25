// merge2dlg.h : header file
//

#ifndef __MERGE2DLG__
#define __MERGE2DLG__

#include "GuiClientMerge.h"
#include "CoolBtn.h"
#include "WinPos.h"

/////////////////////////////////////////////////////////////////////////////
// CMerge2Dlg dialog

class CMerge2Dlg : public CDialog
{
// Construction
public:
	CMerge2Dlg(CWnd* pParent = NULL);   // standard constructor

	void SetMergeInfo(CGuiClientMerge *merge) { m_pMerge= merge; }
	void SetKey(int key) { m_Key = key; }

// Dialog Data
	//{{AFX_DATA(CMerge2Dlg)
	enum { IDD = IDD_MERGE2 };
	CString	m_TheirFile;
	CString	m_YourFile;
	int		m_FilesFlag;
	CCoolButton  m_DiffBtn;
	//}}AFX_DATA

	CRect m_InitRect;
	CWinPos m_WinPos;
	int m_LastWidth;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMerge2Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CGuiClientMerge *m_pMerge;
	int m_DepotDiffSw;
	int m_Key;
	CString m_itemStr;
	CStringList m_StringList;

	BOOL Verify( LPCTSTR txt );
	void Getdepotfile();
	void OnAccepttheirs();
	void OnAcceptyours();
	BOOL Edit(FileSys *file);
	void Diff(FileSys *file1, FileSys *file2, LPCTSTR flag1=NULL, LPCTSTR display1=NULL, LPCTSTR flag2=NULL, LPCTSTR display2=NULL);

	// Generated message map functions
	//{{AFX_MSG(CMerge2Dlg)
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnAccept();
	afx_msg void OnDiff();
	afx_msg void OnDiffMenu();
	afx_msg void OnDiffyoursdepot();
	afx_msg void OnDifftheirsdepot();
	afx_msg void OnEdit();
	afx_msg void OnEdittheirs();
	afx_msg void OnEdityours();
	afx_msg void OnCancelAll();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnFileInformation();
	afx_msg void OnFileRevisionhistory();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	LRESULT OnP4FileInformation( WPARAM wParam, LPARAM lParam );
	LRESULT OnP4EndFileInformation(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4PrepBrowse( WPARAM wParam, LPARAM lParam );
};

#endif //__MERGE2DLG__
