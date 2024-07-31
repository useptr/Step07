#include "stdafx.h"
#include "utilities.h"
#include <numbers>

#include "Tchar.h" // _T
// 
// Create a new block table record and add the entities of the employee to it 
// 
// In : 
// const TCHAR* aszBlockTableRecordName : name of block table record 
// 
Acad::ErrorStatus CreateBlockRecord(const TCHAR* aszBlockTableRecordName) { // TODO error checking
	// Get the block table from the current working database
	AcDbBlockTablePointer pBlockTable(acdbHostApplicationServices()->workingDatabase(), AcDb::kForRead);
	if (pBlockTable.openStatus() != Acad::eOk) {
		acutPrintf(_T("\nERROR: Cannot open BlockTable"));
		return pBlockTable.openStatus();
	}
	//Check if the block table record already exists(AcDbBlockTable::has()).If it exists, return an error status(Acad::eDuplicateKey). Don't forget to close the block table.
	if (pBlockTable->has(aszBlockTableRecordName)) {
		acutPrintf(_T("\nERROR: Block table record with aszBlockTableRecordName %s already exists"), aszBlockTableRecordName);
		return Acad::eDuplicateKey;
	}
	// Create a new blocktablerecord
	AcDbBlockTableRecordPointer pBlockTableRec;
	Acad::ErrorStatus es = pBlockTableRec.create();
	if (es != Acad::eOk) {
		acutPrintf(_T("\nERROR: Cannot create LayerTableRecord '%s'"), aszBlockTableRecordName);
		return es;
	}
	es = pBlockTableRec->setName(aszBlockTableRecordName);
	if (es != Acad::eOk) {
		acutPrintf(_T("\nERROR: BlockTableRecord setName '%s'"), aszBlockTableRecordName);
		return es;
	}
	// Initialize the new block table record.Set the origin to(0, 0, 0) (AcDbBlockTableRecord::setOrigin(AcGePoint3d::origin)).
	es = pBlockTableRec->setOrigin(AcGePoint3d::kOrigin);
	if (es != Acad::eOk) {
		acutPrintf(_T("\nERROR: BlockTableRecord '%s' setOrigin"), aszBlockTableRecordName);
		return es;
	}
	es = pBlockTable->upgradeOpen();
	if (es != Acad::eOk) {
		acutPrintf(_T("\nERROR: Cannot open BlockTable for write"));
		return es;
	}
	// Add the new block table record to the block table.
	// add it to the block table (AcDbBlockTableRecord, AcDbBlockTable::add() )
	es = pBlockTable->add(pBlockTableRec);
	if (es != Acad::eOk) {
		acutPrintf(_T("\nERROR: Cannot add record '%s' to BlockTable"), aszBlockTableRecordName);
		return es;
	}

	// make circles (AcDbCircle) and an arc
	// append them to the new block table record. (AcDbBlockTableRecord::appendAcDbEntity()
	AcDbObjectPointer<AcDbCircle> pFace; // TODO or call copy ctor with new AcDbCircle(AcGePoint3d::kOrigin, AcGeVector3d::kZAxis, 1.0)
	pFace.create();
	pFace->setCenter(AcGePoint3d::kOrigin);
	pFace->setNormal(AcGeVector3d::kZAxis);
	pFace->setRadius(1.0);
	pFace->setColorIndex(2); // Yellow
	es = pBlockTableRec->appendAcDbEntity(pFace);
	if (es != Acad::eOk) {
		pBlockTableRec->erase();
		return es;

	}

	AcDbObjectPointer<AcDbCircle> pLeftEye;
	pLeftEye.create();
	pLeftEye->setCenter(AcGePoint3d(0.33, 0.25, 0));
	pLeftEye->setNormal(AcGeVector3d::kZAxis);
	pLeftEye->setRadius(0.1);
	pLeftEye->setColorIndex(5);  // Blue
	es = pBlockTableRec->appendAcDbEntity(pLeftEye);
	if (es != Acad::eOk) {
		pBlockTableRec->erase();
		return es;
	}

	AcDbObjectPointer<AcDbCircle> pRightEye;
	pRightEye.create();
	pRightEye->setCenter(AcGePoint3d(-0.33, 0.25, 0));
	pRightEye->setNormal(AcGeVector3d::kZAxis);
	pRightEye->setRadius(0.1);
	pRightEye->setColorIndex(5);  // Blue
	es = pBlockTableRec->appendAcDbEntity(pRightEye);
	if (es != Acad::eOk) {
		pBlockTableRec->erase();
		return es;
	}

	constexpr double dPi = std::numbers::pi_v<double>;
	AcDbObjectPointer<AcDbArc> pMouth;
	pMouth.create();
	pMouth->setCenter(AcGePoint3d(0.0, 0.5, 0));
	pMouth->setRadius(1.0);
	pMouth->setStartAngle(dPi + (dPi * 0.3));
	pMouth->setEndAngle(dPi + (dPi * 0.7));
	pMouth->setColorIndex(1);  // Red
	es = pBlockTableRec->appendAcDbEntity(pMouth);
	if (es != Acad::eOk) {
		pBlockTableRec->erase();
		return es;
	}

	return Acad::eOk;
}

void PrintDbEvent(const AcDbObject* apObj, const TCHAR* apEvent)
{
	acutPrintf(_T("  Event: AcDbDatabaseReactor::%s "), apEvent);
	PrintObj(apObj);
}

void PrintObj(const AcDbObject* apObj)
{
	if (apObj == NULL) {
		acutPrintf(_T("(NULL)"));
		return;
	}

	AcDbHandle objHand;
	TCHAR  szHandbuf[17];

	// Get the handle as a string
	//
	apObj->getAcDbHandle(objHand);
	objHand.getIntoAsciiBuffer(szHandbuf);

	acutPrintf(
		_T("\n   (class==%s, handle==%s, id==%lx, db==%lx)"),
		apObj->isA()->name(), szHandbuf,
		apObj->objectId().asOldId(), apObj->database());
}