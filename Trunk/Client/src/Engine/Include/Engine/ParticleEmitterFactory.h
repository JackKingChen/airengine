// ***************************************************************
//  �ļ�: ParticleEmitterFactory.h
//  ����: 2010-1-8
//  ����: huangjunjie
//  ˵��: ���ӷ�����������
// ***************************************************************


#pragma once

#ifndef __ParticleEmitterFactory_H_
#define __ParticleEmitterFactory_H_

#include "ParticleEmitter.h"
#include "Singleton.h"

class EngineExport CParticleEmitterFactory : public Singleton<CParticleEmitterFactory>
{
    BE_SINGLETON(CParticleEmitterFactory);
public:
    ~CParticleEmitterFactory();

    CParticleEmitter* CreateEmitter(const AIRString& strType, CMarkupSTL& xml);

    CParticleEmitter* CreateEmitter(const AIRString& strType);

protected:


};

#define PARTICLEMITTER_FACTORY CParticleEmitterFactory::GetInstance()


#endif //! end __ParticleEmitterFactory_H_