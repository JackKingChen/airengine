/********************************************************************
	created:	2010/01/16
	created:	16:1:2010   15:04
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\AIRAnimationState.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	AIRAnimationState
	file ext:	h
	author:		liangairan
	
	purpose:	动画控制器，主要用于控制动画
*********************************************************************/

#pragma once 

#include "AIRAnimation.h"

enum AnimateStateStatus
{
    eSTATE_IN,
    eSTATE_STEADY,
    eSTATE_OUT,
};


class EngineExport CAIRAnimationState// : public EngineAllocator(CAIRAnimationState)
{
	EngineAllocator(CAIRAnimationState)
public:
    CAIRAnimationState();
    CAIRAnimationState(CAIRAnimation* pAnimation);
    virtual ~CAIRAnimationState();

    bool IsLoop() 
    {
        return m_bLoop;
    }

    void SetLoop(bool bLoop);

    void SetTimeLength(float fTimeLength);

    float GetTimePos() const
    {
        return m_fTimePos;
    }

    void SetTimePos(float fTimePos);

    void AddTime(float fOffset);

    float GetTimeLength() const
    {
        return m_fTimeLength;
    }

    const AIRString& GetName() const
    {
        return m_pAnimation->GetName();
    }

    CAIRAnimation* GetAnimationRes() const
    {
        return m_pAnimation;
    }

    //virtual CAIRSkeleton* GetFrameSkeleton(int nFrame);

    //float GetKeyFramesAtTime(CAIRSkeleton** pKeyFrame1, CAIRSkeleton** pKeyFrame2);

    void  SetAnimation(CAIRAnimation* pAnimation);

    float GetWeight() const
    {
        return m_fWeight;
    }

    void SetWeight(float fWeight);

	void SetPauseAnimation( bool bPause )
	{
		m_bPauseAnim = bPause;
	}

	void SetEnable( bool b )
	{
		m_bEnable = b;
	}

    //注意，这个函数是用在动作混合中
    //delayin是指从circle过度到action动作的时间
    //delayout是指action快要完结，过度到circle的时间
    void ExcuteAction(float fDelayIn, float fDelayOut);

    //在mixer里的update
    //返回true代表动作还没完成，返回false代表已经完成
    bool Update(float fOffset);

    AnimateStateStatus GetAnimationStateStatus() const
    {
        return m_state;
    }

    float GetMixTimePos() const
    {
        return m_fMixTimePos;
    }

    float GetDelayIn() const
    {
        return m_fDelayIn;
    }

    float GetDelayOut() const
    {
        return m_fDelayOut;
    }

    //根据时间计算出真正的位置
    float CaculateTimePos(float fTimePos) const;

    float GetMixTimeLength() const
    {
        return m_fMixTimeLength;
    }

    bool IsPause() const
    {
        return m_bPauseAnim;
    }

    bool IsEnable() const
    {
        return m_bEnable;
    }
protected:
    CAIRAnimation* m_pAnimation;
    //是否循环
    bool    m_bLoop;
    //时间长度
    float   m_fTimeLength;
    //位置
    float   m_fTimePos;
    //权重，用于动作混合
    float   m_fWeight;
	bool	m_bPauseAnim;
	bool	m_bEnable;

    AnimateStateStatus  m_state;   //当前状态
    float   m_fDelayIn;            //从circle过渡到action的时间
    float   m_fDelayOut;        
    float   m_fMixTimeLength;
    float   m_fMixTimePos;
private:
};

