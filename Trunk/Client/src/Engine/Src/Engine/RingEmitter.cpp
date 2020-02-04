#include "stdafx.h"
#include "RingEmitter.h"

CRingEmitter::CRingEmitter() : m_fInnerSizeX(0.5), m_fInnerSizeY(0.5)
{
    m_strType = "RingEmitter";
}

CRingEmitter::~CRingEmitter()
{

}

void CRingEmitter::InitParitcle(CParticle* pParticle)
{
    float a, b, x, y, z;

    // Call superclass
    //AreaEmitter::_initParticle(pParticle);
    // create a random angle from 0 .. PI*2
    float alpha ( AIR::RangeRandom(0, AIR::TWO_PI) );
 
    // create two random radius values that are bigger than the inner size
    a = AIR::RangeRandom(m_fInnerSizeX, 1.0);
    b = AIR::RangeRandom(m_fInnerSizeY, 1.0);

    // with a and b we have defined a random ellipse inside the inner
    // ellipse and the outer circle (radius 1.0)
    // with alpha, and a and b we select a random point on this ellipse
    // and calculate it's coordinates
    x = a * sin(alpha);
    z = b * cos(alpha);
    // the height is simple -1 to 1
    y = AIR::SymmetricRandom();     

    // scale the found point to the ring's size and move it
    // relatively to the center of the emitter point

    pParticle->m_position = m_position + 
       m_XRange * x * 2.0f + m_YRange * y + m_ZRange * z * 2.0f;

    // Generate complex data by reference
    GenEmissionColour(pParticle->m_color);
    GenEmissionDirection(pParticle->m_direction);
    GenEmissionVelocity(pParticle->m_direction);

    // Generate simpler data
    pParticle->m_fLifeTime = pParticle->m_fTotalTimeToLive = GenEmissionTTL();
}

void CRingEmitter::SetInnerSizeX(float fInnerSizeX)
{
    m_fInnerSizeX = fInnerSizeX;
}

void CRingEmitter::SetInnerSizeY(float fInnerSizeY)
{
    m_fInnerSizeY = fInnerSizeY;
}

bool CRingEmitter::LoadFromXML(CMarkupSTL& xml)
{
    bool bRet = CAreaEmitter::LoadFromXML(xml);

    if (!bRet)
    {
        return false;
    }

    AIRString strValue = xml.GetAttrib("inner_size_x").c_str();
    m_fInnerSizeX = atof(strValue.c_str());

    strValue = xml.GetAttrib("inner_size_y").c_str();
    m_fInnerSizeY = atof(strValue.c_str());

    return true;
}

bool CRingEmitter::SaveToXMLNode(CMarkupSTL& xml)
{
    CAreaEmitter::SaveToXMLNode(xml);

    char szTmp[256] = { 0 };
    sprintf(szTmp, "%f", m_fInnerSizeX);
    xml.AddAttrib("inner_size_x", szTmp);

    sprintf(szTmp, "%f", m_fInnerSizeY);
    xml.AddAttrib("inner_size_y", szTmp);
    return true;
}

void CRingEmitter::Copy(CParticleEmitter* pEmitterClone)
{
    CAreaEmitter::Copy(pEmitterClone);

    CRingEmitter* pCopy = dynamic_cast<CRingEmitter*>(pEmitterClone);
    if (pCopy)
    {
        m_fInnerSizeX = pCopy->m_fInnerSizeX;
        m_fInnerSizeY = pCopy->m_fInnerSizeY;
    }
    
}
