//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//


// LabelsPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLabelsPage dialog

class CLabelsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CLabelsPage)

// Construction
public:
	CLabelsPage();
	~CLabelsPage();

// Dialog Data
	//{{AFX_DATA(CLabelsPage)
	enum { IDD = IDD_PAGE_LABELS };
	CButton	m_LabelFilesInDialog;
	CButton	m_LabelShowPreviewDetail;
	int		m_LabelDragDropOption;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLabelsPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_Inited;
	int  m_CvtLabelDragDropOption[8];

	// Generated message map functions
	//{{AFX_MSG(CLabelsPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
