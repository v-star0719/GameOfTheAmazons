// AmazonsDoc.cpp : implementation of the CAmazonsDoc class
//

#include "stdafx.h"
#include "AmazonsApp.h"
#include "MainFrm.h"
#include "AmazonsDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAmazonsDoc

IMPLEMENT_DYNCREATE(CAmazonsDoc, CDocument)

BEGIN_MESSAGE_MAP(CAmazonsDoc, CDocument)
	//{{AFX_MSG_MAP(CAmazonsDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAmazonsDoc construction/destruction

CAmazonsDoc::CAmazonsDoc()
{
	// TODO: add one-time construction code here

}

CAmazonsDoc::~CAmazonsDoc()
{
}

BOOL CAmazonsDoc::OnNewDocument()
{                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CAmazonsDoc serialization

void CAmazonsDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAmazonsDoc diagnostics

#ifdef _DEBUG
void CAmazonsDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAmazonsDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAmazonsDoc commands
