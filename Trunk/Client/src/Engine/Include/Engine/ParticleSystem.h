/********************************************************************
	created:	2009/11/28
	created:	28:11:2009   16:42
	filename: 	d:\My Codes\Engine\Engine\Include\ParticleSystem.h
	file path:	d:\My Codes\Engine\Engine\Include
	file base:	ParticleSystem
	file ext:	h
	author:		liangairan
	
	purpose:	粒子系统
*********************************************************************/

#pragma once

#include "Renderable.h"
#include "IVertexBuffer.h"
#include "IIndexBuffer.h"
#include "ParticleEmitter.h"
#include "ParticleAffector.h"
#include "ParticleIterator.h"
//#include "VertexDef.h"
#include "RibbonTrail.h"
//#include "BillboardSet.h"
#include "IAnimatedObject.h"
#include "IteratorWrappers.h"
//#include "ParticleSystemResource.h"

///** The type of billboard to use. */
//enum BillboardType
//{
//    //面垂直摄像机的方向
//    BBT_PERPENDICULAR_CAMERA,
//
//    //面平衡于Y轴
//    BBT_HORIZON_Y,
//
//    //面垂直于Y轴
//    BBT_VERTICAL_Y,
//};

//粒子系统显示类型
/*
enum ParticleSystemType
{
	ePST_NONE = 0,	//无
	ePST_QUAD = 1,	//矩形面片
	ePST_OBJ  = 2,	//渲染物(模型,其他粒子系统等)
};

enum BillboardType
{
    //始终面向摄像机
    BBT_POINT,

    //平行于公共方向(m_vCommonDir)并朝向摄像机
    BBT_HORZ_COMMON_DIR_FACE_CAM,

    //平行于公共方向(m_vCommonDir)并朝向公共上方向(m_vCommonUpDir)
    BBT_HORZ_COMMON_DIR_FACE_COMMON_UP_DIR,

    //垂直于公共方向(m_vCommonDir)并朝向摄像机
    BBT_VERT_COMMON_DIR_FACE_CAM,

    //垂直于公共方向(m_vCommonDir)并朝向公共上方向(m_vCommonUpDir)
    BBT_VERT_COMMON_DIR_FACE_COMMON_UP_DIR,

    //平行于自己的运动方向(pParticle->m_direction)并朝向摄像机
    BBT_HORZ_SELF_DIR_FACE_CAM,

    //平行于自己的运动方向(pParticle->m_direction)并朝向公共上方向(m_vCommonUpDir)
    BBT_HORZ_SELF_DIR_FACE_COMMON_UP_DIR,

    //垂直于自己的运动方向(pParticle->m_direction)并朝向摄像机
    BBT_VERT_SELF_DIR_FACE_CAM,

    //垂直于自己的运动方向(pParticle->m_direction)并朝向公共上方向(m_vCommonUpDir)
    BBT_VERT_SELF_DIR_FACE_COMMON_UP_DIR,
};
*/

class ParticleSystemResource;
class BillboardSet;

class EngineExport CParticleSystem : public Renderable, public IAnimatedObject
{
public:
    typedef AIRVector<CParticleEmitter*>			VECTOR_EMITTER;
    typedef VECTOR_EMITTER::iterator				VECTOR_EMITTER_ITER;
    typedef AIRVector<CParticleAffector*>			VECTOR_AFFECTOR;
    typedef VECTOR_AFFECTOR::iterator				VECTOR_AFFECTOR_ITER;
    typedef AIRVector<CParticle*>					VECTOR_PARTICLE;
    typedef VECTOR_PARTICLE::iterator				VECTOR_PARTICLE_ITER;
    typedef AIRList<CParticle*>					PARTICLE_LIST;
    typedef PARTICLE_LIST::iterator					PARTICLE_LIST_ITER;
    typedef AIRList<CParticleEmitter*>			LIST_EMITTER;
    typedef LIST_EMITTER::iterator					LIST_EMITTER_ITER;

    //用于被发射的发射器
    /*
    typedef std::map<AIRString, VECTOR_EMITTER>	MAP_EMITTER_POOL;
    typedef std::map<AIRString, LIST_EMITTER>		MAP_FREE_EMITTER;
    typedef MAP_EMITTER_POOL::iterator				EMITTER_POOL_ITER;
    typedef MAP_FREE_EMITTER::iterator				FREE_EMITTER_ITER;
    */
    static BillboardSet* m_pBillboardSet;   //所有粒子共享一个billboardset
public:
    CParticleSystem();
    virtual ~CParticleSystem();

    //从xml加载
    virtual bool LoadFromFile(const char* szFilename);

    //渲染
    //virtual void            Render(CCamera* pCamera);

    //添加发射器
    void AddEmitter(CParticleEmitter* pEmitter);

    //添加影响器
    void AddAffector(CParticleAffector* pAffector);

    //每帧更新
    virtual void Update(float fTimeElapse);

    virtual void UpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue);

    /*
    //获得最大粒子数
    virtual int             GetQuota()
    {
        return m_nPoolSize;
    }

    //设置最大的粒子数
    virtual void            SetQuota(int nQuota);

    //获得被发射发射器的限额
    virtual size_t          GetEmittedEmitterSize()
    {
        return m_nEmittedEmitterPoolSize;
    }
    */

    //设置被发射发射器的限额
    //virtual void            SetEmittedEmitterSize(size_t nQuota);

    //获得粒子数
    size_t GetParticlesNum() const
    {
        return m_lstActiveParticles.size();
    }

    /*
    //设置默认的宽度
    virtual void            SetDefaultWidth(float fWidth);

    //设置默认的高度
    virtual void            SetDefaultHeight(float fHeight);

    //获得粒子宽
    virtual float           GetDefaultWidth()
    {
        return m_fDefaultWidth;
    }

    //获得粒子高
    virtual float					GetDefaultHeight()
    {
        return m_fDefaultHeight;
    }
    */

    //设置billboard类型
    //virtual void					SetBillBoardType(BillboardType type);

    //获得billboard类型
    //virtual BillboardType			GetBillBoardType()
    //{
    //    return m_billboardType; 
    //}

   
    //获得第一个发射器
    CParticleEmitter* GetFirstEmitter();

    //获得影响器数量
    size_t GetAffectorsCount() const
    {
        return m_vtParticleAffector.size();
    }

    //获得某个影响器
    CParticleAffector* GetAffector(u32 nIndex);

    //获得发射器
    CParticleEmitter* GetEmitter(u32 nIndex);
    

    //保存粒子系统到文件
    //@szFilename   要保存的文件名
    bool SaveToFile(const char* szFilename);

    /*
    //获得公告板类型
    virtual AIRString     GetBillBoardTypeStr();
    */
    //删除发射器
    void RemoveEmitter(u32 nIndex);

	//删除发射器
    void RemoveEmitter(CParticleEmitter* pEmitter);

    //删除影响器
    void RemoveAffector(u32 nIndex);

	//删除影响器
    void RemoveAffector(CParticleAffector* pAffector);
    

    //设置当前是否暂停发射粒子
    void SetPauseEmitParticle( bool b );

    //更新所有的发射器参数
    //void                    RefreshByParentNode( void );

	//当发射器停止发射并且没有粒子的时候,是否自动从节点脱离(不再更新和渲染)
	void SetAutoDetachFromParentNodeWhenEmpty( bool b );

    
    //获得发射器数量
    virtual unsigned GetEmittersCount()
    {
        return (unsigned)m_vtParticleEmitter.size();
    }
    /*
    //设置共同的方向
    void SetCommonDir( const Vector3Df& vCommonDir );
    //获得共同的方向
    const Vector3Df& GetCommonDir( void ) const;

    //设置共同的上方向
    void SetCommonUpDir( const Vector3Df& vCommonUpDir );
    //获得共同的上方向
    const Vector3Df& GetCommonUpDir( void ) const;

    //设置是否背面剔除
    void SetCullBackFace( bool b );
    //获取当前是否背面剔除
    bool GetCullBackFace( void );
    */

    //获得遍历器
    CParticleIterator GetIterator();
    
	//设置拖尾段数
    void SetTrailSegmentCount(int nSegment);

	//设置拖尾节点数量的最大值
    void SetMaxChainElement(int nMaxCount);

	//设置拖尾的最大宽度
    void SetMaxChainWidth(float fWidth);

    
	//设置拖尾宽度的变化值
    void SetChainWidthChange(float fWidth);

    /*
	//获得拖尾的最大宽度
	float GetMaxChainWidth( void )
	{
		return m_fInitTrailWidth;
	}

	//获得拖尾宽度的变化值
	float GetChainWidthChange( void )
	{
		return m_fTrailWidthChange;
	}

	//获得当前拖尾段数
    int GetTrailSegmentCount()
    {
        return m_nTrailSegmentCount;
    }

	//获得拖尾最大节点数
    int GetMaxChainElementNum()
    {
        return m_nMaxChainElement;
    }
    
    //重新设置被发射的发射器
    virtual void ReorganizeEmittedEmitters();
    

	//设置拖尾使用顶点颜色
	void SetTrailUseVertexColor( bool bUseVertexColor )
	{
		m_bTrailUseVertexColor = bUseVertexColor;
	}

	//设置拖尾使用纹理
	void SetTrailUseTexCoord( bool bUseTexCoord )
	{
		m_bTrailUseTexCoord = bUseTexCoord;
	}
    

	//获得当前拖尾是否用到顶点颜色
	bool GetTrailUseVertexColor( void )
	{
		return m_bTrailUseVertexColor;
	}

	//获得当前拖尾是否用到纹理
	bool GetTrailUseTexCoord( void )
	{
		return m_bTrailUseTexCoord;
	}		
    */
    
	//设置拖尾纹理
	void SetTrailTex( const char* szTexFile );

    /*
	//获得拖尾用到的纹理
	const AIRString&		GetTrailTex( void )
	{
		return m_szTrailTex;
	}
    */

	//设置拖尾纹理坐标方向
	void SetTrailTexCoordDir( u32 dir );
    

	//获得拖尾的纹理坐标方向
	//BillBoardChain::TexCoordDirection	GetTrailCoordDir( void )
	//{
	//	return m_TCD;
	//}

	//获得包围盒的更新时间
    float GetBoundingBoxUpdateTime() const
    {
        return m_fBoundingBoxUpdateTime;
    }

	//设置包围盒每个多长时间更新一次
    void SetBoundingBoxUpdateTime(float fTime)
    {
        m_fBoundingBoxUpdateTime = m_fBoundingBoxUpdateElapse = fTime;
    }

	//重置粒子系统
	virtual void Reset( void );

	//获得粒子系统的世界包围盒
    virtual const AABBBox3Df& GetWorldBoundingBox(bool bTransform)
    {
        return m_WorldAABB;
    }

    /*
	//设置粒子系统的源混合参数
	void					SetSrcBlend( const AIRString& szSrcBlend )
	{
		m_szSrcBlend = szSrcBlend;
		m_pMaterial->GetCurrentTechnique()->GetPass(0)->GetShader()->SetBlendSrc( ENGINE_INST->GetRenderer()->GetRenderParm(m_szSrcBlend) );
	}

	//获得粒子系统的源混合参数
	AIRString&			GetSrcBlend( void )
	{
		return m_szSrcBlend;
	}

	//设置粒子系统的目标混合参数
	void					SetDestBlend( const AIRString& szDestBlend )
	{
		m_szDestBlend = szDestBlend;
		m_pMaterial->GetCurrentTechnique()->GetPass(0)->GetShader()->SetBlendDest( ENGINE_INST->GetRenderer()->GetRenderParm(m_szDestBlend) );
	}

	//获得粒子系统的目标混合参数
	AIRString&			GetDestBlend( void )
	{
		return m_szDestBlend;
	}
    */

	//设置粒子系统的混合模式
	//void					SetBlendMode( const AIRString& szBlendMode )
	//{
	//	m_szBlendMode = szBlendMode;
	//	m_pMaterial->GetCurrentTechnique()->GetPass(0)->GetShader()->SetBlendOperation( ENGINE_INST->GetRenderer()->GetRenderParm(m_szBlendMode) );
	//}

	//获得粒子系统的混合模式
	//AIRString&			GetBlendMode( void )
	//{
	//	return m_szBlendMode;
	//}

	//设置粒子系统的渲染对象类型
	//void SetParticleSystemType( ParticleSystemType pst );

	//获得粒子系统的渲染对象类型
	//ParticleSystemType		GetParticleSystemType( void )
	//{
	//	return m_ParticleSystemType;
	//}
    /*
	//设置粒子系统渲染对象的资源
	void SetParticleSystemRenderableObjectResource( const AIRString& szRenderableObject );
    
	//获得粒子系统渲染对象的资源
	const AIRString&		GetParticleSystemRenderableObjectResource( void ) const
	{
		return m_szParticleRenderableResource;
	}

	//获得粒子系统的渲染对象
    
	Renderable* GetParticleSystemRenderableObject( void )
	{
		return m_pRenderableObject;
	}
    */

	//设置粒子系统的渲染对象
	//void	SetParticleSystemRenderableObject( Renderable* pRenderableObject );

    virtual IMaterial* GetMaterial();

    virtual bool IsTransparent();

    size_t GetActiveParticlesNum() const
    {
        return m_lstActiveParticles.size();
    }

	virtual bool IsCompleted();

    virtual void Start();

    //virtual void Stop();

    void SetEmitEndTime(float fTime);

    float GetEmitEndTime() const
    {
        return m_fEmitEndTime;
    }

    VectorIterator<VECTOR_PARTICLE> GetParticlePoolIter()
    {
        return VectorIterator<VECTOR_PARTICLE>(m_vtParticlePool);
    }

    virtual const Matrix4f& GetWorldMatrix() const
    {
        //if (m_bLocalSpace)
        //{
        //    return Renderable::GetWorldMatrix();
        //}
        return Matrix4f::IDENTITY;
    }

    virtual bool Prepare();

    ParticleSystemResource* GetResource()
    {
        return m_pParticleSystemResource;
    }

    //创建一个默认的粒子系统
    void CreateDefault();

    bool IsLocalSpace() const
    {
        return m_bLocalSpace;
    }

    void SetLocalSpace(bool bLocal)
    {
        m_bLocalSpace = bLocal;
    }

    virtual bool IsTransparent() const
    {
        return true;
    }
protected:
    //渲染前的处理
    virtual void            PreRender(CCamera* pCamera);

    //清空粒子系统
    virtual void            ClearRenderableImp();

    //配置粒子系统
    virtual void            Configure();

    //增加粒子池的大小
    virtual void            IncreasePool(size_t size);

    //减少粒子池的大小
    virtual void            DecreasePool(size_t size);

    //增加被发射发射器池的大小
    //virtual void            IncreaseEmittedEmitterPool(size_t size);

    //通知粒子的渲染数据的变更
    //void                    NotifyRenderData();

    //创建buffer，VB和IB
    //bool                    CreateBuffers();

    //销毁buffer
    //void                    DestoryBuffers();

    //更新Buffer
    virtual void            UpdateBuffers(CCamera* pCamera);

    //过期处理
    virtual void            Expire(float fTimeElapsed);

    //触发影响器
    virtual void            TriggerAffectors(float fTimeElapsed);

    //触发触发器
    virtual void            TriggerEmitters(float fTimeElapsed);

    //执行触发器
    virtual void            ExcuteTriggerEmitter(CParticleEmitter* pEmitter, u32 nParticleCount, float fTimeElapse);

    //创建一个粒子
    virtual CParticle*      CreateParticle();

	//创建发射器类型的粒子
	//virtual CParticle*      CreateEmitterParticle(const AIRString& strEmittedEmitter);

    //更新boundingbox
    virtual void            UpdateBoundingBox();

    //更新顶点
    virtual void            UpdateVertices(CCamera* pCamera);

    //更新索引
    //virtual void            UpdateIndices();

    //更新粒子的位置
    virtual void            ApplyMotion(float fTimeElapsed);

    //生成billboard的方向
    virtual void            GenBillboardDirection(CCamera* pCamera, CParticle* pParticle);

    //生成vertexoffset
    virtual void            GenVertexOffset(CParticle* pParticle);

    //加载发射器
   // virtual void            LoadEmitter(CMarkupSTL& xml);

    //加载影响器
    //virtual void            LoadAffector(CMarkupSTL& xml);

    //更新粒子的顶点数据
    virtual void            UpdateParticleVertices( CParticle* pParticle, VERTEX_STANDARD** ppVertices );

    //初始化被发射的发射器
    //virtual void            InitializeEmittedEmitters();

    //初始化被发射发射器池
    //virtual void            InitializeEmittedEmitterPool();

    //添加空闲的被发射发射器
    //virtual void            AddFreeEmittedEmitters();

    //查找空闲的被发射发射器队列
    //virtual LIST_EMITTER*   FindFreeEmittedEmitter(const AIRString& strName);

    //virtual void            RemoveAllEmittedEmitters();

	//virtual void            AddActiveEmittedEmittesToFreeList();

	//从活动队列中删除发射器粒子
	//virtual void            RemoveFromActiveEmittedEmitters(CParticleEmitter* pEmittedEmitter);

    //设置拖尾
    virtual void            SetupRibbonTrail();

	//设置当前要加拖尾的粒子
    virtual void            SetTrackParticle(CParticle* pParticle);

	//声明要销毁拖尾的粒子
    virtual void            NotifyTrackParticleDestroy(CParticle* pParticle);

	//获得当前正在用的拖尾索引
    virtual int             GetActiveChainIndex();

	//更新拖尾
	virtual void			UpdateTrail( float fFrameTime );

	//重置拖尾
	void					ResetTrail( void );

	virtual void  OnUpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue);

    virtual void _GetPrimitive();

    void SetPrepared(bool bPrepared)
    {
        m_bPrepared = bPrepared;
    }

    
protected:
    virtual bool PrepareImpl();

    float           m_fVisableTime;         //可视时间，0为无限时间
    //int             m_nPoolSize;            //粒子限额
    /// The number of emitted emitters in the pool.
    //size_t          m_nEmittedEmitterPoolSize;  //被发射的发射器限额
    //float           m_fDefaultWidth;        //默认宽度
    //float           m_fDefaultHeight;       //默认高度
    bool            m_bConfigured;          //是否已经配置好
    bool            m_bBoundsAutoUpdate;    //是否自动更新boundingbox
    float           m_fBoundsStopUpdateTime;//更新boundingbox的时间，过了该时间就不再更新boundingbox了
    bool            m_bBillboardRender;     //是否billboard渲染
    //BillboardType   m_billboardType;        //billboard类型
    Vector3Df       m_vCameraRight;         //摄像机的右方向
    Vector3Df       m_vCameraUp;            //摄像机的上方向
    Vector3Df       m_worldPosition;        //世界坐标
    VECTOR_EMITTER  m_vtParticleEmitter;    //粒子发射器
    //MAP_EMITTER_POOL m_emittedEmitterPool;  //被发射粒子发射器池
    //MAP_FREE_EMITTER m_lstFreeEmittedEmitters;   //空闲被发射发射器
    //LIST_EMITTER    m_lstActiveEmitters;    //已经被发射出来的发射器
    VECTOR_AFFECTOR m_vtParticleAffector;   //粒子影响器
    VECTOR_PARTICLE m_vtParticlePool;       //粒子池
    PARTICLE_LIST   m_lstActiveParticles;   //当前活动的粒子
    PARTICLE_LIST   m_lstFreeParticles;     //当前空闲的粒子
    Vector3Df       m_vtOffset[4];          //粒子的四个点
    Rectf           m_rcTextureCoord;       //纹理坐标
    bool            m_bPauseEmitParticle;   //是否暂停发射粒子
	bool			m_bAutoDetach;			//当发射器停止发射并且没有粒子的时候,是否自动从节点脱离(不再更新和渲染)
    //Vector3Df       m_vCommonDir;           //粒子系统的共同方向
    //Vector3Df       m_vCommonUpDir;         //粒子系统的共同上方向
    //bool            m_bCullBackFace;        //当前是否对粒子进行背面剔除
    bool            m_bInitializeEmittedEmitters;   //是否已经初始化了被发射的发射器
    bool            m_bNeedCreateRenderData; //是否需要创建VB

    bool            m_bCreateTrail; //是否创建拖尾
    //int             m_nTrailSegmentCount;   //拖尾段数
    //int             m_nMaxChainElement;   //拖尾顶点数
    RibbonTrail*    m_pTrail;               //拖尾

    typedef AIRList<int> ChainIndexList;
    typedef ChainIndexList::iterator ChainIndex_iter;
    PARTICLE_LIST          m_lstTrailParticle;//带拖尾的粒子
    ChainIndexList         m_lstFreeChain;	//可用的拖尾
    ChainIndexList         m_lstActiveChain; //正在使用中的拖尾
    //float                   m_fInitTrailWidth; //拖尾的初始宽度
    //float                   m_fTrailWidthChange; //拖尾的宽度变化

    float                  m_fBoundingBoxUpdateElapse;  //boundingbox更新过去时间
    float                  m_fBoundingBoxUpdateTime;    //boungingbox更新的时间间隔

	//bool			m_bTrailUseVertexColor;	//拖尾是否使用顶点颜色
	//bool			m_bTrailUseTexCoord;	//拖尾是否使用纹理坐标

    float           m_fRenderTimeElapsed;    //上一次渲染的时间间隔
    bool            m_bFirstTimeInit;        //第一次初始化
	//AIRString		m_szTrailTex; //拖尾纹理资源
	//BillBoardChain::TexCoordDirection m_TCD; //拖尾的纹理方向

	//粒子系统的材质属性
	//AIRString		m_szSrcBlend;	// 源混合参数
	//AIRString		m_szDestBlend;	// 目标混合参数
	//AIRString		m_szBlendMode;	// 混合模式


	//AIRString			m_szParticleRenderableResource;	//粒子系统的渲染对象资源
	//ParticleSystemType	m_ParticleSystemType; //粒子系统的渲染类型
	//Renderable*		m_pRenderableObject; //粒子系统的渲染对象
	//CSceneNode*			m_pRenderableObjectNode;//粒子系统的渲染对象场景节点

	bool			m_bParticleSystemUpdated;
    float           m_fEmitEndTime;     //发射持续时间，当粒子系统从启动时间到该时间，发射器持续发射，0则表示该时间无效

    ParticleSystemResource*  m_pParticleSystemResource;

    bool m_bLocalSpace;
};
