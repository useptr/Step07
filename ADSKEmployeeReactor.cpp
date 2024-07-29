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
//----- ADSKEmployeeReactor.cpp : Implementation of ADSKEmployeeReactor
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ADSKEmployeeReactor.h"
#include "Tchar.h" // _T
//-----------------------------------------------------------------------------
ACRX_CONS_DEFINE_MEMBERS(ADSKEmployeeReactor, AcDbObjectReactor, 1)

//-----------------------------------------------------------------------------
ADSKEmployeeReactor::ADSKEmployeeReactor () : AcDbObjectReactor() {
}

//-----------------------------------------------------------------------------
ADSKEmployeeReactor::~ADSKEmployeeReactor () {
}

void ADSKEmployeeReactor::openedForModify(const AcDbObject* dbObj) // This will occur before the actual transformation takes place
{
	// If we are currently repositioning (m_doRepositioning == true) then return.
	if (DocVars.docData().m_doRepositioning)
		return;
	// If none of the monitored commands is active (m_editCommand == false) then return
	if (!DocVars.docData().m_editCommand)
		return;
	// Verify that the AcDbObject passed in, is an AcDbBlockReference entity, if not return.
	AcDbBlockReference* pBlockRef{ AcDbBlockReference::cast(dbObj) };
	if (pBlockRef == nullptr)
		return;
	// Retrieve the name of the AcDbBlockReference
	AcDbObjectId blockTableRecordId{ pBlockRef->blockTableRecord() };
	AcDbBlockTableRecord* pBlockTableRecord{ nullptr };
	if (acdbOpenObject(pBlockTableRecord, blockTableRecordId, AcDb::kForRead) != Acad::eOk) { // 
		return;
	}
	// pBlockTableRecord sucessfully opened
	// If it is not an EMPLOYEE block reference, just return.
	const TCHAR* blockName{ nullptr };
	pBlockTableRecord->getName(blockName);
	pBlockTableRecord->close();
	if (nullptr == blockName || _tcscmp(blockName, _T("EMPLOYEE")))
		return;
	// Store the position of the openedForModify() object in m_employeePositions 
	DocVars.docData().m_employeePositions.append(pBlockRef->position());
	// Store the objectId of the openedForModify() object in m_changedObjects
	DocVars.docData().m_changedObjects.append(pBlockRef->objectId());
}