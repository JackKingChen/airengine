#include "stdafx.h"
#include "VCloudsManager.h"
#include "WeatherManager.h"
#include "Atmosphere.h"

VCloudsManager::VCloudsManager(WeatherManager *s)
: mSkyX(s)
, mVClouds(0)
, mHeight(AIR::Vector2Df(-1, -1))
, mWindSpeed(800.0f)
, mAutoupdate(true)
, mCreated(false)
{
	mAmbientGradient = ColorGradient();
	mAmbientGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(1,1,1)*0.9f, 1.0f));
	mAmbientGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.7f,0.7f,0.65f), 0.625f)); 
	mAmbientGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.6f,0.55f,0.4f)*0.5f, 0.5625f));
	mAmbientGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.6f,0.55f,0.4f)*0.2f, 0.475f));
	mAmbientGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.6f,0.45f,0.3f)*0.1f, 0.5f));
	mAmbientGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.2f,0.2f,0.3f)*0.25f, 0.35f));
	mAmbientGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.2f,0.2f,0.3f)*0.3f, 0));

	mSunGradient = ColorGradient();
	mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(1,1,1) * 0.9f, 1.0f));
	mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(1,1,1) * 0.8f, 0.75f));
	mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.8f, 0.75f, 0.55f)*1.3f, 0.5625f));
	mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.6f, 0.5f, 0.2f) * 1.5f, 0.5f));
	mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.6f, 0.5f, 0.2f) * 0.2f, 0.4725f));
	mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.5f, 0.5f, 0.5f) * 0.05f, 0.45f));
	mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.25f, 0.25f, 0.25f), 0.3725f));
	mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.5f, 0.5f, 0.5f), 0.0f));
}

VCloudsManager::~VCloudsManager()
{
	remove();
}

void VCloudsManager::Create()
{
	if (mCreated)
	{
		return;
	}

	float radius = mSkyX->GetCamera()->GetViewFar();
	// Use default options if the user haven't set any specific Height parameters
	AIR::Vector2Df defaultheight = AIR::Vector2Df(radius*0.03f, radius*0.0525f); 
	AIR::Vector2Df height = (mHeight.x == -1 || mHeight.y == -1) ? defaultheight : mHeight;

	mVClouds = new VClouds(mSkyX->GetSceneManager(), mSkyX->GetCamera(), height, radius * 0.8f);
	mVClouds->setWheater(0.5, 1, 4);
	mVClouds->create();

	mCreated = true;

	_updateWindSpeedConfig();
}

void VCloudsManager::Update(f32 timeSinceLastFrame)
{
	if (!mCreated)
	{
		return;
	}

	_setLightParameters();

	mVClouds->update(timeSinceLastFrame);
}

void VCloudsManager::remove()
{
	if (!mCreated)
	{
		return;
	}

	delete mVClouds;

	mCreated = false;
}

void VCloudsManager::_setLightParameters()
{
	AIR::Vector3Df SunDir = mSkyX->GetAtmosphereManager()->GetSunDirection();
	if (SunDir.y > 0.1f)
	{
		SunDir = -SunDir;
	}

	mVClouds->setSunDirection(SunDir);

	float point = (-mSkyX->GetAtmosphereManager()->GetSunDirection().y + 1.0f) / 2.0f;

	mVClouds->setAmbientColor(mAmbientGradient.GetColor(point));
	mVClouds->setSunColor(mSunGradient.GetColor(point));
}

void VCloudsManager::_updateWindSpeedConfig()
{
	if (!mCreated)
	{
		return;
	}

	if (mAutoupdate)
	{
		mVClouds->setWindSpeed(mSkyX->GetTimeMultiplier() * mWindSpeed);
	}
	else
	{
		mVClouds->setWindSpeed(mWindSpeed);
	}
}