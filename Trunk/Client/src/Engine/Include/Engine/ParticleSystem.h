/********************************************************************
	created:	2009/11/28
	created:	28:11:2009   16:42
	filename: 	d:\My Codes\Engine\Engine\Include\ParticleSystem.h
	file path:	d:\My Codes\Engine\Engine\Include
	file base:	ParticleSystem
	file ext:	h
	author:		liangairan
	
	purpose:	����ϵͳ
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
//    //�洹ֱ������ķ���
//    BBT_PERPENDICULAR_CAMERA,
//
//    //��ƽ����Y��
//    BBT_HORIZON_Y,
//
//    //�洹ֱ��Y��
//    BBT_VERTICAL_Y,
//};

//����ϵͳ��ʾ����
/*
enum ParticleSystemType
{
	ePST_NONE = 0,	//��
	ePST_QUAD = 1,	//������Ƭ
	ePST_OBJ  = 2,	//��Ⱦ��(ģ��,��������ϵͳ��)
};

enum BillboardType
{
    //ʼ�����������
    BBT_POINT,

    //ƽ���ڹ�������(m_vCommonDir)�����������
    BBT_HORZ_COMMON_DIR_FACE_CAM,

    //ƽ���ڹ�������(m_vCommonDir)�����򹫹��Ϸ���(m_vCommonUpDir)
    BBT_HORZ_COMMON_DIR_FACE_COMMON_UP_DIR,

    //��ֱ�ڹ�������(m_vCommonDir)�����������
    BBT_VERT_COMMON_DIR_FACE_CAM,

    //��ֱ�ڹ�������(m_vCommonDir)�����򹫹��Ϸ���(m_vCommonUpDir)
    BBT_VERT_COMMON_DIR_FACE_COMMON_UP_DIR,

    //ƽ�����Լ����˶�����(pParticle->m_direction)�����������
    BBT_HORZ_SELF_DIR_FACE_CAM,

    //ƽ�����Լ����˶�����(pParticle->m_direction)�����򹫹��Ϸ���(m_vCommonUpDir)
    BBT_HORZ_SELF_DIR_FACE_COMMON_UP_DIR,

    //��ֱ���Լ����˶�����(pParticle->m_direction)�����������
    BBT_VERT_SELF_DIR_FACE_CAM,

    //��ֱ���Լ����˶�����(pParticle->m_direction)�����򹫹��Ϸ���(m_vCommonUpDir)
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

    //���ڱ�����ķ�����
    /*
    typedef std::map<AIRString, VECTOR_EMITTER>	MAP_EMITTER_POOL;
    typedef std::map<AIRString, LIST_EMITTER>		MAP_FREE_EMITTER;
    typedef MAP_EMITTER_POOL::iterator				EMITTER_POOL_ITER;
    typedef MAP_FREE_EMITTER::iterator				FREE_EMITTER_ITER;
    */
    static BillboardSet* m_pBillboardSet;   //�������ӹ���һ��billboardset
public:
    CParticleSystem();
    virtual ~CParticleSystem();

    //��xml����
    virtual bool LoadFromFile(const char* szFilename);

    //��Ⱦ
    //virtual void            Render(CCamera* pCamera);

    //��ӷ�����
    void AddEmitter(CParticleEmitter* pEmitter);

    //���Ӱ����
    void AddAffector(CParticleAffector* pAffector);

    //ÿ֡����
    virtual void Update(float fTimeElapse);

    virtual void UpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue);

    /*
    //������������
    virtual int             GetQuota()
    {
        return m_nPoolSize;
    }

    //��������������
    virtual void            SetQuota(int nQuota);

    //��ñ����䷢�������޶�
    virtual size_t          GetEmittedEmitterSize()
    {
        return m_nEmittedEmitterPoolSize;
    }
    */

    //���ñ����䷢�������޶�
    //virtual void            SetEmittedEmitterSize(size_t nQuota);

    //���������
    size_t GetParticlesNum() const
    {
        return m_lstActiveParticles.size();
    }

    /*
    //����Ĭ�ϵĿ��
    virtual void            SetDefaultWidth(float fWidth);

    //����Ĭ�ϵĸ߶�
    virtual void            SetDefaultHeight(float fHeight);

    //������ӿ�
    virtual float           GetDefaultWidth()
    {
        return m_fDefaultWidth;
    }

    //������Ӹ�
    virtual float					GetDefaultHeight()
    {
        return m_fDefaultHeight;
    }
    */

    //����billboard����
    //virtual void					SetBillBoardType(BillboardType type);

    //���billboard����
    //virtual BillboardType			GetBillBoardType()
    //{
    //    return m_billboardType; 
    //}

   
    //��õ�һ��������
    CParticleEmitter* GetFirstEmitter();

    //���Ӱ��������
    size_t GetAffectorsCount() const
    {
        return m_vtParticleAffector.size();
    }

    //���ĳ��Ӱ����
    CParticleAffector* GetAffector(u32 nIndex);

    //��÷�����
    CParticleEmitter* GetEmitter(u32 nIndex);
    

    //��������ϵͳ���ļ�
    //@szFilename   Ҫ������ļ���
    bool SaveToFile(const char* szFilename);

    /*
    //��ù��������
    virtual AIRString     GetBillBoardTypeStr();
    */
    //ɾ��������
    void RemoveEmitter(u32 nIndex);

	//ɾ��������
    void RemoveEmitter(CParticleEmitter* pEmitter);

    //ɾ��Ӱ����
    void RemoveAffector(u32 nIndex);

	//ɾ��Ӱ����
    void RemoveAffector(CParticleAffector* pAffector);
    

    //���õ�ǰ�Ƿ���ͣ��������
    void SetPauseEmitParticle( bool b );

    //�������еķ���������
    //void                    RefreshByParentNode( void );

	//��������ֹͣ���䲢��û�����ӵ�ʱ��,�Ƿ��Զ��ӽڵ�����(���ٸ��º���Ⱦ)
	void SetAutoDetachFromParentNodeWhenEmpty( bool b );

    
    //��÷���������
    virtual unsigned GetEmittersCount()
    {
        return (unsigned)m_vtParticleEmitter.size();
    }
    /*
    //���ù�ͬ�ķ���
    void SetCommonDir( const Vector3Df& vCommonDir );
    //��ù�ͬ�ķ���
    const Vector3Df& GetCommonDir( void ) const;

    //���ù�ͬ���Ϸ���
    void SetCommonUpDir( const Vector3Df& vCommonUpDir );
    //��ù�ͬ���Ϸ���
    const Vector3Df& GetCommonUpDir( void ) const;

    //�����Ƿ����޳�
    void SetCullBackFace( bool b );
    //��ȡ��ǰ�Ƿ����޳�
    bool GetCullBackFace( void );
    */

    //��ñ�����
    CParticleIterator GetIterator();
    
	//������β����
    void SetTrailSegmentCount(int nSegment);

	//������β�ڵ����������ֵ
    void SetMaxChainElement(int nMaxCount);

	//������β�������
    void SetMaxChainWidth(float fWidth);

    
	//������β��ȵı仯ֵ
    void SetChainWidthChange(float fWidth);

    /*
	//�����β�������
	float GetMaxChainWidth( void )
	{
		return m_fInitTrailWidth;
	}

	//�����β��ȵı仯ֵ
	float GetChainWidthChange( void )
	{
		return m_fTrailWidthChange;
	}

	//��õ�ǰ��β����
    int GetTrailSegmentCount()
    {
        return m_nTrailSegmentCount;
    }

	//�����β���ڵ���
    int GetMaxChainElementNum()
    {
        return m_nMaxChainElement;
    }
    
    //�������ñ�����ķ�����
    virtual void ReorganizeEmittedEmitters();
    

	//������βʹ�ö�����ɫ
	void SetTrailUseVertexColor( bool bUseVertexColor )
	{
		m_bTrailUseVertexColor = bUseVertexColor;
	}

	//������βʹ������
	void SetTrailUseTexCoord( bool bUseTexCoord )
	{
		m_bTrailUseTexCoord = bUseTexCoord;
	}
    

	//��õ�ǰ��β�Ƿ��õ�������ɫ
	bool GetTrailUseVertexColor( void )
	{
		return m_bTrailUseVertexColor;
	}

	//��õ�ǰ��β�Ƿ��õ�����
	bool GetTrailUseTexCoord( void )
	{
		return m_bTrailUseTexCoord;
	}		
    */
    
	//������β����
	void SetTrailTex( const char* szTexFile );

    /*
	//�����β�õ�������
	const AIRString&		GetTrailTex( void )
	{
		return m_szTrailTex;
	}
    */

	//������β�������귽��
	void SetTrailTexCoordDir( u32 dir );
    

	//�����β���������귽��
	//BillBoardChain::TexCoordDirection	GetTrailCoordDir( void )
	//{
	//	return m_TCD;
	//}

	//��ð�Χ�еĸ���ʱ��
    float GetBoundingBoxUpdateTime() const
    {
        return m_fBoundingBoxUpdateTime;
    }

	//���ð�Χ��ÿ���೤ʱ�����һ��
    void SetBoundingBoxUpdateTime(float fTime)
    {
        m_fBoundingBoxUpdateTime = m_fBoundingBoxUpdateElapse = fTime;
    }

	//��������ϵͳ
	virtual void Reset( void );

	//�������ϵͳ�������Χ��
    virtual const AABBBox3Df& GetWorldBoundingBox(bool bTransform)
    {
        return m_WorldAABB;
    }

    /*
	//��������ϵͳ��Դ��ϲ���
	void					SetSrcBlend( const AIRString& szSrcBlend )
	{
		m_szSrcBlend = szSrcBlend;
		m_pMaterial->GetCurrentTechnique()->GetPass(0)->GetShader()->SetBlendSrc( ENGINE_INST->GetRenderer()->GetRenderParm(m_szSrcBlend) );
	}

	//�������ϵͳ��Դ��ϲ���
	AIRString&			GetSrcBlend( void )
	{
		return m_szSrcBlend;
	}

	//��������ϵͳ��Ŀ���ϲ���
	void					SetDestBlend( const AIRString& szDestBlend )
	{
		m_szDestBlend = szDestBlend;
		m_pMaterial->GetCurrentTechnique()->GetPass(0)->GetShader()->SetBlendDest( ENGINE_INST->GetRenderer()->GetRenderParm(m_szDestBlend) );
	}

	//�������ϵͳ��Ŀ���ϲ���
	AIRString&			GetDestBlend( void )
	{
		return m_szDestBlend;
	}
    */

	//��������ϵͳ�Ļ��ģʽ
	//void					SetBlendMode( const AIRString& szBlendMode )
	//{
	//	m_szBlendMode = szBlendMode;
	//	m_pMaterial->GetCurrentTechnique()->GetPass(0)->GetShader()->SetBlendOperation( ENGINE_INST->GetRenderer()->GetRenderParm(m_szBlendMode) );
	//}

	//�������ϵͳ�Ļ��ģʽ
	//AIRString&			GetBlendMode( void )
	//{
	//	return m_szBlendMode;
	//}

	//��������ϵͳ����Ⱦ��������
	//void SetParticleSystemType( ParticleSystemType pst );

	//�������ϵͳ����Ⱦ��������
	//ParticleSystemType		GetParticleSystemType( void )
	//{
	//	return m_ParticleSystemType;
	//}
    /*
	//��������ϵͳ��Ⱦ�������Դ
	void SetParticleSystemRenderableObjectResource( const AIRString& szRenderableObject );
    
	//�������ϵͳ��Ⱦ�������Դ
	const AIRString&		GetParticleSystemRenderableObjectResource( void ) const
	{
		return m_szParticleRenderableResource;
	}

	//�������ϵͳ����Ⱦ����
    
	Renderable* GetParticleSystemRenderableObject( void )
	{
		return m_pRenderableObject;
	}
    */

	//��������ϵͳ����Ⱦ����
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

    //����һ��Ĭ�ϵ�����ϵͳ
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
    //��Ⱦǰ�Ĵ���
    virtual void            PreRender(CCamera* pCamera);

    //�������ϵͳ
    virtual void            ClearRenderableImp();

    //��������ϵͳ
    virtual void            Configure();

    //�������ӳصĴ�С
    virtual void            IncreasePool(size_t size);

    //�������ӳصĴ�С
    virtual void            DecreasePool(size_t size);

    //���ӱ����䷢�����صĴ�С
    //virtual void            IncreaseEmittedEmitterPool(size_t size);

    //֪ͨ���ӵ���Ⱦ���ݵı��
    //void                    NotifyRenderData();

    //����buffer��VB��IB
    //bool                    CreateBuffers();

    //����buffer
    //void                    DestoryBuffers();

    //����Buffer
    virtual void            UpdateBuffers(CCamera* pCamera);

    //���ڴ���
    virtual void            Expire(float fTimeElapsed);

    //����Ӱ����
    virtual void            TriggerAffectors(float fTimeElapsed);

    //����������
    virtual void            TriggerEmitters(float fTimeElapsed);

    //ִ�д�����
    virtual void            ExcuteTriggerEmitter(CParticleEmitter* pEmitter, u32 nParticleCount, float fTimeElapse);

    //����һ������
    virtual CParticle*      CreateParticle();

	//�������������͵�����
	//virtual CParticle*      CreateEmitterParticle(const AIRString& strEmittedEmitter);

    //����boundingbox
    virtual void            UpdateBoundingBox();

    //���¶���
    virtual void            UpdateVertices(CCamera* pCamera);

    //��������
    //virtual void            UpdateIndices();

    //�������ӵ�λ��
    virtual void            ApplyMotion(float fTimeElapsed);

    //����billboard�ķ���
    virtual void            GenBillboardDirection(CCamera* pCamera, CParticle* pParticle);

    //����vertexoffset
    virtual void            GenVertexOffset(CParticle* pParticle);

    //���ط�����
   // virtual void            LoadEmitter(CMarkupSTL& xml);

    //����Ӱ����
    //virtual void            LoadAffector(CMarkupSTL& xml);

    //�������ӵĶ�������
    virtual void            UpdateParticleVertices( CParticle* pParticle, VERTEX_STANDARD** ppVertices );

    //��ʼ��������ķ�����
    //virtual void            InitializeEmittedEmitters();

    //��ʼ�������䷢������
    //virtual void            InitializeEmittedEmitterPool();

    //��ӿ��еı����䷢����
    //virtual void            AddFreeEmittedEmitters();

    //���ҿ��еı����䷢��������
    //virtual LIST_EMITTER*   FindFreeEmittedEmitter(const AIRString& strName);

    //virtual void            RemoveAllEmittedEmitters();

	//virtual void            AddActiveEmittedEmittesToFreeList();

	//�ӻ������ɾ������������
	//virtual void            RemoveFromActiveEmittedEmitters(CParticleEmitter* pEmittedEmitter);

    //������β
    virtual void            SetupRibbonTrail();

	//���õ�ǰҪ����β������
    virtual void            SetTrackParticle(CParticle* pParticle);

	//����Ҫ������β������
    virtual void            NotifyTrackParticleDestroy(CParticle* pParticle);

	//��õ�ǰ�����õ���β����
    virtual int             GetActiveChainIndex();

	//������β
	virtual void			UpdateTrail( float fFrameTime );

	//������β
	void					ResetTrail( void );

	virtual void  OnUpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue);

    virtual void _GetPrimitive();

    void SetPrepared(bool bPrepared)
    {
        m_bPrepared = bPrepared;
    }

    
protected:
    virtual bool PrepareImpl();

    float           m_fVisableTime;         //����ʱ�䣬0Ϊ����ʱ��
    //int             m_nPoolSize;            //�����޶�
    /// The number of emitted emitters in the pool.
    //size_t          m_nEmittedEmitterPoolSize;  //������ķ������޶�
    //float           m_fDefaultWidth;        //Ĭ�Ͽ��
    //float           m_fDefaultHeight;       //Ĭ�ϸ߶�
    bool            m_bConfigured;          //�Ƿ��Ѿ����ú�
    bool            m_bBoundsAutoUpdate;    //�Ƿ��Զ�����boundingbox
    float           m_fBoundsStopUpdateTime;//����boundingbox��ʱ�䣬���˸�ʱ��Ͳ��ٸ���boundingbox��
    bool            m_bBillboardRender;     //�Ƿ�billboard��Ⱦ
    //BillboardType   m_billboardType;        //billboard����
    Vector3Df       m_vCameraRight;         //��������ҷ���
    Vector3Df       m_vCameraUp;            //��������Ϸ���
    Vector3Df       m_worldPosition;        //��������
    VECTOR_EMITTER  m_vtParticleEmitter;    //���ӷ�����
    //MAP_EMITTER_POOL m_emittedEmitterPool;  //���������ӷ�������
    //MAP_FREE_EMITTER m_lstFreeEmittedEmitters;   //���б����䷢����
    //LIST_EMITTER    m_lstActiveEmitters;    //�Ѿ�����������ķ�����
    VECTOR_AFFECTOR m_vtParticleAffector;   //����Ӱ����
    VECTOR_PARTICLE m_vtParticlePool;       //���ӳ�
    PARTICLE_LIST   m_lstActiveParticles;   //��ǰ�������
    PARTICLE_LIST   m_lstFreeParticles;     //��ǰ���е�����
    Vector3Df       m_vtOffset[4];          //���ӵ��ĸ���
    Rectf           m_rcTextureCoord;       //��������
    bool            m_bPauseEmitParticle;   //�Ƿ���ͣ��������
	bool			m_bAutoDetach;			//��������ֹͣ���䲢��û�����ӵ�ʱ��,�Ƿ��Զ��ӽڵ�����(���ٸ��º���Ⱦ)
    //Vector3Df       m_vCommonDir;           //����ϵͳ�Ĺ�ͬ����
    //Vector3Df       m_vCommonUpDir;         //����ϵͳ�Ĺ�ͬ�Ϸ���
    //bool            m_bCullBackFace;        //��ǰ�Ƿ�����ӽ��б����޳�
    bool            m_bInitializeEmittedEmitters;   //�Ƿ��Ѿ���ʼ���˱�����ķ�����
    bool            m_bNeedCreateRenderData; //�Ƿ���Ҫ����VB

    bool            m_bCreateTrail; //�Ƿ񴴽���β
    //int             m_nTrailSegmentCount;   //��β����
    //int             m_nMaxChainElement;   //��β������
    RibbonTrail*    m_pTrail;               //��β

    typedef AIRList<int> ChainIndexList;
    typedef ChainIndexList::iterator ChainIndex_iter;
    PARTICLE_LIST          m_lstTrailParticle;//����β������
    ChainIndexList         m_lstFreeChain;	//���õ���β
    ChainIndexList         m_lstActiveChain; //����ʹ���е���β
    //float                   m_fInitTrailWidth; //��β�ĳ�ʼ���
    //float                   m_fTrailWidthChange; //��β�Ŀ�ȱ仯

    float                  m_fBoundingBoxUpdateElapse;  //boundingbox���¹�ȥʱ��
    float                  m_fBoundingBoxUpdateTime;    //boungingbox���µ�ʱ����

	//bool			m_bTrailUseVertexColor;	//��β�Ƿ�ʹ�ö�����ɫ
	//bool			m_bTrailUseTexCoord;	//��β�Ƿ�ʹ����������

    float           m_fRenderTimeElapsed;    //��һ����Ⱦ��ʱ����
    bool            m_bFirstTimeInit;        //��һ�γ�ʼ��
	//AIRString		m_szTrailTex; //��β������Դ
	//BillBoardChain::TexCoordDirection m_TCD; //��β��������

	//����ϵͳ�Ĳ�������
	//AIRString		m_szSrcBlend;	// Դ��ϲ���
	//AIRString		m_szDestBlend;	// Ŀ���ϲ���
	//AIRString		m_szBlendMode;	// ���ģʽ


	//AIRString			m_szParticleRenderableResource;	//����ϵͳ����Ⱦ������Դ
	//ParticleSystemType	m_ParticleSystemType; //����ϵͳ����Ⱦ����
	//Renderable*		m_pRenderableObject; //����ϵͳ����Ⱦ����
	//CSceneNode*			m_pRenderableObjectNode;//����ϵͳ����Ⱦ���󳡾��ڵ�

	bool			m_bParticleSystemUpdated;
    float           m_fEmitEndTime;     //�������ʱ�䣬������ϵͳ������ʱ�䵽��ʱ�䣬�������������䣬0���ʾ��ʱ����Ч

    ParticleSystemResource*  m_pParticleSystemResource;

    bool m_bLocalSpace;
};
