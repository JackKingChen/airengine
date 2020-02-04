
#include "stdafx.h"
#include "PointAttractAffector.h"
#include "StringUtil.h"
#include "ParticleIterator.h"
#include "ParticleSystem.h"
#include "Line3D.h"
#include "Quaternion.h"

void CPointAttractAffector::AffectParticle(CParticleSystem* pParticleSystem, float fTimeElapsed)
{
	Vector3Df vPoint = pParticleSystem->GetWorldPosition() + m_vAttractPos;
	// ���㵱ǰ֡�����ӵ�����Ӱ��
	float fAffectStrength = m_fStrength * fTimeElapsed;
	Vector3Df vTrans;
	CParticleIterator particleIter = pParticleSystem->GetIterator();
	CParticle* pParticle = NULL;
	float fLen = 0.0f;
	while (!particleIter.IsEnd())
	{
		pParticle = particleIter.GetNext();
		// �������ӵ������ӵ�����		
		vTrans = vPoint - pParticle->m_position;

		fLen = vTrans.GetLength();
		vTrans.Normalize();
		// �����ٶ�����
		vTrans *= fAffectStrength;
		// �޸����ӵ��˶������Լ��ٶ�
		pParticle->m_direction *= m_fExpAttenuate;
		pParticle->m_direction += vTrans;
		//if ( fLen <= m_fEARadius )
		//{
		//	pParticle->m_direction *= m_fExpAttenuate;
		//}
	}
}

bool CPointAttractAffector::LoadFromXML(CMarkupSTL& xml)
{
	AIRString strValue = xml.GetAttrib("attract_position").c_str();
	if ( strValue.length() > 0 )
	{
		sscanf(strValue.c_str(), "(%f,%f,%f)", &(m_vAttractPos.x), &(m_vAttractPos.y), &(m_vAttractPos.z));

	}

	strValue = xml.GetAttrib("strength").c_str();
	if ( strValue.length() > 0 )
	{
		m_fStrength = atof( strValue.c_str() );
	}

	strValue = xml.GetAttrib("exp_attenuate").c_str();
	if ( strValue.length() > 0 )
	{
		m_fExpAttenuate = atof( strValue.c_str() );
	}

	strValue = xml.GetAttrib("ea_radius").c_str();
	if ( strValue.length() > 0 )
	{
		m_fEARadius = atof( strValue.c_str() );
	}

	return true;
}

bool CPointAttractAffector::SaveToXMLNode(CMarkupSTL& xml)
{
	xml.AddAttrib("type", "PointAttract");
	char szTmp[256] = { 0 };

	snprintf(szTmp, 255, "(%f, %f, %f)", m_vAttractPos.x, m_vAttractPos.y, m_vAttractPos.z);
	xml.AddAttrib("attract_position", szTmp);

	snprintf(szTmp, 255, "%f", m_fStrength);
	xml.AddAttrib("strength", szTmp);

	snprintf(szTmp, 255, "%f", m_fExpAttenuate);
	xml.AddAttrib("exp_attenuate", szTmp);

	snprintf(szTmp, 255, "%f", m_fEARadius);
	xml.AddAttrib("ea_radius", szTmp);

	return true;
}
