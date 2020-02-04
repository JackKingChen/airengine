#include "stdafx.h"
#include "FullScaleSegmentAffector.h"
#include "ParticleIterator.h"
#include "ParticleSystem.h"

void CFullScaleSegmentAffector::AffectParticle(CParticleSystem* pParticleSystem, float fTimeElapsed)
{
	CParticleIterator particleIter = pParticleSystem->GetIterator();
	CParticle* pParticle = NULL;
	while (!particleIter.IsEnd())
	{
		pParticle = particleIter.GetNext();
		SetParticleSize( pParticle );
	}
}

bool CFullScaleSegmentAffector::LoadFromXML(CMarkupSTL& xml)
{
	AIRString strValue = xml.GetAttrib("time_length").c_str();
	if ( strValue.length() > 0 )
	{
		m_fTimeLength = atof( strValue.c_str() );
	}

	xml.IntoElem();
	FullScaleSeg cs;
	while ( xml.FindElem("time_segment") )
	{
		strValue = xml.GetAttrib("width_scale").c_str();
		if ( strValue.length() > 0 )
		{
			cs.m_fWScale = atof( strValue.c_str() );
		}

		strValue = xml.GetAttrib("height_scale").c_str();
		if ( strValue.length() > 0 )
		{
			cs.m_fHScale = atof( strValue.c_str() );
		}

		strValue = xml.GetAttrib("time_flag").c_str();
		if ( strValue.length() > 0 )
		{
			cs.m_fTimeSeg = atof( strValue.c_str() );
		}

		m_lstFullScaleSeg.push_back( cs );
	}
	xml.OutOfElem();

	return true;
}

bool CFullScaleSegmentAffector::SaveToXMLNode(CMarkupSTL& xml)
{
	xml.AddAttrib("type", "FullScaleSegment");
	char szTmp[256] = { 0 };

	snprintf(szTmp, 255, "%f", m_fTimeLength);
	xml.AddAttrib("time_length", szTmp);

	xml.IntoElem();

	LIST_FULL_SCALE_SEG::iterator it = m_lstFullScaleSeg.begin();
	for ( ; it != m_lstFullScaleSeg.end(); ++it )
	{
		xml.AddElem("time_segment");

		snprintf(szTmp, 255, "%f", (*it).m_fWScale );
		xml.AddAttrib( "width_scale", szTmp );

		snprintf(szTmp, 255, "%f", (*it).m_fHScale );
		xml.AddAttrib( "height_scale", szTmp );

		snprintf(szTmp, 255, "%f", (*it).m_fTimeSeg);
		xml.AddAttrib( "time_flag", szTmp );
	}

	xml.OutOfElem();

	return true;
}


FullScaleSeg* CFullScaleSegmentAffector::GetScaleSeg( size_t id )
{
	if ( id >= m_lstFullScaleSeg.size() )
	{
		return NULL;
	}
	LIST_FULL_SCALE_SEG::iterator it = m_lstFullScaleSeg.begin();
	std::advance( it, id );
	return &(*it);
}


FullScaleSeg* CFullScaleSegmentAffector::GetScaleSeg( float fTimeFlag, int & index )
{
	index = -1;
	if ( m_lstFullScaleSeg.size() == 0 )
	{
		return NULL;
	}
	index = 0;
	LIST_FULL_SCALE_SEG::iterator it = m_lstFullScaleSeg.begin();
	for ( ; it != m_lstFullScaleSeg.end(); ++it, ++index )
	{
		if ( (*it).m_fTimeSeg == fTimeFlag )
		{
			return &(*it);
		}
	}
	index = -1;
	return NULL;
}

int CFullScaleSegmentAffector::InsertScaleSeg( FullScaleSeg& CS )
{
	if ( m_lstFullScaleSeg.size() == 0 )
	{
		m_lstFullScaleSeg.push_back( CS );
		return 0;
	}
	int i = 0;
	LIST_FULL_SCALE_SEG::iterator it = m_lstFullScaleSeg.begin();
	for ( ; it != m_lstFullScaleSeg.end(); ++it, ++i )
	{
		if ( (*it).m_fTimeSeg > CS.m_fTimeSeg )
		{
			m_lstFullScaleSeg.insert( it, CS );
			return i;
		}
		else if ( (*it).m_fTimeSeg == CS.m_fTimeSeg )
		{
			(*it).m_fWScale = CS.m_fWScale;
			(*it).m_fHScale = CS.m_fHScale;
			return i;
		}
	}
	m_lstFullScaleSeg.push_back( CS );
	return m_lstFullScaleSeg.size()-1;
}

void CFullScaleSegmentAffector::InsertScaleSeg( FullScaleSeg& CS, size_t  id )
{
	if ( id >= m_lstFullScaleSeg.size() )
	{
		m_lstFullScaleSeg.push_back( CS );
		return;
	}
	LIST_FULL_SCALE_SEG::iterator it = m_lstFullScaleSeg.begin();
	std::advance( it, id );
	m_lstFullScaleSeg.insert( it, CS );
}

int CFullScaleSegmentAffector::DeleteScaleSeg( FullScaleSeg& CS )
{
	if ( m_lstFullScaleSeg.size() == 0 )
	{
		return -1;
	}
	int i = 0;
	LIST_FULL_SCALE_SEG::iterator it = m_lstFullScaleSeg.begin();
	for ( ; it != m_lstFullScaleSeg.end(); ++it, ++i )
	{
		if ( (*it).m_fTimeSeg == CS.m_fTimeSeg )
		{
			m_lstFullScaleSeg.erase( it );
			return i;
		}
	}
	return -1;
}

void CFullScaleSegmentAffector::DeleteScaleSeg(size_t id )
{
	if ( m_lstFullScaleSeg.size() == 0 )
	{
		return;
	}
	LIST_FULL_SCALE_SEG::iterator it = m_lstFullScaleSeg.begin();
	std::advance( it, id );
	m_lstFullScaleSeg.erase( it );
}


void CFullScaleSegmentAffector::SetParticleSize( CParticle* pPar )
{
	if ( m_lstFullScaleSeg.size() == 0 )
	{
		return;
	}
	int i = 0;
	float fParticleLife = pPar->m_fTotalTimeToLive - pPar->m_fLifeTime;
	LIST_FULL_SCALE_SEG::iterator it = m_lstFullScaleSeg.begin(), itEnd = m_lstFullScaleSeg.begin(), itBefore = it;
	for ( ; it != m_lstFullScaleSeg.end(); ++it, ++i )
	{
		if ( fParticleLife <= (*it).m_fTimeSeg )
		{
			break;
		}
	}
	float fCurWScale, fCurHScale;
	if ( m_lstFullScaleSeg.size() == 1 || i == 0 )
	{
		it = m_lstFullScaleSeg.begin();
		fCurWScale = (*it).m_fWScale;
		fCurHScale = (*it).m_fHScale;
	}
	else if ( it == m_lstFullScaleSeg.end() )
	{
		--it;
		fCurWScale = (*it).m_fWScale;
		fCurHScale = (*it).m_fHScale;
	}
	else
	{
		std::advance( itBefore, i-1 );
		float percentage = ( fParticleLife - (*itBefore).m_fTimeSeg ) / ( (*it).m_fTimeSeg - (*itBefore).m_fTimeSeg );
		float scale;
		CampScaleValue( (*itBefore).m_fWScale, (*it).m_fWScale, fCurWScale, percentage );
		CampScaleValue( (*itBefore).m_fHScale, (*it).m_fHScale, fCurHScale, percentage );
	}
	pPar->m_fHeight = pPar->m_fInitHeight * fCurHScale;
	pPar->m_fWidth  = pPar->m_fInitWidth * fCurWScale;
}

void CFullScaleSegmentAffector::CampScaleValue( float colIN1, float colIN2, float& colOUT, float percentage )
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