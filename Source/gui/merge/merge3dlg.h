// merge3dlg.h : header file
//

#ifndef __MERGE3DLG__
#define __MERGE3DLG__
/////////////////////////////////////////////////////////////////////////////
// CMerge3Dlg dialog
#include "GuiClientMerge.h"
#include "CoolBtn.h"
#include "WinPos.h"

class CMerge3Dlg : public CDialog
{
// Construction
public:
	CMerge3Dlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMerge3Dlg( );

	void SetMergeInfo(CGuiClientMerge *merge) { m_pMerge= merge; }
	void SetForceFlag(BOOL forceFlag) { m_ForcedResolve = forceFlag; }
	void SetTextualFlag(BOOL textualMerge) { m_TextualMerge = textualMerge; }
	void SetRunMerge(BOOL bRunMerge) { m_bRunMerge = bRunMerge; }
	void SetKey(int key) { m_Key = key; }

// Dialog Data
	//{{AFX_DATA(CMerge3Dlg)
	enum { IDD = IDD_MERGE3 };
	int		m_BothChunks;
	int		m_YourChunks;
	int		m_ConflictChunks;
	int		m_TheirChunks;
	CString	m_TheirFile;
	CString	m_YourFile;
	CString	m_BaseFile;
	CString m_ConflictsRemaining;
	int		m_FilesFlag;
	CCoolButton  m_DiffBtn;
	//}}AFX_DATA

	CRect m_InitRect;
	CWinPos m_WinPos;
	int m_LastWidth;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMerge3Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CGuiClientMerge *m_pMerge;
	int m_Key;
	int m_DepotDiffSw;
	BOOL m_ForcedResolve;
	BOOL m_TextualMerge;
	BOOL m_bRunMerge;
	BOOL m_bHeadIsText;
	CStringList m_StringList;
	CString m_itemStr;

	// MD5 for the original version of the m_pMerge->ResultFile()
	StrBuf m_MD5;

	BOOL Verify( LPCTSTR txt );
	void CheckResult();
	void Diff(FileSys *file1, FileSys *file2, LPCTSTR flag1=NULL, LPCTSTR display1=NULL, LPCTSTR flag2=NULL, LPCTSTR display2=NULL);
	BOOL Edit(FileSys *file);
	void Merge(FileSys *base, FileSys *theirs, 
				FileSys *yours, FileSys *result,
				CString theirFileName, CString baseFileName);
	void OnAcceptmerged();
	void OnAccepttheirs();
	void OnAcceptyours();
	void OnEditmerged();
	void OnViewtheirs();
	void OnEdityours();
	void Getdepotfile();

	// Generated message map functions
	//{{AFX_MSG(CMerge3Dlg)
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	afx_msg void OnAccept();
	afx_msg void OnDiff();
	afx_msg void OnDiffMenu();
	afx_msg void OnDiffmerged();
	afx_msg void OnDiffyours();
	afx_msg void OnDifftheirs();
	afx_msg void OnDiffyourstheirs();
	afx_msg void OnDifftheirsyours();
	afx_msg void OnDiffyoursmerged();
	afx_msg void OnDifftheirsmerged();
	afx_msg void OnDiffyoursdepot();
	afx_msg void OnDifftheirsdepot();
	afx_msg void OnDiffdepotmerged();
	afx_msg void OnEdit();
	afx_msg void OnRunmerge();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnCancelAll();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnFileInformation();
	afx_msg void OnFileRevisionhistory();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	LRESULT OnP4FileInformation( WPARAM wParam, LPARAM lParam );
	LRESULT OnP4EndFileInformation(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4PrepBrowse( WPARAM wParam, LPARAM lParam );
};

#endif //__MERGE3DLG__
