// ***************************************************************
//  文件: CFullScaleSegmentAffector.h
//  日期: 2010-4-2010/04/26
//  作者: huangjunjie
//  说明: width and height scale segment affector
// ***************************************************************


#pragma once

#ifndef __CFullScaleSegmentAffector_H_
#define __CFullScaleSegmentAffector_H_

#include "ParticleAffector.h"
#include <list>

struct FullScaleSeg
{
	float   m_fWScale;		// 当前时刻宽度的大小 
	float	m_fHScale;		// 当前时刻高度的大小
	float	m_fTimeSeg;		// 当前时刻
};

typedef AIRList<FullScaleSeg>			LIST_FULL_SCALE_SEG;
typedef LIST_FULL_SCALE_SEG::iterator	LIST_FULL_SCALE_SEG_ITOR;

class EngineExport CFullScaleSegmentAffector  : public CParticleAffector
{
public:
	CFullScaleSegmentAffector() : m_fTimeLength(10.0f)
	{
		m_lstFullScaleSeg.clear();
	}

	virtual ~CFullScaleSegmentAffector()
	{
		m_lstFullScaleSeg.clear();
	}

	virtual void AffectParticle(CParticleSystem* pParticleSystem, float fTimeElapsed);

	virtual bool LoadFromXML(CMarkupSTL& xml);

	virtual bool SaveToXMLNode(CMarkupSTL& xml);

	virtual AIRString GetTypeStr()
	{
		return "FullScaleSegment";
	}

	LIST_FULL_SCALE_SEG* GetLstScaleSeg( void )
	{
		return &m_lstFullScaleSeg;
	}

	FullScaleSeg* GetScaleSeg( size_t id );

	float GetTimeLength( void )
	{
		return m_fTimeLength;
	}

	void SetTimeLength( float fTimeLength )
	{
		m_fTimeLength = fTimeLength;
	}

	FullScaleSeg* GetScaleSeg( float fTimeFlag, int & index );

	int InsertScaleSeg( FullScaleSeg& CS );

	void InsertScaleSeg( FullScaleSeg& CS, size_t id );

	int DeleteScaleSeg( FullScaleSeg& CS );

	void DeleteScaleSeg(size_t id );

protected:
	void SetParticleSize( CParticle* pPar );

	void CampScaleValue( float colIN1, float colIN2, float& colOUT, float percentage );


protected:
	LIST_FULL_SCALE_SEG	m_lstFullScaleSeg;	// 缩放段的容器
	float				m_fTimeLength;	// 影响器的时间长度


}; //! end class CFullScaleSegmentAffector


#endif //! end __CFullScaleSegmentAffector_H_