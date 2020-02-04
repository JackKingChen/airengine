// ***************************************************************
//  文件: ParticleEmitterFactory.h
//  日期: 2010-1-8
//  作者: huangjunjie
//  说明: 粒子发射器工厂类
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