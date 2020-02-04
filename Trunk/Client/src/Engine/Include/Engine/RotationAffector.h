/********************************************************************
	created:	2009/12/22
	created:	22:12:2009   10:42
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\RotateAffector.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	RotateAffector
	file ext:	h
	author:		liangairan
	
	purpose:	粒子旋转影响器
*********************************************************************/

#pragma once 

#include "ParticleAffector.h"

class EngineExport CRotationAffector : public CParticleAffector
{
public:
    CRotationAffector();
    virtual ~CRotationAffector();

    virtual void AffectParticle(CParticleSystem* pParticleSystem, float fTimeElapsed);

    virtual void InitParticle(CParticle* pParticle);

    virtual bool LoadFromXML(CMarkupSTL& xml);

    virtual bool SaveToXMLNode(CMarkupSTL& xml);

    virtual AIRString GetTypeStr()
    {
        return "Rotator";
    }

    float GetRotationSpeedRangeStart()
    {
        return m_fRotationSpeedRangeStart;
    }

    float GetRotationSpeedRangeEnd()
    {
        return m_fRotationSpeedRangeEnd;
    }

    float GetRotationRangeStart()
    {
        return m_fRotationRangeStart;
    }

    float GetRotationRangeEnd()
    {
        return m_fRotationRangeEnd;
    }

    void SetRotationSpeedRangeStart(float fSpeedRangeStart);

    void SetRotationSpeedRangeEnd(float fSpeedRangeEnd);

    void SetRotationRangeStart(float fRangeStart);

    void SetRotationRangeEnd(float fRangeEnd);
protected:
private:
    //以下单位全部是弧度
    /// Initial rotation speed of particles (range start)
    float m_fRotationSpeedRangeStart;
    /// Initial rotation speed of particles (range end)
    float m_fRotationSpeedRangeEnd;
    /// Initial rotation angle of particles (range start)
    float m_fRotationRangeStart;
    /// Initial rotation angle of particles (range end)
    float m_fRotationRangeEnd;
};

