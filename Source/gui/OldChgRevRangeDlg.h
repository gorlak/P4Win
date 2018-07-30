#pragma once
#include "WinPos.h"


// COldChgRevRangeDlg dialog

class COldChgRevRangeDlg : public CDialog
{
	DECLARE_DYNAMIC(COldChgRevRangeDlg)

	int m_dy;
public:
	COldChgRevRangeDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COldChgRevRangeDlg();

	void GetFilterRevRange(CString *filter);

// Dialog Data
	enum { IDD = IDD_OLDCHG_REVRANGE };

	int m_from;
	BOOL m_fromIsHaveExclusive;
	CString m_fromRev;
	CString m_fromSymbol;
	CComboBox m_fromTypeCombo;
	int m_fromTypeComboIX;
	int m_to;
	BOOL m_toIsHaveInclusive;
	CString m_toRev;
	CString m_toSymbol;
	CComboBox m_toTypeCombo;
	int m_toTypeComboIX;

protected:
	CWinPos m_WinPos;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnFrom();
	afx_msg void OnFromComboValueChg();
	afx_msg void OnFromBrowse();
	afx_msg void OnTo();
	afx_msg void OnToComboValueChg();
	afx_msg void OnToBrowse();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	virtual void OnOK();
	virtual void OnCancel();
	LRESULT OnFromBrowseCallBack(WPARAM wParam, LPARAM lParam);
	LRESULT OnToBrowseCallBack(WPARAM wParam, LPARAM lParam);
};
