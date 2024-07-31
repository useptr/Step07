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
//----- acrxEntryPoint.cpp
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include "Tchar.h" // _T
#include <memory>
#include "utilities.h"
#include "ADSKEdEmployeeReactor.h"
#include "ADSKEmployeeReactor.h"
//#include "ADSKDbEmployeeReactor.h"
//-----------------------------------------------------------------------------
#define szRDS _RXST("ADSK")

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint

std::unique_ptr<ADSKEdEmployeeReactor> g_pEdEmployeeReactor;
std::unique_ptr <ADSKEmployeeReactor> g_pEmployeeReactor;

class CS7App : public AcRxArxApp {

public:
	CS7App () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		
		// TODO: Add your initialization code here
		g_pEdEmployeeReactor = std::move(std::make_unique<ADSKEdEmployeeReactor>(true));
		g_pEmployeeReactor = std::move(std::make_unique<ADSKEmployeeReactor>());

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		// TODO: Add your code here

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;

		// TODO: Unload dependencies here
		detachAllEmployeeReactors();
		g_pEdEmployeeReactor.release();
		g_pEmployeeReactor.release();

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kLoadDwgMsg(void* pkt) {
		AcRx::AppRetCode retCode = AcRxArxApp::On_kLoadDwgMsg(pkt);
		// Create a new instance of  the database reactor for every new drawing
		DocVars.docData().m_pDbEmployeeReactor = new ADSKDbEmployeeReactor(acdbHostApplicationServices()->workingDatabase());
		return (retCode);
	}

	virtual void RegisterServerComponents () {
	}
	
	static void AsdkStep07_CREATE(void) {
		if (CreateBlockRecord(_T("EMPLOYEE")) != Acad::eOk) {
			acutPrintf(_T("\nERROR: Couldn't create block record"));
			return;
		}
		acutPrintf(_T("\nBlock record EMPLOYEE successfully created"));
	}
	
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CS7App)

ACED_ARXCOMMAND_ENTRY_AUTO(CS7App, AsdkStep07, _CREATE, CREATE, ACRX_CMD_TRANSPARENT, NULL)

