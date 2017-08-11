#ifndef AMAZONS_AI_GAME_CONTROLLER_H
#define AMAZONS_AI_GAME_CONTROLLER_H

#include "StdAfx.h"

//玩家类型
#define PLAYER_HUMAN	0
#define PLAYER_COMPUTER 1
#define PLAYER_NETWORK	2


/*
棋盘数据表示
（1）用10*10的数组存储
（2）玩家1的棋子对应值为1，2，3，4；玩家2的棋子对应值为5，6，7，8
（3）空的位置值为0，放箭位置为-1
*/

//位置坐标
struct SPosition
{
	int i;//行
	int j;//列

	SPosition()
	{
		i = -1;
		j = -1;
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

class CAmazonsGameController
{
public:
	bool m_bPlayer1Black;//玩家1是否是红的
	bool m_bPlayer1First;//玩家1是否先走
	int m_nPlayer1Type;//玩家1类型
	int m_nPlayer2Type;//玩家2类型

private:
	int m_plate[10][10];
	SPosition m_pieces[9];
	//
	SHistoryRecord m_histroy[92];
	int m_nHistoryPtr;//初始值为0，满了后为93
	int m_nCurHisttoryPtr;//当进行前进或后退时，它的值会变化，而m_nHistoryPtr不变

public:
	CAmazonsGameController(){};
	~CAmazonsGameController(){};

public:
	void Init();
	void NewGame();
	//
	void GetPlate(int outPlate[10][10]);
	int  GetPlateCellValue(int i, int j);
	void GetPieces(SPosition outPieces[9]);
	//
	void Move(SPosition posFrom, SPosition posTo);
	void Shoot(SPosition posShootAt);
	void CancelMove(SPosition posFrom, SPosition posTo);
	void CancelShoot(SPosition posShootAt);
	bool CanMoveOrShoot(SPosition posFrom, SPosition posTo);
	//
	bool IsPieces(int n);
	bool IsEmpty(int n);
	bool IsArrow(int n);
	bool IsPlayer1(int n);
	bool IsPlayer2(int n);
	int IsGameOver();//返回值：0=未分胜负，1=玩家1赢，2=玩家2赢，3=平局
	bool IsPlayer1Turn();//是否轮到玩家1走子，返回true表示轮到玩家1，返回false表示轮到玩家2
	bool IsPlayer1Turn(int nHistoryPtr);//是否轮到玩家1走子，返回true表示轮到玩家1，返回false表示轮到玩家2
	//
	void SaveToFile(CString filePathName);
	void LoadFromFile(CString filePathName);
	void SaveTempFile(CString filePathName);
	//
	int GetTotalTime(bool bPlayer1);
	void GetHistory(SHistoryRecord history[], int &outPtr, int &outCurPtr);
	SHistoryRecord GetCurHistoryMove();
	void AddToHistory(SPosition posSelect, SPosition posMoveTo, SPosition posShootAt, int time);//AddToHistory会使指针后移，从而让对方开始下棋
	void Forward(int step);
	void Backward(int step);
	bool CanForward(int step);
	bool CanBackward(int step);

public:
	static void Move(int plate[10][10], SPosition pieces[9], SPosition posFrom, SPosition posTo);
	static void Shoot(int plate[10][10], SPosition pieces[9], SPosition posShootAt);
	static void CancelMove(int plate[10][10], SPosition pieces[9], SPosition posFrom, SPosition posTo);
	static void CancelShoot(int plate[10][10], SPosition pieces[9], SPosition posShootAt);
	static bool CanMoveOrShoot(int plate[10][10], SPosition pieces[9], SPosition posFrom, SPosition posTo);
};

#endif