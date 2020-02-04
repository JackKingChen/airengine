/********************************************************************
	created:	2009/12/21
	created:	21:12:2009   9:41
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\ParticleSystemManager.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	ParticleSystemManager
	file ext:	h
	author:		liangairan
	
	purpose:	粒子系统管理器
*********************************************************************/

#pragma once 

#include "ParticleSystem.h"
#include "Singleton.h"

class EngineExport CParticleSystemManager : public Singleton<CParticleSystemManager>
{
    BE_SINGLETON(CParticleSystemManager);
public:
    ~CParticleSystemManager();


    //创建粒子系统
    CParticleSystem* CreateParticleSystem(const char* szName, const char* szFilename);

    //销毁某个粒子系统
    void DestroyParticleSystem(unsigned int nID);

    void DestroyParticleSystem(const AIRString& strName);

    void DestroyParticleSystem(CParticleSystem* pParticleSystem);

    //更新所有粒子系统
    void UpdateAllParticleSystems(float fTimeElapsed);

    //清空所有粒子系统
    void Clear();

    //添加粒子系统
    void AddParticleSystem(CParticleSystem* pRenderable);


    CParticleSystem* GetParticleSystem(const AIRString& strName);

	CParticleSystem* GetParticleSystem(unsigned int nID);

    //ParticleSystemResource* CreateParticleSystemResource(const char* szFilename);

    //void ClearParticleSystemResource(ParticleSystemResource*& pResource);

    //void AddParticleSystemResource(ParticleSystemResource* pResource);
protected:
private:

    typedef AIRVector<CParticleSystem*> PARTICLESYSEM_ARRAY;
    typedef PARTICLESYSEM_ARRAY::iterator PARTICLESYSEM_ARRAY_ITER;
    PARTICLESYSEM_ARRAY    m_vtParticleSystem;

    //typedef std::map<AIRString, ParticleSystemResource*> ParticleSystemResourceMap;
    //typedef ParticleSystemResourceMap::iterator ParticleSystemResourceMapIter;

    //ParticleSystemResourceMap m_mapParticleSystemResource;

    unsigned int m_nCounter;    //计数器
};

//#define PARTICLESYSTEM_MANAGER CParticleSystemManager::GetInstance()

