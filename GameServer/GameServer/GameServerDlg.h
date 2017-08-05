
// GameServerDlg.h : header file
//

#pragma once

// CGameServerDlg dialog
class CGameServerDlg : public CDialogEx
{
// Construction
public:
	CGameServerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_GAMESERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	DWORD m_dwIp;
	int m_nPort;

public:
	void UpdateClientList();
	void UpdateLog();
	void UpdateConnectStatus();

private:
	static UINT GameServerThread(LPVOID pParam);
public:
	afx_msg void OnClose();
	
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnStop();
};
