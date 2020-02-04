// ***************************************************************
//  文件: CAxisRotateAffector.h
//  日期: 2010-3-2010/03/17
//  作者: huangjunjie
//  说明: 绕某条轴旋转的影响器
// ***************************************************************


#pragma once

#ifndef __CAxisRotateAffector_H_
#define __CAxisRotateAffector_H_

#include "ParticleAffector.h"
#include "Line3D.h"
#include "ParticleSystem.h"

class EngineExport CAxisRotateAffector : public CParticleAffector
{
public:
    CAxisRotateAffector();
    virtual ~CAxisRotateAffector();

	virtual void AffectParticle(CParticleSystem* pParticleSystem, float fTimeElapsed);

	virtual bool LoadFromXML(CMarkupSTL& xml);

	virtual bool SaveToXMLNode(CMarkupSTL& xml);

	virtual AIRString GetTypeStr();

	virtual void InitParticle( CParticle* pParticel );

	virtual void RefreshAffectorParam( void );

	void SetAxisDir( const AIR::Vector3Df& vDir );

	void SetAxisPos( const AIR::Vector3Df& vPos );

	void SetIsClockwise( bool b );

	void SetIsAttenuateFromInToOut( bool b );

	void SetAffectRadius( float fAffectRadius );

	void SetAffectMin( float fMin );

	void SetAffectMax( float fMax );

	void SetAttenuateRateByTimeElapse( float fAttenuateRateByTimeElapse );

	void SetInitRotationalSpeed( float fInitRotationalSpeed );

	const Vector3Df& GetAxisDir( );

	const Vector3Df& GetAxisPos( );

	bool GetIsClockwise( );

	bool GetIsAttenuateFromInToOut( );

	float GetAffectRadius( );

	float GetAffectMin( );

	float GetAffectMax( );

	float GetAttenuateRateByTimeElapse( );

	float GetInitRotationalSpeed( );


protected:

	Vector3Df		m_vAxisDir;						// 旋转轴的方向
	Vector3Df		m_vAxisPos;						// 旋转轴上某个点的坐标
	bool			m_bIsClockwise;					// 例子是否绕轴顺时针旋转
	bool			m_bIsAttenuateFromInToOut;		// 旋转角速度是否由内向外衰减
	float			m_fAffectRadius;				// 绕轴旋转的影响半径
	float			m_fAffectMin;					// 旋转半径上的角速度最小影响系数(影响粒子生成时的角速度) (0.0f-1.0f)
	float			m_fAffectMax;					// 旋转半径上的角速度的最大影响系数(影响粒子生成时的角速度) (0.0f - 1.0f)
	float			m_fAttenuateRateByTimeElapse;	// 粒子随时间流逝的角速度衰减系数(影响粒子运行过程中的角速度)
	float			m_fInitRotationalSpeed;			// 初始旋转角速度 (单位:度/秒)

	Line3Df			m_Axis;
	float			m_fRotateSpeedMin;
	float			m_fRotateSpeedMax;


}; //! end class CAxisRotateAffector


#endif //! end __CAxisRotateAffector_H_