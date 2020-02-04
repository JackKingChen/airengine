#include "stdafx.h"
#include "Atmosphere.h"
#include "WeatherManager.h"
#include "Camera.h"
#include "SkyDome.h"
#include "MathUtil.h"
//#include "WeatherManualGPUParamSource.h"
#include "CloudsManager.h"
#include "MoonObject.h"
#include "GPUVariant.h"

Atmosphere::Atmosphere(WeatherManager *s)
: mSkyX(s)
, mOptions(Options())
{
    m_struLightDir = "uLightDir";
    m_struCameraPos = "uCameraPos";
    m_struInvWaveLength = "uInvWaveLength";
    m_struInnerRadius = "uInnerRadius";
    m_struKrESun = "uKrESun";
    m_struKmESun = "uKmESun";
    m_struKr4PI = "uKr4PI";
    m_struKm4PI = "uKm4PI";
    m_struScale = "uScale";
    m_struScaleDepth = "uScaleDepth";
    m_struScaleOverScaleDepth = "uScaleOverScaleDepth";
    m_struG = "uG";
    m_struG2 = "uG2";
    m_strweatherColor = "weatherColor";
    m_struExposure = "uExposure";
    m_struTime = "uTime";
    m_strweatherMultiplier = "weatherMultiplier";
    m_struNumberOfSamples = "uNumberOfSamples";
    m_struSamples = "uSamples";
}

Atmosphere::~Atmosphere()
{
}

void Atmosphere::UpdateInternal()
{
	//AtmosphereManualGPUDataParamSource* pGPUParmSrc = mSkyX->GetAtmosphereGPUParamSrc();
    IMaterial* pMaterial = mSkyX->GetMeshManager()->GetMaterial();
    GPUVariant var;

	if (mSkyX->IsStarfieldEnabled())
	{
		//pGPUParmSrc->SetTime( mSkyX->_GetTimeOffset()*0.5f );
        var.SetFloat(mSkyX->_GetTimeOffset()*0.5f);
        pMaterial->SetMaterialParam(m_struTime, var);
	}

	//pGPUParmSrc->SetLightDir( -GetSunDirection() );
    var.SetVector3(-GetSunDirection());
    pMaterial->SetMaterialParam(m_struLightDir, var);
	mSkyX->GetMoonManager()->Update();


	float Scale = 1.0f / (mOptions.OuterRadius - mOptions.InnerRadius),
		ScaleDepth = (mOptions.OuterRadius - mOptions.InnerRadius) / 2.0f,
		ScaleOverScaleDepth = Scale / ScaleDepth;

	Vector3Df vPos = Vector3Df(0, mOptions.InnerRadius + (mOptions.OuterRadius-mOptions.InnerRadius)*mOptions.HeightPosition, 0);

	//pGPUParmSrc->SetInnerRadius( mOptions.InnerRadius );
	//pGPUParmSrc->SetCameraPos( vPos );
	//pGPUParmSrc->SetScale( Scale );
	//pGPUParmSrc->SetScaleDepth( ScaleDepth );
	//pGPUParmSrc->SetScaleOverScaleDepth( ScaleOverScaleDepth );


	vPos =  Vector3Df(0, mOptions.InnerRadius + (mOptions.OuterRadius-mOptions.InnerRadius)*mOptions.HeightPosition, 0);
	//pGPUParmSrc->SetCameraPos( vPos );

	float Kr4PI  = mOptions.RayleighMultiplier * 4.0f * AIR::PI,
		KrESun = mOptions.RayleighMultiplier * mOptions.SunIntensity;

	//pGPUParmSrc->SetKr4PI( Kr4PI );
	//pGPUParmSrc->SetKrESun( KrESun );

	float Km4PI  = mOptions.MieMultiplier * 4.0f * AIR::PI,
		KmESun = mOptions.MieMultiplier * mOptions.SunIntensity;

	//pGPUParmSrc->SetKm4PI( Km4PI );
	//pGPUParmSrc->SetKmESun( KmESun );
    var.SetFloat(Kr4PI);
    pMaterial->SetMaterialParam(m_struKr4PI, var);

    var.SetFloat(KrESun);
    pMaterial->SetMaterialParam(m_struKrESun, var);

    var.SetFloat(Km4PI);
    pMaterial->SetMaterialParam(m_struKm4PI, var);

    var.SetFloat(KmESun);
    pMaterial->SetMaterialParam(m_struKmESun, var);

	//pGPUParmSrc->SetNumberOfSamples( mOptions.NumberOfSamples );
	//pGPUParmSrc->SetSamples( f32(mOptions.NumberOfSamples) );
    var.SetInt(mOptions.NumberOfSamples);
    pMaterial->SetMaterialParam(m_struNumberOfSamples, var);

    var.SetFloat((float)mOptions.NumberOfSamples);
    pMaterial->SetMaterialParam(m_struSamples, var);


    var.SetVector3(vPos);
    pMaterial->SetMaterialParam(m_struCameraPos, var);

    var.SetFloat(mOptions.InnerRadius);
    pMaterial->SetMaterialParam(m_struInnerRadius, var);

    var.SetFloat(Scale);
    pMaterial->SetMaterialParam(m_struScale, var);

    var.SetFloat(ScaleDepth);
    pMaterial->SetMaterialParam(m_struScaleDepth, var);

    var.SetFloat(ScaleOverScaleDepth);
    pMaterial->SetMaterialParam(m_struScaleOverScaleDepth, var);

	Vector3Df invWaveLength = Vector3Df(
		1.0f / CMathUtil::Pow(mOptions.WaveLength.x, 4.0f),
		1.0f / CMathUtil::Pow(mOptions.WaveLength.y, 4.0f),
		1.0f / CMathUtil::Pow(mOptions.WaveLength.z, 4.0f) 	);

	//pGPUParmSrc->SetInvWaveLength( invWaveLength );
    var.SetVector3(invWaveLength);
    pMaterial->SetMaterialParam(m_struInvWaveLength, var);

	//pGPUParmSrc->SetG( mOptions.G );
	//pGPUParmSrc->SetG2( mOptions.G * mOptions.G );

    var.SetFloat(mOptions.G);
    pMaterial->SetMaterialParam(m_struG, var);

    var.SetFloat(mOptions.G * mOptions.G);
    pMaterial->SetMaterialParam(m_struG2, var);

	//pGPUParmSrc->SetExposure( mOptions.Exposure );
    var.SetFloat(mOptions.Exposure);
    pMaterial->SetMaterialParam(m_struExposure, var);

	//pGPUParmSrc->SetWeatherColor(mOptions.WeatherColor);
	//pGPUParmSrc->SetWeatherMultiplier(mOptions.WeatherMultiplier);

    var.SetVector3(mOptions.WeatherColor);
    pMaterial->SetMaterialParam(m_strweatherColor, var);

    var.SetFloat(mOptions.WeatherMultiplier);
    pMaterial->SetMaterialParam(m_strweatherMultiplier, var);
}

void Atmosphere::_Update(const Options& NewOptions, bool ForceToUpdateAll)
{
	//AtmosphereManualGPUDataParamSource* pGPUParmSrc = mSkyX->GetAtmosphereGPUParamSrc();

    Vector3Df cameraPos;
    Vector3Df invWaveLength;
    float Kr4PI = 0;
    float KrESun = 0;
    float Km4PI = 0;
    float KmESun = 0;
    float Scale = 0;
    float ScaleDepth = 0;
    float ScaleOverScaleDepth = 0;
    float fTime = 0;

    GPUVariant var;
    SkyDome* pSkyMesh = mSkyX->GetMeshManager();
    IMaterial* pMaterial = pSkyMesh->GetMaterial();

	if (NewOptions.Time != mOptions.Time || 
		NewOptions.EastPosition != mOptions.EastPosition ||
		ForceToUpdateAll)
	{
		mOptions.Time = NewOptions.Time;
		mOptions.EastPosition = NewOptions.EastPosition;

		if (mSkyX->IsStarfieldEnabled())
		{
            fTime = mSkyX->_GetTimeOffset() * 0.5f;
			//pGPUParmSrc->SetTime( fTime );

            var.SetFloat(fTime);
            pMaterial->SetMaterialParam(m_struTime, var);
		}

		//pGPUParmSrc->SetLightDir( -GetSunDirection() );
		mSkyX->GetMoonManager()->Update();

        var.SetVector3(-GetSunDirection());
        pMaterial->SetMaterialParam(m_struLightDir, var);
	}

	if (NewOptions.InnerRadius != mOptions.InnerRadius || 
		NewOptions.OuterRadius != mOptions.OuterRadius ||
		ForceToUpdateAll)
	{
		mOptions.InnerRadius = NewOptions.InnerRadius;
		mOptions.OuterRadius = NewOptions.OuterRadius;

		Scale = 1.0f / (mOptions.OuterRadius - mOptions.InnerRadius);
		ScaleDepth = (mOptions.OuterRadius - mOptions.InnerRadius) / 2.0f;
		ScaleOverScaleDepth = Scale / ScaleDepth;

		Vector3Df vPos = Vector3Df(0, mOptions.InnerRadius + (mOptions.OuterRadius-mOptions.InnerRadius)*mOptions.HeightPosition, 0);
        cameraPos = vPos;

		//pGPUParmSrc->SetInnerRadius( mOptions.InnerRadius );
		//pGPUParmSrc->SetCameraPos( vPos );
		//pGPUParmSrc->SetScale( Scale );
		//pGPUParmSrc->SetScaleDepth( ScaleDepth );
		//pGPUParmSrc->SetScaleOverScaleDepth( ScaleOverScaleDepth );

        var.SetVector3(cameraPos);
        pMaterial->SetMaterialParam(m_struCameraPos, var);

        

        var.SetFloat(mOptions.InnerRadius);
        pMaterial->SetMaterialParam(m_struInnerRadius, var);

        var.SetFloat(Scale);
        pMaterial->SetMaterialParam(m_struScale, var);

        var.SetFloat(ScaleDepth);
        pMaterial->SetMaterialParam(m_struScaleDepth, var);

        var.SetFloat(ScaleOverScaleDepth);
        pMaterial->SetMaterialParam(m_struScaleOverScaleDepth, var);
	}

	if (NewOptions.HeightPosition != mOptions.HeightPosition ||
		ForceToUpdateAll)
	{
		mOptions.HeightPosition = NewOptions.HeightPosition;

		Vector3Df vPos =  Vector3Df(0, mOptions.InnerRadius + (mOptions.OuterRadius-mOptions.InnerRadius)*mOptions.HeightPosition, 0);
		//pGPUParmSrc->SetCameraPos( vPos );
        cameraPos = vPos;

        var.SetVector3(cameraPos);
        pMaterial->SetMaterialParam(m_struCameraPos, var);
	}

	if (NewOptions.RayleighMultiplier != mOptions.RayleighMultiplier ||
		NewOptions.SunIntensity       != mOptions.SunIntensity       ||
		ForceToUpdateAll)
	{
		mOptions.RayleighMultiplier = NewOptions.RayleighMultiplier;

		Kr4PI  = mOptions.RayleighMultiplier * 4.0f * AIR::PI;
		KrESun = mOptions.RayleighMultiplier * mOptions.SunIntensity;

		//pGPUParmSrc->SetKr4PI( Kr4PI );
		//pGPUParmSrc->SetKrESun( KrESun );

        var.SetFloat(Kr4PI);
        pMaterial->SetMaterialParam(m_struKr4PI, var);

        var.SetFloat(KrESun);
        pMaterial->SetMaterialParam(m_struKrESun, var);
	}

	if (NewOptions.MieMultiplier != mOptions.MieMultiplier ||
		NewOptions.SunIntensity  != mOptions.SunIntensity  ||
		ForceToUpdateAll)
	{
		mOptions.MieMultiplier = NewOptions.MieMultiplier;

		Km4PI  = mOptions.MieMultiplier * 4.0f * AIR::PI;
		KmESun = mOptions.MieMultiplier * mOptions.SunIntensity;

		//pGPUParmSrc->SetKm4PI( Km4PI );
		//pGPUParmSrc->SetKmESun( KmESun );

        var.SetFloat(Km4PI);
        pMaterial->SetMaterialParam(m_struKm4PI, var);

        var.SetFloat(KmESun);
        pMaterial->SetMaterialParam(m_struKmESun, var);
	}

	if (NewOptions.NumberOfSamples != mOptions.NumberOfSamples ||
		ForceToUpdateAll)
	{
		mOptions.NumberOfSamples = NewOptions.NumberOfSamples;

		//pGPUParmSrc->SetNumberOfSamples( mOptions.NumberOfSamples );
		//pGPUParmSrc->SetSamples( f32(mOptions.NumberOfSamples) );

        var.SetInt(mOptions.NumberOfSamples);
        pMaterial->SetMaterialParam(m_struNumberOfSamples, var);

        var.SetFloat((float)mOptions.NumberOfSamples);
        pMaterial->SetMaterialParam(m_struSamples, var);
	}

	if (NewOptions.WaveLength != mOptions.WaveLength ||
		ForceToUpdateAll)
	{
		mOptions.WaveLength = NewOptions.WaveLength;

		Vector3Df val = Vector3Df(
			1.0f / CMathUtil::Pow(mOptions.WaveLength.x, 4.0f),
			1.0f / CMathUtil::Pow(mOptions.WaveLength.y, 4.0f),
			1.0f / CMathUtil::Pow(mOptions.WaveLength.z, 4.0f) 	);

        invWaveLength = val;

		//pGPUParmSrc->SetInvWaveLength( val );

        var.SetVector3(invWaveLength);
        pMaterial->SetMaterialParam(m_struInvWaveLength, var);
	}

	if (NewOptions.G != mOptions.G ||
		ForceToUpdateAll)
	{
		mOptions.G = NewOptions.G;

		//pGPUParmSrc->SetG( mOptions.G );
		//pGPUParmSrc->SetG2( mOptions.G * mOptions.G );

        var.SetFloat(mOptions.G);
        pMaterial->SetMaterialParam(m_struG, var);

        var.SetFloat(mOptions.G * mOptions.G);
        pMaterial->SetMaterialParam(m_struG2, var);
	}

	if ((NewOptions.Exposure != mOptions.Exposure ||
		ForceToUpdateAll) && 
		(mSkyX->GetLightingMode() == LM_LDR))
	{
		mOptions.Exposure = NewOptions.Exposure;

		//pGPUParmSrc->SetExposure( mOptions.Exposure );

        var.SetFloat(mOptions.Exposure);
        pMaterial->SetMaterialParam(m_struExposure, var);
	}

	mOptions.WeatherColor = NewOptions.WeatherColor;
	mOptions.WeatherMultiplier = NewOptions.WeatherMultiplier;


	mSkyX->GetCloudsManager()->UpdateAllCloudLayerInternalParams();
	//pGPUParmSrc->SetWeatherColor(mOptions.WeatherColor);
	//pGPUParmSrc->SetWeatherMultiplier(mOptions.WeatherMultiplier);
    

    var.SetVector3(mOptions.WeatherColor);
    pMaterial->SetMaterialParam(m_strweatherColor, var);

    var.SetFloat(mOptions.WeatherMultiplier);
    pMaterial->SetMaterialParam(m_strweatherMultiplier, var);
}

float Atmosphere::_Scale( float cos,  float uScaleDepth)
{
	float x = 1.0 - cos;
	return uScaleDepth * CMathUtil::Exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}

AIR::Vector3Df Atmosphere::GetColorAt(const AIR::Vector3Df& Direction)
{
	if (Direction.y<0)
	{
		return AIR::Vector3Df(0,0,0);
	}

	// Parameters
	double Scale = 1.0f / (mOptions.OuterRadius - mOptions.InnerRadius),
		ScaleDepth = (mOptions.OuterRadius - mOptions.InnerRadius) / 2.0f,
		ScaleOverScaleDepth = Scale / ScaleDepth,
		Kr4PI  = mOptions.RayleighMultiplier * 4.0f * AIR::PI,
		KrESun = mOptions.RayleighMultiplier * mOptions.SunIntensity,
		Km4PI  = mOptions.MieMultiplier * 4.0f * AIR::PI,
		KmESun = mOptions.MieMultiplier * mOptions.SunIntensity;

	// --- Start vertex program simulation ---
	AIR::Vector3Df
		uLightDir = -GetSunDirection(),
		v3Pos = Direction,
		uCameraPos = AIR::Vector3Df(0, mOptions.InnerRadius + (mOptions.OuterRadius-mOptions.InnerRadius)*mOptions.HeightPosition, 0),
		uInvWaveLength = AIR::Vector3Df(
		1.0f / CMathUtil::Pow(mOptions.WaveLength.x, 4.0f),
		1.0f / CMathUtil::Pow(mOptions.WaveLength.y, 4.0f),
		1.0f / CMathUtil::Pow(mOptions.WaveLength.z, 4.0f));

	// Get the ray from the camera to the vertex, and it's length (far point)
	v3Pos.y += mOptions.InnerRadius;
	AIR::Vector3Df v3Ray = v3Pos - uCameraPos;
	double fFar = v3Ray.GetLength();
	v3Ray /= fFar;

	// Calculate the ray's starting position, then calculate its scattering offset
	AIR::Vector3Df v3Start = uCameraPos;
	double fHeight = uCameraPos.y,
		fStartAngle = v3Ray.DotProduct(v3Start) / fHeight,
		fDepth = CMathUtil::Exp(ScaleOverScaleDepth * (mOptions.InnerRadius - uCameraPos.y)),
		fStartOffset = fDepth * _Scale(fStartAngle, ScaleDepth);

	// Init loop variables
	double fSampleLength = fFar /(double)mOptions.NumberOfSamples,
		fScaledLength = fSampleLength * Scale,
		fHeight_, fDepth_, fLightAngle, fCameraAngle, fScatter;
	AIR::Vector3Df v3SampleRay = v3Ray * fSampleLength,
		v3SamplePoint = v3Start + v3SampleRay * 0.5f,
		color, v3Attenuate;

	// Loop the ray
	for (int i = 0; i < mOptions.NumberOfSamples; i++)
	{
		fHeight_ = v3SamplePoint.GetLength();
		fDepth_ = CMathUtil::Exp(ScaleOverScaleDepth * (mOptions.InnerRadius-fHeight_));

		fLightAngle = uLightDir.DotProduct(v3SamplePoint) / fHeight_;
		fCameraAngle = v3Ray.DotProduct(v3SamplePoint) / fHeight_;

		fScatter = (fStartOffset + fDepth*(_Scale(fLightAngle, ScaleDepth) - _Scale(fCameraAngle, ScaleDepth)));

		v3Attenuate = AIR::Vector3Df(
			CMathUtil::Exp(-fScatter * (uInvWaveLength.x * Kr4PI + Km4PI)),
			CMathUtil::Exp(-fScatter * (uInvWaveLength.y * Kr4PI + Km4PI)),
			CMathUtil::Exp(-fScatter * (uInvWaveLength.z * Kr4PI + Km4PI)));

		// Accumulate color
		v3Attenuate *= (fDepth_ * fScaledLength);
		color += v3Attenuate;

		// Next sample point
		v3SamplePoint += v3SampleRay;
	}

	// Outputs
	AIR::Vector3Df oRayleighColor = color * (uInvWaveLength * KrESun),
		oMieColor      = color * KmESun,
		oDirection     = uCameraPos - v3Pos;

	// --- End vertex program simulation ---
	// --- Start fragment program simulation ---

	double cos = uLightDir.DotProduct(oDirection) / oDirection.GetLength(),
		cos2 = cos*cos,
		rayleighPhase = 0.75 * (1.0 + 0.5*cos2),
		g2 = mOptions.G*mOptions.G,
		miePhase = 1.5f * ((1.0f - g2) / (2.0f + g2)) * 
		(1.0f + cos2) / CMathUtil::Pow(1.0f + g2 - 2.0f * mOptions.G * cos, 1.5f);

	AIR::Vector3Df oColor;

	if (mSkyX->GetLightingMode() == LM_LDR)
	{
		oColor = AIR::Vector3Df(
			1 - CMathUtil::Exp(-mOptions.Exposure * (rayleighPhase * oRayleighColor.x + miePhase * oMieColor.x)),
			1 - CMathUtil::Exp(-mOptions.Exposure * (rayleighPhase * oRayleighColor.y + miePhase * oMieColor.y)),
			1 - CMathUtil::Exp(-mOptions.Exposure * (rayleighPhase * oRayleighColor.z + miePhase * oMieColor.z)));
	}
	else
	{
		oColor = rayleighPhase * oRayleighColor + miePhase * oMieColor;
	}

	// For night rendering
	oColor += AIR::Clamp<f32>(((1 - std::max(oColor.x, std::max(oColor.y, oColor.z))*10)), 0, 1) 
		* (AIR::Vector3Df(0.05, 0.05, 0.1)
		* (2-0.75f*AIR::Clamp<f32>(-uLightDir.y, 0, 1)) * CMathUtil::Pow(1-Direction.y, 3));

	// --- End fragment program simulation ---

	// Output color
	return oColor;
}

AIR::Vector3Df Atmosphere::GetSunDirection()
{
	// 24h day: 
	// 0______A(Sunrise)_______B(Sunset)______24
	//                     

	float y,
		X = mOptions.Time.x,
		A = mOptions.Time.y,
		B = mOptions.Time.z,
		AB  = A+24-B,
		AB_ = B-A,
		XB  = X+24-B;

	if( X == A )
	{
		return AIR::Vector3Df(-mOptions.EastPosition.x, 0, -mOptions.EastPosition.y);
	}
	else if ( X == B )
	{
		return AIR::Vector3Df(mOptions.EastPosition.x, 0, mOptions.EastPosition.y);
	}

	if (X<A || X>B)
	{
		if (X<A)
		{
			y = -XB / AB;
		}
		else
		{
			y = -(X-B) / AB;
		}

		if (y > -0.5f)
		{
			y *= 2;
		}
		else
		{
			y = -(1 + y)*2;
		}
	}
	else
	{
		y = (X-A)/(B-A);

		if (y < 0.5f)
		{
			y *= 2;
		}
		else
		{
			y = (1 - y)*2;
		}
	}

	AIR::Vector2Df East = mOptions.EastPosition;//mOptions.EastPosition.normalisedCopy();
	East.Normalize();

	if (X > A && X < B)
	{
		if (X > (A + AB_/2))
		{
			East = -East;
		}
	}
	else
	{
		if (X<A)
		{
			if (XB < (24-AB_)/2)
			{
				East = -East;
			}
		}
		else
		{
			if ((X-B) < (24-AB_)/2)
			{
				East = -East;
			}
		}
	}

	float ydeg = (AIR::PI/2)*y,
		//sn = AIR::asm_sin(ydeg),
		//cs = AIR::asm_cos(ydeg);
		sn = sin( ydeg ),
		cs = cos( ydeg );

	AIR::Vector3Df SPos = AIR::Vector3Df(East.x*cs, sn, East.y*cs);

	return -SPos;
}

AIR::Vector3Df Atmosphere::GetSunPosition()
{
	return mSkyX->GetCamera()->GetPosition() - GetSunDirection()*mSkyX->GetMeshManager()->GetSkydomeRadius();
}