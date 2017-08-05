#include "StdAfx.h"
#include "AmazonsGameController.h"
#include "AmazonsAI.h"

void CAmazonsAI::AICompute(
	int plate[10][10],			//棋盘数据
	SPosition pieces[],			//8个棋子的位置，长度为9
	bool isPlayer1,				//是否是作为玩家1进行计算，否则作为玩家2进行计算
	SPosition& outPosSelected,	//输出着子
	SPosition& outPosMoveTo,	//输出着子
	SPosition& outPosShootAt)	//输出着子
{
	m_nOutMaxValue = 0;
	m_nOutDepth = 0;
	m_nOutNumOfMoves = 0;
	m_nOutNumOfNodes = 0;
	m_nOutNumOfSkippedNodes = 0;

	bool bFinished = false;

	while (!bFinished)
	{
		//随机选择一个棋子
		int piece = 0;
		if (isPlayer1)
			piece = rand() % 4 + 1;
		else
			piece = rand() % 4 + 5;
		outPosSelected = pieces[piece];

		//随机走子位置
		SPosition pos;
		pos.i = rand() % 10;
		pos.j = rand() % 10;

		if (CAmazonsGameController::CanMoveOrShoot(plate, pieces, outPosSelected, pos))
		{
			CAmazonsGameController::Move(plate, pieces, outPosSelected, pos);
			outPosMoveTo = pos;
		}
		else
			continue;

		//随机放箭
		pos.i = rand() % 10;
		pos.j = rand() % 10;
		if (CAmazonsGameController::CanMoveOrShoot(plate, pieces, outPosMoveTo, pos))
		{
			outPosShootAt = pos;
			bFinished = true;
		}
		else
		{
			CAmazonsGameController::CancelMove(plate, pieces, outPosSelected, outPosMoveTo);
		}
	}
}