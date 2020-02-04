#include "stdafx.h"
#include "ScaleAffector.h"
#include "ParticleSystem.h"

CScaleAffector::CScaleAffector() : m_fScaleFactor(0)
{
    //m_type = AT_Scalor;
}

CScaleAffector::~CScaleAffector()
{

}

void CScaleAffector::AffectParticle(CParticleSystem* pParticleSystem, float fTimeElapsed)
{
    float fDS = m_fScaleFactor * fTimeElapsed;

    CParticleIterator particleIter = pParticleSystem->GetIterator();

    CParticle* pParticle = NULL;
    while (!particleIter.IsEnd())
    {
        pParticle = particleIter.GetNext();
        pParticle->m_fWidth += fDS;
        pParticle->m_fHeight += fDS;
    }
    
}

void CScaleAffector::SetScaleFactor(float fFactor)
{
    m_fScaleFactor = fFactor;
}

bool CScaleAffector::LoadFromXML(CMarkupSTL& xml)
{
    AIRString strValue = xml.GetAttrib("scale_factor").c_str();

    if (strValue.length() == 0)
    {
        return false;
    }

    m_fScaleFactor = atof(strValue.c_str());

    return true;
}

bool CScaleAffector::SaveToXMLNode(CMarkupSTL& xml)
{
    xml.AddAttrib("type", "Scaler");
    char szTmp[32] = { 0 };
    sprintf(szTmp, "%f", m_fScaleFactor);
    xml.AddAttrib("scale_factor", szTmp);
    return true;
}
