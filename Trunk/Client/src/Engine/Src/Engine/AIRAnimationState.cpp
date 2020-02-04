#include "stdafx.h"
#include "AIRAnimationState.h"

CAIRAnimationState::CAIRAnimationState() : 
m_pAnimation(NULL),
m_bLoop(true),
m_fTimeLength(0),
m_fTimePos(0),
m_fWeight(1.0f),
m_bPauseAnim(false),
m_bEnable(true)
, m_state(eSTATE_STEADY)
, m_fDelayIn(0)
, m_fDelayOut(0)
, m_fMixTimeLength(0)
, m_fMixTimePos(0)
{

}

CAIRAnimationState::CAIRAnimationState(CAIRAnimation* pAnimation) : 
m_pAnimation(pAnimation),
m_bLoop(true),
m_fTimeLength(pAnimation->GetTimeRange()),
m_fTimePos(0),
m_fWeight(1.0f),
m_bPauseAnim(false),
m_bEnable(true)
, m_state(eSTATE_STEADY)
, m_fDelayIn(0)
, m_fDelayOut(0)
, m_fMixTimeLength(0)
, m_fMixTimePos(0)
{

}

CAIRAnimationState::~CAIRAnimationState()
{

}

void CAIRAnimationState::SetLoop(bool bLoop)
{
    m_bLoop = bLoop;
}

void CAIRAnimationState::SetTimeLength(float fTimeLength)
{
    m_fTimeLength = fTimeLength;
}

void CAIRAnimationState::SetTimePos(float fTimePos)
{
    if (fTimePos != m_fTimePos)
    {
        m_fTimePos = fTimePos;
		//如果是循环播放
        if (m_bLoop)
        {
            // Wrap
            m_fTimePos = fmod(m_fTimePos, m_fTimeLength);
            if(m_fTimePos < 0)
                m_fTimePos += m_fTimeLength;     
        }
		//非循环播放
        else
        {
            // Clamp
            if(m_fTimePos < 0)
                m_fTimePos = 0;
            else if (m_fTimePos > m_fTimeLength)
            {
                m_fTimePos = m_fTimeLength;
                //m_bEnable = false;
            }
        }

        //if (mEnabled)
        //    mParent->_notifyDirty();
    }
    //OutputDebugString(GetString("animation set time pos %f\n", m_fTimePos));
}

float CAIRAnimationState::CaculateTimePos(float fTimePos) const
{
    float fResPos = fmod(fTimePos, m_fTimeLength);
    if (fResPos < 0)
    {
        fResPos += m_fTimeLength;
    }

    return fResPos;
}

void CAIRAnimationState::AddTime(float fOffset)
{
	if ( !m_bPauseAnim && m_bEnable )
	{
		SetTimePos(m_fTimePos + fOffset);
	}    
}

//CAIRSkeleton* CAIRAnimationState::GetFrameSkeleton(int nFrame)
//{
//    return m_pAnimation->GetFrameSkeleton(nFrame);
//}

//float CAIRAnimationState::GetKeyFramesAtTime(CAIRSkeleton** pKeyFrame1, CAIRSkeleton** pKeyFrame2)
//{
//    return m_pAnimation->GetKeyFramesAtTime(m_fTimePos, pKeyFrame1, pKeyFrame2);
//}

void CAIRAnimationState::SetAnimation(CAIRAnimation* pAnimation)
{
    if (m_pAnimation)
    {
        m_pAnimation->Clear();
        SafeDelete(m_pAnimation);
    }

    m_pAnimation = pAnimation;
}

void CAIRAnimationState::SetWeight(float fWeight)
{
    m_fWeight = fWeight;
}

void CAIRAnimationState::ExcuteAction(float fDelayIn, float fDelayOut)
{
    m_fTimePos = 0;
    m_fDelayIn = fDelayIn;
    m_fDelayOut = fDelayOut;
    m_fMixTimeLength = m_fTimeLength + m_fDelayIn + m_fDelayOut;
    m_fMixTimePos = 0;
    m_state = eSTATE_IN;
}

bool CAIRAnimationState::Update(float fOffset)
{
    
    m_fMixTimePos += fOffset;

    if (m_state == eSTATE_IN)
    {
        if (m_fMixTimePos < m_fDelayIn)
        {
            m_fWeight = m_fMixTimePos / m_fDelayIn;
        }
        else
        {
            m_state = eSTATE_STEADY;
            m_fWeight = 1.0f;
        }
    }
    
    if (m_state == eSTATE_STEADY)
    {
        m_fTimePos += fOffset;
        //SetTimePos(m_fTimePos + fOffset);
        if (m_fMixTimePos >= m_fTimeLength + m_fDelayIn)
        {
            m_state = eSTATE_OUT;
            //m_fTimePos = m_fTimeLength;
        }
    }

    if (m_state == eSTATE_OUT)
    {
        if (m_fMixTimePos < m_fMixTimeLength)
        {
            m_fWeight = (m_fMixTimeLength - m_fMixTimePos) / m_fDelayOut;
        }
        else
        {
            m_fWeight = 0.0f;
            return false;
        }
    }
    
    /*
    m_fTimePos += fOffset;
    if (m_state == eSTATE_IN)
    {
        if (m_fTimePos < m_fDelayIn)
        {
            m_fWeight = m_fTimePos / m_fDelayIn;
        }
        else
        {
            m_state = eSTATE_STEADY;
            m_fWeight = 1.0f;
        }
    }

    if (m_state == eSTATE_STEADY)
    {
        //SetTimePos(m_fTimePos + fOffset);
        if (m_fTimePos >= m_fTimeLength)
        {
            m_state = eSTATE_OUT;
            //m_fTimePos = m_fTimeLength;
        }
    }

    if (m_state == eSTATE_OUT)
    {
        if (m_fTimePos < m_fTimeLength + m_fDelayOut)
        {
            m_fWeight = (m_fDelayOut - m_fTimePos + m_fTimeLength) / m_fDelayOut;
        }
        else
        {
            m_fWeight = 0.0f;
            return false;
        }
    }
    */

    return true;
}

