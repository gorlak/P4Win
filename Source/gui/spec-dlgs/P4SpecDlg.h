/*
 * Copyright 1997, 1999 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */


// P4SpecDlg.h : header file
//
#ifndef __SPECDLG__
#define __SPECDLG__

#include "reviewlist.h"
#include "P4EditBox.h"

//      IDOK already defined as 1
//      IDCANCEL already defined as 2
//      IDABORT already defined as 3
#define IDNEEDTOREFRESH	4
//		IDIGNORE already defined as 5
#define IDALTERNATE 6



//		A name that no user will likely use, so that
//		branch -o and label -o will spew specs
//
#define NEWSPECNAME "x_new_spec_name_x"



// Macro to make a control ID that is unique to each perforce spec code.
//
// Ultimately, there will need to be matching entries in the resource.hm file
// for each spec code that has a help context.  If spec codes change, all the
// help entry numbers will have to be checked - the bane of help.

#define IDC(specType, specCode) (100 + (10*specType) + specCode)

#include "P4SpecData.h"
#include "WinPos.h"

class CCmd_EditSpec;
class CDeltaView;

// Child window attribute and position tracking
//
enum WindowType
{
	CHILD_BUTTON= 1,
	CHILD_MULTILINEEDIT,
	CHILD_CHECKLISTBOX,
	CHILD_STATIC,
	CHILD_SINGLELINEEDIT,
	CHILD_DROPLIST,
	CHILD_CHECKBOX,
	CHILD_RIGHTSTATIC
};

class CChildWindow
{
protected:
	CRect m_OrigRect;
	WindowType m_WinType;
	BOOL m_IsVisible;
	BOOL m_IsEnabled;
	CWnd *m_pWnd;
	int m_MaxLines;
	BOOL m_IsHalfWidth;
	BOOL m_IsIndent2Middle;
	BOOL m_IsIndent2Right;

public:
	CChildWindow()
	{
		m_IsVisible = TRUE;
		m_IsEnabled = TRUE;
		m_pWnd = 0;
	}
	CChildWindow(CWnd *pWnd, WindowType type, LPCRECT rect, 
		bool isEnabled = TRUE, int maxLines=0, 
		bool isHalfWidth=FALSE, bool isIndent2Middle=FALSE, bool isIndent2Right=FALSE)
	{
		m_IsVisible = TRUE;
		m_IsEnabled = isEnabled;
		m_pWnd = pWnd;
		m_WinType = type;
		m_OrigRect = rect;
		m_MaxLines = maxLines;
		m_IsHalfWidth = isHalfWidth;
		m_IsIndent2Middle = isIndent2Middle;
		m_IsIndent2Right  = isIndent2Right;
	}
	void SetVisible( BOOL vis ) { m_IsVisible= vis; }
	BOOL GetVisible() { return m_IsVisible; }

	void SetEnabled( BOOL enabled ) { m_IsEnabled= enabled; }
	BOOL GetEnabled() { return m_IsEnabled; }

	void SetWindow( CWnd *pWnd ) { m_pWnd= pWnd; } 
	CWnd *GetWindow() { return m_pWnd; }

	void SetOrigRect( const LPRECT rect ) { m_OrigRect= CRect(rect); }
	CRect GetOrigRect() { return CRect(m_OrigRect); }

	void SetType( const WindowType winType) { m_WinType= winType; }
	WindowType GetType() { return m_WinType; }

	void SetMaxLines( const int maxLines) { m_MaxLines= maxLines; }
	int GetMaxLines() { return m_MaxLines; }

	void SetIsHalfWidth( BOOL isHalfWidth ) { m_IsHalfWidth= isHalfWidth; }
	BOOL IsHalfWidth() { return m_IsHalfWidth; }

	void SetIsIndent2Middle( BOOL isIndent2Middle ) { m_IsIndent2Middle= isIndent2Middle; }
	BOOL IsIndent2Middle() { return m_IsIndent2Middle; }

	void SetIsIndent2Right( BOOL isIndent2Right ) { m_IsIndent2Right= isIndent2Right; }
	BOOL IsIndent2Right() { return m_IsIndent2Right; }
};


struct SpecControl
{
	CStatic * label;
	CWnd * control;
	int code;
	CString tip;
	BOOL isChkList;

	SpecControl() { label = 0; control = 0; code = 0; isChkList = 0;}

	CStatic * CreateLabel(CWnd * parent, LPCRECT rect, LPCTSTR prompt);
	CStatic * CreateLabel(CWnd *parent);
	CComboBox * CreateCombo(CWnd *parent, LPCRECT rect, DWORD style, HMENU menu);
	CButton * CreateCheckBox(CWnd *parent, LPCRECT rect, DWORD style, int id, LPCTSTR prompt);
	CReviewList * CreateList(CWnd *parent, LPCRECT rect, DWORD style, HMENU menu, int code);
	CP4EditBox * CreateEdit(CWnd *parent, LPCRECT rect, DWORD style, HMENU menu, int code, BOOL allowDD=FALSE, int specType=0);
	CP4EditBox * CreateEdit(CWnd *parent, int code);
	void AddToolTip(LPCTSTR prompt, CString const & instructionText);
};

/////////////////////////////////////////////////////////////////////////////
// CP4SpecDlg dialog

class CP4SpecDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CP4SpecDlg)

// Construction
public:
	CP4SpecDlg();
	~CP4SpecDlg();


// Dialog Data
	//{{AFX_DATA(CP4SpecDlg)
	enum { IDD = IDD_SPECDLG };
	CStatic	m_BusyMessage;
	CStatic	m_ReqStatic;
//}}AFX_DATA

	BOOL m_bReopen;
	int m_UnchangedFlag;
	BOOL m_bIsModal;

protected:
	int m_StdHeight;	// standard static, edit dimension
	int m_StdWidth;
	int m_MaxWidth;		// max width that fits dlg
	int m_HlfWidth;		// width that fits 1/2 dlg
	int m_StdSpaceV;	// standard vertical control spacing
	int m_StdSpaceH;
	int m_ComboWidth;

	// Sizing support
public:
	CWinPos m_WinPos;
	CSize m_MinSize;	// Initially zeroed, then set in initdialog
protected:
	int	m_VscrollMax;
	int m_VscrollPos;
	int m_ScreenHeight;
	int m_LogPixelsX;
	int m_MinLi;

	// Store a ptr to the calling command
	CCmd_EditSpec *m_pCallingCommand;

	// Store the handle of the calling window
	HWND m_CallingWnd;

    // Save spec comments for tool tip support
    CString m_InstructionText;  

	// Was the submit button hit?
	BOOL m_Ok;

	// Is the server busy with CP4SpecDlg's task?  
	BOOL m_SendingSpec;
	
	int m_X;  // X indent for controls
	int m_Y;  // Y coord of next control
	int m_Width;
	CDWordArray m_LiY;
	CString m_PrevCBPmt;

	CArray<SpecControl,SpecControl> m_specControls;
	CReviewList* m_jobList;
	CReviewList* m_fileList;
	CArray<CChildWindow,CChildWindow> m_childControls;
	int m_NumMultiLineChildWnds;

	CImageList *m_pImageList;

	//		variables for all spec info in server code
	//
	CString m_SpecDefStr;
	CharString m_SpecDefStrA;
	CString m_OrigSpecDefStr;
	CSpecData m_SpecData;
	Spec m_Spec;
	int m_FoundLineElemWithValues;
	BOOL m_HasRequired;

	CString GetNewValue( CWnd *pControl, int i );

	CString m_NewChangeDesc;
	BOOL m_EditedLists;

	void CreateALabel ( LPRECT rect, LPCTSTR prompt );

	CWnd *m_pFirstControl;
	CWnd *m_pFocusControl;

	BOOL m_AddFilesControl;

public:
	int m_SpecType;
	BOOL m_AllowSubmit;
	int m_OldHeight;
	int m_NewHeight;
	int m_OldAveCharWidth;
	int m_NewAveCharWidth;
protected:
	CString m_Tag;
	SpecElem m_SpecDef;

	//		m_OldForm is the spec from cmd_editspec's m_SpecIn
	//		that the user wants to change. m_NewForm is what we 
	//		input after the user has edited it.
	//
	CString m_OldForm;
	CharString m_OldFormA;
	CString m_NewForm;

	// Force all jobs to be checked by default.  Handy for use with 
	// a numbered change which has already had the job list edited once
	// by the user
	BOOL m_CheckAllJobs;

public:
	// Don't check files that are unchanged so that they don't
	// automatically get submitted
	BOOL m_CheckOnlyChgedFiles;

protected:
	// If called from a selection of files,
	// don't check files that are not selected
	BOOL m_SubmitOnlySelected;

	// If this is TRUE, call On_OK() at end of InitDialog()
	BOOL m_AutomaticallyUpdate;

	// Support for user-defined font, as read from the registry
	void CreateUserFonts();
	int  SetUserFont();
public:
	CFont m_Font;
protected:
	CFont m_FontBold;
	CFont m_FontFixed;

	// Track any possible change to user password
	CString m_OrigPassword;
	BOOL m_ReadOrigPassword;
	BOOL m_SetPermPassword;
	BOOL m_SetTempPassword;
	CString m_NewPassword;

	// Track any possible change to client root
	CString m_OrigRoot;
	BOOL m_ReadOrigRoot;

	// If set, substitute this root for the one in the spec before displaying spec
	CString m_Root2Use;

	// Track any possible change to client view
	CString m_OrigView;
	BOOL m_ReadOrigView;
	BOOL m_SyncAfter;

	// Flags for disabling Editor button
	BOOL m_WindowShown;
public:
	BOOL m_EditorBtnDisabled;
	BOOL m_ChangesHaveBeenMade;
protected:

	// Useful general purpose stringlist
	CStringList m_StringList;

	// Last View or Review had a blank line in it
	BOOL m_SetFocusHere;

	// Pointer to Pending Changelist class
	CDeltaView *m_pDeltaView;

	// Only 1 Browse button allowed per spec;
	BOOL m_BrowseShown;
	int m_BrowseFldCtrlID;	// id of the field that is browsable
	int m_BrowseBtnCtrlID;	// id of the Browse button itself
	CString m_BrowseTag;

	// Pointer to the last files widget created
	// This will be checked/uncheck when the
	// Revert Unchanged Files checkbox is checked/unchecked
	CReviewList *m_pLastFilesList;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CP4SpecDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL


// Implementation
protected:
	void AddDummy();
	void AddInput( const CString &prompt, const CString &editText, 
					int specCode, BOOL readOnly, BOOL multiLine, 
					BOOL promptAbove, int height, int width, 
					BOOL required = FALSE,
					const CString &indent=_T("B"), const CString &wCode=_T("A"), 
					int lioff=1, BOOL showBrowse=FALSE, BOOL allowDD=FALSE);
	void AddList( const CString &prompt, const CStringArray &list,
				    int specCode, int height, int width, int scrollWidth);
	BOOL IsFileChanged(CString filename);
	void AddEditBox( int i );
	void AddHiddenEditBox( int i );
	void AddComboBox( const CString &prompt, const CStringArray &values
					 ,const CString &editText
					 ,int specCode, BOOL readOnly, int height, int width, int required
					 ,const CString &indent, const CString &wCode, int lioff);

	void AddCheckBox( const CString &prompt, const CStringArray &values
					 , const CString &editText 
					 , int specCode, BOOL readOnly, int height, int width, int required
					 , const CString &indent, const CString &wCode, int lioff
					 , CRect &rect, SpecControl &sc);

	void AddView( const CString &tag, const CStringArray &sa, BOOL allowDD=FALSE );
	void AddView( const CString &tag, const CObArray &oa, BOOL allowDD=FALSE );

	CStringArray m_asSpecElems;
	BOOL ParseSpecIntoForm( );
	void GetComboBoxValues( CString &value, CStringArray &aPresets );

	void EnableControls();
	void DisableControls();

	int WordCount( const CString &cst, int type );
	BOOL ShowUserError( const CString &msg, CWnd *pControl );
	///////////BOOL ShowUserError( const CString &msg, int i );


protected:
	//		to read m_OldForm and place dialog controls per the spec
	//
	virtual BOOL SetControls();

	//		to read controls, and write a new spec to m_NewForm
	//
	virtual BOOL UpdateSpec( );
	void RemovePasswordFromSpec();
	BOOL CheckNumWords ( const CString tag, const CString &cst
					, int type , int words, int required, CString &msg );

	//	For 2003.1 or later servers, reoder the spec fields into the requested order
	void ReorderSpecDefString( CString &form, CString &specDefStr );

	//	to hack a Line Elements with Values into Selects
	int FindLineElemWithValues( CString specDefStr, int offset = 0 );
	void HandleLineElemWithValues( CString form, CString specDefStr );
	BOOL RestoreLineElemWithValues( CString form, CString specDefStr );

private:
	int     GetLengthLongestPrompt();
	void 	SetButtons( int adjamt );
	CString GetDialogueCaption ( );
	BOOL SendSpec(LPCTSTR specText, BOOL submit/*=FALSE*/, BOOL reopen=FALSE, int unchangedFlag=0);
	BOOL m_Submitting;
	
public:
	LPCTSTR GetSpec();
	BOOL EditedLists() const { return m_EditedLists; }
	void SetCallingCommand( CCmd_EditSpec *pCmd );
	CCmd_EditSpec *GetCallingCommand() { return m_pCallingCommand; }
	void SetCallingWnd(HWND hwnd) { m_CallingWnd = hwnd; }
	HWND GetCallingWnd() { return m_CallingWnd; }
	BOOL SetSpec(LPCTSTR spec, LPCTSTR specDefStr, int specType, BOOL allowSubmit=FALSE);
	void SetChangeParms(BOOL checkAllJobs, BOOL allowSubmit, BOOL checkOnlyChgedFiles=FALSE, BOOL addFilesControl=TRUE, BOOL submitOnlySelected=FALSE, BOOL automaticallyUpdate=FALSE);
	void SetClientParms(LPCTSTR root, BOOL automaticallyUpdate=FALSE);
	LPCTSTR GetChangeDesc() { return LPCTSTR(m_NewChangeDesc); }
	void GetCP4Wrapper(CObject *wrapper);
	BOOL IsSyncAfter() { return m_SyncAfter; }
	void SetIsModal(BOOL b) { m_bIsModal = b; }
	int DoCleanup();
	void ResetFileChecks(BOOL bCheck);

	inline CP4winApp *TheApp() { return (CP4winApp *) AfxGetApp(); }
    
	// Generated message map functions
	//{{AFX_MSG(CP4SpecDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnEnter();
	afx_msg void On_OK();
	afx_msg void OnAlternate();
	virtual void On_Cancel();
	virtual void OnCancel();   
	afx_msg void OnHelpnotes();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnEditor();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnCancelButton();
	afx_msg void OnBrowse();
	//}}AFX_MSG
    BOOL OnToolTipNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult );
	LRESULT OnP4SendSpec(WPARAM wParam, LPARAM lParam);
	LRESULT OnP4EndHelpnotes(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

#endif //__SPECDLG__

