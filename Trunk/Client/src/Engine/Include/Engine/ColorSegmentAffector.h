// ***************************************************************
//  文件: CColorSegmentAffector.h
//  日期: 2010-3-2010/03/24
//  作者: huangjunjie
//  说明: 分段颜色影响器
// ***************************************************************


#pragma once

#ifndef __CColorSegmentAffector_H_
#define __CColorSegmentAffector_H_

#include "ParticleAffector.h"
#include <list>

struct ColorSeg
{
	CColorValue		m_Color;		// 当前时刻的颜色
	float			m_fTimeSeg;		// 当前时刻
};

typedef AIRList<ColorSeg> LIST_COLOR_SEG;

class EngineExport CColorSegmentAffector : public CParticleAffector
{
public:
	CColorSegmentAffector() : m_fTimeLength(10.0f)
	{
		m_vtColorSeg.clear();
	}

    virtual ~CColorSegmentAffector()
	{
		m_vtColorSeg.clear();
	}

	virtual void AffectParticle(CParticleSystem* pParticleSystem, float fTimeElapsed);

	virtual bool LoadFromXML(CMarkupSTL& xml);

	virtual bool SaveToXMLNode(CMarkupSTL& xml);

	virtual AIRString GetTypeStr()
	{
		return "ColorSegment";
	}

	LIST_COLOR_SEG* GetVecColorSeg( void )
	{
		return &m_vtColorSeg;
	}

	ColorSeg* GetColorSeg( int id )
	{
		if ( id >= m_vtColorSeg.size() )
		{
			return NULL;
		}
		LIST_COLOR_SEG::iterator it = m_vtColorSeg.begin();
		std::advance( it, id );
		return &(*it);
	}

	float GetTimeLength( void )
	{
		return m_fTimeLength;
	}

	void SetTimeLength( float fTimeLength )
	{
		m_fTimeLength = fTimeLength;
	}

	ColorSeg* GetColorSeg( float fTimeFlag, int & index );

	int InsertColorSeg( ColorSeg& CS );

	void InsertColorSeg( ColorSeg& CS, int id );

	int DeleteColorSeg( ColorSeg& CS );

	void DeleteColorSeg( int id );

protected:
	void SetParticleColorValue( CParticle* pPar );

	void CampColorValue( CColorValue& colIN1, CColorValue& colIN2, CColorValue& colOUT, float percentage );


protected:
	LIST_COLOR_SEG	m_vtColorSeg;	// 颜色影响段的容器
	float			m_fTimeLength;	// 影响器的时间长度


}; //! end class CColorSegmentAffector


#endif //! end __CColorSegmentAffector_H_