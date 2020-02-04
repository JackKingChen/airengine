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
    // 骨骼实例容器
    typedef AIRVector<CAIRBoneInstance*>              VECTOR_BONEINSTANCE;
    typedef VECTOR_BONEINSTANCE::iterator               BONEINSTANCE_ITER;

    // 子网格容器
    typedef AIRVector<CAIRSubMesh*>                   MEST_VEC;
    typedef AIRVector<CAIRSubMesh*>::iterator         MESH_ITER;

    // 网格实例容器
    typedef AIRVector<CMeshInstance*>                 MESHINST_VEC;
    typedef AIRVector<CMeshInstance*>::iterator       MESHINST_ITER;

    // 可渲染物容器
    typedef AIRList<Renderable*>         RenderableList;
    typedef RenderableList::iterator        RenderableListIter;

    // 动画状态容器
    typedef AIRMap<AIRString, CAIRAnimationState*>  AnimationStateMap;
    typedef AnimationStateMap::iterator                 AnimationStateMap_Iter;

	// 模型的lod选择策略,一般以模型到摄像机的距离参数为衡量值
	typedef AIRVector<float>							LOD_STRATEGY;

    friend class CAIRSubMesh;


public:
    CModelInstance();
    virtual ~CModelInstance();

    // 渲染
    virtual void				Render(CCamera* pCamera);

    // 从文件加载
    virtual bool				LoadFromFile(const char* szFilename);

    // 模型是否透明显示
    virtual bool				IsTransparent();

    // 获得当前的动画
    virtual void				SetCurrentAnimation(const char* szAnimationName, float fBlendTime = 0.2f );

	virtual void				SetCurrentAnimationImmediate( const char* szAnimationName );

    // 获得当前动画播放的帧
    int							GetCurrentFrame() const { return m_nCurFrame; }

    // 获得当前的动画状态
    CAIRAnimationState*			GetCurrentAnimation() 
    { 
        return m_pCurAnimation; 
    }

	// 获得动画混合对象
    CAIRAnimationBlend*			GetAnimBlend( void )
    {
        return &m_animationBlend;
    }

    // 是否为静态模型
    virtual bool				IsStatic()    { return m_bStatic; }

    // 模型是否在做动画融合
    virtual bool				IsAnimationBlending( void );

    // 更新模型信息
    virtual void				Update(float fTimeElapsed);

    // 设置模型透明显示的属性
    virtual void				SetTransparent(bool bAlpha = true, u8 nAlpha = 128);

    // 设置链接的节点
    virtual void				SetAttachedNode(CSceneNode* pNode);

    // 获得骨骼实例
    CAIRBoneInstance*			GetBoneInst(const AIRString& strName);

    // 获得骨骼实例
    CAIRBoneInstance*			GetBoneInst(u32 nBoneID);

	// 添加渲染物到某个骨
	virtual Renderable*		AttachRenderableToBone(const AIRString& strBoneName, Renderable* pRenderable);

	// 解除骨上的渲染物
	Renderable*		DetachRenderableFromBone(const AIRString& strBoneName, Renderable* pRenderable);

    // 解除骨上的渲染物
	//virtual Renderable*		DetachRenderableFromBone(const AIRString& strBoneName, const AIRString& strRenderableName);

	// 解除某条骨上的所有渲染物
	virtual void				DetachAllRenderableFromBone(const AIRString& strBoneName);

    // 获得动画实例
    virtual CAIRAnimationState* GetAnimationState(const AIRString& strName);

    // 获得动画实例
    virtual CAIRAnimationState* GetAnimationState(int nIndex);

    // 获得动画状态数量
    size_t						GetAnimationStateCount( void )  { return m_mapAnimationState.size(); }

    // 更新到结点
    virtual void				UpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue);

    // 获得骨骼数量
    virtual size_t				GetBoneInstanceCount() { return m_vtBones.size(); }

    // 获得子网格对象实例
    CAIRSubMesh*		GetSubMesh(size_t nIndex);

    // 获得子网格数量
    size_t						GetSubMeshCount( void )         { return m_vtSubMeshes.size(); }

    // 获得网格实例对象
    CMeshInstance*	GetMeshInstance( size_t nIndex );

    // 获得网格实例数量
    size_t						GetMeshInstanceCount( void )    { return m_vtMeshInst.size(); }

    // 获得连接到模型的可渲染物的数量
    size_t						GetAttachObjsCount( void )      { return m_lstAttachObjs.size(); } 

	// 获得模型信息
    AIRModel* GetModel( void ) { return m_pModel; }

	// 获得当前的渲染技术
	//virtual void				SetCurrentTechnique( const AIRString& szName );

	// 打开固定管线光照
    virtual void				EnableLights(bool bEnable);

	// 设置渲染模型网格的法线
	virtual void				SetDrawMeshNormal( bool b );

	// 创建阴影技术
    virtual void				GenRenderToShadowTechnique();

	// 渲染阴影
    //virtual void				RenderToShadowMap(CCamera* pCamera);

	//virtual void				RenderToDepthMap(CCamera* pCamera, CPass* pPass);

	// 是否
    virtual bool				IsPick(const Vector3Df& vOrg, const Vector3Df& vPickDir);

	// 检查射线是否点中模型,如果点中,distFromEyeToObject记录最近击中点到射线起点的距离
	virtual bool				IsPick(const Vector3Df& vOrg, const Vector3Df& vPickDir, float& distFromEyeToObject);

    // 接受shadow
    virtual void				AcceptShadowmap(bool bAccept);

    // 提交动画数据
    //void						CommitAnimation();

	// 获得骨骼资源
    //CAIRSkeletonRes*			GetSkeletonRes()
    //{
    //    return m_pSkeletonRes;
    //}

	// 获得顶点动画资源
    AIRKeyframeAnimationRes*	GetVertexAnimationRes()
    {
        return m_pVertexAnimationRes;
    }

	// 获得骨骼矩阵
    Matrix4f* GetBoneMatrices()
    {
        return m_boneMatrix;
    }

	// 设置模型当前的lod级别
	void SetCurLodLevel( u16 nCurLodLevel );

	// 获得模型当前的lod级别
	u16 GetCurLodLevel( void )
	{
		return m_nCurLodLevel;
	}

    //设置是否能批量渲染
    virtual void SetInstancingEnable(bool bInstancing);

    //判断是否能批量渲染
    virtual bool IsInstancingEnable() const
    {
        return m_bInstancing;
    }

    virtual void				SetAcceptLighting(bool bAccept);

    virtual const AABBBox3Df&   GetWorldBoundingBox(bool bTransform);

	//获得模型的lod判断策略数组
	LOD_STRATEGY*				GetLodStrategy( void )
	{
		return &m_vtLodStrategy;
	}

	//设置是否使用纹理发光技术
	void SetUseTexLuminance( bool b );

	//是否整体发光
	void SetTexLuminanceWholeModel( int n );

	//获得模型当前是否使用纹理发光
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

	//重置当前的渲染技术
    virtual void ResetCurrentTechnique();

    //设置当前的动作混合
    void SetCurrentAnimationMixer(const char* szMixName, float fDelayIn = 0.2f, float fDelayOut = 0.2f);

    //创建动作混合器
    void  GenAnimationMixers();

    AIRAnimationMixer* GetCurrentAnimationMixer() 
    {
        return m_pCurrentAnimationMixer;
    }

    //停止动画
    void StopAnimation();

    //暂停动画
    void PauseAnimation(bool bPause);

    virtual void SetRenderToShadow(bool bRenderToShadow);

    virtual bool Prepare();

    u8 GetAnimationType() const
    {
        return m_pModel->GetAnimationType();
    }

    virtual void FillRenderContext(CCamera* pCamera, RenderContextQueue* pQueue, eRenderFlow flow);

protected:
    // 渲染前的预处理
    virtual void				PreRender(CCamera* pCamera);

    // 释放可渲染物的执行函数
    virtual void				ClearRenderableImp();

    // 计算骨的位置
    virtual void				CaculateBones();

    // 计算包围盒
    void						ComputeBoundingBox();

    // 生成过渡动画
    //@pCurFrame，当前帧的骨骼
    //@pNextFrame 下一帧的骨骼
    //virtual void				CreateAnimationTransition(CAIRSkeleton* pCurFrame, CAIRSkeleton* pNextFrame);

    // 创建动画状态
    virtual void				CreateAnimationStates();

	// 创建顶点动画状态
    virtual void				CreateVertexAnimationStates(AIRKeyframeAnimationRes* pVertexAnimationRes);

    // 清除动画状态
    virtual void				ClearAnimationStates();

    // 更新动画
    virtual void				UpdateAnimation();

    // 获得骨骼矩阵，以后要放到SkeletonInstance类里
    virtual void				GetBoneMatrix();

    // 创建骨骼
    virtual void				BuildBones();

    // 创建子模型
    virtual void				CreateMeshInst();

    // 重置骨骼
    virtual void				ResetSkeleton();

	// 更新到渲染队列
    virtual void				OnUpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue);

	// 自动选择lod级别
	virtual void				AutoSelectLod( CCamera* pCamera );

    

    //销毁所有动作混合
    void ClearAllAnimationMixers();
    
protected:
    bool						m_bInstancing;          // 是否要批量渲染
    u16							m_nCurLodLevel;			// 当前lod级别
    //bool  m_bPrepared;
protected:
    
    int							m_nCurFrame;            // 当前帧，静态模型没作用
    int							m_nNextFrame;           // 下一帧
    float						m_fInterpolate;         // 时间间隔
    float						m_fAnimateTime;         // 动画运行了时间
    bool						m_bStatic;              // 是否静态
    CAIRAnimationState*			m_pCurAnimation;        // 当前动画，静态模型没作用
    CAIRAnimationState*			m_pNextAnimation;       // 下一个动作，过渡结束后的动画
    CAIRAnimationState			m_animationTransition;  // 过渡动画
    MEST_VEC					m_vtSubMeshes;          // 子网格容器
    MESHINST_VEC				m_vtMeshInst;           // 网格实例容器
    VECTOR_BONEINSTANCE			m_vtBones;              // 骨骼信息，存在骨骼时才起作用
	RenderableList				m_lstAttachObjs;        // 挂在模型上的渲染物
    AnimationStateMap			m_mapAnimationState;    // 模型身上存在的动画
    Matrix4f*					m_boneMatrix;           // 影响顶点的骨骼动画
    CAIRAnimationBlend			m_animationBlend;       // 动作混合器
    
    //CAIRSkeletonRes*			m_pSkeletonRes;			// 骨骼实例
    AIRKeyframeAnimationRes*	m_pVertexAnimationRes;	// 顶点动画实例
    bool						m_bAnimationUpdated;    // 动画数据是否已经更新
    

	//bool                      m_bAnimationCommitted;  //动画顶点数据是否已经更新过，此方案行不通，因为共享顶点数据的问题
    //bool						m_bHardwareSkeleton
    //CAIRSkeleton				m_curSkeleton;          // 当前时刻的骨骼信息
    //Matrix4f					m_matWorld;
    //Matrix4f					m_matRotate;
    //Matrix4f					m_matReflect;

	
	LOD_STRATEGY				m_vtLodStrategy;		// lod策略容器

	bool						m_bUseTexLuminance;		// 是否使用纹理自发光

    AIRAnimationMixer*          m_pCurrentAnimationMixer;    //当前的动作混合器

    bool                        m_bPauseAnimation;

    AIRAnimationMixRes* m_pAnimationMixRes;
    

    typedef AIRMap<AIRString, AIRAnimationMixer*> MapAnimationMixer;
    typedef MapAnimationMixer::iterator MapAnimationMixerIter;
    MapAnimationMixer  m_mapAnimationMixer;

protected:
    //
    void AddBoneAttachedRenderable(Renderable* pRenderable);

    void RemoveBoneAttachedRenderable(Renderable* pRenderable);

    //准备模型数据
    void PrepareModel();
    //准备骨骼动画数据
    void PrepareSkeletonAnimation();
    //准备动作混合数据
    void PrepareAnimationMixer();
    //准备顶点动画
    void PrepareVertexAnimation();

    char m_szCurrentAnimation[256];

    AIRModel*					m_pModel;               // 模型
};
