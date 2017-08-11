// NetworkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AmazonsApp.h"
#include "NetworkDlg.h"
#include "afxdialogex.h"
#include "NetMsg.h"
#include "GlobalFunctions.h"

// CNetworkDlg dialog

IMPLEMENT_DYNAMIC(CNetworkDlg, CDialogEx)

CNetworkDlg::CNetworkDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CNetworkDlg::IDD, pParent)
	, m_strPlayerName(_T(""))
	, m_dwIp(0)
	, m_nPort(0)
	, m_strID(_T(""))
	, m_strChallengePlayer(_T(""))
	, m_strChoosedPlayer(_T(""))
	, m_strOpponentName(_T(""))
{
	m_nSeletdPlayerIndex = -1;
}

CNetworkDlg::~CNetworkDlg()
{
}

void CNetworkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_ID, m_strID);
	DDX_Text(pDX, IDC_EDIT_Name, m_strPlayerName);
	DDV_MaxChars(pDX, m_strPlayerName, 10);
	DDX_IPAddress(pDX, IDC_IPADDRESS, m_dwIp);
	DDX_Text(pDX, IDC_EDIT_PORT, m_nPort);
	DDX_Text(pDX, IDC_STATIC_ID, m_strID);
	DDX_Control(pDX, IDC_LIST_PLAYER_CAN_CHALLENGE, m_playerListCtrl);
	DDX_Text(pDX, IDC_EDIT_OPPONENT, m_strChallengePlayer);
	DDX_Text(pDX, IDC_EDIT_CHOOSED_PLAYER, m_strChoosedPlayer);
	DDX_Text(pDX, IDC_EDIT_OPPONENT, m_strOpponentName);
}


BEGIN_MESSAGE_MAP(CNetworkDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_CONNECT, &CNetworkDlg::OnBnClickedConnect)
	ON_LBN_SELCHANGE(IDC_LIST_PLAYER_CAN_CHALLENGE, &CNetworkDlg::OnLbnSelchange)
	ON_BN_CLICKED(IDC_BTN_CHALLENGE, &CNetworkDlg::OnBnClickedBtnChallenge)
	ON_BN_CLICKED(IDC_BTN_CANCEL_CHALLENGE, &CNetworkDlg::OnBnClickedBtnCancelChallenge)
	ON_BN_CLICKED(IDC_BTN_DISCONNECT, &CNetworkDlg::OnBnClickedBtnDisconnect)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CNetworkDlg message handlers

void CNetworkDlg::RequirePlayerList()
{
	if (m_pNetworkPlayer == NULL) return;
	//if (m_pNetworkPlayer->HasConnected)

	m_pNetworkPlayer->RequirePlayerList();
}

void CNetworkDlg::UpdatePlayerList()
{
	m_playerListCtrl.ResetContent();
	SNetPlayerInfo *pArray = m_pNetworkPlayer->m_playerList.pArray;
	CString temp;
	for (int i = 0; i < m_pNetworkPlayer->m_playerList.playerCount; i++)
	{
		temp.Format(_T("id = %d, player%d, name = %s"),
			pArray[i].nId,
			pArray[i].playerCamp,
			pArray[i].szName);
		m_playerListCtrl.AddString(temp);
	}
}

void CNetworkDlg::UpdateConnectStatus()
{
	int id = m_pNetworkPlayer->GetId();
	if (id <= 0)
		m_strID = _T("ID = 未连接");
	else
		m_strID.Format(_T("ID = %d"), id );
	bool isStarted = m_pNetworkPlayer->IsStarted();
	GetDlgItem(IDC_BTN_CONNECT)->EnableWindow(!isStarted);
	GetDlgItem(IDC_BTN_DISCONNECT)->EnableWindow(isStarted);
	UpdateData(FALSE);
}

void CNetworkDlg::UpdateLog()
{
	GetDlgItem(IDC_EDIT_LOG)->SetWindowText(m_pNetworkPlayer->m_strLog);
}

void CNetworkDlg::UpdateOpponentInfo()
{
	int opponentId = m_pNetworkPlayer->GetOpponentId();
	if (opponentId > 0)
	{
		SNetPlayerInfo *pInfo = m_pNetworkPlayer->GetPlayerInfoById(opponentId);
		if (pInfo != NULL)
			m_strOpponentName.Format(_T("%s [%d]"), pInfo->szName, opponentId);
		else
			m_strOpponentName.Format(_T("%s [%d]"), _T("unkonw"), opponentId);
	}
	else
	{
		m_strOpponentName = _T("");
	}
	GetDlgItem(IDC_BTN_CANCEL_CHALLENGE)->EnableWindow(opponentId > 0);
	GetDlgItem(IDC_BTN_CHALLENGE)->EnableWindow(opponentId < 0);
	UpdateData(FALSE);
}

BOOL CNetworkDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	UpdateConnectStatus();
	UpdateOpponentInfo();

	m_pNetworkPlayer->SetNetwork(m_strPlayerName, GblRevertByByte(m_dwIp), m_nPort);
	((CEdit*)GetDlgItem(IDC_EDIT_Name))->SetLimitText(NET_MAX_PLAYER_NAME_LENGTH);

	if (m_pNetworkPlayer->IsConnected())
		RequirePlayerList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CNetworkDlg::OnBnClickedConnect()
{
	UpdateData(TRUE);
	m_pNetworkPlayer->SetNetwork(m_strPlayerName, GblRevertByByte(m_dwIp), m_nPort);
	m_pNetworkPlayer->Start();
	UpdateConnectStatus();
}

void CNetworkDlg::OnBnClickedBtnDisconnect()
{
	if (AfxMessageBox(_T("确定要断开吗"), MB_OKCANCEL) == IDOK)
	{
		m_pNetworkPlayer->Disconnect();
		UpdateConnectStatus();
		AfxMessageBox(_T("已断开"));
	}
}

void CNetworkDlg::OnLbnSelchange()
{
	// TODO: Add your control notification handler code here
	m_nSeletdPlayerIndex = m_playerListCtrl.GetCurSel();
	m_strChoosedPlayer = m_pNetworkPlayer->m_playerList.pArray[m_nSeletdPlayerIndex].szName;
	UpdateData(FALSE);
}

void CNetworkDlg::OnBnClickedBtnChallenge()
{
	if (m_nSeletdPlayerIndex < 0)
	{
		AfxMessageBox(_T("请先选择一个玩家"));
		return;
	}

	int id = m_pNetworkPlayer->m_playerList.pArray[m_nSeletdPlayerIndex].nId;
	m_pNetworkPlayer->SendChallengeMsg(id);
}

void CNetworkDlg::OnBnClickedBtnCancelChallenge()
{
	m_pNetworkPlayer->CancelChallenge();
}

void CNetworkDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	UpdateData(TRUE);
	CDialogEx::OnClose();
}