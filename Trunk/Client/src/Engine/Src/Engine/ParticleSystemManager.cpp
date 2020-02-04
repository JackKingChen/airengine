#include "stdafx.h"
#include "ParticleSystemManager.h"
#include "ParticleSystemResource.h"
#include "StringUtil.h"

CParticleSystemManager::CParticleSystemManager() : m_nCounter(0)
{
    //m_vtParticleSystem.reserve(100);
}

CParticleSystemManager::~CParticleSystemManager()
{
    Clear();
}

CParticleSystem* CParticleSystemManager::CreateParticleSystem(const char* szName, const char* szFilename)
{
    //CParticleSystem* pSystem = GetParticleSystem(szName);
    //if (pSystem != NULL)
    //{
    //    return pSystem;
    //}
    //pSystem = new CParticleSystem;
    //if (pSystem->LoadFromFile(szFilename))
    //{
    //    pSystem->SetName(szName);
    //    pSystem->SetID(m_nCounter++);
    //    AddParticleSystem(pSystem);
    //}
    //else 
    //{
    //    SafeDelete(pSystem);
    //}
    //return pSystem;


	CParticleSystem* pSystem = new CParticleSystem();
	if (pSystem->LoadFromFile(szFilename))
	{
		pSystem->SetName(szName);
		pSystem->SetID(m_nCounter++);
		m_vtParticleSystem.push_back(pSystem);

        pSystem->SetResourceName( CStringUtil::GetFilename(szFilename) );
	}
	else 
	{
		SafeDelete(pSystem);
	}
	return pSystem;
}

void CParticleSystemManager::DestroyParticleSystem(unsigned int nID)
{
    //CParticleSystem* pSystem = NULL;
    //ParticleSystemList_Iter itEnd = m_lstParticleSystem.end();
    //for (ParticleSystemList_Iter it = m_lstParticleSystem.begin(); it != itEnd; ++it)
    //{
    //    pSystem = it->second;
    //    if (pSystem->GetID() == nID)
    //    {
    //        pSystem->Clear();
    //        SafeDelete(pSystem);
    //        m_lstParticleSystem.erase(it);
    //        break;
    //    }
    //}

	CParticleSystem* pSystem = NULL;
	PARTICLESYSEM_ARRAY_ITER itEnd = m_vtParticleSystem.end();
	for (PARTICLESYSEM_ARRAY_ITER it = m_vtParticleSystem.begin(); it != itEnd; ++it)
	{
		pSystem = *it;
		if (pSystem->GetID() == nID)
		{
			pSystem->Clear();
			SafeDelete(pSystem);
			m_vtParticleSystem.erase(it);
			break;
		}
	}
}

void CParticleSystemManager::DestroyParticleSystem(const AIRString& strName)
{
	const AIRString pname = strName;
	int nFind = 0;
    CParticleSystem* pSystem = NULL;
    PARTICLESYSEM_ARRAY_ITER itEnd = m_vtParticleSystem.end();
    for (PARTICLESYSEM_ARRAY_ITER it = m_vtParticleSystem.begin(); it != itEnd; ++it)
    {
        pSystem = *it;
        if (pSystem->GetName() == strName)
        {
            pSystem->Clear();
            SafeDelete(pSystem);
            m_vtParticleSystem.erase(it);
			++nFind;
            break;
        }
    }

	if ( nFind == 0 )
	{
		DT_TO_DBGSTR( LT_WARN, GetString("************** did not find particlesystem [%s]\n", pname.c_str()) );
	}
	else
	{
		DT_TO_DBGSTR( LT_NONE, GetString("@@@@@@@@@@@@@@ destroy [%d] particlesystems [%s]\n", nFind, pname.c_str()) );
	}
}

void CParticleSystemManager::DestroyParticleSystem(CParticleSystem* pParticleSystem)
{
    CParticleSystem* pSystem = NULL;
    PARTICLESYSEM_ARRAY_ITER itEnd = m_vtParticleSystem.end();
    for (PARTICLESYSEM_ARRAY_ITER it = m_vtParticleSystem.begin(); it != itEnd; ++it)
    {
        pSystem = *it;
        if (pSystem == pParticleSystem)
        {
            pSystem->Clear();
            SafeDelete(pSystem);
            m_vtParticleSystem.erase(it);
            break;
        }
    }
}

void CParticleSystemManager::UpdateAllParticleSystems(float fTimeElapsed)
{
    //CParticleSystem* pSystem = NULL;
    //ParticleSystemList_Iter itEnd = m_lstParticleSystem.end();
    //for (ParticleSystemList_Iter it = m_lstParticleSystem.begin(); it != itEnd; ++it)
    //{
    //    pSystem = it->second;
    //    pSystem->Update(fTimeElapsed);
    //}

	CParticleSystem* pSystem = NULL;
	PARTICLESYSEM_ARRAY_ITER itEnd = m_vtParticleSystem.end();
	for (PARTICLESYSEM_ARRAY_ITER it = m_vtParticleSystem.begin(); it != itEnd; ++it)
	{
		pSystem = *it;
		pSystem->Update(fTimeElapsed);
	}
}

void CParticleSystemManager::Clear()
{
    //CParticleSystem* pSystem = NULL;
    //ParticleSystemList_Iter itEnd = m_lstParticleSystem.end();
    //for (ParticleSystemList_Iter it = m_lstParticleSystem.begin(); it != itEnd; ++it)
    //{
    //    pSystem = it->second;
    //    pSystem->Clear();
    //    SafeDelete(pSystem);
    //}
    //m_lstParticleSystem.clear();

	CParticleSystem* pSystem = NULL;
	PARTICLESYSEM_ARRAY_ITER itEnd = m_vtParticleSystem.end();
	for (PARTICLESYSEM_ARRAY_ITER it = m_vtParticleSystem.begin(); it != itEnd; ++it)
	{
		pSystem = *it;
		pSystem->Clear();
		SafeDelete(pSystem);
	}
	m_vtParticleSystem.clear();
}

void CParticleSystemManager::AddParticleSystem(CParticleSystem* pRenderable)
{
    //CParticleSystem* pExist = GetParticleSystem(pRenderable->GetName());
    //if (pExist == NULL)
    //{
    //    m_lstParticleSystem.insert(std::make_pair(pRenderable->GetName(), pRenderable));
    //}
    //else if (pExist != pRenderable)
    //{
    //    //弹出对话框，提示重名
    //    DT_TO_MSGBOX_FMT(LT_ERROR, "添加粒子系统有重名！");
    //    //assert(false);
    //}

	for (PARTICLESYSEM_ARRAY_ITER it = m_vtParticleSystem.begin(); it != m_vtParticleSystem.end(); ++it)
	{
		if (pRenderable == *it)
		{
			return;
		}
	}
	m_vtParticleSystem.push_back(pRenderable);
}

CParticleSystem* CParticleSystemManager::GetParticleSystem(const AIRString& strName)
{
    //ParticleSystemList_Iter it = m_lstParticleSystem.find(strName);
    //if (it != m_lstParticleSystem.end())
    //{
    //    return it->second;
    //}
    //return NULL;

    for (PARTICLESYSEM_ARRAY_ITER it = m_vtParticleSystem.begin(); it != m_vtParticleSystem.end(); ++it)
    {
        if ((*it)->GetName() == strName)
        {
            return *it;
        }
    }

    return NULL;
}


CParticleSystem* CParticleSystemManager::GetParticleSystem(unsigned int nID)
{
	//CParticleSystem* pSystem = NULL;
	//ParticleSystemList_Iter itEnd = m_lstParticleSystem.end();
	//for (ParticleSystemList_Iter it = m_lstParticleSystem.begin(); it != itEnd; ++it)
	//{
	//	if (it->second->GetID() == nID)
	//	{
	//		return *it;
	//	}
	//}

	for (PARTICLESYSEM_ARRAY_ITER it = m_vtParticleSystem.begin(); it != m_vtParticleSystem.end(); ++it)
	{
		if ((*it)->GetID() == nID)
		{
			return *it;
		}
	}

    return NULL;
}

/*
ParticleSystemResource* CParticleSystemManager::CreateParticleSystemResource(const char* szFilename)
{
    ParticleSystemResource* pResource = NULL;
    ParticleSystemResourceMapIter it = m_mapParticleSystemResource.find(szFilename);
    if (it != m_mapParticleSystemResource.end())
    {
        pResource = it->second;
        pResource->AddRef();
    }
    else
    {
        pResource = new ParticleSystemResource;
        pResource->LoadFromFile(szFilename);
        m_mapParticleSystemResource.insert(std::make_pair(pResource->GetFilename(), pResource));
    }

    return pResource;
}

void CParticleSystemManager::ClearParticleSystemResource(ParticleSystemResource*& pResource)
{
    if (pResource->Release() == 0)
    {
        ParticleSystemResourceMapIter it = m_mapParticleSystemResource.find(pResource->GetFilename());
        if (it != m_mapParticleSystemResource.end())
        {
            m_mapParticleSystemResource.erase(it);
        }
        SafeDelete(pResource);
    }
}

void CParticleSystemManager::AddParticleSystemResource(ParticleSystemResource* pResource)
{
    m_mapParticleSystemResource.insert(std::make_pair(pResource->GetFilename(), pResource));
}
*/
