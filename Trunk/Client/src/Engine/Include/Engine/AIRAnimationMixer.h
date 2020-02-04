/********************************************************************
	created:	2010/09/08
	created:	8:9:2010   17:42
	filename: 	AIRAnimationMixer.h
	author:		liangairan
	
	purpose:	动作混合，支持不同动作的混合
*********************************************************************/

#pragma once 

#include "EngineConfig.h"
#include <vector>
#include "AIRAnimationBlend.h"

class CModelInstance;
class AIRAnimationMixData;
class CAIRAnimationState;
class CModelInstance;

class EngineExport AIRAnimationMixer
{
public:
    AIRAnimationMixer(const char* szName, AIRAnimationMixData* pMixData);
    virtual ~AIRAnimationMixer();

    void Apply(CModelInstance* pModelInstance);

    void AddAnimationStateAction(CAIRAnimationState* pState);

    void AddAnimationStateCircle(CAIRAnimationState* pState);

    //更新混合
    bool Update(float fTimeElapsed);

    const AIRString& GetName() const
    {
        return m_strName;
    }

    //动作混合是否已经结束
    bool IsEnd();

    //通过角色现有状态来调整
    bool IsWorkable(CAIRAnimationState* pCurState);

    //source
    void SetBlend(CAIRAnimationBlend::AnimationBlendMode blendMode, CAIRAnimationState* pSource, float fBlendTime = 0.15f);

    void Excute(CModelInstance* pModelInst, float fDelayIn = 0.2f, float fDelayOut = 0.2f);

    bool IsLoop() const
    {
        return m_bLoop;
    }

    void SetLoop(bool bLoop)
    {
        m_bLoop = bLoop;
    }
protected:
private:
    CAIRAnimationState*  GetTargetState(CAIRAnimationState* pSource);

    AIRVector<CAIRAnimationState*> m_vtAnimationActions;

    AIRVector<CAIRAnimationState*> m_vtAnimationCircles;   //m_vtAnimationStates;
    typedef AIRVector<CAIRAnimationState*>::iterator AnimationStateIter;

    AIRAnimationMixData* m_pAnimationMixData;
    AIRString m_strName;    //名字
    CAIRAnimationBlend			m_animationBlend;       // 动作混合器
    float                m_fSourceStartBlendTime;       //源动作的当前时间，即混合的时间
    float                m_fTargetBlendEndTime;
    bool                 m_bBlendToNoMixer;      //mix动作做完后，过渡回当前动作
    bool                 m_bLoop;                //是否循环
};

