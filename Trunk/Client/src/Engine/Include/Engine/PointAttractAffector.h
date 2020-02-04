// ***************************************************************
//  文件: CPointAttractAffector.h
//  日期: 2010-3-2010/03/23
//  作者: huangjunjie
//  说明: 点状吸引子
// ***************************************************************


#pragma once

#ifndef __CPointAttractAffector_H_
#define __CPointAttractAffector_H_

#include "ParticleAffector.h"

class EngineExport CPointAttractAffector : public CParticleAffector
{
public:
	CPointAttractAffector() : m_vAttractPos(0.0f, 0.0f, 0.0f), m_fStrength(10.0f), m_fExpAttenuate(0.0f), m_fEARadius(10.0f)
	{
		
	}

    virtual ~CPointAttractAffector()
	{

	}

	void SetAttractPos( const Vector3Df& pos )
	{
		m_vAttractPos = pos;
	}

	const Vector3Df& GetAttractPos( void ) const
	{
		return m_vAttractPos;
	}

	void SetStrength( float fStrength )
	{
		m_fStrength = fStrength;
	}

	float GetStrength( void )
	{
		return m_fStrength;
	}

	void SetExpAttenuate( float fEA )
	{
		m_fExpAttenuate = fEA;
	}

	float GetExpAttenuate( void )
	{
		return m_fExpAttenuate;
	}

	void SetEARadius( float fEARadius )
	{
		m_fEARadius = fEARadius;
	}

	float GetEARadius( void )
	{
		return m_fEARadius;
	}


	virtual void AffectParticle(CParticleSystem* pParticleSystem, float fTimeElapsed);

	virtual bool LoadFromXML(CMarkupSTL& xml);

	virtual bool SaveToXMLNode(CMarkupSTL& xml);

	virtual AIRString GetTypeStr()
	{
		return "PointAttract";
	}

protected:
	Vector3Df	m_vAttractPos;	// 吸引子的位置
	float		m_fStrength;	// 影响的力度
	float		m_fExpAttenuate;// 增益/衰减系数(影响粒子到达吸引子后的速度)
	float		m_fEARadius;	// 增益/衰减的影响范围

}; //! end class CPointAttractAffector


#endif //! end __CPointAttractAffector_H_