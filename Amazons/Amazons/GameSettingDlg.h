#include "afxwin.h"
#if !defined(AFX_GAMESETTINGS_H__BCC71853_3C98_43B9_B042_0ACEF13C7D25__INCLUDED_)
#define AFX_GAMESETTINGS_H__BCC71853_3C98_43B9_B042_0ACEF13C7D25__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GameSettingDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGameSettings dialog

class CGameSettingDlg : public CDialog
{
// Construction
public:
	CGameSettingDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGameSettings)
	enum { IDD = IDD_SETTINGS };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGameSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGameSettings)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	int m_nPlayer1Type;
	int m_nPlayer1IsBlack;
	int m_nPlayer1IsFirst;

	int m_nPlayer2Type;
	int m_nPlayer2IsBlack;//和上面的互斥
	int m_nPlayer2IsFirst;//和上面的互斥
	
	afx_msg void OnBnClickedRadioP1Color();
	afx_msg void OnBnClickedRadioP2Color();
	afx_msg void OnBnClickedRadioP1Turn();
	afx_msg void OnBnClickedRadioP2Turn();

public:
	void Init(int nPlayer1Type, int nPlayer2Type, bool bPlayer1IsBlack, bool bPlayer1IsFirst);
	CComboBox m_choosePlayerType1;
	CComboBox m_choosePlayerType2;
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAMESETTINGS_H__BCC71853_3C98_43B9_B042_0ACEF13C7D25__INCLUDED_)
