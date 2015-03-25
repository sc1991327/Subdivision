// SubdivisionDoc.h : interface of the CSubdivisionDoc class
//


#pragma once

#include "Mesh3D.h"

class CSubdivisionDoc : public CDocument
{
protected: // create from serialization only
	CSubdivisionDoc();
	DECLARE_DYNCREATE(CSubdivisionDoc)

// Attributes
public:
	Mesh3D* m_pmesh;
// Operations
public:

// Overrides
	public:
	//virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CSubdivisionDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	afx_msg void OnImport();
	afx_msg void OnExport();
	afx_msg void OnSubdivisionDoo();
	afx_msg void OnSubdivisionCatmull();
	afx_msg void OnUpdateSubdivisionDoo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSubdivisionCatmull(CCmdUI *pCmdUI);
};


