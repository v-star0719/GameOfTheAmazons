#pragma once


// CTipDlg dialog

class CTipDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTipDlg)

public:
	CTipDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTipDlg();

// Dialog Data
	enum { IDD = IDD_TIP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strMsg;
	int m_nType;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk2();
	afx_msg void OnBnClickedBtnOk1();
	afx_msg void OnBnClickedBtnCancel1();
};
