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

    //�����ֵĹ��캯����һ����Ҫ�ŵ�����ÿ֡update�����
    //Renderable(const char* szName);
    virtual ~Renderable();

    virtual const AIRString&  GetTypeString( void ) const; 

    virtual void Render(CCamera* pCamera, eRenderFlow flow = FLOW_SCENE);

    //���ļ��м��ض��� 
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

    //��ն���
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

    //��ò���
    virtual IMaterial* GetMaterial()
    {
        return m_pMaterial;
    }

    //������ĵ�
    virtual const Vector3Df& GetCenter() const
    {
        return m_vtCenter;
    }

    //�Ƿ�Ϊ͸��
    virtual bool IsTransparent() const;

    //����Ϊ͸��
    virtual void SetTransparent(bool bAlpha = true, u8 nAlpha = 128);

    //���õ�ǰ�Ķ���
    virtual void SetCurrentAnimation(const char* szAnimationName, float fBlendTime = 0.1f);

    //���ð󶨵�scenenode
    virtual void SetAttachedNode(CSceneNode* pNode);

    //���Object Space��boundingbox
    const AABBBox3Df& GetBoundingBox() const
    {
        return m_boundingBox;
    }

    virtual const AABBBox3Df&   GetWorldBoundingBox(bool bTransform);

    //virtual void                SetRenderableType(RenderableType type);

    //RenderableType              GetRenderableType() const;

    // �����Ƿ���ʾ��Χ��
    void                        SetShowBoundingBox( bool b );

    bool                        GetShowBoundingBox( void ) const
    {
        return m_bShowBoundingBox;
    }

    //��ø��׽ڵ�
    virtual CSceneNode*         GetNodeParent( void )
    {
        return m_pNode;
    }

    //��Ⱦ�ӱ��еĸ���
    virtual void                UpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue);

	// ���ڵ���º�,ˢ����Ⱦ����Ϣ
    //virtual void				RefreshByParentNode( void ) {}

    //�����Ƿ�Ҫ��ʾ
    virtual void                SetVisable(bool bVisable);

    //��֪ͨҪ�ƶ�
    virtual void                NotifyMove() {}

    //���ù��գ�ע�⣬�����ǹ̶����ߵĹ���
    virtual void                EnableLights(bool bEnable);

    bool						IsVisable()
    {
        return m_bVisable;
    }

    //�����������ϵͳ�е�λ��
    const Vector3Df&			GetWorldPosition() const;

    //���þ���任
    //virtual void  SetWorldTransform(CPass* pPass);

    //�������
    //virtual void                ProcessLights(CPass* pPass);

	virtual void				Reset( void ) {}

	virtual void				Update( float fTimeElapse ) {}

	//CRenderData*				GetRenderData( void ){ return &m_renderData; }

    //�Ƿ������Ӱ
    virtual void AcceptShadowmap(bool bAccept) 
    {
        m_bAcceptShadow = bAccept;
    }

    virtual CAIRAnimationState*  GetCurrentNodeAnimationState()
    {
        return m_pCurNodeAnimState;
    }

    //������Ⱦ����Ӱ��technique������shadowmap
    //virtual void GenRenderToShadowTechnique();

    //��Ⱦ��GBuffer��technique�������ӳ���Ⱦ
    //virtual void GenRenderToGBufferTechnique() {}

    //��Ⱦ����Ӱ
    //virtual void RenderToShadowMap(CCamera* pCamera);

    //��Ⱦ��GBuffer�������ӳ���Ⱦ
    //virtual void RenderToGBuffer(CCamera* pCamera);

	//virtual void RenderToDepthMap(CCamera* pCamera, CPass* pPass);

    //������Ⱦ�����ͼ��technique
    //virtual void GenRenderToDepthMapTechnique();

    bool IsRenderToShadow() const
    {
        return m_bRenderToShadow;
    }

    virtual void SetRenderToShadow(bool bRenderToShadow)
    {
        m_bRenderToShadow = bRenderToShadow;
    }

    //�ж��Ƿ������Ӱ
    virtual bool IsAcceptShadowMap() const
    {
        return m_bAcceptShadow;
    }

    //�Ƿ�����ӳ���Ⱦ�Ĺ���
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

    // �Ƿ�
    virtual bool IsPick(const Vector3Df& vOrg, const Vector3Df& vPickDir)
    {
        return IsPickByWorldBoundingBox(vOrg, vPickDir);
    }

    // ��������Ƿ����ģ��,�������,distFromEyeToObject��¼������е㵽�������ľ���
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

    // �����ɫ
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

    //��ʾboundingbox
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

    //����ʵ������������Ⱦ����
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
    // ��Ⱦ��Χ��
    //void                        RenderBoundingBoxImpl( void );

    // ��ȾǰҪ������
    virtual void                PreRender(CCamera* pCamera) {}

	virtual void				AfterRender( void );

    //��ն����ʵ��
    virtual void                ClearRenderableImp() {};

    //��ʼ��
    virtual bool                Initialize();

    //�û�ȥʵ�ֵ���Ⱦ
    //virtual void                RenderImpl(IRenderer* pRenderer);

    //��ӵ���Ⱦ����Ҫ��������
    virtual void  OnUpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue);
protected:
	typedef AIRList<CLight*>								LightList;
	//typedef std::map<string, CShaderParameter*>				SHADERPARAM;
 //   typedef std::map<string, CShaderParameter*>::iterator	SHADERPARAM_ITER;

    IMaterial*                          m_pMaterial;            //����
    CSceneNode*                         m_pNode;                //�����

    Matrix4f                            m_matWorld;             //�������
    Vector3Df                           m_vtCenter;             //���ĵ�
    AABBBox3Df                          m_boundingBox;          //Object space��Χ��
    AABBBox3Df                          m_WorldAABB;            //World space��Χ��
    //SHADERPARAM							m_mapShaderParam;       //shader��Ҫ�Ĳ���
    AIRString                           m_strName;              //����
    AIRString                           m_strResourceName;      //��Դ���ƣ������ļ���
    bool                                m_bVisable;             //�Ƿ�ɼ�
    bool                                m_bShowBoundingBox;     //�Ƿ���ʾ��Χ��
    bool                                m_bInitialize;          //�Ƿ��Ѿ���ʼ��
    bool                                m_bTransformByParent;   //�Ƿ�Ҫ�����ڵ�Ӱ��
    AIRString                           m_szType;               //�������
    CRenderData                         m_renderData;           //��Ⱦ����
    LightList                           m_lstAffectedLights;    //Ӱ���renderable��light�����ڹ̶�����
    u32									m_nID;

    bool                                m_bAcceptShadow;        //�Ƿ����shadowmap
    bool                                m_bRenderToShadow;      //�Ƿ�Ҫ��Ⱦ��shadwomap
    bool                                m_bAcceptLighting;      //�Ƿ�����ӳ���Ⱦ�Ĺ���

	BaseShape*                          m_pShape;               //��ײ��
    BoundingBoxEntity*                  m_boundingBoxEntity;    //��Ⱦboundingbox����
    SColor						        m_colorDiffuse;         // diffuse��ɫ

    Primitive*                          m_pPrimitive;       //��Ⱦ����
	CVertexDeclaration*                 m_pVertexDecl;      //��������

    bool                                m_bPrepared;
	bool                                m_bCollision;   //�Ƿ���ײ

	FixedString m_strVertexDecl;
public:
	bool								m_bIsCameraInAABB;

    //���Primitive����Ҫÿ���������أ��Ժ�Ҫ��ɴ��麯��
    virtual void _GetPrimitive() {};
private:


	//----------------------------------------------------
	//! add by hjj 2010-5-24 NodeAnimation implementation
public:
	typedef std::map<AIRString, NodeAnimation*>			NodeAnimationMap;
	typedef std::map<AIRString, CAIRAnimationState*>		NodeAnimationStateMap;

	// ������������
	virtual NodeAnimation*		CreateAnimation( const AIRString& szName, float fLength );

	// ��ö�������
	virtual NodeAnimation*		GetAnimation( const AIRString& szName );

	// �Ƿ���ڶ���
	virtual bool				HasAnimation(const AIRString& szName) const;

	// ���ٶ���, ��ͬ������Ӧ�Ķ���״̬����
	virtual bool				DestroyAnimation(const AIRString& name);

	// �������ж���, ��ͬ�������еĶ���״̬����
	virtual void				DestroyAllAnimations(void);

	// ��������״̬
	virtual CAIRAnimationState* CreateNodeAnimationState(const AIRString& animName);

	// ��ö���״̬
	virtual CAIRAnimationState* GetNodeAnimationState(const AIRString& animName);

	// �Ƿ���ڶ���״̬
	virtual bool				HasAnimationState(const AIRString& name) const;

	// ���ٶ���״̬
	virtual void				DestroyAnimationState(const AIRString& name);

	// �������ж���״̬
	virtual void				DestroyAllAnimationStates(void);

	// ���õ�ǰ�Ķ���
	virtual void				SetCurrentNodeAnimation( const AIRString& szName );

	// ÿ֡�������д򿪵Ķ���
	virtual void				UpdateNodeAnimation( float fTimeElapse );

    //�ύ��������
    virtual void                CommitNodeAnimation();

    //��ӽ�㶯��
    virtual void                AddNodeAnimation(NodeAnimation* pAnimation);

	//���õ�ǰ�Ľڵ㶯��
	virtual void				ResetNodeAnimation( void );

    MapIterator<Renderable::NodeAnimationMap> GetNodeAnimationIterator()
    {
        return MapIterator<Renderable::NodeAnimationMap>(m_mapNodeAnimations);
    }

    //�������õ�ǰtechnique
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

    ////�ύ������
    //virtual void                CommitTextureAnimation();
protected:
	NodeAnimationMap			m_mapNodeAnimations;		// �����б�
	NodeAnimationStateMap		m_mapNodeAnimationStates;	// ����״̬��
	CAIRAnimationState*			m_pCurNodeAnimState;		// ��ǰ�Ķ���

    
	//----------------------------------------------------
    u32  m_renderGroupType;    //���������������Ⱦ�������ĸ���Ⱦ����
    bool  m_bShareParentMaterial;     //�Ƿ������
	bool m_bForceRenderGroup;

    //������һЩ�߼���Ч��technique
protected:
    //CTechnique*			m_pTechniqueRenderToShadow;			//��Ⱦ����Ӱ��technique
    //CTechnique*         m_pTechniqueRenderToGBuffer;		//�ӳ���Ⱦ����Ⱦ��GBuffer
    //CTechnique*			m_pTechniqueRenderToDepthMap;		//��Ⱦ�����ͼ
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