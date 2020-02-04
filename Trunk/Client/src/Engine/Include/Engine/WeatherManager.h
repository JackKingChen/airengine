/********************************************************************
	author:		HJJ
	created:	2011:10:10
	purpose:	
*********************************************************************/

#pragma once

#ifndef __WeatherManager_H__
#define __WeatherManager_H__

#include "Common.h"
#include "Singleton.h"
#include "VCloudsManager.h"
#include "Atmosphere.h"
#include "CloudsManager.h"


//class Atmosphere;
class MoonObject;
class SkyDome;
//class CloudsManager;
class AtmosphereManualGPUDataParamSource;
class ISceneManager;
class CCamera;
class LensFlare;




enum LightingMode
{
	LM_LDR = 0,
	LM_HDR = 1
};


struct WMInitOpt
{
	LightingMode						mLightMode;
	f32									mTimeMultiplier;
	Atmosphere::Options					mAtmosphere;
	u32									mSkyDomeSteps;
	u32									mSkyDomeCircles;
	bool								mSkyDomeSmoothFading;
	f32									mSkyDomeFadePercent;
	f32									mMoonSize;
	f32									mMoonRelDist;
	AIRVector<CloudLayer::Options>	mCloudLayerOptions;
};


class EngineExport WeatherManager : public Singleton<WeatherManager>
{
	BE_SINGLETON(WeatherManager);
public:
	~WeatherManager();

    void Create(ISceneManager* sm, CCamera *c);

	void Clear();

    void Update(f32 timeSinceLastFrame);

	inline bool IsCreated() 
	{
		return mCreated;
	}

	inline void SetTimeMultiplier(f32 TimeMultiplier)
	{
		mTimeMultiplier = TimeMultiplier;
		mVCloudsManager->_updateWindSpeedConfig();
	}

	inline f32 GetTimeMultiplier() 
	{
		return mTimeMultiplier;
	}

	inline f32 _GetTimeOffset() 
	{
		return mTimeOffset;
	}

	inline SkyDome* GetMeshManager()
	{
		return mMeshManager;
	}

	inline Atmosphere* GetAtmosphereManager()
	{
		return mAtmosphereManager;
	}

	inline MoonObject* GetMoonManager()
	{
		return mMoonManager;
	}

	inline CloudsManager* GetCloudsManager()
	{
		return mCloudsManager;
	}

	inline VCloudsManager* GetVCloudsManager()
	{
		return mVCloudsManager;
	}

	void SetLightingMode( LightingMode lm);

	inline  LightingMode GetLightingMode() 
	{
		return mLightingMode;
	}

	void SetStarfieldEnabled( bool Enabled);

	inline  bool IsStarfieldEnabled() 
	{
		return mStarfield;
	}

	inline ISceneManager* GetSceneManager()
	{
		return mSceneManager;
	}

	inline CCamera* GetCamera()
	{
        return mCamera;
	}

	//static void RegisterManualGPUParamBuilder();

	//inline AtmosphereManualGPUDataParamSource* GetAtmosphereGPUParamSrc()
	//{
	//	return m_pAtmosphereGPUParam;
	//}

	bool LoadFromXML( CMarkupSTL& xml );

	bool SaveToXML( CMarkupSTL& xml );

	inline LensFlare* GetLensFlare()
	{
		return m_pLensFlare;
	}

	void CreateInternal();

    //Ç¿±Æ¸üÐÂ
    void ForceUpdate()
    {
        m_bForceUpdate = true;
    }

    void SetSceneAmbientColor(const CColorValue& color)
    {
        m_sceneAmbientColor = color;
    }
private:
	bool mCreated;
	SkyDome* mMeshManager;
	Atmosphere* mAtmosphereManager;
	MoonObject* mMoonManager;
	CloudsManager* mCloudsManager;
	VCloudsManager* mVCloudsManager;
    ISceneManager *mSceneManager;
	CCamera* mCamera;
	AIR::Vector3Df mLastCameraPosition;
	f32 mLastCameraFarClipDistance;
	LightingMode mLightingMode;
	bool mStarfield;
	f32 mTimeMultiplier;
	f32 mTimeOffset;
	//AtmosphereManualGPUDataParamSource* m_pAtmosphereGPUParam;
	ColorGradient mAmbientGradient;
	ColorGradient mSunGradient;
	LensFlare* m_pLensFlare;

	WMInitOpt mInitOpt;

    bool m_bForceUpdate;
    CColorValue m_sceneAmbientColor;
};

#define WEATHER_SYSTEM WeatherManager::GetInstance()

#endif //__WeatherManager_H__