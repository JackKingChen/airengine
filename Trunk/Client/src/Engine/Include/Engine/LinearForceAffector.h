/********************************************************************
	created:	2010/01/27
	created:	27:1:2010   11:51
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\LinearForceAffector.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	LinearForceAffector
	file ext:	h
	author:		liangairan
	
	purpose:	��Ӱ����
*********************************************************************/

#pragma once 


#include "ParticleAffector.h"

class EngineExport CLinearForceAffector : public CParticleAffector
{
public:
    enum ForceType
    {
        Force_Add,
        Force_Average,
    };
public:
    CLinearForceAffector();
    virtual ~CLinearForceAffector();

    virtual void AffectParticle(CParticleSystem* pParticleSystem, float fTimeElapsed);

    virtual bool LoadFromXML(CMarkupSTL& xml);

    virtual bool SaveToXMLNode(CMarkupSTL& xml);

    const Vector3Df& GetForceVector() const
    {
        return m_vecForce;
    }

    ForceType GetForceType()
    {
        return m_forceType;
    }

    void SetForceVector(const Vector3Df& vec);

    void SetForceType(ForceType type);

    virtual AIRString GetTypeStr()
    {
        return "LinearForceAffector";
    }
protected:
    Vector3Df  m_vecForce;   //Ӱ��ķ���
    ForceType  m_forceType;  
private:
};

