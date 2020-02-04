/********************************************************************
	author:		HJJ
	created:	2011:10:27
	purpose:	
*********************************************************************/

#pragma once

#ifndef __LensFlare_H__
#define __LensFlare_H__

#include "Renderable.h"

class CCamera;

class EngineExport LensFlare : public Renderable
{
public:
	LensFlare();
	~LensFlare();

	void Clear();

	void Create();

	void Update( float fTimeElapse );

protected:
	void PreRender(CCamera* pCamera);

	void _GetPrimitive();

private:
	s32 mSunX;
	s32 mSunY;
	s32 mCenterX;
	s32 mCenterY;

};

#endif //__LensFlare_H__