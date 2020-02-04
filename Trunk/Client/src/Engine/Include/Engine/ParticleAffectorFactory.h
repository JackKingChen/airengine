/********************************************************************
	created:	2009/12/22
	created:	22:12:2009   9:37
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\ParticleAffectorFactory.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	ParticleAffectorFactory
	file ext:	h
	author:		liangairan
	
	purpose:	����Ӱ��������
*********************************************************************/

#pragma once 

#include "ParticleAffector.h"
#include "Singleton.h"

class EngineExport CParticleAffectorFactory : public Singleton<CParticleAffectorFactory>
{
    BE_SINGLETON(CParticleAffectorFactory);
public:
    ~CParticleAffectorFactory();

	//����Ӱ����
    CParticleAffector* CreateAffector(const AIRString& strName, CMarkupSTL& xml);

};

#define PARTICLEAFFECTOR_FACTORY CParticleAffectorFactory::GetInstance()