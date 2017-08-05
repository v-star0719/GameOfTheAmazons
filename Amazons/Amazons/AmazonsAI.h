#ifndef AMAZONS_AI__H
#define AMAZONS_AI__H

#include "AmazonsInterface.h"

class CAmazonsAI : public CAmazonsAIInterface
{
public:
	virtual void AICompute(
		int plate[10][10],			//棋盘数据
		SPosition pieces[],			//8个棋子的位置，长度为9
		bool isPlayer1,				//是否是作为玩家1进行计算，否则作为玩家2进行计算
		SPosition& outPosSelected,	//输出着子
		SPosition& outPosMoveTo,	//输出着子
		SPosition& outPosShootAt);	//输出着子
};
#endif