// TipDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AmazonsApp.h"
#include "TipDlg.h"
#include "afxdialogex.h"


// CTipDlg dialog

IMPLEMENT_DYNAMIC(CTipDlg, CDialogEx)

CTipDlg::CTipDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTipDlg::IDD, pParent)
	, m_strMsg(_T(""))
{

}

CTipDlg::~CTipDlg()
{
}

void CTipDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_MSG, m_strMsg);
}


BEGIN_MESSAGE_MAP(CTipDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_OK2, &CTipDlg::OnBnClickedOk2)
	ON_BN_CLICKED(IDC_BTN_OK1, &CTipDlg::OnBnClickedBtnOk1)
	ON_BN_CLICKED(IDC_BTN_CANCEL1, &CTipDlg::OnBnClickedBtnCancel1)
END_MESSAGE_MAP()


// CTipDlg message handlers


BOOL CTipDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	GetDlgItem(IDC_BTN_OK1)->ShowWindow(m_nType == MB_OKCANCEL);
	GetDlgItem(IDC_BTN_CANCEL1)->ShowWindow(m_nType == MB_OKCANCEL);
	GetDlgItem(IDC_BTN_OK2)->ShowWindow(m_nType == MB_OK);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTipDlg::OnBnClickedOk2()
{
	EndDialog(IDOK);
}


void CTipDlg::OnBnClickedBtnOk1()
{
	EndDialog(IDOK);
}


void CTipDlg::OnBnClickedBtnCancel1()
{
	EndDialog(IDCANCEL);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

CTipDlg g_dlg;

//dlgType就是MessageBox的type
//返回值是IDOK或IDCANCEL
int TipMessage(CString text, int type /*= MB_OK*/)
{
	g_dlg.m_strMsg = text;
	g_dlg.m_nType = type;
	return g_dlg.DoModal();
}

