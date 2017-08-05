// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__4BB760C1_D0E3_438E_8871_C0078D875913__INCLUDED_)
#define AFX_STDAFX_H__4BB760C1_D0E3_438E_8871_C0078D875913__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <afxmt.h>
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>            // MFC socket extensions
#include <afxcontrolbars.h>

#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"msimg32.lib")
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#define WM_AI_COMPUTING_FINISHED			WM_USER+101
#define WM_NETWORK_CONNECTED_TO_SERVER		WM_USER+104
#define WM_NETWORK_GET_PLAYER_LIST			WM_USER+105
#define WM_NETWORK_CONNECTED_TO_OPPONENT	WM_USER+106
#define WM_NETWORK_DISCONNECT_TO_OPPONENT	WM_USER+107
#define WM_NETWORK_MOVE						WM_USER+108
#define WM_NETWORK_LOG						WM_USER+109
#define WM_NETWORK_GAME_CONTROL				WM_USER+110

int TipMessage(CString text, int type = MB_OK);

#endif // !defined(AFX_STDAFX_H__4BB760C1_D0E3_438E_8871_C0078D875913__INCLUDED_)
