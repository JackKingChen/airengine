#include "stdafx.h"
#include "DecalManager.h"
#include "Terrain.h"

DecalManager::DecalManager()
{
    
}

DecalManager::~DecalManager()
{

}

TerrainDecal* DecalManager::CreateTerrainDecal(const char* szName, const char* szFilename)
{
    TerrainDecal* pDecal = GetTerrainDecal(szName);
    if (pDecal == NULL)
    {
        pDecal = new TerrainDecal();
        pDecal->SetName(szName);
        if (pDecal->LoadFromFile(szFilename))
        {
            m_mapTerrainDecals.insert(std::make_pair(pDecal->GetName(), pDecal));
        }
    }

    return pDecal;
}

void DecalManager::DestroyTerrainDecal(TerrainDecal* pDecal)
{
    TerrainDecalMapIter it = m_mapTerrainDecals.find(pDecal->GetName());
    if (it != m_mapTerrainDecals.end())
    {
        pDecal->Clear();
        SafeDelete(pDecal);
        m_mapTerrainDecals.erase(it);
    }
}

TerrainDecal* DecalManager::GetTerrainDecal(const AIRString& strName)
{
    TerrainDecalMapIter it = m_mapTerrainDecals.find(strName);
    if (it != m_mapTerrainDecals.end())
    {
        return it->second;
    }

    return NULL;
}

void DecalManager::Clear()
{
    TerrainDecalMapIter itEnd = m_mapTerrainDecals.end();
    for (TerrainDecalMapIter it = m_mapTerrainDecals.begin(); it != itEnd; ++it)
    {
        TerrainDecal* pDecal = it->second;
        pDecal->Clear();
        SafeDelete(pDecal);
    }

    m_mapTerrainDecals.clear();
}

void DecalManager::AddTerrainDecal(TerrainDecal* pDecal)
{
    m_mapTerrainDecals.insert(std::make_pair(pDecal->GetName(), pDecal));
}
