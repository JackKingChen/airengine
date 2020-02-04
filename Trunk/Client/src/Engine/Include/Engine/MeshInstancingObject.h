/********************************************************************
	created:	2010/07/31
	created:	31:7:2010   11:44
	filename: 	MeshInstancingObject.h
	author:		liangairan
	
	purpose:	instancing���ݣ�����������Ⱦ��ͬ����Ͳ��ʵ�ģ��
*********************************************************************/

#pragma once 

#include "Renderable.h"
#include <set>

class CAIRSubMesh;
//class CModelInstance;
class CMeshInstance;
class InstancingObject;
class ITexture;

const int MaxInstancesNum = 50;    //����instance����

typedef struct tagVertexHardwareInstanceData
{
    Matrix4f  matTransform;
    //u32       color;
	CColorValue color;
}VERTEXHARDWARE_INSTANCEDATA, *LPVERTEXHARDWARE_INSTANCEDATA;

class EngineExport MeshInstancingObject : public Renderable
{
public:
    MeshInstancingObject(InstancingObject* pOwner, int nLod);
    MeshInstancingObject(InstancingObject* pOwner, int nLod, CAIRSubMesh* pMeshRes);
    virtual ~MeshInstancingObject();

    virtual void Render(CCamera* pCamera);

    //�����������������
    virtual bool Create(int nMaxInstanceNum);

    virtual u32 GetInstancesNum() const
    {
        return m_lstMeshInstances.size();
    }

    //int GetMaxInstancesNum() const
    //{
    //    return m_nMaxInstanceNum;
    //}

    CAIRSubMesh* GetSubMeshRes()
    {
        return m_pMeshRes;
    }

    virtual void Update(float fTimeElapse);

    void AddModelInstance(CMeshInstance* pInstance);

    //void AddMaxInstancesNum()
    //{
    //    m_nMaxInstanceNum++;
    //}

    virtual void Clear();

    //bool IsInRenderQueue(CRenderQueue* pQueue) const;

    /*
    void SetIsInRenderQueue(bool bInQueue)
    {
        m_bIsInRenderQueue = bInQueue;
    }
    */
    //�����������Ⱦ����
    //void AddRenderQueue(CRenderQueue* pQueue);

    virtual void GenRenderToShadowTechnique();

    //virtual void GenRenderToGBufferTechnique();

    //virtual void GenRenderToDepthMapTechnique();

    virtual void AcceptShadowmap(bool bAccept);

    virtual void SetAcceptLighting(bool bAccept);

    bool IsHardwareInstance() const
    {
        return m_bHardwareInstance;
    }

    //virtual void RenderToGBuffer(CCamera* pCamera);

    //virtual void RenderToShadowMap(CCamera* pCamera);

    //virtual void ResetCurrentTechnique();

    //virtual void DrawOneByOne();

    //CVertexDeclaration* GetVertexDeclaration() 
    //{
    //    return m_pHardwareVertexDecl;
    //}

    bool IsInRenderQueue() const
    {
        return m_bIsInRenderQueue;
    }

    void SetIsInRenderQueue(bool bInQueue)
    {
        m_bIsInRenderQueue = bInQueue;
    }
protected:
    virtual void PreRender(CCamera* pCamera);

    //virtual void ClearRenderableImp();
    virtual void _GetPrimitive();

    bool  m_bIsInRenderQueue;
private:
    CAIRSubMesh*  m_pMeshRes;

protected:
    typedef std::set<CMeshInstance*> ListMeshInstance;
    typedef ListMeshInstance::iterator ListMeshInstanceIter;

    //int   m_nMaxInstanceNum;   //ʵ������

    ListMeshInstance  m_lstMeshInstances;

    IVertexBuffer*  m_pInstanceDataVB;
    IIndexBuffer*   m_pInstanceDataIB;
    bool            m_bHardwareInstance;
    //bool            m_bIsInRenderQueue;
    //CRenderQueue*   m_pCurrentRenderQueue;
    //typedef std::list<CRenderQueue*>  RenderQueueList;
    //typedef RenderQueueList::iterator RenderQueueListIter;
    //RenderQueueList  m_lstRenderQueues;   //���ڵ���Ⱦ����

    //CVertexDeclaration* m_pHardwareVertexDecl;
    //CTechnique*         m_pTechniqueRenderToShadow;
    //CTechnique*         m_pTechniqueAcceptShadow;
    //CTechnique*         m_pTechniqueDeferredLighting;    //�ӳ���Ⱦtechnique
    //CTechnique*         m_pCurrentTechnique;    //��ǰ��technique
    //CTechnique*         m_pTechniqueRenderToGBuffer;    //�ӳ���Ⱦ����Ⱦ��GBuffer

    ITexture*           m_pMeshTexture;    //mesh������

    InstancingObject*   m_pOwner;
    int                 m_nLod;            //�����ڵ�lod��
};

class EngineExport InstancingObject
{
public:
    
public:
    InstancingObject();
    InstancingObject(CAIRSubMesh* pSubMesh);
    virtual ~InstancingObject();

    //���
    virtual void Clear();

    MeshInstancingObject* GetMeshInstancingObject(CRenderQueue* pQueue, int nLod);
    //{
    //    return m_vtMeshInstancings[nLod];
    //}

    void AddMaxInstancesNum()
    {
        m_nMaxInstancesNum++;
    }

    size_t GetMaxInstancesNum() const
    {
        return m_nMaxInstancesNum;
    }

    void DelMaxInstancesNum()
    {
        m_nMaxInstancesNum--;
        if ((int)m_nMaxInstancesNum < 0)
        {
            m_nMaxInstancesNum = 0;
        }
    }

    virtual void ResetCurrentTechnique();

    virtual void Update(f32 fTimeElapse);

    
protected:
    virtual void CreateMeshInstancings(CRenderQueue* pQueue);
    
    typedef AIRVector<MeshInstancingObject*> MeshInstancingObjectArray;    //����lod��������
    typedef MeshInstancingObjectArray::iterator MeshInstancingObjectArrayIter;

    typedef std::map<CRenderQueue*, MeshInstancingObjectArray> MeshInstancingObjectMap;
    typedef MeshInstancingObjectMap::iterator MeshInstancingObjectMapIter;
    MeshInstancingObjectMap m_mapMeshInstancingArray;
    //MeshInstancingObjectArray  m_vtMeshInstancings;

    size_t m_nMaxInstancesNum;   //���ֵ
private:
    CAIRSubMesh*  m_pMeshRes;
};
