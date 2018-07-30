#pragma once

#include "ClientWizBegin.h"
#include "ClientWizCreate.h"

// CClientWizSheet

class CClientWizSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CClientWizSheet)

public:
	CClientWizSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CClientWizSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CClientWizSheet();

	void SetMsg(CString& txt) { m_Message = txt; }
	void SetPort(CString& port) { m_Port = port; }
	void SetName(CString& name) { m_Name = name; }
	void AllowBrowse(BOOL b) { m_AllowBrowse = b; }
	CString GetName() { return m_ClientWizCreate.m_Name; }
	CString GetRoot() { return m_ClientWizCreate.m_Root; }
	BOOL IsEdit() { return m_ClientWizCreate.m_bEdit; }
	BOOL IsTmpl() { return m_ClientWizCreate.m_bTmplate; }
	CString GetTmpl() { return m_ClientWizCreate.m_Tmplate; }
	void DeleteAdobeBitmap() { m_ClientWizBegin.m_Bitmap.DeleteObject(); }

	CClientWizBegin m_ClientWizBegin;
	CClientWizCreate m_ClientWizCreate;

	CString m_Message;
	CString m_Port;
	CString m_Name;
	CString m_Root;
	BOOL m_AllowBrowse;

protected:
	//{{AFX_MSG(CClientWizSheet)
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


