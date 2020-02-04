/********************************************************************
	created:	2009/12/19
	created:	19:12:2009   11:51
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\PointEmitter.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	PointEmitter
	file ext:	h
	author:		liangairan
	
	purpose:	粒子系统点发射器
*********************************************************************/

#pragma once 

#include "ParticleEmitter.h"

class EngineExport CPointEmitter : public CParticleEmitter
{
public:
    CPointEmitter();
    virtual ~CPointEmitter();

    virtual void InitParitcle(CParticle* pParticle);

    virtual bool SaveToXMLNode(CMarkupSTL& xml); 

protected:
    virtual AIRString GetType()
    {
        return "PointEmitter";
    }
    
private:
};
