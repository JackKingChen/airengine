#include "stdafx.h"
#include "PointEmitter.h"

CPointEmitter::CPointEmitter()
{
    m_strType = "PointEmitter";
}

CPointEmitter::~CPointEmitter()
{

}

void CPointEmitter::InitParitcle(CParticle* pParticle)
{
    pParticle->m_position = m_position;

    // Generate complex data by reference
    GenEmissionColour(pParticle->m_color);
    GenEmissionDirection(pParticle->m_direction);
    GenEmissionVelocity(pParticle->m_direction);

    // Generate simpler data
    pParticle->m_fLifeTime = pParticle->m_fTotalTimeToLive = GenEmissionTTL();
}

bool CPointEmitter::SaveToXMLNode(CMarkupSTL& xml)
{
    xml.AddAttrib("type", "point");
    return CParticleEmitter::SaveToXMLNode(xml);
}
