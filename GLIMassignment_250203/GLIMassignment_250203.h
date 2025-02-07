
// GLIMassignment_250203.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CGLIMassignment250203App:
// See GLIMassignment_250203.cpp for the implementation of this class
//

class CGLIMassignment250203App : public CWinApp
{
public:
	CGLIMassignment250203App();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CGLIMassignment250203App theApp;
