#pragma once 

#include "Common.h"
#include "Color.h"
#include "Camera.h"
#include "AABBBox3D.h"
//#include "IMaterial.h"
//#include "ShaderParameter.h"
#include "RenderData.h"
#include "IteratorWrappers.h"
#include "MarkupSTL.h"
#include "MemoryAlloter.h"
#include "BaseShape.h"
#include "EngineDefination.h"
#include "FixedString.h"
//#include "BoundingBoxEntity.h"
//#include "VertexDef.h"

class   CSceneNode;
class   IVertexBuffer;
class   CRenderQueue;

class	NodeAnimation;
class   CAIRAnimationState;

class   BoundingBoxEntity;

class   TextureAnimationTrack;
class   Primitive;
//class   CPass;
class   IMaterial;
class   CLight;
class   RenderContextQueue;
//class   CTechnique;
//enum RenderableType
//{
//    RT_ModelInstance,
//    RT_ParticleSystem,
//    RT_Other,
//};

class EngineExport Renderable// : public EngineAllocator(Renderable)
{
	EngineAllocator(Renderable)
public:
    bool IsRenderable()
    {
        return true;
    }
    friend class RenderControl;
    Renderable(); 

    //有名字的构造函数，一般是要放到场景每帧update的物件
    //Renderable(const char* szName);
    virtual ~Renderable();

    virtual const AIRString&  GetTypeString( void ) const; 

    virtual void Render(CCamera* pCamera, eRenderFlow flow = FLOW_SCENE);

    //从文件中加载对像 
    virtual bool LoadFromFile(const char* szFilename) 
    {
        return true;
    }

    u32 GetID() const
    {
        return m_nID;
    }

    void SetID(u32 nID)
    {
        m_nID = nID;
    }

    //清空对像
    virtual void Clear();

    virtual void SetMaterial(IMaterial* pMaterial);

    void SetName(const AIRString& strName);

    const AIRString&  GetName() const
    { 
        return m_strName; 
    } 

    const AIRString& GetResourceName() const
    {
        return m_strResourceName;
    }

    virtual const Matrix4f& GetWorldMatrix() const;
    

    virtual bool LoadFromXML(CMarkupSTL& xml);

    //获得材质
    virtual IMaterial* GetMaterial()
    {
        return m_pMaterial;
    }

    //获得中心点
    virtual const Vector3Df& GetCenter() const
    {
        return m_vtCenter;
    }

    //是否为透明
    virtual bool IsTransparent() const;

    //设置为透明
    virtual void SetTransparent(bool bAlpha = true, u8 nAlpha = 128);

    //设置当前的动画
    virtual void SetCurrentAnimation(const char* szAnimationName, float fBlendTime = 0.1f);

    //设置绑定的scenenode
    virtual void SetAttachedNode(CSceneNode* pNode);

    //获得Object Space的boundingbox
    const AABBBox3Df& GetBoundingBox() const
    {
        return m_boundingBox;
    }

    virtual const AABBBox3Df&   GetWorldBoundingBox(bool bTransform);

    //virtual void                SetRenderableType(RenderableType type);

    //RenderableType              GetRenderableType() const;

    // 设置是否显示包围盒
    void                        SetShowBoundingBox( bool b );

    bool                        GetShowBoundingBox( void ) const
    {
        return m_bShowBoundingBox;
    }

    //获得父亲节点
    virtual CSceneNode*         GetNodeParent( void )
    {
        return m_pNode;
    }

    //渲染队表中的更新
    virtual void                UpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue);

	// 父节点更新后,刷新渲染物信息
    //virtual void				RefreshByParentNode( void ) {}

    //设置是否要显示
    virtual void                SetVisable(bool bVisable);

    //被通知要移动
    virtual void                NotifyMove() {}

    //设置光照，注意，这里是固定管线的光照
    virtual void                EnableLights(bool bEnable);

    bool						IsVisable()
    {
        return m_bVisable;
    }

    //获得世界坐标系统中的位置
    const Vector3Df&			GetWorldPosition() const;

    //设置矩阵变换
    //virtual void  SetWorldTransform(CPass* pPass);

    //处理光照
    //virtual void                ProcessLights(CPass* pPass);

	virtual void				Reset( void ) {}

	virtual void				Update( float fTimeElapse ) {}

	//CRenderData*				GetRenderData( void ){ return &m_renderData; }

    //是否接受阴影
    virtual void AcceptShadowmap(bool bAccept) 
    {
        m_bAcceptShadow = bAccept;
    }

    virtual CAIRAnimationState*  GetCurrentNodeAnimationState()
    {
        return m_pCurNodeAnimState;
    }

    //生成渲染到阴影的technique，用于shadowmap
    //virtual void GenRenderToShadowTechnique();

    //渲染到GBuffer的technique，用于延迟渲染
    //virtual void GenRenderToGBufferTechnique() {}

    //渲染到阴影
    //virtual void RenderToShadowMap(CCamera* pCamera);

    //渲染到GBuffer，用于延迟渲染
    //virtual void RenderToGBuffer(CCamera* pCamera);

	//virtual void RenderToDepthMap(CCamera* pCamera, CPass* pPass);

    //生成渲染到深度图的technique
    //virtual void GenRenderToDepthMapTechnique();

    bool IsRenderToShadow() const
    {
        return m_bRenderToShadow;
    }

    virtual void SetRenderToShadow(bool bRenderToShadow)
    {
        m_bRenderToShadow = bRenderToShadow;
    }

    //判断是否接收阴影
    virtual bool IsAcceptShadowMap() const
    {
        return m_bAcceptShadow;
    }

    //是否接收延迟渲染的光照
    bool IsAcceptLighting() const
    {
        return m_bAcceptLighting;
    }

    virtual void SetAcceptLighting(bool bAccept)
    {
        m_bAcceptLighting = bAccept;
    }

    //virtual IVertexBuffer* GetVertexBuffer()
    //{
    //    return m_renderData.m_pVB;
    //}

    //virtual IIndexBuffer* GetIndexBuffer()
    //{
    //    return m_renderData.m_pIB;
    //}

    // 是否
    virtual bool IsPick(const Vector3Df& vOrg, const Vector3Df& vPickDir)
    {
        return IsPickByWorldBoundingBox(vOrg, vPickDir);
    }

    // 检查射线是否点中模型,如果点中,distFromEyeToObject记录最近击中点到射线起点的距离
    virtual bool IsPick(const Vector3Df& vOrg, const Vector3Df& vPickDir, float& distFromEyeToObject)
    {
        return IsPickByWorldBoundingBox(vOrg, vPickDir, distFromEyeToObject);
    }

    virtual bool IsPickByWorldBoundingBox(const Vector3Df& vOrg, const Vector3Df& vPickDir)
    {
		return m_WorldAABB.IntersectsWithLine(vOrg, vPickDir, float(0xfffffffe));
    }

	virtual bool IsPickByWorldBoundingBox(const Vector3Df& vOrg, const Vector3Df& vPickDir, float& distFromEyeToObject)
	{
		if ( m_WorldAABB.IntersectsWithLine(vOrg, vPickDir, float(0xfffffffe)) )
		{
			distFromEyeToObject = m_WorldAABB.GetCenter().GetDistanceFrom( vOrg );
			return true;
		}
		return false;
	}

    // 获得颜色
    const SColor& GetColorDiffuse() const { return m_colorDiffuse; }

    virtual void SetColorDiffuse(const SColor color);

	void    SetResourceName(const AIRString& strName)
	{
		m_strResourceName = strName;
	}

    u32 GetRenderGroupType() const
    {
        return m_renderGroupType;
    }

    //显示boundingbox
    void ShowBoundingBox();

	void RenderOcclusionBoundingBox();

    Primitive* GetPrimitive()
    {
        _GetPrimitive();
        return m_pPrimitive;
    }

	CRenderData* GetRenderData()
	{
		return &m_renderData;
	}

    //这里实际上是生成渲染数据
    virtual bool Prepare();

    bool IsPrepared() const
    {
        return m_bPrepared;
    }

    virtual void SetInstancingEnable(bool bInstancing) {}

    virtual bool IsInstancingEnable() const
    {
        return false;
    }

	//lbh
	void SetShape(BaseShape* pShape) { m_pShape = pShape ;}
	BaseShape* GetShape();

	/*get set collision flag
	*/
	void SetCollision(bool bCollision)
	{
		m_bCollision = bCollision;
	}

	bool GetCollision()
	{
		return m_bCollision;
	}

	//CVertexDeclaration* GetVertexDecl()
	//{
	//	return m_pVertexDecl;
	//}
	const FixedString& GetVertexDeclName() const
	{
		return m_strVertexDecl;
	}

    virtual void FillRenderContext(CCamera* pCamera, RenderContextQueue* pQueue, eRenderFlow flow);

	//virtual CVertexDeclaration* CreateVertexDecl(IGPUProgram* pVertexShader);
protected:
    // 渲染包围盒
    //void                        RenderBoundingBoxImpl( void );

    // 渲染前要做的事
    virtual void                PreRender(CCamera* pCamera) {}

	virtual void				AfterRender( void );

    //清空对象的实现
    virtual void                ClearRenderableImp() {};

    //初始化
    virtual bool                Initialize();

    //用户去实现的渲染
    //virtual void                RenderImpl(IRenderer* pRenderer);

    //添加到渲染队列要做的事情
    virtual void  OnUpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue);
protected:
	typedef AIRList<CLight*>								LightList;
	//typedef std::map<string, CShaderParameter*>				SHADERPARAM;
 //   typedef std::map<string, CShaderParameter*>::iterator	SHADERPARAM_ITER;

    IMaterial*                          m_pMaterial;            //材质
    CSceneNode*                         m_pNode;                //父结点

    Matrix4f                            m_matWorld;             //世界矩阵
    Vector3Df                           m_vtCenter;             //中心点
    AABBBox3Df                          m_boundingBox;          //Object space包围盒
    AABBBox3Df                          m_WorldAABB;            //World space包围盒
    //SHADERPARAM							m_mapShaderParam;       //shader需要的参数
    AIRString                           m_strName;              //名称
    AIRString                           m_strResourceName;      //资源名称，就是文件名
    bool                                m_bVisable;             //是否可见
    bool                                m_bShowBoundingBox;     //是否显示包围盒
    bool                                m_bInitialize;          //是否已经初始化
    bool                                m_bTransformByParent;   //是否要被父节点影响
    AIRString                           m_szType;               //类别名称
    CRenderData                         m_renderData;           //渲染数据
    LightList                           m_lstAffectedLights;    //影响该renderable的light，用于固定管线
    u32									m_nID;

    bool                                m_bAcceptShadow;        //是否接收shadowmap
    bool                                m_bRenderToShadow;      //是否要渲染到shadwomap
    bool                                m_bAcceptLighting;      //是否接收延迟渲染的光照

	BaseShape*                          m_pShape;               //碰撞框
    BoundingBoxEntity*                  m_boundingBoxEntity;    //渲染boundingbox的类
    SColor						        m_colorDiffuse;         // diffuse颜色

    Primitive*                          m_pPrimitive;       //渲染数据
	CVertexDeclaration*                 m_pVertexDecl;      //顶点声明

    bool                                m_bPrepared;
	bool                                m_bCollision;   //是否碰撞

	FixedString m_strVertexDecl;
public:
	bool								m_bIsCameraInAABB;

    //获得Primitive，需要每个子类重载，以后要变成纯虚函数
    virtual void _GetPrimitive() {};
private:


	//----------------------------------------------------
	//! add by hjj 2010-5-24 NodeAnimation implementation
public:
	typedef std::map<AIRString, NodeAnimation*>			NodeAnimationMap;
	typedef std::map<AIRString, CAIRAnimationState*>		NodeAnimationStateMap;

	// 创建动画对象
	virtual NodeAnimation*		CreateAnimation( const AIRString& szName, float fLength );

	// 获得动画对象
	virtual NodeAnimation*		GetAnimation( const AIRString& szName );

	// 是否存在动画
	virtual bool				HasAnimation(const AIRString& szName) const;

	// 销毁动画, 连同销毁相应的动画状态对象
	virtual bool				DestroyAnimation(const AIRString& name);

	// 销毁所有动画, 连同销毁所有的动画状态对象
	virtual void				DestroyAllAnimations(void);

	// 创建动画状态
	virtual CAIRAnimationState* CreateNodeAnimationState(const AIRString& animName);

	// 获得动画状态
	virtual CAIRAnimationState* GetNodeAnimationState(const AIRString& animName);

	// 是否存在动画状态
	virtual bool				HasAnimationState(const AIRString& name) const;

	// 销毁动画状态
	virtual void				DestroyAnimationState(const AIRString& name);

	// 销毁所有动画状态
	virtual void				DestroyAllAnimationStates(void);

	// 设置当前的动画
	virtual void				SetCurrentNodeAnimation( const AIRString& szName );

	// 每帧更新所有打开的动画
	virtual void				UpdateNodeAnimation( float fTimeElapse );

    //提交动画更新
    virtual void                CommitNodeAnimation();

    //添加结点动画
    virtual void                AddNodeAnimation(NodeAnimation* pAnimation);

	//重置当前的节点动画
	virtual void				ResetNodeAnimation( void );

    MapIterator<Renderable::NodeAnimationMap> GetNodeAnimationIterator()
    {
        return MapIterator<Renderable::NodeAnimationMap>(m_mapNodeAnimations);
    }

    //重新设置当前technique
    //virtual void  ResetCurrentTechnique() {}

	inline void SetForceRenderGroup( bool b )
	{
		m_bForceRenderGroup = b;
	}

	inline bool GetForceRenderGroup()
	{
		return m_bForceRenderGroup;
	}

    //
    //virtual void                SetTextureAnimation(TextureAnimationTrack* pAnimationTexture);

    //virtual void                UpdateTextureAnimation(float fTimeElapse);

    ////提交纹理动画
    //virtual void                CommitTextureAnimation();
protected:
	NodeAnimationMap			m_mapNodeAnimations;		// 动画列表
	NodeAnimationStateMap		m_mapNodeAnimationStates;	// 动画状态集
	CAIRAnimationState*			m_pCurNodeAnimState;		// 当前的动画

    
	//----------------------------------------------------
    u32  m_renderGroupType;    //这个变量决定了渲染物属于哪个渲染队列
    bool  m_bShareParentMaterial;     //是否共享材质
	bool m_bForceRenderGroup;

    //以下是一些高级特效的technique
protected:
    //CTechnique*			m_pTechniqueRenderToShadow;			//渲染到阴影的technique
    //CTechnique*         m_pTechniqueRenderToGBuffer;		//延迟渲染，渲染到GBuffer
    //CTechnique*			m_pTechniqueRenderToDepthMap;		//渲染到深度图
};






typedef Renderable ManualRenderObject;
typedef std::map< AIRString, ManualRenderObject* > MAP_MANUAL_RENDER_OBJECT;
class EngineExport ManualRenderObjectManager
{
private:
	ManualRenderObjectManager( void )
	{

	}

public:
	static ManualRenderObjectManager* GetSingleton( void )
	{
		static ManualRenderObjectManager inst;
		return &inst;
	}


	~ManualRenderObjectManager( void )
	{
		DestroyAllManualRenderObject();
	}

	ManualRenderObject* CreateManualRenderObject( const AIRString& szName )
	{
		ManualRenderObject* pObj = FindManualRenderObject( szName );
		if ( pObj )
		{
			return pObj;
		}
		pObj = new Renderable();
		pObj->SetName( szName );
		return pObj;
	}

	ManualRenderObject* FindManualRenderObject( const AIRString& szName )
	{
		MAP_MANUAL_RENDER_OBJECT::iterator it = m_mapManualRenderObjects.find( szName );
		if ( it != m_mapManualRenderObjects.end() )
		{
			return it->second;
		}
		return NULL;
	}

	bool RemoveAndDestroyManualRenderObject( const AIRString& szName )
	{
		MAP_MANUAL_RENDER_OBJECT::iterator it = m_mapManualRenderObjects.find( szName );
		if ( it != m_mapManualRenderObjects.end() )
		{
			it->second->Clear();
			SafeDelete( it->second );
			m_mapManualRenderObjects.erase( it );
			return true;
		}
		return false;
	}

	bool RemoveManualRenderObject( const AIRString& szName )
	{
		MAP_MANUAL_RENDER_OBJECT::iterator it = m_mapManualRenderObjects.find( szName );
		if ( it != m_mapManualRenderObjects.end() )
		{
			m_mapManualRenderObjects.erase( it );
			return true;
		}
		return false;
	}

	bool DestroyAllManualRenderObject( void )
	{
		MAP_MANUAL_RENDER_OBJECT::iterator itb = m_mapManualRenderObjects.begin(), ite = m_mapManualRenderObjects.end();
		for ( ; itb != ite; ++itb )
		{
			itb->second->Clear();
			SafeDelete( itb->second );
		}
		m_mapManualRenderObjects.clear();
		return true;
	}

protected:
	MAP_MANUAL_RENDER_OBJECT m_mapManualRenderObjects;

    
};


#define MANUAL_RENDER_OBJECT_MGR	(ManualRenderObjectManager::GetSingleton())