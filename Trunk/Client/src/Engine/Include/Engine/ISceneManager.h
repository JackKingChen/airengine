#pragma once 

#include "Common.h"
#include "Camera.h"
//#include "Renderable.h"
#include "Light.h"
#include "IMaterial.h"
#include "EventReader.h"
#include "Quaternion.h"
#include "RenderQueue.h"
#include "ColorValue.h"
#include "Color.h"
#include "RTTI.h"
//#include "RenderContext.h"
//#include "MemorySTLAllocator.h"

//#include "EngineDefination.h"
//#include "GPUDataParam.h"
//#include "Actor.h"

class Renderable;
class NavTriangle;
class CModelInstance;
class Resource;
class CHeightMap;
class IOcclusionCull;
class NormalSceneRenderProcess;

typedef AIRList<CSceneNode*>   LIST_SCENENODE;
typedef LIST_SCENENODE::iterator LIST_SCENENODE_ITER;

class ISceneManager;

typedef Renderable* (*SceneCreateRenderableFunc)(CMarkupSTL& xml, ISceneManager* pScene, AIRString& strFile);

enum ScenePickMode
{
	eSPM_BoundingBox,
	eSPM_Face,
};

enum RayQueryMask
{
    eRQM_SCENE          = 0x00000000,
    eRQM_STATIC_ACTOR   = 0x00000001,
    eRQM_MOVABLE_ACTOR  = 0x00000002, 
};

class EngineExport ISceneManager //: public EngineAllocator(ISceneManager)
{
	EngineAllocator(ISceneManager)
	DECLARE_ROOT_RTTI;
public:
    typedef AIRList<CLight*> LightList;

    typedef AIRMap<AIRString, CSceneNode*> MAP_SCENENODE;    
    typedef MAP_SCENENODE::iterator            SCENENODE_ITER;
    typedef AIRMap<u32, Renderable*> MapRenderable;
public:
    ISceneManager();
    virtual ~ISceneManager();

    //��Ⱦ����
    //virtual void RenderScene();

	//ִ�г����ڱ��޳�
	virtual void ExecOcclusionCull();

    //��ճ���
    virtual void ClearScene();

    //��ʼ���������û����Լ����곡�������ظú���
    virtual void InitScene() { }

	virtual void InitScene( int nLeftTopX, int nLeftTopY, int nWidth, int nHeight ) {}

    //���³���
    virtual void UpdateScene();

    //��XML��ȡ����
    virtual bool LoadFromXML(const char* szFilename)
    {
        return true;
    }

    //������Ⱦ��
    //virtual void AddRenderableObject(Renderable* pRenderable) = 0;

    virtual void SetCamera(CCamera* pCamera);

    //��������
    virtual CCamera* GetCamera()
    {
        return m_pCamera;
    }

    //��ó�����̫����
    virtual CLight* GetSunLight() { return &m_sunLight; }

    //��������ɫ
    virtual const CColorValue& GetFogColor() const 
    {
        return m_fogColor;
    }

    //����Renderable
    //virtual Renderable* CreateRenderable(const char* szName, const char* szType);

    //virtual Renderable* CreateRenderable(const char* szName, const char* szType, const char* szFilename);

    template <class T>
    T* CreateRenderable()
    {
        //T::IsRenderable();
        T* pRenderable = new T();
        pRenderable->SetID(m_nRenderableIndex++);
        pRenderable->IsRenderable();
        m_mapSceneRenderables.insert(std::make_pair(pRenderable->GetID(), pRenderable));
        OnSceneRenderableCreate(pRenderable);
        return pRenderable;
    }

    template <class T>
    T* CreateRenderable(const char* szFilename)
    {
        //T::IsRenderable();
        T* pRenderable = pRenderable = new T();
        pRenderable->IsRenderable();
        if (pRenderable->LoadFromFile(szFilename))
        {
            pRenderable->SetID(m_nRenderableIndex++);
            m_mapSceneRenderables.insert(std::make_pair(pRenderable->GetID(), pRenderable));
            OnSceneRenderableCreate(pRenderable);
        }
        else
        {
            pRenderable->Clear();
            delete pRenderable;
            pRenderable = NULL;
            return 0;
        }     
        
        return pRenderable;
    }

    //���Skybox
    virtual Renderable* GetSkybox() { return NULL; };

    //������Ϣ true���������´��ݣ�false��������´���
    virtual bool ProcessInputEvent(const LPINPUTEVENT pEvent) = 0;

    //��ӳ������
    virtual void AddSceneNode(CSceneNode* pNode) {}

    virtual CSceneNode* CreateSceneNode(const AIRString& strName, bool bMovable = false, const Vector3Df& vPosition = Vector3Df::ZERO, const Quaternion& qRotation = Quaternion(0, 0, 0, 1), const Vector3Df& vScale = Vector3Df::UNIT_SCALE);

    //virtual CActor* CreateActor(const AIRString& strName, const Vector3Df& vSrcDirection = Vector3Df::UNIT_Z, const Vector3Df& vPosition = Vector3Df::ZERO, const Quaternion& qRotation = Quaternion(0, 0, 0, 1), const Vector3Df& vScale = Vector3Df::UNIT_SCALE);
    
    //��õ�ǰ֡����Ҫ��Ⱦ�����������
    //virtual u32 GetRenderableObjectsNum() const;

    //virtual void AddRenderableObject(Renderable* pRenderable);

    //virtual void AddActor(CActor* pActor) = 0;
    //����renderable
    virtual Renderable* FindRenderable(u32 nID);

    //����Renderable
    virtual void DestroyRenderable(Renderable* pRenderable, bool bAutoDeleteNode = true);

    //�������е���Ⱦ��
    virtual void DestroyAllRenderable();

    //���ٳ������
    virtual void DestroySceneNode(const AIRString& strName);

    virtual void DestroySceneNode(CSceneNode* pNode);

    //���ҳ������
    virtual CSceneNode* FindSceneNode(const AIRString& strName);

    virtual CSceneNode* GetNearestPickNode(const Vector3Df& vOrg, const Vector3Df& vPickDir);

    //ListIterator<LIST_SCENENODE> GetVisibleSceneNodes()
    //{
    //    return ListIterator<LIST_SCENENODE>(m_lstVisableNodes);
    //}

    virtual void SetSceneRayQueryMask( RayQueryMask mask )
    {
        m_SceneRayQueryMask = mask;
    }

    virtual RayQueryMask GetSceneRayQueryMask( void )
    {
        return m_SceneRayQueryMask;
    }

	//virtual void RenderBoundingBoxImpl( Vector3Df* pVertexBuf );

    //��������
    virtual CLight* CreateLight(const char* szName);

    //����Ӱ��ý��Ĺ����б�
    //
    virtual void PopulateLightList(CSceneNode* pNode, LightList& destList, float fRadus);

    virtual void PopulateLightList(const Vector3Df& position, LightList& destList, float fRadus);

    void SetAmbientLight(const CColorValue& color);

    void SetFogColor(const CColorValue& color);

    void SetFogMode(FogMode mode);

    void SetFogStart(float fStart);

    void SetFogEnd(float fEnd);

    void SetFogDensity(float fDensity);

    float GetFogStart() const
    {
        return m_fFogStart;
    }

    float GetFogEnd() const
    {
        return m_fFogEnd;
    }

    float GetFogDensity() const
    {
        return m_fDensity;
    }

    FogMode GetFogMode() const
    {
        return m_fogMode;
    }

    //��pass���õĺ�����setfogenable
    void SetFogEnable(bool bFogEnable);

    //��û�������ɫ
    const CColorValue& GetAmbientColor() const
    {
        return m_clrAmbient;
    }

    // �ı�scenenode����
    bool ChangeSceneNodeName(CSceneNode* pNode, const AIRString& strName);

	// ��һ������,��������������ĳ����ڵ�
	LIST_SCENENODE& GetPickNodes( const Vector3Df& vOrg, const Vector3Df& vPickDir, bool bNeedArrange = false );

    // ��������������Ⱦ����
    //virtual void  GetVisableRenderables(CCamera* pCamera, CRenderQueue& renderQueue);

    //�����һ�������ڵ�renderables
    //virtual void GetVisableRenderables(CCamera* pCamera, CRenderQueue& renderQueue, f32 fDistanceIn);

	//��ȡ��һ����Χ�ڵ���ײ��
	virtual void GetCollisionRenderables(const AABBBox3Df& aabbTest, AIRList<Renderable*>& renderList)  = 0;

	// ��ò��԰�Χ���ڵĽڵ�
	LIST_SCENENODE& GetRangeNodes( const AABBBox3Df& rangeBoundingBox );

    const AIRString& GetName() const
    {
        return m_strName;
    }

    void SetName(const AIRString& strName)
    {
        m_strName = strName;
    }

    MapIterator<ISceneManager::MAP_SCENENODE> GetMapItorSceneNode( void )
    {
        return MapIterator<MAP_SCENENODE>( m_mapSceneNodes );
    }

    size_t GetSceneNodesNum() const
    {
        return m_mapSceneNodes.size();
    }

    //�Ƿ���shadowmap
    virtual void EnableShadowMap(bool bEnable);

    //�Ƿ񿪻���Ӱ
    //bool  IsShadowmapEnable() const
    //{
    //    return m_bShadowmapEnable;
    //}

    //ʰȡ������
    bool FindPickNAVTriangle(const Line3Df& ray, NavTriangle& navTriOut);

    //���ù̶����߹�Դ����
    void SetFixPipeLightingEnable(bool bEnable);


	//��ǰ���������¼��Ĭ�������
	void CurFogToDefaultFog( void )
	{
		m_fogColorDefault = m_fogColor;
		m_fogModeDefault = m_fogMode;
		m_fDensityDefault = m_fDensity;
		m_fFogStartDefault = m_fFogStart;
		m_fFogEndDefault = m_fFogEnd;
	}

	//Ĭ����������õ���ǰ�������
	void DefaultFogToCurFog( void )
	{
		m_fogColor = m_fogColorDefault;
		m_fogMode = m_fogModeDefault;
		m_fDensity = m_fDensityDefault;
		m_fFogStart = m_fFogStartDefault;
		m_fFogEnd = m_fFogEndDefault;
	}

	//void SetScenePickMode( ScenePickMode spm )
	//{
	//	m_ScenePickMode = spm;
	//}

	//ScenePickMode GetScenePickMode( void )
	//{
	//	return m_ScenePickMode;
	//}

	virtual Renderable* GetNearestPickObject( const Vector3Df& vOrg, const Vector3Df& vEnd, ScenePickMode spm = eSPM_Face );

	virtual CModelInstance* GetNearestPickModel( const Vector3Df& vOrg, const Vector3Df& vEnd, ScenePickMode spm = eSPM_Face  );

    void AddNodeAnimation(NodeAnimation* pAnimation);

    NodeAnimation* GetSceneNodeAnimation(const AIRString& strName);

    bool IsReady() const
    {
        return m_bReady;
    }

    Resource* GetResource()
    {
        return (Resource*)m_pResource;
    }

    //���غó�����Գ�������cache
    virtual void CacheScene() {}

    //����������Ⱦ���technique
    //void ResetAllRenderablesTechnique();

    MapIterator< MapRenderable > GetSceneRenderables()
    {
        return MapIterator<MapRenderable>(m_mapSceneRenderables);
    }

    const AIRString& GetPlantFile() const
    {
        return m_strPlantFile;
    }

    //�ѵ�ǰ�����Ŀ�����ŵ�queue���档
    //virtual void GetRenderQueue(CRenderQueue* pQueue);

    //���ڱ༭��������plant
    //void GenerateOptimiseTrunkPSG( int nRowCount, int nColCount );

	void SetAttachPlantToTerrain( bool b )
	{
		m_bAttachPlantToTerrain = b;
	}

	bool GetAttachPlantToTerrain( void )
	{
		return m_bAttachPlantToTerrain;
	}

    virtual CHeightMap* GetHeightMap()
    {
        return NULL;
    }

    static void RegistSceneCreateRenderableFunc(const AIRString& strName, SceneCreateRenderableFunc pFunc)
    {
        s_mapSceneCreateRenderableFunc.insert(std::make_pair(strName, pFunc));
    }

    //�����ü�
    virtual void Cull(CCamera* pCamera, AIRList<CSceneNode*>& lstVisableNodes, u32 nCullFlag);

	//���
	const Vector3Df GetSunLightDirection()
	{
		return m_sunLight.GetDerivedDirection();
	}
protected:
	//�������ʵ��
	virtual CSceneNode* CreateSceneNodeImp(const AIRString& strName, const Vector3Df& vPosition = Vector3Df::ZERO, const Quaternion& qRotation = Quaternion(0, 0, 0, 1), const Vector3Df& vScale = Vector3Df::UNIT_SCALE);

	//�ҵ�����������
	virtual Renderable* GetNearestPickObjectByFace(const Vector3Df& vOrg, const Vector3Df& vEnd);

	virtual Renderable* GetNearestPickObjectByBoundingBox(const Vector3Df& vOrg, const Vector3Df& vEnd);

	virtual CModelInstance* GetNearestPickModelByFace(const Vector3Df& vOrg, const Vector3Df& vEnd);

	virtual CModelInstance* GetNearestPickModelByBoundingBox(const Vector3Df& vOrg, const Vector3Df& vEnd);

    //����ֻ��Ҫ�̳��������
    //virtual void Render(CCamera* pCamera);
    
    //��ÿ��ӷ�Χ����Ⱦ��
    //virtual void GetVisableObjects();

    //virtual void GetVisableObjects(f32 fDistanceIn);

    //������ӷ�Χ�ڵĽ��
    //virtual void ProcessVisableObjects();
    //virtual CActor*     CreateActorImp(const AIRString& strName, const Vector3Df& vSrcDirection = Vector3Df::UNIT_Z, const Vector3Df& vPosition = Vector3Df::ZERO, const Quaternion& qRotation = Quaternion(0, 0, 0, 1), const Vector3Df& vScale = Vector3Df::UNIT_SCALE);

	//virtual void CreateBoundingBoxVBImpl( void );

	//virtual void ClearBoundingBoxVBImpl( void );

    //��Ⱦ���ӻ�����
    //virtual void RenderVisableObjects(CCamera* pCamera);

    //��Ⱦ��һһ������
    virtual void RenderSingleObject(CCamera* pCamera, Renderable* pRenderable);

    virtual void ClearRenderable(Renderable* pRenderable);

    //�ύ��������
    void ApplySceneAnimations(f32 fTimeElapse);

    //virtual void RenderObject(Renderable* pRenderable
    virtual bool CreatePlants(const char* szFilename);

    //���������Ļص�����
    virtual void OnSceneRenderableCreate(Renderable* pRenderable) {}

    virtual void OnSceneRenderableDestroy(Renderable* pRenderable) {}
protected:
    //typedef AIRList<Renderable*> RENDERABLE_LIST;
    //typedef RENDERABLE_LIST::iterator RENDERABLE_LIST_ITER;

    //RENDERABLE_LIST m_lstRenderable;   //��Ⱦ����У���ģ�ͺ����ӣ�

    //��̬������㣬ÿ֡����Ҫ��������
    
    MAP_SCENENODE      m_mapSceneNodes; 
    //��̬���
    MAP_SCENENODE      m_mapMovableNodes;

    //LIST_SCENENODE     m_lstVisableNodes;

    //CRenderQueue       m_renderQueue;  //���յ���Ⱦ����
    //RenderContextQueue m_renderContextQueue;

    //���������
    CCamera*           m_pCamera;      //

	//--------
    //��ǰ�����
    CColorValue        m_fogColor;   //�����ɫ
    FogMode            m_fogMode;    //���ģʽ
    float              m_fDensity;   //����ܶ�
    float              m_fFogStart;  //�����ʼ����
    float              m_fFogEnd;    //��Ľ�������

	//����Ĭ�ϵ������
	CColorValue        m_fogColorDefault;   //�����ɫ
	FogMode            m_fogModeDefault;    //���ģʽ
	float              m_fDensityDefault;   //����ܶ�
	float              m_fFogStartDefault;  //�����ʼ����
	float              m_fFogEndDefault;    //��Ľ�������
    //--------

    int                m_nVisableObjsNum;

    //IMaterial*  m_pBoundingBoxMaterial;

    RayQueryMask    m_SceneRayQueryMask;

	//IVertexBuffer*	m_pBoundingBoxVB;

    CColorValue     m_clrAmbient;  //��������ɫ��Ĭ���ǰ�ɫ

    CLight m_sunLight;     //̫���⣬һ����Ψһ��

    //bool   m_bShadowmapEnable;    //�Ƿ񿪻�shadowmap

    typedef AIRMap<AIRString, CLight*> LightMap;
    typedef LightMap::iterator LightMap_Iter;
    LightMap       m_mapLights;

    //CGPUDataParamSource       m_gpuDataParam;     //GPU���������б�

	bool m_bHasPickNodes;
	LIST_SCENENODE	m_lstPickNodes;

	LIST_SCENENODE	m_lstRangeNodes;

    AIRString m_strName;

	//ScenePickMode m_ScenePickMode;
    typedef AIRMap<AIRString, NodeAnimation*>			NodeAnimationMap;
    typedef AIRList<CAIRAnimationState*>		NodeAnimationStateList;
    typedef NodeAnimationMap::iterator NodeAnimationMapIter;
    typedef NodeAnimationStateList::iterator NodeAnimationStateListIter;
    NodeAnimationMap			m_mapNodeAnimations;		// �����б�
    NodeAnimationStateList		m_lstNodeAnimationStates;	// ����״̬��

    FileData*  m_pResource;   //������Դ�ļ�
    bool       m_bReady;      //�Ƿ��Ѿ��������

    
    typedef MapRenderable::iterator MapRenderableIter;
    MapRenderable m_mapSceneRenderables;

    u32 m_nRenderableIndex;

    AIRString m_strPlantFile;    //������Ӧ��ֲ���ļ�
	bool m_bAttachPlantToTerrain;

	IOcclusionCull* m_pOcclCuller;
    //NormalSceneRenderProcess* m_pDefaultSceneRenderer;   //������Ⱦ�����Ķ���
public:
    static AIRMap<AIRString, SceneCreateRenderableFunc> s_mapSceneCreateRenderableFunc;
};
