/********************************************************************
	author:		HJJ
	created:	2011:10:10
	purpose:	
*********************************************************************/

#pragma once

#ifndef __SkyDome_H__
#define __SkyDome_H__


#include "Renderable.h"

class WeatherManager;

class EngineExport SkyDome : public Renderable
{
public:
		struct POS_UV_VERTEX
		{
			// λ��
			f32 x, y, z;
			// ��һ����λ��
			f32 nx, ny, nz;
			// ��������
			f32 u, v;
			// ͸����
			f32 o;
		};


		SkyDome(WeatherManager *s);

		~SkyDome();

        void Create();

		void _UpdateGeometry();

		inline void SetSkydomeFadingParameters( bool SmoothSkydomeFading, f32 SkydomeFadingPercent = 0.05f)
		{
			mSmoothSkydomeFading = SmoothSkydomeFading;
			mSkydomeFadingPercent = SkydomeFadingPercent;
		}

		inline bool GetSmoothSkydomeFading() 
		{
			return mSmoothSkydomeFading;
		}

		inline f32 GetSkydomeFadingPercent() 
		{
			return mSkydomeFadingPercent;
		}

		inline bool IsCreated() 
		{
			return mCreated;
		}

		void _SetGeometryParameters( int Steps,  int Circles);

		inline  int GetSteps() 
		{
			return mSteps;
		}

		inline  int GetCircles() 
		{
			return mCircles;
		}

		float GetSkydomeRadius() ;

		void Update( float fTimeElapse );

		bool Prepare();

		void _GetPrimitive();
		
		void Clear();
protected:
    virtual void PreRender(CCamera* pCamera);

	private:

		void _CreateGeometry();

		bool			mCreated;
		u32				mCircles;
		u32				mSteps;
		bool			mSmoothSkydomeFading;
		f32				mSkydomeFadingPercent;
		WeatherManager* mSkyX;


};

#endif //__SkyDome_H__