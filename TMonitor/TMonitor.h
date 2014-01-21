
// TMonitor.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CTMonitorApp:
// See TMonitor.cpp for the implementation of this class
//

class CTMonitorApp : public CWinApp
{
public:
	CTMonitorApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CTMonitorApp theApp;