#pragma once

#include "NetMsg.h"
#include "AmazonsGameController.h"

#define MAX_MSG_COUNT 10

class CAmazonsView;

//要求玩家打开网络对话框手动进行连接，所以一开始是没有创建连接和线程的
class CNetworkPlayer
{
public: 
	CNetworkPlayer();
	~CNetworkPlayer();

private:
	bool m_bHasInit;
	SOCKET m_socket;
	SPosition m_posSelected;//发送时，是要发送的；接受时，这个是接收到的
	SPosition m_posMoveTo;//发送时，是要发送的；接受时，这个是接收到的
	SPosition m_posShootAt;//发送时，是要发送的；接受时，这个是接收到的
	CWinThread *m_pThreadSend;
	CWinThread *m_pThreadRecv;
	bool m_bConnected;

	char m_recvBuffer[4096];
	char m_sendBuffer[4096];
	int m_nRecvBufferLen;
	int m_nSendBufferLen;//发送失败后，安装这个长度重新发送

	int m_nId;
	int m_nOpponentId;
	int m_nPort;
	long m_lIp;//addr格式的，socket直接用就行
	TCHAR m_szName[NET_MAX_PLAYER_NAME_LENGTH];
	CString m_strLogTemp;
	
	//发送线程用
	SNetMsgBase *m_msgArray[MAX_MSG_COUNT];//一个消息占据一个位置

public:
	CAmazonsView *m_pView;
	SNetMsgScPlayerList m_playerList;
	CString m_strLog;
	int m_nplayerCamp;

private:
	void Init();
	void ConnectToServer();
	void SendMsgToBuffer(SNetMsgBase *pMsg);
	void SendMsg();
	void RecvMsg();
	void ReadMsgFromBufferAndDispatch();
	void TipSocketError(CString msg);
	void Log(CString str);

public:
	void SetNetwork(CString strPlayerName, long ip, int nPort);
	void Start();
	void Disconnect();
	void Stop();
	void CancelChallenge();
	bool IsStarted();
	bool IsConnected();
	bool HasOpponent(){ return m_nOpponentId > 0; }
	int GetId(){ return m_nId; }
	int GetOpponentId(){ return m_nOpponentId; }
	void GetMove(SPosition &posSelected, SPosition &posMoveTo, SPosition &posShootAt);
	//
	void RequireId();
	void SendPlayerInfo();
	void RequirePlayerList();
	void SendChallengeMsg(int nInviteeId);
	void SendChallengeConfirmMsg(int nInviterId, int nInviteeId, int nAnswer);
	void SendCancelChallengeMsg();
	void SendMoveMsg(SPosition posSelected, SPosition posMoveTo, SPosition posShootAt);
	void SendGameControlMsg(int action);
	//
	void OnIdReceived(SNetMsgScId &msg);
	void OnPlayerListReceived(SNetMsgScPlayerList &msg);
	void OnChallenge(SNetMsgScChallenge &msg);
	void OnChallengConfirmRecevied(SNetMsgScChallengeConfirm &msg);
	void OnCancelChallengRecevied(SNetMsgScCancelChallenge &msg);
	void OnMoveReceved(SNetMsgScMove &msg);
	void OnGameControlReceved(SNetMsgScGameControl &msg);
	void OnErrorReceived(SNetMsgScError &msg);
	//
	SNetPlayerInfo * GetPlayerInfoById(int playerId);

public:
	static UINT WorkThread_Send(LPVOID pParam);
	static UINT WorkThread_Recv(LPVOID pParam);
};