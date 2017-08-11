// AmazonsView.cpp : implementation of the CAmazonsView class
//

#include "stdafx.h"
#include "AmazonsApp.h"
#include "math.h"
#include "MainFrm.h"

#include "AmazonsDoc.h"
#include "AmazonsView.h"
#include "AmazonsGameController.h"
#include "AmazonsAI.h"
#include "GlobalFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define STATE_UNSELECT 0	//等待选择棋子
#define STATE_SELECTED 1	//等待走子，或取消走子
#define STATE_MOVED 2		//等待放箭
#define STATE_SHOOTED 3		//放完箭

#define GAME_STATE_NOT_START 0
#define GAME_STATE_PLAYING 1
#define GAME_STATE_SUSPEND 2
#define GAME_STATE_OVER 3

#define  TIMER_INTERVAL 200//UI时钟间隔，单位ms

#define TEMP_FILE_NAME _T("$Amazons_temp.amz")


//游戏控制
CAmazonsGameController *g_pGameController = new CAmazonsGameController;

//电脑AI
CAmazonsAI *g_pAI = new CAmazonsAI;

/////////////////////////
//线程通信
SPosition g_posSelected, g_posMoveTo, g_posShootAt;
CEvent g_eventContinue(FALSE, FALSE);
bool g_bPlayAsPlayer1 = false;
/////////////////////////////////////////////////////////////////////////////
// CAmazonsView

IMPLEMENT_DYNCREATE(CAmazonsView, CView)

BEGIN_MESSAGE_MAP(CAmazonsView, CView)
	//{{AFX_MSG_MAP(CAmazonsView)
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_DESTROY()

	ON_COMMAND(ID_FILE_OPEN, OnCmdFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnCmdFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnCmdFileSaveAs)

	ON_COMMAND(ID_GAME_START, OnCmdGameStart)
	ON_UPDATE_COMMAND_UI(ID_GAME_START, OnCmdGameStartUpdate)
	ON_COMMAND(ID_NEW_GAME, OnCmdNewGame)
	ON_UPDATE_COMMAND_UI(ID_NEW_GAME, OnCmdNewGameUpdate)
	ON_COMMAND(ID_GAMESUSPEND, OnCmdGameSuspend)
	ON_UPDATE_COMMAND_UI(ID_GAMESUSPEND, OnCmdGamesuspendUpdate)
	ON_COMMAND(ID_BACKWARD_ONE, OnCmdBackwardOne)
	ON_UPDATE_COMMAND_UI(ID_BACKWARD_ONE, OnCmdBackwardOneUpdate)
	ON_COMMAND(ID_BACKWARD_TWO, OnCmdBackwardTwo)
	ON_UPDATE_COMMAND_UI(ID_BACKWARD_TWO, OnCmdBackwardTwoUpdate)
	ON_COMMAND(ID_BACKWARD_TO_START, OnCmdBackwardToStart)
	ON_UPDATE_COMMAND_UI(ID_BACKWARD_TO_START, OnCmdBackwardToStartUpdate)
	ON_COMMAND(ID_FORWARD_ONE, OnCmdForwardOne)
	ON_UPDATE_COMMAND_UI(ID_FORWARD_ONE, OnCmdForwardOneUpdate)
	ON_COMMAND(ID_FORWARD_TWO, OnCmdForwardTwo)
	ON_UPDATE_COMMAND_UI(ID_FORWARD_TWO, OnCmdForwardTwoUpdate)
	ON_COMMAND(ID_FORWARD_TO_END, OnCmdForwardToEnd)
	ON_UPDATE_COMMAND_UI(ID_FORWARD_TO_END, OnCmdForwardToEndUpdate)
	ON_COMMAND(ID_SETTINGS, OnCmdSettings)
	ON_UPDATE_COMMAND_UI(ID_SETTINGS, OnCmdSettingsUpdate)
	ON_COMMAND(ID_SHOW_HISTORY_DLG, OnCmdShowHistoryDlg)
	ON_UPDATE_COMMAND_UI(ID_SHOW_HISTORY_DLG, OnCmdShowHistoryDlgUpdate)
	ON_COMMAND(ID_MARK, OnCmdMark)
	ON_UPDATE_COMMAND_UI(ID_MARK, OnCmdMarkUpdate)
	ON_COMMAND(ID_SUGGEST_A_MOVE, OnCmdSuggest)
	ON_UPDATE_COMMAND_UI(ID_SUGGEST_A_MOVE, OnCmdSuggestUpdate)
	ON_COMMAND(ID_STOP_COMPUTING, OnCmdStopComputing)
	ON_UPDATE_COMMAND_UI(ID_STOP_COMPUTING, OnCmdStopComputingUpdate)
	ON_COMMAND(ID_NETWORK, OnCmdNetwork)
	ON_UPDATE_COMMAND_UI(ID_NETWORK, OnCmdNetworkUpdate)

	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_MESSAGE(WM_AI_COMPUTING_FINISHED, OnAIComputingFinished)
	ON_MESSAGE(WM_NETWORK_CONNECTED_TO_SERVER, OnNetworkConnectedToServer)
	ON_MESSAGE(WM_NETWORK_GET_PLAYER_LIST, OnNetworkGetPlayerList)
	ON_MESSAGE(WM_NETWORK_CONNECTED_TO_OPPONENT, OnNetworkConnectedToOpponent)
	ON_MESSAGE(WM_NETWORK_DISCONNECT_TO_OPPONENT, OnNetworkDisConnectToOpponent)
	ON_MESSAGE(WM_NETWORK_MOVE, OnNetworkMove)
	ON_MESSAGE(WM_NETWORK_LOG, OnNetworkLog)
	ON_MESSAGE(WM_NETWORK_GAME_CONTROL, OnNetworkGameControl)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAmazonsView construction/destruction

CAmazonsView::CAmazonsView()
{
	//绘图用变量
	m_ptTopLeft1 = CPoint(20, 20);
	m_ptTopLeft2 = m_ptTopLeft1 + CPoint(30, 30);
	m_nRightMinWidth = 270;
	m_clrDark = RGB(207, 140, 72);
	m_clrLight = RGB(253, 206, 162);
	m_clrArrow = RGB(128, 0, 0);
	m_clrViewBg = RGB(226, 224, 226);
	m_clrPlateBg = RGB(255, 255, 255);
	///////
	ResetVariables();
	m_bMarkMove = true;
	m_pDlgHistory = new CHistoryDlg(this);
	m_pDlgHistory->Create();//CDialog(CHistoryDlg::IDD, pParent)
	m_networkPlayer.m_pView = this;
	m_bThreadStart = false;
	m_bHistoryDlgOpened = false;
	m_pThread = NULL;
	m_bSetTimer = false;
	m_bHasInitDCAndBitmap = false;

	m_strMoves = "Moves: ";
	m_strDepth = "Depth: ";
	m_strNodes = "Nodes: ";
}

void CAmazonsView::ResetVariables()
{
	m_nGameState = GAME_STATE_NOT_START;
	m_bComputerMoving = false;
	m_bComputerThinking = false;
	//
	m_nCurState = STATE_UNSELECT;
	m_posSelected = m_posMoveTo = m_posShootAt = SPosition(-1, -1);

	m_strPlayer1TotalTime = _T("00:00:00");
	m_strPlayer2TotalTime = _T("00:00:00");
	m_strPlayer1Timer = _T("00:00:00");
	m_strPlayer2Timer = _T("00:00:00");
	m_strTotalTimer = _T("00:00:00");
	m_strGameState = _T("");
	m_strDepth.Format(_T("Depth:"));
	m_strNodes.Format(_T("Nodes:"));
	m_strMoves.Format(_T("Moves:"));
	m_nPlayerTimer = 0;
	m_nTotalTimer = 0;
}

CAmazonsView::~CAmazonsView()
{	
}

#ifdef _DEBUG
void CAmazonsView::AssertValid() const
{
	CView::AssertValid();
}

void CAmazonsView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAmazonsDoc* CAmazonsView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAmazonsDoc)));
	return (CAmazonsDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAmazonsView message handlers

#pragma region Thread

UINT CAmazonsView::Thread_AICompute(LPVOID pParam)
{
	TRACE("ComputingThreadStarted\n");
	HWND  hWnd = (HWND)pParam;
	int plate[10][10];
	SPosition pieces[9];
	while (1)
	{
		::WaitForSingleObject(g_eventContinue, INFINITE);
		g_pGameController->GetPlate(plate);
		g_pGameController->GetPieces(pieces);
		g_pAI->AICompute(plate, pieces, g_pGameController->IsPlayer1Turn(), g_posSelected, g_posMoveTo, g_posShootAt);
		::PostMessage(hWnd, WM_AI_COMPUTING_FINISHED, 0, 0);
	}
	return 0;
}

#pragma endregion Thread

#pragma region System Message

BOOL CAmazonsView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return true;
	//return CView::OnEraseBkgnd(pDC);
}

int CAmazonsView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	if (!m_bHasInitDCAndBitmap)
	{
		CDC *pDC = GetDC();
		m_clientMemDC.CreateCompatibleDC(pDC);
		m_bitmapMemDC.CreateCompatibleDC(pDC);
		m_bmClient.CreateCompatibleBitmap(pDC, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		m_clientMemDC.SelectObject(m_bmClient);
		m_clientMemDC.SetBkMode(TRANSPARENT);

		m_bmBlackPiece.LoadBitmap(IDB_BLACK);
		m_bmRedPiece.LoadBitmap(IDB_RED);
		m_bmSelectMark.LoadBitmap(IDB_SELECT);

		ReleaseDC(pDC);
		//pDC->DeleteDC();
		m_bHasInitDCAndBitmap = true;
	}
	return 0;
}

void CAmazonsView::OnDestroy()
{
	CView::OnDestroy();
	::DeleteFile(TEMP_FILE_NAME);
	m_clientMemDC.DeleteDC();
	m_bitmapMemDC.DeleteDC();
	m_networkPlayer.Stop();

	delete g_pGameController;
	delete g_pAI;
	delete m_pDlgHistory;
	delete m_pThread;
}

void CAmazonsView::OnInitialUpdate()
{
	if (!m_bThreadStart)
	{
		m_pThread = AfxBeginThread(Thread_AICompute, GetSafeHwnd(), THREAD_PRIORITY_ABOVE_NORMAL);
		m_bThreadStart = true;
	}
	if (!m_bSetTimer){
		SetTimer(1, TIMER_INTERVAL, NULL);
		m_bSetTimer = true;
	}
	
	g_pGameController->Init();

	CView::OnInitialUpdate();
}

void CAmazonsView::OnDraw(CDC* pDC)
{
	int plate[10][10];//读取plate数据进行展示，只读
	g_pGameController->GetPlate(plate);

	// TODO: add draw code for native data here
	double dRatioX, dRatioY;
	int n;

	CRect rectClient;

	//绘制棋盘
	CPoint ptCellTopLeft(0, 0);
	CBrush bgBrush(m_clrPlateBg);
	CBrush brushDark(m_clrDark);
	CBrush brushLigth(m_clrLight);
	CBrush brushArrow(m_clrArrow);
	int i, j;
	CString str;

	//绘制左边
	int nMargin = 15;
	CPoint ptTemp;
	CFont font, *pOldFont;
	font.CreatePointFont(100, _T("微软雅黑"));
	///////////
	//
	//刷背景
	GetClientRect(rectClient);
	m_clientMemDC.SelectObject(bgBrush);
	m_clientMemDC.FillSolidRect(rectClient, m_clrViewBg);
	//
	//确定棋盘的宽度，也就是小格子的宽度
	n = (m_ptTopLeft2.x - m_ptTopLeft1.x)*2;
	dRatioX = ((double)rectClient.Width() - 2 * m_ptTopLeft1.x - m_nRightMinWidth - n) / 400;
	dRatioY = ((double)rectClient.Height() - 2 * m_ptTopLeft1.y - n) / 400;
	if (dRatioX > dRatioY)	dRatioX = dRatioY;
	m_nCellWidth = (int)(40 * dRatioX);//不可由外往内，会造成右边和下面的坐标宽度与左边和上边的不一致
	if (m_nCellWidth < 10) m_nCellWidth = 10;
	//
	//左边整个棋盘区域，即格子和外边一圈数字一起的矩形区域
	//右边文字提示区域
	m_rectLeft = CRect(m_ptTopLeft1, CSize(m_nCellWidth * 10 + n, m_nCellWidth * 10 + n));
	m_rectRight = CRect(CPoint(m_ptTopLeft1.x + m_rectLeft.Width(), m_ptTopLeft1.y),
		CSize(rectClient.Width() - m_ptTopLeft1.x * 2 - m_rectLeft.Width(), m_rectLeft.Width()));
	m_clientMemDC.SelectObject(&bgBrush);
	m_clientMemDC.Rectangle(m_rectLeft);
	//
	//左边右边区域的包围框，调试用
	/*m_clientMemDC.Rectangle(CRect(m_ptTopLeft2, CSize(m_nCellWidth * 10, m_nCellWidth * 10)));
	m_clientMemDC.SelectStockObject(NULL_BRUSH);
	m_clientMemDC.Rectangle(m_rectLeft);
	m_clientMemDC.Rectangle(m_rectRight);*/
	//
	///画棋盘
	for (i = 0; i < 10; i++)//写数字
	{
		str.Format(_T("%d"), i);
		m_clientMemDC.TextOut(int(m_ptTopLeft2.x + (i + 0.5)*m_nCellWidth), m_ptTopLeft2.y - 21, str);//横上
		m_clientMemDC.TextOut(int(m_ptTopLeft2.x + (i + 0.5)*m_nCellWidth), m_ptTopLeft2.y + 7 + 10 * m_nCellWidth, str);//横下
		m_clientMemDC.TextOut(m_ptTopLeft2.x - 18, int(m_ptTopLeft2.y + (i + 0.3)*m_nCellWidth), str);//竖左
		m_clientMemDC.TextOut(m_ptTopLeft2.x + 10 + 10 * m_nCellWidth, int(m_ptTopLeft2.y + (i + 0.3)*m_nCellWidth), str);//竖右
	}
	//11条横线
	CPoint ptLineBegin = m_ptTopLeft2;
	CPoint ptLineEnd = m_ptTopLeft2 + CPoint(m_nCellWidth*10, 0);
	for (i = 0; i < 11; i++)
	{
		m_clientMemDC.MoveTo(ptLineBegin);
		m_clientMemDC.LineTo(ptLineEnd);
		ptLineBegin.y += m_nCellWidth;
		ptLineEnd.y += m_nCellWidth;
	}
	//11条竖线
	ptLineBegin = m_ptTopLeft2;
	ptLineEnd = m_ptTopLeft2 + CPoint(0, m_nCellWidth * 10);
	for (i = 0; i < 11; i++)
	{
		m_clientMemDC.MoveTo(ptLineBegin);
		m_clientMemDC.LineTo(ptLineEnd);
		ptLineBegin.x += m_nCellWidth;
		ptLineEnd.x += m_nCellWidth;
	}
	//
	BITMAP pieceBmpInfo, slectMarkBmpInfo;
	m_bmRedPiece.GetBitmap(&pieceBmpInfo);
	m_bmSelectMark.GetBitmap(&slectMarkBmpInfo);
	//
	//填充小方格
	int nCellValue = 0;
	for (i = 0; i < 10; i++)
	{
		for (j = 0; j < 10; j++)
		{
			nCellValue = plate[i][j];
			ptCellTopLeft = CPoint(j*m_nCellWidth + 1, i*m_nCellWidth + 1) + m_ptTopLeft2;

			//用背景填充格子
			if ((i + j) % 2 == 0)//浅色格子
				m_clientMemDC.SelectObject(&brushLigth);
			else//深色格子
				m_clientMemDC.SelectObject(&brushDark);
			m_clientMemDC.SelectStockObject(NULL_PEN);
			m_clientMemDC.Rectangle(CRect(ptCellTopLeft, CSize(m_nCellWidth, m_nCellWidth)));

			//格子图案
			if (g_pGameController->IsArrow(nCellValue))//放箭图案
			{
				m_clientMemDC.SelectObject(&brushArrow);
				m_clientMemDC.Rectangle(CRect(ptCellTopLeft, CSize(m_nCellWidth, m_nCellWidth)));
			}
			else if (g_pGameController->IsPlayer1(nCellValue))//棋子图案(玩家1用黑的)
			{
				m_bitmapMemDC.SelectObject(&m_bmBlackPiece);
				TransparentBlt(m_clientMemDC.m_hDC, ptCellTopLeft.x, ptCellTopLeft.y, m_nCellWidth, m_nCellWidth,
					m_bitmapMemDC.m_hDC, 0, 0, pieceBmpInfo.bmWidth, pieceBmpInfo.bmHeight, RGB(255, 255, 255));
				//TRACE("(%d,%d)\n",i,j);
			}
			else if (g_pGameController->IsPlayer2(nCellValue))//棋子图案(玩家2用红的)
			{
				m_bitmapMemDC.SelectObject(&m_bmRedPiece);
				TransparentBlt(m_clientMemDC.m_hDC, ptCellTopLeft.x, ptCellTopLeft.y, m_nCellWidth, m_nCellWidth,
					m_bitmapMemDC.m_hDC, 0, 0, pieceBmpInfo.bmWidth, pieceBmpInfo.bmHeight, RGB(255, 255, 255));
				//TRACE("(%d,%d)\n",i,j);
			}

			//显示选中标记
			bool showMark = false;
			if (m_nCurState == STATE_SELECTED)
				showMark = (i == m_posSelected.i && j == m_posSelected.j);
			else if (m_nCurState == STATE_MOVED)
				showMark = (m_posMoveTo.i == i && m_posMoveTo.j == j);
			if (showMark)
			{
				m_bitmapMemDC.SelectObject(&m_bmSelectMark);
				TransparentBlt(m_clientMemDC.m_hDC, ptCellTopLeft.x, ptCellTopLeft.y, m_nCellWidth, m_nCellWidth,
					m_bitmapMemDC.m_hDC, 0, 0, pieceBmpInfo.bmWidth, pieceBmpInfo.bmHeight, RGB(255, 255, 255));
			}
		}
	}
	if (m_bMarkMove)
	{
		MarkTheAction(m_posSelected, m_posMoveTo, false);
		MarkTheAction(m_posMoveTo, m_posShootAt, true);
	}

	//绘制右边
	//右边(应该设为固定高度，否则重绘时第四部分不能完全重绘,70+120+55+100=345)
	m_rectRight.bottom = m_rectRight.top + 345;
	pOldFont = m_clientMemDC.SelectObject(&font);
	m_clientMemDC.SelectStockObject(NULL_BRUSH);
	m_clientMemDC.SelectStockObject(BLACK_PEN);
	//m_clientMemDC.Rectangle(m_rectRight);
	////分四个部分
	ptTemp = m_rectRight.TopLeft();
	ptTemp.x = ptTemp.x + nMargin;
	ptTemp.y = ptTemp.y + nMargin;
	/////////////////////第一部分：基本信息
	m_clientMemDC.MoveTo(ptTemp.x, ptTemp.y); m_clientMemDC.LineTo(m_rectRight.right, ptTemp.y);
	m_clientMemDC.MoveTo(ptTemp.x, ptTemp.y - 1); m_clientMemDC.LineTo(m_rectRight.right, ptTemp.y - 1);
	//
	//【上】玩家1--黑/红方--电脑/人   先手
	//【下】玩家1--黑/红方--电脑/人 
	CString drawText = _T("【上】玩家1 - ");
	drawText += g_pGameController->m_bPlayer1Black ? _T("黑方 - ") : _T("红方 - ");
	drawText += GetPlayerName(g_pGameController->m_nPlayer1Type);
	m_clientMemDC.TextOut(ptTemp.x + nMargin, ptTemp.y + 15, drawText);
	//
	drawText = _T("【下】玩家2 - ");
	drawText += g_pGameController->m_bPlayer1Black ? _T("红方 - ") : _T("黑方 - ");
	drawText += GetPlayerName(g_pGameController->m_nPlayer2Type);
	m_clientMemDC.TextOut(ptTemp.x + nMargin, ptTemp.y + 35, drawText);
	//
	drawText = g_pGameController->m_bPlayer1First ? GetPlayer1Color() : GetPlayer2Color();
	m_clientMemDC.TextOut(ptTemp.x + nMargin + 200, ptTemp.y + 24 , drawText + _T("先"));

	/////////////////////第二部分：计时
	ptTemp.y = ptTemp.y + 70;
	m_clientMemDC.MoveTo(ptTemp.x, ptTemp.y); m_clientMemDC.LineTo(m_rectRight.right, ptTemp.y);
	m_clientMemDC.MoveTo(ptTemp.x, ptTemp.y - 1); m_clientMemDC.LineTo(m_rectRight.right, ptTemp.y - 1);
	//绘制表格
	m_clientMemDC.MoveTo(ptTemp.x + nMargin, ptTemp.y + 20); m_clientMemDC.LineTo(ptTemp.x + nMargin + 240, ptTemp.y + 20);//横0
	m_clientMemDC.MoveTo(ptTemp.x + nMargin, ptTemp.y + 45); m_clientMemDC.LineTo(ptTemp.x + nMargin + 240, ptTemp.y + 45);//横1
	m_clientMemDC.MoveTo(ptTemp.x + nMargin, ptTemp.y + 70); m_clientMemDC.LineTo(ptTemp.x + nMargin + 240, ptTemp.y + 70);//横2
	m_clientMemDC.MoveTo(ptTemp.x + nMargin, ptTemp.y + 95); m_clientMemDC.LineTo(ptTemp.x + nMargin + 240, ptTemp.y + 95);//横3
	m_clientMemDC.MoveTo(ptTemp.x + nMargin, ptTemp.y + 20);    m_clientMemDC.LineTo(ptTemp.x + nMargin, ptTemp.y + 95);	//列0
	m_clientMemDC.MoveTo(ptTemp.x + nMargin + 70, ptTemp.y + 20); m_clientMemDC.LineTo(ptTemp.x + nMargin + 70, ptTemp.y + 95);	//列1
	m_clientMemDC.MoveTo(ptTemp.x + nMargin + 155, ptTemp.y + 20); m_clientMemDC.LineTo(ptTemp.x + nMargin + 155, ptTemp.y + 95);//列2
	m_clientMemDC.MoveTo(ptTemp.x + nMargin + 239, ptTemp.y + 20); m_clientMemDC.LineTo(ptTemp.x + nMargin + 239, ptTemp.y + 95);//列3(退1px系统误差)
	//m_clientMemDC.TextOut(ptTemp.x + nMargin + 10, ptTemp.y + 22, m_strTotalTimer);	//列1横1
	m_clientMemDC.TextOut(ptTemp.x + nMargin + 15, ptTemp.y + 48, "已用时");			//列2横2
	m_clientMemDC.TextOut(ptTemp.x + nMargin + 10, ptTemp.y + 73, "单步用时");		//列3横3
	m_clientMemDC.TextOut(ptTemp.x + nMargin + 30 + 70, ptTemp.y + 23, g_pGameController->m_bPlayer1Black ? _T("黑方") : _T("红方"));		//列2横1
	m_clientMemDC.TextOut(ptTemp.x + nMargin + 15 + 70, ptTemp.y + 48, m_strPlayer1TotalTime);									//列2横2
	m_clientMemDC.TextOut(ptTemp.x + nMargin + 15 + 70, ptTemp.y + 73, m_strPlayer1Timer);										//列2横3
	m_clientMemDC.TextOut(ptTemp.x + nMargin + 30 + 155, ptTemp.y + 23, g_pGameController->m_bPlayer1Black ? _T("红方") : _T("黑方"));	//列3横1
	m_clientMemDC.TextOut(ptTemp.x + nMargin + 15 + 155, ptTemp.y + 48, m_strPlayer2TotalTime);									//列3横2
	m_clientMemDC.TextOut(ptTemp.x + nMargin + 15 + 155, ptTemp.y + 73, m_strPlayer2Timer);										//列3横3
	//m_clientMemDC.Rectangle(m_rectRight2);
	/////////////////////第三部分：谁着子
	ptTemp.y = ptTemp.y + 120;
	m_clientMemDC.MoveTo(ptTemp.x, ptTemp.y); m_clientMemDC.LineTo(m_rectRight.right, ptTemp.y);
	m_clientMemDC.MoveTo(ptTemp.x, ptTemp.y - 1); m_clientMemDC.LineTo(m_rectRight.right, ptTemp.y - 1);
	m_clientMemDC.SetTextColor(RGB(255, 0, 128));
	m_clientMemDC.TextOut(ptTemp.x + nMargin, ptTemp.y + 15, m_strGameState);
	m_clientMemDC.SetTextColor(RGB(0, 0, 0));
	/////////////////////第四部分：状态显示
	ptTemp.y = ptTemp.y + 55;
	m_clientMemDC.MoveTo(ptTemp.x, ptTemp.y); m_clientMemDC.LineTo(m_rectRight.right, ptTemp.y);
	m_clientMemDC.MoveTo(ptTemp.x, ptTemp.y - 1); m_clientMemDC.LineTo(m_rectRight.right, ptTemp.y - 1);
	//
	m_clientMemDC.TextOut(ptTemp.x + nMargin, ptTemp.y + 15, m_strMoves);
	m_clientMemDC.TextOut(ptTemp.x + nMargin, ptTemp.y + 40, m_strDepth);
	m_clientMemDC.TextOut(ptTemp.x + nMargin, ptTemp.y + 65, m_strNodes);
	///////////////
	m_clientMemDC.SelectObject(pOldFont);
	m_clientMemDC.SelectStockObject(NULL_BRUSH);
	m_clientMemDC.SelectStockObject(BLACK_PEN);

	pDC->BitBlt(0, 0, rectClient.Width(), rectClient.Height(), &m_clientMemDC, 0, 0, SRCCOPY);

	//pDC->GetClipBox(rectClient);//借用下这个变量
	//if (rectClient != m_rectRight)
		//m_bThereIsMark = false;//发生重绘棋盘操作标记会消失

}

void CAmazonsView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_nGameState != GAME_STATE_PLAYING || m_bComputerThinking)
		return;

	if (GetCurPlayerType() == PLAYER_NETWORK)
		return;

	//TRACE("point(%d,%d)\n",point.x,point.y);
	CString str;
	CPoint pointUnit(0, 0);

	//点击是否在棋盘内
	point = point - m_ptTopLeft2;
	if (point.x < 0 || point.x > m_nCellWidth * 10 || point.y < 0 || pointUnit.y > m_nCellWidth * 10)
		return;

	//所点击的格子坐标
	int i = point.y / m_nCellWidth;
	int j = point.x / m_nCellWidth;
	int cellValue = g_pGameController->GetPlateCellValue(i, j);
	SPosition pos(i, j);
	//TRACE("Click:(%d,%d)\n", i, j);

	bool redraw = false;
	if (m_nCurState == STATE_UNSELECT)
	{
		if (IsCurPlayerPiece(cellValue))
		{
			//选择棋子
			m_posMoveTo.i = -1;
			m_posMoveTo.j = -1;
			m_posShootAt = m_posMoveTo;
			m_posSelected = pos;
			m_nCurState = STATE_SELECTED;
			redraw = true;
			//TRACE("Selected\n");
		}
	}
	else if (m_nCurState == STATE_SELECTED)
	{
		if (IsCurPlayerPiece(cellValue))
		{
			//选择其他棋子
			m_posSelected = pos;
			m_nCurState = STATE_SELECTED;
			redraw = true;
			//TRACE("SelectedLoop\n");
		}
		else if (g_pGameController->IsEmpty(cellValue))
		{
			//走子
			if (g_pGameController->CanMoveOrShoot(m_posSelected, pos))
			{
				m_posMoveTo = pos;
				g_pGameController->Move(m_posSelected, pos);
				m_nCurState = STATE_MOVED;
				redraw = true;
			}
		}
	}
	else if (m_nCurState == STATE_MOVED)
	{
		if (m_posMoveTo.i == i && m_posMoveTo.j == j)
		{
			//取消走子
			g_pGameController->CancelMove(m_posSelected, pos);
			m_posMoveTo.i = -1;
			m_posMoveTo.j = -1;
			m_nCurState = STATE_SELECTED;
			redraw = true;
		}
		else if (g_pGameController->IsEmpty(cellValue))
		{
			//放箭
			if (g_pGameController->CanMoveOrShoot(m_posMoveTo, pos))
			{
				m_posShootAt = pos;
				g_pGameController->Shoot(pos);
				m_nCurState = STATE_UNSELECT;
				redraw = true;
				HumanMoveFinished();
			}
		}
	}

	if (redraw)
		InvalidateRect(NULL);

	CView::OnLButtonDown(nFlags, point);
}

void CAmazonsView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	OnCmdGameStart();
	CView::OnRButtonDown(nFlags, point);
}

void CAmazonsView::OnTimer(UINT nIDEvent)
{
	UpdateComputerMove();
	UpdateRightInfo();
	InvalidateRect(m_rectRight);
	CView::OnTimer(nIDEvent);
}

#pragma endregion

#pragma region Command Message

void CAmazonsView::OnCmdFileOpen()
{
	if (m_bComputerThinking)
	{
		AfxMessageBox(_T("电脑计算中..."));
		return;
	}
	
	CFileDialog dlg(true, _T("amz"), _T("*.amz"), 6, _T("amz files (*.amz)||all(*.*)"));
	if (dlg.DoModal() == IDOK)
	{
		m_strFilePathName = dlg.GetPathName();
		g_pGameController->LoadFromFile(dlg.GetPathName());
		ResetVariables();
		UpdateHistoryDlg();
		IsGameOver();
		UpdateTimeCounter();
		InvalidateRect(NULL);

		UpdateAppTitle();

		//获取当前步骤，进行mark
		SHistoryRecord record = g_pGameController->GetCurHistoryMove();
		m_posSelected = record.posSelected;
		m_posMoveTo = record.posMoveTo;
		m_posShootAt = record.posShootAt;
	}
}

void CAmazonsView::OnCmdFileSave()
{
	if (m_strFilePathName.IsEmpty())
	{
		CFileDialog dlg(false, _T("amz"), _T("amz.amz"), 6, _T("amz files (*.amz)||all(*.*)"));
		if (dlg.DoModal() == IDOK)
		{
			m_strFilePathName = dlg.GetPathName();
		}
		else
			return;
	}

	g_pGameController->SaveToFile(m_strFilePathName);
	UpdateAppTitle();
}

void CAmazonsView::OnCmdFileSaveAs()
{
	CFileDialog dlg(false, _T("amz"), _T("amz.amz"), 6, _T("amz files (*.amz)||all(*.*)"));
	if (dlg.DoModal() == IDOK)
	{
		m_strFilePathName = dlg.GetPathName();
	}
	else
		return;

	g_pGameController->SaveToFile(m_strFilePathName);
	UpdateAppTitle();
}

void CAmazonsView::OnCmdNewGame()
{
	if (m_nGameState != GAME_STATE_NOT_START)
	{
		if (TipMessage(_T("真的要重新开始吗？"), MB_OKCANCEL) != IDOK)
			return;
	}

	g_pGameController->NewGame();
	ResetVariables();
	UpdateHistoryDlg();
	InvalidateRect(NULL);
}

void CAmazonsView::OnCmdNewGameUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_bComputerThinking);
}

void CAmazonsView::OnCmdGameStart()
{
	StartGame(TRUE);
}

void CAmazonsView::OnCmdGameStartUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_nGameState == GAME_STATE_NOT_START || m_nGameState == GAME_STATE_SUSPEND);
}

void CAmazonsView::OnCmdGameSuspend()
{
	m_nGameState = GAME_STATE_SUSPEND;
}

void CAmazonsView::OnCmdGamesuspendUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_nGameState == GAME_STATE_PLAYING && !m_bComputerThinking);
}

void CAmazonsView::OnCmdBackwardOne()
{
	if (g_pGameController->CanBackward(1))
	{
		g_pGameController->Backward(1);
		ForwardOrBackwardHistoryEnd();
	}
}

void CAmazonsView::OnCmdBackwardOneUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((!m_bComputerThinking) && g_pGameController->CanBackward(1));
}

void CAmazonsView::OnCmdBackwardTwo()
{
	if (g_pGameController->CanBackward(2))
	{
		g_pGameController->Backward(2);
		ForwardOrBackwardHistoryEnd();
	}
}

void CAmazonsView::OnCmdBackwardTwoUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((!m_bComputerThinking) && g_pGameController->CanBackward(2));
}

void CAmazonsView::OnCmdBackwardToStart()
{
	if (g_pGameController->CanBackward(1))
	{
		g_pGameController->Backward(-1);
		ForwardOrBackwardHistoryEnd();
	}
}

void CAmazonsView::OnCmdBackwardToStartUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((!m_bComputerThinking) && g_pGameController->CanBackward(1));
}

void CAmazonsView::OnCmdForwardOne()
{
	if (g_pGameController->CanForward(1))
	{
		g_pGameController->Forward(1);
		ForwardOrBackwardHistoryEnd();
	}
}

void CAmazonsView::OnCmdForwardOneUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((!m_bComputerThinking) && g_pGameController->CanForward(1));
}

void CAmazonsView::OnCmdForwardTwo()
{
	if (g_pGameController->CanForward(2))
	{
		g_pGameController->Forward(2);
		ForwardOrBackwardHistoryEnd();
	}
}

void CAmazonsView::OnCmdForwardTwoUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((!m_bComputerThinking) && g_pGameController->CanForward(2));
}

void CAmazonsView::OnCmdForwardToEnd()
{
	if (g_pGameController->CanForward(1))
	{
		g_pGameController->Forward(-1);
		ForwardOrBackwardHistoryEnd();
	}
}

void CAmazonsView::OnCmdForwardToEndUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((!m_bComputerThinking) && g_pGameController->CanForward(1));
}

void CAmazonsView::OnCmdSettings()
{
	m_dlgSettings.Init(g_pGameController->m_nPlayer1Type,
		g_pGameController->m_nPlayer2Type,
		g_pGameController->m_bPlayer1Black, 
		g_pGameController->m_bPlayer1First);

	if (m_dlgSettings.DoModal() == IDOK)
	{
		g_pGameController->m_nPlayer1Type = m_dlgSettings.m_nPlayer1Type;
		g_pGameController->m_nPlayer2Type = m_dlgSettings.m_nPlayer2Type;
		g_pGameController->m_bPlayer1Black = m_dlgSettings.m_nPlayer1IsBlack==0 ? true : false;
		g_pGameController->m_bPlayer1First = m_dlgSettings.m_nPlayer1IsFirst==0 ? true : false;
		OnCmdNewGame();
	}
}

void CAmazonsView::OnCmdSettingsUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_bComputerThinking);
	pCmdUI->SetCheck(m_dlgSettings.m_hWnd != NULL);
}


void CAmazonsView::OnCmdShowHistoryDlg()
{
	if (!m_bHistoryDlgOpened)
	{
		m_bHistoryDlgOpened = true;
		m_pDlgHistory->UpdateHistoryText(g_pGameController);
		m_pDlgHistory->ShowWindow(SW_SHOW);
		m_pDlgHistory->UpdateData(false);
	}
}

void CAmazonsView::OnCmdShowHistoryDlgUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bHistoryDlgOpened);
}

void CAmazonsView::OnCmdMark()
{
	m_bMarkMove = !m_bMarkMove;
	Invalidate(NULL);
}

void CAmazonsView::OnCmdMarkUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bMarkMove);
}

void CAmazonsView::OnCmdSuggest()
{
	Suggest();
}

void CAmazonsView::OnCmdSuggestUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_bComputerThinking && m_nGameState == GAME_STATE_PLAYING);
}


void CAmazonsView::OnCmdStopComputing()
{
	g_pAI->StopComputing();
	TipMessage(_T("无此功能"));
}

void CAmazonsView::OnCmdStopComputingUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bComputerThinking);
}

void CAmazonsView::OnCmdNetwork()
{
	m_networkPlayer.m_nplayerCamp = 0;
	if (g_pGameController->m_nPlayer1Type == PLAYER_NETWORK)
		m_networkPlayer.m_nplayerCamp = 1;
	if (g_pGameController->m_nPlayer2Type == PLAYER_NETWORK)
		m_networkPlayer.m_nplayerCamp = 2;

	CMainFrame *pMainFrame = (CMainFrame *)AfxGetApp()->GetMainWnd();
	m_dlgNetwork.m_pNetworkPlayer = &m_networkPlayer;
	m_dlgNetwork.m_nPort = pMainFrame->m_nPort;
	m_dlgNetwork.m_strPlayerName = pMainFrame->m_strNetWorkName;
	m_dlgNetwork.m_dwIp = GblRevertByByte(GblGetIPLong(pMainFrame->m_strIp));

	if (m_dlgNetwork.DoModal() == IDOK)
	{
		pMainFrame->m_nPort = m_dlgNetwork.m_nPort;
		pMainFrame->m_strIp = GblGetIPString(GblRevertByByte(m_dlgNetwork.m_dwIp));
		pMainFrame->m_strNetWorkName = m_dlgNetwork.m_strPlayerName;
	}
}

void CAmazonsView::OnCmdNetworkUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_dlgNetwork.m_hWnd != NULL);
}

#pragma endregion

#pragma region Network Message

LRESULT CAmazonsView::OnNetworkConnectedToServer(WPARAM wParam, LPARAM lParam)
{
	if (m_dlgNetwork.m_hWnd != NULL)
		m_dlgNetwork.UpdateConnectStatus();
	return 0L;
}

LRESULT CAmazonsView::OnNetworkGetPlayerList(WPARAM wParam, LPARAM lParam)
{
	if (m_dlgNetwork.m_hWnd != NULL)
		m_dlgNetwork.UpdatePlayerList();
	return 0L;
}

LRESULT CAmazonsView::OnNetworkConnectedToOpponent(WPARAM wParam, LPARAM lParam)
{
	if (m_dlgNetwork.m_hWnd != NULL)
		m_dlgNetwork.UpdateOpponentInfo();
	return 0L;
}

LRESULT CAmazonsView::OnNetworkDisConnectToOpponent(WPARAM wParam, LPARAM lParam)
{
	if (m_dlgNetwork.m_hWnd != NULL)
		m_dlgNetwork.UpdateOpponentInfo();
	return 0L;
}

LRESULT CAmazonsView::OnNetworkLog(WPARAM wParam, LPARAM lParam)
{
	if (m_dlgNetwork.m_hWnd != NULL)
		m_dlgNetwork.UpdateLog();
	return 0L;
}

LRESULT CAmazonsView::OnNetworkMove(WPARAM wParam, LPARAM lParam)
{
	m_posSelected = m_posMoveTo = m_posShootAt = SPosition(-1, -1);
	m_networkPlayer.GetMove(g_posSelected, g_posMoveTo, g_posShootAt);
	m_bComputerMoving = true;
	m_nComputerMoveTimer = 0;
	return 0L;
}

LRESULT CAmazonsView::OnNetworkGameControl(WPARAM wParam, LPARAM lParam)
{
	if (wParam == NET_GAME_CONTROLL_START)
	{
		StartGame(FALSE);
	}
	else if (wParam == NET_GAME_CONTROLL_PAUSE)
	{
		OnCmdGameSuspend();
	}
	else if (wParam == NET_GAME_CONTROLL_CONTINUE)
	{
		AfxMessageBox(_T("待实现"));
	}
	else if (wParam == NET_GAME_CONTROLL_STOP)
	{
		AfxMessageBox(_T("待实现"));
	}
	else
	{
		AfxMessageBox(_T("错误的指令"));
	}
	return 0L;
}

#pragma endregion

#pragma region Other Message


LRESULT CAmazonsView::OnAIComputingFinished(WPARAM wParam, LPARAM lParam)
{
	m_posSelected = m_posMoveTo = m_posShootAt = SPosition(-1, -1);
	m_bComputerMoving = true;
	m_nComputerMoveTimer = 0;
	return 0L;
}

#pragma endregion

#pragma region deal things

void CAmazonsView::StartGame(bool sendMsgToNetPlayer)
{
	if (m_nGameState == GAME_STATE_PLAYING) return;

	if (g_pGameController->m_nPlayer1Type == PLAYER_NETWORK || g_pGameController->m_nPlayer2Type == PLAYER_NETWORK)
	{
		if (!m_networkPlayer.HasOpponent())
		{
			OnCmdNetwork();
			return;
		}

		if (sendMsgToNetPlayer)
			m_networkPlayer.SendGameControlMsg(NET_GAME_CONTROLL_START);
	}

	OpponentTurn(true);
	m_nGameState = GAME_STATE_PLAYING;
}

void CAmazonsView::OpponentTurn(bool isStartGame /*= false*/)
{
	if (IsGameOver())
	{
		AfxMessageBox(m_strGameState, MB_OK);
		return;
	}

	if (GetCurPlayerType() == PLAYER_COMPUTER)
		StartAIComputing();

	m_nPlayerTimer = 0;
}

//荐子是电脑帮玩家想，所以m_nPlayer1Type是PLAYER_HUMAN
void CAmazonsView::Suggest()
{
	if (m_bComputerThinking || m_nGameState != GAME_STATE_PLAYING) return;

	if (GetCurPlayerType() == PLAYER_HUMAN)
		StartAIComputing();

	m_nPlayerTimer = 0;
}

//bPlayer1 = true电脑扮演玩家1，否则扮演玩家2
void CAmazonsView::StartAIComputing()
{
	g_eventContinue.SetEvent();//给个信号
	m_bComputerThinking = true;
}

void CAmazonsView::Select(SPosition posSelected)
{
	m_posSelected = posSelected;
	InvalidateRect(m_rectLeft);
}

void CAmazonsView::Move(SPosition posSelected, SPosition posMoveTo)
{
	m_posSelected = posSelected;
	m_posMoveTo = posMoveTo;
	g_pGameController->Move(posSelected, posMoveTo);
	InvalidateRect(m_rectLeft);
}

void CAmazonsView::Shoot(SPosition posShootAt)
{
	m_posShootAt = posShootAt;
	g_pGameController->Shoot(posShootAt);
	InvalidateRect(m_rectLeft);
}

void CAmazonsView::MarkTheAction(SPosition posActBgn, SPosition posActEnd, bool bShoot)
{
	if (posActBgn.i == -1 || posActBgn.j == -1) return;
	if (posActEnd.i == -1 || posActEnd.j == -1) return;

	int i, j;
	CPoint ptBgn, ptEnd;
	CPen penMove(PS_SOLID, 2, RGB(0, 255, 0));
	CPen penShoot(PS_SOLID, 2, RGB(0, 20, 255));
	CBrush brushMove(RGB(0, 255, 0));

	if (bShoot){
		m_clientMemDC.SelectObject(&penShoot);
	}
	else{
		m_clientMemDC.SelectObject(&penMove);
		m_clientMemDC.SelectObject(&brushMove);
	}

	i = posActBgn.i; j = posActBgn.j;
	ptBgn = CPoint(int((j + 0.5)*m_nCellWidth) + 1, int((i + 0.5)*m_nCellWidth) + 1) + m_ptTopLeft2;
	i = posActEnd.i; j = posActEnd.j;
	ptEnd = CPoint(int((j + 0.5)*m_nCellWidth) + 1, int((i + 0.5)*m_nCellWidth) + 1) + m_ptTopLeft2;
	m_clientMemDC.MoveTo(ptBgn); m_clientMemDC.LineTo(ptEnd);
	if (!bShoot)
		m_clientMemDC.Ellipse(ptBgn.x - 5, ptBgn.y - 5, ptBgn.x + 5, ptBgn.y + 5);
	//计算箭头(以下均指箭头方向)(箭头一律先左后右)
	ptBgn = ptEnd;
	if (j == posActBgn.j&&i < posActBgn.i){//垂直向上
		ptEnd = CPoint(ptBgn.x - 4, ptBgn.y + 8); m_clientMemDC.MoveTo(ptBgn); m_clientMemDC.LineTo(ptEnd);
		ptEnd = CPoint(ptBgn.x + 4, ptBgn.y + 8); m_clientMemDC.MoveTo(ptBgn); m_clientMemDC.LineTo(ptEnd);
	}
	if (j == posActBgn.j&&i > posActBgn.i){//垂直向下
		ptEnd = CPoint(ptBgn.x - 4, ptBgn.y - 8); m_clientMemDC.MoveTo(ptBgn); m_clientMemDC.LineTo(ptEnd);
		ptEnd = CPoint(ptBgn.x + 4, ptBgn.y - 8); m_clientMemDC.MoveTo(ptBgn); m_clientMemDC.LineTo(ptEnd);
	}
	if (i == posActBgn.i&&j < posActBgn.j){//水平向左
		ptEnd = CPoint(ptBgn.x + 8, ptBgn.y - 4); m_clientMemDC.MoveTo(ptBgn); m_clientMemDC.LineTo(ptEnd);
		ptEnd = CPoint(ptBgn.x + 8, ptBgn.y + 4); m_clientMemDC.MoveTo(ptBgn); m_clientMemDC.LineTo(ptEnd);
	}
	if (i == posActBgn.i&&j > posActBgn.j){//水平向右
		ptEnd = CPoint(ptBgn.x - 8, ptBgn.y - 4); m_clientMemDC.MoveTo(ptBgn); m_clientMemDC.LineTo(ptEnd);
		ptEnd = CPoint(ptBgn.x - 8, ptBgn.y + 4); m_clientMemDC.MoveTo(ptBgn); m_clientMemDC.LineTo(ptEnd);
	}
	if (i<posActBgn.i&&j < posActBgn.j){//左斜向上
		ptEnd = CPoint(ptBgn.x + 2, ptBgn.y + 8); m_clientMemDC.MoveTo(ptBgn); m_clientMemDC.LineTo(ptEnd);
		ptEnd = CPoint(ptBgn.x + 8, ptBgn.y + 2); m_clientMemDC.MoveTo(ptBgn); m_clientMemDC.LineTo(ptEnd);
	}
	if (i > posActBgn.i&&j>posActBgn.j){//左斜向下
		ptEnd = CPoint(ptBgn.x - 8, ptBgn.y - 2); m_clientMemDC.MoveTo(ptBgn); m_clientMemDC.LineTo(ptEnd);
		ptEnd = CPoint(ptBgn.x - 2, ptBgn.y - 8); m_clientMemDC.MoveTo(ptBgn); m_clientMemDC.LineTo(ptEnd);
	}
	if (i<posActBgn.i&&j>posActBgn.j){//右斜向上
		ptEnd = CPoint(ptBgn.x - 8, ptBgn.y + 2); m_clientMemDC.MoveTo(ptBgn); m_clientMemDC.LineTo(ptEnd);
		ptEnd = CPoint(ptBgn.x - 2, ptBgn.y + 8); m_clientMemDC.MoveTo(ptBgn); m_clientMemDC.LineTo(ptEnd);
	}
	if (i > posActBgn.i&&j < posActBgn.j){//右斜向下
		ptEnd = CPoint(ptBgn.x + 2, ptBgn.y - 8); m_clientMemDC.MoveTo(ptBgn); m_clientMemDC.LineTo(ptEnd);
		ptEnd = CPoint(ptBgn.x + 8, ptBgn.y - 2); m_clientMemDC.MoveTo(ptBgn); m_clientMemDC.LineTo(ptEnd);
	}

	m_clientMemDC.SelectStockObject(BLACK_PEN);
	m_clientMemDC.SelectStockObject(NULL_BRUSH);
}

void CAmazonsView::HumanMoveFinished()
{
	if (IsOpponentNetplayer())
		m_networkPlayer.SendMoveMsg(m_posSelected, m_posMoveTo, m_posShootAt);

	g_pGameController->AddToHistory(m_posSelected, m_posMoveTo, m_posShootAt, m_nPlayerTimer);
	g_pGameController->SaveTempFile(TEMP_FILE_NAME);
	UpdateHistoryDlg();
	OpponentTurn();
}

void CAmazonsView::ComputerMoveFinished()
{
	if (IsOpponentNetplayer())
		m_networkPlayer.SendMoveMsg(m_posSelected, m_posMoveTo, m_posShootAt);

	g_pGameController->AddToHistory(m_posSelected, m_posMoveTo, m_posShootAt, m_nPlayerTimer);
	g_pGameController->SaveTempFile(TEMP_FILE_NAME);
	UpdateHistoryDlg();
	OpponentTurn();
}

bool CAmazonsView::IsGameOver()
{
	int nWhoWin = g_pGameController->IsGameOver();

	if (nWhoWin == 1)
	{
		m_strGameState = g_pGameController->m_bPlayer1Black ? _T("黑方赢") : _T("红方赢");
		m_nGameState = GAME_STATE_OVER;
	}
	else if (nWhoWin == 2)
	{
		m_strGameState = g_pGameController->m_bPlayer1Black ? _T("红方赢") : _T("黑方赢");
		m_nGameState = GAME_STATE_OVER;
	}
	else if (nWhoWin == 3)
	{
		m_strGameState = _T("平局");
		m_nGameState = GAME_STATE_OVER;
	}

	return nWhoWin >= 1;
}

bool CAmazonsView::IsOpponentNetplayer()
{
	if (g_pGameController->IsPlayer1Turn())
		return g_pGameController->m_nPlayer2Type == PLAYER_NETWORK;
	else
		return g_pGameController->m_nPlayer1Type == PLAYER_NETWORK;
}

bool CAmazonsView::IsCurPlayerPiece(int piece)
{
	//是否是玩家对应的棋子
	if (g_pGameController->IsPlayer1Turn())
		return g_pGameController->IsPlayer1(piece);
	else
		return g_pGameController->IsPlayer2(piece);
}

void CAmazonsView::UpdateHistoryDlg()
{
	if (!m_bHistoryDlgOpened) return;

	m_pDlgHistory->UpdateHistoryText(g_pGameController);
	m_pDlgHistory->UpdateData(FALSE);
	m_pDlgHistory->m_editHistory.SendMessage(WM_VSCROLL, SB_BOTTOM, 0);
}

void CAmazonsView::UpdateAppTitle()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	pMainFrame->m_strDocName = GblGetFileNameWithExFromPathName(m_strFilePathName);
	pMainFrame->UpdateApplicationTitle();
}

void CAmazonsView::HistoryDlgClosed()
{
	//现在用close了，可能窗口直接销毁了
	m_pDlgHistory->ShowWindow(SW_HIDE);
	m_bHistoryDlgOpened = false;
}

void CAmazonsView::UpdateComputerMove()
{
	//电脑走子流程
	if (!m_bComputerMoving) return;

	m_nComputerMoveTimer += TIMER_INTERVAL;
	if (m_nCurState == STATE_UNSELECT)
	{
		Select(g_posSelected);
		m_nCurState = STATE_SELECTED;
	}
	if (m_nCurState == STATE_SELECTED && m_nComputerMoveTimer > 300)
	{
		Move(g_posSelected, g_posMoveTo);
		m_nCurState = STATE_MOVED;
		m_nComputerMoveTimer = 0;
	}
	if (m_nCurState == STATE_MOVED && m_nComputerMoveTimer > 300)
	{
		Shoot(g_posShootAt);
		m_nCurState = STATE_SHOOTED;
		m_nComputerMoveTimer = 0;
	}
	if (m_nCurState == STATE_SHOOTED && m_nComputerMoveTimer > 200)
	{
		m_nCurState = STATE_UNSELECT;
		m_bComputerMoving = false;
		m_bComputerThinking = false;
		ComputerMoveFinished();
	}
}

void CAmazonsView::UpdateRightInfo()
{
	if (m_nGameState == GAME_STATE_NOT_START)
	{
		m_strGameState = _T("请点击工具栏【开始按钮】或【鼠标右键】开始游戏");
	}
	else if (m_nGameState == GAME_STATE_PLAYING)
	{
		int playerColor = 0;//1为黑，2为红
		int playerType = 0;

		if (g_pGameController->IsPlayer1Turn())
		{
			playerColor = g_pGameController->m_bPlayer1Black ? 1 : 2;
			playerType = g_pGameController->m_nPlayer1Type;
		}
		else
		{
			playerColor = g_pGameController->m_bPlayer1Black ? 2 : 1;
			playerType = g_pGameController->m_nPlayer2Type;
		}

		m_strGameState = playerColor == 1 ? _T("黑方") : _T("红方");
		if      (playerType == PLAYER_HUMAN)    m_strGameState += _T("  人类着子");
		else if (playerType == PLAYER_COMPUTER) m_strGameState += _T("  电脑着子");
		else if (playerType == PLAYER_NETWORK)  m_strGameState += _T("  网络玩家着子");

		if (m_bComputerThinking)
		{
			m_strDepth.Format(_T("Depth: %d"), g_pAI->m_nOutDepth);
			m_strNodes.Format(_T("Nodes: %d %d"), g_pAI->m_nOutNumOfSkippedNodes, g_pAI->m_nOutNumOfNodes);
			m_strMoves.Format(_T("Moves: %d,%d"), g_pAI->m_nOutNumOfMoves, g_pAI->m_nOutMaxValue);
		}

		UpdateTimeCounter();

	}
	else if (m_nGameState == GAME_STATE_SUSPEND)
	{
		m_strGameState = _T("暂停中,请点击工具栏【开始按钮】或【鼠标右键】继续");
	}
	else if (m_nGameState == GAME_STATE_OVER)
	{
		//IsGameOver设置了状态，这里什么也不用做
	}
}

void CAmazonsView::UpdateTimeCounter()
{
	m_nTotalTimer += TIMER_INTERVAL;
	m_nPlayerTimer += TIMER_INTERVAL;

	if (g_pGameController->IsPlayer1Turn())
		GetTimeString(m_nPlayerTimer, m_strPlayer1Timer);
	else
		GetTimeString(m_nPlayerTimer, m_strPlayer2Timer);

	GetTimeString(g_pGameController->GetTotalTime(false), m_strPlayer2TotalTime);
	GetTimeString(g_pGameController->GetTotalTime(true), m_strPlayer1TotalTime);
	GetTimeString(m_nTotalTimer, m_strTotalTimer);
}

void CAmazonsView::ForwardOrBackwardHistoryEnd()
{
	m_nCurState = STATE_UNSELECT;
	m_nGameState = GAME_STATE_SUSPEND;

	//获取当前步骤，进行mark
	SHistoryRecord record = g_pGameController->GetCurHistoryMove();
	m_posSelected = record.posSelected;
	m_posMoveTo = record.posMoveTo;
	m_posShootAt = record.posShootAt;

	IsGameOver();
	UpdateHistoryDlg();
	InvalidateRect(m_rectLeft);
}

TCHAR * CAmazonsView::GetPlayerName(int playerType)
{
	TCHAR *playerNames[3] = {
		_T("人"),
		_T("电脑"),
		_T("网络"),
	};
	return playerNames[playerType];
}

TCHAR * CAmazonsView::GetPlayer1Color()
{
	return g_pGameController->m_bPlayer1Black ? _T("黑方") : _T("红方");
}

TCHAR * CAmazonsView::GetPlayer2Color()
{
	return g_pGameController->m_bPlayer1Black ? _T("红方") : _T("黑方");
}

int CAmazonsView::GetCurPlayerType()
{
	if (g_pGameController->IsPlayer1Turn())
		return g_pGameController->m_nPlayer1Type;
	else
		return g_pGameController->m_nPlayer2Type;
}

void CAmazonsView::GetTimeString(int nTime, CString& strTime)
{
	nTime /= 1000;
	int nHour, nMinute, nSecond;
	CString str;
	nHour = nTime / 3600;
	nSecond = nTime % 60;
	nMinute = (nTime - 3600 * nHour - nSecond) / 60;
	strTime.Empty();
	if (nHour < 10)   str.Format(_T("0%d:"), nHour); else str.Format(_T("%d:"), nHour);     strTime += str;
	if (nMinute < 10) str.Format(_T("0%d:"), nMinute); else str.Format(_T("%d:"), nMinute); strTime += str;
	if (nSecond < 10) str.Format(_T("0%d"), nSecond); else str.Format(_T("%d"), nSecond); strTime += str;
}

#pragma endregion







