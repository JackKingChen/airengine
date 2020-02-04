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
	//�Ʋ���Ʋ���
	struct Options 
	{
		//�Ʋ�߶�
		f32				Height;
		//�Ʋ�����ϵ��
		f32				Scale;
		//����
		AIR::Vector2Df	WindDirection;
		//ʱ�����ϵͳ
		f32				TimeMultiplier;
		//����˥��ϵ��
		f32				DistanceAttenuation;
		//ϸ��˥��ϵ��
		f32				DetailAttenuation;
		//�������ϵ��
		f32				NormalMultiplier;
		//�Ʋ�߶����
		f32				HeightVolume;
		//����û�ϵ��
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

	//����Ʋ�Ŀ��Ʋ���
	inline const Options& GetOptions() const
	{
		return mOptions;
	}

	//����Ʋ�Ŀ��Ʋ���
	inline Options& GetOptionsDynm()
	{
		return mOptions;
	}

	//���û�������ɫ�ݶȱ�
	inline void SetAmbientGradient(const ColorGradient& AmbientGradient)
	{
		mAmbientGradient = AmbientGradient;
	}

	//��û�������ɫ�ݶȱ�
	inline const ColorGradient& GetAmbientGradient() const
	{
		return mAmbientGradient;
	}

	//����̫������ɫ�ݶȱ�
	inline void SetSunGradient(const ColorGradient& SunGradient)
	{
		mSunGradient = SunGradient;
	}

	//���̫������ɫ�ݶȱ�
	inline const ColorGradient& GetSunGradient() const
	{
		return mSunGradient;
	}

	//����Ʋ��GPU�������
	//CloudManualGPUDataParamSource* GetCloudGPUParamSrc();

	//�����Ʋ�
	void CreateCloudLayerImpl();

	//�ͷ�
	void Clear();

	//���²���
	void _UpdateInternalPassParameters();

	void Update( float fTimeElapse );

	bool Prepare();

	void _GetPrimitive();

	void PreRender(CCamera* pCamera);

private:
	//�Ʋ���Ʋ���
	Options							mOptions;
	//��������ɫ�ݶȱ�
	ColorGradient					mAmbientGradient;
	//̫������ɫ�ݶȱ�
	ColorGradient					mSunGradient;
	//����ϵͳ
	WeatherManager *				mSkyX;
	//�Ʋ��GPU���Ʋ���
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

		//����һ���Ʋ�
		CloudLayer* Add(const CloudLayer::Options& o);

		//����Ʋ�
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

		//ɾ���Ʋ�
		void Remove(CloudLayer *cl);

		//ɾ�������Ʋ�
		void RemoveAll();

		//���������Ʋ�Ĳ���
		void UpdateAllCloudLayerInternalParams();

		//�Զ������Ʋ��technique
		void AutoUpdateTechnique();

	private:
		typedef AIRVector<CloudLayer*> VEC_CLOUDLAYER;
		typedef VEC_CLOUDLAYER::iterator VEC_CLOUDLAYER_ITOR;

		//�Ʋ��б�
		VEC_CLOUDLAYER		mCloudLayers;
		//�Ʋ��б������
		VEC_CLOUDLAYER_ITOR CloudLayersIt;
		//����ϵͳ
		WeatherManager *	mSkyX;
};

#endif //__CloudsManager_H__