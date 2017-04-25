#if !defined(AFX_FILETYPE_H__6155DB91_ADC8_11D3_A3A0_00105AC64526__INCLUDED_)
#define AFX_FILETYPE_H__6155DB91_ADC8_11D3_A3A0_00105AC64526__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileType.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileType dialog

class CFileType : public CDialog
{
// Construction
public:
	CFileType(CWnd* pParent = NULL);   // standard constructor

	CString m_itemStr;
	CString m_fileType;
	
// Dialog Data
	//{{AFX_DATA(CFileType)
	enum { IDD = IDD_FILETYPE };
	CButton	m_OK;
	CButton	m_Cancel;
	CButton	m_FileTypeHelp;
	int		m_Action;
	int		m_BaseType;
	int		m_StoreType;
	BOOL	m_TypeK;
	BOOL	m_TypeW;
	BOOL	m_TypeX;
	BOOL	m_TypeM;
	BOOL	m_TypeO;
	BOOL	m_TypeL;
	BOOL	m_TypeS;
	CComboBox m_NbrRevsCombo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileType)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

protected:

	int		m_SaveBaseType;
	int		m_SaveStoreType;
	BOOL	m_SaveTypeK;
	BOOL	m_SaveTypeW;
	BOOL	m_SaveTypeX;
	BOOL	m_SaveTypeM;
	BOOL	m_SaveTypeO;
	BOOL	m_SaveTypeL;
	BOOL	m_SaveTypeS;
	int		m_NbrRevs;
	int		m_SaveNbrRevs;

	// Generated message map functions
	//{{AFX_MSG(CFileType)
	afx_msg void OnFiletypehelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnFileTypeK();
	afx_msg void OnFileTypeO();
	afx_msg void OnFileTypeS();
	afx_msg void OnActionChange();
	afx_msg void OnActionAdd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void UpdateNbrRevs();

	CString fileType;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILETYPE_H__6155DB91_ADC8_11D3_A3A0_00105AC64526__INCLUDED_)
