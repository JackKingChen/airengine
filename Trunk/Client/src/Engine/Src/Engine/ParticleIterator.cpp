#include "stdafx.h"
#include "ParticleIterator.h"


CParticleIterator::CParticleIterator(AIRList<CParticle*>::iterator start, AIRList<CParticle*>::iterator end)
{
    m_Start = m_Pos = start;
    m_End = end;
}

CParticleIterator::~CParticleIterator()
{

}

bool CParticleIterator::IsEnd()
{
    return m_Pos == m_End;
}

CParticle* CParticleIterator::GetNext()
{
    return static_cast<CParticle*>(*m_Pos++);
}
