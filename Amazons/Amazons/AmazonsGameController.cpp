#include "StdAfx.h"
#include "AmazonsGameController.h"

#define SET_PIECES_POS(r, c, n) m_plate[r][c] = n; m_pieces[n].i = r; m_pieces[n].j = c;

bool IsCellEmpty(int plate[10][10], int i, int j)
{
	if (0 <= i && i <= 9 && 0 <= j && j <= 9)
		return plate[i][j] == 0;
	return false;
}

int GetMovableDirections(int plate[10][10], SPosition piece)
{
	int i = piece.i;
	int j = piece.j;
	int n = 0;
	if (IsCellEmpty(plate, i + 1, j - 1)) n++;
	if (IsCellEmpty(plate, i + 1, j    )) n++;
	if (IsCellEmpty(plate, i + 1, j + 1)) n++;
	if (IsCellEmpty(plate, i,     j - 1)) n++;
	if (IsCellEmpty(plate, i,     j + 1)) n++;
	if (IsCellEmpty(plate, i - 1, j - 1)) n++;
	if (IsCellEmpty(plate, i - 1, j    )) n++;
	if (IsCellEmpty(plate, i - 1, j + 1)) n++;
	return n;
}

void CAmazonsGameController::Init()
{
	m_bPlayer1Black = true;
	m_bPlayer1First = true;
	m_nPlayer1Type = PLAYER_COMPUTER;
	m_nPlayer2Type = PLAYER_HUMAN;
	NewGame();
}

void CAmazonsGameController::NewGame()
{
	memset(m_plate, 0, sizeof(m_plate));
	memset(m_histroy, 0, sizeof(m_histroy));
	m_nHistoryPtr = 0;
	m_nCurHisttoryPtr = 0;

	//玩家1
	SET_PIECES_POS(0, 3, 1)
	SET_PIECES_POS(0, 6, 2)
	SET_PIECES_POS(3, 0, 3)
	SET_PIECES_POS(3, 9, 4)
	//玩家2
	SET_PIECES_POS(6, 0, 5)
	SET_PIECES_POS(6, 9, 6)
	SET_PIECES_POS(9, 3, 7)
	SET_PIECES_POS(9, 6, 8)
}

void CAmazonsGameController::GetPlate(int outPlate[10][10])
{
	memcpy(outPlate, m_plate, sizeof(m_plate));
}

int CAmazonsGameController::GetPlateCellValue(int i, int j)
{
	return m_plate[i][j];
}

void CAmazonsGameController::GetPieces(SPosition outPieces[9])
{
	memcpy(outPieces, m_pieces, sizeof(m_pieces));
}

bool CAmazonsGameController::IsPieces(int n)
{ 
	return 1<=n && n<=8;
};
bool CAmazonsGameController::IsEmpty(int n)
{ 
	return n == 0;
};
bool CAmazonsGameController::IsArrow(int n)
{
	return n == -1;
};

bool CAmazonsGameController::IsPlayer1(int n)
{
	return 1<=n && n<=4;
}

bool CAmazonsGameController::IsPlayer2(int n)
{
	return  5<=n && n<=8;
}

bool CAmazonsGameController::IsPlayer1Turn()
{
	return IsPlayer1Turn(m_nCurHisttoryPtr);
}

bool CAmazonsGameController::IsPlayer1Turn(int nHistoryPtr)
{
	if (m_bPlayer1First)
		return (nHistoryPtr & 0x1) == 0;
	else
		return (nHistoryPtr & 0x1) == 1;
}

//返回值：0=未分胜负，1=玩家1赢，2=玩家2赢，3=平局
int CAmazonsGameController::IsGameOver()
{
	int nPlay1 = 0;
	int nPlay2 = 0;
	for (int i = 1; i < 5; i++)
		nPlay1 += GetMovableDirections(m_plate, m_pieces[i]);
	for (int i = 5; i < 9; i++)
		nPlay2 += GetMovableDirections(m_plate, m_pieces[i]);

	//规则是：一方走子后，对方不能再移动，为胜利
	//换个理解方式就是：轮到某一方走子时，该方不能走子，对方胜
	if (IsPlayer1Turn())
	{
		if (nPlay1 == 0 && nPlay2 == 0) return 3;
		if (nPlay1 == 0) return 2;
	}
	else
	{
		if (nPlay1 == 0 && nPlay2 == 0) return 3;
		if (nPlay2 == 0) return 1;
	}

	return 0;
}


void CAmazonsGameController::Move(SPosition posFrom, SPosition posTo)
{
	Move(m_plate, m_pieces, posFrom, posTo);
}

void CAmazonsGameController::Shoot(SPosition posShootAt)
{
	Shoot(m_plate, m_pieces, posShootAt);
}

void CAmazonsGameController::CancelMove(SPosition posFrom, SPosition posTo)
{
	CancelMove(m_plate, m_pieces, posFrom, posTo);
}

void CAmazonsGameController::CancelShoot(SPosition posShootAt)
{
	CancelShoot(m_plate, m_pieces, posShootAt);
}

bool CAmazonsGameController::CanMoveOrShoot(SPosition posFrom, SPosition posTo)
{
	return CanMoveOrShoot(m_plate, m_pieces, posFrom, posTo);
}

/////////////////////////////////////////////////////////////////////////////
void CAmazonsGameController::Move(int plate[10][10], SPosition pieces[9], SPosition posFrom, SPosition posTo)
{
	int piece = plate[posFrom.i][posFrom.j];
	plate[posFrom.i][posFrom.j] = 0;
	plate[posTo.i][posTo.j] = piece;
	pieces[piece] = posTo;
}

void CAmazonsGameController::Shoot(int plate[10][10], SPosition pieces[9], SPosition posShootAt)
{
	plate[posShootAt.i][posShootAt.j] = -1;
}

void CAmazonsGameController::CancelMove(int plate[10][10], SPosition pieces[9], SPosition posFrom, SPosition posTo)
{
	Move(plate, pieces, posTo, posFrom);
}

void CAmazonsGameController::CancelShoot(int plate[10][10], SPosition pieces[9], SPosition posShootAt)
{
	plate[posShootAt.i][posShootAt.j] = 0;
}

bool CAmazonsGameController::CanMoveOrShoot(int plate[10][10], SPosition pieces[9], SPosition posFrom, SPosition posTo)
{
	int fromI = posFrom.i;
	int fromJ = posFrom.j;
	int toI = posTo.i;
	int toJ = posTo.j;
	if (plate[toI][toJ] != 0) return false;

	int i, j, k;
	int count1 = 0, count2 = 0;
	if (fromJ == toJ) k = 0;//在垂直方向上
	else if (fromI == toI) k = 1;//在水平方向上
	else if (fromI - fromJ == toI - toJ) k = 2;//在左斜线上
	else if (fromI + fromJ == toI + toJ) k = 3;//在右斜线上
	else return false;
	switch (k)
	{
	case 0:///////////////////////////////////////////////////////垂直方向
		j = fromJ;
		if (toI > fromI)//下
			for (i = fromI + 1; i < toI; i++)
				if (plate[i][j] != 0)	return false;
		if (toI < fromI)//上
			for (i = fromI - 1; i > toI; i--)
				if (plate[i][j] != 0)	return false;
		return 1;
	case 1:////////////////////////////////////////////////////水平线
		i = fromI;
		if (toJ > fromJ)//右
			for (j = fromJ + 1; j < toJ; j++)
				if (plate[i][j] != 0)	return false;

		if (toJ < fromJ)//左
			for (j = fromJ - 1; j > toJ; j--)
				if (plate[i][j] != 0)	return false;
		return 1;
	case 2://////////////////////////////////////////////////左斜线
		i = fromI; j = fromJ;
		if (toI > fromI)//下
			for (i = i + 1; i < toI; i++)
				if (plate[i][++j] != 0)	return false;
		if (toI < fromI)//上
			for (i = i - 1; i > toI; i--)
				if (plate[i][--j] != 0)	return false;
		return 1;
	case 3://////////////////////////////////////////////////右斜线
		i = fromI; j = fromJ;
		if (toI > fromI)//下
			for (i = i + 1; i < toI; i++)
				if (plate[i][--j] != 0)	return false;
		if (toI < fromI)//上
			for (i = i - 1; i > toI; i--)
				if (plate[i][++j] != 0)	return false;

	}
	return true;
}

int CAmazonsGameController::GetTotalTime(bool bPlayer1)
{
	int t = 0;
	for (int i = 0; i < m_nCurHisttoryPtr; i++)
	{
		if (bPlayer1)
		{
			if (IsPlayer1Turn(i))
				t += m_histroy[i].spentTime;
		}
		else
		{
			if (!IsPlayer1Turn(i))
				t += m_histroy[i].spentTime;
		}
	}
	return t;
}

void CAmazonsGameController::GetHistory(SHistoryRecord history[], int &outPtr, int &outCurPtr)
{
	outPtr = m_nHistoryPtr;
	outCurPtr = m_nCurHisttoryPtr;
	memcpy(history, m_histroy, sizeof(m_histroy));
}

SHistoryRecord CAmazonsGameController::GetCurHistoryMove()
{
	if (m_nCurHisttoryPtr > 0)
		return m_histroy[m_nCurHisttoryPtr - 1];
	else
	{
		SHistoryRecord record;
		record.posSelected = SPosition(-1, -1);
		record.posMoveTo = SPosition(-1, -1);
		record.posShootAt = SPosition(-1, -1);
		return record;
	}
}

void CAmazonsGameController::AddToHistory(SPosition posSelect, SPosition posMoveTo, SPosition posShootAt, int time)
{
	//如果两个指针不相等，说明进行了回退，就覆盖掉后面的记录
	if (m_nCurHisttoryPtr != m_nHistoryPtr)
		m_nHistoryPtr = m_nCurHisttoryPtr;

	SHistoryRecord &record = m_histroy[m_nHistoryPtr];
	record.posSelected = posSelect;
	record.posMoveTo = posMoveTo;
	record.posShootAt = posShootAt;
	record.spentTime = time;
	m_nHistoryPtr++;
	m_nCurHisttoryPtr++;
}

//如果step = -1，则前进到顶部
void CAmazonsGameController::Forward(int step)
{
	if (step < 0) step = 92;

	//cur是指向当前的下一步，从cur开始，一直前进到顶部（顶部是m_nHistoryPtr）
	while (step > 0 && m_nCurHisttoryPtr < m_nHistoryPtr)
	{
		SHistoryRecord record = m_histroy[m_nCurHisttoryPtr];
		Move(record.posSelected, record.posMoveTo);
		Shoot(record.posShootAt);
		m_nCurHisttoryPtr++;
		step--;
	}
}

//如果step = -1，则后退到底部
void CAmazonsGameController::Backward(int step)
{
	if (step < 0) step = 92;

	//cur是指向当前的下一步，从cur-1开始，一直退到底部（底部cur=0说明是空的）
	while (step > 0 && m_nCurHisttoryPtr > 0)
	{
		SHistoryRecord record = m_histroy[m_nCurHisttoryPtr - 1];
		CancelShoot(record.posShootAt);
		CancelMove(record.posSelected, record.posMoveTo);
		m_nCurHisttoryPtr--;
		step--;
	}
}

bool CAmazonsGameController::CanForward(int step)
{
	return m_nCurHisttoryPtr + step <= m_nHistoryPtr;
}

bool CAmazonsGameController::CanBackward(int step)
{
	return m_nCurHisttoryPtr - step >= 0;
}

void CAmazonsGameController::SaveToFile(CString filePathName)
{
	CFile file;
	if (!file.Open(filePathName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
	{
		AfxMessageBox(_T("文件保存失败"));
		return;
	}

	file.Write(&m_bPlayer1Black, sizeof(m_bPlayer1Black));
	file.Write(&m_bPlayer1First, sizeof(m_bPlayer1First));
	file.Write(&m_nPlayer1Type, sizeof(m_nPlayer1Type));
	file.Write(&m_nPlayer2Type, sizeof(m_nPlayer2Type));
	//
	file.Write(m_plate, sizeof(m_plate));
	file.Write(m_pieces, sizeof(m_pieces));
	//
	file.Write(&m_histroy, sizeof(m_histroy));
	file.Write(&m_nHistoryPtr, sizeof(m_nHistoryPtr));

	file.Close();
}

void CAmazonsGameController::LoadFromFile(CString filePathName)
{
	CFile file;
	if (!file.Open(filePathName, CFile::modeRead | CFile::typeBinary))
	{
		AfxMessageBox(_T("文件打开失败"));
		return;
	}

	file.Read(&m_bPlayer1Black, sizeof(m_bPlayer1Black));
	file.Read(&m_bPlayer1First, sizeof(m_bPlayer1First));
	file.Read(&m_nPlayer1Type, sizeof(m_nPlayer1Type));
	file.Read(&m_nPlayer2Type, sizeof(m_nPlayer2Type));
	//
	file.Read(m_plate, sizeof(m_plate));
	file.Read(m_pieces, sizeof(m_pieces));
	//
	file.Read(&m_histroy, sizeof(m_histroy));
	file.Read(&m_nHistoryPtr, sizeof(m_nHistoryPtr));

	file.Close();

	m_nCurHisttoryPtr = m_nHistoryPtr;
}

void CAmazonsGameController::SaveTempFile(CString filePathName)
{
	SaveToFile(filePathName);
}