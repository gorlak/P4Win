#ifndef P4ABOUTDIALOG_H_
#define P4ABOUTDIALOG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RichEdCtrlEx.h"

class CP4AboutDialog : public CDialog
{
public:
	CP4AboutDialog();
    CP4AboutDialog(CWnd* parent);

// Implementation
protected:
	COLORREF m_backgroundColor;
	CBrush m_backgroundBrush;
	CImageList m_logo;
	CPoint m_logoPos;

	virtual BOOL OnInitDialog();
	afx_msg void OnEditCopy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	void CP4AboutDialog::OnPaint();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#endif // P4ABOUTDIALOG_H_
