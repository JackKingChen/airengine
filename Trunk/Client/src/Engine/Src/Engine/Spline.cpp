#include "stdafx.h"
#include "Spline.h"
#include "Engine.h"
#include "VertexDef.h"
#include "SceneNode.h"
#include "FileSystem.h"
#include "Primitive.h"

//---------------------------------------------------------------------
VertexSpline::VertexSpline()
{
	m_matCoeffs[0] = 2;
	m_matCoeffs[4] = -2;
	m_matCoeffs[8] = 1;
	m_matCoeffs[12] = 1;
	m_matCoeffs[1] = -3;
	m_matCoeffs[5] = 3;
	m_matCoeffs[9] = -2;
	m_matCoeffs[13] = -1;
	m_matCoeffs[2] = 0;
	m_matCoeffs[6] = 0;
	m_matCoeffs[10] = 1;
	m_matCoeffs[14] = 0;
	m_matCoeffs[3] = 1;
	m_matCoeffs[7] = 0;
	m_matCoeffs[11] = 0;
	m_matCoeffs[15] = 0;

	m_bAutoCalc = true;
}
//---------------------------------------------------------------------
VertexSpline::~VertexSpline()
{
}
//---------------------------------------------------------------------
void VertexSpline::AddPoint(const Vector3Df& p)
{
	m_vtPoints.push_back(p);
	if (m_bAutoCalc)
	{
		RecalcTangents();
	}
}
//---------------------------------------------------------------------
Vector3Df VertexSpline::Interpolate(float t)
{
	// Currently assumes points are evenly spaced, will cause velocity
	// change where this is not the case
	// TODO: base on arclength?

	// Work out which segment this is in
	float fSeg = t * (m_vtPoints.size() - 1);
	unsigned int segIdx = (unsigned int)fSeg;
	// Apportion t 
	t = fSeg - segIdx;

	return Interpolate(segIdx, t);

}
//---------------------------------------------------------------------
Vector3Df VertexSpline::Interpolate(unsigned int fromIndex, float t)
{
	// Bounds check
	assert (fromIndex < m_vtPoints.size() && "fromIndex out of bounds");

	if ((fromIndex + 1) == m_vtPoints.size())
	{
		// Duff request, cannot blend to nothing
		// Just return source
		return m_vtPoints[fromIndex];
	}

	// Fast special cases
	if (t == 0.0f)
	{
		return m_vtPoints[fromIndex];
	}
	else if(t == 1.0f)
	{
		return m_vtPoints[fromIndex + 1];
	}

	// Real interpolation
	// Form a vector of powers of t
	float t2, t3;
	t2 = t * t;
	t3 = t2 * t;
	Vector4f powers(t3, t2, t, 1);


	// Algorithm is ret = powers * m_matCoeffs * Matrix4(point1, point2, tangent1, tangent2)
	Vector3Df& point1 = m_vtPoints[fromIndex];
	Vector3Df& point2 = m_vtPoints[fromIndex+1];
	Vector3Df& tan1 = m_vtTangents[fromIndex];
	Vector3Df& tan2 = m_vtTangents[fromIndex+1];
	Matrix4f pt;

	pt[0] = point1.x;
	pt[4] = point1.y;
	pt[8] = point1.z;
	pt[12] = 1.0f;
	pt[1] = point2.x;
	pt[5] = point2.y;
	pt[9] = point2.z;
	pt[13] = 1.0f;
	pt[2] = tan1.x;
	pt[6] = tan1.y;
	pt[10] = tan1.z;
	pt[14] = 1.0f;
	pt[3] = tan2.x;
	pt[7] = tan2.y;
	pt[11] = tan2.z;
	pt[15] = 1.0f;

	Vector4f ret = pt * m_matCoeffs * powers;

	return Vector3Df(ret.x, ret.y, ret.z);
}
//---------------------------------------------------------------------
void VertexSpline::RecalcTangents(void)
{
	// Catmull-Rom approach
	// 
	// tangent[i] = 0.5 * (point[i+1] - point[i-1])
	//
	// Assume endpoint tangents are parallel with line with neighbour

	size_t i, num_vtPoints;
	bool isClosed;

	num_vtPoints = m_vtPoints.size();
	if (num_vtPoints < 2)
	{
		// Can't do anything yet
		return;
	}

	// Closed or open?
	if (m_vtPoints[0] == m_vtPoints[num_vtPoints-1])
	{
		isClosed = true;
	}
	else
	{
		isClosed = false;
	}

	m_vtTangents.resize(num_vtPoints);



	for(i = 0; i < num_vtPoints; ++i)
	{
		if (i ==0)
		{
			// Special case start
			if (isClosed)
			{
				// Use num_vtPoints-2 since num_vtPoints-1 is the last point and == [0]
				m_vtTangents[i] = 0.5 * (m_vtPoints[1] - m_vtPoints[num_vtPoints-2]);
			}
			else
			{
				m_vtTangents[i] = 0.5 * (m_vtPoints[1] - m_vtPoints[0]);
			}
		}
		else if (i == num_vtPoints-1)
		{
			// Special case end
			if (isClosed)
			{
				// Use same tangent as already calculated for [0]
				m_vtTangents[i] = m_vtTangents[0];
			}
			else
			{
				m_vtTangents[i] = 0.5 * (m_vtPoints[i] - m_vtPoints[i-1]);
			}
		}
		else
		{
			m_vtTangents[i] = 0.5 * (m_vtPoints[i+1] - m_vtPoints[i-1]);
		}

	}



}
//---------------------------------------------------------------------
const Vector3Df& VertexSpline::GetPoint(unsigned short index) const
{
	assert (index < m_vtPoints.size() && "Point index is out of bounds!!");

	return m_vtPoints[index];
}
//---------------------------------------------------------------------
unsigned short VertexSpline::GetNumPoints(void) const
{
	return (unsigned short)m_vtPoints.size();
}
//---------------------------------------------------------------------
void VertexSpline::Clear(void)
{
	m_vtPoints.clear();
	m_vtTangents.clear();
}
//---------------------------------------------------------------------
void VertexSpline::UpdatePoint(unsigned short index, const Vector3Df& value)
{
	assert (index < m_vtPoints.size() && "Point index is out of bounds!!");

	m_vtPoints[index] = value;
	if (m_bAutoCalc)
	{
		RecalcTangents();
	}
}
//---------------------------------------------------------------------
void VertexSpline::SetAutoCalculate(bool autoCalc)
{
	m_bAutoCalc = autoCalc;
}














//---------------------------------------------------------------------
RotationalSpline::RotationalSpline() : m_bAutoCalc(true)
{
}
//---------------------------------------------------------------------
RotationalSpline::~RotationalSpline()
{
}
//---------------------------------------------------------------------
void RotationalSpline::AddPoint(const Quaternion& p)
{
	m_vtPoints.push_back(p);
	if (m_bAutoCalc)
	{
		RecalcTangents();
	}
}
//---------------------------------------------------------------------
Quaternion RotationalSpline::Interpolate(float t, bool useShortestPath)
{
	// Work out which segment this is in
	float fSeg = t * (m_vtPoints.size() - 1);
	unsigned int segIdx = (unsigned int)fSeg;
	// Apportion t 
	t = fSeg - segIdx;

	return Interpolate(segIdx, t, useShortestPath);

}
//---------------------------------------------------------------------
Quaternion RotationalSpline::Interpolate(unsigned int fromIndex, float t,bool useShortestPath)
{
	// Bounds check
	assert (fromIndex >= 0 && fromIndex < m_vtPoints.size() &&	"fromIndex out of bounds");

	if ((fromIndex + 1) == m_vtPoints.size())
	{
		// Duff request, cannot blend to nothing
		// Just return source
		return m_vtPoints[fromIndex];

	}
	// Fast special cases
	if (t == 0.0f)
	{
		return m_vtPoints[fromIndex];
	}
	else if(t == 1.0f)
	{
		return m_vtPoints[fromIndex + 1];
	}

	// Real interpolation
	// Use squad using tangents we've already set up
	Quaternion &p = m_vtPoints[fromIndex];
	Quaternion &q = m_vtPoints[fromIndex+1];
	Quaternion &a = m_vtTangents[fromIndex];
	Quaternion &b = m_vtTangents[fromIndex+1];

	// NB interpolate to nearest rotation
	return Quaternion::Squad(p, a, b, q, t, useShortestPath);

}
//---------------------------------------------------------------------
void RotationalSpline::RecalcTangents(void)
{
	// ShoeMake (1987) approach
	// Just like Catmull-Rom really, just more gnarly
	// And no, I don't understand how to derive this!
	//
	// let p = point[i], pInv = p.Inverse
	// tangent[i] = p * exp( -0.25 * ( log(pInv * point[i+1]) + log(pInv * point[i-1]) ) )
	//
	// Assume endpoint tangents are parallel with line with neighbour

	unsigned int i, numPoints;
	bool isClosed;

	numPoints = (unsigned int)m_vtPoints.size();

	if (numPoints < 2)
	{
		// Can't do anything yet
		return;
	}

	m_vtTangents.resize(numPoints);

	if (m_vtPoints[0] == m_vtPoints[numPoints-1])
	{
		isClosed = true;
	}
	else
	{
		isClosed = false;
	}

	Quaternion invp, part1, part2, preExp;
	for(i = 0; i < numPoints; ++i)
	{
		Quaternion &p = m_vtPoints[i];
		invp = p.Inverse();

		if (i ==0)
		{
			// special case start
			part1 = (invp * m_vtPoints[i+1]).Log();
			if (isClosed)
			{
				// Use numPoints-2 since numPoints-1 == end == start == this one
				part2 = (invp * m_vtPoints[numPoints-2]).Log();
			}
			else
			{
				part2 = (invp * p).Log();
			}
		}
		else if (i == numPoints-1)
		{
			// special case end
			if (isClosed)
			{
				// Wrap to [1] (not [0], this is the same as end == this one)
				part1 = (invp * m_vtPoints[1]).Log();
			}
			else
			{
				part1 = (invp * p).Log();
			}
			part2 = (invp * m_vtPoints[i-1]).Log();
		}
		else
		{
			part1 = (invp * m_vtPoints[i+1]).Log();
			part2 = (invp * m_vtPoints[i-1]).Log();
		}

		preExp = -0.25 * (part1 + part2);
		m_vtTangents[i] = p * preExp.Exp();

	}



}
//---------------------------------------------------------------------
const Quaternion& RotationalSpline::GetPoint(unsigned short index) const
{
	assert (index < m_vtPoints.size() && "Point index is out of bounds!!");

	return m_vtPoints[index];
}
//---------------------------------------------------------------------
unsigned short RotationalSpline::GetNumPoints(void) const
{
	return (unsigned short)m_vtPoints.size();
}
//---------------------------------------------------------------------
void RotationalSpline::Clear(void)
{
	m_vtPoints.clear();
	m_vtTangents.clear();
}
//---------------------------------------------------------------------
void RotationalSpline::UpdatePoint(unsigned short index, const Quaternion& value)
{
	assert (index < m_vtPoints.size() && "Point index is out of bounds!!");

	m_vtPoints[index] = value;
	if (m_bAutoCalc)
	{
		RecalcTangents();
	}
}
//---------------------------------------------------------------------
void RotationalSpline::SetAutoCalculate(bool autoCalc)
{
	m_bAutoCalc = autoCalc;
}
//---------------------------------------------------------------------























//----------------------------------------------------------------------

SplineObject::SplineObject() : m_pVB(NULL)
{
	m_szType = "SplineObject";
	m_nMaxSegmentCount = 2048;
	m_nSegmentCount = 64;
	m_Color.Set( 0xffffffff );
}

SplineObject:: ~SplineObject( void )
{
	Clear();
}

VertexSpline* SplineObject::GetSpline( void )
{
	return &m_Spline;
}

bool SplineObject::LoadFromFile( const AIRString& szFile )
{
	CDataStream* pStream = FILESYSTEM->GetFileStream( szFile.c_str() );
	if ( !pStream )
	{
		DT_TO_MSGBOX_FMT( LT_ERROR, GetString("Load spline cfg file [%s] failed!", szFile.c_str()) );
		return false;
	}
	CMarkupSTL xml(pStream->GetAsString().c_str());

	if ( !xml.FindElem())
	{
		SafeDelete( pStream );
		return false;
	}
	
	m_strName = xml.GetAttrib( "Name" ).c_str();

	int a,r,g,b;
	AIRString szValue = xml.GetAttrib( "Color" ).c_str();
	if ( szValue.length() )
	{
		sscanf( szValue.c_str(), "(%d,%d,%d,%d)", &a, &r, &g, &b );
		m_Color.Set( a, r, g, b );
	}

	szValue = xml.GetAttrib( "MaxSegment" ).c_str();
	if ( szValue.length() )
	{
		m_nMaxSegmentCount = atoi( szValue.c_str() );
	}

	szValue = xml.GetAttrib( "CurSegment" ).c_str();
	if ( szValue.length() )
	{
		m_nSegmentCount = atoi( szValue.c_str() );
	}

	Vector3Df vPos;
	xml.IntoElem();
	while( xml.FindElem() )
	{
		szValue = xml.GetAttrib( "Position" ).c_str();
		if ( szValue.length() )
		{
			sscanf( szValue.c_str(), "(%f,%f,%f)", &vPos.x, &vPos.y, &vPos.z );
			m_Spline.AddPoint( vPos );
		}
	}
	xml.OutOfElem();

	SafeDelete( pStream );

	GenLine();
	return true;
}

void SplineObject::SetMaxSegmentCount( int nMaxSC )
{
	if ( nMaxSC > m_nMaxSegmentCount )
	{
		m_nMaxSegmentCount = nMaxSC;
		Clear();
		CreateSplineVertexBuffer();
	}
}

void SplineObject::SetSegmentCount( int nSC )
{
	if ( nSC > m_nMaxSegmentCount )
	{
		m_nSegmentCount = m_nMaxSegmentCount;
	}
	else if ( nSC > 0)
	{
		m_nSegmentCount = nSC;
	}
	GenLine();
}

void SplineObject::SetColor( const SColor& color )
{
	if ( color != m_Color )
	{
		m_Color = color;
		GenLine();
	}
}

void SplineObject::CreateSplineVertexBuffer( void )
{
	if ( !m_pVB )
	{
		//m_pVB = ENGINE_INST->GetRenderer()->CreateVertexBuffer( E_VB_NOTEX, 2 * m_nMaxSegmentCount, false );
        m_pVB = ENGINE_INST->GetRenderer()->CreateVertexBuffer( 2 * m_nMaxSegmentCount, sizeof(VERTEX_NOTEX), false );
	}
}

void SplineObject::GenLine( void )
{
	CreateSplineVertexBuffer();
	LPVERTEX_NOTEX pVertex = (LPVERTEX_NOTEX)(m_pVB->Lock());
	float fDelta = 1.0f / (float)m_nSegmentCount;
	int id = 0;
	float fBegin = 0.0f;
	for ( int i = 0; i <= m_nSegmentCount; ++i )
	{
		pVertex[id].color = m_Color;
		pVertex[id].position = m_Spline.Interpolate( fBegin );

		fBegin += fDelta;
		++id;

		pVertex[id].color = m_Color;
		pVertex[id].position = m_Spline.Interpolate( fBegin );
		++id;
	}
	m_pVB->Unlock();
}

void SplineObject::Update( float fTimeElapsed )
{

}

void SplineObject::PreRender(CCamera* pCamera)
{

}

/*
void SplineObject::Render(CCamera* pCamera)
{
	if ( !m_pVB || !m_pNode || !m_bVisable )
	{
		return;
	}

	PreRender( pCamera );
	IRenderer* pRenderer = ENGINE_INST->GetRenderer();

	if ( m_pNode )
	{
		pRenderer->SetWorldTransform( &(m_pNode->GetFullTransform()) );
	}
	else
	{
		pRenderer->SetWorldTransform( &Matrix4f::IDENTITY );
	}

	if (m_pMaterial)
	{
		CTechnique* pTechnique = m_pMaterial->GetCurrentTechnique();
		if (pTechnique)
		{
			pRenderer->SetVertexBuffer(0, m_pVB);
			size_t nPassNum = pTechnique->GetPassesNum();
			for (size_t i = 0; i < nPassNum; ++i)
			{
				CPass* pPass = pTechnique->GetPass(i);
				pPass->ApplyPass(pRenderer);
				pRenderer->DrawLineList( 0, m_nSegmentCount );
			}
		}
	}
}
*/

void SplineObject::Clear( void )
{
	if ( m_pVB )
	{
		ENGINE_INST->GetRenderer()->ClearVertexBuffer( m_pVB );
		m_pVB = NULL;
	}
}

void SplineObject::_GetPrimitive()
{
    if (m_pPrimitive == NULL)
    {
        m_pPrimitive = new Primitive(Primitive::Primitive_LineList);
        m_pPrimitive->m_pSrcRenderable = this;
    }

    m_pPrimitive->m_pVertexBuffer = m_pVB;
    //m_pPrimitive->m_pVertexDecl = m_pVB->GetVertexDeclaration();
}
