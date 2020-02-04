/********************************************************************
	created:	2010/01/27
	created:	27:1:2010   13:58
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\ParticleIterator.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	ParticleIterator
	file ext:	h
	author:		liangairan
	
	purpose:	Á£×Óµø´úÆ÷
*********************************************************************/

#pragma once 

class CParticleSystem;
#include "Particle.h"

class EngineExport CParticleIterator
{
public:
    friend class CParticleSystem;

    virtual ~CParticleIterator();

    bool IsEnd();

    CParticle* GetNext();
protected:
    AIRList<CParticle*>::iterator m_Pos;
    AIRList<CParticle*>::iterator m_Start;
    AIRList<CParticle*>::iterator m_End;

    /// Protected constructor, only available from ParticleSystem::getIterator
    CParticleIterator(AIRList<CParticle*>::iterator start, AIRList<CParticle*>::iterator end);
private:
};
