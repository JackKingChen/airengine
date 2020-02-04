// ***************************************************************
//  �ļ�: GEffect.h
//  ����: 2010-4-2010/04/28
//  ����: huangjunjie
//  ˵��: ��Ч�ļ�
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
    eES_Inactive,   //δ����״̬������Ч��Ⱦ�ﻹû����Ⱦʱ��
    eES_Active,     //����״̬������Ч��Ⱦ���Ѿ�����Ⱦ
    eES_Stop,       //ֹͣ״̬����Чʱ�䵽��Ҫֹͣ
    eES_Completed,  //���״̬����Ч��ʱ���Ѿ������������ڣ����ʱ������ȫ����ʾ��Ч
};

//��Ч�任��Ϣ
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

	Vector3Df	m_Position;	//λ��
	Quaternion	m_Rotation;	//��ת
};


//��ЧԪ����Ϣ
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
	f32				m_LifeTime;	//��Ч����ʱ��
	f32				m_StartTime; //��Ч����ʼ����ʱ��
	f32				m_fIterativeInterval; //��Ч�ĸ���ʱ����
	TransformInfo	m_Transform; //��Ч�ı任��Ϣ
	AIRString		m_szRenderableType;//��Ч����Ⱦ������
	AIRString		m_szRenderableFile;//��Ч����Ⱦ���ļ�
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
	GEffect*		m_pCreator;//���õ���resource����Ч����,��catch��ʱ��,ֱ��Ϊm_pCreator������������Ч����
};
*/


//��Ч����
class EngineExport GEffect : public Renderable//, public IAnimatedObject
{
public:
	GEffect();

	virtual ~GEffect();

	//��������Ч���ҽӵ���Ⱦ�����
	virtual Renderable* CreateRenderable() { return NULL; }

	//�������Ч���ҽӵ���Ⱦ�����
	inline Renderable* GetRenderable() { return m_pRenderObject; }

	//���ø���Ч����
	inline void			SetParentGEffect( GEffect* pParent ) { m_pParentGEffect = pParent; }

	//��ø���Ч����
	inline GEffect*		GetParentGEffect() { return m_pParentGEffect; }

	//�ͷ���Ч����
	virtual void		Clear();

	//������Ч
	virtual void		Update( f32 fTimeElapsed );

	//������Ч����Ⱦ����
	virtual void		UpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue);

	//���ز�������Ч
	virtual bool		LoadFromFile( const char* szFile );

	virtual void		LoadChildFromXML( CMarkupSTL& xml ) {}

	virtual void		SaveChildToXML( CMarkupSTL& xml );

	//����Ч���浽�ļ�
	virtual bool		SaveToFile( const AIRString& szFile );

	//�����Ч�Ƿ��Ѿ�׼����
	virtual bool		Prepare();

    EffectState GetEffectState() const
    {
        return m_state;
    }

    f32 GetStartTime() const
    {
        return m_Info.GetStartTime();
    }

	//�����Ч��Ϣ
	inline GEffectElementInfo& GetEffectElementInfo() { return m_Info; }

	//��������Ч
	GEffect*			CreateChildGEffect( GEffectElementInfo* pInfo );

	//�Ƿ�ر�����Ч
	//bool				IsStoped( void );

	//��Ч�Ƿ񲥷����
	virtual bool        IsCompleted( void );

	//����������Ч
	//virtual void		Start();

	//ֹͣ������Ч
	//virtual void		Stop();

	//��Ч�Ƿ��Ѿ�����
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
	//��Ⱦǰ�Ĵ���
	virtual void		PreRender(CCamera* pCamera);

	//�ͷ�����
	virtual void		ClearSelf();

	//�����Ƿ�׼����
	bool				PrepareSelf();

	//����Ч�Ƿ�׼����
	bool				PrepareChild();

	//��������
	//void				StartSelf();

	//������Чʱ����Ⱦ����������
	virtual void		StartRenderableObject();

	//ֹͣ��Чʱ����Ⱦ��رմ���
	virtual void		StopRenderableObject() {}

	//��������
	void				UpdateSelf( f32 fTimeLength );

	//��������Ч
	void				UpdateChild( f32 fTimeLength );

	//ֹͣ����
	//void				StopSelf();

	//������Ч����Ⱦ������ӽڵ�
	inline void			SetLinkNode( CSceneNode* pNode ) { m_pLinkNode = pNode; }

	//�����Ч����Ⱦ������ӽڵ�
	inline CSceneNode*	GetLinkNode() { return m_pLinkNode; }

	virtual bool		IsRenderObjectCompleted() { return true; }

	//virtual bool		CompleteStopImpl( f32 fFrameTime );

	virtual void		UpdateRenderObjectImpl( f32 fFrameTime );

    virtual void OnStateChange(EffectState state);

    

    bool IsChildEffectCompleted();

protected:	
	Renderable*		m_pRenderObject; //��Ч��Ӧ����Ⱦ�����
	GEffect*			m_pParentGEffect; //����Ч����
	//GEffectResource*	m_pGEffectResource;	 //��Ч��Դ
    FileData*           m_pResource;
	GEffectElementInfo	m_Info; //��ЧԪ����Ϣ
	VEC_GEFFECT			m_vtChildGEffects;	//����Ч�б�

	//bool				m_bIsActive; //��Ч�Ƿ񱻼���
	//bool				m_bIsStoped; //��Ч�Ƿ��Ѿ�ֹͣ
	//bool				m_bIsCompleted;
	//f32					m_fCurrentLifeTime; //��Ч��ǰ����������
	//f32					m_fCurrentStartTime; //��Ч��ǰ������ʱ�䳤��
	//f32					m_fIterateRemainTime; //��Ч���µĵ�ǰʱ�䳤��
    f32   m_fCurrentTime;   //��Ч�������ʱ��
	CSceneNode*			m_pLinkNode;//��Ч��Ⱦ������ӽڵ�

	bool				m_bLoop;//�Ƿ�ѭ�����ţ�ֻ�и���Ч������
	f32					m_fDelayTimeLen;//��ѭ��������,ÿ�β�����Ϻ��ӳ�m_fDelayTimeLen�������һ�β���
	f32					m_fDelayTimeCounter;
    EffectState         m_state;    //��Ч��ǰ״̬
    bool m_bResourceReady;
private:
	CSceneNode*			m_pLoopBackupNode;
};


//����ϵͳ���͵�����Ч
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


//������������Ч
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


//ģ������Ч
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


//��β����Ч
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

 
//����Ч������
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