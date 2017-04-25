// ChgDescribe.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "ChgDescribe.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChgDescribe dialog


CChgDescribe::CChgDescribe(CWnd* pParent /*=NULL*/)
	: CDialog(CChgDescribe::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChgDescribe)
	m_Nbr = _T("");
	//}}AFX_DATA_INIT
}


void CChgDescribe::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChgDescribe)
	DDX_Text(pDX, IDC_CHNAGELIST_NBR, m_Nbr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChgDescribe, CDialog)
	//{{AFX_MSG_MAP(CChgDescribe)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChgDescribe message handlers

void CChgDescribe::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

CString CChgDescribe::GetNbr()
{
	m_Nbr.TrimRight( );
	m_Nbr.TrimLeft( );
	m_Nbr.TrimLeft('0');
	return m_Nbr;
}
