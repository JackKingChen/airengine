/********************************************************************
	created:	9:3:2011   16:00
	filename: 	ParticleSystemResource.h
	author:		liangairan
	
	purpose:	����ϵͳ��Դ
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

//����ϵͳ��ʾ����
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

    //������������
    u32 GetQuota()
    {
        return m_nPoolSize;
    }

    //��������������
    void SetQuota(int nQuota);

    //��ñ����䷢�������޶�
    size_t GetEmittedEmitterSize()
    {
        return m_nEmittedEmitterPoolSize;
    }

    //����Ĭ�ϵĿ��
    void SetDefaultWidth(float fWidth);

    //����Ĭ�ϵĸ߶�
    void SetDefaultHeight(float fHeight);

    //������ӿ�
    float GetDefaultWidth()
    {
        return m_fDefaultWidth;
    }

    //������Ӹ�
    float GetDefaultHeight()
    {
        return m_fDefaultHeight;
    }

    /*
    //���ĳ��Ӱ����
    CParticleAffector* GetAffector(int nIndex);

    //��÷�����
    CParticleEmitter* GetEmitter(int nIndex);

    //ɾ��������
    void RemoveEmitter(int nIndex);

    //ɾ��������
    void RemoveEmitter(CParticleEmitter* pEmitter);

    //ɾ��Ӱ����
    void RemoveAffector(int nIndex);

    //ɾ��Ӱ����
    void RemoveAffector(CParticleAffector* pAffector);
    */

    //�����β�������
    float GetMaxChainWidth( void )
    {
        return m_fInitTrailWidth;
    }

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

    //��õ�һ��������
    //CParticleEmitter* GetFirstEmitter();

    //���Ӱ��������
    /*
    size_t GetAffectorsCount()
    {
        return m_vtParticleAffector.size();
    }
    

    //��÷���������
    virtual unsigned        GetEmittersCount()
    {
        return (unsigned)m_vtParticleEmitter.size();
    }
    */

    //���ù�ͬ�ķ���
    void SetCommonDir( const Vector3Df& vCommonDir );
    //��ù�ͬ�ķ���
    const Vector3Df& GetCommonDir( void ) const
    {
        return m_vCommonDir;
    }

    //���ù�ͬ���Ϸ���
    void SetCommonUpDir( const Vector3Df& vCommonUpDir );
    //��ù�ͬ���Ϸ���
    const Vector3Df& GetCommonUpDir( void ) const
    {
        return m_vCommonUpDir;
    }

    //�������ϵͳ��Ⱦ�������Դ
    const AIRString& GetParticleSystemRenderableObjectResource( void ) const
    {
        return m_szParticleRenderableResource;
    }

    //������β����
    void SetTrailSegmentCount(int nSegment);

    //������β�ڵ����������ֵ
    void SetMaxChainElement(int nMaxCount);

    //������β�������
    void SetMaxChainWidth(float fWidth)
    {
        m_fInitTrailWidth = fWidth;
        //if ( m_pTrail )
        //{
        //    m_pTrail->SetMaxChainWidth( fWidth );
        //}
    }

    //������β��ȵı仯ֵ
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

    //��ӷ�����
    //void  AddEmitter(CParticleEmitter* pEmitter);

    //���Ӱ����
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

    //�����Ƿ����޳�
    void SetCullBackFace( bool b );
    //��ȡ��ǰ�Ƿ����޳�
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

    //�����β�õ�������
    const AIRString& GetTrailTex( void )
    {
        return m_szTrailTex;
    }

    //�����β���������귽��
    BillBoardChain::TexCoordDirection GetTrailCoordDir( void )
    {
    	return m_TCD;
    }

    
    //��������ϵͳ��Դ��ϲ���
    void SetSrcBlend( const AIRString& szSrcBlend );

    //�������ϵͳ��Դ��ϲ���
    const AIRString& GetSrcBlend( void ) const
    {
        return m_szSrcBlend;
    }

    //��������ϵͳ��Ŀ���ϲ���
    void SetDestBlend( const AIRString& szDestBlend );

    //�������ϵͳ��Ŀ���ϲ���
    const AIRString& GetDestBlend( void ) const
    {
        return m_szDestBlend;
    }

    //��������ϵͳ�Ļ��ģʽ
    void SetBlendMode( const AIRString& szBlendMode );

    //�������ϵͳ�Ļ��ģʽ
    const AIRString& GetBlendMode( void ) const
    {
    	return m_szBlendMode;
    }

    //������β����
    void SetTrailTex( const char* szTexFile );

    void SetTrailTexCoordDir(u32 dir);

    virtual void OnResourceLoaded(Resource* pResource);

    virtual void OnResourceReady(Resource* pResource);

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

    bool GetTrailUseVertexColor( void )
    {
        return m_bTrailUseVertexColor;
    }

    //��õ�ǰ��β�Ƿ��õ�����
    bool GetTrailUseTexCoord( void )
    {
        return m_bTrailUseTexCoord;
    }		

    //��������ϵͳ��Ⱦ�������Դ
    void SetParticleSystemRenderableObjectResource( const AIRString& szRenderableObject );

    void SetEmittedEmitterSize(size_t nQuota);
    
    //����buffer��VB��IB
    //bool CreateBuffers();

    //void SetMaterial(IMaterial* pMaterial);

    //������е�emitter
    void GetEmittersAndAffectors(VECTOR_EMITTER& emitters, VECTOR_AFFECTOR& affectors);

    //������е�affector
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

    //����Ӱ����
    //void            LoadAffector(CMarkupSTL& xml);

    void            LoadEmitter(CMarkupSTL& xml, VECTOR_EMITTER& emitters);

    //����Ӱ����
    void            LoadAffector(CMarkupSTL& xml, VECTOR_AFFECTOR& affectors);

    //����buffer
    //void  DestoryBuffers();
private:
    //IMaterial* m_pMaterial;
    //BillboardSet*   m_pBillboardSet;        //������Ⱦ����

    float           m_fVisableTime;         //����ʱ�䣬0Ϊ����ʱ��
    u32             m_nPoolSize;            //�����޶�
    /// The number of emitted emitters in the pool.
    size_t          m_nEmittedEmitterPoolSize;  //������ķ������޶�
    float           m_fDefaultWidth;        //Ĭ�Ͽ��
    float           m_fDefaultHeight;       //Ĭ�ϸ߶�
    bool            m_bCullBackFace;        //��ǰ�Ƿ�����ӽ��б����޳�

    int             m_nTrailSegmentCount;   //��β����
    int             m_nMaxChainElement;   //��β������

    float           m_fInitTrailWidth; //��β�ĳ�ʼ���
    float           m_fTrailWidthChange; //��β�Ŀ�ȱ仯

    bool			m_bTrailUseVertexColor;	//��β�Ƿ�ʹ�ö�����ɫ
    bool			m_bTrailUseTexCoord;	//��β�Ƿ�ʹ����������

    Vector3Df       m_vCommonDir;           //����ϵͳ�Ĺ�ͬ����
    Vector3Df       m_vCommonUpDir;         //����ϵͳ�Ĺ�ͬ�Ϸ���

    AIRString			m_szParticleRenderableResource;	//����ϵͳ����Ⱦ������Դ
    ParticleSystemType	m_ParticleSystemType; //����ϵͳ����Ⱦ����

    AIRString		m_szTrailTex; //��β������Դ
    BillBoardChain::TexCoordDirection m_TCD; //��β��������

    float           m_fEmitEndTime;     //�������ʱ�䣬������ϵͳ������ʱ�䵽��ʱ�䣬�������������䣬0���ʾ��ʱ����Ч

    BillboardType   m_billboardType;        //billboard����

    bool m_bCreateTrail;
    bool m_bBoundsAutoUpdate;    //�Ƿ��Զ�����boundingbox
    bool m_bHasTrial;    //�Ƿ�����β

    //����ϵͳ�Ĳ�������
    AIRString		m_szSrcBlend;	// Դ��ϲ���
    AIRString		m_szDestBlend;	// Ŀ���ϲ���
    AIRString		m_szBlendMode;	// ���ģʽ

    //VECTOR_AFFECTOR m_vtParticleAffector;   //����Ӱ����
    //VECTOR_EMITTER  m_vtParticleEmitter;    //���ӷ�����

    AABBBox3Df m_boundingBox;

    AIRString m_strParticleTexture;   //��������
    ITexture* m_pParticleTexture;     //��������

    bool m_bLocalSpace;

    //CMarkupSTL m_xml;
};


