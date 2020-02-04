/********************************************************************
	created:	2009/12/21
	created:	21:12:2009   9:41
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\ParticleSystemManager.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	ParticleSystemManager
	file ext:	h
	author:		liangairan
	
	purpose:	����ϵͳ������
*********************************************************************/

#pragma once 

#include "ParticleSystem.h"
#include "Singleton.h"

class EngineExport CParticleSystemManager : public Singleton<CParticleSystemManager>
{
    BE_SINGLETON(CParticleSystemManager);
public:
    ~CParticleSystemManager();


    //��������ϵͳ
    CParticleSystem* CreateParticleSystem(const char* szName, const char* szFilename);

    //����ĳ������ϵͳ
    void DestroyParticleSystem(unsigned int nID);

    void DestroyParticleSystem(const AIRString& strName);

    void DestroyParticleSystem(CParticleSystem* pParticleSystem);

    //������������ϵͳ
    void UpdateAllParticleSystems(float fTimeElapsed);

    //�����������ϵͳ
    void Clear();

    //�������ϵͳ
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

    unsigned int m_nCounter;    //������
};

//#define PARTICLESYSTEM_MANAGER CParticleSystemManager::GetInstance()

