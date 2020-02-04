/********************************************************************
	created:	3:3:2011   17:33
	filename: 	EngineResource.h
	author:		liangairan
	
	purpose:	资源加载系统
*********************************************************************/

#pragma once

#include "IReference.h"
#include <list>
#include <set>
#include "_Mutex.h"
#include "DebugTools.h"
#include "Singleton.h"
#include "MemoryAlloter.h"

class CDataStream;
class Resource;
class IOThread;

class EngineExport ResourceListener
{
public:
    ResourceListener() {}
    virtual ~ResourceListener() {}

    virtual void OnResourceLoaded(Resource* pResource) = 0;
    virtual void OnResourceReady(Resource* pResource) = 0;
protected:
private:
};

class EngineExport Resource : public IReference//, public EngineAllocator(Resource)
{
	EngineAllocator(Resource)
	DECLARE_RTTI;
public:
    enum ResourceType
    {
        Res_Model,             //模型资源
        Res_Skeleton,          //骨骼资源
        Res_VertexAnimation,   //顶点动画资源
        Res_AnimationMixer,    //动作混合资源
        Res_Material,
        Res_Texture,
        Res_GPUProgram,
        Res_ParticleSystem,
        Res_RibbonTrail,
        Res_SpeedTree,
        Res_NormalFile,
        Res_ImageSet,           //imageset文件，用于GUI方面
		Res_GEffect,
        Res_Other = 20,
    };

    enum ResourceStatus
    {
        ResStatus_UnLoaded,  //没加载
        ResStatus_Loading,   //加载中
        ResStatus_Loaded,    //加载完成
        ResStatus_LoadFailed,  //加载失败
    };
public:
    Resource();

    virtual ~Resource();

    //加载文件资源，IO操作，这里可以后台线程完成
    virtual bool Load();

    //数据操作，如创建纹理数据，创建顶点数据等。
    virtual bool Cache() = 0;

    const AIRString& GetFilename() const
    {
        return m_strFilename;
    }

    bool IsLoaded() const
    {
        return m_bLoaded;
    }

    bool IsReady() const
    {
        return m_bReady;
    }

    bool IsShare() const
    {
        return m_bShare;
    }

    void SetFilename(const char* szFilename);

    virtual bool LoadFromFile(const char* szFilename, bool bBackgroundThread = true);

    void SetIsReady(bool bReady)
    {
        m_bReady = bReady;
    }

    void AddListener(ResourceListener* pListener)
    {
        m_lstListeners.insert(pListener);
    }

    void NotifyAllListeners();

    void NotifyAllListenerReady();

    void SetIsLoaded(bool bLoaded)
    {
        m_bLoaded = bLoaded;
    }

    virtual void Clear() = 0;

    ResourceStatus GetStatus() const
    {
        return m_status;
    }

    virtual s32 Release();
protected:
    //virtual bool LoadImpl() = 0;
    bool m_bLoaded;   //是否加载完成（I/O操作）
    bool m_bReady;    //是否完全装好
    bool m_bShare;    //是否共享，默认为true，如果是false，则不共享，计数器没效

    AIRString  m_strFilename;  //文件名

    CDataStream*  m_pStream;   //文件流

    typedef std::set<ResourceListener*> ResourceListenerList;
    typedef ResourceListenerList::iterator ResourceListenerListIter;
    ResourceListenerList m_lstListeners;

    ResourceType  m_type;

    ResourceStatus  m_status;
private:
};






class EngineExport ResourceManager : public Singleton<ResourceManager>
{
    BE_SINGLETON(ResourceManager);
public:
    typedef std::set<Resource*>     ListResource;
    typedef ListResource::iterator  ListResourceIter;

    typedef std::map<AIRString, Resource*>    ResourceMap;
    typedef ResourceMap::iterator               ResourceMapIter;
public:
    ~ResourceManager();

    void AddLoadingResource(Resource* pResource);

    void RemoveLoadingResource(Resource* pResource);

    void LoadAllResources();

    template< class T >
    T* CreateResource(const char* szFilename)
    {
        ResourceMapIter it = m_mapResources.find(szFilename);
        if (it != m_mapResources.end())
        {
            it->second->AddRef();
            return static_cast<T*>(it->second);
        }

        it = m_mapGarbageResources.find(szFilename);
        if (it != m_mapGarbageResources.end())
        {
            T* pResource = static_cast<T*>(it->second);
            m_mapGarbageResources.erase(it);
            pResource->AddRef();
            return pResource;
        }
        T* pResource = /*( Resource* )*/( new T );
        if( pResource == NULL )
        {
            DT_TO_MSGBOX( LT_ERROR, "子类没有继承自 Resource " );
            return pResource;
        }    
        if (!CreateResourceImpl( szFilename, pResource ))
        {
            SafeDelete(pResource);
        }
        return pResource;
    }

    void ClearResource(Resource* pResource);

    //清空所有资源
    void DestroyAllResources();

    void AddResource(Resource* pResource);

    size_t GetLoadingResourcesNum();

    void DestroyResource(const char* szResource);

    void Update(f32 fTimeElapse);

    void ClearAllGarbageResources();

    //主线程调用
    void CheckLoadingStatus();

    void AddLoadedResource(Resource* pResource);
protected:

    bool CreateResourceImpl( const char* szFilename, Resource* pResource );

    //添加要回收的resource
    void AddGarbageResource(Resource* pResource);
private:

    ListResource m_lstResource;  //这个resourcelist要是线程安全的
    ListResource m_lstLoadedResoures;   //这个也必须是线程安全的
    _Mutex_Body_ m_sect;

    ResourceMap m_mapResources;

    ResourceMap m_mapGarbageResources;    //回收的资源

    IOThread*  m_pThread;

    f32 m_fTime;
};

#define RESOURCE_MANAGER ResourceManager::GetInstance()


