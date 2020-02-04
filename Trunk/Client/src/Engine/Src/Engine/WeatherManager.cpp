#include "stdafx.h"
#include "WeatherManager.h"
#include "VCloudsManager.h"
#include "CloudsManager.h"
#include "Atmosphere.h"
#include "SkyDome.h"
#include "MoonObject.h"
//#include "WeatherManualGPUParamSource.h"
#include "ISceneManager.h"
#include "Camera.h"
#include "Engine.h"
#include "Billboard.h"
#include "SceneNode.h"
//#include "WeatherTechniqueSelecter.h"
#include "LensFlare.h"





WeatherManager::WeatherManager()
: mSceneManager(NULL)
, mCamera(NULL)
, mMeshManager(NULL)
, mAtmosphereManager(NULL)
//, mGPUManager(new GPUManager(this))
, mMoonManager(NULL)
, mCloudsManager(NULL)
, mVCloudsManager(NULL)
, mCreated(false)
, mLastCameraPosition(AIR::Vector3Df(0,0,0))
, mLastCameraFarClipDistance(0)
, mLightingMode(LM_LDR)
, mStarfield(true)
, mTimeMultiplier(0.0f)
, mTimeOffset(0.0f)
, m_bForceUpdate(true)
{
	//m_pAtmosphereGPUParam = static_cast<AtmosphereManualGPUDataParamSource*>( CREATE_MANUAL_GPU_PARAM_SOURCE(AtmosphereManualGPUDataParamSource) );

	mAmbientGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(1,1,1) * 0.65f, 1.0f));
	mAmbientGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.8f,0.8f,0.8f) * 0.65f, 0.625f)); 
	mAmbientGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.72f,0.72f,0.65f) * 0.65f, 0.5625f));
	mAmbientGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.7f, 0.7f, 0.65f) * 0.65f, 0.5f));
	mAmbientGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.62f, 0.62f, 0.6f) * 0.65f, 0.45f));
	mAmbientGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.5f, 0.5f, 0.4f) * 0.65f, 0.35f));
	mAmbientGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.4f, 0.2f, 0.2f) * 0.65f, 0.17f));
	mAmbientGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.1f, 0.1f, 0.1f) * 0.65f, 0));


	mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(1,1,1)*0.95f, 1.0f));
	mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(1,1,1)*0.8, 0.75f));
	mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.8,0.75,0.55)*1.3, 0.5625f));
	mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.6,0.5,0.2)*0.75, 0.5f));
	mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.6,0.5,0.2)*0.35, 0.4725f));
	mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.5,0.5,0.5)*0.15, 0.45f));
	mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.2,0.2,0.25)*0.5, 0.3f));
	mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.2,0.2,0.2)*0.15, 0.15f));
	mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.2,0.2,0.2)*0.1, 0.0f));
}

WeatherManager::~WeatherManager()
{

}

void WeatherManager::Create(ISceneManager* sm, CCamera *c)
{
	if (mCreated)
	{
		return;
	}
	mSceneManager = sm;
	mCamera = c;

	mMeshManager = new SkyDome(this);
	mAtmosphereManager = new Atmosphere(this);
	mMoonManager = new MoonObject(this);
	mCloudsManager = new CloudsManager(this);
	mVCloudsManager = new VCloudsManager(this);
	//m_pLensFlare = new LensFlare;

	mMeshManager->Create();
	mAtmosphereManager->_Update(mAtmosphereManager->GetOptions(), true);
	mLastCameraPosition = mCamera->GetPosition();
	mLastCameraFarClipDistance = mCamera->GetViewFar();
	mMoonManager->Create();
	//m_pLensFlare->Create();

    m_sceneAmbientColor = sm->GetAmbientColor();

	mCreated = true;
}

void WeatherManager::Clear()
{
	if (!mCreated)
	{
		return;
	}

	if ( mCloudsManager )
	{
		mCloudsManager->RemoveAll();
		SafeDelete( mCloudsManager );
	}

	if ( mMeshManager )
	{
		mMeshManager->Clear();
		SafeDelete( mMeshManager );
	}

	if ( mMoonManager )
	{
		mMoonManager->Clear();
		SafeDelete( mMoonManager );
	}
	
	if ( mVCloudsManager )
	{
		mVCloudsManager->remove();
		SafeDelete( mVCloudsManager );
	}

	if ( m_pLensFlare )
	{
		m_pLensFlare->Clear();
		SafeDelete( m_pLensFlare );
	}

	mCreated = false;
}

void WeatherManager::Update(f32 timeSinceLastFrame)
{
	if (!mCreated)
	{
		return;
	}
	if ( mCamera != mSceneManager->GetCamera() )
	{
		mCamera = mSceneManager->GetCamera();
	}

	if ( mMeshManager )
	{
		mMeshManager->Update( timeSinceLastFrame );
	}

	if (mTimeMultiplier != 0 || m_bForceUpdate)
	{
		Atmosphere::Options opt = mAtmosphereManager->GetOptions();

		float timemultiplied = timeSinceLastFrame * mTimeMultiplier;
		opt.Time.x += timemultiplied;
		mTimeOffset += timemultiplied;
		if (opt.Time.x > 24)
		{
			opt.Time.x -= 24;
		} 
		else if (opt.Time.x < 0)
		{
			opt.Time.x += 24;
		}

		mAtmosphereManager->SetOptions(opt);

        CLight* pSunLight = mSceneManager->GetSunLight();
        Vector3Df SunDir = -(mAtmosphereManager->GetSunDirection());
        if (SunDir.y < 0.01f)
        {
            SunDir.Set( 0, 0, 0 );
        }
        else
        {
            SunDir = -SunDir;
        }
        pSunLight->SetDirection( SunDir );

        //float point = SunDir.y;//(-SunDir.y + 1.0f) / 2.0f;
        Vector3Df vpos( mAtmosphereManager->GetOptions().EastPosition.x, 0, mAtmosphereManager->GetOptions().EastPosition.y );
        vpos.Normalize();
        Vector3Df sunCol = mAtmosphereManager->GetColorAt( vpos );//mSunGradient.GetColor(point);
        Vector3Df ambientCol = mAmbientGradient.GetColor(AIR::Abs(SunDir.y));
        //pSunLight->SetAmbient( CColorValue( 1.0f, ambientCol.x, ambientCol.y, ambientCol.z ) );
        //pSunLight->SetDiffuse( CColorValue( 1.0f, sunCol.x, sunCol.y, sunCol.z ) );
        mSceneManager->SetFogColor( CColorValue( 1.0f, ambientCol.x, ambientCol.y, ambientCol.z ) );
        //f32 fY = AIR::Abs(SunDir.y);
        mSceneManager->SetAmbientLight(CColorValue( 1.0f, ambientCol.x, ambientCol.y, ambientCol.z));

        m_bForceUpdate = false;
	}

	if (mLastCameraPosition != mCamera->GetPosition())
	{
		mMeshManager->GetNodeParent()->SetPosition(mCamera->GetPosition());
		mMoonManager->GetMoonBillboard()->GetNodeParent()->SetPosition(mCamera->GetPosition());

		mLastCameraPosition = mCamera->GetPosition();
	}

	if (mLastCameraFarClipDistance != mCamera->GetViewFar())
	{
		mMeshManager->_UpdateGeometry();
		mMoonManager->Update();

		mLastCameraFarClipDistance = mCamera->GetViewFar();
	}

	mVCloudsManager->Update(timeSinceLastFrame);

	if ( m_pLensFlare )
	{
		m_pLensFlare->Update( timeSinceLastFrame );
	}
	
}

void WeatherManager::SetLightingMode( LightingMode lm)
{
	mLightingMode = lm;

	if (!mCreated)
	{
		return;
	}

	//WeatherTechniqueSelecter::SelectTechnique( mMeshManager );

	mCloudsManager->AutoUpdateTechnique();

	mAtmosphereManager->_Update(mAtmosphereManager->GetOptions(), true);
}

void WeatherManager::SetStarfieldEnabled( bool Enabled)
{
	mStarfield = Enabled;

	if (!mCreated)
	{
		return;
	}

	//WeatherTechniqueSelecter::SelectTechnique( mMeshManager );

	mAtmosphereManager->_Update(mAtmosphereManager->GetOptions(), true);
}


//void WeatherManager::RegisterManualGPUParamBuilder()
//{
//	REGISTOR_MANUAL_GPU_PARAM_SOURCE_BUILDER( AtmosphereManualGPUDataParamSource, AtmosphereManualGPUDataParamSourceBuilder );
//	REGISTOR_MANUAL_GPU_PARAM_SOURCE_BUILDER( CloudManualGPUDataParamSource, CloudManualGPUDataParamSourceBuilder );
//}



bool WeatherManager::LoadFromXML( CMarkupSTL& xml )
{
	AIRString szSubValue;

	//获取照明模式
	szSubValue = xml.GetAttrib( "LightMode" ).c_str();
	if ( szSubValue == "LDR" )
	{
		SetLightingMode( LM_LDR );
		mInitOpt.mLightMode = LM_LDR;
	}
	else if ( szSubValue == "HDR" )
	{
		SetLightingMode( LM_HDR );
		mInitOpt.mLightMode = LM_HDR;
	}

	//时间变化系数
	szSubValue = xml.GetAttrib( "TimeMultiplier" ).c_str();
	mInitOpt.mTimeMultiplier = atof(szSubValue.c_str());
	SetTimeMultiplier( mInitOpt.mTimeMultiplier );
	

	AIRString szChildElem;
	Vector3Df vec3;
	Vector2Df vec2;

	while ( xml.FindChildElem() )
	{
		szChildElem = xml.GetChildTagName().c_str();

		//创建大气散射参数
		if ( szChildElem == "Atmosphere" )
		{
			Atmosphere::Options opt;

			szSubValue = xml.GetChildAttrib( "Time" ).c_str();
			if ( szSubValue.length() )
			{
				sscanf( szSubValue.c_str(), "(%f,%f,%f)", &vec3.x, &vec3.y, &vec3.z );
				opt.Time = vec3;
				mInitOpt.mAtmosphere.Time = vec3;
			}

			szSubValue = xml.GetChildAttrib( "EastPosition" ).c_str();
			if ( szSubValue.length() )
			{
				sscanf( szSubValue.c_str(), "(%f,%f)", &vec2.x, &vec2.y );
				opt.EastPosition = vec2;
				mInitOpt.mAtmosphere.EastPosition = vec2;
			}			

			szSubValue = xml.GetChildAttrib( "InnerRadius" ).c_str();
			if ( szSubValue.length() )
			{
				opt.InnerRadius = atof( szSubValue.c_str() );
				mInitOpt.mAtmosphere.InnerRadius = opt.InnerRadius;
			}			

			szSubValue = xml.GetChildAttrib( "OuterRadius" ).c_str();
			if ( szSubValue.length() )
			{
				opt.OuterRadius = atof( szSubValue.c_str() );
				mInitOpt.mAtmosphere.OuterRadius = opt.OuterRadius;
			}			

			szSubValue = xml.GetChildAttrib( "HeightPosition" ).c_str();
			if ( szSubValue.length() )
			{
				opt.HeightPosition = atof( szSubValue.c_str() );
				mInitOpt.mAtmosphere.HeightPosition = opt.HeightPosition;
			}			

			szSubValue = xml.GetChildAttrib( "RayleighMultiplier" ).c_str();
			if ( szSubValue.length() )
			{
				opt.RayleighMultiplier = atof( szSubValue.c_str() );
				mInitOpt.mAtmosphere.RayleighMultiplier = opt.RayleighMultiplier;
			}			

			szSubValue = xml.GetChildAttrib( "MieMultiplier" ).c_str();
			if ( szSubValue.length() )
			{
				opt.MieMultiplier = atof( szSubValue.c_str() );
				mInitOpt.mAtmosphere.MieMultiplier = opt.MieMultiplier;
			}			

			szSubValue = xml.GetChildAttrib( "SunIntensity" ).c_str();
			if ( szSubValue.length() )
			{
				opt.SunIntensity = atof( szSubValue.c_str() );
				mInitOpt.mAtmosphere.SunIntensity = opt.SunIntensity;
			}			

			szSubValue = xml.GetChildAttrib( "WaveLength" ).c_str();
			if ( szSubValue.length() )
			{
				sscanf( szSubValue.c_str(), "(%f,%f,%f)", &vec3.x, &vec3.y, &vec3.z );
				opt.WaveLength = vec3;
				mInitOpt.mAtmosphere.WaveLength = opt.WaveLength;
			}

			szSubValue = xml.GetChildAttrib( "G" ).c_str();
			if ( szSubValue.length() )
			{
				opt.G = atof( szSubValue.c_str() );
				mInitOpt.mAtmosphere.G = opt.G;
			}			

			szSubValue = xml.GetChildAttrib( "Exposure" ).c_str();
			if ( szSubValue.length() )
			{
				opt.Exposure = atof( szSubValue.c_str() );
				mInitOpt.mAtmosphere.Exposure = opt.Exposure;
			}			

			szSubValue = xml.GetChildAttrib( "NumberOfSamples" ).c_str();
			if ( szSubValue.length() )
			{
				opt.NumberOfSamples = atoi( szSubValue.c_str() );
				mInitOpt.mAtmosphere.NumberOfSamples = opt.NumberOfSamples;
			}			

			mAtmosphereManager->SetOptions( opt );
		}
		//创建天空球
		else if ( szChildElem == "SkyDome" )
		{
			s32 nCircles = 8;
			s32 nSteps = 8;
			bool bSmoothFading = true;
			f32 fFadePercent = 0.05f;
			szSubValue = xml.GetChildAttrib( "Circles" ).c_str();
			if ( szSubValue.length() )
			{
				nCircles = atoi( szSubValue.c_str() );
			}	
			mInitOpt.mSkyDomeCircles = nCircles;

			szSubValue = xml.GetChildAttrib( "Steps" ).c_str();
			if ( szSubValue.length() )
			{
				nSteps = atoi( szSubValue.c_str() );
			}		
			mInitOpt.mSkyDomeSteps = nSteps;

			szSubValue = xml.GetChildAttrib( "SmoothFading" ).c_str();
			if ( szSubValue == "TRUE" )
			{
				bSmoothFading = true;
			}
			else
			{
				bSmoothFading = false;
			}
			mInitOpt.mSkyDomeSmoothFading = bSmoothFading;

			szSubValue = xml.GetChildAttrib( "FadingPercent" ).c_str();
			if ( szSubValue.length() )
			{
				fFadePercent = atof( szSubValue.c_str() );
			}		
			mInitOpt.mSkyDomeFadePercent = fFadePercent;

			mMeshManager->_SetGeometryParameters( nSteps, nCircles );
			mMeshManager->SetSkydomeFadingParameters( bSmoothFading, fFadePercent );

		}
		//创建月亮
		else if ( szChildElem == "MoonObject" )
		{
			f32 fSize = 1.0f;
			f32 fRelDist = 1.0f;
			szSubValue = xml.GetChildAttrib( "Size" ).c_str();
			if ( szSubValue.length() )
			{
				fSize = atof( szSubValue.c_str() );
				mMoonManager->SetMoonSize( fSize );
			}
			mInitOpt.mMoonSize = fSize;

			szSubValue = xml.GetChildAttrib( "RelDist" ).c_str();
			if ( szSubValue.length() )
			{
				fRelDist = atof( szSubValue.c_str() );
				mMoonManager->SetMoonRelDist( fRelDist );
			}
			mInitOpt.mMoonRelDist = fRelDist;
		}
		//创建平面云层
		else if ( szChildElem == "CloudsManager" )
		{
			xml.IntoElem();

			while ( xml.FindChildElem("CloudLayer") )
			{
				CloudLayer::Options opt;

				szSubValue = xml.GetChildAttrib( "Height" ).c_str();
				if ( szSubValue.length() )
				{
					opt.Height = atof( szSubValue.c_str() );
				}				

				szSubValue = xml.GetChildAttrib( "Scale" ).c_str();
				if ( szSubValue.length() )
				{
					opt.Scale = atof( szSubValue.c_str() );
				}				

				szSubValue = xml.GetChildAttrib( "WindDirection" ).c_str();
				if ( szSubValue.length() )
				{
					sscanf( szSubValue.c_str(), "(%f,%f)", &opt.WindDirection.x, &opt.WindDirection.y );
				}				

				szSubValue = xml.GetChildAttrib( "TimeMultiplier" ).c_str();
				if ( szSubValue.length() )
				{
					opt.TimeMultiplier = atof( szSubValue.c_str() );
				}				

				szSubValue = xml.GetChildAttrib( "DistanceAttenuation" ).c_str();
				if ( szSubValue.length() )
				{
					opt.DistanceAttenuation = atof( szSubValue.c_str() );
				}				

				szSubValue = xml.GetChildAttrib( "DetailAttenuation" ).c_str();
				if ( szSubValue.length() )
				{
					opt.DetailAttenuation = atof( szSubValue.c_str() );
				}				

				szSubValue = xml.GetChildAttrib( "NormalMultiplier" ).c_str();
				if ( szSubValue.length() )
				{
					opt.NormalMultiplier = atof( szSubValue.c_str() );
				}				

				szSubValue = xml.GetChildAttrib( "HeightVolume" ).c_str();
				if ( szSubValue.length() )
				{
					opt.HeightVolume = atof( szSubValue.c_str() );
				}				

				szSubValue = xml.GetChildAttrib( "VolumetricDisplacement" ).c_str();
				if ( szSubValue.length() )
				{
					opt.VolumetricDisplacement = atof( szSubValue.c_str() );
				}		

				mInitOpt.mCloudLayerOptions.push_back( opt );

				mCloudsManager->Add( opt );
			}

			xml.OutOfElem();
		}
		else if ( szChildElem == "VCloudsManager" )
		{
		}
	}
	return true;
}



void WeatherManager::CreateInternal()
{
	//获取照明模式
	SetLightingMode( mInitOpt.mLightMode );

	//时间变化系数
	SetTimeMultiplier( mInitOpt.mTimeMultiplier );

	//创建大气散射参数
	mAtmosphereManager->SetOptions( mInitOpt.mAtmosphere );
	
	//创建天空球
	mMeshManager->_SetGeometryParameters( mInitOpt.mSkyDomeSteps, mInitOpt.mSkyDomeCircles );
	mMeshManager->SetSkydomeFadingParameters( mInitOpt.mSkyDomeSmoothFading, mInitOpt.mSkyDomeFadePercent );

	//创建月亮
	mMoonManager->SetMoonSize( mInitOpt.mMoonSize );
	mMoonManager->SetMoonRelDist( mInitOpt.mMoonRelDist );

	//创建平面云层
	u32 i = 0;
	u32 c = mInitOpt.mCloudLayerOptions.size();
	for ( ; i < c; ++i )
	{
		mCloudsManager->Add( mInitOpt.mCloudLayerOptions[i] );
	}
}


bool WeatherManager::SaveToXML( CMarkupSTL& xml )
{
	xml.AddElem( "WeatherManager" );
	if ( this->mLightingMode == LM_LDR )
	{
		xml.AddAttrib( "LightMode", "LDR" );
	}
	else
	{
		xml.AddAttrib( "LightMode", "HDR" );
	}

	xml.AddAttrib( "TimeMultiplier", GetString("%f", mTimeMultiplier) );

	xml.IntoElem();

	Atmosphere::Options& opt = mAtmosphereManager->GetOptions();
	xml.AddElem( "Atmosphere" );
	xml.AddAttrib( "Time", GetString("(%f,%f,%f)", opt.Time.x, opt.Time.y, opt.Time.z) );
	xml.AddAttrib( "EastPosition", GetString("(%f,%f)", opt.EastPosition.x, opt.EastPosition.y) );
	xml.AddAttrib( "InnerRadius", GetString("%f", opt.InnerRadius) );
	xml.AddAttrib( "OuterRadius", GetString("%f", opt.OuterRadius) );
	xml.AddAttrib( "HeightPosition", GetString("%f", opt.HeightPosition) );
	xml.AddAttrib( "RayleighMultiplier", GetString("%f", opt.RayleighMultiplier) );
	xml.AddAttrib( "MieMultiplier", GetString("%f", opt.MieMultiplier) );
	xml.AddAttrib( "SunIntensity", GetString("%f", opt.SunIntensity) );
	xml.AddAttrib( "WaveLength", GetString("(%f,%f,%f)", opt.WaveLength.x, opt.WaveLength.y, opt.WaveLength.z) );
	xml.AddAttrib( "G", GetString("%f", opt.G) );
	xml.AddAttrib( "Exposure", GetString("%f", opt.Exposure) );
	xml.AddAttrib( "NumberOfSamples", GetString("%f", opt.NumberOfSamples) );

	xml.AddElem( "SkyDome" );
	xml.AddAttrib( "Circles", mMeshManager->GetCircles() );
	xml.AddAttrib( "Steps", mMeshManager->GetSteps() );
	if ( mMeshManager->GetSmoothSkydomeFading() )
	{
		xml.AddAttrib( "SmoothFading", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "SmoothFading", "FALSE" );
	}
	xml.AddAttrib( "FadingPercent", GetString("%f",mMeshManager->GetSkydomeFadingPercent()));


	xml.AddElem( "MoonObject" );
	xml.AddAttrib( "Size", GetString("%f",mMoonManager->GetMoonSize()));
	xml.AddAttrib( "RelDist", GetString("%f",mMoonManager->GetMoonRelDist()));

	s32 count = mCloudsManager->GetCloudLayerCount();
	if ( count )
	{
		xml.AddElem( "CloudsManager" );
		xml.IntoElem();
		for ( int i = 0; i < count; ++i )
		{
			CloudLayer::Options& opt = mCloudsManager->GetCloudLayer(i)->GetOptionsDynm();

			xml.AddElem( "CloudLayer" );
			xml.AddAttrib( "Height", GetString("%f", opt.Height) );
			xml.AddAttrib( "Scale", GetString("%f", opt.Scale) );
			xml.AddAttrib( "WindDirection", GetString("(%f,%f)", opt.WindDirection.x, opt.WindDirection.y) );
			xml.AddAttrib( "TimeMultiplier", GetString("%f", opt.TimeMultiplier) );
			xml.AddAttrib( "DistanceAttenuation", GetString("%f", opt.DistanceAttenuation) );
			xml.AddAttrib( "DetailAttenuation", GetString("%f", opt.DetailAttenuation) );
			xml.AddAttrib( "NormalMultiplier", GetString("%f", opt.NormalMultiplier) );
			xml.AddAttrib( "HeightVolume", GetString("%f", opt.HeightVolume) );
			xml.AddAttrib( "VolumetricDisplacement", GetString("%f", opt.VolumetricDisplacement) );
		}
		xml.OutOfElem();
	}

	xml.OutOfElem();
	return true;
}