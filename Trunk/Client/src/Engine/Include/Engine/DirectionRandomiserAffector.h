/********************************************************************
	created:	2010/01/29
	created:	29:1:2010   11:11
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\DirectionRandomiserAffector.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	DirectionRandomiserAffector
	file ext:	h
	author:		liangairan
	
	purpose:	随机方向影响器
*********************************************************************/

#pragma once 


#include "ParticleAffector.h"

class EngineExport CDirectionRandomiserAffector : public CParticleAffector
{
public:
    CDirectionRandomiserAffector();

    virtual ~CDirectionRandomiserAffector();

    virtual void AffectParticle(CParticleSystem* pParticleSystem, float fTimeElapsed);

    virtual bool LoadFromXML(CMarkupSTL& xml);

    virtual bool SaveToXMLNode(CMarkupSTL& xml);

    float GetRandomness()
    {
        return m_fRandomness;
    }

    float GetScope()
    {
        return m_fScope;
    }

    bool IsKeepVelocity()
    {
        return m_bKeepVelocity;
    }

    void SetRandomness(float fRandomness);

    void SetScope(float fScope);

    void SetKeepVelocity(bool bKeepVolicity);

    virtual AIRString GetTypeStr()
    {
        return "DirectionRandomiser";
    }
protected:
    float m_fRandomness;
    float m_fScope;
    //是否要保持速度
    bool  m_bKeepVelocity;
private:
};
