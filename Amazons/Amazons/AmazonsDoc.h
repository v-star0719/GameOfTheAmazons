// AmazonsDoc.h : interface of the CAmazonsDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_AMAZONSDOC_H__AAEB52C5_0DA6_4854_8735_F7B4C2D3AC3E__INCLUDED_)
#define AFX_AMAZONSDOC_H__AAEB52C5_0DA6_4854_8735_F7B4C2D3AC3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAmazonsDoc : public CDocument
{
protected: // create from serialization only
	CAmazonsDoc();
	DECLARE_DYNCREATE(CAmazonsDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAmazonsDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAmazonsDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAmazonsDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AMAZONSDOC_H__AAEB52C5_0DA6_4854_8735_F7B4C2D3AC3E__INCLUDED_)
