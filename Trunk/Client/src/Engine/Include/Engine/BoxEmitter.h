/********************************************************************
	created:	2009/12/24
	created:	24:12:2009   10:17
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\BoxEmitter.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	BoxEmitter
	file ext:	h
	author:		liangairan
	
	purpose:	粒子立方体发射器
*********************************************************************/


#pragma once 
#include "AreaEmitter.h"

class EngineExport CBoxEmitter : public CAreaEmitter
{
public:
    CBoxEmitter();
    virtual ~CBoxEmitter();

    virtual void InitParitcle(CParticle* pParticle);

    virtual bool LoadFromXML(CMarkupSTL& xml);

    virtual bool SaveToXMLNode(CMarkupSTL& xml);

protected:
    virtual AIRString GetType()
    {
        return "BoxEmitter";
    }
private:
};
