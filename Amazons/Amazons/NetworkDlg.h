#pragma once
#include "afxcmn.h"
#include "NetworkPlayer.h"
#include "afxwin.h"

class CNetworkDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNetworkDlg)

public:
	CNetworkDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNetworkDlg();

// Dialog Data
	enum { IDD = IDD_NETWORK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CNetworkPlayer *m_pNetworkPlayer;
	CString m_strPlayerName;
	DWORD m_dwIp;
	int m_nPort;
	CString m_strID;
	int m_nSeletdPlayerIndex;

public:
	void UpdateConnectStatus();
	void RequirePlayerList();
	void UpdatePlayerList();
	void UpdateOpponentInfo();
	void UpdateLog();

	CListBox m_playerListCtrl;
	CString m_strChallengePlayer;
	CString m_strChoosedPlayer;
	CString m_strOpponentName;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedConnect();
	afx_msg void OnLbnSelchange();
	afx_msg void OnBnClickedBtnChallenge();
	afx_msg void OnBnClickedBtnCancelChallenge();
	afx_msg void OnBnClickedBtnDisconnect();
	afx_msg void OnClose();
};
