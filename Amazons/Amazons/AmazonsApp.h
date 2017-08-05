// Amazons.h : main header file for the AMAZONS application
//

#if !defined(AFX_AMAZONS_H__D5853F8F_FF5C_44FC_863B_97DDC3F8EACD__INCLUDED_)
#define AFX_AMAZONS_H__D5853F8F_FF5C_44FC_863B_97DDC3F8EACD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CAmazonsApp:
// See Amazons.cpp for the implementation of this class
//

class CAmazonsApp : public CWinApp
{
public:
	CAmazonsApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAmazonsApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CAmazonsApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AMAZONS_H__D5853F8F_FF5C_44FC_863B_97DDC3F8EACD__INCLUDED_)
