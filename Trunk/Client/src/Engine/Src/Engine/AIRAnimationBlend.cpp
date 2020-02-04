#include "stdafx.h"
#include "AIRAnimationBlend.h"

CAIRAnimationBlend::CAIRAnimationBlend() :
m_pSource(NULL),
m_pTarget(NULL),
m_fBlendRemain(0),
m_fBlendTime(0),
m_bBlending(false)
{

}

CAIRAnimationBlend::~CAIRAnimationBlend()
{

}

void CAIRAnimationBlend::StartBlend(AnimationBlendMode blendMode, CAIRAnimationState* pSource, CAIRAnimationState* pTarget, float fBlendTime)
{
    //========================================================
    // start modify by hjj 2010-3-8
    // reason: 添加直接切换动画的功能
    //// 新的代码:
    //m_pSource = pSource;
    //m_pTarget = pTarget;
    //m_pTarget->SetTimePos(0.0f);
    //if (blendMode == Blend_Transition)
    //{
    //    m_pTarget->SetWeight(0.0);
    //    m_pSource->SetWeight(1.0);
    //    m_fBlendRemain = fBlendTime;
    //    m_fBlendTime = fBlendTime;
    //    m_bBlending = true;
    //}
    //else if ( blendMode == Blend_Switch )
    //{
    //    m_pTarget->SetWeight( 1.0f );
    //    // 如果m_pTarget != m_pSource,那么m_pSource的混合权重应该设为0.0
    //    if ( m_pTarget != m_pSource )
    //    {
    //        m_pSource->SetWeight( 0.0f );
    //    }
    //    m_fBlendRemain  = 0.0f;
    //    m_fBlendTime = fBlendTime;
    //    // 不再进行动画混合
    //    m_bBlending = false;
    //}
    //旧代码:
    m_pSource = pSource;
    m_pTarget = pTarget;
    m_pTarget->SetTimePos(0.0f);
    if (blendMode == Blend_Transition)
    {
        m_pTarget->SetWeight(0.0);
        m_pSource->SetWeight(1.0);
    }
    m_fBlendRemain = fBlendTime;
    m_fBlendTime = fBlendTime;
    m_bBlending = true;
    // end modify by hjj 2010-3-8
    //========================================================
}

void CAIRAnimationBlend::StartBlend(AnimationBlendMode blendMode, CAIRAnimationState* pSource, CAIRAnimationState* pTarget, float fStartBlendTime, float fBlendTime )
{
	if ( fStartBlendTime < 0.0 )
	{
		fStartBlendTime = 0.0;
	}
	if ( fBlendTime < 0.0 )
	{
		fBlendTime = 0.5;
	}
	if ( fStartBlendTime > fBlendTime )
	{
		fStartBlendTime = fBlendTime;
	}
	m_pSource = pSource;
	m_pTarget = pTarget;
	m_pTarget->SetTimePos(0.0);
	if (blendMode == Blend_Transition)
	{
		float weight = fStartBlendTime / fBlendTime;
		m_pTarget->SetWeight(weight);
		m_pSource->SetWeight(1.0 - weight);
	}
	m_fBlendRemain = fBlendTime - fStartBlendTime;
	m_fBlendTime = fBlendTime;
	m_bBlending = true;
}

void CAIRAnimationBlend::Update(float fTimeElapse)
{
    if (m_pSource && m_pTarget )
    {
        //m_pSource->AddTime(fTimeElapse);
        //m_pTarget->AddTime(fTimeElapse);
        m_fBlendRemain -= fTimeElapse;
        m_bBlending = (m_fBlendRemain >= 0.0f);

        if (m_bBlending)
        {
            float fSrcWeight = m_fBlendRemain / m_fBlendTime;
            m_pSource->SetWeight(fSrcWeight);
            m_pTarget->SetWeight(1.0f - fSrcWeight);
        }
        else
        {
            m_pSource->SetWeight(0.0f);
            m_pTarget->SetWeight(1.0f);
        }
    }
    
}

void CAIRAnimationBlend::StopBlend( void )
{
	if (m_pSource && m_pTarget )
	{
		m_pSource->SetWeight(0.0f);
		m_pTarget->SetWeight(1.0f);
		m_bBlending = false;
	}
}

void CAIRAnimationBlend::UpdateForMixer(float fTimeElapse)
{
    m_fBlendRemain -= fTimeElapse;
    m_bBlending = (m_fBlendRemain >= 0.0f);

    if (m_pSource)
    {
        m_pSource->AddTime(fTimeElapse);
    }
}

bool CAIRAnimationBlend::IsBlending()
{
    return m_bBlending;
}

float CAIRAnimationBlend::GetSourceWeight(float fStartWeight) const
{
    if (!m_bBlending)
    {
        return 0;
    }
    return m_fBlendRemain / m_fBlendTime * fStartWeight;
}

float CAIRAnimationBlend::GetTargetWeight(float fEndWeight) const
{
    if (!m_bBlending)
    {
        return fEndWeight;
    }
    return (1.0f - m_fBlendRemain / m_fBlendTime) * fEndWeight;
}
