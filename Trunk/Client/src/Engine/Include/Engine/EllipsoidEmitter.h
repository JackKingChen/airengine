/********************************************************************
	created:	2010/01/24
	created:	24:1:2010   11:30
	filename: 	d:\My Codes\91wan\client\Current\Engine\Engine\Include\EllipsoidEmitter.h
	file path:	d:\My Codes\91wan\client\Current\Engine\Engine\Include
	file base:	EllipsoidEmitter
	file ext:	h
	author:		liangairan
	
	purpose:	Õ÷«ÚÃÂ∑¢…‰∆˜
*********************************************************************/

#include "AreaEmitter.h"

class EngineExport CEllipsoidEmitter : public CAreaEmitter
{
public:
	CEllipsoidEmitter();
	virtual ~CEllipsoidEmitter();

	virtual void InitParitcle(CParticle* pParticle);

protected:

    virtual AIRString GetType()
    {
        return "EllipsoidEmitter";
    }
private:
};
