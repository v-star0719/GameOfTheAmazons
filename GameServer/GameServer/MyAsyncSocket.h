
#pragma once

class CGameServer;
class CMyAsyncSocket : public CAsyncSocket
{
public:


public:
	void Init(CGameServer *pGameServer);

public:
	 virtual void OnReceive(int nErrorCode);
	 virtual void OnSend(int nErrorCode);
	 virtual void OnAccept(int nErrorCode);
	 virtual void OnConnect(int nErrorCode);
	 virtual void OnClose(int nErrorCode);

private:
	int m_nID;
	CGameServer *m_pGameServer;
};
