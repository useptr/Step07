// (C) Copyright 2002-2012 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- DocData.cpp : Implementation file
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "Tchar.h"
#include <memory>
#include "ADSKEmployeeReactor.h"
extern std::unique_ptr<ADSKEmployeeReactor> g_pEmployeeReactor;

Acad::ErrorStatus attachEmployeeReactorToAllEmployee(bool attach)
{
	// pBlockTable successfully opened
	// Get the model space record
	AcDbBlockTableRecordPointer pBlockTableRecord(acdbHostApplicationServices()->workingDatabase()->currentSpaceId());
	if (pBlockTableRecord.openStatus() != Acad::eOk) {
		acutPrintf(_T("\nERROR: Cannot open BlockTableRecord"));
		return pBlockTableRecord.openStatus();
	}
	// Get a block table record iterator
	AcDbBlockTableRecordIterator* pIt;
	Acad::ErrorStatus es = pBlockTableRecord->newIterator(pIt);
	if (es != Acad::eOk) {
		return es;
	}
	std::unique_ptr<AcDbBlockTableRecordIterator> pIteratorScopedDeleter(pIt);
	// Iterate through the Model Space records and retrieve the entities 
	for (; !pIt->done(); pIt->step()) {
		AcDbObjectId entityId{ AcDbObjectId::kNull};
		es = pIt->getEntityId(entityId);
		if (es != Acad::eOk)
			continue;
		AcDbEntityPointer pEnt(entityId);
		if (pEnt.openStatus() != Acad::eOk)
			continue;
		// pEnt successfully opened
		AcDbBlockReference* pBlockRef = AcDbBlockReference::cast(pEnt);
		if (nullptr == pBlockRef)
			continue;
		AcDbBlockTableRecordPointer pBlockTableRecord(pBlockRef->blockTableRecord());
		if (pBlockTableRecord.openStatus() != Acad::eOk) 
			continue;
		// If the retrieved block reference entity is not an EMPLOYEE object, just return
		AcString sBlockName;
		es = pBlockTableRecord->getName(sBlockName);
		if (es != Acad::eOk) 
			continue;
		if (_tcscmp(sBlockName, _T("EMPLOYEE")))
			continue;
		// If the retrieved entity is an "EMPLOYEE" block reference, attach the global object reactor or if the parameter "attach" is false then remove the already attached object reactor
		if (attach)
			pEnt->addReactor(g_pEmployeeReactor.get());
		else
			pEnt->removeReactor(g_pEmployeeReactor.get());
	}
	return Acad::eOk;
}

void detachAllEmployeeReactors()
{
	// Get an AcApDocumentIterator
	std::unique_ptr<AcApDocumentIterator> pIt(acDocManager->newAcApDocumentIterator());
	if (pIt == nullptr)
		return;
	// Store a pointer to the current document
	AcApDocument* pCurDoc = acDocManager->curDocument();
	// Iterate through all documents, make each document the current document and call attachEmployeeReactorToAllEmployee(false)
	for (; !pIt->done(); pIt->step()) { // TODO improve
		AcApDocument* pDoc = pIt->document();
		// TODO check errors
		if (pDoc->lockMode() == AcAp::kNone && acDocManager->setCurDocument(pDoc, AcAp::kAutoWrite, Adesk::kFalse) != Acad::eOk) {
			continue;
		}
		acDocManager->setCurDocument(pDoc);
		attachEmployeeReactorToAllEmployee(false);
	}
	// After iterating, reset the current document to be the one that was current before iterating
	acDocManager->setCurDocument(pCurDoc, AcAp::kNone, Adesk::kFalse);
}
//-----------------------------------------------------------------------------
//----- The one and only document manager object. You can use the DocVars object to retrieve
//----- document specific data throughout your application
AcApDataManager<CDocData> DocVars ;

//-----------------------------------------------------------------------------
//----- Implementation of the document data class.
CDocData::CDocData () : m_bEditCommand(false), m_bDoRepositioning(false), m_pDbEmployeeReactor(nullptr) {
	attachEmployeeReactorToAllEmployee(true);
}

//-----------------------------------------------------------------------------
CDocData::CDocData (const CDocData &data) {
	m_bEditCommand = false;
	m_bDoRepositioning = false;
	m_pDbEmployeeReactor = nullptr;
}

//-----------------------------------------------------------------------------
CDocData::~CDocData () {
	if (m_pDbEmployeeReactor) 
		delete m_pDbEmployeeReactor;
}
