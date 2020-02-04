#include "stdafx.h"
#include "ParticleEmitterFactory.h"
#include "ParticleEmitter.h"
#include "BoxEmitter.h"
#include "PointEmitter.h"
#include "AreaEmitter.h"
#include "RingEmitter.h"
#include "EllipsoidEmitter.h"
#include "CylinderEmitter.h"
#include "Util.h"


CParticleEmitterFactory::CParticleEmitterFactory()
{

}

CParticleEmitterFactory::~CParticleEmitterFactory()
{

}

CParticleEmitter* CParticleEmitterFactory::CreateEmitter(const AIRString& strType, CMarkupSTL& xml)
{
    CParticleEmitter* pEmitter = CreateEmitter(strType);

    if (!pEmitter)
    {
        return 0;
    }

    if ( !pEmitter->LoadFromXML(xml) )
    {
        SafeDelete(pEmitter);
    }

    return pEmitter;
}

CParticleEmitter* CParticleEmitterFactory::CreateEmitter(const AIRString& strType)
{
    CParticleEmitter* pEmitter = NULL;
    if (strType == "PointEmitter")
    {
        pEmitter = new CPointEmitter;
    }
    else if (strType == "BoxEmitter")
    {
        pEmitter = new CBoxEmitter;
    }
    else if (strType == "RingEmitter")
    {
        pEmitter = new CRingEmitter;
    }
    else if (strType == "EllipsoidEmitter")
    {
        pEmitter = new CEllipsoidEmitter;
    }
    else if (strType == "CylinderEmitter")
    {
        pEmitter = new CCylinderEmitter;
    }

    return pEmitter;
}
