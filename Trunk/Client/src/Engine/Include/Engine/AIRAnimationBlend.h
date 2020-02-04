/********************************************************************
	created:	2010/01/18
	created:	18:1:2010   17:29
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\AIRAnimationBlend.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	AIRAnimationBlend
	file ext:	h
	author:		liangairan
	
	purpose:	���������еĶ�����ϣ���ʱֻ���ڶ������ɺ�ƽ�����
*********************************************************************/
#pragma once

#include "AIRAnimationState.h"


class EngineExport CAIRAnimationBlend
{
public:
    enum AnimationBlendMode
    {
        Blend_Switch,		//�����л�
        Blend_Transition,	//��������
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

    //��ʼ���
    //@blendMode ��Ϸ�ʽ
    //@pSource   Դ����
    //@pTarget   Ŀ�궯��
    virtual void StartBlend(AnimationBlendMode blendMode, CAIRAnimationState* pSource, CAIRAnimationState* pTarget, float fBlendTime);

	virtual void StartBlend(AnimationBlendMode blendMode, CAIRAnimationState* pSource, CAIRAnimationState* pTarget, float fStartBlendTime, float fBlendTime );

    //����
    virtual void Update(float fTimeElapse);

	virtual void StopBlend( void );

    //ֻ��mix���õ��ĸ���
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
