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

    //渲染场景
    //virtual void RenderScene();

	//执行场景遮蔽剔除
	virtual void ExecOcclusionCull();

    //清空场景
    virtual void ClearScene();

    //初始化场景，用户可以加载完场景后重载该函数
    virtual void InitScene() { }

	virtual void InitScene( int nLeftTopX, int nLeftTopY, int nWidth, int nHeight ) {}

    //更新场景
    virtual void UpdateScene();

    //从XML读取场景
    virtual bool LoadFromXML(const char* szFilename)
    {
        return true;
    }

    //增加渲染物
    //virtual void AddRenderableObject(Renderable* pRenderable) = 0;

    virtual void SetCamera(CCamera* pCamera);

    //获得摄像机
    virtual CCamera* GetCamera()
    {
        return m_pCamera;
    }

    //获得场景的太阳光
    virtual CLight* GetSunLight() { return &m_sunLight; }

    //获得雾的颜色
    virtual const CColorValue& GetFogColor() const 
    {
        return m_fogColor;
    }

    //创建Renderable
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

    //获得Skybox
    virtual Renderable* GetSkybox() { return NULL; };

    //处理消息 true不继续向下传递，false则继续向下传递
    virtual bool ProcessInputEvent(const LPINPUTEVENT pEvent) = 0;

    //添加场景结点
    virtual void AddSceneNode(CSceneNode* pNode) {}

    virtual CSceneNode* CreateSceneNode(const AIRString& strName, bool bMovable = false, const Vector3Df& vPosition = Vector3Df::ZERO, const Quaternion& qRotation = Quaternion(0, 0, 0, 1), const Vector3Df& vScale = Vector3Df::UNIT_SCALE);

    //virtual CActor* CreateActor(const AIRString& strName, const Vector3Df& vSrcDirection = Vector3Df::UNIT_Z, const Vector3Df& vPosition = Vector3Df::ZERO, const Quaternion& qRotation = Quaternion(0, 0, 0, 1), const Vector3Df& vScale = Vector3Df::UNIT_SCALE);
    
    //获得当前帧所有要渲染的物体的数量
    //virtual u32 GetRenderableObjectsNum() const;

    //virtual void AddRenderableObject(Renderable* pRenderable);

    //virtual void AddActor(CActor* pActor) = 0;
    //查找renderable
    virtual Renderable* FindRenderable(u32 nID);

    //销毁Renderable
    virtual void DestroyRenderable(Renderable* pRenderable, bool bAutoDeleteNode = true);

    //销毁所有的渲染物
    virtual void DestroyAllRenderable();

    //销毁场景结点
    virtual void DestroySceneNode(const AIRString& strName);

    virtual void DestroySceneNode(CSceneNode* pNode);

    //查找场景结点
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

    //创建光照
    virtual CLight* CreateLight(const char* szName);

    //创建影响该结点的光照列表
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

    //由pass调用的函数，setfogenable
    void SetFogEnable(bool bFogEnable);

    //获得环境光颜色
    const CColorValue& GetAmbientColor() const
    {
        return m_clrAmbient;
    }

    // 改变scenenode名字
    bool ChangeSceneNodeName(CSceneNode* pNode, const AIRString& strName);

	// 给一条射线,获得射线所穿过的场景节点
	LIST_SCENENODE& GetPickNodes( const Vector3Df& vOrg, const Vector3Df& vPickDir, bool bNeedArrange = false );

    // 根据摄像机获得渲染队列
    //virtual void  GetVisableRenderables(CCamera* pCamera, CRenderQueue& renderQueue);

    //获得在一定距离内的renderables
    //virtual void GetVisableRenderables(CCamera* pCamera, CRenderQueue& renderQueue, f32 fDistanceIn);

	//获取在一定范围内的碰撞体
	virtual void GetCollisionRenderables(const AABBBox3Df& aabbTest, AIRList<Renderable*>& renderList)  = 0;

	// 获得测试包围盒内的节点
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

    //是否开启shadowmap
    virtual void EnableShadowMap(bool bEnable);

    //是否开户阴影
    //bool  IsShadowmapEnable() const
    //{
    //    return m_bShadowmapEnable;
    //}

    //拾取导肮面
    bool FindPickNAVTriangle(const Line3Df& ray, NavTriangle& navTriOut);

    //设置固定管线光源开启
    void SetFixPipeLightingEnable(bool bEnable);


	//当前的雾参数记录到默认雾参数
	void CurFogToDefaultFog( void )
	{
		m_fogColorDefault = m_fogColor;
		m_fogModeDefault = m_fogMode;
		m_fDensityDefault = m_fDensity;
		m_fFogStartDefault = m_fFogStart;
		m_fFogEndDefault = m_fFogEnd;
	}

	//默认雾参数设置到当前的雾参数
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

    //加载好场景后对场景进行cache
    virtual void CacheScene() {}

    //重置所有渲染物的technique
    //void ResetAllRenderablesTechnique();

    MapIterator< MapRenderable > GetSceneRenderables()
    {
        return MapIterator<MapRenderable>(m_mapSceneRenderables);
    }

    const AIRString& GetPlantFile() const
    {
        return m_strPlantFile;
    }

    //把当前场景的可视物放到queue里面。
    //virtual void GetRenderQueue(CRenderQueue* pQueue);

    //用于编辑器，划分plant
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

    //场景裁剪
    virtual void Cull(CCamera* pCamera, AIRList<CSceneNode*>& lstVisableNodes, u32 nCullFlag);

	//获得
	const Vector3Df GetSunLightDirection()
	{
		return m_sunLight.GetDerivedDirection();
	}
protected:
	//创建结点实例
	virtual CSceneNode* CreateSceneNodeImp(const AIRString& strName, const Vector3Df& vPosition = Vector3Df::ZERO, const Quaternion& qRotation = Quaternion(0, 0, 0, 1), const Vector3Df& vScale = Vector3Df::UNIT_SCALE);

	//找到最近点的物体
	virtual Renderable* GetNearestPickObjectByFace(const Vector3Df& vOrg, const Vector3Df& vEnd);

	virtual Renderable* GetNearestPickObjectByBoundingBox(const Vector3Df& vOrg, const Vector3Df& vEnd);

	virtual CModelInstance* GetNearestPickModelByFace(const Vector3Df& vOrg, const Vector3Df& vEnd);

	virtual CModelInstance* GetNearestPickModelByBoundingBox(const Vector3Df& vOrg, const Vector3Df& vEnd);

    //子类只需要继承这个函数
    //virtual void Render(CCamera* pCamera);
    
    //获得可视范围的渲染物
    //virtual void GetVisableObjects();

    //virtual void GetVisableObjects(f32 fDistanceIn);

    //处理可视范围内的结点
    //virtual void ProcessVisableObjects();
    //virtual CActor*     CreateActorImp(const AIRString& strName, const Vector3Df& vSrcDirection = Vector3Df::UNIT_Z, const Vector3Df& vPosition = Vector3Df::ZERO, const Quaternion& qRotation = Quaternion(0, 0, 0, 1), const Vector3Df& vScale = Vector3Df::UNIT_SCALE);

	//virtual void CreateBoundingBoxVBImpl( void );

	//virtual void ClearBoundingBoxVBImpl( void );

    //渲染可视化队列
    //virtual void RenderVisableObjects(CCamera* pCamera);

    //渲染单一一个物体
    virtual void RenderSingleObject(CCamera* pCamera, Renderable* pRenderable);

    virtual void ClearRenderable(Renderable* pRenderable);

    //提交场景动画
    void ApplySceneAnimations(f32 fTimeElapse);

    //virtual void RenderObject(Renderable* pRenderable
    virtual bool CreatePlants(const char* szFilename);

    //创建物件后的回调函数
    virtual void OnSceneRenderableCreate(Renderable* pRenderable) {}

    virtual void OnSceneRenderableDestroy(Renderable* pRenderable) {}
protected:
    //typedef AIRList<Renderable*> RENDERABLE_LIST;
    //typedef RENDERABLE_LIST::iterator RENDERABLE_LIST_ITER;

    //RENDERABLE_LIST m_lstRenderable;   //渲染物对列（非模型和粒子）

    //静态场景结点，每帧不需要更新这里
    
    MAP_SCENENODE      m_mapSceneNodes; 
    //动态结点
    MAP_SCENENODE      m_mapMovableNodes;

    //LIST_SCENENODE     m_lstVisableNodes;

    //CRenderQueue       m_renderQueue;  //最终的渲染队列
    //RenderContextQueue m_renderContextQueue;

    //场景摄像机
    CCamera*           m_pCamera;      //

	//--------
    //当前雾参数
    CColorValue        m_fogColor;   //雾的颜色
    FogMode            m_fogMode;    //雾的模式
    float              m_fDensity;   //雾的密度
    float              m_fFogStart;  //雾的起始参数
    float              m_fFogEnd;    //雾的结束参数

	//保存默认的雾参数
	CColorValue        m_fogColorDefault;   //雾的颜色
	FogMode            m_fogModeDefault;    //雾的模式
	float              m_fDensityDefault;   //雾的密度
	float              m_fFogStartDefault;  //雾的起始参数
	float              m_fFogEndDefault;    //雾的结束参数
    //--------

    int                m_nVisableObjsNum;

    //IMaterial*  m_pBoundingBoxMaterial;

    RayQueryMask    m_SceneRayQueryMask;

	//IVertexBuffer*	m_pBoundingBoxVB;

    CColorValue     m_clrAmbient;  //环境光颜色，默认是白色

    CLight m_sunLight;     //太阳光，一般是唯一的

    //bool   m_bShadowmapEnable;    //是否开户shadowmap

    typedef AIRMap<AIRString, CLight*> LightMap;
    typedef LightMap::iterator LightMap_Iter;
    LightMap       m_mapLights;

    //CGPUDataParamSource       m_gpuDataParam;     //GPU参数更新列表

	bool m_bHasPickNodes;
	LIST_SCENENODE	m_lstPickNodes;

	LIST_SCENENODE	m_lstRangeNodes;

    AIRString m_strName;

	//ScenePickMode m_ScenePickMode;
    typedef AIRMap<AIRString, NodeAnimation*>			NodeAnimationMap;
    typedef AIRList<CAIRAnimationState*>		NodeAnimationStateList;
    typedef NodeAnimationMap::iterator NodeAnimationMapIter;
    typedef NodeAnimationStateList::iterator NodeAnimationStateListIter;
    NodeAnimationMap			m_mapNodeAnimations;		// 动画列表
    NodeAnimationStateList		m_lstNodeAnimationStates;	// 动画状态集

    FileData*  m_pResource;   //场景资源文件
    bool       m_bReady;      //是否已经创建完成

    
    typedef MapRenderable::iterator MapRenderableIter;
    MapRenderable m_mapSceneRenderables;

    u32 m_nRenderableIndex;

    AIRString m_strPlantFile;    //场景对应的植物文件
	bool m_bAttachPlantToTerrain;

	IOcclusionCull* m_pOcclCuller;
    //NormalSceneRenderProcess* m_pDefaultSceneRenderer;   //用于渲染场景的对象
public:
    static AIRMap<AIRString, SceneCreateRenderableFunc> s_mapSceneCreateRenderableFunc;
};
