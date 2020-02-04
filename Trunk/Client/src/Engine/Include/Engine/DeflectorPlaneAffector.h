/********************************************************************
	created:	2010/01/28
	created:	28:1:2010   18:30
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\DeflectorPlaneAffector.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	DeflectorPlaneAffector
	file ext:	h
	author:		liangairan
	
	purpose:	Ãæ·´µ¯Ó°ÏìÆ÷
*********************************************************************/

#pragma once 

#include "ParticleAffector.h"

class EngineExport CDeflectorPlaneAffector : public CParticleAffector
{
public:
    CDeflectorPlaneAffector();
    virtual ~CDeflectorPlaneAffector();

    virtual void AffectParticle(CParticleSystem* pParticleSystem, float fTimeElapsed);

    virtual bool LoadFromXML(CMarkupSTL& xml);

    virtual bool SaveToXMLNode(CMarkupSTL& xml);

    virtual const Vector3Df& GetPlanePoint() const
    {
        return m_PlanePoint;
    }

    virtual const Vector3Df& GetPlaneNormal() const
    {
        return m_vPlaneNormal;
    }

    float GetBounce()
    {
        return m_fBounce;
    }

    void SetBounce(float fBounce);

    virtual void SetPlanePoint(const Vector3Df& point);

    virtual void SetPlaneNormal(const Vector3Df& vNormal);
    
    virtual AIRString GetTypeStr()
    {
        return "DeflectorPlane";
    }

protected:
    /// deflector plane point
    Vector3Df m_PlanePoint;
    /// deflector plane normal vector
    Vector3Df m_vPlaneNormal;

    /// bounce factor (0.5 means 50 percent)
    float m_fBounce;
private:
};
