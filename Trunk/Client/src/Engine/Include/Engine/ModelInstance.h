#pragma once

#include "Renderable.h"
#include "AIRModel.h"
#include "MarkupSTL.h"
#include "AIRBoneInstance.h"
#include "AIRAnimationState.h"
#include "AIRAnimationBlend.h"
#include "MeshInstance.h"

class AIRAnimationMixer;

class EngineExport CModelInstance : public Renderable
{
public:
    friend class CAIRBoneInstance;
public:
    // ����ʵ������
    typedef AIRVector<CAIRBoneInstance*>              VECTOR_BONEINSTANCE;
    typedef VECTOR_BONEINSTANCE::iterator               BONEINSTANCE_ITER;

    // ����������
    typedef AIRVector<CAIRSubMesh*>                   MEST_VEC;
    typedef AIRVector<CAIRSubMesh*>::iterator         MESH_ITER;

    // ����ʵ������
    typedef AIRVector<CMeshInstance*>                 MESHINST_VEC;
    typedef AIRVector<CMeshInstance*>::iterator       MESHINST_ITER;

    // ����Ⱦ������
    typedef AIRList<Renderable*>         RenderableList;
    typedef RenderableList::iterator        RenderableListIter;

    // ����״̬����
    typedef AIRMap<AIRString, CAIRAnimationState*>  AnimationStateMap;
    typedef AnimationStateMap::iterator                 AnimationStateMap_Iter;

	// ģ�͵�lodѡ�����,һ����ģ�͵�������ľ������Ϊ����ֵ
	typedef AIRVector<float>							LOD_STRATEGY;

    friend class CAIRSubMesh;


public:
    CModelInstance();
    virtual ~CModelInstance();

    // ��Ⱦ
    virtual void				Render(CCamera* pCamera);

    // ���ļ�����
    virtual bool				LoadFromFile(const char* szFilename);

    // ģ���Ƿ�͸����ʾ
    virtual bool				IsTransparent();

    // ��õ�ǰ�Ķ���
    virtual void				SetCurrentAnimation(const char* szAnimationName, float fBlendTime = 0.2f );

	virtual void				SetCurrentAnimationImmediate( const char* szAnimationName );

    // ��õ�ǰ�������ŵ�֡
    int							GetCurrentFrame() const { return m_nCurFrame; }

    // ��õ�ǰ�Ķ���״̬
    CAIRAnimationState*			GetCurrentAnimation() 
    { 
        return m_pCurAnimation; 
    }

	// ��ö�����϶���
    CAIRAnimationBlend*			GetAnimBlend( void )
    {
        return &m_animationBlend;
    }

    // �Ƿ�Ϊ��̬ģ��
    virtual bool				IsStatic()    { return m_bStatic; }

    // ģ���Ƿ����������ں�
    virtual bool				IsAnimationBlending( void );

    // ����ģ����Ϣ
    virtual void				Update(float fTimeElapsed);

    // ����ģ��͸����ʾ������
    virtual void				SetTransparent(bool bAlpha = true, u8 nAlpha = 128);

    // �������ӵĽڵ�
    virtual void				SetAttachedNode(CSceneNode* pNode);

    // ��ù���ʵ��
    CAIRBoneInstance*			GetBoneInst(const AIRString& strName);

    // ��ù���ʵ��
    CAIRBoneInstance*			GetBoneInst(u32 nBoneID);

	// �����Ⱦ�ﵽĳ����
	virtual Renderable*		AttachRenderableToBone(const AIRString& strBoneName, Renderable* pRenderable);

	// ������ϵ���Ⱦ��
	Renderable*		DetachRenderableFromBone(const AIRString& strBoneName, Renderable* pRenderable);

    // ������ϵ���Ⱦ��
	//virtual Renderable*		DetachRenderableFromBone(const AIRString& strBoneName, const AIRString& strRenderableName);

	// ���ĳ�����ϵ�������Ⱦ��
	virtual void				DetachAllRenderableFromBone(const AIRString& strBoneName);

    // ��ö���ʵ��
    virtual CAIRAnimationState* GetAnimationState(const AIRString& strName);

    // ��ö���ʵ��
    virtual CAIRAnimationState* GetAnimationState(int nIndex);

    // ��ö���״̬����
    size_t						GetAnimationStateCount( void )  { return m_mapAnimationState.size(); }

    // ���µ����
    virtual void				UpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue);

    // ��ù�������
    virtual size_t				GetBoneInstanceCount() { return m_vtBones.size(); }

    // ������������ʵ��
    CAIRSubMesh*		GetSubMesh(size_t nIndex);

    // �������������
    size_t						GetSubMeshCount( void )         { return m_vtSubMeshes.size(); }

    // �������ʵ������
    CMeshInstance*	GetMeshInstance( size_t nIndex );

    // �������ʵ������
    size_t						GetMeshInstanceCount( void )    { return m_vtMeshInst.size(); }

    // ������ӵ�ģ�͵Ŀ���Ⱦ�������
    size_t						GetAttachObjsCount( void )      { return m_lstAttachObjs.size(); } 

	// ���ģ����Ϣ
    AIRModel* GetModel( void ) { return m_pModel; }

	// ��õ�ǰ����Ⱦ����
	//virtual void				SetCurrentTechnique( const AIRString& szName );

	// �򿪹̶����߹���
    virtual void				EnableLights(bool bEnable);

	// ������Ⱦģ������ķ���
	virtual void				SetDrawMeshNormal( bool b );

	// ������Ӱ����
    virtual void				GenRenderToShadowTechnique();

	// ��Ⱦ��Ӱ
    //virtual void				RenderToShadowMap(CCamera* pCamera);

	//virtual void				RenderToDepthMap(CCamera* pCamera, CPass* pPass);

	// �Ƿ�
    virtual bool				IsPick(const Vector3Df& vOrg, const Vector3Df& vPickDir);

	// ��������Ƿ����ģ��,�������,distFromEyeToObject��¼������е㵽�������ľ���
	virtual bool				IsPick(const Vector3Df& vOrg, const Vector3Df& vPickDir, float& distFromEyeToObject);

    // ����shadow
    virtual void				AcceptShadowmap(bool bAccept);

    // �ύ��������
    //void						CommitAnimation();

	// ��ù�����Դ
    //CAIRSkeletonRes*			GetSkeletonRes()
    //{
    //    return m_pSkeletonRes;
    //}

	// ��ö��㶯����Դ
    AIRKeyframeAnimationRes*	GetVertexAnimationRes()
    {
        return m_pVertexAnimationRes;
    }

	// ��ù�������
    Matrix4f* GetBoneMatrices()
    {
        return m_boneMatrix;
    }

	// ����ģ�͵�ǰ��lod����
	void SetCurLodLevel( u16 nCurLodLevel );

	// ���ģ�͵�ǰ��lod����
	u16 GetCurLodLevel( void )
	{
		return m_nCurLodLevel;
	}

    //�����Ƿ���������Ⱦ
    virtual void SetInstancingEnable(bool bInstancing);

    //�ж��Ƿ���������Ⱦ
    virtual bool IsInstancingEnable() const
    {
        return m_bInstancing;
    }

    virtual void				SetAcceptLighting(bool bAccept);

    virtual const AABBBox3Df&   GetWorldBoundingBox(bool bTransform);

	//���ģ�͵�lod�жϲ�������
	LOD_STRATEGY*				GetLodStrategy( void )
	{
		return &m_vtLodStrategy;
	}

	//�����Ƿ�ʹ�������⼼��
	void SetUseTexLuminance( bool b );

	//�Ƿ����巢��
	void SetTexLuminanceWholeModel( int n );

	//���ģ�͵�ǰ�Ƿ�ʹ��������
	bool GetUseTexLuminance( void )
	{
		return m_bUseTexLuminance;
	}

	void InsertTexLuminanceColorPair( float time, CColorValue& col, Vector4f& strength )
	{
		for (size_t i = 0; i < m_vtMeshInst.size(); ++i)
		{
			m_vtMeshInst[i]->InsertTexLuminanceColorPair( time, col, strength );
		}
	}

	void DeleteTexLuminanceColorPair( float time )
	{
		for (size_t i = 0; i < m_vtMeshInst.size(); ++i)
		{
			m_vtMeshInst[i]->DeleteTexLuminanceColorPair( time );
		}
	}

	void ClearTexLuminanceInfo( void )
	{
		for (size_t i = 0; i < m_vtMeshInst.size(); ++i)
		{
			m_vtMeshInst[i]->ClearTexLuminanceInfo();
		}
	}

	//���õ�ǰ����Ⱦ����
    virtual void ResetCurrentTechnique();

    //���õ�ǰ�Ķ������
    void SetCurrentAnimationMixer(const char* szMixName, float fDelayIn = 0.2f, float fDelayOut = 0.2f);

    //�������������
    void  GenAnimationMixers();

    AIRAnimationMixer* GetCurrentAnimationMixer() 
    {
        return m_pCurrentAnimationMixer;
    }

    //ֹͣ����
    void StopAnimation();

    //��ͣ����
    void PauseAnimation(bool bPause);

    virtual void SetRenderToShadow(bool bRenderToShadow);

    virtual bool Prepare();

    u8 GetAnimationType() const
    {
        return m_pModel->GetAnimationType();
    }

    virtual void FillRenderContext(CCamera* pCamera, RenderContextQueue* pQueue, eRenderFlow flow);

protected:
    // ��Ⱦǰ��Ԥ����
    virtual void				PreRender(CCamera* pCamera);

    // �ͷſ���Ⱦ���ִ�к���
    virtual void				ClearRenderableImp();

    // ����ǵ�λ��
    virtual void				CaculateBones();

    // �����Χ��
    void						ComputeBoundingBox();

    // ���ɹ��ɶ���
    //@pCurFrame����ǰ֡�Ĺ���
    //@pNextFrame ��һ֡�Ĺ���
    //virtual void				CreateAnimationTransition(CAIRSkeleton* pCurFrame, CAIRSkeleton* pNextFrame);

    // ��������״̬
    virtual void				CreateAnimationStates();

	// �������㶯��״̬
    virtual void				CreateVertexAnimationStates(AIRKeyframeAnimationRes* pVertexAnimationRes);

    // �������״̬
    virtual void				ClearAnimationStates();

    // ���¶���
    virtual void				UpdateAnimation();

    // ��ù��������Ժ�Ҫ�ŵ�SkeletonInstance����
    virtual void				GetBoneMatrix();

    // ��������
    virtual void				BuildBones();

    // ������ģ��
    virtual void				CreateMeshInst();

    // ���ù���
    virtual void				ResetSkeleton();

	// ���µ���Ⱦ����
    virtual void				OnUpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue);

	// �Զ�ѡ��lod����
	virtual void				AutoSelectLod( CCamera* pCamera );

    

    //�������ж������
    void ClearAllAnimationMixers();
    
protected:
    bool						m_bInstancing;          // �Ƿ�Ҫ������Ⱦ
    u16							m_nCurLodLevel;			// ��ǰlod����
    //bool  m_bPrepared;
protected:
    
    int							m_nCurFrame;            // ��ǰ֡����̬ģ��û����
    int							m_nNextFrame;           // ��һ֡
    float						m_fInterpolate;         // ʱ����
    float						m_fAnimateTime;         // ����������ʱ��
    bool						m_bStatic;              // �Ƿ�̬
    CAIRAnimationState*			m_pCurAnimation;        // ��ǰ��������̬ģ��û����
    CAIRAnimationState*			m_pNextAnimation;       // ��һ�����������ɽ�����Ķ���
    CAIRAnimationState			m_animationTransition;  // ���ɶ���
    MEST_VEC					m_vtSubMeshes;          // ����������
    MESHINST_VEC				m_vtMeshInst;           // ����ʵ������
    VECTOR_BONEINSTANCE			m_vtBones;              // ������Ϣ�����ڹ���ʱ��������
	RenderableList				m_lstAttachObjs;        // ����ģ���ϵ���Ⱦ��
    AnimationStateMap			m_mapAnimationState;    // ģ�����ϴ��ڵĶ���
    Matrix4f*					m_boneMatrix;           // Ӱ�춥��Ĺ�������
    CAIRAnimationBlend			m_animationBlend;       // ���������
    
    //CAIRSkeletonRes*			m_pSkeletonRes;			// ����ʵ��
    AIRKeyframeAnimationRes*	m_pVertexAnimationRes;	// ���㶯��ʵ��
    bool						m_bAnimationUpdated;    // ���������Ƿ��Ѿ�����
    

	//bool                      m_bAnimationCommitted;  //�������������Ƿ��Ѿ����¹����˷����в�ͨ����Ϊ���������ݵ�����
    //bool						m_bHardwareSkeleton
    //CAIRSkeleton				m_curSkeleton;          // ��ǰʱ�̵Ĺ�����Ϣ
    //Matrix4f					m_matWorld;
    //Matrix4f					m_matRotate;
    //Matrix4f					m_matReflect;

	
	LOD_STRATEGY				m_vtLodStrategy;		// lod��������

	bool						m_bUseTexLuminance;		// �Ƿ�ʹ�������Է���

    AIRAnimationMixer*          m_pCurrentAnimationMixer;    //��ǰ�Ķ��������

    bool                        m_bPauseAnimation;

    AIRAnimationMixRes* m_pAnimationMixRes;
    

    typedef AIRMap<AIRString, AIRAnimationMixer*> MapAnimationMixer;
    typedef MapAnimationMixer::iterator MapAnimationMixerIter;
    MapAnimationMixer  m_mapAnimationMixer;

protected:
    //
    void AddBoneAttachedRenderable(Renderable* pRenderable);

    void RemoveBoneAttachedRenderable(Renderable* pRenderable);

    //׼��ģ������
    void PrepareModel();
    //׼��������������
    void PrepareSkeletonAnimation();
    //׼�������������
    void PrepareAnimationMixer();
    //׼�����㶯��
    void PrepareVertexAnimation();

    char m_szCurrentAnimation[256];

    AIRModel*					m_pModel;               // ģ��
};
