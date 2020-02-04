#include "stdafx.h"
#include "AlphaSegmentAffector.h"
#include "StringUtil.h"
#include "ParticleIterator.h"
#include "ParticleSystem.h"
#include "Line3D.h"
#include "Quaternion.h"



void CAlphaSegmentAffector :: AffectParticle(CParticleSystem* pParticleSystem, float fTimeElapsed)
{
	CParticleIterator particleIter = pParticleSystem->GetIterator();
	CParticle* pParticle = NULL;
	while (!particleIter.IsEnd())
	{
		pParticle = particleIter.GetNext();
		SetParticleAlphaValue( pParticle );
	}
}

bool CAlphaSegmentAffector :: LoadFromXML(CMarkupSTL& xml)
{
	AIRString strValue = xml.GetAttrib("time_length").c_str();
	if ( strValue.length() > 0 )
	{
		m_fTimeLength = atof( strValue.c_str() );
	}

	xml.IntoElem();
	AlphaSeg cs;
	while ( xml.FindElem("time_segment") )
	{
		strValue = xml.GetAttrib("alpha").c_str();
		if ( strValue.length() > 0 )
		{
			cs.m_fAlpha = atof( strValue.c_str() );
		}

		strValue = xml.GetAttrib("time_flag").c_str();
		if ( strValue.length() > 0 )
		{
			cs.m_fTimeSeg = atof( strValue.c_str() );
		}

		m_lstAlphaSeg.push_back( cs );
	}
	xml.OutOfElem();

	return true;
}

bool CAlphaSegmentAffector :: SaveToXMLNode(CMarkupSTL& xml)
{
	xml.AddAttrib("type", "AlphaSegment");
	char szTmp[256] = { 0 };

	snprintf(szTmp, 255, "%f", m_fTimeLength);
	xml.AddAttrib("time_length", szTmp);

	xml.IntoElem();

	LIST_ALPHA_SEG::iterator it = m_lstAlphaSeg.begin();
	for ( ; it != m_lstAlphaSeg.end(); ++it )
	{
		xml.AddElem("time_segment");

		snprintf(szTmp, 255, "%f", (*it).m_fAlpha );
		xml.AddAttrib( "alpha", szTmp );

		snprintf(szTmp, 255, "%f", (*it).m_fTimeSeg);
		xml.AddAttrib( "time_flag", szTmp );
	}

	xml.OutOfElem();

	return true;
}


void CAlphaSegmentAffector :: SetParticleAlphaValue( CParticle* pPar )
{
	if ( m_lstAlphaSeg.size() == 0 )
	{
		return;
	}
	int i = 0;
	float fParticleLife = pPar->m_fTotalTimeToLive - pPar->m_fLifeTime;
	LIST_ALPHA_SEG::iterator it = m_lstAlphaSeg.begin(), itEnd = m_lstAlphaSeg.begin(), itBefore = it;
	for ( ; it != m_lstAlphaSeg.end(); ++it, ++i )
	{
		if ( fParticleLife <= (*it).m_fTimeSeg )
		{
			break;
		}
	}
	// 到了最后一个颜色段
	if ( m_lstAlphaSeg.size() == 1  || i == 0 )
	{
		it = m_lstAlphaSeg.begin();
		pPar->m_color.a = (*it).m_fAlpha;
	}
	else if ( it == m_lstAlphaSeg.end() )
	{
		--it;
		pPar->m_color.a = (*it).m_fAlpha;
	}
	else
	{
		std::advance( itBefore, i-1 );
		float percentage = ( fParticleLife - (*itBefore).m_fTimeSeg ) / ( (*it).m_fTimeSeg - (*itBefore).m_fTimeSeg );
		CampAlphaValue( (*itBefore).m_fAlpha, (*it).m_fAlpha, pPar->m_color.a, percentage );
	}
}

void CAlphaSegmentAffector :: CampAlphaValue( float colIN1, float colIN2, float& colOUT, float percentage )
{
	if ( percentage < 0.0f )
	{
		percentage = 0.0f;
	}
	if ( percentage > 1.0f )
	{
		percentage = 1.0f;
	}
	colOUT = (colIN2 * percentage + colIN1 * (1.0f - percentage));
}


AlphaSeg* CAlphaSegmentAffector :: GetAlphaSeg( float fTimeFlag, int & index )
{
	index = -1;
	if ( m_lstAlphaSeg.size() == 0 )
	{
		return NULL;
	}
	index = 0;
	LIST_ALPHA_SEG::iterator it = m_lstAlphaSeg.begin();
	for ( ; it != m_lstAlphaSeg.end(); ++it, ++index )
	{
		if ( (*it).m_fTimeSeg == fTimeFlag )
		{
			return &(*it);
		}
	}
	index = -1;
	return NULL;
}

int CAlphaSegmentAffector :: InsertAlphaSeg( AlphaSeg& CS )
{
	if ( m_lstAlphaSeg.size() == 0 )
	{
		m_lstAlphaSeg.push_back( CS );
		return 0;
	}
	int i = 0;
	LIST_ALPHA_SEG::iterator it = m_lstAlphaSeg.begin();
	for ( ; it != m_lstAlphaSeg.end(); ++it, ++i )
	{
		if ( (*it).m_fTimeSeg > CS.m_fTimeSeg )
		{
			m_lstAlphaSeg.insert( it, CS );
			return i;
		}
		else if ( (*it).m_fTimeSeg == CS.m_fTimeSeg )
		{
			(*it).m_fAlpha = CS.m_fAlpha;
			return i;
		}
	}
	m_lstAlphaSeg.push_back( CS );
	return m_lstAlphaSeg.size()-1;
}

void CAlphaSegmentAffector :: InsertAlphaSeg( AlphaSeg& CS, int id )
{
	if ( id >= m_lstAlphaSeg.size() )
	{
		m_lstAlphaSeg.push_back( CS );
		return;
	}
	LIST_ALPHA_SEG::iterator it = m_lstAlphaSeg.begin();
	std::advance( it, id );
	m_lstAlphaSeg.insert( it, CS );
}

int CAlphaSegmentAffector :: DeleteAlphaSeg( AlphaSeg& CS )
{
	if ( m_lstAlphaSeg.size() == 0 )
	{
		return -1;
	}
	int i = 0;
	LIST_ALPHA_SEG::iterator it = m_lstAlphaSeg.begin();
	for ( ; it != m_lstAlphaSeg.end(); ++it, ++i )
	{
		if ( (*it).m_fTimeSeg == CS.m_fTimeSeg )
		{
			m_lstAlphaSeg.erase( it );
			return i;
		}
	}
	return -1;
}

void CAlphaSegmentAffector :: DeleteAlphaSeg( int id )
{
	if ( m_lstAlphaSeg.size() == 0 )
	{
		return;
	}
	LIST_ALPHA_SEG::iterator it = m_lstAlphaSeg.begin();
	std::advance( it, id );
	m_lstAlphaSeg.erase( it );
}