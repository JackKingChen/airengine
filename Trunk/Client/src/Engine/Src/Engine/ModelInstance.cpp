#include "stdafx.h"
#include "ModelInstance.h"
#include "Engine.h"
//#include "SEFps.h"
#include "SceneNode.h"
#include "FileSystem.h"
#include "AIRSkeletonAnimation.h"
#include <algorithm> 
#include "ISceneManager.h"
#include "TimeBaseProfile.h"
#include "InstancingObjectManager.h"
#include "AIRAnimationMixer.h"
#include "StringUtil.h"
#include "SceneDebug.h"

CModelInstance::CModelInstance() : m_nCurFrame(0), 
m_nNextFrame(0),
m_fInterpolate(0.0f), 
m_fAnimateTime(0.0f), 
m_bStatic(false), 
m_pCurAnimation(NULL),
m_pNextAnimation(NULL),
//m_pSkeletalRes(NULL),
m_boneMatrix(NULL)
//m_colorDiffuse(0xffffffff)
//, m_pSkeletonRes(NULL)
, m_pVertexAnimationRes(NULL)
//, m_bAnimationCommitted(false)
, m_bAnimationUpdated(false)
, m_bInstancing(false)
, m_pCurrentAnimationMixer(NULL)
, m_bPauseAnimation(false)
, m_nCurLodLevel(0)
, m_pModel(NULL)
, m_pAnimationMixRes(NULL)
//, m_bPrepared(false)
{
    m_szType = "model";
    SetRenderToShadow(true);
	m_bUseTexLuminance = false;
    m_bAcceptLighting = true;
    m_renderGroupType = eRGT_Solid;
    memset(m_szCurrentAnimation, 0, sizeof(m_szCurrentAnimation));
    m_bAcceptShadow = true;
}

CModelInstance::~CModelInstance()
{

}

bool CModelInstance::LoadFromFile(const char* szFilename)
{
    //创建对应的骨骼与动画
    m_pModel = RESOURCE_MANAGER->CreateResource<AIRModel>(szFilename); //MODELMGR->CreateModel(szFilename);

    if (!m_pModel)
    {
        return false;
    }
    
    SetResourceName(CStringUtil::GetFilename(szFilename));

    if (m_pModel->IsLoaded())
    {
        if (!m_pModel->IsReady())
        {
            m_pModel->Cache();
        }
        
        //if (m_pModel->IsReady())
        //{
        //    PrepareModel();

        //    m_bPrepared = true;
        //}
        
    }

    
    return true;
}


void CModelInstance::GenAnimationMixers()
{
    //先清空原来的
    ClearAllAnimationMixers();
    m_pAnimationMixRes = m_pModel->GetAnimationMixRes();
    if (m_pAnimationMixRes && m_pAnimationMixRes->IsReady())
    {
        MapIterator<AIRAnimationMixRes::MapAnimationMixData> iter = m_pAnimationMixRes->GetAnimationMixDataIter();
        while (!iter.IsEnd())
        {
            AIRAnimationMixData* pData = iter.GetNext();
            AIRAnimationMixer* pAnimationMixer = new AIRAnimationMixer(pData->GetName().c_str(), pData);
            m_mapAnimationMixer.insert(std::make_pair(pAnimationMixer->GetName(), pAnimationMixer));

            MapIterator<AIRAnimationMixData::MapAnimationWeights> weight_iter = pData->GetAnimationActionWeightsIter();
            while (!weight_iter.IsEnd())
            {
                CAIRAnimationState* pState = GetAnimationState(weight_iter.PeekNextKey()->GetName());
                if (pState)
                {
                    pAnimationMixer->AddAnimationStateAction(pState);
                }
                weight_iter.MoveNext();
            }

            weight_iter = pData->GetAnimationCircleWeightsIter();
            while (!weight_iter.IsEnd())
            {
                CAIRAnimationState* pState = GetAnimationState(weight_iter.PeekNextKey()->GetName());
                if (pState)
                {
                    pAnimationMixer->AddAnimationStateCircle(pState);
                }
                weight_iter.MoveNext();
            }
        }
    }
}

void CModelInstance::SetDrawMeshNormal( bool b )
{
	for (MESHINST_ITER it = m_vtMeshInst.begin(); it != m_vtMeshInst.end(); ++it)
	{
		//pRenderer->SetWorldTransform(&m_matWorld);
		CMeshInstance* pMesh = *it;
		pMesh->GetMeshRes()->SetDrawMeshNormal( b );
	}
}



void CModelInstance::Render(CCamera* pCamera)
{
    //if ( !m_bVisable )
    //{
    //    return;
    //}

    PreRender(pCamera);

    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    //m_matWorld.Identity();
    //Matrix4f matWorld;
    //m_pNode->GetWorldTransform(&matWorld);
    if ((m_pNode && m_bStatic) || m_pVertexAnimationRes)
    {
        pRenderer->SetWorldTransform(&m_pNode->GetFullTransform());
    }
    else
    {
        if (m_pCurAnimation == NULL && m_pNode)
        {
            pRenderer->SetWorldTransform(&m_pNode->GetFullTransform());
        }
        else
		{
            pRenderer->SetWorldTransform(&Matrix4f::IDENTITY);
		}
    }

	CMeshInstance* pMesh = NULL;
    for (MESHINST_ITER it = m_vtMeshInst.begin(); it != m_vtMeshInst.end(); ++it)
    {
        //pRenderer->SetWorldTransform(&m_matWorld);
        pMesh = *it;
        pMesh->Render(pCamera);
    }

	Renderable::AfterRender();
    //OutputDebugString("model instance render commit!\n");

  //  for (RenderableListIter rit = m_lstAttachObjs.begin(); rit != m_lstAttachObjs.end(); ++rit)
  //  {
		//rit->second->Render(pCamera);
  //  }

    //RenderBoundingBoxImpl();
}

void CModelInstance::SetTransparent(bool bAlpha /* = true */, u8 nAlpha /* = 128 */)
{
    //for (MESH_ITER it = m_vtSubMeshes.begin(); it != m_vtSubMeshes.end(); ++it)
    //{
    //    CAIRSubMesh* pMesh = *it;
    //    pMesh->SetTransparent(bAlpha, nAlpha);
    //}
    if (bAlpha)
    {
        m_colorDiffuse.SetAlpha(nAlpha);
    }
    else
    {
        m_colorDiffuse.SetAlpha(255);
    }
    MESHINST_ITER itEnd = m_vtMeshInst.end();
    for (MESHINST_ITER it = m_vtMeshInst.begin(); it != itEnd; ++it)
    {
        (*it)->SetTransparent(bAlpha, nAlpha);
    }
    
}

void CModelInstance::PreRender(CCamera* pCamera)
{
    //m_pMaterial->Update(pCamera, this);
    //char szDebugStr[256] = { 0 };
    //sprintf(szDebugStr, "modelinstance:%x\n", this);
    //OutputDebugStr(szDebugStr);

	//AutoSelectLod(pCamera);


	//BEGIN_FUNC_PROFILE;
    
    if (m_pCurAnimation)
    {
        if (ENGINE_INST->IsHardwareSkeleton())
        {
			IRenderer* pRenderer = ENGINE_INST->GetRenderer();
			pRenderer->GetGPUDataParam()->SetBoneMatrices(m_boneMatrix, m_pModel->GetBoneInstanceCount());
			pRenderer->GetGPUDataParam()->SetColorDiffuse(m_colorDiffuse);
            
        }
        else// if (!m_bAnimationCommitted)
        {
			CAIRSubMesh* pSubMesh = NULL;
            MESH_ITER itEnd = m_vtSubMeshes.end();
            for (MESH_ITER it = m_vtSubMeshes.begin(); it != itEnd; ++it)
            {
				//BEGIN_FUNC_PROFILE;

                pSubMesh = *it;
                pSubMesh->CaculateVertices(m_colorDiffuse, m_boneMatrix, m_pCurAnimation == NULL ? 0 : m_pCurAnimation->GetTimePos());

				//END_FUNC_PROFILE_WITH_REPORT( GetString("update sub mesh : %s", pSubMesh->GetName().c_str()) );
            }
        }
        
        //m_bAnimationCommitted = true;
        
    }

	//END_FUNC_PROFILE_WITH_REPORT( GetString("----------------------update model inst : %s", m_strName.c_str()) );
    
}


void CModelInstance::CaculateBones()
{
    if (m_animationBlend.IsBlending())
    {
        CAIRAnimationState* pState = m_animationBlend.GetSource();
        CAIRAnimation*      pAnimation = pState->GetAnimationRes();
        pAnimation->Apply(this, pState->GetTimePos(), pState->GetWeight());
        pState = m_animationBlend.GetTarget();
        pAnimation = pState->GetAnimationRes();
        pAnimation->Blend(this, pState->GetTimePos(), pState->GetWeight());
    }
    else if (m_pCurrentAnimationMixer)
    {
        m_pCurrentAnimationMixer->Apply(this);
    }
    else if (m_pCurAnimation)
    {
        //新代码，用于动作混合
        CAIRAnimation* pAnimation = m_pCurAnimation->GetAnimationRes();
        pAnimation->Apply(this, m_pCurAnimation->GetTimePos(), m_pCurAnimation->GetWeight());
        //OutputDebugString("apply skeleton animation \n");
    }
    
}

void CModelInstance::ClearRenderableImp()
{
    // 模型实例的场景节点
    if (m_pNode)
    {
		CAIRBoneInstance* pBoneInst = NULL;
        // 遍历所有的骨骼节点
        for (BONEINSTANCE_ITER it = m_vtBones.begin(); it != m_vtBones.end(); ++it)
        {
            // 取得骨骼节点
            pBoneInst = *it;
            // 从场景节点移除骨骼节点
            m_pNode->RemoveChild(pBoneInst);
            //pBoneInst->SetParent(NULL);
            // 销毁骨骼节点
            //SafeDelete(pBoneInst);
        }
        m_pNode->DetachRenderable(this);
        
    }

    for (BONEINSTANCE_ITER it = m_vtBones.begin(); it != m_vtBones.end(); ++it)
    {
        // 取得骨骼节点
        CAIRBoneInstance* pBoneInst = *it;
		pBoneInst->DetachAllRenderable();
        //pBoneInst->SetParent(NULL);
        // 从场景节点移除骨骼节点
        //m_pNode->RemoveChild(pBoneInst);
        // 销毁骨骼节点
        SafeDelete(pBoneInst);
    }

    //MODELMGR->ClearModel(m_pModel);
    if (m_pModel)
    {
        RESOURCE_MANAGER->ClearResource(m_pModel);
    }
    
    
    for (MESHINST_ITER it = m_vtMeshInst.begin(); it != m_vtMeshInst.end(); ++it)
    {
        CMeshInstance* pMesh = *it;
        pMesh->Clear();
        SafeDelete(pMesh);
    }
    m_vtMeshInst.clear();

    m_vtBones.clear();

	ISceneManager* pScene = ENGINE_INST->GetSceneMgr();
	for (RenderableListIter rit = m_lstAttachObjs.begin(); rit != m_lstAttachObjs.end(); ++rit)
	{
		pScene->DestroyRenderable(*rit);
    }
	m_lstAttachObjs.clear();

    ClearAnimationStates();

    //if (m_pSkeletalRes)
    //{
    //    MODELMGR->ClearSkeletonRes(m_pSkeletalRes);
    //}
    if (m_boneMatrix)
    {
        SafeDeleteArray(m_boneMatrix);
    }
    m_vtSubMeshes.clear();

    ClearAllAnimationMixers();
}

void CModelInstance::ClearAllAnimationMixers()
{
    MapAnimationMixerIter itEnd = m_mapAnimationMixer.end();
    for (MapAnimationMixerIter it = m_mapAnimationMixer.begin(); it != itEnd; ++it)
    {
        AIRAnimationMixer* pMixer = it->second;
        SafeDelete(pMixer);
    }
    m_mapAnimationMixer.clear();
}

bool CModelInstance::IsTransparent()
{
    if (m_pMaterial)
    {
        return m_pMaterial->IsTransparent();
    }

    MESHINST_ITER it = m_vtMeshInst.begin();
    if (it != m_vtMeshInst.end())
    {
        CMeshInstance* pMesh = *it;
        if (pMesh)
        {
            return pMesh->IsTransparent();
        }
    }
    

    return false;
}

void CModelInstance::SetCurrentAnimation(const char* szAnimationName, float fBlendTime)
{
    
    if (!m_bStatic)
    {
        //OutputDebugString( GetString("set current animation: %s\n", szAnimationName) );
        strcpy(m_szCurrentAnimation, szAnimationName);

		//如果当前动画状态的名字和将要设置的动画名字一直,就直接返回
        if (m_pCurAnimation && strcmp(m_pCurAnimation->GetName().c_str(), szAnimationName) == 0)
        {
            return;
        }

        //OutputDebugString("character change animation!\n");

		//如果当前动画存在
        if ( m_pCurAnimation )
        {
			//获得目标动画状态
			CAIRAnimationState* pState = GetAnimationState(szAnimationName);

			//如果当前不是在动画混合状态
			if ( !m_animationBlend.IsBlending() )
			{
				//设置时间点为0
				pState->SetTimePos(0);
				//开始执行动画混合
				m_animationBlend.StartBlend(CAIRAnimationBlend::Blend_Transition, m_pCurAnimation, pState, fBlendTime);
			}
			else
			{
				m_animationBlend.StartBlend(CAIRAnimationBlend::Blend_Transition, m_pCurAnimation, pState, m_pCurAnimation->GetTimePos(), fBlendTime);
			}
        }
        else
        {
            m_pCurAnimation = GetAnimationState(szAnimationName);
            if (m_pCurAnimation)
            {
                m_pCurAnimation->SetTimePos(0.0f);
            }
        }
    }
}


void CModelInstance::SetCurrentAnimationImmediate( const char* szAnimationName )
{
    strcpy(m_szCurrentAnimation, szAnimationName);
	if (!m_bStatic)
	{
		m_pCurAnimation = GetAnimationState(szAnimationName);
		m_animationBlend.StartBlend(CAIRAnimationBlend::Blend_Transition, m_pCurAnimation, m_pCurAnimation, 0);
		m_animationBlend.StopBlend();
		m_pCurAnimation->SetTimePos(0.0f);
	}
}


bool CModelInstance::IsAnimationBlending( void )
{
    return m_animationBlend.IsBlending();
}

void CModelInstance::Update(float fTimeElapsed)
{
	//如果正在做动画混合
    if (m_animationBlend.IsBlending())
    {
		//更新动画混合机
        m_animationBlend.Update(fTimeElapsed);
		//如果已经完成动画混合
        if (!m_animationBlend.IsBlending())
        {
			//设置当前的动画为目标动画
            m_pCurAnimation = m_animationBlend.GetTarget();
            if (strcmp(m_pCurAnimation->GetName().c_str(), m_szCurrentAnimation) != 0)
            {
                SetCurrentAnimation(m_szCurrentAnimation);
            }
        }

        //for (size_t i = 0; i < m_vtMeshInst.size(); ++i)
        //{
        //    m_vtMeshInst[i]->SetVerticesCaculate(false);
        //}
        //DT_TO_DBGSTR(LT_COMMAND, GetString("AnimationBlending, time elapsed: %f, blend remain:%f\n", fTimeElapsed, m_animationBlend.GetBlendRemain()));
    }
    else if (m_pCurrentAnimationMixer)
    {
        m_pCurrentAnimationMixer->Update(fTimeElapsed);

        //for (size_t i = 0; i < m_vtMeshInst.size(); ++i)
        //{
        //    m_vtMeshInst[i]->SetVerticesCaculate(false);
        //}
    }
    else if (m_pCurAnimation && !m_bPauseAnimation)
    {

        m_pCurAnimation->AddTime(fTimeElapsed);
        
        
        //for (size_t i = 0; i < m_vtMeshInst.size(); ++i)
        //{
        //    m_vtMeshInst[i]->SetVerticesCaculate(false);
        //}
    }

    //m_bAnimationCommitted = false;  
    m_bAnimationUpdated = false;
	//-----------------------------------
	//! add by hjj 2010-5-24
	Renderable::UpdateNodeAnimation(fTimeElapsed);
	//-----------------------------------

	//this->AutoSelectLod( ENGINE_INST->GetSceneMgr()->GetCamera() );
}

void CModelInstance::ComputeBoundingBox()
{
    m_boundingBox.MaxEdge = Vector3Df(-99999.0f, -99999.0f, -99999.0f);
    m_boundingBox.MinEdge = Vector3Df(99999.0f, 99999.0f, 99999.0f);
	size_t nSubMeshCount = m_vtSubMeshes.size();
	CAIRSubMesh* pAIRMesh = NULL;
    for (size_t i = 0; i < nSubMeshCount; i++)
    {
        pAIRMesh = m_vtSubMeshes[i];
        m_boundingBox.MaxEdge.x = Max(m_boundingBox.MaxEdge.x, pAIRMesh->GetBoundingBox().MaxEdge.x);
        m_boundingBox.MaxEdge.y = Max(m_boundingBox.MaxEdge.y, pAIRMesh->GetBoundingBox().MaxEdge.y);
        m_boundingBox.MaxEdge.z = Max(m_boundingBox.MaxEdge.z, pAIRMesh->GetBoundingBox().MaxEdge.z);

        m_boundingBox.MinEdge.x = Min(m_boundingBox.MinEdge.x, pAIRMesh->GetBoundingBox().MinEdge.x);
        m_boundingBox.MinEdge.y = Min(m_boundingBox.MinEdge.y, pAIRMesh->GetBoundingBox().MinEdge.y);
        m_boundingBox.MinEdge.z = Min(m_boundingBox.MinEdge.z, pAIRMesh->GetBoundingBox().MinEdge.z);
    }
}

void CModelInstance::SetAttachedNode(CSceneNode* pNode)
{
    Renderable::SetAttachedNode(pNode);
    /*
    if (pNode != NULL && m_vtBones.size() > 0)
    {
        m_pNode->AddChild(m_vtBones[0]);

		size_t nBoneCount = m_vtBones.size();
        for (size_t i = 1; i < nBoneCount; ++i)
        {
            if (m_vtBones[i]->GetParentBoneID() == -1)
            {
                m_pNode->AddChild(m_vtBones[i]);
            }
        }
    }
    */
	size_t nMeshInstCount = m_vtMeshInst.size();
    for (size_t i = 0; i < nMeshInstCount; ++i)
    {
        m_vtMeshInst[i]->SetAttachedNode(pNode);
    }
}

CAIRBoneInstance* CModelInstance::GetBoneInst(u32 nBoneID)
{
    if (nBoneID >= m_vtBones.size())
    {
        return NULL;
    }
    return m_vtBones[nBoneID];
}

CAIRBoneInstance* CModelInstance::GetBoneInst(const AIRString& strName)
{
	BONEINSTANCE_ITER it_end = m_vtBones.end();
    for (BONEINSTANCE_ITER it = m_vtBones.begin(); it != it_end; ++it)
    {
        if ((*it)->GetName() == strName)
        {
            return *it;
        }
    }

    return NULL;
}

Renderable* CModelInstance::AttachRenderableToBone(const AIRString& strBoneName, Renderable* pRenderable)
{
	CAIRBoneInstance* pBone = GetBoneInst(strBoneName);
	Renderable* pRet = NULL;
	if (pBone)
	{
		pBone->AttachRenderable(pRenderable);
		//m_lstAttachObjs.insert(std::make_pair(pRenderable->GetName(), pRenderable));
		pRet = pRenderable;
	}

	return pRet;
}
/*
Renderable* CModelInstance::DetachRenderableFromBone(const AIRString& strBoneName, const AIRString& strRenderableName)
{
	CAIRBoneInstance* pBone = GetBoneInst(strBoneName);
	Renderable* pRet = NULL;
	if (pBone)
	{
		pRet = pBone->DetachRenderable(strRenderableName);
	}

	return pRet;
}
*/
Renderable* CModelInstance::DetachRenderableFromBone(const AIRString& strBoneName, Renderable* pRenderable)
{
    CAIRBoneInstance* pBone = GetBoneInst(strBoneName);
    Renderable* pRet = NULL;
    if (pBone)
    {
        pRet = pBone->DetachRenderable(pRenderable);
    }

    return pRet;
	//return DetachRenderableFromBone(strBoneName, pRenderable->GetName());
}

void CModelInstance::AddBoneAttachedRenderable(Renderable* pRenderable)
{
    RenderableListIter it = std::find(m_lstAttachObjs.begin(), m_lstAttachObjs.end(), pRenderable);
    if (it == m_lstAttachObjs.end())
    {
        m_lstAttachObjs.push_back(pRenderable);
    }
    
}

void CModelInstance::RemoveBoneAttachedRenderable(Renderable* pRenderable)
{
	if( !pRenderable )
		return;

    RenderableListIter it = std::find(m_lstAttachObjs.begin(), m_lstAttachObjs.end(), pRenderable);
    if (it != m_lstAttachObjs.end())
    {
    	m_lstAttachObjs.erase(it);
    }
}

void CModelInstance::DetachAllRenderableFromBone(const AIRString& strBoneName)
{
	CAIRBoneInstance* pBone = GetBoneInst(strBoneName);
	if (pBone)
	{
		ListIterator<CSceneNode::RenderableList> iter = pBone->GetMapItorRenderable();
        while (!iter.IsEnd())
        {
            Renderable* pRenderable = iter.GetNext();
            
            pBone->DetachRenderable(pRenderable);

            //RenderableListIter it = m_lstAttachObjs.find(pRenderable->GetName());
            //if (it != m_lstAttachObjs.end())
            //{
            //    m_lstAttachObjs.erase(it);
            //}
        }
	}
}
/*
void CModelInstance::CreateAnimationTransition(CAIRSkeleton* pCurFrame, CAIRSkeleton* pNextFrame)
{
    CAIRSkeletonAnimation* pAnimation = static_cast<CAIRSkeletonAnimation*>(m_animationTransition.GetAnimationRes());
    if (pAnimation == NULL)
    {
        pAnimation = new CAIRSkeletonAnimation();
        pAnimation->SetName("transition");
        m_animationTransition.SetLoop(false);
        m_animationTransition.SetAnimation(pAnimation);
    }
    m_animationTransition.SetTimePos(0.0f);
    pAnimation->RemoveAllFrames();
    pAnimation->AddFrameSkeleton(pCurFrame);
    pAnimation->AddFrameSkeleton(pNextFrame);
    pAnimation->AddKeyFrameTime(0.0f);
    pAnimation->AddKeyFrameTime(5.0f);
    pAnimation->SetTimeRange(5.0f);
    
    m_animationTransition.SetTimeLength(5.0f);
}
*/
void CModelInstance::ClearAnimationStates()
{
    for (AnimationStateMap_Iter it = m_mapAnimationState.begin(); it != m_mapAnimationState.end(); ++it)
    {
        SafeDelete(it->second);
    }
    m_mapAnimationState.clear();
	m_pCurAnimation = NULL;
}

void CModelInstance::CreateAnimationStates()
{
    ClearAnimationStates();
    if (m_pModel)
    {
        m_pModel->CreateAnimationStates(m_mapAnimationState);
    }
}

void CModelInstance::CreateVertexAnimationStates(AIRKeyframeAnimationRes* pVertexAnimationRes)
{
    if (pVertexAnimationRes)
    {
        pVertexAnimationRes->CreateAnimationStates(m_mapAnimationState);
        if (!m_mapAnimationState.empty())
        {
            AnimationStateMap_Iter it = m_mapAnimationState.begin();
            m_pCurAnimation = it->second;
        }
    }
}

CAIRAnimationState* CModelInstance::GetAnimationState(const AIRString& strName)
{
    AnimationStateMap_Iter it = m_mapAnimationState.find(strName);

    if (it != m_mapAnimationState.end())
    {
        return it->second;
    }
	else
	{
		CAIRSkeletonAnimation* pAnimation = m_pModel->CreateSkeletonAnimation(strName.c_str());
		if (pAnimation)
		{
			CAIRAnimationState* pState = NULL;
			if (pAnimation->Cache())
			{
				pState = new CAIRAnimationState(pAnimation);
			}
			else
			{
				pState = new CAIRAnimationState;
			}
			m_mapAnimationState.insert(std::make_pair(strName, pState));
			return pState;
		}
	}

    return NULL;
}


CAIRAnimationState* CModelInstance::GetAnimationState(int nIndex)
{
    if ( nIndex < 0 || (size_t)nIndex >= m_mapAnimationState.size() )
    {
        return NULL;
    }
    AnimationStateMap_Iter it = m_mapAnimationState.begin();
    for ( int i = 0; i <= nIndex; ++i )
    {
        ++it;
    }

    if (it != m_mapAnimationState.end())
    {
        return it->second;
    }

    return NULL;
}

void CModelInstance::UpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue)
{
    //OutputDebugString(GetString("worldaabb [%f,%f,%f] [%f,%f,%f]\n", m_pNode->GetWorldBoundingBox().MinEdge.x, 
    //    m_pNode->GetWorldBoundingBox().MinEdge.y, m_pNode->GetWorldBoundingBox().MinEdge.z,
    //    m_pNode->GetWorldBoundingBox().MaxEdge.x, m_pNode->GetWorldBoundingBox().MaxEdge.y, m_pNode->GetWorldBoundingBox().MaxEdge.z));
    if (ENGINE_INST->GetConfig()->m_bAutoGenLod)
        AutoSelectLod(pCamera);
    //BEGIN_FUNC_PROFILE;
    Renderable::UpdateToRenderQueue(pCamera, pQueue);
    //END_FUNC_PROFILE_WITH_REPORT("Renderable::UpdateToRenderQueue time");

    //这里为了是把子mesh放进去
    

    
}

void CModelInstance::OnUpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue)
{
	/*
    //pQueue->AddRenderable(this);
    if (m_bInstancing)
    {
        CMeshInstance* pMeshInst = NULL;
        
        for (size_t i = 0; i < m_vtMeshInst.size(); ++i)
        {
            pMeshInst = m_vtMeshInst[i];
            InstancingObject* pObject = INSTANCING_MANAGER->GetInstancingObject(pMeshInst->GetMeshRes());
            if (pObject && pObject->GetMaxInstancesNum() > 3)
            {
                MeshInstancingObject* pMeshInstancing = pObject->GetMeshInstancingObject(pQueue, m_nCurLodLevel);
                pMeshInstancing->AddModelInstance(pMeshInst);
                if (!pMeshInstancing->IsInRenderQueue())
                {
                    pQueue->AddRenderable(pMeshInstancing);
                    pMeshInstancing->SetIsInRenderQueue(true);
                //    pMeshInstancing->AddRenderQueue(pQueue);
                }
            }
            else
            {
                m_vtMeshInst[i]->UpdateToRenderQueue(pCamera, pQueue);
                //pQueue->AddRenderable(m_vtMeshInst[i]);
            }
        }
        
    }
    else
    {
//#ifdef TEXTURE_RENDER_GROUP
        for (size_t i = 0; i < m_vtMeshInst.size(); ++i)
        {
            if (m_vtMeshInst[i]->Prepare())
            {
                pQueue->AddRenderable(m_vtMeshInst[i]);
            }
            
            //m_vtMeshInst[i]->UpdateToRenderQueue(pCamera, pQueue);
        }
//#else
//        Renderable::OnUpdateToRenderQueue(pQueue);
//#endif
    }
    */
    for (size_t i = 0; i < m_vtMeshInst.size(); ++i)
    {
        m_vtMeshInst[i]->UpdateToRenderQueue(pCamera, pQueue);
    }

    if (!m_bStatic)
    {
        UpdateAnimation();   //
    }

    RenderableListIter it_end = m_lstAttachObjs.end();
    for ( RenderableListIter itor = m_lstAttachObjs.begin(); itor != it_end; ++itor )
    {
        (*itor)->UpdateToRenderQueue(pCamera, pQueue);
        //pQueue->AddRenderable( itor->second );
    }
}

void CModelInstance::UpdateAnimation()
{
    if (!m_bAnimationUpdated)
    {
        if (m_pModel->HasSkeletons())
        {
            //注意，如果是带有骨骼动画，并且带有顶点动画的，在MeshInstance里面更新顶点动画数据
            //ResetSkeleton();
			if (m_pCurAnimation)
			{
				CAIRAnimation* pAnim = m_pCurAnimation->GetAnimationRes();
				if (IS_KIND_OF(CAIRSkeletonAnimation, pAnim))
				{
					CAIRSkeletonAnimation* pSkelAnim = (CAIRSkeletonAnimation*)pAnim;
					if (!pSkelAnim->Cache())
					{
						return;
					}
				}
			}
            CaculateBones();

            GetBoneMatrix();
        }
        else if (m_pVertexAnimationRes)
        {
            //如果只带顶点动画，不带骨骼动画，在这里更新顶点数据
            if (m_pCurAnimation)
            {
                CAIRAnimation* pAnimation = m_pCurAnimation->GetAnimationRes();
                pAnimation->Apply(this, m_pCurAnimation->GetTimePos(), m_pCurAnimation->GetWeight());
            }
        }
        
        m_bAnimationUpdated = true;

        if (m_pCurrentAnimationMixer && m_pCurrentAnimationMixer->IsEnd())
        {
            if (m_pCurrentAnimationMixer->IsLoop())
            {
                m_pCurrentAnimationMixer->Excute(this);
            }
            else
                m_pCurrentAnimationMixer = NULL;
        }
    }
    
        
}


CMeshInstance*		CModelInstance::GetMeshInstance( size_t nIndex ) 
{
	if ( nIndex >= m_vtMeshInst.size() )
	{
		return NULL;
	}
	return m_vtMeshInst[nIndex];
}

void CModelInstance::GetBoneMatrix()
{
    Matrix4f* pMatrix = m_boneMatrix;
	u32 dwBoneCount = m_vtBones.size();
    for (u32 i = 0; i < dwBoneCount; ++i)
    {
        m_vtBones[i]->GetOffsetTranform(*pMatrix);
        pMatrix++;
    }
}

void CModelInstance::BuildBones()
{
    u32 nBonesNum = m_pModel->GetBoneInstanceCount();
    m_vtBones.resize(nBonesNum);
    for (u32 i = 0; i < nBonesNum; ++i)
    {
        CAIRBoneInstance* pBoneInst = new CAIRBoneInstance(i, this);
        m_vtBones[i] = pBoneInst;
    }

    for (u32 i = 0; i < nBonesNum; ++i)
    {
        CAIRBoneInstance* pBoneInst = m_vtBones[i];
        LPAIRBONE pBone = m_pModel->GetBone(i);
        if (pBone->nParentID != -1)
        {
            //pBoneInst->SetParent(m_vtBones[pBone->nParentID]);
            m_vtBones[pBone->nParentID]->AddChild(pBoneInst);
        }
    }

    for (u32 i = 0; i < nBonesNum; ++i)
    {
        CAIRBoneInstance* pBoneInst = m_vtBones[i];
        pBoneInst->SetInitState();
    }

    m_boneMatrix = new Matrix4f[nBonesNum];
    for (u32 i = 0; i < nBonesNum; ++i)
    {
        m_boneMatrix[i].Identity();
    }
}

void CModelInstance::ResetSkeleton()
{
    for (size_t i = 0; i < m_vtBones.size(); ++i)
    {
        m_vtBones[i]->Reset();
    }
}

CAIRSubMesh* CModelInstance::GetSubMesh(size_t nIndex)
{
    if (nIndex < m_vtSubMeshes.size())
    {
        return m_vtSubMeshes[nIndex];
    }
    return NULL;
}

void CModelInstance::CreateMeshInst()
{
    for (size_t i = 0; i < m_vtSubMeshes.size(); ++i)
    {
        CMeshInstance* pMesh = new CMeshInstance(m_vtSubMeshes[i], this);
        pMesh->SetRenderToShadow(m_bRenderToShadow);
        pMesh->SetAcceptLighting(m_bAcceptLighting);
        pMesh->AcceptShadowmap(m_bAcceptShadow);
        m_vtMeshInst.push_back(pMesh);
    }
}

/*
void CModelInstance::SetCurrentTechnique( const AIRString& szName )
{
	IMaterial* pMaterial = NULL;
	for (size_t i = 0; i < m_vtMeshInst.size(); ++i)
	{
		pMaterial = m_vtMeshInst[i]->GetMaterial();
		if ( pMaterial )
		{
			pMaterial->SetCurrentTechnique( szName );
		}
	}
}
*/

void CModelInstance::EnableLights(bool bEnable)
{
    for (size_t i = 0; i < m_vtMeshInst.size(); ++i)
    {
        m_vtMeshInst[i]->EnableLights(bEnable);
    }
}

void CModelInstance::GenRenderToShadowTechnique()
{
    for (size_t i = 0; i < m_vtMeshInst.size(); ++i)
    {
        m_vtMeshInst[i]->GenRenderToShadowTechnique();
    }
}
/*
void CModelInstance::RenderToShadowMap(CCamera* pCamera)
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    PreRender(pCamera);
    //m_matWorld.Identity();
    //Matrix4f matWorld;
    //m_pNode->GetWorldTransform(&matWorld);
    if (m_pNode && m_bStatic)
    {
        pRenderer->SetWorldTransform(&m_pNode->GetFullTransform());
    }
    else
    {
        if (m_pCurAnimation == NULL && m_pNode)
        {
            pRenderer->SetWorldTransform(&m_pNode->GetFullTransform());
        }
        else
            pRenderer->SetWorldTransform(&Matrix4f::IDENTITY);
    }
    for (size_t i = 0; i < m_vtMeshInst.size(); ++i)
    {
        m_vtMeshInst[i]->RenderToShadowMap(pCamera);
    }
}
*/
/*
void CModelInstance::RenderToDepthMap(CCamera* pCamera, CPass* pPass)
{
	IRenderer* pRenderer = ENGINE_INST->GetRenderer();
	PreRender(pCamera);
	if (m_pNode && m_bStatic)
	{
		pRenderer->SetWorldTransform(&m_pNode->GetFullTransform());
	}
	else
	{
		if (m_pCurAnimation == NULL && m_pNode)
		{
			pRenderer->SetWorldTransform(&m_pNode->GetFullTransform());
		}
		else
			pRenderer->SetWorldTransform(&Matrix4f::IDENTITY);
	}
	for (size_t i = 0; i < m_vtMeshInst.size(); ++i)
	{
		//m_vtMeshInst[i]->RenderToDepthMap(pCamera, pPass);
	}
}
*/

bool CModelInstance::IsPick(const Vector3Df& vOrg, const Vector3Df& vPickDir)
{
	//如果是使用硬件加速的骨骼动画
	if ( !m_bStatic && ENGINE_INST->IsHardwareSkeleton() )
	{
		return m_WorldAABB.IntersectsWithLine(vOrg, vPickDir, float(0xfffffffe));
	}

	//静态模型或者使用cpu进行骨骼蒙皮
    for (size_t i = 0; i < m_vtMeshInst.size(); ++i)
    {
        if (m_bStatic)
        {
            if (m_vtMeshInst[i]->IsPick(vOrg, vPickDir, m_pNode ? m_pNode->GetFullTransform() : Matrix4f::IDENTITY))
            {
                return true;
            }
        }
        else
        {
			//如果是使用cpu进行骨骼蒙皮
			if (m_vtMeshInst[i]->IsPick(vOrg, vPickDir, Matrix4f::IDENTITY))
			{
				return true;
			}
	    }        
    }

    return false;
}


bool CModelInstance::IsPick(const Vector3Df& vOrg, const Vector3Df& vPickDir, float& distFromEyeToObject)
{
	//是否点中
	bool bPick = false;
	//击中点到vOrg的距离
	float fCurDist = float(0xfffffffe);

	//如果是使用硬件加速的骨骼动画
	if ( !m_bStatic && ENGINE_INST->IsHardwareSkeleton() )
	{
		//暂时采用包围盒测试的方式来计算点击测试
		if ( m_WorldAABB.IntersectsWithLine(vOrg, vPickDir, float(0xfffffffe)) )
		{
			bPick = true;
			fCurDist = m_WorldAABB.GetCenter().GetDistanceFrom( vOrg );
			if ( fCurDist < distFromEyeToObject )
			{
				distFromEyeToObject = fCurDist;
			}
		}
		return bPick;
	}

	//如果是静态网格或者是使用cpu计算的动画
	//遍历所有的子网格
	for (size_t i = 0; i < m_vtMeshInst.size(); ++i)
	{
		//静态网格
		if (m_bStatic)
		{
			//测试是否点中该子网格
			if ( m_vtMeshInst[i]->IsPick(vOrg, vPickDir, (m_pNode ? m_pNode->GetFullTransform() : Matrix4f::IDENTITY), fCurDist) )
			{
				bPick = true;
				if ( fCurDist < distFromEyeToObject )
				{
					distFromEyeToObject = fCurDist;
				}
			}
		}
		//动态网格
		else
		{
			//测试是否点中该子网格
			if ( m_vtMeshInst[i]->IsPick(vOrg, vPickDir, Matrix4f::IDENTITY, fCurDist) )
			{
				bPick = true;
				if ( fCurDist < distFromEyeToObject )
				{
					distFromEyeToObject = fCurDist;
				}
			}
		}
	}

	return bPick;
}

void CModelInstance::AcceptShadowmap(bool bAccept)
{
    for (size_t i = 0; i < m_vtMeshInst.size(); ++i)
    {
        m_vtMeshInst[i]->AcceptShadowmap(bAccept);
    }
    m_bAcceptShadow = bAccept;
}


void CModelInstance::SetUseTexLuminance( bool b )
{
	m_bUseTexLuminance = b;
	for (size_t i = 0; i < m_vtMeshInst.size(); ++i)
	{
		m_vtMeshInst[i]->SetUseTexLiminance( b );
	}
}


void CModelInstance::SetTexLuminanceWholeModel( int n )
{
	for (size_t i = 0; i < m_vtMeshInst.size(); ++i)
	{
		m_vtMeshInst[i]->SetTexLuminanceWholeMesh( n );
	}
}


void CModelInstance::SetInstancingEnable(bool bInstancing)
{
    m_bInstancing = bInstancing;
}


void CModelInstance::SetCurLodLevel( u16 nCurLodLevel )
{
	m_nCurLodLevel = nCurLodLevel;
	CAIRSubMesh* pSubMesh = NULL;
	MESH_ITER itEnd = m_vtSubMeshes.end();
	for (MESH_ITER it = m_vtSubMeshes.begin(); it != itEnd; ++it)
	{
		pSubMesh = *it;
		pSubMesh->SetCurLodLevel( nCurLodLevel );
	}
}

const AABBBox3Df& CModelInstance::GetWorldBoundingBox(bool bTransform)
{
    if (bTransform)
    {
        m_WorldAABB = GetBoundingBox();
        if (m_pNode && m_bTransformByParent)
        {
            m_WorldAABB.TransformAffine( m_pNode->GetFullTransform() );
        }
        m_vtCenter = m_WorldAABB.GetCenter();

        //这里计算的目的是，求出每个子mesh的aabb，因为子mesh在渲染队列里面要排序
        for (size_t i = 0; i < m_vtMeshInst.size(); ++i)
        {
            m_vtMeshInst[i]->GetWorldBoundingBox(true);
        }
    }
    return m_WorldAABB;
}


void CModelInstance::AutoSelectLod( CCamera* pCamera )
{
	Vector3Df vCenter = GetCenter();
	float dist = vCenter.GetDistanceFrom( pCamera->GetPosition() );
	m_nCurLodLevel = 0;
	int nMaxLodLevel = m_vtLodStrategy.size() - 1;
    if (nMaxLodLevel < 0)
    {
        nMaxLodLevel = 0;
    }	
	if ( m_vtLodStrategy.size() )
	{
		int i = 0;
		for ( ; i <= nMaxLodLevel ; ++i )
		{
			if ( dist <= m_vtLodStrategy[i] )
			{
				m_nCurLodLevel = i;
				break;
			}
		}
		if ( i > nMaxLodLevel )
		{
			m_nCurLodLevel = nMaxLodLevel;
		}
	}
	SetCurLodLevel( m_nCurLodLevel );
}

void CModelInstance::SetAcceptLighting(bool bAccept)
{
    Renderable::SetAcceptLighting(bAccept);
    for (size_t i = 0; i < m_vtMeshInst.size(); ++i)
    {
        m_vtMeshInst[i]->SetAcceptLighting(bAccept);
    }
}

void CModelInstance::ResetCurrentTechnique()
{
    for (size_t i = 0; i < m_vtMeshInst.size(); ++i)
    {
        //m_vtMeshInst[i]->ResetCurrentTechnique();
    }
}

void CModelInstance::SetCurrentAnimationMixer(const char* szMixName, float fDelayIn, float fDelayOut)
{
    MapAnimationMixerIter it = m_mapAnimationMixer.find(szMixName);
    AIRAnimationMixer* pAnimationMix = NULL;
    if (it != m_mapAnimationMixer.end())
    {
        pAnimationMix = it->second;

        if (pAnimationMix->IsWorkable(m_pCurAnimation) && pAnimationMix != m_pCurrentAnimationMixer)
        {
            m_pCurrentAnimationMixer = pAnimationMix;
            m_pCurrentAnimationMixer->Excute(this, fDelayIn, fDelayOut);
            //m_pCurrentAnimationMixer->SetBlend(CAIRAnimationBlend::Blend_Transition)
        }
    }
}

void CModelInstance::StopAnimation()
{
    m_pCurAnimation = NULL;
}

void CModelInstance::PauseAnimation(bool bPause)
{
    m_bPauseAnimation = bPause;
}

void CModelInstance::SetRenderToShadow(bool bRenderToShadow)
{
    Renderable::SetRenderToShadow(bRenderToShadow);
    for (size_t i = 0; i < m_vtMeshInst.size(); ++i)
    {
        m_vtMeshInst[i]->SetRenderToShadow(bRenderToShadow);
    }
}

bool CModelInstance::Prepare()
{
    if (m_bPrepared)
    {
        return true;
    }
    if (m_pModel)
    {
        /*
        if (!m_pModel->IsReady())
        {
            if (!m_pModel->IsLoaded())
            {
                return false;
            }
            else
            {
                m_pModel->Cache();

                if (m_pModel->IsReady())
                {
                    PrepareModel();
                }
                else
                {
                    return false;
                }
            }
            return false;
        }
        
        if (m_pModel->GetAnimationType() == eMAT_SkeletionAnimation)
        {
            if (m_pSkeletonRes)
            {
                if (!m_pSkeletonRes->IsReady())
                {
                    if (m_pSkeletonRes->IsLoaded())
                    {
                        PrepareSkeletonAnimation();
                        
                    }
                    return false;
                }
            }

            if (m_pAnimationMixRes)
            {
                if (m_pAnimationMixRes->IsReady())
                {
                    m_bPrepared = true;
                }
                else
                {
                    if (m_pAnimationMixRes->IsLoaded())
                    {
                        PrepareAnimationMixer();
                        m_bPrepared = true;
                    }
                    else
                    {
                        m_bPrepared = false;
                    }
                }
            }
            else
            {
                m_bPrepared = true;
            }
        }
        else if (m_pModel->GetAnimationType() == eMAT_VertexAnimation)
        {
            if (m_pVertexAnimationRes)
            {
                if (m_pVertexAnimationRes->IsReady())
                {
                    m_bPrepared = true;
                }
                else
                {
                    if (m_pVertexAnimationRes->IsLoaded())
                    {
                        PrepareVertexAnimation();
                    }
                    else
                    {
                        m_bPrepared = false;
                    }
                }
            }
        }
        else
        {
            m_bPrepared = true;
        }
        */
		
        if (!m_pModel->IsReady())
        {
            if (m_pModel->IsLoaded())
            {
                if (m_pModel->Cache())
                {
                    PrepareModel();
                    m_bPrepared = true;
                }
                
            }
        }
        else
        {
            PrepareModel();
            m_bPrepared = true;
        }

        if (m_bPrepared)
        {
            ResetCurrentTechnique();
        }
    }

    return m_bPrepared;
}

void CModelInstance::PrepareModel()
{
    DT_TO_DBGSTR(LT_NONE, GetString("[%s] prepare model!\n", m_strResourceName.c_str()));
    m_pModel->GetMeshVector(m_vtSubMeshes);

    ComputeBoundingBox();

    

    //m_pSkeletonRes = m_pModel->GetSkeletonRes();

    m_bStatic = !m_pModel->HasSkeletons();
    
    CreateMeshInst();
    //m_pSkeletalRes = MODELMGR->CreateSkeletonRes(szSkeletalFile);

  //  if (m_pSkeletonRes)
  //  {
		//assert( m_pSkeletonRes->IsReady() );
  //      PrepareSkeletonAnimation();
  //  }

    m_pVertexAnimationRes = m_pModel->GetVertexAnimationRes();
    if (m_pVertexAnimationRes)
    {
        //if (!m_pVertexAnimationRes->IsReady())
            PrepareVertexAnimation();
    }

    if (m_pNode)
    {
        SetAttachedNode(m_pNode);
        m_pNode->NeedUpdate();
        m_pNode->Update();
    }

    assert(m_vtMeshInst.size() != 0);

    if (m_pModel->HasSkeletons())     //这个时候可能还没计算骨，会出现混乱，要补回计算
    {
        //ResetSkeleton();
		PrepareSkeletonAnimation();
        if (strlen(m_szCurrentAnimation))
        {
            SetCurrentAnimation(m_szCurrentAnimation);
        }
        //OutputDebugString(GetString("CaculateBones is prepare model, current animation is %x \n", m_pCurAnimation));
        CaculateBones();

        GetBoneMatrix();
    }

    DT_TO_DBGSTR(LT_NONE, GetString("[%s] prepare model finished!\n", m_strResourceName.c_str()));
}

void CModelInstance::PrepareSkeletonAnimation()
{
    //if (!m_pSkeletonRes->IsReady())
    //{
    //    m_pSkeletonRes->Cache();
    //}
    
    BuildBones();
    //CreateAnimationStates();

    if ( m_pModel->HasSkeletons() )
    {
        PrepareAnimationMixer();
    }

    //把相关的bone attach上scenenode上
    if (m_pNode != NULL && m_vtBones.size() > 0)
    {
        /*
        m_pNode->AddChild(m_vtBones[0]);

        size_t nBoneCount = m_vtBones.size();
        for (size_t i = 1; i < nBoneCount; ++i)
        {
            if (m_vtBones[i]->GetParentBoneID() == -1)
            {
                m_pNode->AddChild(m_vtBones[i]);
            }
        }
        */
    }

    size_t nMeshInstCount = m_vtMeshInst.size();
    for (size_t i = 0; i < nMeshInstCount; ++i)
    {
        m_vtMeshInst[i]->SetAttachedNode(m_pNode);
    }
}

void CModelInstance::PrepareAnimationMixer()
{
    if (m_pAnimationMixRes)
    {
        m_pAnimationMixRes->Cache();
    }
    GenAnimationMixers();
}

void CModelInstance::PrepareVertexAnimation()
{
    //m_pVertexAnimationRes->Cache();
    CreateVertexAnimationStates(m_pVertexAnimationRes);
    m_bStatic = false;
}

void CModelInstance::FillRenderContext(CCamera* pCamera, RenderContextQueue* pQueue, eRenderFlow flow)
{
    if (Prepare())
    {
		if (m_bShowBoundingBox)
		{
			SCENE_DEBUGER->RenderAABB(m_WorldAABB, SColor::White, true);
		}
        if (ENGINE_INST->GetConfig()->m_bAutoGenLod)
            AutoSelectLod(pCamera);
        CommitNodeAnimation();
        if (!m_bStatic)
        {
            UpdateAnimation();   //
        }
        for (size_t i = 0; i < m_vtMeshInst.size(); ++i)
        {
            m_vtMeshInst[i]->FillRenderContext(pCamera, pQueue, flow);
        }
    }
    
}
