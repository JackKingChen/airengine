/********************************************************************
	created:	9:3:2011   16:00
	filename: 	ParticleSystemResource.h
	author:		liangairan
	
	purpose:	粒子系统资源
*********************************************************************/

#pragma once 

#include "EngineResource.h"
#include "Vector3D.h"
#include "BillBoardChain.h"
#include "IteratorWrappers.h"

class IMaterial;
class BillboardSet;
class CParticleEmitter;
class CParticleAffector;

//粒子系统显示类型
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

class EngineExport ParticleSystemResource : public Resource, public ResourceListener
{
public:
    friend class CParticleSystem;
    typedef AIRVector<CParticleEmitter*>			VECTOR_EMITTER;
    typedef VECTOR_EMITTER::iterator				VECTOR_EMITTER_ITER;
    typedef AIRVector<CParticleAffector*>			VECTOR_AFFECTOR;
    typedef VECTOR_AFFECTOR::iterator				VECTOR_AFFECTOR_ITER;
public:
    ParticleSystemResource();
    virtual ~ParticleSystemResource();

    virtual bool Cache();

    //IMaterial* GetMaterial()
    //{
    //    return m_pMaterial;
    //}

    //BillboardSet* GetBillboardSet()
    //{
    //    return m_pBillboardSet;
    //}

    void SetBillBoardType(BillboardType type);

    //获得最大粒子数
    u32 GetQuota()
    {
        return m_nPoolSize;
    }

    //设置最大的粒子数
    void SetQuota(int nQuota);

    //获得被发射发射器的限额
    size_t GetEmittedEmitterSize()
    {
        return m_nEmittedEmitterPoolSize;
    }

    //设置默认的宽度
    void SetDefaultWidth(float fWidth);

    //设置默认的高度
    void SetDefaultHeight(float fHeight);

    //获得粒子宽
    float GetDefaultWidth()
    {
        return m_fDefaultWidth;
    }

    //获得粒子高
    float GetDefaultHeight()
    {
        return m_fDefaultHeight;
    }

    /*
    //获得某个影响器
    CParticleAffector* GetAffector(int nIndex);

    //获得发射器
    CParticleEmitter* GetEmitter(int nIndex);

    //删除发射器
    void RemoveEmitter(int nIndex);

    //删除发射器
    void RemoveEmitter(CParticleEmitter* pEmitter);

    //删除影响器
    void RemoveAffector(int nIndex);

    //删除影响器
    void RemoveAffector(CParticleAffector* pAffector);
    */

    //获得拖尾的最大宽度
    float GetMaxChainWidth( void )
    {
        return m_fInitTrailWidth;
    }

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

    //获得第一个发射器
    //CParticleEmitter* GetFirstEmitter();

    //获得影响器数量
    /*
    size_t GetAffectorsCount()
    {
        return m_vtParticleAffector.size();
    }
    

    //获得发射器数量
    virtual unsigned        GetEmittersCount()
    {
        return (unsigned)m_vtParticleEmitter.size();
    }
    */

    //设置共同的方向
    void SetCommonDir( const Vector3Df& vCommonDir );
    //获得共同的方向
    const Vector3Df& GetCommonDir( void ) const
    {
        return m_vCommonDir;
    }

    //设置共同的上方向
    void SetCommonUpDir( const Vector3Df& vCommonUpDir );
    //获得共同的上方向
    const Vector3Df& GetCommonUpDir( void ) const
    {
        return m_vCommonUpDir;
    }

    //获得粒子系统渲染对象的资源
    const AIRString& GetParticleSystemRenderableObjectResource( void ) const
    {
        return m_szParticleRenderableResource;
    }

    //设置拖尾段数
    void SetTrailSegmentCount(int nSegment);

    //设置拖尾节点数量的最大值
    void SetMaxChainElement(int nMaxCount);

    //设置拖尾的最大宽度
    void SetMaxChainWidth(float fWidth)
    {
        m_fInitTrailWidth = fWidth;
        //if ( m_pTrail )
        //{
        //    m_pTrail->SetMaxChainWidth( fWidth );
        //}
    }

    //设置拖尾宽度的变化值
    void SetChainWidthChange(float fWidth)
    {
        m_fTrailWidthChange = fWidth;
        //if ( m_pTrail )
        //{
        //    m_pTrail->SetChainWidthChange( fWidth );
        //}
    }

    ParticleSystemType GetParticleSystemType( void )
    {
    	return m_ParticleSystemType;
    }

    //添加发射器
    //void  AddEmitter(CParticleEmitter* pEmitter);

    //添加影响器
    //void  AddAffector(CParticleAffector* pAffector);

    virtual void Clear();

    //bool SaveToFile(const char* szFilename);

    AIRString GetBillBoardTypeStr();

    //virtual s32 Release();

    /*
    VectorIterator<VECTOR_AFFECTOR> GetAffectorIter()
    {
        return VectorIterator<VECTOR_AFFECTOR>(m_vtParticleAffector);
    }

    VectorIterator<VECTOR_EMITTER> GetEmitterIter()
    {
        return VectorIterator<VECTOR_EMITTER>(m_vtParticleEmitter);
    }
    */

    //设置是否背面剔除
    void SetCullBackFace( bool b );
    //获取当前是否背面剔除
    bool GetCullBackFace( void )
    {
        return m_bCullBackFace;
    }

    void SetParticleSystemType( ParticleSystemType pst );

    bool HasTrail() const
    {
        return m_bHasTrial;
    }

    BillboardType GetBillBoardType() const
    {
        return m_billboardType;
    }

    //获得拖尾用到的纹理
    const AIRString& GetTrailTex( void )
    {
        return m_szTrailTex;
    }

    //获得拖尾的纹理坐标方向
    BillBoardChain::TexCoordDirection GetTrailCoordDir( void )
    {
    	return m_TCD;
    }

    
    //设置粒子系统的源混合参数
    void SetSrcBlend( const AIRString& szSrcBlend );

    //获得粒子系统的源混合参数
    const AIRString& GetSrcBlend( void ) const
    {
        return m_szSrcBlend;
    }

    //设置粒子系统的目标混合参数
    void SetDestBlend( const AIRString& szDestBlend );

    //获得粒子系统的目标混合参数
    const AIRString& GetDestBlend( void ) const
    {
        return m_szDestBlend;
    }

    //设置粒子系统的混合模式
    void SetBlendMode( const AIRString& szBlendMode );

    //获得粒子系统的混合模式
    const AIRString& GetBlendMode( void ) const
    {
    	return m_szBlendMode;
    }

    //设置拖尾纹理
    void SetTrailTex( const char* szTexFile );

    void SetTrailTexCoordDir(u32 dir);

    virtual void OnResourceLoaded(Resource* pResource);

    virtual void OnResourceReady(Resource* pResource);

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

    bool GetTrailUseVertexColor( void )
    {
        return m_bTrailUseVertexColor;
    }

    //获得当前拖尾是否用到纹理
    bool GetTrailUseTexCoord( void )
    {
        return m_bTrailUseTexCoord;
    }		

    //设置粒子系统渲染对象的资源
    void SetParticleSystemRenderableObjectResource( const AIRString& szRenderableObject );

    void SetEmittedEmitterSize(size_t nQuota);
    
    //创建buffer，VB和IB
    //bool CreateBuffers();

    //void SetMaterial(IMaterial* pMaterial);

    //获得所有的emitter
    void GetEmittersAndAffectors(VECTOR_EMITTER& emitters, VECTOR_AFFECTOR& affectors);

    //获得所有的affector
    //void GetAffectors(VECTOR_AFFECTOR& affectors);

    bool IsLocalSpace() const
    {
        return m_bLocalSpace;
    }

    ITexture* GetParticleTexture() 
    {
        return m_pParticleTexture;
    }
protected:
    //void            LoadEmitter(CMarkupSTL& xml);

    //加载影响器
    //void            LoadAffector(CMarkupSTL& xml);

    void            LoadEmitter(CMarkupSTL& xml, VECTOR_EMITTER& emitters);

    //加载影响器
    void            LoadAffector(CMarkupSTL& xml, VECTOR_AFFECTOR& affectors);

    //销毁buffer
    //void  DestoryBuffers();
private:
    //IMaterial* m_pMaterial;
    //BillboardSet*   m_pBillboardSet;        //粒子渲染集合

    float           m_fVisableTime;         //可视时间，0为无限时间
    u32             m_nPoolSize;            //粒子限额
    /// The number of emitted emitters in the pool.
    size_t          m_nEmittedEmitterPoolSize;  //被发射的发射器限额
    float           m_fDefaultWidth;        //默认宽度
    float           m_fDefaultHeight;       //默认高度
    bool            m_bCullBackFace;        //当前是否对粒子进行背面剔除

    int             m_nTrailSegmentCount;   //拖尾段数
    int             m_nMaxChainElement;   //拖尾顶点数

    float           m_fInitTrailWidth; //拖尾的初始宽度
    float           m_fTrailWidthChange; //拖尾的宽度变化

    bool			m_bTrailUseVertexColor;	//拖尾是否使用顶点颜色
    bool			m_bTrailUseTexCoord;	//拖尾是否使用纹理坐标

    Vector3Df       m_vCommonDir;           //粒子系统的共同方向
    Vector3Df       m_vCommonUpDir;         //粒子系统的共同上方向

    AIRString			m_szParticleRenderableResource;	//粒子系统的渲染对象资源
    ParticleSystemType	m_ParticleSystemType; //粒子系统的渲染类型

    AIRString		m_szTrailTex; //拖尾纹理资源
    BillBoardChain::TexCoordDirection m_TCD; //拖尾的纹理方向

    float           m_fEmitEndTime;     //发射持续时间，当粒子系统从启动时间到该时间，发射器持续发射，0则表示该时间无效

    BillboardType   m_billboardType;        //billboard类型

    bool m_bCreateTrail;
    bool m_bBoundsAutoUpdate;    //是否自动更新boundingbox
    bool m_bHasTrial;    //是否有拖尾

    //粒子系统的材质属性
    AIRString		m_szSrcBlend;	// 源混合参数
    AIRString		m_szDestBlend;	// 目标混合参数
    AIRString		m_szBlendMode;	// 混合模式

    //VECTOR_AFFECTOR m_vtParticleAffector;   //粒子影响器
    //VECTOR_EMITTER  m_vtParticleEmitter;    //粒子发射器

    AABBBox3Df m_boundingBox;

    AIRString m_strParticleTexture;   //粒子纹理
    ITexture* m_pParticleTexture;     //粒子纹理

    bool m_bLocalSpace;

    //CMarkupSTL m_xml;
};


