/********************************************************************
	created:	2010/10/18
	created:	18:10:2010   16:24
	filename: 	ManualParticleSystem.h
	author:		liangairan
	
	purpose:	�ֶ�����ϵͳ��������ϵͳ�����ӷ�����ȫ�ֶ�����
*********************************************************************/

#pragma once 

#include "ParticleSystem.h"

class EngineExport ManualParticleSystem : public CParticleSystem
{
public:
    ManualParticleSystem();
    virtual ~ManualParticleSystem();

    void EmitParticles(int nParticlesNum);

    CParticle* EmitAParticle();

    virtual void Update(float fTimeElapse);

    virtual void PreRender(CCamera* pCamera);
protected: 
    virtual void OnUpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue);
private:
};
