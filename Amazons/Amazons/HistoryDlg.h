#if !defined(AFX_HISTORYDLG_H__1E364040_8387_4CE9_83D1_64CD133B10CE__INCLUDED_)
#define AFX_HISTORYDLG_H__1E364040_8387_4CE9_83D1_64CD133B10CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HistoryDlg.h : header file
//

#include "AmazonsGameController.h"

/////////////////////////////////////////////////////////////////////////////
// CHistoryDlg dialog

class CHistoryDlg : public CDialog
{
// Construction
public:
	CHistoryDlg(CWnd* pParent = NULL);   // standard constructor
	CHistoryDlg(CView* pView);
int Create();
	CView* m_pView;
// Dialog Data
	//{{AFX_DATA(CHistoryDlg)
	enum { IDD = IDD_HISTORY };
	CEdit	m_editHistory;
	CString	m_strHistory;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHistoryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHistoryDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void UpdateHistoryText(CAmazonsGameController *pAmazons);
	afx_msg void OnClose();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HISTORYDLG_H__1E364040_8387_4CE9_83D1_64CD133B10CE__INCLUDED_)
