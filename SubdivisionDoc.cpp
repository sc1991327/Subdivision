// SubdivisionDoc.cpp : implementation of the CSubdivisionDoc class
//

#include "stdafx.h"
#include "Subdivision.h"

#include "SubdivisionDoc.h"
#include ".\subdivisiondoc.h"

#include "MeshSubdivision.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSubdivisionDoc

IMPLEMENT_DYNCREATE(CSubdivisionDoc, CDocument)

BEGIN_MESSAGE_MAP(CSubdivisionDoc, CDocument)
	ON_COMMAND(ID_IMPORT, OnImport)
	ON_COMMAND(ID_EXPORT, OnExport)
	ON_COMMAND(ID_SUBDIVISION_DOO, OnSubdivisionDoo)
	ON_COMMAND(ID_SUBDIVISION_CATMULL, OnSubdivisionCatmull)
	ON_UPDATE_COMMAND_UI(ID_SUBDIVISION_DOO, OnUpdateSubdivisionDoo)
	ON_UPDATE_COMMAND_UI(ID_SUBDIVISION_CATMULL, OnUpdateSubdivisionCatmull)
END_MESSAGE_MAP()


// CSubdivisionDoc construction/destruction

CSubdivisionDoc::CSubdivisionDoc()
{
	// TODO: add one-time construction code here
	m_pmesh = new Mesh3D;
}

CSubdivisionDoc::~CSubdivisionDoc()
{
	if (m_pmesh) {
		delete m_pmesh;
		m_pmesh = NULL;
	}
}

//BOOL CSubdivisionDoc::OnNewDocument()
//{
//
//	if (!CDocument::OnNewDocument())
//		return FALSE;
//
//	// TODO: add reinitialization code here
//	// (SDI documents will reuse this document)
//
//	return TRUE;
//}




// CSubdivisionDoc serialization

void CSubdivisionDoc::Serialize(CArchive& ar)
{
	//if (ar.IsStoring())
	//{
	//	// TODO: add storing code here
	//}
	//else
	//{
	//	// TODO: add loading code here
	//}
}


// CSubdivisionDoc diagnostics

#ifdef _DEBUG
void CSubdivisionDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSubdivisionDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CSubdivisionDoc commands

BOOL CSubdivisionDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	// get extension
	CString file = lpszPathName;
	CString extension = lpszPathName;
	extension = extension.Right(extension.GetLength()-extension.ReverseFind('.'));

	extension.MakeLower();

	CString path = lpszPathName;
	path = path.Left(path.ReverseFind('\\'));
	SetCurrentDirectory(path);

	if(extension == ".off")
	{
		std::ifstream stream(lpszPathName);

		if (stream.fail()) {
			return FALSE;
		}
		
		if (m_pmesh==NULL) {
			m_pmesh = new Mesh3D;
		}
		
		bool suc = m_pmesh->load_data(stream);
		stream.close();
		return  suc == true;
	}

	return FALSE;
}


BOOL CSubdivisionDoc::OnSaveDocument(LPCTSTR lpszPathName)
{

	if (m_pmesh==NULL || (!m_pmesh->isvalid())) {
		return FALSE;
	}

	// Extension-based checking
	CString file = lpszPathName;

	// Extension
	CString extension = lpszPathName;
	extension = extension.Right(extension.GetLength()-extension.ReverseFind('.'));
	extension.MakeLower();

	// Path "c:\path\file.wrl" -> c:\path
	CString path = lpszPathName;
	path = path.Left(path.ReverseFind('\\'));

	// Current path
	SetCurrentDirectory(path);

	
	if(extension == ".off")
	{
		// OFF extension
		std::ofstream stream(lpszPathName);
		if (stream.fail()) {
			return FALSE;
		}
		m_pmesh->write_data(stream);
		stream.close();
		return TRUE;

	}

	return FALSE;
}


void CSubdivisionDoc::OnImport()
{
	// TODO: Add your command handler code here
	char lpszFilter[] = "Off files (*.off)|*.off||";
	CFileDialog dlg(true, NULL, NULL, OFN_READONLY, lpszFilter);
	dlg.m_ofn.lpstrTitle = "Import 3D model";		////
	dlg.m_ofn.lpstrDefExt = "off";		

	if(dlg.DoModal() == IDOK)
	{
		CString		strFile = dlg.GetPathName();
		std::ifstream fin(strFile);
		if (fin.fail()) {
			return;
		}
		if (m_pmesh==NULL) {
			m_pmesh = new Mesh3D;
		}
		m_pmesh->load_data(fin);
		fin.close();
	}
	UpdateAllViews(NULL);
}

void CSubdivisionDoc::OnExport()
{
	if(m_pmesh == NULL)
		return;
		
	// TODO: Add your command handler code here
	char lpszFilter[] = "Off files (*.off)|*.off||";

	CFileDialog dlg(false, NULL, NULL, OFN_OVERWRITEPROMPT, lpszFilter);
	dlg.m_ofn.lpstrTitle = "Save As OFF File";		////
	dlg.m_ofn.lpstrDefExt = "off";		////
	if(dlg.DoModal() == IDOK)
	{

		CString		strFile = dlg.GetPathName();
		std::ofstream	fp (strFile);
		if (fp.fail()) {
			return;
		}
		m_pmesh->write_data(fp);
		fp.close();

	}
}

void CSubdivisionDoc::OnSubdivisionDoo()
{
	// TODO: Add your command handler code here
	MeshSubdivision m_sub(m_pmesh);
	m_pmesh = m_sub.Doo_Sabin();
	UpdateAllViews(NULL);

}

void CSubdivisionDoc::OnSubdivisionCatmull()
{
	// TODO: Add your command handler code here
	MeshSubdivision m_sub(m_pmesh);
	m_pmesh = m_sub.Catmull_Clark();
	UpdateAllViews(NULL);
}

void CSubdivisionDoc::OnUpdateSubdivisionDoo(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	//pCmdUI->Enable(m_pmesh&&m_pmesh->isclosed()&&m_pmesh->isvalid());
	pCmdUI->Enable(m_pmesh&&m_pmesh->isvalid()&&m_pmesh->isclosed());
}

void CSubdivisionDoc::OnUpdateSubdivisionCatmull(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_pmesh&&m_pmesh->isvalid());
}

