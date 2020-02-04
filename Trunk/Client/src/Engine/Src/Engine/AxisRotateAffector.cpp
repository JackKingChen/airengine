#include "stdafx.h"
#include "AxisRotateAffector.h"
#include "StringUtil.h"
#include "ParticleIterator.h"
#include "ParticleSystem.h"
#include "Line3D.h"
#include "Quaternion.h"

CAxisRotateAffector :: CAxisRotateAffector() :
m_vAxisDir( 0.0f, 1.0f, 0.0f ),
m_vAxisPos( 0.0f, 0.0f, 0.0f ),
m_bIsClockwise( false ),
m_bIsAttenuateFromInToOut( false ),
m_fAffectRadius( 10.0f ),
m_fAffectMin( 0.0f ),
m_fAffectMax( 1.0f ),
m_fAttenuateRateByTimeElapse( 10.0f ),
m_fInitRotationalSpeed( 30.0f )
{
	m_Axis.SetLineByPosAndDir( m_vAxisPos, m_vAxisDir );
	m_fRotateSpeedMin = m_fInitRotationalSpeed * m_fAffectMin;
	m_fRotateSpeedMax = m_fInitRotationalSpeed * m_fAffectMax;
}


CAxisRotateAffector :: ~CAxisRotateAffector()
{

}

void CAxisRotateAffector :: AffectParticle(CParticleSystem* pParticleSystem, float fTimeElapsed)
{
	CParticleIterator particleIter = pParticleSystem->GetIterator();
	CParticle* pParticle = NULL;
	while (!particleIter.IsEnd())
	{
		// ������Ӷ���
		pParticle = particleIter.GetNext();
		// �жϵ�ǰ����������ת�Ľ��ٶ�
		if ( pParticle->m_fAxisRotateSpeed > 0.0f )
		{
			// ���㵱ǰ֡����������ת�ĽǶ�
			float fRotateAngle = pParticle->m_fAxisRotateSpeed * fTimeElapsed;
			if ( !m_bIsClockwise )
			{
				fRotateAngle = -fRotateAngle;
			}
			// ������ת��Ԫ��
			Quaternion q;
			q.FromAngleAxis( DegreeToRadius(fRotateAngle), m_vAxisDir );
			// �������ӵ�λ��
			pParticle->m_position = q * (pParticle->m_position - pParticleSystem->GetWorldPosition()) + pParticleSystem->GetWorldPosition();
			// �������ӵ��˶�����
			pParticle->m_direction = q * pParticle->m_direction;
			// �������ӵ�������ת���ٶ�
			pParticle->m_fAxisRotateSpeed -= fTimeElapsed * m_fAttenuateRateByTimeElapse;
			if ( pParticle->m_fAxisRotateSpeed < 0.0f )
			{
				pParticle->m_fAxisRotateSpeed = 0.0f;
			}
		}
	}
}

bool CAxisRotateAffector :: LoadFromXML(CMarkupSTL& xml)
{
	AIRString strValue = xml.GetAttrib("axis_direction").c_str();
	if ( strValue.length() > 0 )
	{
		sscanf(strValue.c_str(), "(%f, %f, %f)", &m_vAxisDir.x, &m_vAxisDir.y, &m_vAxisDir.z);
	}

	strValue = xml.GetAttrib("axis_position").c_str();
	if ( strValue.length() > 0 )
	{
		sscanf(strValue.c_str(), "(%f, %f, %f)", &m_vAxisPos.x, &m_vAxisPos.y, &m_vAxisPos.z);
	}

	strValue = xml.GetAttrib("rotate_clockwise").c_str();
	if ( strValue.length() > 0 )
	{
		CStringUtil::ToUpperCase( strValue );
		if ( strValue == "TRUE" )
		{
			m_bIsClockwise = true;
		}
		else
		{
			m_bIsClockwise = false;
		}
	}

	strValue = xml.GetAttrib("attenuate_from_in_to_out").c_str();
	if ( strValue.length() > 0 )
	{
		CStringUtil::ToUpperCase( strValue );
		if ( strValue == "TRUE" )
		{
			m_bIsAttenuateFromInToOut = true;
		}
		else
		{
			m_bIsAttenuateFromInToOut = false;
		}
	}

	strValue = xml.GetAttrib("affect_radius").c_str();
	if ( strValue.length() > 0 )
	{
		m_fAffectRadius = atof(strValue.c_str());
	}

	strValue = xml.GetAttrib("affect_min").c_str();
	if ( strValue.length() > 0 )
	{
		m_fAffectMin = atof(strValue.c_str());
	}

	strValue = xml.GetAttrib("affect_max").c_str();
	if ( strValue.length() > 0 )
	{
		m_fAffectMax = atof(strValue.c_str());
	}

	strValue = xml.GetAttrib("attenuate_rate_by_time_elapse").c_str();
	if ( strValue.length() > 0 )
	{
		m_fAttenuateRateByTimeElapse = atof(strValue.c_str());
	}

	strValue = xml.GetAttrib("init_rotate_speed").c_str();
	if ( strValue.length() > 0 )
	{
		m_fInitRotationalSpeed = atof(strValue.c_str());
	}

	m_Axis.SetLineByPosAndDir( m_vAxisPos, m_vAxisDir );
	m_fRotateSpeedMin = m_fInitRotationalSpeed * m_fAffectMin;
	m_fRotateSpeedMax = m_fInitRotationalSpeed * m_fAffectMax;

	return true;
}

bool CAxisRotateAffector :: SaveToXMLNode(CMarkupSTL& xml)
{
	xml.AddAttrib("type", "AxisRotator");
	char szTmp[256] = { 0 };

	snprintf(szTmp, 255, "(%f, %f, %f)", m_vAxisDir.x, m_vAxisDir.y, m_vAxisDir.z);
	xml.AddAttrib("axis_direction", szTmp);

	snprintf(szTmp, 255, "(%f, %f, %f)", m_vAxisPos.x, m_vAxisPos.y, m_vAxisPos.z);
	xml.AddAttrib("axis_position", szTmp);

	if ( m_bIsClockwise )
	{
		xml.AddAttrib( "rotate_clockwise", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "rotate_clockwise", "FALSE" );
	}

	if ( m_bIsAttenuateFromInToOut )
	{
		xml.AddAttrib( "attenuate_from_in_to_out", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "attenuate_from_in_to_out", "FALSE" );
	}

	snprintf(szTmp, 255, "%f", m_fAffectRadius);
	xml.AddAttrib("affect_radius", szTmp);

	snprintf(szTmp, 255, "%f", m_fAffectMin);
	xml.AddAttrib("affect_min", szTmp);

	snprintf(szTmp, 255, "%f", m_fAffectMax);
	xml.AddAttrib("affect_max", szTmp);

	snprintf(szTmp, 255, "%f", m_fAttenuateRateByTimeElapse);
	xml.AddAttrib("attenuate_rate_by_time_elapse", szTmp);

	snprintf(szTmp, 255, "%f", m_fInitRotationalSpeed);
	xml.AddAttrib("init_rotate_speed", szTmp);

	return true;
}

AIRString CAxisRotateAffector :: GetTypeStr()
{
	return "AxisRotator";
}

void CAxisRotateAffector :: RefreshAffectorParam( void )
{
	if ( m_pPS )
	{
		m_Axis.SetLineByPosAndDir( m_vAxisPos + m_pPS->GetWorldPosition(), m_vAxisDir );
	}
}

void CAxisRotateAffector :: InitParticle( CParticle* pParticel )
{
	if ( pParticel )
	{
		// �������ӵ���ת��ľ���
		float fDistP2A = m_Axis.GetDistanceFromPointToLine( pParticel->m_position );
		// �����ʼ��ת���ٶ�����������˥��
		if ( m_bIsAttenuateFromInToOut )
		{
			// ������ӵ���ת��ľ�����ڵ���Ӱ��뾶
			if ( fDistP2A >= m_fAffectRadius )
			{
				// �������ӵ���ת���ٶ�Ϊ��ʼ���ٶȵ���Сֵ
				pParticel->m_fAxisRotateSpeed = m_fRotateSpeedMin;
			}
			else
			{
				// ����������ӵ���ת��ľ���������ʼ��ת���ٶ�
				pParticel->m_fAxisRotateSpeed = (1.0 - fDistP2A / m_fAffectRadius) * (m_fRotateSpeedMax - m_fRotateSpeedMin) + m_fRotateSpeedMin;
			}
		}
		// �����ʼ��ת���ٶ��������������
		else
		{
			// ������ӵ���ת��ľ�����ڵ���Ӱ��뾶
			if ( fDistP2A >= m_fAffectRadius )
			{
				// �������ӵ���ת���ٶ�Ϊ��ʼ���ٶȵ����ֵ
				pParticel->m_fAxisRotateSpeed = m_fRotateSpeedMax;
			}
			else
			{
				// ����������ӵ���ת��ľ���������ʼ��ת���ٶ�
				pParticel->m_fAxisRotateSpeed = (fDistP2A / m_fAffectRadius) * (m_fRotateSpeedMax - m_fRotateSpeedMin) + m_fRotateSpeedMin;
			}
		}
	}
}

void CAxisRotateAffector :: SetAxisDir( const AIR::Vector3Df& vDir )
{
	m_vAxisDir = vDir;
	m_Axis.SetLineByPosAndDir( m_vAxisPos, m_vAxisDir );
}

void CAxisRotateAffector :: SetAxisPos( const AIR::Vector3Df& vPos )
{
	m_vAxisPos = vPos;
	m_Axis.SetLineByPosAndDir( m_vAxisPos, m_vAxisDir );
}

void CAxisRotateAffector :: SetIsClockwise( bool b )
{
	m_bIsClockwise = b;
}

void CAxisRotateAffector :: SetIsAttenuateFromInToOut( bool b )
{
	m_bIsAttenuateFromInToOut = b;
}

void CAxisRotateAffector :: SetAffectRadius( float fAffectRadius )
{
	m_fAffectRadius = fAffectRadius;
}

void CAxisRotateAffector :: SetAffectMin( float fMin )
{
	m_fAffectMin = fMin;
	m_fRotateSpeedMin = m_fInitRotationalSpeed * m_fAffectMin;
	m_fRotateSpeedMax = m_fInitRotationalSpeed * m_fAffectMax;
}

void CAxisRotateAffector :: SetAffectMax( float fMax )
{
	m_fAffectMax = fMax;
	m_fRotateSpeedMin = m_fInitRotationalSpeed * m_fAffectMin;
	m_fRotateSpeedMax = m_fInitRotationalSpeed * m_fAffectMax;
}

void CAxisRotateAffector :: SetAttenuateRateByTimeElapse( float fAttenuateRateByTimeElapse )
{
	m_fAttenuateRateByTimeElapse = fAttenuateRateByTimeElapse;
}

void CAxisRotateAffector :: SetInitRotationalSpeed( float fInitRotationalSpeed )
{
	m_fInitRotationalSpeed = fInitRotationalSpeed;
	m_fRotateSpeedMin = m_fInitRotationalSpeed * m_fAffectMin;
	m_fRotateSpeedMax = m_fInitRotationalSpeed * m_fAffectMax;
}

const AIR::Vector3Df& CAxisRotateAffector :: GetAxisDir( )
{
	return m_vAxisDir;
}

const AIR::Vector3Df& CAxisRotateAffector :: GetAxisPos( )
{
	return m_vAxisPos;
}

bool CAxisRotateAffector :: GetIsClockwise( )
{
	return m_bIsClockwise;
}

bool CAxisRotateAffector :: GetIsAttenuateFromInToOut( )
{
	return m_bIsAttenuateFromInToOut;
}

float CAxisRotateAffector :: GetAffectRadius( )
{
	return m_fAffectRadius;
}

float CAxisRotateAffector :: GetAffectMin( )
{
	return m_fAffectMin;
}

float CAxisRotateAffector :: GetAffectMax( )
{
	return m_fAffectMax;
}

float CAxisRotateAffector :: GetAttenuateRateByTimeElapse( )
{
	return m_fAttenuateRateByTimeElapse;
}

float CAxisRotateAffector :: GetInitRotationalSpeed( )
{
	return m_fInitRotationalSpeed;
}