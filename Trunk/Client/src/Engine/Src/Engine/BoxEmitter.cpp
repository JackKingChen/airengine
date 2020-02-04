#include "stdafx.h"
#include "BoxEmitter.h"
#include "MyMath.h"

CBoxEmitter::CBoxEmitter() 
{
    m_strType = "BoxEmitter";
}

CBoxEmitter::~CBoxEmitter()
{

}

void CBoxEmitter::InitParitcle(CParticle* pParticle)
{
    Vector3Df xOff, yOff, zOff;

    // Call superclass
    //CParticleEmitter::InitParitcle(pParticle);

    xOff = m_XRange * AIR::SymmetricRandom();
    yOff = m_YRange * AIR::SymmetricRandom();
    zOff = m_ZRange * AIR::SymmetricRandom();

    pParticle->m_position = m_position + xOff + yOff + zOff;


    // Generate complex data by reference
    GenEmissionColour(pParticle->m_color);
    GenEmissionDirection(pParticle->m_direction);
    GenEmissionVelocity(pParticle->m_direction);

    // Generate simpler data
    pParticle->m_fLifeTime = pParticle->m_fTotalTimeToLive = GenEmissionTTL();
}

bool CBoxEmitter::LoadFromXML(CMarkupSTL& xml)
{
    CAreaEmitter::LoadFromXML(xml);
    return true;
}

bool CBoxEmitter::SaveToXMLNode(CMarkupSTL& xml)
{
    //xml.AddAttrib("type", m_strType.c_str());
    CAreaEmitter::SaveToXMLNode(xml);
    return true;
}
