// ***************************************************************
//  文件: ScaleSegmentAffector.h
//  日期: 2010-3-2010/03/25
//  作者: huangjunjie
//  说明: 
// ***************************************************************


#pragma once

#ifndef __ScaleSegmentAffector_H_
#define __ScaleSegmentAffector_H_


#include "ParticleAffector.h"
#include <list>

struct ScaleSeg
{
	float	m_fScale;		// 当前时刻的大小
	float	m_fTimeSeg;		// 当前时刻
};

typedef AIRList<ScaleSeg> LIST_SCALE_SEG;

class EngineExport CScaleSegmentAffector : public CParticleAffector
{
public:
	CScaleSegmentAffector() : m_fTimeLength(10.0f)
	{
		m_lstScaleSeg.clear();
	}

	virtual ~CScaleSegmentAffector()
	{
		m_lstScaleSeg.clear();
	}

	virtual void AffectParticle(CParticleSystem* pParticleSystem, float fTimeElapsed);

	virtual bool LoadFromXML(CMarkupSTL& xml);

	virtual bool SaveToXMLNode(CMarkupSTL& xml);

	virtual AIRString GetTypeStr()
	{
		return "ScaleSegment";
	}

	LIST_SCALE_SEG* GetLstScaleSeg( void )
	{
		return &m_lstScaleSeg;
	}

	ScaleSeg* GetScaleSeg( size_t id );

	float GetTimeLength( void )
	{
		return m_fTimeLength;
	}

	void SetTimeLength( float fTimeLength )
	{
		m_fTimeLength = fTimeLength;
	}

	ScaleSeg* GetScaleSeg( float fTimeFlag, int & index );

	int InsertScaleSeg( ScaleSeg& CS );

	void InsertScaleSeg( ScaleSeg& CS, int id );

	int DeleteScaleSeg( ScaleSeg& CS );

	void DeleteScaleSeg( int id );

protected:
	void SetParticleSize( CParticle* pPar );

	void CampScaleValue( float colIN1, float colIN2, float& colOUT, float percentage );


protected:
	LIST_SCALE_SEG	m_lstScaleSeg;	// 缩放段的容器
	float			m_fTimeLength;	// 影响器的时间长度


}; //! end class CScaleSegmentAffector



#endif //! end __ScaleSegmentAffector_H_