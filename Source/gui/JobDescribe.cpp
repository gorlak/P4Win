// JobDescribe.cpp : implementation file
//

#include "stdafx.h"
#include "p4win.h"
#include "JobDescribe.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJobDescribe dialog


CJobDescribe::CJobDescribe(CWnd* pParent /*=NULL*/)
	: CDialog(CJobDescribe::IDD, pParent)
{
	//{{AFX_DATA_INIT(CJobDescribe)
	m_JobStr = _T("");
	//}}AFX_DATA_INIT
}


void CJobDescribe::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJobDescribe)
	DDX_Text(pDX, IDC_JOB_NBR, m_JobStr);
	DDX_Check(pDX, IDC_AUTOCONVERTNBR, m_IsConvertNbr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CJobDescribe, CDialog)
	//{{AFX_MSG_MAP(CJobDescribe)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJobDescribe message handlers

BOOL CJobDescribe::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_IsConvertNbr = GET_P4REGPTR()->GetConvertJobNbr( );
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CJobDescribe::OnOK() 
{
	UpdateData();
	if (m_IsConvertNbr)
	{
		BOOL b = TRUE;
		m_JobStr = GetJobStr();
		for (int i=-1; ++i < m_JobStr.GetLength(); )
		{
			if (!_istdigit(m_JobStr.GetAt(i)))
			{
				b = FALSE;
				break;
			}
		}
		if (b)
			m_JobStr.FormatMessage(IDS_JOBNAME_FORMAT_n, _tstoi(m_JobStr));
		UpdateData(FALSE);
	}
	if (m_IsConvertNbr != GET_P4REGPTR()->GetConvertJobNbr( ))
		GET_P4REGPTR()->SetConvertJobNbr(m_IsConvertNbr);

	CDialog::OnOK();
}

CString CJobDescribe::GetJobStr()
{
	m_JobStr.TrimRight( );
	m_JobStr.TrimLeft( );
	if (m_IsConvertNbr)
		m_JobStr.TrimLeft(_T('0'));
	return m_JobStr;
}
