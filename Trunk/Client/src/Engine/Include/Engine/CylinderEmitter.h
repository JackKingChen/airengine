/********************************************************************
	created:	2010/01/23
	created:	23:1:2010   17:08
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\CCylinderEmitter.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	CCylinderEmitter
	file ext:	h
	author:		liangairan
	
	purpose:	Ô²ÖùÌå·¢ÉäÆ÷
*********************************************************************/

#include "AreaEmitter.h"

class EngineExport CCylinderEmitter : public CAreaEmitter
{
public:
    CCylinderEmitter();
    virtual ~CCylinderEmitter();

    virtual void InitParitcle(CParticle* pParticle);

protected:
    virtual AIRString GetType()
    {
        return "CylinderEmitter";
    }
private:
};
