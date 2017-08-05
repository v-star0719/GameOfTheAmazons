#include "stdafx.h"
#include "GameServerApp.h"
#include "NetMsg.h"
#include "GameServerDlg.h"
#include "MyAsyncSocket.h"
#include "GameServer.h"
#include "GlobalFunctions.h"

//在这里启动一个服务器线程
//数据结构：
//    

//采用非阻塞模式，因为每隔一定时间向客户端发送信息检查是否存活，超时没有或者收到了客户端退出信号就删除，


//login,logout两个消息。

CGameServer::CGameServer()
{
	m_pSocketServer = NULL;
	clientArray.SetSize(0);
}

CGameServer::~CGameServer()
{
}

void CGameServer::Init(CGameServerDlg* pDlg)
{
	m_pDlg = pDlg;
}

void CGameServer::Start()
{
	if (m_pSocketServer != NULL)
		return;

	m_nPort = m_pDlg->m_nPort;
	m_strIp = GblGetIPString(GblRevertByByte(m_pDlg->m_dwIp));
	m_nRecvBufferLen = 0;
	m_nCounter = 0;

	m_pSocketServer = new CMyAsyncSocket();
	m_pSocketServer->Init(this);
	if (!m_pSocketServer->Create(m_nPort, SOCK_STREAM, 63L, m_strIp))
	{
		TipSocketError(_T("服务器socket创建失败"));
		return;
	}

	if (!m_pSocketServer->Listen())
	{
		TipSocketError(_T("服务器socket监听失败"));
		return;
	}
	Log(_T("启动成功\r\n"));
}

void CGameServer::Stop()
{
	if (m_pSocketServer == NULL)
		return;

	for (int i = 0; i < clientArray.GetCount(); i++){
		clientArray[i].CloseSocket();
	}
	clientArray.RemoveAll();

	delete m_pSocketServer;
	m_pSocketServer = NULL;

	Log(_T("已停止\r\n"));
}

void CGameServer::TipSocketError(CString msg)
{
	CString text;
	text.Format(_T("%s, error = %d\r\n"), msg, WSAGetLastError());
	//AfxMessageBox(text);
	Log(text);
}

void CGameServer::OnReceive(CMyAsyncSocket *pS)
{
	RecvMsg(pS);
}

void CGameServer::OnSend(CMyAsyncSocket *pS)
{

}

void CGameServer::OnAccept(CMyAsyncSocket *pS)
{
	AcceptClient();
}

void CGameServer::OnConnect(CMyAsyncSocket *pS)
{

}

void CGameServer::OnClose(CMyAsyncSocket *pS)
{
	RemoveClient(pS);
}

//////////////////////////////////////////////////////////////////////////
void CGameServer::SendMsg(CMyAsyncSocket *pS, SNetMsgBase *pMsg)
{
	int msgLen = pMsg->Write(m_sendBuffer);
	ASSERT(msgLen <= sizeof(m_sendBuffer));
	pS->Send(m_sendBuffer, msgLen, 0);

	m_strTempLog.Format(_T("receive msg id = %d %s\r\n"), pMsg->msgId, SNetMsgName::GetMsgName(pMsg->msgId));
	Log(m_strTempLog);
}

void CGameServer::RecvMsg(CMyAsyncSocket *pS)
{
	//同为Windows平台，不考虑大小头优先的问题
	m_nRecvBufferLen = pS->Receive(m_recvBuffer, sizeof(m_recvBuffer));
	ReadMsgFromBufferAndDispatch(pS);
}

void CGameServer::AcceptClient()
{
	CAsyncSocket s;
	SOCKADDR addr;
	int addrLen = sizeof(SOCKADDR);
	if (m_pSocketServer->Accept(s, &addr, &addrLen))
	{
		SClientInfo client = { 0 };
		memcpy(&client.m_playerInfo.addr, &addr, sizeof(addr));
		client.m_playerInfo.nId = -1;
		client.m_nOpponentId = -1;
		client.m_pSocket = new CMyAsyncSocket();
		client.m_pSocket->Init(this);
		client.m_pSocket->Attach(s.Detach());
		clientArray.Add(client);
		m_pDlg->UpdateClientList();

		Log(_T("与客户端建立连接\r\n"));
		return;
	}
	else
	{
		TipSocketError(_T("连接客户端失败"));
	}
	return;
}

void CGameServer::RemoveClient(CMyAsyncSocket *pS)
{
	for (int i = 0; i < clientArray.GetCount(); i++)
	{
		if (clientArray[i].m_pSocket == pS)
		{
			m_strLog.Format(_T("remove client\r\n"), clientArray[i].m_playerInfo.nId);
			Log(m_strLog);
			clientArray.RemoveAt(i);
			break;
		}
	}
	m_pDlg->UpdateClientList();
	PushClientListToAllClients(0);
}

//excepPlayerId：主要是为了刚连上的这个玩家，不发送列表
void CGameServer::PushClientListToAllClients(int excepPlayerId)
{
	for (int i = 0; i < clientArray.GetCount(); i++)
	{
		SClientInfo &info = clientArray[i];
		if (info.m_playerInfo.nId > 0 && info.m_playerInfo.nId != excepPlayerId)
		{
			SendNetMsgScPlayerList(info.m_pSocket, info.m_playerInfo.nId);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//send server to client msg
void CGameServer::SendNetMsgScPlayerId(CMyAsyncSocket *pS, int id)
{
	SNetMsgScId msg;
	msg.playerId = id;
	SendMsg(pS, &msg);
}

void CGameServer::SendNetMsgScPlayerList(CMyAsyncSocket *pS, int id)
{
	SNetMsgScPlayerList msg;
	msg.playerCount = clientArray.GetCount();
	msg.pArray = new SNetPlayerInfo[msg.playerCount];
	for (int i = 0; i < msg.playerCount; i++)
	{
		msg.pArray[i].nId = clientArray[i].m_playerInfo.nId;
		msg.pArray[i].playerCamp = clientArray[i].m_playerInfo.playerCamp;
		memcpy(msg.pArray[i].szName, clientArray[i].m_playerInfo.szName, sizeof(clientArray[i].m_playerInfo.szName));
	}
	SendMsg(pS, &msg);
}

void CGameServer::SendNetMsgScConnect(CMyAsyncSocket *pS, int inviterId, int inviteeId)
{
	SNetMsgScChallenge msg;
	msg.inviterId = inviterId;
	msg.inviteeId = inviteeId;
	SendMsg(pS, &msg);
}

void CGameServer::SendNetMsgScConnectConfirm(CMyAsyncSocket *pS, int inviterId, int inviteeId, int answer)
{
	SNetMsgScChallengeConfirm msg;
	msg.inviterId = inviterId;
	msg.inviteeId = inviteeId;
	msg.answer = answer;
	SendMsg(pS, &msg);
}

void CGameServer::SendNetMsgScCancelChallenge(CMyAsyncSocket *pS)
{
	SNetMsgScCancelChallenge msg;
	SendMsg(pS, &msg);
}

void CGameServer::SendNetMsgScMove(CMyAsyncSocket *pS, int ijArray[6])
{
	SNetMsgScMove msg;
	memcpy(msg.ijArray, ijArray, sizeof(msg.ijArray));
	SendMsg(pS, &msg);
}

void CGameServer::SendNetMsgScGameControl(CMyAsyncSocket *pS, int action)
{
	SNetMsgScGameControl msg;
	msg.action = action;
	SendMsg(pS, &msg);
}

void CGameServer::SendNetMsgScError(CMyAsyncSocket *pS, CString strError)
{
	SNetMsgScError msg;
	memset(msg.szError, 0, sizeof(msg.szError));
	memcpy(msg.szError, strError.GetBuffer(), (strError.GetLength() + 1) * sizeof(TCHAR));
	SendMsg(pS, &msg);
}

//////////////////////////////////////////////////////////////////////////
//handle client to server msg

void CGameServer::ReadMsgFromBufferAndDispatch(CMyAsyncSocket *pS)
{
	int readMsgLen = 0;
	int msgId = 0;
	memcpy(&msgId, m_recvBuffer, sizeof(msgId));
	m_strTempLog.Format(_T("receive msg id = %d %s\r\n"), msgId, SNetMsgName::GetMsgName(msgId));
	Log(m_strTempLog);

	switch (msgId)
	{
	case NETMSG_CS_REQUIRE_ID:
	{
		SNetMsgCsRequireId msg;
		readMsgLen = msg.Read(m_recvBuffer);
		HandleNetMsgCsPlayerId(pS, msg);
		break;
	}
	case NETMSG_CS_PLAYER_INFO:
	{
		SNetMsgCsPlayerInfo msg;
		readMsgLen = msg.Read(m_recvBuffer);
		HandleNetMsgCsPlayerInfo(msg);
		break;
	}
	case NETMSG_CS_REQUIRE_PLAYER_LIST:
	{
		SNetMsgCsRequirePlayerList msg;
		readMsgLen = msg.Read(m_recvBuffer);
		HandleNetMsgCsRequirePlayerList(msg);
		break;
	}
	case NETMSG_CS_CHALLENGE:
	{
		SNetMsgCsChallenge msg;
		readMsgLen = msg.Read(m_recvBuffer);
		HandleNetMsgCsChallenge(msg);
		break;
	}
	case NETMSG_CS_CHALLENGE_CONFIRM:
	{
		SNetMsgCsChallengeConfirm msg;
		readMsgLen = msg.Read(m_recvBuffer);
		HandleNetMsgCsChallengeConfirm(msg);
		break;
	}
	case NETMSG_CS_CANCEL_CHALLENGE:
	{
		SNetMsgCsCancelChallenge msg;
		readMsgLen = msg.Read(m_recvBuffer);
		HandleNetMsgCsCancelChallenge(msg);
		break;
	}
	case NETMSG_CS_MOVE:
	{
		SNetMsgCsMove msg;
		readMsgLen = msg.Read(m_recvBuffer);
		HandleNetMsgCsMove(msg);
		break;
	}
	case NETMSG_CS_GAME_CONTROL:
	{
		SNetMsgCsGameControl msg;
		readMsgLen = msg.Read(m_recvBuffer);
		HandleNetMsgCsGameControl(msg);
		break;
	}
	default:
		break;
	}
	ASSERT(readMsgLen <= m_nRecvBufferLen);

	m_nRecvBufferLen -= readMsgLen;
	if (m_nRecvBufferLen > 0)
	{
		memmove(m_recvBuffer, m_recvBuffer + readMsgLen, m_nRecvBufferLen);
		ReadMsgFromBufferAndDispatch(pS);
	}
}

void CGameServer::HandleNetMsgCsPlayerId(CMyAsyncSocket *pS, SNetMsgCsRequireId &msg)
{
	int id = -1;
	for (int i = 0; i < clientArray.GetCount(); i++)
	{
		SClientInfo &info = clientArray[i];
		if (info.m_pSocket == pS)
		{
			ASSERT(info.m_playerInfo.nId <= 0);
			info.m_playerInfo.nId = ++m_nCounter;//算出id
			id = info.m_playerInfo.nId;
			break;
		}
	}

	if (id > 0)
		SendNetMsgScPlayerId(pS, id);
	else
		Log(_T("HandelNetMsgCsPlayerId: error client is not in record\r\n"));
}

void CGameServer::HandleNetMsgCsPlayerInfo(SNetMsgCsPlayerInfo &msg)
{
	SClientInfo *pInfo = GetClientInfoById(msg.playerId);
	if (pInfo == NULL)
	{
		m_strTempLog.Format(_T("HandelNetMsgCsPlayerInfo: player id = %d is not existed\r\n", msg.playerId));
		Log(m_strTempLog);
		return;
	}

	pInfo->m_playerInfo.playerCamp = msg.playerCamp;
	memcpy(pInfo->m_playerInfo.szName, msg.name, sizeof(msg.name));
	m_pDlg->UpdateClientList();
	PushClientListToAllClients(msg.playerId);
}

void CGameServer::HandleNetMsgCsRequirePlayerList(SNetMsgCsRequirePlayerList &msg)
{
	SClientInfo *pInfo = GetClientInfoById(msg.playerId);
	if (pInfo == NULL) return;

	SendNetMsgScPlayerList(pInfo->m_pSocket, msg.playerId);

	m_strTempLog.Format(_T("send player list to %d\r\n"), msg.playerId);
	Log(m_strTempLog);
}

//将邀请发给被邀请方
void CGameServer::HandleNetMsgCsChallenge(SNetMsgCsChallenge &msg)
{
	SClientInfo *pInviterInfo = GetClientInfoById(msg.inviterId);
	SClientInfo *pInviteeInfo = GetClientInfoById(msg.inviteeId);

	if (pInviterInfo == NULL || pInviterInfo == NULL) return;

	if (pInviterInfo->m_playerInfo.playerCamp == pInviteeInfo->m_playerInfo.playerCamp)
	{
		SendNetMsgScError(pInviterInfo->m_pSocket, CString(_T("错误：双方同为玩家1或玩家2")));
		return;
	}

	ASSERT(pInviteeInfo->m_nOpponentId <= 0);
	SendNetMsgScConnect(pInviteeInfo->m_pSocket, msg.inviterId, msg.inviteeId);

	m_strTempLog.Format(_T("translate challenge from %d to %d\r\n", msg.inviterId, msg.inviteeId));
	Log(m_strTempLog);
}

//将确认结果返回给邀请方
void CGameServer::HandleNetMsgCsChallengeConfirm(SNetMsgCsChallengeConfirm &msg)
{
	SClientInfo *pInviterInfo = GetClientInfoById(msg.inviterId);
	if (pInviterInfo == NULL) return;

	if (pInviterInfo->m_nOpponentId > 0)
	{
		m_strTempLog.Format(_T("HandelNetMsgCsChallengeConfirm: %d is play with othersd\r\n", msg.inviterId));
		Log(m_strTempLog);
		return;
	}

	if (msg.answer == NET_CHALLENGE_CONFIRM_YES)
	{
		pInviterInfo->m_nOpponentId = msg.inviteeId;
		SClientInfo *pInviteeInfo = GetClientInfoById(msg.inviteeId);
		pInviteeInfo->m_nOpponentId = msg.inviterId;
	}

	SendNetMsgScConnectConfirm(pInviterInfo->m_pSocket, msg.inviterId, msg.inviteeId, msg.answer);
}

void CGameServer::HandleNetMsgCsCancelChallenge(SNetMsgCsCancelChallenge &msg)
{
	SClientInfo *pInfo = GetClientInfoById(msg.playerId);
	if (pInfo == NULL)
	{
		TRACE("HandelNetMsgCsChallengeConfirm: player id = %d is not existed", msg.playerId);
		return;
	}

	if (pInfo->m_nOpponentId < 0)
	{
		TRACE("HandelNetMsgCsChallengeConfirm: player id = %d has no opponent", msg.playerId);
		return;
	}

	int player1 = pInfo->m_playerInfo.nId;
	int player2 = pInfo->m_nOpponentId;

	SClientInfo * pOpponentInfo = GetClientInfoById(pInfo->m_nOpponentId);
	pInfo->m_nOpponentId = -1;
	pOpponentInfo->m_nOpponentId = -1;

	SendNetMsgScCancelChallenge(pOpponentInfo->m_pSocket);
}

void CGameServer::HandleNetMsgCsMove(SNetMsgCsMove &msg)
{
	SClientInfo *pInfo = GetClientInfoById(msg.playerId);
	if (pInfo == NULL)
	{
		TRACE("HandleNetMsgCsMove: player id = %d is not existed", msg.playerId);
		return;
	}

	if (pInfo->m_nOpponentId < 0)
	{
		TRACE("HandleNetMsgCsMove: player id = %d has no opponent", msg.playerId);
		return;
	}

	SClientInfo * pOpponentInfo = GetClientInfoById(pInfo->m_nOpponentId);
	SendNetMsgScMove(pOpponentInfo->m_pSocket, msg.ijArray);
}

void CGameServer::HandleNetMsgCsGameControl(SNetMsgCsGameControl &msg)
{
	SClientInfo *pInfo = GetClientInfoById(msg.playerId);
	if (pInfo == NULL) return;

	if (pInfo->m_nOpponentId < 0)
	{
		TRACE("HandleNetMsgCsMove: player id = %d has no opponent", msg.playerId);
		return;
	}

	SClientInfo * pOpponentInfo = GetClientInfoById(pInfo->m_nOpponentId);
	SendNetMsgScGameControl(pOpponentInfo->m_pSocket, msg.action);
}

SClientInfo * CGameServer::GetClientInfoById(int playerId)
{
	for (int i = 0; i < clientArray.GetCount(); i++)
	{
		if (clientArray[i].m_playerInfo.nId == playerId)
		{
			return &clientArray[i];
		}
	}

	m_strTempLog.Format(_T("player id = %d is not existed\r\n", msg.playerId));
	Log(m_strTempLog);

	return NULL;
}

void CGameServer::Log(CString str)
{
	CString timer = CTime::GetCurrentTime().Format(_T("%m-%d %H:%M:%S    "));
	m_strLog += timer + str;
	m_pDlg->UpdateLog();
}