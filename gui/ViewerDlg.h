// ViewerDlg.h : header file
//

#ifndef __VIEWERDLG__
#define __VIEWERDLG__
/////////////////////////////////////////////////////////////////////////////
// CViewerDlg dialog

class CViewerDlg : public CDialog
{
// Construction
public:
	CViewerDlg(CWnd* pParent = NULL);   // standard constructor
	CString m_Viewer;
	CString GetViewer() { return m_Viewer; }
	int m_NumMRUViewers;

// Dialog Data
	//{{AFX_DATA(CViewerDlg)
	enum { IDD = IDD_VIEWERSELECT };
	CP4ListBox	m_MRUList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    void GetViewerFromControls();

	// Generated message map functions
	//{{AFX_MSG(CViewerDlg)
	afx_msg void OnRadio();
	afx_msg void OnFindviewer();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkMruList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // __VIEWERDLG__
