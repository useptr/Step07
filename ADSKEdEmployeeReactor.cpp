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
//----- ADSKEdEmployeeReactor.cpp : Implementation of ADSKEdEmployeeReactor
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ADSKEdEmployeeReactor.h"
#include "Tchar.h" // _T
#include <ranges>
//-----------------------------------------------------------------------------
ADSKEdEmployeeReactor::ADSKEdEmployeeReactor (const bool autoInitAndRelease) : AcEditorReactor(), mbAutoInitAndRelease(autoInitAndRelease) {
	if ( autoInitAndRelease ) {
		if ( acedEditor )
			acedEditor->addReactor (this) ;
		else
			mbAutoInitAndRelease =false ;
	}
}

//-----------------------------------------------------------------------------
ADSKEdEmployeeReactor::~ADSKEdEmployeeReactor () {
	Detach () ;
}

//-----------------------------------------------------------------------------
void ADSKEdEmployeeReactor::Attach () {
	Detach () ;
	if ( !mbAutoInitAndRelease ) {
		if ( acedEditor ) {
			acedEditor->addReactor (this) ;
			mbAutoInitAndRelease =true ;
		}
	}
}

void ADSKEdEmployeeReactor::Detach () {
	if ( mbAutoInitAndRelease ) {
		if ( acedEditor ) {
			acedEditor->removeReactor (this) ;
			mbAutoInitAndRelease =false ;
		}
	}
}

AcEditor *ADSKEdEmployeeReactor::Subject () const {
	return (acedEditor) ;
}

bool ADSKEdEmployeeReactor::IsAttached () const {
	return (mbAutoInitAndRelease) ;
}

void ADSKEdEmployeeReactor::commandWillStart(const ACHAR* cmdStr)
{
	// Check if one of the commands "MOVE", "ROTATE", "STRETCH", "SCALE" or "GRIP_STRETCH" is started 
	if (_tcscmp(cmdStr, _T("MOVE")) && _tcscmp(cmdStr, _T("ROTATE")) && _tcscmp(cmdStr, _T("STRETCH")) && _tcscmp(cmdStr, _T("SCALE")) && _tcscmp(cmdStr, _T("GRIP_STRETCH"))) {
		return; // 
	}
	// If it is one of the monitored commands, set m_editCommand = true; and m_doRepositioning = false;
	DocVars.docData().m_editCommand = true;
	DocVars.docData().m_doRepositioning = false;
	// Reset any objectIds and position information 
	DocVars.docData().m_changedObjects.setLogicalLength(0);
	DocVars.docData().m_employeePositions.setLogicalLength(0);
}

void ADSKEdEmployeeReactor::commandEnded(const ACHAR* cmdStr)
{
	// If the command being monitored is not a monitored command from the above list of commands then m_editCommand == false; simply return. 
	if (!DocVars.docData().m_editCommand)
		return;
	// One of our monitored teams is active. Reset m_editCommand to false
	DocVars.docData().m_editCommand = false;
	// Set m_doRepositioning to true and start repositioning the moved objects 
	DocVars.docData().m_doRepositioning = true;
	for (int i : std::views::iota(0, DocVars.docData().m_changedObjects.length())) {
		// Open the entity
		AcDbBlockReference* pBlockRef{ nullptr };
		if (acdbOpenObject(pBlockRef, DocVars.docData().m_changedObjects.at(i), AcDb::kForWrite) != Acad::eOk)
			continue; // TODO maybe need to do something
		// If the actual position is different from the stored position, reposition the object 
		AcGePoint3d finalPos = pBlockRef->position();
		AcGePoint3d originalPos = DocVars.docData().m_employeePositions.at(i);
		if (finalPos != originalPos) {
			pBlockRef->setPosition(originalPos);
		}
		pBlockRef->close();
	}
}