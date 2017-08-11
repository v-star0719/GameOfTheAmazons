// AmazonsView.h : interface of the CAmazonsView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_AMAZONSVIEW_H__8655EBF0_EA24_43CB_A748_4DEA4F7F9150__INCLUDED_)
#define AFX_AMAZONSVIEW_H__8655EBF0_EA24_43CB_A748_4DEA4F7F9150__INCLUDED_

#include "GameSettingDlg.h"
#include "HistoryDlg.h"
#include "AmazonsDoc.h"
#include "NetworkPlayer.h"
#include "NetworkDlg.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CAmazonsView : public CView
{
protected: // create from serialization only
	CAmazonsView();
	~CAmazonsView();
	DECLARE_DYNCREATE(CAmazonsView)

// Attributes
public:
	CAmazonsDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAmazonsView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void OnInitialUpdate();
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	//绘图
	CDC m_clientMemDC;
	CDC m_bitmapMemDC;
	CBitmap m_bmClient;
	CBitmap m_bmBlackPiece;
	CBitmap m_bmRedPiece;
	CBitmap m_bmSelectMark;
	bool m_bHasInitDCAndBitmap;
	//
	CPoint m_ptTopLeft1;//左上角1，标注绘图起始位置
	CPoint m_ptTopLeft2;//左上角2，标注棋盘位置
	CRect m_rectLeft;//
	CRect m_rectRight;
	//
	int m_nCellWidth;//小方格的尺寸
	int m_nRightMinWidth;//右边区域的最小宽度
	COLORREF m_clrPlateBg;
	COLORREF m_clrViewBg;
	COLORREF m_clrDark;
	COLORREF m_clrLight;
	COLORREF m_clrArrow;

	//游戏流程控制
	int m_nGameState;
	//走子控制与实现
	bool m_bMarkMove;
	bool m_bComputerThinking;//电脑是否在计算，从开始计算，一直到走子完成才算思考结束
	bool m_bComputerMoving;
	int m_nComputerMoveTimer;
	int m_nCurState;//0未选择棋子，2已选择棋子，3选择棋子并走了一步，等待放箭
	SPosition m_posSelected;//选子，如果没有，值为(-1,-1)
	SPosition m_posMoveTo;//走子位置，如果没有，值为(-1,-1)
	SPosition m_posShootAt;//放箭位置，如果没有，值为(-1,-1)
	//
	CNetworkPlayer m_networkPlayer;

	//状态显示
	CString m_strPlayer1TotalTime;
	CString m_strPlayer2TotalTime;
	CString m_strPlayer1Timer;
	CString m_strPlayer2Timer;
	CString m_strTotalTimer;
	CString m_strGameState;
	CString m_strDepth;
	CString m_strMoves;
	CString m_strNodes;
	int m_nPlayerTimer;
	int m_nTotalTimer;//调试用
	//程序其他部分的控制
	CGameSettingDlg m_dlgSettings;
	CHistoryDlg* m_pDlgHistory;
	CNetworkDlg m_dlgNetwork;
	bool m_bHistoryDlgOpened;//是否打开了历史步骤
	bool m_bSetTimer;
	bool m_bThreadStart;
	CWinThread *m_pThread;
	/////////////////////////

	CString m_strFilePathName;//存档文件路径名，只有在打开或保存文件后会变


private:
	void Select(SPosition posSelected);//选择棋子(只绘图)
	void Move(SPosition posSelected, SPosition posMoveTo);//走子(绘图并更新棋盘)
	void Shoot(SPosition posShootAt);//放箭(绘图并更新棋盘)
	void MarkTheAction(SPosition posActBgn,SPosition posActEnd, bool isShoot);//标志某一着法
	void HumanMoveFinished();
	void ComputerMoveFinished();//电脑走完

	void UpdateHistoryDlg();//更新历史步骤对话框
	void ForwardOrBackwardHistoryEnd();//当执行后退、前进操作时，借此更新棋盘
	void ResetVariables();//初始化一些变量以开始新一轮游戏
	void GetTimeString(int nTime,CString& strTime);//用nTime给定的秒数计算时间，由strTime返回
	void UpdateTimeCounter();
	void UpdateComputerMove();//电脑着子和放箭都有一个时间间隔
	void UpdateRightInfo();
	void UpdateAppTitle();
	TCHAR * GetPlayerName(int playerType);
	TCHAR * GetPlayer1Color();
	TCHAR * GetPlayer2Color();
	int GetCurPlayerType();

	void StartAIComputing();//电脑走步
	void Suggest();
	void OpponentTurn(bool isStartGame = false);//对方走之，在下棋的两方之间切换
	bool IsOpponentNetplayer();
	bool IsCurPlayerPiece(int piece);
	bool IsGameOver();//游戏是否结束,结束的话将设置m_nGameState为OVER

	static UINT Thread_AICompute(LPVOID pParam);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	void StartGame(bool sendMsgToNetPlayer);

// Generated message map functions
protected:
	//{{AFX_MSG(CAmazonsView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnCmdFileOpen();
	afx_msg void OnCmdFileSave();
	afx_msg void OnCmdFileSaveAs();

	afx_msg void OnCmdNewGame();
	afx_msg void OnCmdNewGameUpdate(CCmdUI* pCmdUI);
	afx_msg void OnCmdGameStart();
	afx_msg void OnCmdGameStartUpdate(CCmdUI* pCmdUI);
	afx_msg void OnCmdGameSuspend();
	afx_msg void OnCmdGamesuspendUpdate(CCmdUI* pCmdUI);
	afx_msg void OnCmdBackwardOne();
	afx_msg void OnCmdBackwardOneUpdate(CCmdUI* pCmdUI);
	afx_msg void OnCmdBackwardTwo();
	afx_msg void OnCmdBackwardTwoUpdate(CCmdUI* pCmdUI);
	afx_msg void OnCmdBackwardToStart();
	afx_msg void OnCmdBackwardToStartUpdate(CCmdUI* pCmdUI);
	afx_msg void OnCmdForwardOne();
	afx_msg void OnCmdForwardOneUpdate(CCmdUI* pCmdUI);
	afx_msg void OnCmdForwardTwo();
	afx_msg void OnCmdForwardTwoUpdate(CCmdUI* pCmdUI);
	afx_msg void OnCmdForwardToEnd();
	afx_msg void OnCmdForwardToEndUpdate(CCmdUI* pCmdUI);
	afx_msg void OnCmdSettings();
	afx_msg void OnCmdSettingsUpdate(CCmdUI* pCmdUI);
	afx_msg void OnCmdShowHistoryDlg();
	afx_msg void OnCmdShowHistoryDlgUpdate(CCmdUI* pCmdUI);
	afx_msg void OnCmdMark();
	afx_msg void OnCmdMarkUpdate(CCmdUI* pCmdUI);
	afx_msg void OnCmdSuggest();
	afx_msg void OnCmdSuggestUpdate(CCmdUI* pCmdUI);
	afx_msg void OnCmdStopComputing();
	afx_msg void OnCmdStopComputingUpdate(CCmdUI* pCmdUI);
	afx_msg void OnCmdNetwork();
	afx_msg void OnCmdNetworkUpdate(CCmdUI* pCmdUI);

	//}}AFX_MSG
	afx_msg LRESULT OnAIComputingFinished(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnNetworkConnectedToServer(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNetworkGetPlayerList(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNetworkConnectedToOpponent(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNetworkDisConnectToOpponent(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNetworkMove(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNetworkLog(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNetworkGameControl(WPARAM wParam, LPARAM lParam);//wparam是要进行的操作，见SNetMsgCsGameControl
	DECLARE_MESSAGE_MAP()

public:
	void HistoryDlgClosed();
};

#ifndef _DEBUG  // debug version in AmazonsView.cpp
inline CAmazonsDoc* CAmazonsView::GetDocument()
   { return (CAmazonsDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AMAZONSVIEW_H__8655EBF0_EA24_43CB_A748_4DEA4F7F9150__INCLUDED_)
