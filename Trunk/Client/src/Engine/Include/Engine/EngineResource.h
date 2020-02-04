/********************************************************************
	created:	3:3:2011   17:33
	filename: 	EngineResource.h
	author:		liangairan
	
	purpose:	��Դ����ϵͳ
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
        Res_Model,             //ģ����Դ
        Res_Skeleton,          //������Դ
        Res_VertexAnimation,   //���㶯����Դ
        Res_AnimationMixer,    //���������Դ
        Res_Material,
        Res_Texture,
        Res_GPUProgram,
        Res_ParticleSystem,
        Res_RibbonTrail,
        Res_SpeedTree,
        Res_NormalFile,
        Res_ImageSet,           //imageset�ļ�������GUI����
		Res_GEffect,
        Res_Other = 20,
    };

    enum ResourceStatus
    {
        ResStatus_UnLoaded,  //û����
        ResStatus_Loading,   //������
        ResStatus_Loaded,    //�������
        ResStatus_LoadFailed,  //����ʧ��
    };
public:
    Resource();

    virtual ~Resource();

    //�����ļ���Դ��IO������������Ժ�̨�߳����
    virtual bool Load();

    //���ݲ������紴���������ݣ������������ݵȡ�
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
    bool m_bLoaded;   //�Ƿ������ɣ�I/O������
    bool m_bReady;    //�Ƿ���ȫװ��
    bool m_bShare;    //�Ƿ���Ĭ��Ϊtrue�������false���򲻹���������ûЧ

    AIRString  m_strFilename;  //�ļ���

    CDataStream*  m_pStream;   //�ļ���

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
            DT_TO_MSGBOX( LT_ERROR, "����û�м̳��� Resource " );
            return pResource;
        }    
        if (!CreateResourceImpl( szFilename, pResource ))
        {
            SafeDelete(pResource);
        }
        return pResource;
    }

    void ClearResource(Resource* pResource);

    //���������Դ
    void DestroyAllResources();

    void AddResource(Resource* pResource);

    size_t GetLoadingResourcesNum();

    void DestroyResource(const char* szResource);

    void Update(f32 fTimeElapse);

    void ClearAllGarbageResources();

    //���̵߳���
    void CheckLoadingStatus();

    void AddLoadedResource(Resource* pResource);
protected:

    bool CreateResourceImpl( const char* szFilename, Resource* pResource );

    //���Ҫ���յ�resource
    void AddGarbageResource(Resource* pResource);
private:

    ListResource m_lstResource;  //���resourcelistҪ���̰߳�ȫ��
    ListResource m_lstLoadedResoures;   //���Ҳ�������̰߳�ȫ��
    _Mutex_Body_ m_sect;

    ResourceMap m_mapResources;

    ResourceMap m_mapGarbageResources;    //���յ���Դ

    IOThread*  m_pThread;

    f32 m_fTime;
};

#define RESOURCE_MANAGER ResourceManager::GetInstance()


