#include "stdafx.h"
#include "SceneDebug.h"
#include "MaterialManager.h"
#include "Primitive.h"
#include "IVertexBuffer.h"
#include "Engine.h"
#include "IRenderer.h"
#include "Camera.h"
#include "MathUtil.h"
#include "RenderContext.h"
#include "VertexDeclarationManager.h"

SceneDebug::SceneDebug()
	: m_pMatCommonWithDepth(NULL)
	, m_pMatCommonWithoutDepth(NULL)
	, m_pMatScreenWithDepth(NULL)
	, m_pMatScreenWithoutDepth(NULL)
	, m_pPrimLine(NULL)
	, m_pPrimTriangle(NULL)
	, m_pVBLine(NULL)
	, m_pVBTriangle(NULL)
	, m_bReady(false)
	, m_pPrimLineScreen(NULL)
	, m_pPrimTriangleScreen(NULL)
	, m_pVBLineScreen(NULL)
	, m_pVBTriangleScreen(NULL)
	, m_bInit(false)
	//, m_strVertexDecl("E_VB_RHW_NOTEX")
{
	this->GetSphereTriangles(m_Sphere, 5);

//	if (!m_bInit)
//	{
//		Init();
//	}
}

SceneDebug::~SceneDebug()
{

}

void SceneDebug::Init()
{
	m_pMatCommonWithDepth    = MATERIAL_MANAGER->CreateMaterial("TemplateMaterial.xml/SceneDbgCommonWithDepth");
	m_pMatCommonWithoutDepth = MATERIAL_MANAGER->CreateMaterial("TemplateMaterial.xml/SceneDbgCommonWithoutDepth");
	m_pMatScreenWithDepth    = MATERIAL_MANAGER->CreateMaterial("TemplateMaterial.xml/SceneDbgScreenWithDepth");
	m_pMatScreenWithoutDepth = MATERIAL_MANAGER->CreateMaterial("TemplateMaterial.xml/SceneDbgScreenWithoutDepth");
	assert( m_pMatCommonWithDepth );
	assert( m_pMatCommonWithoutDepth );
	assert( m_pMatScreenWithDepth );
	assert( m_pMatScreenWithoutDepth );

	m_bInit = true;
}

void SceneDebug::RenderRay(AIR::Ray &ray,bool zEnabled)
{
	stLine line;
	line.pos1 = ray.m_Origin;
	line.pos2 = ray.m_Dir * 100.0f;
	line.color = SColor::White;
	if (zEnabled)
	{
		m_LinesWithDepth.push_back(line);
	}
	else
	{
		m_LinesWithoutDepth.push_back(line);
	}
}

void SceneDebug::RenderLine(const AIR::Vector3Df &pos1, const AIR::Vector3Df &pos2, SColor color,bool zEnabled )
{
	stLine line;
	line.pos1 = pos1;
	line.pos2 = pos2;
	line.color = color;

	if (zEnabled)
	{
		m_LinesWithDepth.push_back(line);
	}
	else
	{
		m_LinesWithoutDepth.push_back(line);
	}
}

void SceneDebug::RenderLineScreen(const AIR::Vector3Df &pos1, const AIR::Vector3Df &pos2, SColor color)
{
	stLine line;
	line.pos1 = pos1;
	line.pos2 = pos2;
	line.color = color;
	m_ScreenLines.push_back(line);
}

void SceneDebug::RenderRectScreen(const AIR::Rect& rect, const SColor& color, bool solid)
{
	AIR::Vector3Df lt((float)rect.left, (float)rect.top, 0);
	AIR::Vector3Df lb((float)rect.left, (float)rect.bottom, 0);
	AIR::Vector3Df rt((float)rect.right, (float)rect.top, 0);
	AIR::Vector3Df rb((float)rect.right, (float)rect.bottom, 0);
	if (solid)
	{
		this->RenderTriangleScreen(lt, rt, lb, color);
		this->RenderTriangleScreen(lb, rt, rb, color);
	}
	else
	{
		this->RenderLineScreen(lt, lb, color);
		this->RenderLineScreen(lb, rb, color);
		this->RenderLineScreen(rb, rt, color);
		this->RenderLineScreen(rt, lt, color);
	}
}

void SceneDebug::RenderTriangleScreen(const AIR::Vector3Df &pos1, const AIR::Vector3Df &pos2, const AIR::Vector3Df &pos3, const SColor& color)
{
	stTriangle triangle;
	triangle.pos1 = pos1;
	triangle.pos2 = pos2;
	triangle.pos3 = pos3;
	triangle.color = color;

	m_TriangleScreen.push_back(triangle);
}

void SceneDebug::RenderTriangle(const AIR::Vector3Df &pos1, const AIR::Vector3Df &pos2, const AIR::Vector3Df &pos3, const SColor& color, bool zEnabled )
{
	stTriangle triangle;
	triangle.pos1 = pos1;
	triangle.pos2 = pos2;
	triangle.pos3 = pos3;
	triangle.color = color;

	if (zEnabled)
		m_TriWithDepth.push_back(triangle);
	else
		m_TriWithoutDepth.push_back(triangle);
}

void SceneDebug::RenderCircle(const AIR::Vector3Df& center, const AIR::Vector3Df& normal, AIR::f32 radius, const SColor& color, bool zEnabled )
{
	AIRVector<AIR::Vector3Df> points;
	points.resize(20);
	this->GetCirclePoints(points, center, normal, radius);
	// add to line list
	this->RenderLine(points.back(), points.front(), color, zEnabled);
	for (u32 i = 0; i < points.size() - 1; ++i)
	{
		this->RenderLine(points[i], points[i + 1], color, zEnabled);
	}
}

void SceneDebug::RenderSphere(const AIR::Vector3Df& center, AIR::f32 radius, const SColor& color, bool zEnabled )
{
	AIRVector<stTriangle> triangles = m_Sphere;

	AIRVector<stTriangle>::iterator it = triangles.begin();

	// transform
	for (;it != triangles.end(); ++it)
	{
		stTriangle& triangle = *it;
		triangle.color = color;
		triangle.pos1 = triangle.pos1 * radius + center;
		triangle.pos2 = triangle.pos2 * radius + center;
		triangle.pos3 = triangle.pos3 * radius + center;
	}

	if (zEnabled)
	{
		m_TriWithDepth.insert(m_TriWithDepth.end(),
			triangles.begin(), triangles.end());
	}
	else
	{
		m_TriWithoutDepth.insert(m_TriWithoutDepth.end(),
			triangles.begin(), triangles.end());
	}
}

void SceneDebug::RenderAABB(const AIR::AABBBox3Df& aabb, const SColor& color, bool zEnabled )
{
	AIRVector<stLine> & LineArray = zEnabled ? m_LinesWithDepth : m_LinesWithoutDepth;
	this->GetBoxEdgeLines(LineArray, aabb.MinEdge, aabb.MaxEdge, color);
}

void SceneDebug::RenderAABB(const AIR::AABBBox3Df& aabb, const AIR::Matrix4f& transform, const SColor& color, bool zEnabled )
{
	AIRVector<stLine> & LineArray = zEnabled ? m_LinesWithDepth : m_LinesWithoutDepth;
	size_t index = LineArray.size();
	this->GetBoxEdgeLines(LineArray, aabb.MinEdge, aabb.MaxEdge, color);
	for (size_t i = index; i < LineArray.size(); ++i)
	{
		transform.Apply4x4(LineArray[i].pos1, LineArray[i].pos1);
		transform.Apply4x4(LineArray[i].pos2, LineArray[i].pos2);
	}
}

void SceneDebug::RenderOBB(const AIR::AABBBox3Df& aabb, const AIR::Matrix4f& xform, const SColor& color, bool zEnabled )
{
	AIR::Vector3Df points[8];
	// calc point error
	/*for (int i = 0; i < 8; ++i)
	{
		points[i] = aabb.GetCornerPoint(i) * xform;
	}*/
	AIR::Vector3Df ext = aabb.GetExtent();
	AIR::Vector3Df rx = xform.GetRow3(0) * ext.x;
	AIR::Vector3Df ry = xform.GetRow3(1) * ext.y;
	AIR::Vector3Df rz = xform.GetRow3(2) * ext.z;
	AIR::Vector3Df p = aabb.GetCenter();
	points[0] = p - rx + ry - rz;
	points[1] = p - rx + ry + rz;
	points[2] = p + rx + ry + rz;
	points[3] = p + rx + ry - rz;
	points[4] = p - rx - ry - rz;
	points[5] = p - rx - ry + rz;
	points[6] = p + rx - ry + rz;
	points[7] = p + rx - ry - rz;

	this->RenderLine(points[0], points[1], color, zEnabled);
	this->RenderLine(points[1], points[2], color, zEnabled);
	this->RenderLine(points[2], points[3], color, zEnabled);
	this->RenderLine(points[3], points[0], color, zEnabled);
	this->RenderLine(points[4], points[5], color, zEnabled);
	this->RenderLine(points[5], points[6], color, zEnabled);
	this->RenderLine(points[6], points[7], color, zEnabled);
	this->RenderLine(points[7], points[4], color, zEnabled);
	this->RenderLine(points[0], points[4], color, zEnabled);
	this->RenderLine(points[1], points[5], color, zEnabled);
	this->RenderLine(points[2], points[6], color, zEnabled);
	this->RenderLine(points[3], points[7], color, zEnabled);
}

void SceneDebug::RenderOBB(const AIR::OBBoxf& obb, const SColor& color, bool zEnabled /* = false */ )
{
	AIR::AABBBox3Df aabb(obb.center_ - obb.extent_, obb.center_ + obb.extent_);
	AIR::Matrix4f mat = obb.rotation_.GetMatrix();
	RenderOBB( aabb, mat, color, zEnabled );
}

void SceneDebug::RenderBox(const AIR::AABBBox3Df& aabb, const SColor& color, bool zEnabled )
{
	AIR::Vector3Df points[8];
	for (int i = 0; i < 8; ++i)
	{
		points[i] = aabb.Point(i);
	}
	this->RenderBox(points, color, zEnabled);
}

void SceneDebug::RenderBox(const AIR::AABBBox3Df& aabb, const AIR::Matrix4f& xform, const SColor& color, bool zEnabled )
{
	AIR::Vector3Df points[8];
	for (int i = 0; i < 8; ++i)
	{
		points[i] = aabb.Point(i) * xform;
	}
	this->RenderBox(points, color, zEnabled);
}


/*
    Edges are stored in this way:
    Hey, am I an ascii artist, or what? :) niko.
      /1--------/0
     /  |      / |
    /   |     /  |
    5---------4  |
    |   3- - -| -2
    |  /      |  /
    |/        | /
    7---------6/
*/
void SceneDebug::RenderBox(const AIR::Vector3Df points[8], const SColor& color, bool zEnabled )
{
	this->RenderTriangle(points[0], points[1], points[2], color, zEnabled);
	this->RenderTriangle(points[2], points[1], points[3], color, zEnabled);
	this->RenderTriangle(points[5], points[4], points[7], color, zEnabled);
	this->RenderTriangle(points[7], points[4], points[6], color, zEnabled);
	this->RenderTriangle(points[4], points[0], points[6], color, zEnabled);
	this->RenderTriangle(points[6], points[0], points[2], color, zEnabled);
	this->RenderTriangle(points[1], points[5], points[3], color, zEnabled);
	this->RenderTriangle(points[3], points[5], points[7], color, zEnabled);
	this->RenderTriangle(points[1], points[0], points[5], color, zEnabled);
	this->RenderTriangle(points[5], points[0], points[4], color, zEnabled);
	this->RenderTriangle(points[7], points[6], points[3], color, zEnabled);
	this->RenderTriangle(points[3], points[6], points[2], color, zEnabled);
}

void SceneDebug::RenderCone(const AIR::Vector3Df& pos, AIR::f32 angle, AIR::f32 height, const AIR::Vector3Df& dir, const SColor& color, bool zEnabled )
{
	AIR::Vector3Df center = pos + dir * height;
	float radius = height * tanf(angle * 0.5f);
	AIRVector<AIR::Vector3Df> points;
	points.resize(20);
	this->GetCirclePoints(points, center, dir, radius);
	// side
	this->RenderTriangle(pos, points.back(), points.front(), color, zEnabled);
	for (u32 i = 0; i < points.size() - 1; ++i)
	{
		this->RenderTriangle(pos, points[i], points[i + 1], color, zEnabled);
	}
}

void SceneDebug::RenderCapsule(const AIR::Vector3Df& a, const AIR::Vector3Df& b, AIR::f32 radius, const SColor& color, bool zEnabled )
{
	this->RenderSphere(a, radius, color, zEnabled);
	this->RenderSphere(b, radius, color, zEnabled);
	this->RenderCylinder(a, b, radius, color, zEnabled);
}

void SceneDebug::RenderCylinder(const AIR::Vector3Df& a, const AIR::Vector3Df& b, AIR::f32 radius, const SColor& color, bool zEnabled )
{
	AIR::Vector3Df dir = a - b;
	dir.Normalize();
	AIRVector<AIR::Vector3Df> apoints;
	apoints.resize(20);
	AIRVector<AIR::Vector3Df> bpoints;
	bpoints.resize(20);
	this->GetCirclePoints(apoints, a, dir, radius);
	this->GetCirclePoints(bpoints, b, dir, radius);

	for (u32 i = 0; i < bpoints.size() - 1; ++i)
	{
		this->RenderTriangle(b, bpoints[i], bpoints[i + 1], color, zEnabled);
	}
	this->RenderTriangle(b, bpoints.back(), bpoints.front(), color, zEnabled);

	for (u32 i = 0; i < apoints.size() - 1; ++i)
	{
		this->RenderTriangle(apoints[i], apoints[i + 1], bpoints[i], color, zEnabled);
		this->RenderTriangle(apoints[i + 1], bpoints[i + 1], bpoints[i], color, zEnabled);
	}
	this->RenderTriangle(apoints.back(), apoints.front(), bpoints.back(), color, zEnabled);
	this->RenderTriangle(apoints.front(), bpoints.front(), bpoints.back(), color, zEnabled);

	for (u32 i = 0; i < apoints.size() - 1; ++i)
	{
		this->RenderTriangle(a, apoints[i], apoints[i + 1], color, zEnabled);
	}
	this->RenderTriangle(a, apoints.back(), apoints.front(), color, zEnabled);
}

void SceneDebug::GetBoxEdgeLines(AIRVector<stLine>&lines, const AIR::Vector3Df &minpos, const AIR::Vector3Df &maxpos, SColor color)
{
	stLine line;
	line.color = color;

	line.pos1 = minpos;

	line.pos2 = AIR::Vector3Df(maxpos.x, minpos.y, minpos.z);
	lines.push_back(line);
	line.pos2 = AIR::Vector3Df(minpos.x, maxpos.y, minpos.z);
	lines.push_back(line);
	line.pos2 = AIR::Vector3Df(minpos.x, minpos.y, maxpos.z);
	lines.push_back(line);

	line.pos1 = maxpos;

	line.pos2 = AIR::Vector3Df(minpos.x, maxpos.y, maxpos.z);
	lines.push_back(line);
	line.pos2 = AIR::Vector3Df(maxpos.x, minpos.y, maxpos.z);
	lines.push_back(line);
	line.pos2 = AIR::Vector3Df(maxpos.x, maxpos.y, minpos.z);
	lines.push_back(line);

	line.pos1 = AIR::Vector3Df(minpos.x, maxpos.y, minpos.z);
	line.pos2 = AIR::Vector3Df(maxpos.x, maxpos.y, minpos.z);
	lines.push_back(line);

	line.pos1 = AIR::Vector3Df(minpos.x, minpos.y, maxpos.z);
	line.pos2 = AIR::Vector3Df(maxpos.x, minpos.y, maxpos.z);
	lines.push_back(line);

	line.pos1 = AIR::Vector3Df(minpos.x, maxpos.y, minpos.z);
	line.pos2 = AIR::Vector3Df(minpos.x, maxpos.y, maxpos.z);
	lines.push_back(line);

	line.pos1 = AIR::Vector3Df(maxpos.x, minpos.y, minpos.z);
	line.pos2 = AIR::Vector3Df(maxpos.x, minpos.y, maxpos.z);
	lines.push_back(line);

	line.pos1 = AIR::Vector3Df(minpos.x, maxpos.y, maxpos.z);
	line.pos2 = AIR::Vector3Df(minpos.x, minpos.y, maxpos.z);
	lines.push_back(line);

	line.pos1 = AIR::Vector3Df(maxpos.x, maxpos.y, minpos.z);
	line.pos2 = AIR::Vector3Df(maxpos.x, minpos.y, minpos.z);
	lines.push_back(line);
}

void SceneDebug::GetCirclePoints(AIRVector<AIR::Vector3Df>& points, const AIR::Vector3Df& center, const AIR::Vector3Df& normal, AIR::f32 radius)
{
	assert(!points.empty());

	Vector3Df vLeft = Vector3Df::UNIT_Y;
	if (false == AIR::iszero(normal.y))
	{
		if (AIR::iszero(normal.x) && AIR::iszero(normal.z))
		{
			vLeft = Vector3Df::UNIT_X;
		}
		else
		{
			Vector3Df vUp = normal;
			vUp.y = 0.0f;
			vLeft = normal.CrossProduct(vUp);
		}
	}
	vLeft.Normalize();
	Matrix4f matRot;
	float step = TWO_PI / points.size();
	for (u32 i = 0; i < points.size(); ++i)
	{
		float radian = i * step;
		Vector3Df vDir = vLeft;
		matRot.BuildRotationMatrixByAxisRadians(normal, radian);
		matRot.TransformVect(vDir);
		vDir.Normalize();
		points[i] = center + vDir * radius;
	}


	//AIR::Vector3Df u, v;
	//if (AIR::Abs(normal.x) > AIR::Abs(normal.z))
	//{
	//	float invLength = 1.0f / sqrtf(normal.x * normal.x + normal.z * normal.z);
	//	u.x = -normal.y * invLength;
	//	u.z = 0;
	//	u.y = normal.x * invLength;
	//	v.x = normal.y * u.z;
	//	v.z = normal.y * u.x - normal.x * u.y;
	//	v.y = -normal.z * u.x;
	//}
	//else
	//{
	//	float invLength = 1.0f / sqrtf(normal.z * normal.z + normal.y * normal.y);
	//	u.x = 0;
	//	u.z = normal.y * invLength;
	//	u.y = -normal.z * invLength;
	//	v.x = normal.z * u.y - normal.y * u.z;
	//	v.z = -normal.x * u.y;
	//	v.y = normal.x * u.z;
	//}
	//Normalize(u);
	//Normalize(v);
	//// compute points
	//float step = TWO_PI / points.size();
	//for (u32 i = 0; i < points.size(); ++i)
	//{
	//	float t = i * step;
	//	points[i] = center + u * radius * cosf(t) + v * radius * sinf(t);
	//}
}

void SceneDebug::GetSphereTriangles(AIRVector<stTriangle>& triangles, AIR::u32 iterations)
{
	int numVertices = iterations * iterations + 2;
	int numEle = 6 * iterations * (iterations-1) + 3 * iterations * 2;
	int primityCount = numEle / 3;
	u32 nLastVert = numVertices - 1;
	AIRVector<Vector3Df> pVertices;
	pVertices.resize(numVertices);

	pVertices[0].x  = 0;  pVertices[0].z  = 0;	 pVertices[0].y = 1.0f;
	pVertices[nLastVert].x  = 0;  pVertices[nLastVert].z  = 0;	 pVertices[nLastVert].y = -1.0f;
	float AngleStep = AIR::PI / (iterations+1);
	float r, uvr, c, s, sc;
	u32   x, y;
	float dirAngle;
	for ( y = 0; y < iterations; ++y )
	{
		dirAngle = AIR::HALF_PI - AngleStep * (y + 1);
		r = CMathUtil::Cos( dirAngle );
		uvr = static_cast<float>(y+1)/(iterations + 1);

		for ( x = 0; x < iterations; ++x )
		{
			c = CMathUtil::Cos(AIR::TWO_PI * x / iterations) * r;
			s = CMathUtil::Sin(AIR::TWO_PI * x / iterations) * r;
			sc = CMathUtil::Sin( dirAngle );

			u32 id = 1+y*iterations + x;
			pVertices[id].x = c;
			pVertices[id].z = s;
			pVertices[id].y = sc;
		}
	}

	triangles.resize(primityCount);

	//////////////////////////////////////////////////////////////////////////
	u32 nCurID = 0;
	for (u32 k = 0; k < iterations; k++)
	{
		triangles[k].pos1 = pVertices[0];
		triangles[k].pos3 = pVertices[k+1];

		if (k != iterations-1)
		{
			triangles[k].pos2 = pVertices[k+2];
		}
		else
		{
			triangles[k].pos2 = pVertices[1];
		}

		nCurID += 3;
	}

	for(u32 y=0; y<iterations-1; y++) 
	{
		for(u32 x=0; x<iterations; x++) 
		{
			//u16 *twoface = indexbuffer + (y*iterations+x)*6 + 3 * iterations;
			u32 tri = ((y*iterations+x)*6 + 3 * iterations) / 3;

			u16 p0 = 1+y * iterations + x;
			u16 p1 = 1+y * iterations + x + 1;
			u16 p2 = 1+(y+1)* iterations + x;
			u16 p3 = 1+(y+1)* iterations + x + 1;

			if (x == iterations-1)
			{
				p1 -= x+1;
				p3 -= x+1;
			}

			// First triangle
			triangles[tri].pos1=pVertices[p0];
			triangles[tri].pos2=pVertices[p1];
			triangles[tri].pos3=pVertices[p2];

			++tri;
			// Second triangle
			triangles[tri].pos1=pVertices[p2];
			triangles[tri].pos2=pVertices[p1];
			triangles[tri].pos3=pVertices[p3];

			nCurID += 6;
		}
	}

	//begine add by hjj 2011-11-3
	int curVert = numVertices - 1 - iterations;
	int lastVert = numVertices - 1;
	int beginVert = curVert;
	int trid = nCurID / 3;
	for (u32 k = 0; k < iterations; k++)
	{
		triangles[trid].pos3 = pVertices[curVert];	
		triangles[trid].pos2 = pVertices[lastVert];
		if (k != iterations-1)
		{
			triangles[trid].pos1= pVertices[++curVert];
		}
		else
		{
			triangles[trid].pos1 = pVertices[beginVert];
		}
	}
	//end add
	//////////////////////////////////////////////////////////////////////////


	//triangles.resize(_Pow_int(4, iterations) * 8);
	//AIR::Vector3Df p[6] = 
	//{
	//	//AIR::Vector3Df(0, 0, 1),	AIR::Vector3Df(0, 0, -1),
	//	//AIR::Vector3Df(-1, -1, 0),  AIR::Vector3Df(1, -1, 0),
	//	//AIR::Vector3Df(1, 1, 0),	AIR::Vector3Df(-1, 1, 0)
	//	AIR::Vector3Df( 0, 1,  0),	AIR::Vector3Df( 0, -1,  0),
	//	AIR::Vector3Df(-1, 0, -1),  AIR::Vector3Df( 1,  0, -1),
	//	AIR::Vector3Df( 1, 0,  1),	AIR::Vector3Df(-1,  0,  1)
	//};
	//AIR::Vector3Df pa, pb, pc;
	//int numTriangles = 0, numOld;

	//// Create the level 0 object
	//f32 a = 1.0f / sqrtf(2.0f);
	//for (u32 i = 0; i < 6; i++)
	//{
	//	p[i].x *= a;
	//	p[i].y *= a;
	//}
	//triangles[0].pos1 = p[0], triangles[0].pos2 = p[3], triangles[0].pos3 = p[4];
	//triangles[1].pos1 = p[0], triangles[1].pos2 = p[4], triangles[1].pos3 = p[5];
	//triangles[2].pos1 = p[0], triangles[2].pos2 = p[5], triangles[2].pos3 = p[2];
	//triangles[3].pos1 = p[0], triangles[3].pos2 = p[2], triangles[3].pos3 = p[3];
	//triangles[4].pos1 = p[1], triangles[4].pos2 = p[4], triangles[4].pos3 = p[3];
	//triangles[5].pos1 = p[1], triangles[5].pos2 = p[5], triangles[5].pos3 = p[4];
	//triangles[6].pos1 = p[1], triangles[6].pos2 = p[2], triangles[6].pos3 = p[5];
	//triangles[7].pos1 = p[1], triangles[7].pos2 = p[3], triangles[7].pos3 = p[2];
	//numTriangles = 8;

	///* Bisect each edge and move to the surface of a unit sphere */
	//for (u32 it = 0; it < iterations; it++)
	//{
	//	numOld = numTriangles;
	//	for (int i = 0; i < numOld; i++)
	//	{
	//		pa = (triangles[i].pos1 + triangles[i].pos2) * 0.5f;
	//		pb = (triangles[i].pos2 + triangles[i].pos3) * 0.5f;
	//		pc = (triangles[i].pos3 + triangles[i].pos1) * 0.5f;
	//		Normalize(pa);
	//		Normalize(pb);
	//		Normalize(pc);
	//		triangles[numTriangles].pos1 = triangles[i].pos1;
	//		triangles[numTriangles].pos2 = pa;
	//		triangles[numTriangles].pos3 = pc;
	//		numTriangles++;
	//		triangles[numTriangles].pos1 = pa;
	//		triangles[numTriangles].pos2 = triangles[i].pos2;
	//		triangles[numTriangles].pos3 = pb;
	//		numTriangles++;
	//		triangles[numTriangles].pos1 = pb;
	//		triangles[numTriangles].pos2 = triangles[i].pos3;
	//		triangles[numTriangles].pos3 = pc;
	//		numTriangles++;
	//		triangles[i].pos1 = pa;
	//		triangles[i].pos2 = pb;
	//		triangles[i].pos3 = pc;
	//	}
	//}
	//std::sort(triangles.begin(), triangles.end(), triangle_compare);

}


bool SceneDebug::triangle_compare(const stTriangle& t1, const stTriangle& t2)
{
	f32 y1 = t1.pos1.y + t1.pos2.y + t1.pos3.y;
	f32 y2 = t2.pos1.y + t2.pos2.y + t2.pos3.y;
	return (y1 < y2);
}

void SceneDebug::RenderLineImpl(bool bZenable, bool bScreenLine, CCamera* pCam, RenderContextQueue* pQueue, eRenderFlow flow)
{
	AIRVector<stLine> & LineArray = bScreenLine ? m_ScreenLines : (bZenable ? m_LinesWithDepth : m_LinesWithoutDepth);
	//IMaterial* pMat = bZenable ? (bScreenLine ? m_pMatScreenLineWithDepth : m_pMatLineWithDepth) : (bScreenLine ? m_pMatLineWithoutDepth : m_pMatLineWithoutDepth );
	IMaterial* pMat = bZenable ? (bScreenLine ? m_pMatScreenWithDepth : m_pMatCommonWithDepth) : (bScreenLine ? m_pMatScreenWithoutDepth : m_pMatCommonWithoutDepth );

	u32 lines = LineArray.size();
	if (lines == 0)
	{
		return;
	}
	IRenderer* pRenderer = ENGINE_INST->GetRenderer();

	Matrix4f worldMat = Matrix4f::IDENTITY;
	Matrix4f matProj = Matrix4f::IDENTITY;
	Matrix4f matView = Matrix4f::IDENTITY;
	pRenderer->SetWorldTransform(&worldMat);

	u32 vertexs = lines * 2;
	if (bScreenLine)
	{
		Vector2D dimensionScreen;
		ENGINE_INST->GetRenderer()->GetBackBufferSize(dimensionScreen);
		Matrix4f matTemp;
		matTemp.Identity();
		matTemp.SetScale(Vector3Df(1.0f, -1.0f, 1.0f));
		//matProj.BuildProjectionMatrixOrthoOffCenterRH(0, dimensionScreen.Width, 0, dimensionScreen.Height, 1, -1);
		matProj.BuildProjectionMatrixOrthoOffCenterLH(0, dimensionScreen.x, 0, dimensionScreen.y, 0.0f, 1000.0f);
		matProj = matProj * matTemp;
		//ENGINE_INST->GetRenderer()->SetProjectionTransform(&matProj);
		matView.BuildCameraLookAtMatrixLH(Vector3Df(0, 0, 10), Vector3Df(0, 0, 1), Vector3Df(0, 1, 0));
		//ENGINE_INST->GetRenderer()->SetViewTransform(&matView);

		if (m_RHWVertexArray.size() < vertexs)
		{
			m_RHWVertexArray.resize(vertexs);
		}
		u32 l = 0, v = 0;
		for (; l < lines; ++l)
		{
			stLine& line = LineArray[l];
			m_RHWVertexArray[v].x = line.pos1.x;
			m_RHWVertexArray[v].y = line.pos1.y;
			m_RHWVertexArray[v].z = line.pos1.z;
			m_RHWVertexArray[v].rhw = 1.0f;
			m_RHWVertexArray[v].color = line.color;
			++v;
			m_RHWVertexArray[v].x = line.pos2.x;
			m_RHWVertexArray[v].y = line.pos2.y;
			m_RHWVertexArray[v].z = line.pos2.z;
			m_RHWVertexArray[v].rhw = 1.0f;
			m_RHWVertexArray[v].color = line.color;
			++v;
		}

		if (m_pVBLineScreen && m_pVBLineScreen->GetVertexCount() < vertexs)
		{
			pRenderer->ClearVertexBuffer(m_pVBLineScreen);
			m_pVBLineScreen = NULL;
		}
		if (!m_pVBLineScreen)
		{
			//m_pVBLineScreen = pRenderer->CreateVertexBuffer(E_VB_RHW_NOTEX, &m_RHWVertexArray[0], vertexs, true);
			m_pVBLineScreen = pRenderer->CreateVertexBuffer( &m_RHWVertexArray[0], vertexs, sizeof(RHWVERTEXNOTEX), true );
		}
		else
		{
			LPRHWVERTEXNOTEX pData = (LPRHWVERTEXNOTEX)m_pVBLineScreen->Lock();
			memcpy( pData, &m_RHWVertexArray[0], vertexs * sizeof(RHWVERTEXNOTEX) );
			m_pVBLineScreen->Unlock();
		}

		if (m_pPrimLineScreen == NULL)
		{
			m_pPrimLineScreen = new Primitive(Primitive::Primitive_LineList/*, m_pVBLineScreen->GetVertexDeclaration()*/);
			m_pPrimLineScreen->m_pSrcRenderable = NULL;
			m_pPrimLineScreen->SetUseUserPrimitive(true);
		}
		m_pPrimLineScreen->SetPrimitiveCount(lines);
		m_pPrimLineScreen->SetVerticesNum(vertexs);
		m_pPrimLineScreen->m_pIndexBuffer = NULL;
		m_pPrimLineScreen->m_pVertexBuffer = m_pVBLineScreen; 

		pQueue->DrawPrimitive( m_pPrimLineScreen, pMat, worldMat, matView, matProj, flow, "E_VB_RHW_NOTEX" );
		//m_pPrimLineScreen->Draw(pMat->GetCurrentTechnique());
	}
	else
	{
		//if (m_VertexArray.size() < vertexs)
		//{
		//	m_VertexArray.resize(vertexs);
		//}
		//u32 l = 0, v = 0;
		//for (; l < lines; ++l)
		//{
		//	stLine& line = LineArray[l];
		//	m_VertexArray[v].position = line.pos1;
		//	m_VertexArray[v].color = line.color;
		//	++v;
		//	m_VertexArray[v].position = line.pos2;
		//	m_VertexArray[v].color = line.color;
		//	++v;
		//}
		pCam->GetViewMatrix(&matView);
		pCam->GetProjectionMatrix(&matProj);
		//ENGINE_INST->GetRenderer()->SetViewTransform(&matView);
		//ENGINE_INST->GetRenderer()->SetProjectionTransform(&matProj);

		if (m_RHWVertexArray.size() < vertexs)
		{
			m_RHWVertexArray.resize(vertexs);
		}
		u32 l = 0, v = 0;
		for (; l < lines; ++l)
		{
			stLine& line = LineArray[l];
			m_RHWVertexArray[v].x = line.pos1.x;
			m_RHWVertexArray[v].y = line.pos1.y;
			m_RHWVertexArray[v].z = line.pos1.z;
			m_RHWVertexArray[v].rhw = 1.0f;
			m_RHWVertexArray[v].color = line.color;
			++v;
			m_RHWVertexArray[v].x = line.pos2.x;
			m_RHWVertexArray[v].y = line.pos2.y;
			m_RHWVertexArray[v].z = line.pos2.z;
			m_RHWVertexArray[v].rhw = 1.0f;
			m_RHWVertexArray[v].color = line.color;
			++v;
		}

		if (m_pVBLine && m_pVBLine->GetVertexCount() < vertexs)
		{
			pRenderer->ClearVertexBuffer(m_pVBLine);
			m_pVBLine = NULL;
		}
		if (!m_pVBLine)
		{
			//m_pVBLine = pRenderer->CreateVertexBuffer(E_VB_NOTEX, &m_VertexArray[0], vertexs, true);
			m_pVBLine = pRenderer->CreateVertexBuffer( &m_RHWVertexArray[0], vertexs, sizeof(RHWVERTEXNOTEX), true );
		}
		else
		{
			//LPVERTEX_NOTEX pData = (LPVERTEX_NOTEX)m_pVBLine->Lock(true);
			//memcpy( pData, &m_VertexArray[0], vertexs * sizeof(VERTEX_NOTEX) );
			//m_pVBLine->Unlock();

			m_pVBLine->WriteData( 0, vertexs * sizeof(RHWVERTEXNOTEX), &m_RHWVertexArray[0], true );
		}

		if (m_pPrimLine == NULL)
		{
			m_pPrimLine = new Primitive(Primitive::Primitive_LineList/*, m_pVBLine->GetVertexDeclaration()*/);
			m_pPrimLine->m_pSrcRenderable = NULL;
			m_pPrimLine->SetUseUserPrimitive(true);
		}
		m_pPrimLine->SetPrimitiveCount(lines);
		m_pPrimLine->SetVerticesNum(vertexs);
		m_pPrimLine->m_pIndexBuffer = NULL;
		m_pPrimLine->m_pVertexBuffer = m_pVBLine; 

		pQueue->DrawPrimitive( m_pPrimLine, pMat, worldMat, matView, matProj, flow, "VS_Pos4_color");
		//m_pPrimLine->Draw(pMat->GetCurrentTechnique());
	}
	
	LineArray.clear();
}


void SceneDebug::RenderTriangleImpl(bool bZenable, bool bScreenTriangle, CCamera* pCam, RenderContextQueue* pQueue, eRenderFlow flow)
{
	AIRVector<stTriangle> & TriArray = bScreenTriangle ? m_TriangleScreen : (bZenable ? m_TriWithDepth : m_TriWithoutDepth);
	//IMaterial* pMat = bZenable ? (bScreenTriangle ? m_pMatScreenTriangleWithDepth : m_pMatTriangleWithDepth) : (bScreenTriangle ? m_pMatScreenTriangleWithoutDepth : m_pMatTriangleWithoutDepth);
	IMaterial* pMat = bZenable ? (bScreenTriangle ? m_pMatScreenWithDepth : m_pMatCommonWithDepth) : (bScreenTriangle ? m_pMatScreenWithoutDepth : m_pMatCommonWithoutDepth);

	u32 prims = TriArray.size();
	if (prims == 0)
	{
		return;
	}

	IRenderer* pRenderer = ENGINE_INST->GetRenderer();
	Matrix4f worldMat = Matrix4f::IDENTITY;
	Matrix4f matProj = Matrix4f::IDENTITY;
	Matrix4f matView = Matrix4f::IDENTITY;
	//pRenderer->SetWorldTransform(&worldMat);

	u32 vertexs = prims * 3;
	if (bScreenTriangle)
	{
		Vector2D dimensionScreen;
		ENGINE_INST->GetRenderer()->GetBackBufferSize(dimensionScreen);
		Matrix4f matTemp;
		matTemp.Identity();
		matTemp.SetScale(Vector3Df(1.0f, -1.0f, 1.0f));
		//matProj.BuildProjectionMatrixOrthoOffCenterRH(0, dimensionScreen.Width, 0, dimensionScreen.Height, 1, -1);
		matProj.BuildProjectionMatrixOrthoOffCenterLH(0, dimensionScreen.x, 0, dimensionScreen.y, 0.0f, 1000.0f);
		matProj = matProj * matTemp;
		//ENGINE_INST->GetRenderer()->SetProjectionTransform(&matProj);
		matView.BuildCameraLookAtMatrixLH(Vector3Df(0, 0, 10), Vector3Df(0, 0, 1), Vector3Df(0, 1, 0));
		//ENGINE_INST->GetRenderer()->SetViewTransform(&matView);

		if (m_RHWVertexArray.size() < vertexs)
		{
			m_RHWVertexArray.resize(vertexs);
		}
		u32 l = 0, v = 0;
		for (; l < prims; ++l)
		{
			stTriangle& tri = TriArray[l];
			m_RHWVertexArray[v].x = tri.pos1.x;
			m_RHWVertexArray[v].y = tri.pos1.y;
			m_RHWVertexArray[v].z = tri.pos1.z;
			m_RHWVertexArray[v].rhw = 1.0f;
			m_RHWVertexArray[v].color = tri.color;
			++v;
			m_RHWVertexArray[v].x = tri.pos2.x;
			m_RHWVertexArray[v].y = tri.pos2.y;
			m_RHWVertexArray[v].z = tri.pos2.z;
			m_RHWVertexArray[v].rhw = 1.0f;
			m_RHWVertexArray[v].color = tri.color;
			++v;
			m_RHWVertexArray[v].x = tri.pos3.x;
			m_RHWVertexArray[v].y = tri.pos3.y;
			m_RHWVertexArray[v].z = tri.pos3.z;
			m_RHWVertexArray[v].rhw = 1.0f;
			m_RHWVertexArray[v].color = tri.color;
			++v;
		}
		if (m_pVBTriangleScreen && m_pVBTriangleScreen->GetVertexCount() < vertexs)
		{
			pRenderer->ClearVertexBuffer(m_pVBTriangleScreen);
			m_pVBTriangleScreen = NULL;
		}
		if (!m_pVBTriangleScreen)
		{
			//m_pVBTriangleScreen = pRenderer->CreateVertexBuffer(E_VB_RHW_NOTEX, &m_RHWVertexArray[0], vertexs, true);
			m_pVBTriangleScreen = pRenderer->CreateVertexBuffer(&m_RHWVertexArray[0], vertexs, sizeof(RHWVERTEXNOTEX), true);
		}
		else
		{
			LPRHWVERTEXNOTEX pData = (LPRHWVERTEXNOTEX)m_pVBTriangleScreen->Lock();
			memcpy( pData, &m_RHWVertexArray[0], vertexs * sizeof(RHWVERTEXNOTEX) );
			m_pVBTriangleScreen->Unlock();
		}

		if (m_pPrimTriangleScreen == NULL)
		{
			m_pPrimTriangleScreen = new Primitive(Primitive::Primitive_TriangleList/*, m_pVBTriangleScreen->GetVertexDeclaration()*/);
			m_pPrimTriangleScreen->m_pSrcRenderable = NULL;
			m_pPrimTriangleScreen->SetUseUserPrimitive(true);
		}
		m_pPrimTriangleScreen->SetPrimitiveCount(prims);
		m_pPrimTriangleScreen->SetVerticesNum(vertexs);
		m_pPrimTriangleScreen->m_pIndexBuffer = NULL;
		m_pPrimTriangleScreen->m_pVertexBuffer = m_pVBTriangleScreen; 

		pQueue->DrawPrimitive( m_pPrimTriangleScreen, pMat, worldMat, matView, matProj, flow, "E_VB_RHW_NOTEX" );
		//m_pPrimTriangleScreen->Draw(pMat->GetCurrentTechnique());
	}
	else
	{
		//if (m_VertexArray.size() < vertexs)
		//{
		//	m_VertexArray.resize(vertexs);
		//}
		//u32 l = 0, v = 0;
		//for (; l < prims; ++l)
		//{
		//	stTriangle& tri = TriArray[l];
		//	m_VertexArray[v].position = tri.pos1;
		//	m_VertexArray[v].color = tri.color;
		//	++v;
		//	m_VertexArray[v].position = tri.pos2;
		//	m_VertexArray[v].color = tri.color;
		//	++v;
		//	m_VertexArray[v].position = tri.pos3;
		//	m_VertexArray[v].color = tri.color;
		//	++v;
		//}

		pCam->GetViewMatrix(&matView);
		pCam->GetProjectionMatrix(&matProj);
		//ENGINE_INST->GetRenderer()->SetViewTransform(&matView);
		//ENGINE_INST->GetRenderer()->SetProjectionTransform(&matProj);

		if (m_RHWVertexArray.size() < vertexs)
		{
			m_RHWVertexArray.resize(vertexs);
		}
		u32 l = 0, v = 0;
		for (; l < prims; ++l)
		{
			stTriangle& tri = TriArray[l];
			m_RHWVertexArray[v].x = tri.pos1.x;
			m_RHWVertexArray[v].y = tri.pos1.y;
			m_RHWVertexArray[v].z = tri.pos1.z;
			m_RHWVertexArray[v].rhw = 1.0f;
			m_RHWVertexArray[v].color = tri.color;
			++v;
			m_RHWVertexArray[v].x = tri.pos2.x;
			m_RHWVertexArray[v].y = tri.pos2.y;
			m_RHWVertexArray[v].z = tri.pos2.z;
			m_RHWVertexArray[v].rhw = 1.0f;
			m_RHWVertexArray[v].color = tri.color;
			++v;
			m_RHWVertexArray[v].x = tri.pos3.x;
			m_RHWVertexArray[v].y = tri.pos3.y;
			m_RHWVertexArray[v].z = tri.pos3.z;
			m_RHWVertexArray[v].rhw = 1.0f;
			m_RHWVertexArray[v].color = tri.color;
			++v;
		}
		if (m_pVBTriangle && m_pVBTriangle->GetVertexCount() < vertexs)
		{
			pRenderer->ClearVertexBuffer(m_pVBTriangle);
			m_pVBTriangle = NULL;
		}
		if (!m_pVBTriangle)
		{
			//m_pVBTriangle = pRenderer->CreateVertexBuffer(E_VB_NOTEX, &m_VertexArray[0], vertexs, true);
			//m_pVBTriangle = pRenderer->CreateVertexBuffer(&m_VertexArray[0], vertexs, sizeof(VERTEX_NOTEX), true);
			m_pVBTriangle = pRenderer->CreateVertexBuffer(&m_RHWVertexArray[0], vertexs, sizeof(RHWVERTEXNOTEX), true);
		}
		else
		{
			//LPVERTEX_NOTEX pData = (LPVERTEX_NOTEX)m_pVBTriangle->Lock();
			//memcpy( pData, &m_VertexArray[0], vertexs * sizeof(VERTEX_NOTEX) );
			//m_pVBTriangle->Unlock();

			m_pVBTriangle->WriteData( 0, vertexs * sizeof(RHWVERTEXNOTEX), &m_RHWVertexArray[0], true );
		}

		if (m_pPrimTriangle == NULL)
		{
			m_pPrimTriangle = new Primitive(Primitive::Primitive_TriangleList/*, m_pVBTriangle->GetVertexDeclaration()*/);
			m_pPrimTriangle->m_pSrcRenderable = NULL;
			m_pPrimTriangle->SetUseUserPrimitive(true);
		}
		m_pPrimTriangle->SetPrimitiveCount(prims);
		m_pPrimTriangle->SetVerticesNum(vertexs);
		m_pPrimTriangle->m_pIndexBuffer = NULL;
		m_pPrimTriangle->m_pVertexBuffer = m_pVBTriangle; 

		pQueue->DrawPrimitive( m_pPrimTriangle, pMat, worldMat, matView, matProj, flow, "E_VB_RHW_NOTEX" );
		//m_pPrimTriangle->Draw(pMat->GetCurrentTechnique());
	}
	
	TriArray.clear();
}


void SceneDebug::Render(CCamera* pCam, RenderContextQueue* pQueue, eRenderFlow flow)
{
	if (!m_bInit)
	{
		Init();
	}
	else
	{
		if (m_bReady)
		{
			RenderLineImpl(		true,	false,	pCam, pQueue, flow);
			RenderTriangleImpl(	true,	false,	pCam, pQueue, flow);

			RenderLineImpl(		false,	false,	pCam, pQueue, flow);
			RenderTriangleImpl(	false,	false,	pCam, pQueue, flow);

			RenderLineImpl(		false,	true,	pCam, pQueue, flow);
			RenderTriangleImpl(	false,	true,	pCam, pQueue, flow);
		}
		else
		{
			Prepare();
		}

        m_LinesWithDepth.clear();
        m_LinesWithoutDepth.clear();
	}
}

void SceneDebug::Prepare()
{
	if (!m_bReady)
	{
		m_bReady = true;
		if (!m_pMatCommonWithDepth->IsReady())
		{
			m_pMatCommonWithDepth->Cache();
			m_bReady = false;
		}
		if (!m_pMatCommonWithoutDepth->IsReady())
		{
			m_pMatCommonWithoutDepth->Cache();
			m_bReady = false;
		}
		if (!m_pMatScreenWithDepth->IsReady())
		{
			m_pMatScreenWithDepth->Cache();
			m_bReady = false;
		}
		if (!m_pMatScreenWithoutDepth->IsReady())
		{
			m_pMatScreenWithoutDepth->Cache();
			m_bReady = false;
		}

	}

}

void SceneDebug::Clear()
{
	MATERIAL_MANAGER->ClearMaterial(m_pMatCommonWithDepth);
	MATERIAL_MANAGER->ClearMaterial(m_pMatCommonWithoutDepth);
	MATERIAL_MANAGER->ClearMaterial(m_pMatScreenWithDepth);
	MATERIAL_MANAGER->ClearMaterial(m_pMatScreenWithoutDepth);

	SafeDelete(m_pPrimLine);
	SafeDelete(m_pPrimTriangle);
	SafeDelete(m_pPrimLineScreen);
	SafeDelete(m_pPrimTriangleScreen);

	IRenderer* pRenderer = ENGINE_INST->GetRenderer();
	pRenderer->ClearVertexBuffer(m_pVBLine);
	pRenderer->ClearVertexBuffer(m_pVBTriangle);
	pRenderer->ClearVertexBuffer(m_pVBLineScreen);
	pRenderer->ClearVertexBuffer(m_pVBTriangleScreen);

	m_bInit = false;
	m_bReady = false;
}
