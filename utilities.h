#pragma once
#include "StdAfx.h" 

//Declaration of createBlockRecord():
Acad::ErrorStatus CreateBlockRecord(const TCHAR* aszBlockTableRecordName);

void PrintDbEvent(const AcDbObject* apObj, const TCHAR* apEvent);

void PrintObj(const AcDbObject* apObj);