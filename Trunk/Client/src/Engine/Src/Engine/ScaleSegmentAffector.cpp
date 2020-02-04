#include "stdafx.h"
#include "ScaleSegmentAffector.h"
#include "ParticleIterator.h"
#include "ParticleSystem.h"

void CScaleSegmentAffector::AffectParticle(CParticleSystem* pParticleSystem, float fTimeElapsed)
{
	CParticleIterator particleIter = pParticleSystem->GetIterator();
	CParticle* pParticle = NULL;
	while (!particleIter.IsEnd())
	{
		pParticle = particleIter.GetNext();
		SetParticleSize( pParticle );
	}
}

bool CScaleSegmentAffector::LoadFromXML(CMarkupSTL& xml)
{
	AIRString strValue = xml.GetAttrib("time_length").c_str();
	if ( strValue.length() > 0 )
	{
		m_fTimeLength = atof( strValue.c_str() );
	}

	xml.IntoElem();
	ScaleSeg cs;
	while ( xml.FindElem("time_segment") )
	{
		strValue = xml.GetAttrib("scale").c_str();
		if ( strValue.length() > 0 )
		{
			cs.m_fScale = atof( strValue.c_str() );
		}

		strValue = xml.GetAttrib("time_flag").c_str();
		if ( strValue.length() > 0 )
		{
			cs.m_fTimeSeg = atof( strValue.c_str() );
		}

		m_lstScaleSeg.push_back( cs );
	}
	xml.OutOfElem();

	return true;
}

bool CScaleSegmentAffector::SaveToXMLNode(CMarkupSTL& xml)
{
	xml.AddAttrib("type", "ScaleSegment");
	char szTmp[256] = { 0 };

	snprintf(szTmp, 255, "%f", m_fTimeLength);
	xml.AddAttrib("time_length", szTmp);

	xml.IntoElem();

	LIST_SCALE_SEG::iterator it = m_lstScaleSeg.begin();
	for ( ; it != m_lstScaleSeg.end(); ++it )
	{
		xml.AddElem("time_segment");

		snprintf(szTmp, 255, "%f", (*it).m_fScale );
		xml.AddAttrib( "scale", szTmp );

		snprintf(szTmp, 255, "%f", (*it).m_fTimeSeg);
		xml.AddAttrib( "time_flag", szTmp );
	}

	xml.OutOfElem();

	return true;
}


ScaleSeg* CScaleSegmentAffector::GetScaleSeg( size_t id )
{
	if ( id >= m_lstScaleSeg.size() )
	{
		return NULL;
	}
	LIST_SCALE_SEG::iterator it = m_lstScaleSeg.begin();
	std::advance( it, id );
	return &(*it);
}


ScaleSeg* CScaleSegmentAffector::GetScaleSeg( float fTimeFlag, int & index )
{
	index = -1;
	if ( m_lstScaleSeg.size() == 0 )
	{
		return NULL;
	}
	index = 0;
	LIST_SCALE_SEG::iterator it = m_lstScaleSeg.begin();
	for ( ; it != m_lstScaleSeg.end(); ++it, ++index )
	{
		if ( (*it).m_fTimeSeg == fTimeFlag )
		{
			return &(*it);
		}
	}
	index = -1;
	return NULL;
}

int CScaleSegmentAffector::InsertScaleSeg( ScaleSeg& CS )
{
	if ( m_lstScaleSeg.size() == 0 )
	{
		m_lstScaleSeg.push_back( CS );
		return 0;
	}
	int i = 0;
	LIST_SCALE_SEG::iterator it = m_lstScaleSeg.begin();
	for ( ; it != m_lstScaleSeg.end(); ++it, ++i )
	{
		if ( (*it).m_fTimeSeg > CS.m_fTimeSeg )
		{
			m_lstScaleSeg.insert( it, CS );
			return i;
		}
		else if ( (*it).m_fTimeSeg == CS.m_fTimeSeg )
		{
			(*it).m_fScale = CS.m_fScale;
			return i;
		}
	}
	m_lstScaleSeg.push_back( CS );
	return m_lstScaleSeg.size()-1;
}

void CScaleSegmentAffector::InsertScaleSeg( ScaleSeg& CS, int id )
{
	if ( id >= m_lstScaleSeg.size() )
	{
		m_lstScaleSeg.push_back( CS );
		return;
	}
	LIST_SCALE_SEG::iterator it = m_lstScaleSeg.begin();
	std::advance( it, id );
	m_lstScaleSeg.insert( it, CS );
}

int CScaleSegmentAffector::DeleteScaleSeg( ScaleSeg& CS )
{
	if ( m_lstScaleSeg.size() == 0 )
	{
		return -1;
	}
	int i = 0;
	LIST_SCALE_SEG::iterator it = m_lstScaleSeg.begin();
	for ( ; it != m_lstScaleSeg.end(); ++it, ++i )
	{
		if ( (*it).m_fTimeSeg == CS.m_fTimeSeg )
		{
			m_lstScaleSeg.erase( it );
			return i;
		}
	}
	return -1;
}

void CScaleSegmentAffector::DeleteScaleSeg( int id )
{
	if ( m_lstScaleSeg.size() == 0 )
	{
		return;
	}
	LIST_SCALE_SEG::iterator it = m_lstScaleSeg.begin();
	std::advance( it, id );
	m_lstScaleSeg.erase( it );
}


void CScaleSegmentAffector::SetParticleSize( CParticle* pPar )
{
	if ( m_lstScaleSeg.size() == 0 )
	{
		return;
	}
	int i = 0;
	float fParticleLife = pPar->m_fTotalTimeToLive - pPar->m_fLifeTime;
	LIST_SCALE_SEG::iterator it = m_lstScaleSeg.begin(), itEnd = m_lstScaleSeg.begin(), itBefore = it;
	for ( ; it != m_lstScaleSeg.end(); ++it, ++i )
	{
		if ( fParticleLife <= (*it).m_fTimeSeg )
		{
			break;
		}
	}
	float fCurScale;
	if ( m_lstScaleSeg.size() == 1 || i == 0 )
	{
		it = m_lstScaleSeg.begin();
		fCurScale = (*it).m_fScale;
	}
	else if ( it == m_lstScaleSeg.end() )
	{
		--it;
		fCurScale = (*it).m_fScale;
	}
	else
	{
		std::advance( itBefore, i-1 );
		float percentage = ( fParticleLife - (*itBefore).m_fTimeSeg ) / ( (*it).m_fTimeSeg - (*itBefore).m_fTimeSeg );
		float scale;
		CampScaleValue( (*itBefore).m_fScale, (*it).m_fScale, fCurScale, percentage );
	}
	pPar->m_fHeight = pPar->m_fInitHeight * fCurScale;
	pPar->m_fWidth  = pPar->m_fInitWidth * fCurScale;
}

void CScaleSegmentAffector::CampScaleValue( float colIN1, float colIN2, float& colOUT, float percentage )
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