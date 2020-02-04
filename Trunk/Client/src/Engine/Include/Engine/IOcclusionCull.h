/********************************************************************
	author:		HJJ
	created:	2012:1:3
	purpose:	
*********************************************************************/

#pragma once

#ifndef __IOcclusionCull_H__
#define __IOcclusionCull_H__

#include "EngineConfig.h"

class CRenderQueue;

class EngineExport IOcclusionCull
{
public:
	IOcclusionCull(){};
	virtual ~IOcclusionCull(){};

	virtual bool Create() = 0;
	virtual void Release() = 0;
	virtual void RenderCullScene( CRenderQueue* pQueue ) = 0;
};

#endif //__IOcclusionCull_H__