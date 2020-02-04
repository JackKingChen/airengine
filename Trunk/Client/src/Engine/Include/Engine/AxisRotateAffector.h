// ***************************************************************
//  �ļ�: CAxisRotateAffector.h
//  ����: 2010-3-2010/03/17
//  ����: huangjunjie
//  ˵��: ��ĳ������ת��Ӱ����
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

	Vector3Df		m_vAxisDir;						// ��ת��ķ���
	Vector3Df		m_vAxisPos;						// ��ת����ĳ���������
	bool			m_bIsClockwise;					// �����Ƿ�����˳ʱ����ת
	bool			m_bIsAttenuateFromInToOut;		// ��ת���ٶ��Ƿ���������˥��
	float			m_fAffectRadius;				// ������ת��Ӱ��뾶
	float			m_fAffectMin;					// ��ת�뾶�ϵĽ��ٶ���СӰ��ϵ��(Ӱ����������ʱ�Ľ��ٶ�) (0.0f-1.0f)
	float			m_fAffectMax;					// ��ת�뾶�ϵĽ��ٶȵ����Ӱ��ϵ��(Ӱ����������ʱ�Ľ��ٶ�) (0.0f - 1.0f)
	float			m_fAttenuateRateByTimeElapse;	// ������ʱ�����ŵĽ��ٶ�˥��ϵ��(Ӱ���������й����еĽ��ٶ�)
	float			m_fInitRotationalSpeed;			// ��ʼ��ת���ٶ� (��λ:��/��)

	Line3Df			m_Axis;
	float			m_fRotateSpeedMin;
	float			m_fRotateSpeedMax;


}; //! end class CAxisRotateAffector


#endif //! end __CAxisRotateAffector_H_