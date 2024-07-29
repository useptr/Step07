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
#include "ADSKEmployeeReactor.h"
extern ADSKEmployeeReactor* pEmployeeReactor;

Acad::ErrorStatus attachEmployeeReactorToAllEmployee(bool attach)
{
	// Get the block table
	AcDbBlockTable* pBlockTable;
	Acad::ErrorStatus es = acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable);
	if (es != Acad::eOk)
		return es;
	// pBlockTable successfully opened
	// Get the model space record
	AcDbBlockTableRecord* pBlockTableRecord;
	es = pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord);
	if (es != Acad::eOk) { // release resources
		Acad::ErrorStatus es2 = pBlockTable->close();
		if (es2 != Acad::eOk) {
			acrx_abort(_T("\nApp failed to close BlockTable. Error: %s"), acadErrorStatusText(es2));
		}
		return es;
	}
	// pBlockTableRecord successfully opened
	es = pBlockTable->close();
	if (es != Acad::eOk) { // release BlockTable
		acrx_abort(_T("\nApp failed to close BlockTable. Error: %s"), acadErrorStatusText(es));
	}
	// Get a block table record iterator
	AcDbBlockTableRecordIterator* pIt;
	es = pBlockTableRecord->newIterator(pIt);
	if (es != Acad::eOk) { // release resources
		Acad::ErrorStatus es2 = pBlockTableRecord->close();
		if (es2 != Acad::eOk) {
			acrx_abort(_T("\nApp failed to close AcDbBlockTableRecord. Error: %s"), acadErrorStatusText(es2));
		}
		return es;
	}
	// Iterate through the Model Space records and retrieve the entities 
	for (; !pIt->done(); pIt->step()) {
		AcDbEntity* pEnt;
		es = pIt->getEntity(pEnt);
		if (es != Acad::eOk)
			continue;
		// pEnt successfully opened
		AcDbBlockReference* pBlockRef = AcDbBlockReference::cast(pEnt);
		if (pBlockRef == nullptr) { // release resources
			Acad::ErrorStatus es2 = pEnt->close();
			if (es2 != Acad::eOk) {
				acrx_abort(_T("\nApp failed to close AcDbEntity. Error: %s"), acadErrorStatusText(es2));
			}
			continue;
		}
		AcDbObjectId blockId{ pBlockRef->blockTableRecord() };
		AcDbBlockTableRecord* pBlockTableRecord{ nullptr };
		es = acdbOpenObject(pBlockTableRecord, blockId, AcDb::kForRead);
		if (es != Acad::eOk) { // release resources
			Acad::ErrorStatus es2 = pEnt->close();
			if (es2 != Acad::eOk) {
				acrx_abort(_T("\nApp failed to close AcDbEntity. Error: %s"), acadErrorStatusText(es2));
			}
			return es;
		}
		// pBlockTableRecord successfully opened
		// If the retrieved block reference entity is not an EMPLOYEE object, just return
		const TCHAR* blockName;
		es = pBlockTableRecord->getName(blockName);
		if (es != Acad::eOk) { // TODO release resources
			pBlockTableRecord->close();
			pEnt->close();
			return es;
		}
		pBlockTableRecord->close(); // TODO
		if (_tcscmp(blockName, _T("EMPLOYEE"))) {
			pEnt->close();
			continue;
		}
		// If the retrieved entity is an "EMPLOYEE" block reference, attach the global object reactor or if the parameter "attach" is false then remove the already attached object reactor

		if (attach) {
			pEnt->addReactor(pEmployeeReactor);
		}
		else {
			pEnt->removeReactor(pEmployeeReactor);
		}
		pEnt->close();
	}
	delete pIt;
	pBlockTableRecord->close();
	return Acad::eOk;
}

void detachAllEmployeeReactors()
{
	// Get an AcApDocumentIterator
	AcApDocumentIterator* pIt = acDocManager->newAcApDocumentIterator();
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
	delete pIt;
	// After iterating, reset the current document to be the one that was current before iterating
	acDocManager->setCurDocument(pCurDoc, AcAp::kNone, Adesk::kFalse);
}
//-----------------------------------------------------------------------------
//----- The one and only document manager object. You can use the DocVars object to retrieve
//----- document specific data throughout your application
AcApDataManager<CDocData> DocVars ;

//-----------------------------------------------------------------------------
//----- Implementation of the document data class.
CDocData::CDocData () {
}

//-----------------------------------------------------------------------------
CDocData::CDocData (const CDocData &data) {
}

//-----------------------------------------------------------------------------
CDocData::~CDocData () {
}
