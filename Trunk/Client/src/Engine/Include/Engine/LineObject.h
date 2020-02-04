/********************************************************************
	created:	16:2:2011   18:04
	filename: 	LineObject.h
	author:		liangairan
	
	purpose:	œﬂ‰÷»æÃÂ
*********************************************************************/

#pragma once

#include "Renderable.h"

class EngineExport LineObject : public Renderable
{
public:
    LineObject();
    virtual ~LineObject();
protected:
    virtual bool Initialize();
private:
    static IMaterial* m_pLineMaterial;
};
