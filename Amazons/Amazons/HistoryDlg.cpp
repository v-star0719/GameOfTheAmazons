// HistoryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AmazonsApp.h"
#include "AmazonsGameController.h"
#include "HistoryDlg.h"
#include "AmazonsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHistoryDlg dialog


CHistoryDlg::CHistoryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHistoryDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHistoryDlg)
	m_strHistory = _T("");
	//}}AFX_DATA_INIT
	m_pView=NULL;
}
CHistoryDlg::CHistoryDlg(CView* pView)
{
	m_pView=pView;
}
int CHistoryDlg::Create()
{
	return CDialog::Create(CHistoryDlg::IDD);
}

void CHistoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHistoryDlg)
	DDX_Control(pDX, IDC_HISTORY, m_editHistory);
	DDX_Text(pDX, IDC_HISTORY, m_strHistory);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHistoryDlg, CDialog)
	//{{AFX_MSG_MAP(CHistoryDlg)
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
END_MESSAGE_MAP()

void CHistoryDlg::UpdateHistoryText(CAmazonsGameController *pAmazons)
{
	SHistoryRecord history[92];
	int nPtr = 0;
	int nCurPtr = 0;
	pAmazons->GetHistory(history, nPtr, nCurPtr);

	m_strHistory = "";
	CString strTemp;
	for (int i = 0; i < nPtr; i++)
	{
		CString prefix = i >= nCurPtr - 1 ? _T("-->") : _T("   ");
		if (pAmazons->IsPlayer1Turn(i))
			strTemp.Format(_T("%s%2d.%s\t"), prefix, i, pAmazons->m_bPlayer1Black ? _T("黑方") : _T("红方"));
		else
			strTemp.Format(_T("%s%2d.%s\t"), prefix, i, pAmazons->m_bPlayer1Black ? _T("红方") : _T("黑方"));

		m_strHistory += strTemp;
		strTemp.Format(_T("(%d,%d)\t(%d,%d)\t(%d,%d)\r\n"),
			history[i].posSelected.i, history[i].posSelected.j,
			history[i].posMoveTo.i, history[i].posMoveTo.j,
			history[i].posShootAt.i, history[i].posShootAt.j
			);
		m_strHistory += strTemp;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CHistoryDlg message handlers

void CHistoryDlg::OnClose()
{
	((CAmazonsView*)m_pView)->HistoryDlgClosed();
	CDialog::OnClose();
}
