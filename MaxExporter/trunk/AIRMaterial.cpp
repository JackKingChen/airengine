#include "AIRMaterial.h"

CAIRMaterial::CAIRMaterial(void) : m_pMaterial(NULL), m_bAlphaBlend(false)
{
}

CAIRMaterial::~CAIRMaterial(void)
{
}

void CAIRMaterial::Clear()
{
    if (m_pMaterial)
    {
        delete m_pMaterial;
        m_pMaterial = NULL;
    }
}

bool CAIRMaterial::Create(CMaxMaterial* pMaterial)
{
    m_pMaterial = pMaterial;
    return true;
}

void CAIRMaterial::GetAmbientColor(float *pColor)
{
    m_pMaterial->GetAmbientColor(pColor);
}

void CAIRMaterial::GetDiffuseColor(float *pColor)
{
    m_pMaterial->GetDiffuseColor(pColor);
}

void CAIRMaterial::GetSpecularColor(float *pColor)
{
    m_pMaterial->GetSpecularColor(pColor);
}

float CAIRMaterial::GetShininess()
{
    return m_pMaterial->GetShininess();
}

bool CAIRMaterial::IsAlphaBlend() const
{
    float fOpacity = m_pMaterial->GetOpacity(0);
    return (fOpacity < 1.0f);
}

bool CAIRMaterial::IsAlphaTest() const
{
    return m_pMaterial->IsAlphaTest();
}

bool CAIRMaterial::IsTwoSide() const
{
    return m_pMaterial->GetTwoSide();
}
