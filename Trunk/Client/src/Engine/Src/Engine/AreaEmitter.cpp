#include "stdafx.h"
#include "AreaEmitter.h"
#include "ParticleEmitter.h"


CAreaEmitter::CAreaEmitter() : m_Size(10.0f, 10.0f, 10.0f), m_XRange(Vector3Df::UNIT_X), m_YRange(Vector3Df::UNIT_Y), m_ZRange(Vector3Df::UNIT_Z)
{

}

CAreaEmitter::~CAreaEmitter()
{

}

void CAreaEmitter::SetDirection(const Vector3Df& vDirection)
{
    //m_direction = vDirection;
	CParticleEmitter::SetDirection(vDirection);
    GenAreaAxis();
}

void CAreaEmitter::SetSize(float fWidth, float fHeight, float fDepth)
{
    m_Size.x = fWidth;
    m_Size.y = fHeight;
    m_Size.z = fDepth;

    GenAreaAxis();
}

void CAreaEmitter::SetSize(const Vector3Df& vSize)
{
    m_Size = vSize;
    GenAreaAxis();
}

void CAreaEmitter::SetWidth(float fWidth)
{
    m_Size.x = fWidth;
    GenAreaAxis();
}

void CAreaEmitter::SetHeight(float fHeight)
{
    m_Size.y = fHeight;
    GenAreaAxis();
}

void CAreaEmitter::SetDepth(float fDepth)
{
    m_Size.z = fDepth;
    GenAreaAxis();
}

void CAreaEmitter::GenAreaAxis()
{
    Vector3Df vLeft = m_vUp.CrossProduct(m_direction);

    m_XRange = vLeft * (m_Size.x * 0.5f);
    m_ZRange = m_vUp * (m_Size.z * 0.5f);
    m_YRange = m_direction * (m_Size.y * 0.5f);
}


bool CAreaEmitter::LoadFromXML(CMarkupSTL& xml)
{
    CParticleEmitter::LoadFromXML(xml);
    AIRString strValue("");
    strValue = xml.GetAttrib("size").c_str();
    Vector3Df size;
    sscanf(strValue.c_str(), "(%f,%f,%f)", &size.x, &size.y, &size.z);
    SetSize( size );
    return true;
}

bool CAreaEmitter::SaveToXMLNode(CMarkupSTL& xml)
{
    CParticleEmitter::SaveToXMLNode(xml);
    char szTmp[256] = { 0 };
    sprintf(szTmp, "(%f,%f,%f)", m_Size.x, m_Size.y, m_Size.z);
    xml.AddAttrib("size", szTmp);

    return true;
}

void CAreaEmitter::Copy(CParticleEmitter* pEmitterClone)
{
    CParticleEmitter::Copy(pEmitterClone);

    CAreaEmitter* pCopy = dynamic_cast<CAreaEmitter*>(pEmitterClone);
    if (pCopy)
    {
        m_Size = pCopy->m_Size;
        m_XRange = pCopy->m_XRange;
        m_YRange = pCopy->m_YRange;
        m_ZRange = pCopy->m_ZRange;
    }
    GenAreaAxis();
}
