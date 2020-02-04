// ***************************************************************
//  文件: CAlphaSegmentAffector.h
//  日期: 2010-4-2010/04/28
//  作者: huangjunjie
//  说明: only control the alpha channel with the time
// ***************************************************************


#pragma once

#ifndef __AlphaSegmentAffector_H_
#define __AlphaSegmentAffector_H_

#include "ParticleAffector.h"
#include <list>

struct AlphaSeg
{
	float			m_fAlpha;		// 当前时刻的alpha
	float			m_fTimeSeg;		// 当前时刻
};

typedef AIRList<AlphaSeg> LIST_ALPHA_SEG;

class EngineExport CAlphaSegmentAffector : public CParticleAffector
{
public:
	CAlphaSegmentAffector() : m_fTimeLength(10.0f)
	{
		m_lstAlphaSeg.clear();
	}

    virtual ~CAlphaSegmentAffector()
	{
		m_lstAlphaSeg.clear();
	}

	virtual void AffectParticle(CParticleSystem* pParticleSystem, float fTimeElapsed);

	virtual bool LoadFromXML(CMarkupSTL& xml);

	virtual bool SaveToXMLNode(CMarkupSTL& xml);

	virtual AIRString GetTypeStr()
	{
		return "AlphaSegment";
	}

	LIST_ALPHA_SEG* GetLstAlphaSeg( void )
	{
		return &m_lstAlphaSeg;
	}

	AlphaSeg* GetAlphaSeg( int id )
	{
		if ( id >= m_lstAlphaSeg.size() )
		{
			return NULL;
		}
		LIST_ALPHA_SEG::iterator it = m_lstAlphaSeg.begin();
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

	AlphaSeg* GetAlphaSeg( float fTimeFlag, int & index );

	int InsertAlphaSeg( AlphaSeg& CS );

	void InsertAlphaSeg( AlphaSeg& CS, int id );

	int DeleteAlphaSeg( AlphaSeg& CS );

	void DeleteAlphaSeg( int id );

protected:
	void SetParticleAlphaValue( CParticle* pPar );

	void CampAlphaValue( float colIN1, float colIN2, float& colOUT, float percentage );



protected:
	LIST_ALPHA_SEG	m_lstAlphaSeg;	// 颜色影响段的容器
	float			m_fTimeLength;	// 影响器的时间长度

}; //! end class CAlphaSegmentAffector


#endif //! end __AlphaSegmentAffector_H_