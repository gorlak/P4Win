/*
 * Copyright 1997, 1999 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */


// SpecDescDlg.h : header file
//

#include "WinPos.h"
#include "P4Menu.h"
#include "CoolBtn.h"
#include "RichEdCtrlEx.h"
#include "cmd_describe.h"

#define GET_X_LPARAM(lp)	((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)	((int)(short)HIWORD(lp))

#define	HS_ISAFILE		0x01
#define	HS_ISACHG		0x02
#define	HS_ISABRANCH	0x04
#define	HS_ISALABEL		0x08
#define	HS_ISACLIENT	0x10
#define	HS_ISAUSER		0x20
#define	HS_ISAJOB		0x40
#define	HS_ISAEMAIL		0x80
#define	HS_ISAURL		0x81
#define	HS_ISDIFF2		0x82

/////////////////////////////////////////////////////////////////////////////
// a subclass of CButton to pass Ctrl+F, F3 and Shift F3 to the parent window
class CKeyDownButton : public CButton
{
public:
    CKeyDownButton(CWnd* pParent=NULL);
	virtual ~CKeyDownButton();

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// a subclass of CButton to draw arrows to the left of the text
// The main point of this class is to draw an arrow image on a button.
// This is complicated because CButton and CBitmapButton don't seem
// able to accomplish this simple feat.  So an owner drawn button is
// required.  But once you set the BS_OWNERDRAW style, you are responsible
// for making it look right either with or without XP visual styles.  And
// hot-tracking and defaulting don't work normally when you set BS_OWNERDRAW.
// So there is extra code here to get the desired hot-tracking and
// defaulting behavior.
class CArrowButton : public CButton
{
	CImageList m_il;		// an image list with arrows
	HMODULE m_themeLib;		// handle to UxTheme.dll
	bool m_bOverControl;	// is the mouse over us?
	bool m_bTracking;		// are we tracking the mouse
	BOOL m_bDefault;		// is this button the default button?

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	afx_msg UINT OnGetDlgCode();
	afx_msg LRESULT OnSetStyle(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
public:
    CArrowButton();
	~CArrowButton();
    bool m_bUp;
};

/////////////////////////////////////////////////////////////////////////////
// CSpecDescDlg dialog

class CSpecDescDlg : public CDialog
{
// Construction
public:
	CSpecDescDlg(CWnd* pParent = NULL);  // standard constructor
	virtual ~CSpecDescDlg();

protected:
	// Store a ptr to the calling command
	CCmd_Describe *m_pCallingCommand;

	CWnd* m_pParent;
	CWnd* m_CallingListCtrl;
	CWnd* m_Caller;
	CWinPos m_WinPos;
	CRect m_InitRect;
	int m_Key;

	CString m_Item;
	CString m_Caption;
	CString m_ReportedByTitle;
	CString m_Description;
    LPCWCH m_DescriptionW;
	CString m_ItemStr;
	CFont m_Font;
	BOOL m_Modeless;
	BOOL m_ScrollPastComments;
	int m_SkipLines;
	BOOL m_ShowNextPrev;
	BOOL m_ShowShowDiffs;
	BOOL m_ShowShowFixes;
	BOOL m_ShowShowFiles;
	BOOL m_ShowEditBtn;
	BOOL m_TurnOnReDraw;
	BOOL m_ChkServerBusy;
	BOOL m_bDiffOutput;
	int m_viewType;
	int m_DiffFlag;

	CString m_LabelFiles;
	int m_LabelFileCount;

	COLORREF m_Grey;
	CBrush m_GreyBrush;

	CStringList m_StringList;
	CString m_Viewer;	
	BOOL m_ViewFileIsText;

	CDWordArray m_HotSpotBgn;
	CDWordArray m_HotSpotEnd;
	CByteArray m_HotSpotType;
    int m_numHotSpots;
	DWORD m_LButtonDownTime;
	CString m_SelItem;
	int  m_SelType;
	BOOL m_DoNotActivate;
	BOOL m_MoreThan256Colors;
	BOOL m_RevHistEnable;
	BOOL m_HasBeenMinimized;

	LPCWCH m_TO_;

	UINT m_fPopup;
	UINT m_fMarked;
	UINT m_fFile;
	UINT m_fHist;
	UINT m_fRev;
	UINT m_fProp;
	UINT m_fChg;
	UINT m_fItem;
	UINT m_fCli;
	UINT m_fUser;
	UINT m_fJob;
	UINT m_fEmail;
	UINT m_fURL;
	UINT m_fClTk;
	UINT m_fDiff2;

	CFindReplaceDialog *m_pFRDlg;
	CString m_FindWhatStr;
	int m_FindWhatFlags;

	void ScrollPastComments();
#ifdef UNICODE
#undef IDD_SPECDESC
#define IDD_SPECDESC IDD_SPECDESCW
#endif
// Dialog Data
	//{{AFX_DATA(CSpecDescDlg)
	enum { IDD = IDD_SPECDESC };
	CRichEdCtrlEx	m_Text;
	CArrowButton	m_PrevBtn;
	CArrowButton	m_NextBtn;
	CCoolButton		m_btShowDiffs;
	CKeyDownButton	m_CloseBtn;
	CKeyDownButton	m_PrintBtn;
	CKeyDownButton	m_EditBtn;
	CKeyDownButton	m_ShowFixesBtn;
	CKeyDownButton	m_ShowFilesBtn;
	//}}AFX_DATA
	CToolTipCtrl* m_pToolTip;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpecDescDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetIsModeless(BOOL b) { m_Modeless = b; }
	void SetWinPosName(LPCTSTR name) { m_WinPos.SetWindow(this, name); }
	void SetWinPosDefault(CRect &rect) { m_WinPos.SetDefaultPos(rect); }
	void SetCallingCommand( CCmd_Describe *pCmd, BOOL bModeless=TRUE );
	void SetDescription(LPCTSTR txt, BOOL scrollPastComments=TRUE);
	void SetCaption(LPCTSTR txt) { m_Caption= txt; }
	void SetReportedByTitle(LPCTSTR txt) { m_ReportedByTitle= txt; }
	void SetShowNextPrev(BOOL b) { m_ShowNextPrev = b; }
	void SetShowShowDiffs(BOOL b) { m_ShowShowDiffs = b; }
	void SetShowShowFixes(BOOL b) { m_ShowShowFixes = b; }
	void SetShowShowFiles(BOOL b) { m_ShowShowFiles = b; }
	void SetShowEditBtn(BOOL b) { m_ShowEditBtn = b; }
	void SetDiffFlag(int flag) { m_DiffFlag = flag; }
	void SetFindStrFlags(CString *s, int f) { m_FindWhatStr = *s, m_FindWhatFlags = (f|FR_DOWN) & ~FR_HIDEWHOLEWORD; }
	void SetKey(int key) { m_Key = key; }
	void SetViewType(int vt) { m_viewType = vt; }
	int  GetViewType() { return m_viewType; }
	void SetListCtrl(CWnd *plc) { m_CallingListCtrl = plc; }
	CWnd * GetListCtrl() { return m_CallingListCtrl; }
	void SetCaller(CWnd *caller) { m_Caller = caller; }
	CWnd * GetCaller() { return m_Caller; }
	void SetItemName(LPCTSTR txt) { m_Item= txt; }
	LPCTSTR GetItemName() { return m_Item; }
	LPCTSTR GetDescription() { return m_Description; }
	LRESULT OnP4ViewFile(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Fixes(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4FileInformation(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4EndFileInformation(WPARAM wParam, LPARAM lParam);
protected:

	void CreateTheFont();
    void SetEditText();
	void SetMenuFlags();
	void MakeSmartSelection();
	void OnDescItem(HWND hWnd, int viewType, int flag = 0);
	void OnDescChgLong(int flag);
	void SetHotSpots();
	int  AddHotSpotWord(int index, int offset, int lineStart, int lgth, BOOL bAtSign);
	int  AddHotSpotFile(int index, int offset, int lineStart, int lgth, BOOL bQuoted);
	int  IsItaHotSpot(int nStartChar, int nEndChar);
	BOOL PumpMessages( );
	LRESULT OnP4Diff2(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4Describe( WPARAM wParam, LPARAM lParam );

    void OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu);

    void OnMsgfilterDescription(NMHDR* pNMHDR, LRESULT* pResult);
    void OnPageSetup();
	void OnShowDiffs(int flag);


	// Generated message map functions
	//{{AFX_MSG(CSpecDescDlg)
public:
	afx_msg void OnClose();
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnOK();
	afx_msg void OnNextitem();
	afx_msg void OnPrevitem();
	afx_msg void OnShowDiffsBtn();
	afx_msg void OnShowDiffsNormal();
	afx_msg void OnShowDiffsSummary();
	afx_msg void OnShowDiffsUnified();
	afx_msg void OnShowDiffsContext();
	afx_msg void OnShowDiffsRCS();
	afx_msg void OnShowDiffsNone();
	afx_msg void OnUpdateShowDiffsNormal(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowDiffsSummary(CCmdUI* pCmdUI); 
	afx_msg void OnUpdateShowDiffsUnified(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowDiffsContext(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowDiffsRCS(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowDiffsNone(CCmdUI* pCmdUI); 
	afx_msg void OnPrint();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnSync();
	afx_msg void OnUpdateSync(CCmdUI* pCmdUI);
	afx_msg void OnPositionDepot();
	afx_msg void OnUpdatePositionDepot(CCmdUI* pCmdUI);
	afx_msg void OnDiffHead();
	afx_msg void OnUpdateDiffHead(CCmdUI* pCmdUI);
	afx_msg void OnDiffPrev();
	afx_msg void OnUpdateDiffPrev(CCmdUI* pCmdUI);
	afx_msg void OnDiffCliFile();
	afx_msg void OnUpdateDiffCliFile(CCmdUI* pCmdUI);
	afx_msg void OnFileAutobrowse();
	afx_msg void OnUpdateFileAutobrowse(CCmdUI* pCmdUI);
	afx_msg void OnFileAnnotate();
	afx_msg void OnUpdateFileAnnotate(CCmdUI* pCmdUI);
	afx_msg void OnFileInformation();
	afx_msg void OnUpdateFileInformation(CCmdUI* pCmdUI);
	afx_msg void OnFileRevisionTree();
	afx_msg void OnFileRevisionhistory();
	afx_msg void OnUpdateFileRevisionhistory(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnEditCopy2();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnDescChg();
	afx_msg void OnUpdateDescChg(CCmdUI* pCmdUI);
	afx_msg void OnDescBranch();
	afx_msg void OnUpdateDescBranch(CCmdUI* pCmdUI);
	afx_msg void OnDescLabel();
	afx_msg void OnUpdateDescLabel(CCmdUI* pCmdUI);
	afx_msg void OnDescClient();
	afx_msg void OnUpdateDescClient(CCmdUI* pCmdUI);
	afx_msg void OnDescUser();
	afx_msg void OnUpdateDescUser(CCmdUI* pCmdUI);
	afx_msg void OnDescJob();
	afx_msg void OnUpdateDescJob(CCmdUI* pCmdUI);
	afx_msg void OnDiff2();
	afx_msg void OnUpdateDiff2(CCmdUI* pCmdUI);
	afx_msg void OnEmail();
	afx_msg void OnUpdateEmail(CCmdUI* pCmdUI);
	afx_msg void OnURL();
	afx_msg void OnUpdateURL(CCmdUI* pCmdUI);
	afx_msg void OnCallTrack();
	afx_msg void OnUpdateCallTrack(CCmdUI* pCmdUI);
	afx_msg void OnEditSelectAll();
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnOptions();
	afx_msg void OnUpdateOptions(CCmdUI* pCmdUI);
    afx_msg void OnShowfixes();
    afx_msg void OnShowfiles();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnQuickHelp();
	afx_msg void OnHelp();
	afx_msg void OnEditButton();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnUpdatePositionToPattern(CCmdUI* pCmdUI);
	afx_msg void OnPositionToPattern();
	afx_msg void OnUpdatePositionToPrev(CCmdUI* pCmdUI);
	afx_msg void OnPositionToPrev();
	afx_msg void OnUpdatePositionToNext(CCmdUI* pCmdUI);
	afx_msg void OnPositionToNext();
	afx_msg LRESULT OnFindReplace(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	LRESULT OnP4LabelContents(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4EndDescribe(WPARAM wParam, LPARAM lParam);
	LRESULT OnNewClient(WPARAM wParam, LPARAM lParam);
	LRESULT OnNewUser(WPARAM wParam, LPARAM lParam);
	LRESULT OnFindPattern(WPARAM wParam, LPARAM lParam);
	LRESULT OnQuitting(WPARAM wParam, LPARAM lParam);
};
