// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "AmazonsApp.h"

#include "MainFrm.h"
#include "GlobalFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define APPLICAION_INFO_FILE_NAME "application.txt"

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	CPoint ptTopLeft1(20,20);
	CPoint ptTopLeft2=ptTopLeft1+CPoint(30,30);
	int nRigthMinWidth=270;
	m_nWidth=20+400+2*ptTopLeft2.x+nRigthMinWidth;//客户区宽=cx-20
	m_nHeight=107+400+2*ptTopLeft2.x;//客户区高=cy-107
	m_nX=40;
	m_nY=40;
	m_nPort = 50001;
	m_strIp = _T("127.0.0.1");
	m_strNetWorkName = _T("NoName");
	m_strDocName = _T("new game");
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	//m_wndToolBar.GetToolBarCtrl().HideButton(ID_GAME_START, TRUE);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	UpdateApplicationTitle();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	LoadAppInfoFromFile();
	cs.x=m_nX;
	cs.y=m_nY;
	cs.cx=m_nWidth;
	cs.cy=m_nHeight;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	// TODO: Add your message handler code here and/or call default
	//lpMMI->ptMinTrackSize=CPoint(780,577);
	CFrameWnd::OnGetMinMaxInfo(lpMMI);
}
void CMainFrame::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	m_nX=wp.rcNormalPosition.left;
	m_nY=wp.rcNormalPosition.top;
	m_nWidth=wp.rcNormalPosition.right-wp.rcNormalPosition.left;
	m_nHeight=wp.rcNormalPosition.bottom-wp.rcNormalPosition.top;
	SaveAppInfoToFile();

	CFrameWnd::OnClose();
}

void CMainFrame::SaveAppInfoToFile()
{
	CStdioFile file;
	CString temp;
	if (!file.Open(GblGetDirectoryOfExecutableFile() + APPLICAION_INFO_FILE_NAME, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
	{
		return;
	}
	temp.Format(_T("[window_x]\n%d\n"), m_nX);			file.WriteString(temp);
	temp.Format(_T("[window_y]\n%d\n"), m_nY);			file.WriteString(temp);
	temp.Format(_T("[window_width]\n%d\n"), m_nWidth);	file.WriteString(temp);
	temp.Format(_T("[window_height]\n%d\n"), m_nHeight);	file.WriteString(temp);
	temp.Format(_T("[NetworkName]\n%s\n"), m_strNetWorkName);file.WriteString(temp);
	temp.Format(_T("[IpAdress]\n%s\n"), m_strIp);			file.WriteString(temp);
	temp.Format(_T("[port]\n%d\n"), m_nPort);				file.WriteString(temp);
	file.Close();
}


void CMainFrame::LoadAppInfoFromFile()
{
	CStdioFile file;
	if (!file.Open(GblGetDirectoryOfExecutableFile() + APPLICAION_INFO_FILE_NAME, CFile::modeRead | CFile::typeText))
	{
		return;
	}

	CString strFileContent;

	file.ReadString(strFileContent);file.ReadString(strFileContent);
	m_nX = GetIntegerFromBuffer(strFileContent, m_nX);
	//
	file.ReadString(strFileContent);file.ReadString(strFileContent);
	m_nY = GetIntegerFromBuffer(strFileContent, m_nY);
	//m
	file.ReadString(strFileContent); file.ReadString(strFileContent);
	m_nWidth = GetIntegerFromBuffer(strFileContent, m_nWidth);
	//
	file.ReadString(strFileContent); file.ReadString(strFileContent);
	m_nHeight = GetIntegerFromBuffer(strFileContent, m_nHeight);
	//
	file.ReadString(strFileContent); file.ReadString(strFileContent);
	if (!strFileContent.IsEmpty()) m_strNetWorkName = strFileContent;
	//
	file.ReadString(strFileContent); file.ReadString(strFileContent);
	m_strIp = strFileContent;
	//
	file.ReadString(strFileContent); file.ReadString(strFileContent);
	m_nPort = GetIntegerFromBuffer(strFileContent, m_nPort);

	file.Close();
}

void CMainFrame::UpdateApplicationTitle()
{
	CString title;
	title.Format(_T("%s [%s] - %s"), AfxGetAppName(), m_strNetWorkName, m_strDocName);
	SetWindowText(title);
	SetTitle(title);
}