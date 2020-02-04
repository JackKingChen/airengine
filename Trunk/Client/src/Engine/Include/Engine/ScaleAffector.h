/********************************************************************
	created:	2009/12/21
	created:	21:12:2009   19:26
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\ScaleAffector.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	ScaleAffector
	file ext:	h
	author:		liangairan
	
	purpose:	Ëõ·ÅÓ°ÏìÆ÷
*********************************************************************/


#pragma once 

#include "ParticleAffector.h"

class EngineExport CScaleAffector : public CParticleAffector
{
public:
    CScaleAffector();

    virtual ~CScaleAffector();

    virtual void AffectParticle(CParticleSystem* pParticleSystem, float fTimeElapsed);

    virtual bool LoadFromXML(CMarkupSTL& xml);

    virtual void SetScaleFactor(float fFactor);

    virtual float GetScaleFactor() 
    {
        return m_fScaleFactor;
    }

    virtual bool SaveToXMLNode(CMarkupSTL& xml);

    virtual AIRString GetTypeStr()
    {
        return "Scaler";
    }
protected:
private:

    float m_fScaleFactor;
};
