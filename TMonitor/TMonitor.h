
// TMonitor.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#define SIZE_NO_CHANGE      0
#define SIZE_MOVE_X         0x00000001
#define SIZE_MOVE_Y         0x00000010
#define SIZE_ELASTIC_X      0x00010000
#define SIZE_ELASTIC_Y      0x00100000
#define SIZE_ELASTIC_X_EX   0x01000000  // 宽度和主窗口相对位置固定
#define SIZE_ELASTIC_Y_EX   0x10000000  // 高度和主窗口相对位置固定

#define  TIMER_TIME 1
#define  PRJ_NAME   _T("PHIYO TF/SD DUPLICATOR Ver:")
#define  TEMP_FILE  _T("\\temp.log")
#define  DEFAULT_PASSWORD  _T("phiyo")
#define  CONFIG_FILE _T("\\setting.ini")

#define PAGE_COUNT  5

#define TS_123 0x00180004
#define TS_147 0x00300004
#define TF_147 0x00300008



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