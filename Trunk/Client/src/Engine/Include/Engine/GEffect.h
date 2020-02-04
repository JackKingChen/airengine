// ***************************************************************
//  文件: GEffect.h
//  日期: 2010-4-2010/04/28
//  作者: huangjunjie
//  说明: 特效文件
// ***************************************************************


#pragma once

#ifndef __GEffect_H_
#define __GEffect_H_


#include <map>
#include "IteratorWrappers.h"
#include "IAnimatedObject.h"
#include "Vector3D.h"
#include "Quaternion.h"
#include "EngineResource.h"
#include "Renderable.h"

using namespace AIR;

class CCamera;
class CRenderQueue;
class CParticleSystem;
//class GEffectResource;
class GEffectElementInfo;
class GEffect;
class FileData;


typedef AIRVector< GEffect* >				VEC_GEFFECT;

enum EffectState
{
    eES_Inactive,   //未激活状态，即特效渲染物还没到渲染时间
    eES_Active,     //激活状态，即特效渲染物已经在渲染
    eES_Stop,       //停止状态，特效时间到了要停止
    eES_Completed,  //完成状态，特效的时间已经超过了生命期，这个时候是完全不显示特效
};

//特效变换信息
struct TransformInfo
{
	TransformInfo(const Vector3Df& pos = Vector3Df::ZERO, const Quaternion& rotation = Quaternion::IDENEITY ) :
		m_Position(pos), 
		m_Rotation(rotation)
	{
	}

	inline void operator = ( TransformInfo& other )
	{
		m_Position = other.m_Position;
		m_Rotation = other.m_Rotation;
	}

	Vector3Df	m_Position;	//位置
	Quaternion	m_Rotation;	//旋转
};


//特效元素信息
class EngineExport	GEffectElementInfo
{
public:
	GEffectElementInfo():
		m_LifeTime(-1.0f), 
		m_StartTime(0.0f),
		m_fIterativeInterval(0.0f),
		m_szRenderableType(""),
		m_szRenderableFile("")
	{
	}

	~GEffectElementInfo()
	{
	}

	void operator	= ( GEffectElementInfo& other );
	inline void				SetLifeTime( f32 value ) { m_LifeTime = value; }
	inline f32				GetLifeTime() const { return m_LifeTime; }
	inline void				SetStartTime( f32 value ) { m_StartTime = value; }
	inline f32				GetStartTime() const { return m_StartTime; }
	inline void				SetIterativeInterval( f32 value ) { m_fIterativeInterval = value; }
	inline f32				GetIterativeInterval() { return m_fIterativeInterval; }
	inline void				SetPosition( const Vector3Df& vPos ) { m_Transform.m_Position = vPos; }
	inline void				SetRotation( const Quaternion& vRotate ) { m_Transform.m_Rotation = vRotate; }
	inline TransformInfo&	GetTransform() { return m_Transform; }
	inline const AIRString&		GetRenderableFile()	const { return m_szRenderableFile; }
	inline void				SetRenderableFile( const char* szFile ) { m_szRenderableFile = szFile; }
	inline const AIRString&		GetRenderableType() const { return m_szRenderableType; }
	inline void				SetRenderableType( const char* szType )	{ m_szRenderableType = szType; }

protected:
	f32				m_LifeTime;	//特效生命时间
	f32				m_StartTime; //特效的起始播放时间
	f32				m_fIterativeInterval; //特效的更新时间间隔
	TransformInfo	m_Transform; //特效的变换信息
	AIRString		m_szRenderableType;//特效的渲染物类型
	AIRString		m_szRenderableFile;//特效的渲染物文件
};


/*
class EngineExport GEffectResource : public Resource 
{
public:
	GEffectResource() : m_pCreator(NULL)
	{
	}

	virtual ~GEffectResource();

	virtual bool	Cache();
	virtual void	Clear();
	inline void		SetCreator( GEffect* pCreator ) { m_pCreator = pCreator; }

protected:
	GEffect*		m_pCreator;//引用到该resource的特效对象,在catch的时候,直接为m_pCreator生成所有子特效对象
};
*/


//特效基类
class EngineExport GEffect : public Renderable//, public IAnimatedObject
{
public:
	GEffect();

	virtual ~GEffect();

	//创建子特效所挂接的渲染物对象
	virtual Renderable* CreateRenderable() { return NULL; }

	//获得子特效所挂接的渲染物对象
	inline Renderable* GetRenderable() { return m_pRenderObject; }

	//设置父特效对象
	inline void			SetParentGEffect( GEffect* pParent ) { m_pParentGEffect = pParent; }

	//获得父特效对象
	inline GEffect*		GetParentGEffect() { return m_pParentGEffect; }

	//释放特效对象
	virtual void		Clear();

	//更新特效
	virtual void		Update( f32 fTimeElapsed );

	//更新特效到渲染队列
	virtual void		UpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue);

	//加载并创建特效
	virtual bool		LoadFromFile( const char* szFile );

	virtual void		LoadChildFromXML( CMarkupSTL& xml ) {}

	virtual void		SaveChildToXML( CMarkupSTL& xml );

	//将特效保存到文件
	virtual bool		SaveToFile( const AIRString& szFile );

	//检测特效是否已经准备好
	virtual bool		Prepare();

    EffectState GetEffectState() const
    {
        return m_state;
    }

    f32 GetStartTime() const
    {
        return m_Info.GetStartTime();
    }

	//获得特效信息
	inline GEffectElementInfo& GetEffectElementInfo() { return m_Info; }

	//创建子特效
	GEffect*			CreateChildGEffect( GEffectElementInfo* pInfo );

	//是否关闭子特效
	//bool				IsStoped( void );

	//特效是否播放完毕
	virtual bool        IsCompleted( void );

	//启动播放特效
	//virtual void		Start();

	//停止播放特效
	//virtual void		Stop();

	//特效是否已经激活
	//bool				IsActive() { return m_bIsActive; }

	inline void			SetLoop( bool bLoop ) { m_bLoop = bLoop; }
	inline bool			IsLoop() { return m_bLoop; }

	inline void			SetDelayTimeLen( f32 fTimeLen ) { m_fDelayTimeLen = fTimeLen; }
	inline f32			GetDelayTimeLen() { return m_fDelayTimeLen; }

    virtual void SetAttachedNode(CSceneNode* pNode);

public:
	//void				AddChildGEffect( GEffect* pChildEffect );
	void				SetInfoToNode();

	void				DelChildGEffect( u32 index );

	GEffect*			GetChildGEffect( u32 index );

	inline VectorIterator<VEC_GEFFECT> GetAllChildGEffects()
	{
		return VectorIterator<VEC_GEFFECT>(m_vtChildGEffects);
	}

    void SetCurrentState(EffectState state);
protected:
	//渲染前的处理
	virtual void		PreRender(CCamera* pCamera);

	//释放自身
	virtual void		ClearSelf();

	//自身是否准备好
	bool				PrepareSelf();

	//子特效是否准备好
	bool				PrepareChild();

	//启动自身
	//void				StartSelf();

	//启动特效时的渲染物启动处理
	virtual void		StartRenderableObject();

	//停止特效时的渲染物关闭处理
	virtual void		StopRenderableObject() {}

	//更新自身
	void				UpdateSelf( f32 fTimeLength );

	//更新子特效
	void				UpdateChild( f32 fTimeLength );

	//停止自身
	//void				StopSelf();

	//设置特效的渲染物的连接节点
	inline void			SetLinkNode( CSceneNode* pNode ) { m_pLinkNode = pNode; }

	//获得特效的渲染物的连接节点
	inline CSceneNode*	GetLinkNode() { return m_pLinkNode; }

	virtual bool		IsRenderObjectCompleted() { return true; }

	//virtual bool		CompleteStopImpl( f32 fFrameTime );

	virtual void		UpdateRenderObjectImpl( f32 fFrameTime );

    virtual void OnStateChange(EffectState state);

    

    bool IsChildEffectCompleted();

protected:	
	Renderable*		m_pRenderObject; //特效对应的渲染物对象
	GEffect*			m_pParentGEffect; //父特效对象
	//GEffectResource*	m_pGEffectResource;	 //特效资源
    FileData*           m_pResource;
	GEffectElementInfo	m_Info; //特效元素信息
	VEC_GEFFECT			m_vtChildGEffects;	//子特效列表

	//bool				m_bIsActive; //特效是否被激活
	//bool				m_bIsStoped; //特效是否已经停止
	//bool				m_bIsCompleted;
	//f32					m_fCurrentLifeTime; //特效当前的生命长度
	//f32					m_fCurrentStartTime; //特效当前的启动时间长度
	//f32					m_fIterateRemainTime; //特效更新的当前时间长度
    f32   m_fCurrentTime;   //特效启动后的时间
	CSceneNode*			m_pLinkNode;//特效渲染物的连接节点

	bool				m_bLoop;//是否循环播放，只有根特效有作用
	f32					m_fDelayTimeLen;//再循环播放中,每次播放完毕后延迟m_fDelayTimeLen秒后开启下一次播放
	f32					m_fDelayTimeCounter;
    EffectState         m_state;    //特效当前状态
    bool m_bResourceReady;
private:
	CSceneNode*			m_pLoopBackupNode;
};


//粒子系统类型的子特效
class EngineExport ParticleSystemEffect : public GEffect
{
public:
	ParticleSystemEffect()
	{
		m_szType = C_CHILD_GEFFECT_PARTICLE;
		m_Info.SetRenderableType( m_szType.c_str() );
	}

	~ParticleSystemEffect(){}

	Renderable* CreateRenderable();

	void StartRenderableObject();

	void StopRenderableObject();

	bool IsRenderObjectCompleted();

	bool CompleteStopImpl( f32 fFrameTime );

protected:	
public:
	static const AIRString C_CHILD_GEFFECT_PARTICLE;
};


//贴花类型子特效
class EngineExport DecalEffect : public GEffect
{
public:
	DecalEffect():m_fDecalRadius(200.0f)
	{
		m_szType = C_CHILD_GEFFECT_DECAL;
		m_Info.SetRenderableType( m_szType.c_str() );
	}

	~DecalEffect(){}

	Renderable* CreateRenderable();

	//bool CompleteStopImpl( f32 fFrameTime );

	void UpdateRenderObjectImpl( f32 fFrameTime );

	void SetDecalRadius( f32 fRadius );

	inline f32 GetDecalRadius() { return m_fDecalRadius; }

	void LoadChildFromXML( CMarkupSTL& xml );

	void SaveChildToXML( CMarkupSTL& xml );

protected:
	float m_fDecalRadius;
public:
	static const AIRString C_CHILD_GEFFECT_DECAL;
};


//模型子特效
class EngineExport ModelEffect : public GEffect
{
public:
	ModelEffect():m_szAnimName(""),m_bLoop(false)
	{
		m_szType = C_CHILD_GEFFECT_MODEL;
		m_Info.SetRenderableType( m_szType.c_str() );
	}

	~ModelEffect(){}

	Renderable* CreateRenderable();

	void StartRenderableObject();

	//bool CompleteStopImpl( f32 fFrameTime );

	void UpdateRenderObjectImpl( f32 fFrameTime );

	void LoadChildFromXML( CMarkupSTL& xml );

	void SaveChildToXML( CMarkupSTL& xml );

	inline void SetAnimName( const AIRString& szAnimName ) { m_szAnimName = szAnimName; }

	inline AIRString& GetAnimName() { return m_szAnimName; }

	inline void SetLoop( bool loop ) { m_bLoop = loop; }

	inline bool GetLoop() { return m_bLoop; }

protected:
	AIRString m_szAnimName;
	bool m_bLoop;
public:
	static const AIRString C_CHILD_GEFFECT_MODEL;
};


//拖尾子特效
class EngineExport RibbonTrailEffect : public GEffect
{
public:
	RibbonTrailEffect()
	{
		m_szType = C_CHILD_GEFFECT_RIBBON_TRAIL;
		m_Info.SetRenderableType( m_szType.c_str() );
	}

	~RibbonTrailEffect(){}

	Renderable* CreateRenderable();

	void StartRenderableObject();

	//bool CompleteStopImpl( f32 fFrameTime );

	void UpdateRenderObjectImpl( f32 fFrameTime );

	void LoadChildFromXML( CMarkupSTL& xml );

	void SaveChildToXML( CMarkupSTL& xml );

protected:

public:
	static const AIRString C_CHILD_GEFFECT_RIBBON_TRAIL;
};


class EngineExport BillboardEffect : public GEffect
{
public:
	BillboardEffect()
	{
		m_szType = C_CHILD_GEFFECT_BILLBOARD;
		m_Info.SetRenderableType( m_szType.c_str() );
	}

	~BillboardEffect(){}

	Renderable* CreateRenderable();

	//bool CompleteStopImpl( f32 fFrameTime );

	//void StartRenderableObject();

	void SetBillboardSize( f32 fSize );

	inline f32 GetBillboardSize() { return m_fBillboardSize; }

	void SetBillboardTexture( const char* szTexFile );

	const AIRString& GetBillboardTexture() const;

	void LoadChildFromXML( CMarkupSTL& xml );

	void SaveChildToXML( CMarkupSTL& xml );

protected:
	float m_fBillboardSize;

public:
	static const AIRString C_CHILD_GEFFECT_BILLBOARD;
};

 
//子特效工厂类
class EngineExport GEffectFactory : public Singleton<GEffectFactory>
{
	BE_SINGLETON(GEffectFactory);
public:
	~GEffectFactory(){}

	GEffect* CreateGEffect( const AIRString& strType, const AIRString& szFile );

	GEffect* CreateGEffect(const AIRString& strType);

protected:
};

#define GEFFECT_FACTORY GEffectFactory::GetInstance()






#endif //! end __GEffect_H_