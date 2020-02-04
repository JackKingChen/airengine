/********************************************************************
	author:		HJJ
	created:	2011:10:10
	purpose:	
*********************************************************************/

#pragma once

#ifndef __MoonObject_H__
#define __MoonObject_H__

#include "Common.h"
#include "EngineConfig.h"

class WeatherManager;
class CBillboard;

class EngineExport MoonObject
{
public:

		MoonObject(WeatherManager *s);

		~MoonObject();

		void Create();

		void Update();

		inline CBillboard* GetMoonBillboard()
		{
			return mMoonBillboard;
		}

		void SetMoonSize(f32 MoonSize);

		void SetMoonRelDist(f32 relDist);

		inline f32 GetMoonSize() 
		{
			return mMoonSize;
		}

		inline f32 GetMoonRelDist()
		{
			return mMoonRelDist;
		}

		inline bool IsCreated() 
		{
			return mCreated;
		}

		void Clear();

	private:
		void _UpdateMoonBounds();

		CBillboard*		mMoonBillboard;
		bool			mCreated;
		f32				mMoonSize;
		f32				mMoonRelDist;
		WeatherManager *mSkyX;

};

#endif //__MoonObject_H__