/********************************************************************
	created:	2010/01/18
	created:	18:1:2010   17:29
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\AIRAnimationBlend.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	AIRAnimationBlend
	file ext:	h
	author:		liangairan
	
	purpose:	骨骼动画中的动作混合，暂时只用于动作过渡和平均混合
*********************************************************************/
#pragma once

#include "AIRAnimationState.h"


class EngineExport CAIRAnimationBlend
{
public:
    enum AnimationBlendMode
    {
        Blend_Switch,		//立即切换
        Blend_Transition,	//动作过渡
        //Blend_
    };
public:
    CAIRAnimationBlend();
    virtual ~CAIRAnimationBlend();

    virtual CAIRAnimationState* GetSource()
    {
        return m_pSource;
    }

    virtual CAIRAnimationState* GetTarget()
    {
        return m_pTarget;
    }

    //开始混合
    //@blendMode 混合方式
    //@pSource   源动画
    //@pTarget   目标动画
    virtual void StartBlend(AnimationBlendMode blendMode, CAIRAnimationState* pSource, CAIRAnimationState* pTarget, float fBlendTime);

	virtual void StartBlend(AnimationBlendMode blendMode, CAIRAnimationState* pSource, CAIRAnimationState* pTarget, float fStartBlendTime, float fBlendTime );

    //更新
    virtual void Update(float fTimeElapse);

	virtual void StopBlend( void );

    //只在mix中用到的更新
    void UpdateForMixer(float fTimeElapse);

    virtual bool IsBlending();

    float GetBlendRemain() const
    {
        return m_fBlendRemain;
    }

    float GetSourceWeight(float fStartWeight) const;

    float GetTargetWeight(float fEndWeight) const;
protected:
    CAIRAnimationState*  m_pSource;
    CAIRAnimationState*  m_pTarget;
    float                m_fBlendRemain;
    float                m_fBlendTime;
    bool                 m_bBlending;
private:
};
