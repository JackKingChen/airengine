/********************************************************************
	created:	2010/11/20
	created:	20:11:2010   10:59
	filename: 	DecalManager
	author:		liangairan
	
	purpose:	decal管理器，负责decal的创建，销毁
*********************************************************************/

#pragma once 

#include "TerrainDecal.h"
#include "Singleton.h"

class EngineExport DecalManager : public Singleton<DecalManager>
{
    BE_SINGLETON(DecalManager);
public:
    typedef std::map<AIRString, TerrainDecal*> TerrainDecalMap;
    typedef TerrainDecalMap::iterator TerrainDecalMapIter;
public:
    ~DecalManager();

    TerrainDecal* CreateTerrainDecal(const char* szName, const char* szFilename);

    void DestroyTerrainDecal(TerrainDecal* pDecal);

    //获得decal
    TerrainDecal* GetTerrainDecal(const AIRString& strName);

    //清空所有
    void Clear();

    void AddTerrainDecal(TerrainDecal* pDecal);
protected:
private:
    TerrainDecalMap  m_mapTerrainDecals;
};

//#define DECAL_MANAGER DecalManager::GetInstance()
