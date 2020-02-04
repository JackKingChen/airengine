#include "stdafx.h"
#include "ColorAffector.h"
#include "ParticleSystem.h"

CColorAffector::CColorAffector() : m_fAlphaAdj(0), m_fRedAdj(0), m_fGreenAdj(0), m_fBlueAdj(0)
{

}

CColorAffector::~CColorAffector()
{

}

//void CColorAffector::InitParticle(CParticle* pParticle)
//{
//    int a 
//    pParticle->m_colorDelta = AIR::RangeRandom(m_colorSpeedRangeStart.color, m_colorSpeedRangeEnd.color);
//}

void CColorAffector::AffectParticle(CParticleSystem* pParticleSystem, float fTimeElapsed)
{
    CParticleIterator particleIter = pParticleSystem->GetIterator();

    CParticle* pParticle = NULL;
    while (!particleIter.IsEnd())
    {
        pParticle = particleIter.GetNext();
        ApplyAdjustWithClamp(&pParticle->m_color.a, m_fAlphaAdj * fTimeElapsed);
        ApplyAdjustWithClamp(&pParticle->m_color.r, m_fRedAdj * fTimeElapsed);
        ApplyAdjustWithClamp(&pParticle->m_color.g, m_fGreenAdj * fTimeElapsed);
        ApplyAdjustWithClamp(&pParticle->m_color.b, m_fBlueAdj * fTimeElapsed);
    }
    //pParticle->m_color += pParticle->m_colorDelta * fTimeElapsed;
    
}

bool CColorAffector::LoadFromXML(CMarkupSTL& xml)
{
    AIRString strValue = xml.GetAttrib("color_change_speed_start").c_str();
    float a, r, g, b;
    sscanf(strValue.c_str(), "(%f, %f, %f, %f)", &m_fAlphaAdj, &m_fRedAdj, &m_fGreenAdj, &m_fBlueAdj);


    strValue = xml.GetAttrib("color_change_speed_end").c_str();
    sscanf(strValue.c_str(), "(%f, %f, %f, %f)", &a, &r, &g, &b);
    //m_colorSpeedRangeEnd = SColor(a, r, g, b);

    //strValue = xml.GetAttrib("operator");

    return true;
}

bool CColorAffector::SaveToXMLNode(CMarkupSTL& xml)
{
    xml.AddAttrib("type", "Color");
    char szTmp[256] = { 0 };
    sprintf(szTmp, "(%f, %f, %f, %f)", m_fAlphaAdj, m_fRedAdj, m_fGreenAdj, m_fBlueAdj);
    xml.AddAttrib("color_change_speed_start", szTmp);
    return true;
}

void CColorAffector::SetAlphaAdj(float fAlphaAdj)
{
    m_fAlphaAdj = fAlphaAdj;
}

void CColorAffector::SetGreenAdj(float fGreenAdj)
{
    m_fGreenAdj = fGreenAdj;
}

void CColorAffector::SetRedAdj(float fRedAdj)
{
    m_fRedAdj = fRedAdj;
}

void CColorAffector::SetBlueAdj(float fBlueAdj)
{
    m_fBlueAdj = fBlueAdj;
}
