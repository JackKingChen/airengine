/********************************************************************
	created:	2010/11/20
	created:	20:11:2010   10:59
	filename: 	DecalManager
	author:		liangairan
	
	purpose:	decal������������decal�Ĵ���������
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

    //���decal
    TerrainDecal* GetTerrainDecal(const AIRString& strName);

    //�������
    void Clear();

    void AddTerrainDecal(TerrainDecal* pDecal);
protected:
private:
    TerrainDecalMap  m_mapTerrainDecals;
};

//#define DECAL_MANAGER DecalManager::GetInstance()
