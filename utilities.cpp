#include "stdafx.h"
#include "utilities.h"
#include <numbers>

#include "Tchar.h" // _T
// 
// Create a new block table record and add the entities of the employee to it 
// 
// In : 
// const TCHAR* name : Name of block table record 
// 
Acad::ErrorStatus createBlockRecord(const TCHAR* name) { // TODO error checking
	Acad::ErrorStatus es{ Acad::eOk };
	// Get the block table from the current working database (AcDbBlockTable, AcDbDatabase::getBlockTable() ) 
	AcDbBlockTable* pBlockTable = NULL;
	if ((es = acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable)) != Acad::eOk) {
		acutPrintf(_T("\nERROR: Couldn't open workingDatabase"));
		return es;
	}
	//Check if the block table record already exists(AcDbBlockTable::has()).If it exists, return an error status(Acad::eDuplicateKey). Don't forget to close the block table.
	if (pBlockTable->has(name)) {
		acutPrintf(_T("\nERROR: Block table record with name %s already exists"), name);
		pBlockTable->close();
		return Acad::eDuplicateKey;
	}
	// Create a new blocktablerecord with the "new" operator
	AcDbBlockTableRecord* pBlockTableRec = new AcDbBlockTableRecord();
	pBlockTableRec->setName(name); // TODO check Acad::eOutOfMemory
	// Initialize the new block table record.Set the origin to(0, 0, 0) (AcDbBlockTableRecord::setOrigin(AcGePoint3d::origin)).
	pBlockTableRec->setOrigin(AcGePoint3d::kOrigin);
	if ((es = pBlockTable->upgradeOpen()) != Acad::eOk) {
		acutPrintf(_T("\nERROR: Couldn't modify block table record"));
		delete pBlockTableRec;
		pBlockTable->close();
		return es;
	}
	// Add the new block table record to the block table.
	// add it to the block table (AcDbBlockTableRecord, AcDbBlockTable::add() )
	if ((es = pBlockTable->add(pBlockTableRec)) != Acad::eOk) {
		// The block table record has not been added to the block table, so we have to delete it.
		pBlockTable->close();
		delete pBlockTableRec;
		return es;
	}
	pBlockTable->close();

	// make circles (AcDbCircle) and an arc
	// append them to the new block table record. (AcDbBlockTableRecord::appendAcDbEntity()
	AcDbCircle* pFace = new AcDbCircle(AcGePoint3d::kOrigin, AcGeVector3d::kZAxis, 1.0);
	pFace->setColorIndex(2); // Yellow
	if ((es = pBlockTableRec->appendAcDbEntity(pFace)) != Acad::eOk) {
		delete pFace;
		pBlockTableRec->erase();
		pBlockTableRec->close();
		return es;

	}
	pFace->close();

	AcDbCircle* pLeftEye = new AcDbCircle(AcGePoint3d(0.33, 0.25, 0), AcGeVector3d::kZAxis, 0.1);
	pLeftEye->setColorIndex(5);  // Blue
	if ((es = pBlockTableRec->appendAcDbEntity(pLeftEye)) != Acad::eOk) {
		delete pLeftEye;
		pBlockTableRec->erase();
		pBlockTableRec->close();
		return es;
	}
	pLeftEye->close();

	AcDbCircle* pRightEye = new AcDbCircle(AcGePoint3d(-0.33, 0.25, 0), AcGeVector3d::kZAxis, 0.1);
	pRightEye->setColorIndex(5);  // Blue
	if ((es = pBlockTableRec->appendAcDbEntity(pRightEye)) != Acad::eOk) {
		delete pRightEye;
		pBlockTableRec->erase();
		pBlockTableRec->close();
		return es;
	}
	pRightEye->close();

	constexpr double pi = std::numbers::pi_v<double>;
	AcDbArc* pMouth = new AcDbArc(AcGePoint3d(0.0, 0.5, 0), 1.0, pi + (pi * 0.3), pi + (pi * 0.7));
	pMouth->setColorIndex(1);  // Red
	if ((es = pBlockTableRec->appendAcDbEntity(pMouth)) != Acad::eOk) {
		delete pMouth;
		pBlockTableRec->erase();
		pBlockTableRec->close();
		return es;
	}
	pMouth->close();

	pBlockTableRec->close();
	return Acad::eOk;
}

void printDbEvent(const AcDbObject* pObj, const TCHAR* pEvent)
{
	acutPrintf(_T("  Event: AcDbDatabaseReactor::%s "), pEvent);
	printObj(pObj);
}

void printObj(const AcDbObject* pObj)
{
	if (pObj == NULL) {
		acutPrintf(_T("(NULL)"));
		return;
	}

	AcDbHandle objHand;
	TCHAR  handbuf[17];

	// Get the handle as a string
	//
	pObj->getAcDbHandle(objHand);
	objHand.getIntoAsciiBuffer(handbuf);

	acutPrintf(
		_T("\n   (class==%s, handle==%s, id==%lx, db==%lx)"),
		pObj->isA()->name(), handbuf,
		pObj->objectId().asOldId(), pObj->database());
}