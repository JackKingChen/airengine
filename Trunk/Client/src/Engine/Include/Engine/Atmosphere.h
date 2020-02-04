/********************************************************************
	author:		HJJ
	created:	2011:10:10
	purpose:	
*********************************************************************/

#pragma once

#ifndef __Atmosphere_H__
#define __Atmosphere_H__


#include "Common.h"
#include "Vector2D.h"
#include "Vector3D.h"
#include "EngineConfig.h"
#include "MemoryAlloter.h"
#include "FixedString.h"

class WeatherManager;

class EngineExport Atmosphere// : public EngineAllocator(Atmosphere)
{
	EngineAllocator(Atmosphere)
public:

	//大气散射控制参数
	struct Options 
	{
		//时间信息: x = 当前时间,在 [0, 24]之间, y = 日出时间,在[0, 24]之间, z = 日落时间,在[0, 24]之间
		AIR::Vector3Df Time;
		//定义东方的位置
		AIR::Vector2Df EastPosition;
		//大气层内径
		f32 InnerRadius;
		//大气层外径
		f32 OuterRadius;
		//摄像机高度, 在[0, 1]之间, 0=大气层内径, 1=大气层外径
		f32 HeightPosition;
		//Rayleigh散射系数
		f32 RayleighMultiplier;
		//Mie散射系数
		f32 MieMultiplier;
		//太阳强度
		f32 SunIntensity;
		//RGB通道的波长
		AIR::Vector3Df WaveLength;
		//相位函数
		f32 G;
		//曝光系数
		f32 Exposure;
		//取样个数
		int NumberOfSamples;

		AIR::Vector3Df WeatherColor;
		f32 WeatherMultiplier;

		Options()
			: Time(AIR::Vector3Df(12.0f, 6.0f, 18.0f/*14.0f, 7.50f, 20.50f*/))
			, EastPosition(AIR::Vector2Df(0,1))
			, InnerRadius(9.77501f)
			, OuterRadius(10.2963f)
			, HeightPosition(0.01f)
			, RayleighMultiplier(0.0022f)
			, MieMultiplier(0.000675f)
			, SunIntensity(30)
			, WaveLength(AIR::Vector3Df(0.57f, 0.54f, 0.44f))
			, G(-0.991)
			, Exposure(2.0f)
			, NumberOfSamples(4)
			, WeatherColor(0.1f, 0.1f, 0.1f)
			, WeatherMultiplier(1.0f)
		{
		}


		Options(const AIR::Vector3Df& _Time)
			: Time(_Time)
			, EastPosition(AIR::Vector2Df(0,1))
			, InnerRadius(9.77501f)
			, OuterRadius(10.2963f)
			, HeightPosition(0.01f)
			, RayleighMultiplier(0.0022f)
			, MieMultiplier(0.000675f)
			, SunIntensity(30)
			, WaveLength(AIR::Vector3Df(0.57f, 0.54f, 0.44f))
			, G(-0.991f)
			, Exposure(2.0f)
			, NumberOfSamples(4)
			, WeatherColor(0.1f,0.1f,0.1f)
			, WeatherMultiplier(1.0f)
		{
		}


		Options(const AIR::Vector3Df& _Time,
			    const AIR::Vector2Df& _EastPosition,
			    f32	 _InnerRadius,
			    f32	 _OuterRadius,
				f32	 _HeightPosition,
				f32	 _RayleighMultiplier,
				f32	 _MieMultiplier,
				f32  _SunIntensity,
				const AIR::Vector3Df& _WaveLength,
				f32  _G,
				f32  _Exposure,
				s32	 _NumerOfSamples)
			: Time(_Time)
			, EastPosition(_EastPosition)
			, InnerRadius(_InnerRadius)
			, OuterRadius(_OuterRadius)
			, HeightPosition(_HeightPosition)
			, RayleighMultiplier(_RayleighMultiplier)
			, MieMultiplier(_MieMultiplier)
			, SunIntensity(_SunIntensity)
			, WaveLength(_WaveLength)
			, G(_G)
			, Exposure(_Exposure)
			, NumberOfSamples(_NumerOfSamples)
		{
		}
	};


	Atmosphere(WeatherManager *s);

	~Atmosphere();

	//设置大气参数
	inline void SetOptions(const Options& _Options)
	{
		_Update(_Options, false);
	}

	//获得大气参数
	inline Options& GetOptions()
	{
		return mOptions;
	}



	//根据给定的方向取得大气颜色
	//Direction - 归一化的朝向
	AIR::Vector3Df GetColorAt(const AIR::Vector3Df& Direction);

	//获得当前的太阳方向
	AIR::Vector3Df GetSunDirection();

	//获得太阳的位置
	AIR::Vector3Df GetSunPosition();

	//更新大气参数
	void _Update(const Options& NewOptions, bool ForceToUpdateAll = false);

	void UpdateInternal();

private:
	//模拟shader中的scale function
	float _Scale( float cos,  float uScaleDepth);

	//大气参数
	Options mOptions;

	//天气系统管理器
	WeatherManager *mSkyX;

    FixedString m_struLightDir;
    FixedString m_struCameraPos;
    FixedString m_struInvWaveLength;
    FixedString m_struInnerRadius;
    FixedString m_struKrESun;
    FixedString m_struKmESun;
    FixedString m_struKr4PI;
    FixedString m_struKm4PI;
    FixedString m_struScale;
    FixedString m_struScaleDepth;
    FixedString m_struScaleOverScaleDepth;
    FixedString m_struG;
    FixedString m_struG2;
    FixedString m_strweatherColor;
    FixedString m_strweatherMultiplier;
    FixedString m_struExposure;
    FixedString m_struTime;
    FixedString m_struNumberOfSamples;
    FixedString m_struSamples;
};

#endif //__Atmosphere_H__