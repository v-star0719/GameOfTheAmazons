#pragma once

//该文件在GameServer和Amazons各有一份，需保持一致
#define NET_MAX_PLAYER_NAME_LENGTH 20 //字符数，不是字节数哦
#define NET_CHALLENGE_CONFIRM_YES 1
#define NET_CHALLENGE_CONFIRM_NO 0
//
#define NET_GAME_CONTROLL_START 1
#define NET_GAME_CONTROLL_PAUSE 2
#define NET_GAME_CONTROLL_CONTINUE 3
#define NET_GAME_CONTROLL_STOP 4

enum EmNetMsgId
{
	NETMSG_CS_PLAYER_INFO,
	NETMSG_CS_MOVE,
	NETMSG_CS_REQUIRE_ID,
	NETMSG_CS_RECONNECT,
	NETMSG_CS_REQUIRE_PLAYER_LIST,
	NETMSG_CS_CHALLENGE,//对战邀请step1
	NETMSG_CS_CHALLENGE_CONFIRM, //对战邀请stepstep3
	NETMSG_CS_CANCEL_CHALLENGE,
	NETMSG_CS_GAME_CONTROL,
	//
	NETMSG_SC_MOVE,
	NETMSG_SC_ID,
	NETMSG_SC_PLAYER_LIST,
	NETMSG_SC_CHALLENGE, //对战邀请stepstep2
	NETMSG_SC_CHALLENGE_CONFIRM,//对战邀请stepstep4
	NETMSG_SC_CANCEL_CHALLENGE,
	NETMSG_SC_GAME_CONTROL,
	NETMSG_SC_ERROR,
};

//包装下避免出重复定义的错
struct SNetMsgName
{
	static TCHAR * GetMsgName(int msgId)
	{
		TCHAR *szNetMsgName[] = {
			_T("CS_PLAYER_INFO"),
			_T("CS_MOVE"),
			_T("CS_REQUIRE_ID"),
			_T("CS_RECONNECT"),
			_T("CS_REQUIRE_PLAYER_LIST"),
			_T("CS_CHALLENGE"),
			_T("CS_CHALLENGE_CONFIRM"),
			_T("CS_CANCEL_CHALLENGE"),
			_T("SC_GAME_CONTROL"),
			//
			_T("SC_MOVE"),
			_T("SC_ID"),
			_T("SC_PLAYER_LIST"),
			_T("SC_CHALLENGE"),
			_T("SC_CHALLENGE_CONFIRM"),
			_T("SC_CANCEL_CHALLENGE"),
			_T("SC_GAME_CONTROL"),
			_T(""),
		};

		if (msgId < 0 || msgId >= sizeof(szNetMsgName) / sizeof(TCHAR*))
			return _T("错误的消息号");
		return szNetMsgName[msgId];
	}
};

struct SNetPlayerInfo
{
	int nId;
	int playerCamp;
	TCHAR szName[NET_MAX_PLAYER_NAME_LENGTH];
	SOCKADDR_IN addr;
};

struct SNetMsgBase
{
	int msgId;
	virtual int Read(char *buffer){ return 0; }
	virtual int Write(char *buffer){ return 0; }

	//读完后buffer往后增加
	static void ReadBuffer(void *dest, char **pBuffer, int len)
	{
		memcpy(dest, *pBuffer, len);
		*pBuffer += len;
	}

	//写完后buffer往后增加
	static void WriteBuffer(void *src, char **pBuffer, int len)
	{
		memcpy(*pBuffer, src, len);
		*pBuffer += len;
	}
};

/////////////////////////////////////////////////////////////////////////////////
//Server to client msg

//告诉玩家他的id
struct SNetMsgScId : public SNetMsgBase
{
	int playerId;

	SNetMsgScId()
	{
		msgId = NETMSG_SC_ID;
	}

	//返回读取的字节数
	virtual int Read(char *buffer)
	{
		char *buffeTemprVar = buffer;
		ReadBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		ReadBuffer(&playerId, &buffeTemprVar, sizeof(playerId));
		return buffeTemprVar - buffer;
	}

	//返回写入的字节数
	virtual int Write(char *buffer)
	{
		char *buffeTemprVar = buffer;
		WriteBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		WriteBuffer(&playerId, &buffeTemprVar, sizeof(playerId));
		return buffeTemprVar - buffer;
	}
};

struct SNetMsgScPlayerList : public SNetMsgBase
{
	int playerCount;
	SNetPlayerInfo *pArray;

	//添加析构函数，确保释放内存
	SNetMsgScPlayerList()
	{
		msgId = NETMSG_SC_PLAYER_LIST;
		pArray = NULL;
	}
	~SNetMsgScPlayerList()
	{
		if (pArray != NULL)
		{
			delete pArray;
			pArray = NULL;
		}
	}

	//返回读取的字节数
	virtual int Read(char *buffer)
	{
		char *buffeTemprVar = buffer;
		ReadBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		ReadBuffer(&playerCount, &buffeTemprVar, sizeof(playerCount));
		pArray = new SNetPlayerInfo[playerCount];
		ReadBuffer(pArray, &buffeTemprVar, playerCount * sizeof(SNetPlayerInfo));
		return buffeTemprVar - buffer;
	}

	//返回写入的字节数
	virtual int Write(char *buffer)
	{
		char *buffeTemprVar = buffer;
		WriteBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		WriteBuffer(&playerCount, &buffeTemprVar, sizeof(playerCount));
		WriteBuffer(pArray, &buffeTemprVar, playerCount * sizeof(SNetPlayerInfo));
		return buffeTemprVar - buffer;
	}
};

struct SNetMsgScChallenge : public SNetMsgBase
{
	int inviterId;
	int inviteeId;

	SNetMsgScChallenge()
	{
		msgId = NETMSG_SC_CHALLENGE;
	}

	//返回读取的字节数
	virtual int Read(char *buffer)
	{
		char *buffeTemprVar = buffer;
		ReadBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		ReadBuffer(&inviterId, &buffeTemprVar, sizeof(inviterId));
		ReadBuffer(&inviteeId, &buffeTemprVar, sizeof(inviteeId));
		return buffeTemprVar - buffer;
	}

	//返回写入的字节数
	virtual int Write(char *buffer)
	{
		char *buffeTemprVar = buffer;
		WriteBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		WriteBuffer(&inviterId, &buffeTemprVar, sizeof(inviterId));
		WriteBuffer(&inviteeId, &buffeTemprVar, sizeof(inviteeId));
		return buffeTemprVar - buffer;
	}
};

struct SNetMsgScChallengeConfirm : public SNetMsgBase
{
	int inviterId;
	int inviteeId;
	int answer;

	SNetMsgScChallengeConfirm()
	{
		msgId = NETMSG_SC_CHALLENGE_CONFIRM;
	}

	//返回读取的字节数
	virtual int Read(char *buffer)
	{
		char *buffeTemprVar = buffer;
		ReadBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		ReadBuffer(&inviterId, &buffeTemprVar, sizeof(inviterId));
		ReadBuffer(&inviteeId, &buffeTemprVar, sizeof(inviteeId));
		ReadBuffer(&answer, &buffeTemprVar, sizeof(answer));
		return buffeTemprVar - buffer;
	}

	//返回写入的字节数
	virtual int Write(char *buffer)
	{
		char *buffeTemprVar = buffer;
		WriteBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		WriteBuffer(&inviterId, &buffeTemprVar, sizeof(inviterId));
		WriteBuffer(&inviteeId, &buffeTemprVar, sizeof(inviteeId));
		WriteBuffer(&answer, &buffeTemprVar, sizeof(answer));
		return buffeTemprVar - buffer;
	}
};

struct SNetMsgScCancelChallenge : public SNetMsgBase
{
	SNetMsgScCancelChallenge()
	{
		msgId = NETMSG_SC_CANCEL_CHALLENGE;
	}

	//返回读取的字节数
	virtual int Read(char *buffer)
	{
		char *buffeTemprVar = buffer;
		ReadBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		return buffeTemprVar - buffer;
	}

	//返回写入的字节数
	virtual int Write(char *buffer)
	{
		char *buffeTemprVar = buffer;
		WriteBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		return buffeTemprVar - buffer;
	}
};

struct SNetMsgScMove : public SNetMsgBase
{
	int ijArray[6];//01,23,45分别是选子，走子，放箭的坐标

	SNetMsgScMove()
	{
		msgId = NETMSG_SC_MOVE;
	}

	//返回读取的字节数
	virtual int Read(char *buffer)
	{
		char *buffeTemprVar = buffer;
		ReadBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		ReadBuffer(ijArray, &buffeTemprVar, sizeof(ijArray));
		return buffeTemprVar - buffer;
	}

	//返回写入的字节数
	virtual int Write(char *buffer)
	{
		char *buffeTemprVar = buffer;
		WriteBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		WriteBuffer(ijArray, &buffeTemprVar, sizeof(ijArray));
		return buffeTemprVar - buffer;
	}
};

struct SNetMsgScGameControl : public SNetMsgBase
{
	int action;//1=开始，2=暂停，3=继续，4=停止

	SNetMsgScGameControl()
	{
		msgId = NETMSG_SC_GAME_CONTROL;
	}

	//返回读取的字节数
	virtual int Read(char *buffer)
	{
		char *buffeTemprVar = buffer;
		ReadBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		ReadBuffer(&action, &buffeTemprVar, sizeof(action));
		return buffeTemprVar - buffer;
	}

	//返回写入的字节数
	virtual int Write(char *buffer)
	{
		char *buffeTemprVar = buffer;
		WriteBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		WriteBuffer(&action, &buffeTemprVar, sizeof(action));
		return buffeTemprVar - buffer;
	}
};

struct SNetMsgScError : public SNetMsgBase
{
	TCHAR szError[128];

	SNetMsgScError()
	{
		msgId = NETMSG_SC_ERROR;
	}

	//返回读取的字节数
	virtual int Read(char *buffer)
	{
		char *buffeTemprVar = buffer;
		ReadBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		ReadBuffer(szError, &buffeTemprVar, sizeof(szError));
		return buffeTemprVar - buffer;
	}

	//返回写入的字节数
	virtual int Write(char *buffer)
	{
		char *buffeTemprVar = buffer;
		WriteBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		WriteBuffer(szError, &buffeTemprVar, sizeof(szError));
		return buffeTemprVar - buffer;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//client to server msg

struct SNetMsgCsRequireId : public SNetMsgBase
{
	SNetMsgCsRequireId()
	{
		msgId = NETMSG_CS_REQUIRE_ID;
	}

	//返回读取的字节数
	virtual int Read(char *buffer)
	{
		char *buffeTemprVar = buffer;
		ReadBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		return buffeTemprVar - buffer;
	}

	//返回写入的字节数
	virtual int Write(char *buffer)
	{
		char *buffeTemprVar = buffer;
		WriteBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		return buffeTemprVar - buffer;
	}
};

struct SNetMsgCsPlayerInfo : public SNetMsgBase
{
	int playerId;
	int playerCamp;//玩家1还是玩家2
	TCHAR name[NET_MAX_PLAYER_NAME_LENGTH];

	SNetMsgCsPlayerInfo()
	{
		msgId = NETMSG_CS_PLAYER_INFO;
	}

	//返回读取的字节数
	virtual int Read(char *buffer)
	{
		char *buffeTemprVar = buffer;
		ReadBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		ReadBuffer(&playerId, &buffeTemprVar, sizeof(playerId));
		ReadBuffer(&playerCamp, &buffeTemprVar, sizeof(playerCamp));
		ReadBuffer(&name, &buffeTemprVar, sizeof(name));
		return buffeTemprVar - buffer;
	}

	//返回写入的字节数
	virtual int Write(char *buffer)
	{
		char *buffeTemprVar = buffer;
		WriteBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		WriteBuffer(&playerId, &buffeTemprVar, sizeof(playerId));
		WriteBuffer(&playerCamp, &buffeTemprVar, sizeof(playerCamp));
		WriteBuffer(&name, &buffeTemprVar, sizeof(name));
		return buffeTemprVar - buffer;
	}
};

struct SNetMsgCsRequirePlayerList : public SNetMsgBase
{
	int playerId;

	SNetMsgCsRequirePlayerList()
	{
		msgId = NETMSG_CS_REQUIRE_PLAYER_LIST;
	}

	//返回读取的字节数
	virtual int Read(char *buffer)
	{
		char *buffeTemprVar = buffer;
		ReadBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		ReadBuffer(&playerId, &buffeTemprVar, sizeof(playerId));
		return buffeTemprVar - buffer;
	}

	//返回写入的字节数
	virtual int Write(char *buffer)
	{
		char *buffeTemprVar = buffer;
		WriteBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		WriteBuffer(&playerId, &buffeTemprVar, sizeof(playerId));
		return buffeTemprVar - buffer;
	}
};

struct SNetMsgCsChallenge : public SNetMsgBase
{
	int inviterId;
	int inviteeId;

	SNetMsgCsChallenge()
	{
		msgId = NETMSG_CS_CHALLENGE;
	}

	//返回读取的字节数
	virtual int Read(char *buffer)
	{
		char *buffeTemprVar = buffer;
		ReadBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		ReadBuffer(&inviterId, &buffeTemprVar, sizeof(inviterId));
		ReadBuffer(&inviteeId, &buffeTemprVar, sizeof(inviteeId));
		return buffeTemprVar - buffer;
	}

	//返回写入的字节数
	virtual int Write(char *buffer)
	{
		char *buffeTemprVar = buffer;
		WriteBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		WriteBuffer(&inviterId, &buffeTemprVar, sizeof(inviterId));
		WriteBuffer(&inviteeId, &buffeTemprVar, sizeof(inviteeId));
		return buffeTemprVar - buffer;
	}
};

struct SNetMsgCsChallengeConfirm : public SNetMsgBase
{
	int inviterId;
	int inviteeId;
	int answer;

	SNetMsgCsChallengeConfirm()
	{
		msgId = NETMSG_CS_CHALLENGE_CONFIRM;
	}

	//返回读取的字节数
	virtual int Read(char *buffer)
	{
		char *buffeTemprVar = buffer;
		ReadBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		ReadBuffer(&inviterId, &buffeTemprVar, sizeof(inviterId));
		ReadBuffer(&inviteeId, &buffeTemprVar, sizeof(inviteeId));
		ReadBuffer(&answer, &buffeTemprVar, sizeof(answer));
		return buffeTemprVar - buffer;
	}

	//返回写入的字节数
	virtual int Write(char *buffer)
	{
		char *buffeTemprVar = buffer;
		WriteBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		WriteBuffer(&inviterId, &buffeTemprVar, sizeof(inviterId));
		WriteBuffer(&inviteeId, &buffeTemprVar, sizeof(inviteeId));
		WriteBuffer(&answer, &buffeTemprVar, sizeof(answer));
		return buffeTemprVar - buffer;
	}
};

struct SNetMsgCsCancelChallenge : public SNetMsgBase
{
	int playerId;

	SNetMsgCsCancelChallenge()
	{
		msgId = NETMSG_CS_CANCEL_CHALLENGE;
	}

	//返回读取的字节数
	virtual int Read(char *buffer)
	{
		char *buffeTemprVar = buffer;
		ReadBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		ReadBuffer(&playerId, &buffeTemprVar, sizeof(playerId));
		return buffeTemprVar - buffer;
	}

	//返回写入的字节数
	virtual int Write(char *buffer)
	{
		char *buffeTemprVar = buffer;
		WriteBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		WriteBuffer(&playerId, &buffeTemprVar, sizeof(playerId));
		return buffeTemprVar - buffer;
	}
};

struct SNetMsgCsMove : public SNetMsgBase
{
	int playerId;
	int ijArray[6];//01,23,45分别是选子，走子，放箭的坐标

	SNetMsgCsMove()
	{
		msgId = NETMSG_CS_MOVE;
	}

	//返回读取的字节数
	virtual int Read(char *buffer)
	{
		char *buffeTemprVar = buffer;
		ReadBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		ReadBuffer(&playerId, &buffeTemprVar, sizeof(playerId));
		ReadBuffer(ijArray, &buffeTemprVar, sizeof(ijArray));
		return buffeTemprVar - buffer;
	}

	//返回写入的字节数
	virtual int Write(char *buffer)
	{
		char *buffeTemprVar = buffer;
		WriteBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		WriteBuffer(&playerId, &buffeTemprVar, sizeof(playerId));
		WriteBuffer(ijArray, &buffeTemprVar, sizeof(ijArray));
		return buffeTemprVar - buffer;
	}
};

struct SNetMsgCsGameControl : public SNetMsgBase
{
	int playerId;
	int action;//1=开始，2=暂停，3=继续，4=停止

	SNetMsgCsGameControl()
	{
		msgId = NETMSG_CS_GAME_CONTROL;
	}

	//返回读取的字节数
	virtual int Read(char *buffer)
	{
		char *buffeTemprVar = buffer;
		ReadBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		ReadBuffer(&playerId, &buffeTemprVar, sizeof(playerId));
		ReadBuffer(&action, &buffeTemprVar, sizeof(action));
		return buffeTemprVar - buffer;
	}

	//返回写入的字节数
	virtual int Write(char *buffer)
	{
		char *buffeTemprVar = buffer;
		WriteBuffer(&msgId, &buffeTemprVar, sizeof(msgId));
		WriteBuffer(&playerId, &buffeTemprVar, sizeof(playerId));
		WriteBuffer(&action, &buffeTemprVar, sizeof(action));
		return buffeTemprVar - buffer;
	}
};