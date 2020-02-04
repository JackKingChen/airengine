#include "stdafx.h"
#include "ColorSegmentAffector.h"
#include "StringUtil.h"
#include "ParticleIterator.h"
#include "ParticleSystem.h"
#include "Line3D.h"
#include "Quaternion.h"



void CColorSegmentAffector :: AffectParticle(CParticleSystem* pParticleSystem, float fTimeElapsed)
{
	CParticleIterator particleIter = pParticleSystem->GetIterator();
	CParticle* pParticle = NULL;
	while (!particleIter.IsEnd())
	{
		pParticle = particleIter.GetNext();
		SetParticleColorValue( pParticle );
	}
}

bool CColorSegmentAffector :: LoadFromXML(CMarkupSTL& xml)
{
	AIRString strValue = xml.GetAttrib("time_length").c_str();
	if ( strValue.length() > 0 )
	{
		m_fTimeLength = atof( strValue.c_str() );
	}

	xml.IntoElem();
	ColorSeg cs;
	while ( xml.FindElem("time_segment") )
	{
		strValue = xml.GetAttrib("color").c_str();
		if ( strValue.length() > 0 )
		{
			sscanf(strValue.c_str(), "(%f,%f,%f,%f)", &(cs.m_Color.r), &(cs.m_Color.g), &(cs.m_Color.b), &(cs.m_Color.a));
		}

		strValue = xml.GetAttrib("time_flag").c_str();
		if ( strValue.length() > 0 )
		{
			cs.m_fTimeSeg = atof( strValue.c_str() );
		}

		m_vtColorSeg.push_back( cs );
	}
	xml.OutOfElem();

	return true;
}

bool CColorSegmentAffector :: SaveToXMLNode(CMarkupSTL& xml)
{
	xml.AddAttrib("type", "ColorSegment");
	char szTmp[256] = { 0 };

	snprintf(szTmp, 255, "%f", m_fTimeLength);
	xml.AddAttrib("time_length", szTmp);

	xml.IntoElem();

	LIST_COLOR_SEG::iterator it = m_vtColorSeg.begin();
	for ( ; it != m_vtColorSeg.end(); ++it )
	{
		xml.AddElem("time_segment");

		snprintf(szTmp, 255, "(%f,%f,%f,%f)", (*it).m_Color.r, (*it).m_Color.g, (*it).m_Color.b, (*it).m_Color.a );
		xml.AddAttrib( "color", szTmp );

		snprintf(szTmp, 255, "%f", (*it).m_fTimeSeg);
		xml.AddAttrib( "time_flag", szTmp );
	}

	xml.OutOfElem();

	return true;
}


void CColorSegmentAffector :: SetParticleColorValue( CParticle* pPar )
{
	if ( m_vtColorSeg.size() == 0 )
	{
		return;
	}
	int i = 0;
	float fParticleLife = pPar->m_fTotalTimeToLive - pPar->m_fLifeTime;
	LIST_COLOR_SEG::iterator it = m_vtColorSeg.begin(), itEnd = m_vtColorSeg.begin(), itBefore = it;
	for ( ; it != m_vtColorSeg.end(); ++it, ++i )
	{
		if ( fParticleLife <= (*it).m_fTimeSeg )
		{
			break;
		}
	}
	// 到了最后一个颜色段
	if ( m_vtColorSeg.size() == 1  || i == 0 )
	{
		it = m_vtColorSeg.begin();
		pPar->m_color = (*it).m_Color;
	}
	else if ( it == m_vtColorSeg.end() )
	{
		--it;
		pPar->m_color = (*it).m_Color;
	}
	else
	{
		std::advance( itBefore, i-1 );
		float percentage = ( fParticleLife - (*itBefore).m_fTimeSeg ) / ( (*it).m_fTimeSeg - (*itBefore).m_fTimeSeg );
		CampColorValue( (*itBefore).m_Color, (*it).m_Color, pPar->m_color, percentage );
	}
}

void CColorSegmentAffector :: CampColorValue( CColorValue& colIN1, CColorValue& colIN2, CColorValue& colOUT, float percentage )
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


ColorSeg* CColorSegmentAffector :: GetColorSeg( float fTimeFlag, int & index )
{
	index = -1;
	if ( m_vtColorSeg.size() == 0 )
	{
		return NULL;
	}
	index = 0;
	LIST_COLOR_SEG::iterator it = m_vtColorSeg.begin();
	for ( ; it != m_vtColorSeg.end(); ++it, ++index )
	{
		if ( (*it).m_fTimeSeg == fTimeFlag )
		{
			return &(*it);
		}
	}
	index = -1;
	return NULL;
}

int CColorSegmentAffector :: InsertColorSeg( ColorSeg& CS )
{
	if ( m_vtColorSeg.size() == 0 )
	{
		m_vtColorSeg.push_back( CS );
		return 0;
	}
	int i = 0;
	LIST_COLOR_SEG::iterator it = m_vtColorSeg.begin();
	for ( ; it != m_vtColorSeg.end(); ++it, ++i )
	{
		if ( (*it).m_fTimeSeg > CS.m_fTimeSeg )
		{
			m_vtColorSeg.insert( it, CS );
			return i;
		}
		else if ( (*it).m_fTimeSeg == CS.m_fTimeSeg )
		{
			(*it).m_Color = CS.m_Color;
			return i;
		}
	}
	m_vtColorSeg.push_back( CS );
	return m_vtColorSeg.size()-1;
}

void CColorSegmentAffector :: InsertColorSeg( ColorSeg& CS, int id )
{
	if ( id >= m_vtColorSeg.size() )
	{
		m_vtColorSeg.push_back( CS );
		return;
	}
	LIST_COLOR_SEG::iterator it = m_vtColorSeg.begin();
	std::advance( it, id );
	m_vtColorSeg.insert( it, CS );
}

int CColorSegmentAffector :: DeleteColorSeg( ColorSeg& CS )
{
	if ( m_vtColorSeg.size() == 0 )
	{
		return -1;
	}
	int i = 0;
	LIST_COLOR_SEG::iterator it = m_vtColorSeg.begin();
	for ( ; it != m_vtColorSeg.end(); ++it, ++i )
	{
		if ( (*it).m_fTimeSeg == CS.m_fTimeSeg )
		{
			m_vtColorSeg.erase( it );
			return i;
		}
	}
	return -1;
}

void CColorSegmentAffector :: DeleteColorSeg( int id )
{
	if ( m_vtColorSeg.size() == 0 )
	{
		return;
	}
	LIST_COLOR_SEG::iterator it = m_vtColorSeg.begin();
	std::advance( it, id );
	m_vtColorSeg.erase( it );
}