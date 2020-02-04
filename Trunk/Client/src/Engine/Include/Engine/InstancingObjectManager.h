/********************************************************************
	created:	2010/07/31
	created:	31:7:2010   15:34
	filename: 	InstancingObjectManager.h
	author:		liangairan
	
	purpose:	Instancing���������
*********************************************************************/


#pragma once 
#include "MeshInstancingObject.h"
#include "Singleton.h"
class SpeedTree;

class EngineExport InstancingObjectManager : public Singleton<InstancingObjectManager>
{
    BE_SINGLETON(InstancingObjectManager);
public:
    ~InstancingObjectManager();

    InstancingObject*  CreateInstancingObject(CAIRSubMesh* pSubMesh);

    InstancingObject*  GetInstancingObject(CAIRSubMesh* pSubMesh);

    //InstancingObject*  CreateSpeedTreeInstancingObject(SpeedTree* pTreeRes);

    //InstancingObject*  GetSpeedTreeInstancingObject(SpeedTree* pTreeRes);

    void DestroyAllInstancingObjects();

    void UpdateAllInstancingObjects(float fTimeElapse);

    //����������Ⱦ���technique
    void ResetAllInstancingTechnique();
protected:

    typedef std::map<CAIRSubMesh*, InstancingObject*> InstancingList;
    typedef InstancingList::iterator InstancingListIter;

    InstancingList m_mapInstancingObjects;

    //��ЩҪ�ڵ������
    //typedef std::map<SpeedTree*, InstancingObject*> SpeedTreeInstancingList;
    //typedef SpeedTreeInstancingList::iterator SpeedTreeInstancingListIter;

    //SpeedTreeInstancingList m_mapSpeedTreeInstancingObjects;
         
};

#define INSTANCING_MANAGER InstancingObjectManager::GetInstance()
