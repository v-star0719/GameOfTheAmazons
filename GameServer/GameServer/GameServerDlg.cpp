
// GameServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GameServerApp.h"
#include "GameServerDlg.h"
#include "afxdialogex.h"
#include "NetMsg.h"
#include "GameServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGameServer g_gameServer;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CGameServerDlg dialog



CGameServerDlg::CGameServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGameServerDlg::IDD, pParent)
	, m_dwIp(0)
	, m_nPort(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGameServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_IPAddress(pDX, IDC_IPADDRESS1, m_dwIp);
	DDX_Text(pDX, IDC_EDIT_PORT, m_nPort);
}

BEGIN_MESSAGE_MAP(CGameServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_START, &CGameServerDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_STOP, &CGameServerDlg::OnBnClickedBtnStop)
END_MESSAGE_MAP()


// CGameServerDlg message handlers

BOOL CGameServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	g_gameServer.Init(this);
	UpdateClientList();
	UpdateConnectStatus();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGameServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGameServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGameServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGameServerDlg::OnBnClickedBtnStart()
{
	g_gameServer.Start();
	UpdateConnectStatus();
}


void CGameServerDlg::OnBnClickedBtnStop()
{
	g_gameServer.Stop();
	UpdateConnectStatus();
}

void CGameServerDlg::OnClose()
{
	UpdateData(TRUE);
	g_gameServer.Stop();
	CDialogEx::OnClose();
}

void CGameServerDlg::UpdateClientList()
{
	SClientInfo *pClient1;

	CString text = _T("id | name | camp | address | opponentId\r\n");
	for (int i = 0; i < g_gameServer.clientArray.GetCount(); i++)
	{
		pClient1 = &g_gameServer.clientArray[i];
		CString str;
		str.Format(_T("[%d] %d <%s> %d.%d.%d.%d:%d %d\r\n"),
			pClient1->m_playerInfo.nId,
			pClient1->m_playerInfo.playerCamp,
			pClient1->m_playerInfo.szName,
			pClient1->m_playerInfo.addr.sin_addr.S_un.S_un_b.s_b1,
			pClient1->m_playerInfo.addr.sin_addr.S_un.S_un_b.s_b2,
			pClient1->m_playerInfo.addr.sin_addr.S_un.S_un_b.s_b3,
			pClient1->m_playerInfo.addr.sin_addr.S_un.S_un_b.s_b4,
			pClient1->m_playerInfo.addr.sin_port,
			pClient1->m_nOpponentId
		);
		text += str;
	}

	SetDlgItemText(IDC_EDIT_CLIENT_LIST, text);
}

void CGameServerDlg::UpdateConnectStatus()
{
	bool isStarted = g_gameServer.IsStarted();
	GetDlgItem(IDC_BTN_START)->EnableWindow(!isStarted);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(isStarted);
}

void CGameServerDlg::UpdateLog()
{
	GetDlgItem(IDC_EDIT_LOG)->SetWindowTextW(g_gameServer.m_strLog);
}


