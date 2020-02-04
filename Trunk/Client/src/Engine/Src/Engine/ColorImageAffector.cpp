#include "stdafx.h"
#include "ColorImageAffector.h"
#include "ParticleIterator.h"
#include "ParticleSystem.h"
#include "FileSystem.h"

CColorImageAffector::CColorImageAffector() : m_bLoadImage(false)
{

}

CColorImageAffector::~CColorImageAffector()
{

}

void CColorImageAffector::SetImageFile(const AIRString& strFile)
{
    if (m_imgColor.LoadFromFile(strFile.c_str()))
    {
        m_bLoadImage = true;
        m_strImageFile = strFile;
    }
    else
        m_bLoadImage = false;
}

void CColorImageAffector::AffectParticle(CParticleSystem* pParticleSystem, float fTimeElapsed)
{
    if (!m_bLoadImage)
    {
        LoadImage();
    }
    if (!m_bLoadImage)
    {
        return;
    }

    int nWidth = m_imgColor.GetWidth() - 1;

    CParticleIterator particleIter = pParticleSystem->GetIterator();

    CParticle* pParticle = NULL;
    while (!particleIter.IsEnd())
    {
        pParticle = particleIter.GetNext();
        
        float fParticleTime = 1.0f - (pParticle->m_fLifeTime / pParticle->m_fTotalTimeToLive);
        if (fParticleTime > 1.0f)
        {
            fParticleTime = 1.0f;
        }
        else if (fParticleTime < 0.0f)
        {
            fParticleTime = 0.0f;
        }

        const float fIndex = fParticleTime * nWidth;
        const int nIndex = (int)fIndex;

        if(nIndex >= nWidth) 
        {
            pParticle->m_color = m_imgColor.GetPixelColor(nWidth, 0);
        }
        else
        {
            // Linear interpolation
            const float		fract		= fIndex - (float)nIndex;
            const float		to_colour	= fract;
            const float		from_colour	= 1.0f - to_colour;

            CColorValue from = m_imgColor.GetPixelColorValue(nIndex, 0),
                to = m_imgColor.GetPixelColorValue(nIndex, 0);

            pParticle->m_color.r = from.r * from_colour + to.r * to_colour;
            pParticle->m_color.g = from.g * from_colour + to.g * to_colour;
            pParticle->m_color.b = from.b * from_colour + to.b * to_colour;
            pParticle->m_color.a = from.a * from_colour + to.a * to_colour;
        }
    }
}

void CColorImageAffector::LoadImage()
{
    if (m_strImageFile.length() > 0)
    {
		m_imgColor.LoadFromFile(m_strImageFile.c_str());
    }
    
}

bool CColorImageAffector::SaveToXMLNode(CMarkupSTL& xml)
{
    xml.AddAttrib("type", "ColorImage");
    xml.AddAttrib("color_image_file", m_strImageFile.c_str());

    return true;
}

bool CColorImageAffector::LoadFromXML(CMarkupSTL& xml)
{
    m_strImageFile = xml.GetAttrib("color_image_file").c_str();
    LoadImage();
    return true;
}
