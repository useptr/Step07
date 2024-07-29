// (C) Copyright 2002-2007 by Autodesk, Inc. 
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
//----- ADSKDbEmployeeReactor.cpp : Implementation of ADSKDbEmployeeReactor
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ADSKDbEmployeeReactor.h"
#include "Tchar.h" // _T
#include "ADSKEmployeeReactor.h"
#include "utilities.h"
extern ADSKEmployeeReactor* pEmployeeReactor;
//-----------------------------------------------------------------------------
ACRX_CONS_DEFINE_MEMBERS(ADSKDbEmployeeReactor, AcDbDatabaseReactor, 1)

//-----------------------------------------------------------------------------
ADSKDbEmployeeReactor::ADSKDbEmployeeReactor (AcDbDatabase *pDb) : AcDbDatabaseReactor(), mpDatabase(pDb) {
	if ( pDb )
		pDb->addReactor (this) ;
}

//-----------------------------------------------------------------------------
ADSKDbEmployeeReactor::~ADSKDbEmployeeReactor () {
	Detach () ;
}

//-----------------------------------------------------------------------------
void ADSKDbEmployeeReactor::Attach (AcDbDatabase *pDb) {
	Detach () ;
	if ( mpDatabase == NULL ) {
		if ( (mpDatabase =pDb) != NULL )
			pDb->addReactor (this) ;
	}
}

void ADSKDbEmployeeReactor::Detach () {
	if ( mpDatabase ) {
		mpDatabase->removeReactor (this) ;
		mpDatabase =NULL ;
	}
}

AcDbDatabase *ADSKDbEmployeeReactor::Subject () const {
	return (mpDatabase) ;
}

bool ADSKDbEmployeeReactor::IsAttached () const {
	return (mpDatabase != NULL) ;
}

void ADSKDbEmployeeReactor::objectAppended(const AcDbDatabase* dwg, const AcDbObject* dbObj)
{
	// Check if the appended object is an AcDbBlockReference. If not return
	AcDbBlockReference* pInsert = AcDbBlockReference::cast(dbObj);
	if (nullptr == pInsert)
		return;

	//printDbEvent(dbObj, _T("objectAppended")); // log
	acutPrintf(_T(" AcDbDatabaseReactor::objectAppended"));
	//printObj(dbObj);

	// Get the block table record of the reference
	AcDbObjectId blockId = pInsert->blockTableRecord();
	AcDbBlockTableRecord* pBlockTableRecord{ nullptr };
	if (acdbOpenObject(pBlockTableRecord, blockId, AcDb::kForRead) != Acad::eOk) {
		return;
	}
	// Get the name of the block table record
	const TCHAR* blockName;
	pBlockTableRecord->getName(blockName);
	pBlockTableRecord->close();
	//  check if it is an EMPLOYEE block reference.If not return
	if (_tcscmp(blockName, _T("EMPLOYEE")))
		return;
	// Attach the AsdkEmployeeReactor object reactor to the appended block reference
	dbObj->addReactor(pEmployeeReactor);
}