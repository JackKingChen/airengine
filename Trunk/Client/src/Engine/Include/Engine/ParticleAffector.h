/********************************************************************
	created:	2009/12/21
	created:	21:12:2009   19:21
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\ParticleAffector.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	ParticleAffector
	file ext:	h
	author:		liangairan
	
	purpose:	粒子影响器
*********************************************************************/

#pragma once 

#include "EngineConfig.h"
#include "Particle.h"
#include "MarkupSTL.h"

class CParticleSystem;

//enum eAffectorType
//{
//    AT_Scalor,
//    AT_Rotator,
//    AT_Color,
//    AT_ColorImage,
//};

class EngineExport CParticleAffector //: public EngineAllocator(CParticleAffector)
{
	EngineAllocator(CParticleAffector)
public:
    CParticleAffector() { m_pPS = NULL; }
    virtual ~CParticleAffector() { m_pPS = NULL; }

    virtual void            AffectParticle(CParticleSystem* pParticleSystem, float fTimeElapsed) = 0;

	virtual void			SetParticleSystem( CParticleSystem* pPS ) {	m_pPS = pPS; }

    virtual bool            LoadFromXML(CMarkupSTL& xml) = 0;

    virtual void            InitParticle(CParticle* pParticle) {}

    //virtual eAffectorType   GetAffectorType() { return m_type; }

    //保存到XML结点
    virtual bool            SaveToXMLNode(CMarkupSTL& xml) = 0;

    //获得返回类型
    virtual AIRString		GetTypeStr() = 0;

	virtual void			RefreshAffectorParam( void ) {};

protected:
    //eAffectorType			m_type;
	CParticleSystem*		m_pPS;
private:
};

