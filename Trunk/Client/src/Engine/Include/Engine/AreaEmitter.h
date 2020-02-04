/********************************************************************
	created:	2009/12/22
	created:	22:12:2009   17:40
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\AreaEmitter.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	AreaEmitter
	file ext:	h
	author:		liangairan
	
	purpose:	粒子区域发射器
*********************************************************************/

#pragma once 

#include "ParticleEmitter.h"

class EngineExport CAreaEmitter : public CParticleEmitter
{
public:
    CAreaEmitter();
    virtual ~CAreaEmitter();

    //virtual void InitParitcle(CParticle* pParticle);
    virtual void SetDirection(const Vector3Df& vDirection);

    //设置区域大小
    virtual void SetSize(const Vector3Df& vSize);

    virtual void SetSize(float fWidth, float fHeight, float fDepth);

    virtual void SetWidth(float fWidth);

    virtual void SetHeight(float fHeight);

    virtual void SetDepth(float fDepth);

    virtual float GetWidth()
    {
        return m_Size.x;
    }

    virtual float GetHeight()
    {
        return m_Size.y;
    }

    virtual float GetDepth()
    {
        return m_Size.z;
    }

    virtual bool LoadFromXML(CMarkupSTL& xml);

    virtual const Vector3Df& GetSize() const
    {
        return m_Size;
    }

    virtual bool SaveToXMLNode(CMarkupSTL& xml);

    virtual void Copy(CParticleEmitter* pEmitterClone);

protected:
    /// Size of the area
    Vector3Df m_Size;

    /// Local axes, not normalised, their magnitude reflects area size
    Vector3Df m_XRange, m_YRange, m_ZRange;

    //生成区域的轴
    void GenAreaAxis();
private:
};
