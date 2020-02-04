#include "stdafx.h"
#include "RenderableFactory.h"
//#include "ParticleSystemManager.h"
//#include "ModelInstanceManager.h"
//#include "GEffectManager.h"
//#include "ElectroLightening.h"
//#include "DecalManager.h"
//#include "SpeedTreeInstManager.h"
#include "PageStaticGeometory.h"
#include "Water.h"
#include "ParticleSystem.h"
#include "ModelInstance.h"
//#include "SpeedTreeInstance.h"
#include "Terrain.h"
#include "TerrainDecal.h"
#include "Billboard.h"

Renderable* CreateModelInstance(const char* szName)
{
    return new CModelInstance();
}

Renderable* CreateParticleSystem(const char* szName)
{
    return new CParticleSystem();
}

Renderable* CreateRibbonTrail(const char* szName)
{
    return new RibbonTrail();
}

Renderable* CreateDecal(const char* szName)
{
    return new TerrainDecal();
}

Renderable* CreatePlant(const char* szName)
{
    return new PageStaticGeometory();
}

Renderable* CreateWater(const char* szName)
{
    return new CWater();
}

CRenderableFactory::CRenderableFactory()
{
    //RegisterRenderableCreateFunc("", CreateModelInstance);
}

CRenderableFactory::~CRenderableFactory()
{

}

void CRenderableFactory::RegisterRenderableCreateFunc(const char* szType, CreateRenderableFunc func)
{
    m_mapCreateFunc.insert(std::make_pair(szType, func));
}

Renderable* CRenderableFactory::CreateRenderable(const char* szName, const char* szType)
{
    if (strcmp(szType, "model") == 0)
    {
        return CreateModelInstance(szName);
    }
    else if (strcmp(szType, "particlesystem") == 0)
    {
        return CreateParticleSystem(szName);
    }
    else if (strcmp(szType, "ribbontrail") == 0)
    {
        return CreateRibbonTrail(szName);
    }
    else if (strcmp(szType, "TerrainDecal") == 0)
    {
        return CreateDecal(szName);
    }
    //else if (strcmp(szType, "speedtree") == 0)
    //{
    //    return CreateSpeedTreeInstance(szName);
    //}
    else if (strcmp(szType, "pagestaticgeometry") == 0)
    {
        return CreatePlant(szName);
    }
    else if (strcmp(szType, "water") == 0)
    {
        return CreateWater(szName);
    }
    else if (strcmp(szType, "billboard") == 0)
    {
        return new CBillboard();
    }
    std::map<AIRString, CreateRenderableFunc>::iterator it = m_mapCreateFunc.find(szType);
    if (it != m_mapCreateFunc.end())
    {
        return it->second(szName);
    }

    return NULL;
}

/*
Renderable* CRenderableFactory::CreateRenderable(const char* szName, const char* szFilename)
{
    const char* szFileExt = strstr(szFilename, ".");

    if (szFileExt == NULL)
    {
        return NULL;
    }

    Renderable* pRenderable = NULL;

    if (strcmp(szFileExt, ".airmesh") == 0)
    {
        return CreateModelInstance(szName, szFilename);
    }
    else if (strcmp(szFileExt, ".particle") == 0)
    {
        return CreateParticleSystem(szName, szFilename);
    }
	//else if (strcmp(szFileExt, ".effect") == 0)
	//{
	//	return CreateGEffect(szName, szFilename);
	//}
	else if ( strcmp(szFileExt, ".el") == 0 )
	{
		return CreateElectroLight(szName, szFilename);
	}
	else if ( strcmp(szFileExt, ".ribbontrail") == 0 )
	{
		return CreateRibbonTrail(szName, szFilename);
	}
    else if (strcmp(szFileExt, ".decal") == 0)
    {
        return CreateDecal(szName, szFilename);
    }
    else if (strcmp(szFileExt, ".spt") == 0)
    {
        return CreateSpeedTree(szName, szFilename);
    }

    return NULL;
}



Renderable* CRenderableFactory::FindRenderable( const char* szName, const char* szFilename )
{
	const char* szFileExt = strstr(szFilename, ".");

	if (szFileExt == NULL)
	{
		return NULL;
	}

	Renderable* pRenderable = NULL;

	if (strcmp(szFileExt, ".airmesh") == 0)
	{
		pRenderable = MODELINST_MANAGER->FindModelInstance( szName );
	}
	else if (strcmp(szFileExt, ".particle") == 0)
	{
		pRenderable = PARTICLESYSTEM_MANAGER->GetParticleSystem( szName );
	}
	//else if (strcmp(szFileExt, ".effect") == 0)
	//{
	//	pRenderable = GEFFECT_MANAGER->FindGEffect(szName);
	//}
	else if ( strcmp(szFileExt, ".el") == 0 )
	{
		pRenderable = ELECTROLIGHT_MANAGER->GetElectroLightening( szName );
	}
	else if ( strcmp(szFileExt, ".ribbontrail") == 0 )
	{
		pRenderable = RIBBONTRAIL_MANAGER->GetRibbonTrail( szName );
	}


	return pRenderable;
}



Renderable* CRenderableFactory::CreateModelInstance(const char* szName, const char* szFilename)
{
    //Renderable* pRenderable = NULL;

    //AIRModel* pModel = MODELMGR->CreateModel(szName);
    //if (pModel)
    //{
    //    pRenderable = AURO_NEW_VERBOSE CModelInstance(pModel);
    //}

    //return pRenderable;
    return MODELINST_MANAGER->CreateModelInstance(szName, szFilename);
}

Renderable* CRenderableFactory::CreateParticleSystem(const char* szName, const char* szFilename)
{
    return PARTICLESYSTEM_MANAGER->CreateParticleSystem(szName, szFilename);
}


Renderable* CRenderableFactory::CreateElectroLight(const char* szName, const char* szFilename)
{
	return ELECTROLIGHT_MANAGER->CreateElectroLightening(szName, szFilename);
}


Renderable* CRenderableFactory::CreateRibbonTrail(const char* szName, const char* szFilename)
{
	return RIBBONTRAIL_MANAGER->CreateRibbonTrail(szName, szFilename);
}

Renderable* CRenderableFactory::CreateDecal(const char* szName, const char* szFilename)
{
    return DECAL_MANAGER->CreateTerrainDecal(szName, szFilename);
}

Renderable* CRenderableFactory::CreateSpeedTree(const char* szName, const char* szFilename)
{
    return SPEEDTREEINST_MANAGER->CreateSpeedTreeInstance(szName, szFilename);
}
*/