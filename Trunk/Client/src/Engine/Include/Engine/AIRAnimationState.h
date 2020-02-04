/********************************************************************
	created:	2010/01/16
	created:	16:1:2010   15:04
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\AIRAnimationState.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	AIRAnimationState
	file ext:	h
	author:		liangairan
	
	purpose:	��������������Ҫ���ڿ��ƶ���
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

    //ע�⣬������������ڶ��������
    //delayin��ָ��circle���ȵ�action������ʱ��
    //delayout��ָaction��Ҫ��ᣬ���ȵ�circle��ʱ��
    void ExcuteAction(float fDelayIn, float fDelayOut);

    //��mixer���update
    //����true��������û��ɣ�����false�����Ѿ����
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

    //����ʱ������������λ��
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
    //�Ƿ�ѭ��
    bool    m_bLoop;
    //ʱ�䳤��
    float   m_fTimeLength;
    //λ��
    float   m_fTimePos;
    //Ȩ�أ����ڶ������
    float   m_fWeight;
	bool	m_bPauseAnim;
	bool	m_bEnable;

    AnimateStateStatus  m_state;   //��ǰ״̬
    float   m_fDelayIn;            //��circle���ɵ�action��ʱ��
    float   m_fDelayOut;        
    float   m_fMixTimeLength;
    float   m_fMixTimePos;
private:
};

