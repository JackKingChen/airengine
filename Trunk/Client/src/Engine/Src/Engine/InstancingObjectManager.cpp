#include "stdafx.h"
#include "InstancingObjectManager.h"
//#include "SpeedTreeMeshInstancing.h"
//#include "SpeedTree.h"

InstancingObjectManager::InstancingObjectManager()
{

}

InstancingObjectManager::~InstancingObjectManager()
{

}

void InstancingObjectManager::DestroyAllInstancingObjects()
{
    InstancingListIter itEnd = m_mapInstancingObjects.end();
    for (InstancingListIter it = m_mapInstancingObjects.begin(); it != itEnd; ++it)
    {
        InstancingObject* pInstancing = it->second;
        pInstancing->Clear();
        SafeDelete(pInstancing);
    }

    m_mapInstancingObjects.clear();

    /*
    SpeedTreeInstancingListIter itTreeEnd = m_mapSpeedTreeInstancingObjects.end();
    for (SpeedTreeInstancingListIter it = m_mapSpeedTreeInstancingObjects.begin(); it != itTreeEnd; ++it)
    {
        InstancingObject* pInstancing = it->second;
        pInstancing->Clear();
        SafeDelete(pInstancing);
    }

    m_mapSpeedTreeInstancingObjects.clear();
    */
}

InstancingObject* InstancingObjectManager::CreateInstancingObject(CAIRSubMesh* pSubMesh)
{
    InstancingObject* pMeshObject = NULL;
    InstancingListIter it = m_mapInstancingObjects.find(pSubMesh);

    if (it != m_mapInstancingObjects.end())
    {
        pMeshObject = it->second;
        
    }
    else
    {
        pMeshObject = new InstancingObject(pSubMesh);
        m_mapInstancingObjects.insert(std::make_pair(pSubMesh, pMeshObject));
    }

    pMeshObject->AddMaxInstancesNum();
    return pMeshObject;
}

InstancingObject* InstancingObjectManager::GetInstancingObject(CAIRSubMesh* pSubMesh)
{
    InstancingListIter it = m_mapInstancingObjects.find(pSubMesh);

    if (it != m_mapInstancingObjects.end())
    {
        return it->second;
    }

    return NULL;
}
/*
InstancingObject* InstancingObjectManager::CreateSpeedTreeInstancingObject(SpeedTree* pTreeRes)
{
    InstancingObject* pInstancingObject = NULL;

    SpeedTreeInstancingListIter it = m_mapSpeedTreeInstancingObjects.find(pTreeRes);

    if (it != m_mapSpeedTreeInstancingObjects.end())
    {
        pInstancingObject = it->second;

    }
    else
    {
        pInstancingObject = new SpeedTreeInstancing(pTreeRes);
        m_mapSpeedTreeInstancingObjects.insert(std::make_pair(pTreeRes, pInstancingObject));
    }

    pInstancingObject->AddMaxInstancesNum();

    return pInstancingObject;
}

InstancingObject* InstancingObjectManager::GetSpeedTreeInstancingObject(SpeedTree* pTreeRes)
{
    SpeedTreeInstancingListIter it = m_mapSpeedTreeInstancingObjects.find(pTreeRes);

    if (it != m_mapSpeedTreeInstancingObjects.end())
    {
        return it->second;
    }

    return NULL;
}
*/

void InstancingObjectManager::UpdateAllInstancingObjects(float fTimeElapse)
{
    InstancingListIter itEnd = m_mapInstancingObjects.end();
    for (InstancingListIter it = m_mapInstancingObjects.begin(); it != itEnd; ++it)
    {
        InstancingObject* pInstancing = it->second;
        pInstancing->Update(fTimeElapse);
    }

    /*
    SpeedTreeInstancingListIter itTreeEnd = m_mapSpeedTreeInstancingObjects.end();
    for (SpeedTreeInstancingListIter it = m_mapSpeedTreeInstancingObjects.begin(); it != itTreeEnd; ++it)
    {
        InstancingObject* pInstancing = it->second;
        pInstancing->Update(fTimeElapse);
    }
    */
}

void InstancingObjectManager::ResetAllInstancingTechnique()
{
    InstancingListIter itEnd = m_mapInstancingObjects.end();
    for (InstancingListIter it = m_mapInstancingObjects.begin(); it != itEnd; ++it)
    {
        InstancingObject* pInstancing = it->second;
        pInstancing->ResetCurrentTechnique();
    }

    //SpeedTreeInstancingListIter itTreeEnd = m_mapSpeedTreeInstancingObjects.end();
    //for (SpeedTreeInstancingListIter it = m_mapSpeedTreeInstancingObjects.begin(); it != itTreeEnd; ++it)
    //{
    //    InstancingObject* pInstancing = it->second;
    //    pInstancing->ResetCurrentTechnique();
    //}
}
