#include "stdafx.h"
#include "CloudsManager.h"
#include "WeatherManager.h"
//#include "Technique.h"
//#include "Pass.h"
//#include "WeatherManualGPUParamSource.h"
#include "MaterialManager.h"
#include "Primitive.h"
#include "Atmosphere.h"
#include "SkyDome.h"
//#include "WeatherTechniqueSelecter.h"
#include "RenderQueueGroup.h"
#include "GPUVariant.h"

/// -------------- CloudLayer -----------------
	CloudLayer::CloudLayer(WeatherManager *s)
		: mSkyX(s)
		, mOptions(Options())
        //, m_pGPUParmSrc(NULL)
	{
		m_szType = "CloudLayer";
		m_renderGroupType = eRGT_Skybox;
		m_bForceRenderGroup = true;

		mAmbientGradient = ColorGradient();
		mAmbientGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(1,1,1) * 0.95f, 1.0f));
		mAmbientGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.7f,0.7f,0.65f), 0.625f)); 
		mAmbientGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.6f,0.55f,0.4f), 0.5625f));
		mAmbientGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.6f,0.45f,0.3f)* 0.4f, 0.5f));
		mAmbientGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.5f,0.25f,0.25f)* 0.1f, 0.45f));
		mAmbientGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.2f,0.2f,0.3f)* 0.1, 0.35f));
		mAmbientGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.2f,0.2f,0.5f)* 0.15f, 0));

		mSunGradient = ColorGradient();
		mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(1,1,1)*0.95f, 1.0f));
		mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(1,1,1)*0.8f, 0.75f));
		mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.8f,0.75f,0.55f) * 1.3f, 0.5625f));
		mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.6f,0.5f,0.2f) * 0.75f, 0.5f));
		mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.6f,0.5f,0.2f) * 0.35f, 0.4725f));
		mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.5f,0.5f,0.5f) * 0.15f, 0.45f));
		mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.2f,0.2f,0.25f) * 0.5f, 0.3f));
		mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.5f,0.5f,0.5f) * 0.35f, 0.0f));

        InitFixedStrings();
	}

	CloudLayer::CloudLayer(WeatherManager *s, const Options& o)
		: mSkyX(s)
		, mOptions(o)
        //, m_pGPUParmSrc(NULL)
	{
		m_szType = "CloudLayer";
		m_renderGroupType = eRGT_Skybox;
		m_bForceRenderGroup = true;

		mAmbientGradient = ColorGradient();
		mAmbientGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(1,1,1) * 0.95f, 1.0f));
		mAmbientGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.7f, 0.7f, 0.65f), 0.625f)); 
		mAmbientGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.6f, 0.55f, 0.4f), 0.5625f));
		mAmbientGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.6f, 0.45f, 0.3f)*0.4, 0.5f));
		mAmbientGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.5f, 0.25f, 0.25f)*0.1, 0.45f));
		mAmbientGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.2f, 0.2f, 0.3f)* 0.1f, 0.35f));
		mAmbientGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.2f, 0.2f, 0.5f)* 0.15f, 0));

		mSunGradient = ColorGradient();
		mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(1, 1, 1) * 0.95f, 1.0f));
		mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(1, 1, 1) * 0.8f, 0.75f));
		mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.8f, 0.75f, 0.55f) * 1.3f, 0.5625f));
		mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.6f, 0.5f, 0.2f) * 0.75f, 0.5f));
		mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.6f, 0.5f, 0.2f) * 0.35f, 0.4725f));
		mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.5f, 0.5f, 0.5f) * 0.15f, 0.45f));
		mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.2f, 0.2f, 0.25f) * 0.5f, 0.3f));
		mSunGradient.AddCFrame(ColorGradient::ColorFrame(AIR::Vector3Df(0.5f, 0.5f, 0.5f) * 0.35f, 0.0f));

        InitFixedStrings();
	}

    void CloudLayer::InitFixedStrings()
    {
        m_struExposure = "uExposure";
        m_struSunPosition = "uSunPosition";
        m_struSunColor = "uSunColor";
        m_struHeight = "uHeight";
        m_struTime = "uTime";
        m_struScale = "uScale";
        m_struWindDirection = "uWindDirection";
        m_struCloudLayerHeightVolume = "uCloudLayerHeightVolume";
        m_struCloudLayerVolumetricDisplacement = "uCloudLayerVolumetricDisplacement";
        m_struAmbientLuminosity = "uAmbientLuminosity";
        m_struNormalMultiplier = "uNormalMultiplier";
        m_struDetailAttenuation = "uDetailAttenuation";
        m_struDistanceAttenuation = "uDistanceAttenuation";
        m_strweatherColor = "weatherColor";
        m_strweatherMultiplier = "weatherMultiplier";
		m_strVertexDecl = "VS_CLOUD";
    }

	CloudLayer::~CloudLayer()
	{
	}

	//CloudManualGPUDataParamSource* CloudLayer::GetCloudGPUParamSrc()
	//{
	//	return m_pGPUParmSrc;
	//}

	void CloudLayer::CreateCloudLayerImpl()
	{
		if ( !mSkyX->IsCreated() )
		{
			return;
		}
		SkyDome* pSkyDome = mSkyX->GetMeshManager();
		if ( !pSkyDome->IsCreated() )
		{
			return;
		}
		if ( !m_pMaterial )
		{
			m_pMaterial = MATERIAL_MANAGER->CreateMaterial( "WeatherSystemMaterial.xml/Cloud" );
		}
		if ( !m_pMaterial->IsReady() )
		{
			m_pMaterial->Cache();
		}
		//if ( !m_pGPUParmSrc )
		//{
		//	m_pGPUParmSrc = static_cast<CloudManualGPUDataParamSource*>(CREATE_MANUAL_GPU_PARAM_SOURCE/*_NOT_SHARE*/( CloudManualGPUDataParamSource ) );
		//}
		pSkyDome->GetNodeParent()->AttachRenderable( this );

	}


	void CloudLayer::_UpdateInternalPassParameters()
	{
        /*
		if ( !m_pGPUParmSrc )
		{
			return;
		}
		m_pGPUParmSrc->SetScale( mOptions.Scale );
		m_pGPUParmSrc->SetHeight( mOptions.Height );
		m_pGPUParmSrc->SetWindDirection( mOptions.WindDirection );
		m_pGPUParmSrc->SetCloudLayerHeightVolume( mOptions.HeightVolume );
		m_pGPUParmSrc->SetCloudLayerVolumetricDisplacement( mOptions.VolumetricDisplacement );
		m_pGPUParmSrc->SetNormalMultiplier( mOptions.NormalMultiplier );
		m_pGPUParmSrc->SetDetailAttenuation( mOptions.DetailAttenuation );
		m_pGPUParmSrc->SetDistanceAttenuation( mOptions.DistanceAttenuation );
		m_pGPUParmSrc->SetExposure( mSkyX->GetAtmosphereManager()->GetOptions().Exposure );
		m_pGPUParmSrc->SetTime( mSkyX->_GetTimeOffset()*mOptions.TimeMultiplier );
		Vector3Df SunDir = mSkyX->GetAtmosphereManager()->GetSunDirection();
		if (SunDir.y > 0.15f)
		{
			SunDir = -SunDir;
		}
		m_pGPUParmSrc->SetSunPosition( -SunDir*mSkyX->GetMeshManager()->GetSkydomeRadius() );
		float point = (-mSkyX->GetAtmosphereManager()->GetSunDirection().y + 1.0f) / 2.0f;
		m_pGPUParmSrc->SetSunColor( mSunGradient.GetColor(point) );
		m_pGPUParmSrc->SetAmbientLuminosity( mAmbientGradient.GetColor(point) );

		Atmosphere* pAtmo = WeatherManager::GetInstance()->GetAtmosphereManager();
		m_pGPUParmSrc->SetWeatherColor( pAtmo->GetOptions().WeatherColor );
		m_pGPUParmSrc->SetWeatherMultiplier( pAtmo->GetOptions().WeatherMultiplier );
        */
        GPUVariant var;
        var.SetFloat(mOptions.Scale);
        m_pMaterial->SetMaterialParam(m_struScale, var);

        var.SetFloat(mOptions.Height);
        m_pMaterial->SetMaterialParam(m_struHeight, var);

        var.SetVector2(mOptions.WindDirection);
        m_pMaterial->SetMaterialParam(m_struWindDirection, var);

        var.SetFloat(mOptions.HeightVolume);
        m_pMaterial->SetMaterialParam(m_struCloudLayerHeightVolume, var);

        var.SetFloat(mOptions.VolumetricDisplacement);
        m_pMaterial->SetMaterialParam(m_struCloudLayerVolumetricDisplacement, var);

        var.SetFloat(mOptions.NormalMultiplier);
        m_pMaterial->SetMaterialParam(m_struNormalMultiplier, var);

        var.SetFloat(mOptions.DetailAttenuation);
        m_pMaterial->SetMaterialParam(m_struDetailAttenuation, var);

        var.SetFloat(mOptions.DistanceAttenuation);
        m_pMaterial->SetMaterialParam(m_struDistanceAttenuation, var);

        var.SetFloat(mSkyX->GetAtmosphereManager()->GetOptions().Exposure);
        m_pMaterial->SetMaterialParam(m_struExposure, var);

        var.SetFloat(mSkyX->_GetTimeOffset()*mOptions.TimeMultiplier);
        m_pMaterial->SetMaterialParam(m_struTime, var);

        Vector3Df SunDir = mSkyX->GetAtmosphereManager()->GetSunDirection();
        if (SunDir.y > 0.15f)
        {
            SunDir = -SunDir;
        }

        var.SetVector3(-SunDir * mSkyX->GetMeshManager()->GetSkydomeRadius());
        m_pMaterial->SetMaterialParam(m_struSunPosition, var);

        float point = (-mSkyX->GetAtmosphereManager()->GetSunDirection().y + 1.0f) / 2.0f;
        var.SetVector3(mSunGradient.GetColor(point));
        m_pMaterial->SetMaterialParam(m_struSunColor, var);

        var.SetVector3(mAmbientGradient.GetColor(point));
        m_pMaterial->SetMaterialParam(m_struAmbientLuminosity, var);

        Atmosphere* pAtmo = WeatherManager::GetInstance()->GetAtmosphereManager();
        var.SetVector3(pAtmo->GetOptions().WeatherColor);
        m_pMaterial->SetMaterialParam(m_strweatherColor, var);

        var.SetFloat(pAtmo->GetOptions().WeatherMultiplier);
        m_pMaterial->SetMaterialParam(m_strweatherMultiplier, var);
	}



	bool CloudLayer::Prepare()
	{
		if ( !m_bPrepared )
		{
			if ( !m_pMaterial->IsReady() )
			{
				m_bPrepared = m_pMaterial->Cache();
			}
            else
            {
                m_bPrepared = true;
            }
            /*
			else
			{
				CTechnique* pCurTech = m_pMaterial->GetCurrentTechnique();
				CPass* pPass = pCurTech->GetPass( 0 );
				IGPUProgram* pProgram = pPass->GetVertexShader();
				//pProgram->AddManualGPUDataParamSource( m_pGPUParmSrc );
				pProgram = pPass->GetPixelShader();
				//pProgram->AddManualGPUDataParamSource( m_pGPUParmSrc );
				m_bPrepared = true;
			}
            */
		}
		return m_bPrepared;
	}

	void CloudLayer::Update( float fTimeElapse )
	{
		Prepare();
	}

	void CloudLayer::PreRender(CCamera* pCamera)
	{
		_UpdateInternalPassParameters();
	}

	void CloudLayer::_GetPrimitive()
	{
		if ( !mSkyX->IsCreated() )
		{
			return;
		}
		SkyDome* pSkyDome = mSkyX->GetMeshManager();
		if ( !pSkyDome->IsCreated() )
		{
			return;
		}
		CRenderData* pData = pSkyDome->GetRenderData();
		if (m_pPrimitive == NULL)
		{
			m_pPrimitive = new Primitive(Primitive::Primitive_TriangleList/*, pData->m_pVB->GetVertexDeclaration()*/);
			m_pPrimitive->m_pSrcRenderable = this;
			m_pPrimitive->SetUseUserPrimitive(true);
		}
		u32 nVertexCount = pData->m_pVB->GetVertexCount();
		u32 nFaceCount = pData->m_nPrimCount;
		m_pPrimitive->SetPrimitiveCount(nFaceCount);
		m_pPrimitive->SetVerticesNum(nVertexCount);
		m_pPrimitive->m_pIndexBuffer = pData->m_pIB;
		m_pPrimitive->m_pVertexBuffer = pData->m_pVB; 

		//UpdateInternalPassParameters();

		//CTechnique* pCurTech = m_pMaterial->GetCurrentTechnique();
		//CPass* pPass = pCurTech->GetPass( 0 );
		//IGPUProgram* pProgram = pPass->GetVertexShader();
		//pProgram->RemoveAllManualGPUDataParamSource();
		//pProgram->AddManualGPUDataParamSource( m_pGPUParmSrc );
		//pProgram = pPass->GetPixelShader();
		//pProgram->RemoveAllManualGPUDataParamSource();
		//pProgram->AddManualGPUDataParamSource( m_pGPUParmSrc );
	}

	void CloudLayer::Clear()
	{
		Renderable::Clear();
		if ( m_pPrimitive )
		{
			SafeDelete( m_pPrimitive );
		}
		//m_pGPUParmSrc = NULL;
	}

	/// ------------- CloudsManager ---------------
	CloudsManager::CloudsManager(WeatherManager *s)
		: mSkyX(s)
	{
        
	}

	CloudsManager::~CloudsManager()
	{
	}

	CloudLayer* CloudsManager::Add(const CloudLayer::Options& o)
	{
		CloudLayer *NewCloudLayer = new CloudLayer(mSkyX, o);

		NewCloudLayer->CreateCloudLayerImpl();

		mCloudLayers.push_back(NewCloudLayer);

		//bool changeOrder = false;

		//// Short layers by height
		//for (unsigned int k = 0; k < mCloudLayers.size(); k++)
		//{
		//	if (k+1 < mCloudLayers.size())
		//	{
		//		if (mCloudLayers.at(k)->GetOptions().Height < mCloudLayers.at(k+1)->GetOptions().Height)
		//		{
		//			// Swap
		//			CloudLayer* cl = mCloudLayers.at(k);
		//			mCloudLayers.at(k) = mCloudLayers.at(k+1);
		//			mCloudLayers.at(k+1) = cl;

		//			changeOrder = true;
		//			k = 0;
		//		}
		//	}
		//}

		return NewCloudLayer;
	}

	void CloudsManager::Remove(CloudLayer* cl)
	{
		for(CloudLayersIt = mCloudLayers.begin(); CloudLayersIt != mCloudLayers.end(); CloudLayersIt++)
        {
            if((*CloudLayersIt) == cl)
            {
				(*CloudLayersIt)->Clear();
				SafeDelete( *CloudLayersIt );
				mCloudLayers.erase( CloudLayersIt );
				break;
            }
		}
	}

	void CloudsManager::RemoveAll()
	{
		for(CloudLayersIt = mCloudLayers.begin(); CloudLayersIt != mCloudLayers.end(); CloudLayersIt++)
        {
			(*CloudLayersIt)->Clear();
			SafeDelete( *CloudLayersIt );
		}

		mCloudLayers.clear();
	}


	void CloudsManager::UpdateAllCloudLayerInternalParams()
	{
		//for(CloudLayersIt = mCloudLayers.begin(); CloudLayersIt != mCloudLayers.end(); CloudLayersIt++)
		//{
		//	(*CloudLayersIt)->_UpdateInternalPassParameters();
		//}
	}

	void CloudsManager::AutoUpdateTechnique()
	{
		for(CloudLayersIt = mCloudLayers.begin(); CloudLayersIt != mCloudLayers.end(); CloudLayersIt++)
		{
			//WeatherTechniqueSelecter::SelectTechnique( *CloudLayersIt );
		}
	}