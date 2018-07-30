#pragma once
#include "WinPos.h"


// COldChgFilterDlg dialog

class COldChgFilterDlg : public CDialog
{
	DECLARE_DYNAMIC(COldChgFilterDlg)

	int m_dy;
public:
	COldChgFilterDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COldChgFilterDlg();

	void GetFilterRevRange(CString &filter) { filter = m_filter; }

// Dialog Data
	enum { IDD = IDD_OLDCHG_FILTER };
	BOOL m_useClient;
	CString m_client;

	BOOL m_useUser;
	CString m_user;

	BOOL m_includeIntegrations;

	CComboBox m_fileCombo;
	CString m_filespec;
	CString m_selectedFiles;
	CStringList m_selected;

	int m_filterFiles;
	CString m_filter;
	BOOL m_UseClientSyntax;
	BOOL m_bPending;

protected:
	CWinPos m_WinPos;

	int m_from;
	BOOL m_fromIsHaveExclusive;
	CString m_fromRev;
	CString m_fromSymbol;
	int m_fromTypeComboIX;
	int m_to;
	BOOL m_toIsHaveInclusive;
	CString m_toRev;
	CString m_toSymbol;
	int m_toTypeComboIX;

	// Resize support
	CRect m_InitRect;
	CRect m_LastRect;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	void LoadFilterComboBox();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnClient();
	afx_msg void OnBrowseClients();
	afx_msg void OnUser();
	afx_msg void OnBrowseUsers();
	afx_msg void OnFile();
	afx_msg void OnSetRevRange();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnOK();
	virtual void OnCancel();
	LRESULT OnBrowseClientsCallBack(WPARAM wParam, LPARAM lParam);
	LRESULT OnBrowseUsersCallBack(WPARAM wParam, LPARAM lParam);
};
