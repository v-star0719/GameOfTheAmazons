#ifndef AMAZONS_AI_GAME_CONTROLLER_H
#define AMAZONS_AI_GAME_CONTROLLER_H

#include "StdAfx.h"
#include "AmazonsInterface.h"

/*
棋盘数据表示
（1）用10*10的数组存储
（2）玩家1的棋子对应值为1，2，3，4；玩家2的棋子对应值为5，6，7，8
（3）空的位置值为0，放箭位置为-1
*/

struct SNode//按需要使用，多余的删除即可
{
	int n;//调试时标明深度,正式版删除之
	int nValue;
	int nSelected;
	int nMoveTo;
	int nShootAt;
	SNode* pParent;//父母(Alpha-Beta搜索用)
	SNode* pBrother;//兄弟
	SNode* pChild;//孩子
};

class CAmazonsGameController : public CAmazonsGameControllerInterface
{
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
	virtual void Init();
	virtual void NewGame();
	//
	virtual void GetPlate(int outPlate[10][10]);
	virtual int  GetPlateCellValue(int i, int j);
	virtual void GetPieces(SPosition outPieces[9]);
	//
	virtual void Move(SPosition posFrom, SPosition posTo);
	virtual void Shoot(SPosition posShootAt);
	virtual void CancelMove(SPosition posFrom, SPosition posTo);
	virtual void CancelShoot(SPosition posShootAt);
	virtual bool CanMoveOrShoot(SPosition posFrom, SPosition posTo);
	//
	virtual bool IsPieces(int n);
	virtual bool IsEmpty(int n);
	virtual bool IsArrow(int n);
	virtual bool IsPlayer1(int n);
	virtual bool IsPlayer2(int n);
	virtual int IsGameOver();//返回值：0=未分胜负，1=玩家1赢，2=玩家2赢，3=平局
	virtual bool IsPlayer1Turn();//是否轮到玩家1走子，返回true表示轮到玩家1，返回false表示轮到玩家2
	virtual bool IsPlayer1Turn(int nHistoryPtr);//是否轮到玩家1走子，返回true表示轮到玩家1，返回false表示轮到玩家2
	//
	virtual void SaveToFile(CString filePathName);
	virtual void LoadFromFile(CString filePathName);
	virtual void SaveTempFile(CString filePathName);
	//
	virtual int GetTotalTime(bool bPlayer1);
	virtual void GetHistory(SHistoryRecord history[], int &outPtr, int &outCurPtr);
	virtual SHistoryRecord GetCurHistoryMove();
	virtual void AddToHistory(SPosition posSelect, SPosition posMoveTo, SPosition posShootAt, int time);//AddToHistory会使指针后移，从而让对方开始下棋
	virtual void Forward(int step);
	virtual void Backward(int step);
	virtual bool CanForward(int step);
	virtual bool CanBackward(int step);

public:
	static void Move(int plate[10][10], SPosition pieces[9], SPosition posFrom, SPosition posTo);
	static void Shoot(int plate[10][10], SPosition pieces[9], SPosition posShootAt);
	static void CancelMove(int plate[10][10], SPosition pieces[9], SPosition posFrom, SPosition posTo);
	static void CancelShoot(int plate[10][10], SPosition pieces[9], SPosition posShootAt);
	static bool CanMoveOrShoot(int plate[10][10], SPosition pieces[9], SPosition posFrom, SPosition posTo);
};

#endif