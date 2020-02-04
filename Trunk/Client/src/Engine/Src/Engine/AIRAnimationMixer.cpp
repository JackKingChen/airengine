#include "stdafx.h"
#include "AIRAnimationMixer.h"
#include "AIRAnimationMixRes.h"
#include "AIRSkeletonAnimation.h"
#include "AIRAnimationState.h"
#include "ModelInstance.h"

AIRAnimationMixer::AIRAnimationMixer(const char* szName, AIRAnimationMixData* pMixData) : m_strName(szName)
, m_pAnimationMixData(pMixData)
, m_fSourceStartBlendTime(0)
, m_fTargetBlendEndTime(0)
, m_bBlendToNoMixer(false)
, m_bLoop(false)
{

}

AIRAnimationMixer::~AIRAnimationMixer()
{

}

void AIRAnimationMixer::Apply(CModelInstance* pModelInstance)
{
    int nBonesNum = m_pAnimationMixData->GetBonesNum();

    /*
    if (m_animationBlend.IsBlending())
    {
        CAIRAnimationState* pStateSource = m_animationBlend.GetSource();
        CAIRAnimationState* pStateTarget = NULL;  //m_animationBlend.GetTarget();
        CAIRAnimation* pAnimationSource = pStateSource->GetAnimationRes();
        
        AIRAnimationMixData::WeightVector* pTargetWeights = NULL;//m_pAnimationMixData->GetWeightVector(static_cast<CAIRSkeletonAnimation*>(pAnimaitonTarget));
        //CAIRAnimationState* pCurrentState = NULL;
        
        for (size_t j = 0; j < m_vtAnimationStates.size(); ++j)
        {
            pStateTarget = m_vtAnimationStates[j];
            CAIRAnimation* pAnimaitonTarget = pStateTarget->GetAnimationRes();

            CAIRAnimation* pAnimation = pStateTarget->GetAnimationRes();
            pTargetWeights = m_pAnimationMixData->GetWeightVector(static_cast<CAIRSkeletonAnimation*>(pAnimation));
            
            if (!m_bBlendToNoMixer)
            {
                if (pStateTarget != pStateSource)
                {

                    for (int i = 0; i < nBonesNum; ++i)
                    {
                        if ((*pTargetWeights)[i] > 0.0f)
                        {
                            pAnimationSource->ApplyToNode(pModelInstance->GetBoneInst(i), i, m_fSourceStartBlendTime, m_animationBlend.GetSourceWeight(1.0f));
                            pAnimaitonTarget->BlendToNode(pModelInstance->GetBoneInst(i), i, m_fTargetBlendEndTime, m_animationBlend.GetTargetWeight((*pTargetWeights)[i]));
                        }
                    }
                }
                else
                {
                    for (int i = 0; i < nBonesNum; ++i)
                    {
                        if ((*pTargetWeights)[i] > 0.0f)
                        {
                            pAnimationSource->ApplyToNode(pModelInstance->GetBoneInst(i), i, pStateSource->GetTimePos(), (*pTargetWeights)[i]);
                            //pAnimaitonTarget->BlendToNode(pModelInstance->GetBoneInst(i), i, pStateTarget->GetTimePos(), pStateTarget->GetWeight());
                        }
                    }
                }
            }
            else
            {
                pStateTarget = m_animationBlend.GetTarget();
                pAnimaitonTarget = pStateTarget->GetAnimationRes();
                pStateSource = m_vtAnimationStates[j];
                if (pStateTarget != pStateSource)   
                {
                    for (int i = 0; i < nBonesNum; ++i)
                    {
                        if ((*pTargetWeights)[i] > 0.0f)
                        {
                            pAnimationSource->ApplyToNode(pModelInstance->GetBoneInst(i), i, pStateSource->GetTimeLength(), m_animationBlend.GetSourceWeight((*pTargetWeights)[i]));
                            pAnimaitonTarget->BlendToNode(pModelInstance->GetBoneInst(i), i, m_fTargetBlendEndTime, m_animationBlend.GetTargetWeight(1.0f));
                        }
                    }
                }
                else
                {
                    for (int i = 0; i < nBonesNum; ++i)
                    {
                        if ((*pTargetWeights)[i] > 0.0f)
                        {
                            pAnimationSource->ApplyToNode(pModelInstance->GetBoneInst(i), i, pStateSource->GetTimePos(), (*pTargetWeights)[i]);
                            //pAnimaitonTarget->BlendToNode(pModelInstance->GetBoneInst(i), i, pStateTarget->GetTimePos(), pStateTarget->GetWeight());
                        }
                    }
                }
            }
        }

    }
    else
    {
        CAIRAnimationState* pAnimationState = NULL;
        
        for (size_t i = 0; i < m_vtAnimationStates.size(); ++i)
        {
            pAnimationState = m_vtAnimationStates[i];
            CAIRSkeletonAnimation* pAnimation = static_cast<CAIRSkeletonAnimation*>(pAnimationState->GetAnimationRes());
            AIRAnimationMixData::WeightVector* pWeightVector = m_pAnimationMixData->GetWeightVector(pAnimation);

            for (int i = 0; i < nBonesNum; ++i)
            {
                if ((*pWeightVector)[i])
                {
                    pAnimation->ApplyToNode(pModelInstance->GetBoneInst(i), i, pAnimationState->GetTimePos(), (*pWeightVector)[i]);
                }

            }

        }
    }
    */

    CAIRAnimationState* pState = NULL;
    AIRAnimationMixData::WeightVector* pTargetWeights = NULL;
    CAIRAnimation* pAnimationCircle = NULL;

    for (size_t i = 0; i < m_vtAnimationActions.size(); ++i)
    {
        pState = m_vtAnimationActions[i];
        CAIRAnimation* pAnimation = pState->GetAnimationRes();
        pTargetWeights = m_pAnimationMixData->GetWeightVector(static_cast<CAIRSkeletonAnimation*>(pAnimation));
        for (int i = 0; i < nBonesNum; ++i)
        {
            if ((*pTargetWeights)[i] > 0.0f)
            {
                
                if (pState->GetAnimationStateStatus() == eSTATE_IN)
                {
                    pAnimation->ApplyToNode(pModelInstance->GetBoneInst(i), i, pState->GetTimePos(), pState->GetWeight());
                    pAnimationCircle = m_vtAnimationCircles[0]->GetAnimationRes();
                    
                    pAnimationCircle->BlendToNode(pModelInstance->GetBoneInst(i), i, m_vtAnimationCircles[0]->GetTimePos(), 1.0f - pState->GetWeight());
                    //OutputDebugString(GetString("state in action weight:%.4f, circle weight:%.4f, timepos:%.4f\n", pState->GetWeight(), 1.0f - pState->GetWeight(), pState->GetTimePos()));
                }
                else if (pState->GetAnimationStateStatus() == eSTATE_OUT)
                {
                    pAnimation->ApplyToNode(pModelInstance->GetBoneInst(i), i, pState->GetTimePos(), pState->GetWeight());
                    pAnimationCircle = m_vtAnimationCircles[0]->GetAnimationRes();
                    pAnimationCircle->BlendToNode(pModelInstance->GetBoneInst(i), i, m_fTargetBlendEndTime/*m_vtAnimationCircles[0]->GetTimePos()*/, 1.0f - pState->GetWeight());
                    //OutputDebugString(GetString("state out action weight:%.4f, circle weight:%.4f\n", pState->GetWeight(), 1.0f - pState->GetWeight()));
                }
                else
                {
                    pAnimation->ApplyToNode(pModelInstance->GetBoneInst(i), i, pState->GetTimePos(), pState->GetWeight());
                }
            }
            
        }
    }

    for (size_t i = 0; i < m_vtAnimationCircles.size(); ++i)
    {
        pState = m_vtAnimationCircles[i];
        CAIRAnimation* pAnimation = pState->GetAnimationRes();
        pTargetWeights = m_pAnimationMixData->GetWeightVector(static_cast<CAIRSkeletonAnimation*>(pAnimation));

        for (int i = 0; i < nBonesNum; ++i)
        {
            if ((*pTargetWeights)[i] > 0.0f)
            {
                pAnimation->ApplyToNode(pModelInstance->GetBoneInst(i), i, pState->GetTimePos(), pState->GetWeight());
            }
        }
    }
}

bool AIRAnimationMixer::Update(float fTimeElapsed)
{
    /*
    if (m_animationBlend.IsBlending())
    {
        m_animationBlend.UpdateForMixer(fTimeElapsed);
    }
    else
    {
        for (size_t i = 0; i < m_vtAnimationActions.size(); ++i)
        {
            m_vtAnimationActions[i]->AddTime(fTimeElapsed);
        }

        //判断是否动作已经完成，是的话，要混合回原来的动画
        if (IsEnd())
        {
            
            m_animationBlend.StartBlend(CAIRAnimationBlend::Blend_Transition, NULL, m_animationBlend.GetSource(), 0.15f);
            m_bBlendToNoMixer = true;
        }
        
    }
    */
    AnimationStateIter itEnd = m_vtAnimationActions.end();
    for (AnimationStateIter it = m_vtAnimationActions.begin(); it != m_vtAnimationActions.end();)
    {
        if ((*it)->Update(fTimeElapsed))
        {
            ++it;
        }
        else
        {
            it = m_vtAnimationActions.erase(it);
        }
    }

    itEnd = m_vtAnimationCircles.end();
    for (AnimationStateIter it = m_vtAnimationCircles.begin(); it != itEnd; ++it)
    {
        (*it)->AddTime(fTimeElapsed);
    }

    return true;
}

void AIRAnimationMixer::AddAnimationStateAction(CAIRAnimationState* pState)
{
    //m_vtAnimationStates.push_back(pState);
    m_vtAnimationActions.push_back(pState);
}

void AIRAnimationMixer::AddAnimationStateCircle(CAIRAnimationState* pState)
{
    m_vtAnimationCircles.push_back(pState);
}

bool AIRAnimationMixer::IsEnd()
{
    /*
    if (!m_bBlendToNoMixer)
    {
        return false;
    }
    CAIRAnimationState* pState = NULL;
    for (size_t i = 0; i < m_vtAnimationStates.size(); ++i)
    {
        pState = m_vtAnimationStates[i];
        if (!pState->IsLoop() && pState->GetTimePos() >= pState->GetTimeLength())
        {
            return true;
        }
    }
    */

    return m_vtAnimationActions.size() == 0;
}

bool AIRAnimationMixer::IsWorkable(CAIRAnimationState* pCurState)
{
    /*
    CAIRAnimationState* pState = NULL;
    for (size_t i = 0; i < m_vtAnimationStates.size(); ++i)
    {
        pState = m_vtAnimationStates[i];
        if (pState == pCurState)
        {
            return true;
        }
    }
    */
    if (pCurState == m_vtAnimationCircles[0])
    {
        return true;
    }

    return false;
}

void AIRAnimationMixer::SetBlend(CAIRAnimationBlend::AnimationBlendMode blendMode, CAIRAnimationState* pSource, float fBlendTime)
{
    /*
    if (!m_animationBlend.IsBlending())
    {
        m_animationBlend.StartBlend(blendMode, pSource, GetTargetState(pSource), fBlendTime);
        m_fSourceStartBlendTime = pSource->GetTimePos();
        for (size_t i = 0; i < m_vtAnimationStates.size(); ++i)
        {
            if (pSource != m_vtAnimationStates[i])
            {
                m_vtAnimationStates[i]->SetTimePos(0);
            }
        }
        m_bBlendToNoMixer = false;
    }
    */
}

CAIRAnimationState* AIRAnimationMixer::GetTargetState(CAIRAnimationState* pSource)
{
    return NULL;
}

void AIRAnimationMixer::Excute(CModelInstance* pModelInst, float fDelayIn /* = 0.2f */, float fDelayOut /* = 0.2f */)
{
    m_vtAnimationActions.clear();
    MapIterator<AIRAnimationMixData::MapAnimationWeights> weight_iter = m_pAnimationMixData->GetAnimationActionWeightsIter();
    CAIRAnimationState* pStateAction = NULL;
    while (!weight_iter.IsEnd())
    {
        pStateAction = pModelInst->GetAnimationState(weight_iter.PeekNextKey()->GetName());
        if (pStateAction)
        {
            AddAnimationStateAction(pStateAction);
            //pState->SetLoop(false);
        }
        weight_iter.MoveNext();
    }
    for (size_t i = 0; i < m_vtAnimationActions.size(); ++i)
    {
        m_vtAnimationActions[i]->ExcuteAction(fDelayIn, fDelayOut);
    }
    CAIRAnimationState* pStateCircle = m_vtAnimationCircles[0];
    m_fSourceStartBlendTime = pStateCircle->CaculateTimePos(pStateCircle->GetTimePos() + pStateAction->GetDelayIn());
    m_fTargetBlendEndTime = pStateCircle->CaculateTimePos(pStateAction->GetMixTimeLength() + pStateCircle->GetTimePos());
}
