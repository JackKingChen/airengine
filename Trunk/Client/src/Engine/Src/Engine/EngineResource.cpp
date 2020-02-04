#include "stdafx.h"
#include "EngineResource.h"
#include "FileSystem.h"
#include "RibbonTrail.h"
#include "ParticleSystemResource.h"
#include "AIRModel.h"
//#include "SpeedTree.h"
#include "AIRAnimationMixRes.h"
#include "Thread.h"
#include "FileData.h"
#include "ImageSet.h"

IMPLEMENT_ROOT_RTTI(IReference);
IMPLEMENT_RTTI(Resource, IReference);

Resource::Resource() : m_bLoaded(false)
, m_bReady(false)
, m_bShare(true)
, m_pStream(NULL)
, m_status(ResStatus_UnLoaded)
{

}

Resource::~Resource()
{
    SafeDelete(m_pStream);

	PrintDebugString( "Destruct resource [%s]\n", m_strFilename.c_str() );
}


bool Resource::Load()
{
    m_pStream = FILESYSTEM->GetFileStream(m_strFilename.c_str());

    m_bLoaded = (m_pStream != NULL);

    if (m_bLoaded)
    {
        //RESOURCE_MANAGER->AddResource(this);
        PrintDebugString("Load resource [%s] success！\n", m_strFilename.c_str());
        //NotifyAllListeners();
        m_status = ResStatus_Loaded;
    }
    else
    {
        PrintDebugString("Load resource [%s] failed！\n", m_strFilename.c_str());
        m_status = ResStatus_LoadFailed;
    }

    return m_bLoaded;
}

bool Resource::LoadFromFile(const char* szFilename, bool bBackgroundThread /* = false */)
{
    if (m_bLoaded)
    {
        return true;
    }
    SetFilename(szFilename);

    if (bBackgroundThread)
    {
        m_status = ResStatus_Loading;
        RESOURCE_MANAGER->AddLoadingResource(this);
    }
    else
    {
        return Load();
    }

    return true;
}

void Resource::SetFilename(const char* szFilename)
{
    m_strFilename = szFilename;
}

void Resource::NotifyAllListeners()
{
    ResourceListenerListIter itEnd = m_lstListeners.end();
    for (ResourceListenerListIter it = m_lstListeners.begin(); it != itEnd; ++it)
    {
        (*it)->OnResourceLoaded(this);
    }
}

void Resource::NotifyAllListenerReady()
{
    ResourceListenerListIter itEnd = m_lstListeners.end();
    for (ResourceListenerListIter it = m_lstListeners.begin(); it != itEnd; ++it)
    {
        (*it)->OnResourceReady(this);
    }
}

s32 Resource::Release()
{
    if (DropRef() == 0)
    {
        Clear();
    }

    return GetRef();
}

ResourceManager::ResourceManager() : m_pThread(NULL)
, m_fTime(0)
{
    if (ENGINE_INST->GetConfig()->m_bBackgroundThread)
    {
        m_pThread = new IOThread;
    }
    
}

ResourceManager::~ResourceManager()
{
    SafeDelete(m_pThread);
}

void ResourceManager::AddLoadingResource(Resource* pResource)
{
    
    _Mutex_Help _sc(&m_sect);
    m_lstResource.insert(pResource);
    m_pThread->SetEvent();
}

void ResourceManager::LoadAllResources()
{
    //加锁
    _Mutex_Help _sc(&m_sect);
    for (ListResourceIter it = m_lstResource.begin(); it != m_lstResource.end();)
    {
        (*it)->Load();
		ListResourceIter nit = ++it;
        //it = m_lstResource.erase(it);
		m_lstResource.erase(it);
		it = nit;
    }
    //m_lstResource.clear();
}


bool ResourceManager::CreateResourceImpl( const char* szFilename, Resource* pResource )
{
    if (!pResource->IsLoaded())
    {
        if (!pResource->LoadFromFile(szFilename, ENGINE_INST->GetConfig()->m_bBackgroundThread))
        {
            return false;
        }
        
    }
    AddResource(pResource);
    return true;
}

void ResourceManager::ClearResource(Resource* pResource)
{
    if (!pResource)
    {
        return;
    }
    if (pResource->GetStatus() == Resource::ResStatus_Loading)
    {
        //从加载列表中删除
        RemoveLoadingResource(pResource);
    }

    if (pResource->Release() == 0)
    {
        ResourceMapIter it = m_mapResources.find(pResource->GetFilename());
        if (it != m_mapResources.end())
        {
            m_mapResources.erase(it);
            SafeDelete(pResource);
        }
    }
    /*
    if (pResource->DropRef() == 0)
    {
        ResourceMapIter it = m_mapResources.find(pResource->GetFilename());
        if (it != m_mapResources.end())
        {
            m_mapResources.erase(it);
        }
        AddGarbageResource(pResource);
        //SafeDelete(pResource);
    }
    */
}

void ResourceManager::DestroyAllResources()
{
    ResourceMapIter itEnd = m_mapResources.end();
    for (ResourceMapIter it = m_mapResources.begin(); it != itEnd; ++it)
    {
        Resource* pResource = it->second;
        pResource->Clear();
        SafeDelete(pResource);
    }

    m_mapResources.clear();

    ClearAllGarbageResources();
}

void ResourceManager::AddResource(Resource* pResource)
{
    m_mapResources.insert(std::make_pair(pResource->GetFilename(), pResource));
}

size_t ResourceManager::GetLoadingResourcesNum()
{
    _Mutex_Help _sc(&m_sect);
    return m_lstResource.size();
}

void ResourceManager::DestroyResource(const char* szResource)
{
    ResourceMapIter it = m_mapResources.find(szResource);
    if (it != m_mapResources.end())
    {
        Resource* pResource = it->second;
        pResource->Clear();
        SafeDelete(pResource);
        m_mapResources.erase(it);
    }
}

void ResourceManager::AddGarbageResource(Resource* pResource)
{
    m_mapGarbageResources.insert(std::make_pair(pResource->GetFilename(), pResource));
}

void ResourceManager::Update(f32 fTimeElapse)
{
    CheckLoadingStatus();
    m_fTime += fTimeElapse;
    if (m_fTime >= 300)
    {
        ClearAllGarbageResources();
        m_fTime -= 300;
    }
}

void ResourceManager::ClearAllGarbageResources()
{
    ResourceMapIter itEnd = m_mapGarbageResources.end();

    for (ResourceMapIter it = m_mapGarbageResources.begin(); it != itEnd; ++it)
    {
        Resource* pResource = it->second;
        pResource->Clear();
        SafeDelete(pResource);
    }

    m_mapGarbageResources.clear();
}

void ResourceManager::RemoveLoadingResource(Resource* pResource)
{
    _Mutex_Help _sc(&m_sect);
    ListResourceIter it = std::find(m_lstResource.begin(), m_lstResource.end(), pResource);
    if (it != m_lstResource.end())
    {
        m_lstResource.erase(it);
    }
}

//这里是主线程调用，主要是通知相关的listener已经加载完毕
void ResourceManager::CheckLoadingStatus()
{
    _Mutex_Help _sc(&m_sect);
    for ( ListResourceIter it = m_lstLoadedResoures.begin(); it != m_lstLoadedResoures.end(); )
    {
        (*it)->NotifyAllListeners();

		ListResourceIter itn = it;
		++itn;
        m_lstLoadedResoures.erase(it);
		it = itn;
    }
}

void ResourceManager::AddLoadedResource(Resource* pResource)
{
    _Mutex_Help _sc(&m_sect);
    m_lstLoadedResoures.insert(pResource);
}
