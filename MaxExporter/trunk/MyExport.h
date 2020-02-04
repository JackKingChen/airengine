/**********************************************************************
 *<
	FILE: MyExport.h

	DESCRIPTION:	Includes for Plugins

	CREATED BY:

	HISTORY:

 *>	Copyright (c) 2003, All Rights Reserved.
 **********************************************************************/

#ifndef __MyExport__H
#define __MyExport__H

#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"

#include <conio.h>

#include <vector>
#include <string>

//using namespace AIR;
//SIMPLE TYPE

const char NUM_MESH[] = "numMeshes";


extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;

#endif
