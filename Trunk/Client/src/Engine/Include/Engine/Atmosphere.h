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

	//����ɢ����Ʋ���
	struct Options 
	{
		//ʱ����Ϣ: x = ��ǰʱ��,�� [0, 24]֮��, y = �ճ�ʱ��,��[0, 24]֮��, z = ����ʱ��,��[0, 24]֮��
		AIR::Vector3Df Time;
		//���嶫����λ��
		AIR::Vector2Df EastPosition;
		//�������ھ�
		f32 InnerRadius;
		//�������⾶
		f32 OuterRadius;
		//������߶�, ��[0, 1]֮��, 0=�������ھ�, 1=�������⾶
		f32 HeightPosition;
		//Rayleighɢ��ϵ��
		f32 RayleighMultiplier;
		//Mieɢ��ϵ��
		f32 MieMultiplier;
		//̫��ǿ��
		f32 SunIntensity;
		//RGBͨ���Ĳ���
		AIR::Vector3Df WaveLength;
		//��λ����
		f32 G;
		//�ع�ϵ��
		f32 Exposure;
		//ȡ������
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

	//���ô�������
	inline void SetOptions(const Options& _Options)
	{
		_Update(_Options, false);
	}

	//��ô�������
	inline Options& GetOptions()
	{
		return mOptions;
	}



	//���ݸ����ķ���ȡ�ô�����ɫ
	//Direction - ��һ���ĳ���
	AIR::Vector3Df GetColorAt(const AIR::Vector3Df& Direction);

	//��õ�ǰ��̫������
	AIR::Vector3Df GetSunDirection();

	//���̫����λ��
	AIR::Vector3Df GetSunPosition();

	//���´�������
	void _Update(const Options& NewOptions, bool ForceToUpdateAll = false);

	void UpdateInternal();

private:
	//ģ��shader�е�scale function
	float _Scale( float cos,  float uScaleDepth);

	//��������
	Options mOptions;

	//����ϵͳ������
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