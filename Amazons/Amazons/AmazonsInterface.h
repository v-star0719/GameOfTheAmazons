#ifndef AMAZONS_INTERFACE_H
#define AMAZONS_INTERFACE_H

#include "StdAfx.h"

//玩家类型
#define PLAYER_HUMAN	0
#define PLAYER_COMPUTER 1
#define PLAYER_NETWORK	2

//位置坐标
struct SPosition
{
	int i;//行
	int j;//列

	SPosition()
	{
	}

	SPosition(int i, int j)
	{
		this->i = i;
		this->j = j;
	}
};

//历史步骤
struct SHistoryRecord
{
	int spentTime;
	SPosition posSelected;//选子
	SPosition posMoveTo;//着子
	SPosition posShootAt;//放箭
};

class CAmazonsGameControllerInterface
{
public:
	bool m_bPlayer1Black;//玩家1是否是红的
	bool m_bPlayer1First;//玩家1是否先走
	int m_nPlayer1Type;//玩家1类型
	int m_nPlayer2Type;//玩家2类型

public:
	virtual void Init(){};
	virtual void NewGame(){};
	//
	virtual void GetPlate(int outPlate[10][10]){};
	virtual void GetPieces(SPosition outPieces[9]){};
	virtual int  GetPlateCellValue(int i, int j){ return 0; }
	//
	virtual void Move(SPosition posFrom, SPosition posTo){};
	virtual void Shoot(SPosition posShootAt){};
	virtual void CancelMove(SPosition posFrom, SPosition posTo){};
	virtual void CancelShoot(SPosition posShootAt){};
	virtual bool CanMoveOrShoot(SPosition posFrom, SPosition posTo){ return true; };
	//
	virtual bool IsPieces(int n){ return true; };
	virtual bool IsEmpty(int n){ return true; };
	virtual bool IsArrow(int n){ return true; };
	virtual bool IsPlayer1(int n){ return true; };
	virtual bool IsPlayer2(int n){ return true; };
	virtual int  IsGameOver(){ return 0; };//返回值：0=未分胜负，1=玩家1赢，2=玩家2赢，3=平局
	virtual bool IsPlayer1Turn(){ return true; };//是否轮到玩家1走子，返回true表示轮到玩家1，返回false表示轮到玩家2
	virtual bool IsPlayer1Turn(int nHistoryPtr){ return true; };//是否轮到玩家1走子，返回true表示轮到玩家1，返回false表示轮到玩家2
	//
	virtual void SaveToFile(CString filePathName){};
	virtual void LoadFromFile(CString filePathName){};
	virtual void SaveTempFile(CString filePathName){};
	//
	virtual int GetTotalTime(bool bPlayer1){ return 0; };
	virtual void GetHistory(SHistoryRecord history[], int &outHistoryPtr, int &outCurPtr){};
	virtual SHistoryRecord GetCurHistoryMove(){ return SHistoryRecord(); };
	virtual void AddToHistory(SPosition posSelect, SPosition posMoveTo, SPosition posShootAt, int time){};//AddToHistory会使指针后移，从而让对方开始下棋
	virtual void Forward(int step){};
	virtual void Backward(int step){};
	virtual bool CanForward(int step){ return true; }
	virtual bool CanBackward(int step){ return true; }
};

class CAmazonsAIInterface
{
public:
	int m_nOutMaxValue;//(输出)计算最大评估值
	int m_nOutDepth;//(输出)计算深度
	int m_nOutNumOfMoves;//(输出)计算着法数
	int m_nOutNumOfNodes;//(输出)计算产生的节点数(真正的树的节点数)(调试用)
	int m_nOutNumOfSkippedNodes;//(输出)计算遍历过的节点数(把扔掉的节点也算进来)

public:
	virtual void AICompute(
		int plate[10][10],			//棋盘数据
		SPosition pieces[],			//8个棋子的位置，长度为9
		bool isPlayer1,				//是否是作为玩家1进行计算，否则作为玩家2进行计算
		SPosition& outPosSelected,	//输出着子
		SPosition& outPosMoveTo,	//输出着子
		SPosition& outPosShootAt){};	//输出着子
	virtual void StopComputing(){};
	//
};

#endif