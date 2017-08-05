#include "stdafx.h"
#include "GameServer.h"
#include "MyAsyncSocket.h"

void CMyAsyncSocket::Init(CGameServer *pGameServer)
{
	m_pGameServer = pGameServer;
}

void CMyAsyncSocket::OnConnect(int nErrorCode)
{
	m_pGameServer->OnConnect(this);
}

void CMyAsyncSocket::OnAccept(int nErrorCode)
{
	m_pGameServer->OnAccept(this);
}

void CMyAsyncSocket::OnReceive(int nErrorCode)
{
	m_pGameServer->OnReceive(this);
}

void CMyAsyncSocket::OnSend(int nErrorCode)
{
	m_pGameServer->OnSend(this);
}

void CMyAsyncSocket::OnClose(int nErrorCode)
{
	m_pGameServer->OnClose(this);
}
