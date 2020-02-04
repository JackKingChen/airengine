/********************************************************************
	created:	2010/01/23
	created:	23:1:2010   17:37
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\RingEmitter.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	RingEmitter
	file ext:	h
	author:		liangairan
	
	purpose:	»·ĞÎ·¢ÉäÆ÷
*********************************************************************/

#include "AreaEmitter.h"

class EngineExport CRingEmitter : public CAreaEmitter
{
public:
    CRingEmitter();
    virtual ~CRingEmitter();

    virtual void InitParitcle(CParticle* pParticle);

    float GetInnerSizeX()
    {
        return m_fInnerSizeX;
    }

    float GetInnerSizeY()
    {
        return m_fInnerSizeY;
    }

    void SetInnerSizeX(float fInnerSizeX);

    void SetInnerSizeY(float fInnerSizeY);

    virtual bool LoadFromXML(CMarkupSTL& xml);

    virtual bool SaveToXMLNode(CMarkupSTL& xml);

    virtual void Copy(CParticleEmitter* pEmitterClone);

protected:
    //Size of 'clear' center area (> 0 and < 1.0)
    float m_fInnerSizeX;
    float m_fInnerSizeY;

    virtual AIRString GetType()
    {
        return "RingEmitter";
    }
private:
};