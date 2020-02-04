/********************************************************************
	created:	2010/09/08
	created:	8:9:2010   17:42
	filename: 	AIRAnimationMixer.h
	author:		liangairan
	
	purpose:	������ϣ�֧�ֲ�ͬ�����Ļ��
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

    //���»��
    bool Update(float fTimeElapsed);

    const AIRString& GetName() const
    {
        return m_strName;
    }

    //��������Ƿ��Ѿ�����
    bool IsEnd();

    //ͨ����ɫ����״̬������
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
    AIRString m_strName;    //����
    CAIRAnimationBlend			m_animationBlend;       // ���������
    float                m_fSourceStartBlendTime;       //Դ�����ĵ�ǰʱ�䣬����ϵ�ʱ��
    float                m_fTargetBlendEndTime;
    bool                 m_bBlendToNoMixer;      //mix��������󣬹��ɻص�ǰ����
    bool                 m_bLoop;                //�Ƿ�ѭ��
};

