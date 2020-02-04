/********************************************************************
	author:		HJJ
	created:	2011:10:10
	purpose:	
*********************************************************************/

#pragma once

#ifndef __CloudsManager_H__
#define __CloudsManager_H__

#include "ColorGradient.h"
#include "Renderable.h"
#include "FixedString.h"

//class CloudManualGPUDataParamSource;
class WeatherManager;
//class CPass;



class EngineExport CloudLayer : public Renderable
{
public:
	//云层控制参数
	struct Options 
	{
		//云层高度
		f32				Height;
		//云层缩放系数
		f32				Scale;
		//风向
		AIR::Vector2Df	WindDirection;
		//时间更新系统
		f32				TimeMultiplier;
		//距离衰减系数
		f32				DistanceAttenuation;
		//细节衰减系数
		f32				DetailAttenuation;
		//法向控制系数
		f32				NormalMultiplier;
		//云层高度体积
		f32				HeightVolume;
		//体积置换系数
		f32				VolumetricDisplacement;

		Options()
			: Height(100)
			, Scale(0.001f)
			, WindDirection(AIR::Vector2Df(1,1))
			, TimeMultiplier(0.125f)
			, DistanceAttenuation(0.05f)
			, DetailAttenuation(1)
			, NormalMultiplier(2)
			, HeightVolume(0.25f)
			, VolumetricDisplacement(0.01f)
		{
		}

		Options(f32 _Height, 
			    f32 _Scale, 
				const AIR::Vector2Df& _WindDirection, 
				f32 _TimeMultiplier)
			: Height(_Height)
			, Scale(_Scale)
			, WindDirection(_WindDirection)
			, TimeMultiplier(_TimeMultiplier)
			, DistanceAttenuation(0.05f)
			, DetailAttenuation(1)
			, NormalMultiplier(2)
			, HeightVolume(0.25f)
			, VolumetricDisplacement(0.01f)
		{
		}


		Options(f32 _Height, 
			    f32 _Scale, 
				const AIR::Vector2Df& _WindDirection, 
				f32 _TimeMultiplier,
				f32 _DistanceAttenuation,
				f32 _DetailAttenuation,
				f32 _NormalMultiplier,
				f32 _HeightVolume,
				f32 _VolumetricDisplacement)
			: Height(_Height)
			, Scale(_Scale)
			, WindDirection(_WindDirection)
			, TimeMultiplier(_TimeMultiplier)
			, DistanceAttenuation(_DistanceAttenuation)
			, DetailAttenuation(_DetailAttenuation)
			, NormalMultiplier(_NormalMultiplier)
			, HeightVolume(_HeightVolume)
			, VolumetricDisplacement(_VolumetricDisplacement)
		{
		}
	};


	
	CloudLayer(WeatherManager *s);

	CloudLayer(WeatherManager *s, const Options& o);

    ~CloudLayer();

	//获得云层的控制参数
	inline const Options& GetOptions() const
	{
		return mOptions;
	}

	//获得云层的控制参数
	inline Options& GetOptionsDynm()
	{
		return mOptions;
	}

	//设置环境光颜色梯度表
	inline void SetAmbientGradient(const ColorGradient& AmbientGradient)
	{
		mAmbientGradient = AmbientGradient;
	}

	//获得环境光颜色梯度表
	inline const ColorGradient& GetAmbientGradient() const
	{
		return mAmbientGradient;
	}

	//设置太阳光颜色梯度表
	inline void SetSunGradient(const ColorGradient& SunGradient)
	{
		mSunGradient = SunGradient;
	}

	//获得太阳光颜色梯度表
	inline const ColorGradient& GetSunGradient() const
	{
		return mSunGradient;
	}

	//获得云层的GPU传入参数
	//CloudManualGPUDataParamSource* GetCloudGPUParamSrc();

	//创建云层
	void CreateCloudLayerImpl();

	//释放
	void Clear();

	//更新参数
	void _UpdateInternalPassParameters();

	void Update( float fTimeElapse );

	bool Prepare();

	void _GetPrimitive();

	void PreRender(CCamera* pCamera);

private:
	//云层控制参数
	Options							mOptions;
	//环境光颜色梯度表
	ColorGradient					mAmbientGradient;
	//太阳光颜色梯度表
	ColorGradient					mSunGradient;
	//天气系统
	WeatherManager *				mSkyX;
	//云层的GPU控制参数
	//CloudManualGPUDataParamSource*	m_pGPUParmSrc;

    FixedString m_struExposure;
    FixedString m_struSunPosition;
    FixedString m_struSunColor;
    FixedString m_struHeight;
    FixedString m_struTime;
    FixedString m_struScale;
    FixedString m_struWindDirection;
    FixedString m_struCloudLayerHeightVolume;
    FixedString m_struCloudLayerVolumetricDisplacement;
    FixedString m_struAmbientLuminosity;
    FixedString m_struNormalMultiplier;
    FixedString m_struDetailAttenuation;
    FixedString m_struDistanceAttenuation;
    FixedString m_strweatherColor;
    FixedString m_strweatherMultiplier;

    void InitFixedStrings();
};



class EngineExport CloudsManager
{
public:

		CloudsManager(WeatherManager *h);

        ~CloudsManager();

		//插入一个云层
		CloudLayer* Add(const CloudLayer::Options& o);

		//获得云层
		inline CloudLayer* GetCloudLayer( u32 index )
		{
			if( index < 0 || index >= mCloudLayers.size() )
				return NULL;
			return mCloudLayers.at( index );
		}

		inline s32 GetCloudLayerCount()
		{
			return mCloudLayers.size();
		}

		//删除云层
		void Remove(CloudLayer *cl);

		//删除所有云层
		void RemoveAll();

		//更新所有云层的参数
		void UpdateAllCloudLayerInternalParams();

		//自动更新云层的technique
		void AutoUpdateTechnique();

	private:
		typedef AIRVector<CloudLayer*> VEC_CLOUDLAYER;
		typedef VEC_CLOUDLAYER::iterator VEC_CLOUDLAYER_ITOR;

		//云层列表
		VEC_CLOUDLAYER		mCloudLayers;
		//云层列表迭代器
		VEC_CLOUDLAYER_ITOR CloudLayersIt;
		//天气系统
		WeatherManager *	mSkyX;
};

#endif //__CloudsManager_H__