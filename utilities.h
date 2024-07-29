#pragma once
#include "StdAfx.h" 

//Declaration of createBlockRecord():
Acad::ErrorStatus createBlockRecord(const TCHAR* name);

void printDbEvent(const AcDbObject* pObj, const TCHAR* pEvent);

void printObj(const AcDbObject* pObj);