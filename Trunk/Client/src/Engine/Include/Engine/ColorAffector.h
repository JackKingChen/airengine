/********************************************************************
	created:	2009/12/22
	created:	22:12:2009   14:13
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\ColorAffector.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	ColorAffector
	file ext:	h
	author:		liangairan
	
	purpose:	粒子颜色影响器
*********************************************************************/

#pragma once 

#include "ParticleAffector.h"
#include "ColorValue.h"
#include "Color.h"

class EngineExport CColorAffector : public CParticleAffector
{
public:
    CColorAffector();
    virtual ~CColorAffector();

    virtual void AffectParticle(CParticleSystem* pParticleSystem, float fTimeElapsed);

    //virtual void InitParticle(CParticle* pParticle);

    virtual bool LoadFromXML(CMarkupSTL& xml);

    virtual bool SaveToXMLNode(CMarkupSTL& xml);

    virtual AIRString GetTypeStr()
    {
        return "ColorFader";
    }

    float GetAlphaAdj()
    {
        return m_fAlphaAdj;
    }

    float GetRedAdj()
    {
        return m_fRedAdj;
    }

    float GetGreenAdj()
    {
        return m_fGreenAdj;
    }

    float GetBlueAdj()
    {
        return m_fBlueAdj;
    }

    void SetAlphaAdj(float fAlphaAdj);

    void SetRedAdj(float fRedAdj);

    void SetGreenAdj(float fGreenAdj);

    void SetBlueAdj(float fBlueAdj);
protected:

    /** Internal method for adjusting while clamping to [0,1] */
    inline void ApplyAdjustWithClamp(float* pComponent, float adjust)
    {
        *pComponent += adjust;
        // Limit to 0
        if (*pComponent < 0.0)
        {
            *pComponent = 0.0f;
        }
        // Limit to 1
        else if (*pComponent > 1.0)
        {
            *pComponent = 1.0f;
        }
    }

    float m_fRedAdj;
    float m_fGreenAdj;
    float m_fBlueAdj;
    float m_fAlphaAdj;
private:
    //CColorValue  m_colorSpeedRangeStart;
    //CColorValue  m_colorSpeedRangeEnd;

    //bool    m_bColorAdd;  //是否为颜色加,false为颜色减
};
