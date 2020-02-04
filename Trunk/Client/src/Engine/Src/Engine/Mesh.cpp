#include "Mesh.h"
#include "SceneDebug.h"
#include "Engine.h"
#include "MathUtil.h"

IMPLEMENT_RTTI(Mesh, IReference);

Mesh::Mesh(CustomMeshType meshType) : m_Type(meshType)
, m_pVB(NULL)
, m_pIB(NULL)
{

}

Mesh::~Mesh()
{

}

void Mesh::BuildVertexData()
{
	switch (m_Type)
	{
	case eNone:
		break;
	case eBox:
		break;
	case eSphere:
		BuildSphereVertexData();
		break;
	case ePlane:
		break;
	case eCylinder:
		break;
	default:
		break;
	}
}

void Mesh::BuildBoxVertexData()
{
}

void Mesh::BuildCylinderVertexData()
{

}

void Mesh::BuildPlaneVertexData()
{

}

void Mesh::BuildSphereVertexData()
{
	AIRVector<stTriangle> triangles;
	u32 iterations = 5;

	u32 numVertices = iterations * iterations + 2;
	u32 numEle = 6 * iterations * (iterations - 1) + 3 * iterations * 2;
	u32 primityCount = numEle / 3;
	u32 nLastVert = numVertices - 1;
	AIRVector<Vector3Df> pVertices;
	pVertices.resize(numVertices);

	pVertices[0].x = 0;  pVertices[0].z = 0;	 pVertices[0].y = 1.0f;
	pVertices[nLastVert].x = 0;  pVertices[nLastVert].z = 0;	 pVertices[nLastVert].y = -1.0f;
	float AngleStep = AIR::PI / (iterations + 1);
	float r, uvr, c, s, sc;
	u32   x, y;
	float dirAngle;
	for (y = 0; y < iterations; ++y)
	{
		dirAngle = AIR::HALF_PI - AngleStep * (y + 1);
		r = CMathUtil::Cos(dirAngle);
		uvr = static_cast<float>(y + 1) / (iterations + 1);

		for (x = 0; x < iterations; ++x)
		{
			c = CMathUtil::Cos(AIR::TWO_PI * x / iterations) * r;
			s = CMathUtil::Sin(AIR::TWO_PI * x / iterations) * r;
			sc = CMathUtil::Sin(dirAngle);

			u32 id = 1 + y*iterations + x;
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
		triangles[k].pos3 = pVertices[k + 1];

		if (k != iterations - 1)
		{
			triangles[k].pos2 = pVertices[k + 2];
		}
		else
		{
			triangles[k].pos2 = pVertices[1];
		}

		nCurID += 3;
	}

	for (u32 y = 0; y<iterations - 1; y++)
	{
		for (u32 x = 0; x<iterations; x++)
		{
			//u16 *twoface = indexbuffer + (y*iterations+x)*6 + 3 * iterations;
			u32 tri = ((y*iterations + x) * 6 + 3 * iterations) / 3;

			u16 p0 = 1 + y * iterations + x;
			u16 p1 = 1 + y * iterations + x + 1;
			u16 p2 = 1 + (y + 1)* iterations + x;
			u16 p3 = 1 + (y + 1)* iterations + x + 1;

			if (x == iterations - 1)
			{
				p1 -= x + 1;
				p3 -= x + 1;
			}

			// First triangle
			triangles[tri].pos1 = pVertices[p0];
			triangles[tri].pos2 = pVertices[p1];
			triangles[tri].pos3 = pVertices[p2];

			++tri;
			// Second triangle
			triangles[tri].pos1 = pVertices[p2];
			triangles[tri].pos2 = pVertices[p1];
			triangles[tri].pos3 = pVertices[p3];

			nCurID += 6;
		}
	}

	//begine add by hjj 2011-11-3
	int curVert = numVertices - 1 - iterations;
	int lastVert = numVertices - 1;
	int beginVert = curVert;
	u32 trid = nCurID / 3;
	for (u32 k = 0; k < iterations; k++)
	{
		triangles[trid].pos3 = pVertices[curVert];
		triangles[trid].pos2 = pVertices[lastVert];
		if (k != iterations - 1)
		{
			triangles[trid].pos1 = pVertices[++curVert];
		}
		else
		{
			triangles[trid].pos1 = pVertices[beginVert];
		}
	}

	if (m_meshVertices.size() < numVertices)
	{
		m_meshVertices.resize(numVertices);
	}
	u32 l = 0, v = 0;
	u32 prims = triangles.size();
	for (; l < prims; ++l)
	{
		stTriangle& tri = triangles[l];
		m_meshVertices[v].position.x = tri.pos1.x;
		m_meshVertices[v].position.y = tri.pos1.y;
		m_meshVertices[v].position.z = tri.pos1.z;
		m_meshVertices[v].color = tri.color;
		++v;
		m_meshVertices[v].position.x = tri.pos2.x;
		m_meshVertices[v].position.y = tri.pos2.y;
		m_meshVertices[v].position.z = tri.pos2.z;

		m_meshVertices[v].color = tri.color;
		++v;
		m_meshVertices[v].position.x = tri.pos3.x;
		m_meshVertices[v].position.y = tri.pos3.y;
		m_meshVertices[v].position.z = tri.pos3.z;
		m_meshVertices[v].color = tri.color;
		++v;
	}

	IRenderer* pRenderer = ENGINE_INST->GetRenderer();
	if (m_pVB && m_pVB->GetVertexCount() < numVertices)
	{
		pRenderer->ClearVertexBuffer(m_pVB);
		m_pVB = NULL;
	}
	if (!m_pVB)
	{
		m_pVB = pRenderer->CreateVertexBuffer(&m_meshVertices[0], numVertices, sizeof(VERTEX_NOTEX), true);
	}
	else
	{
		//LPVERTEX_NOTEX pData = (LPVERTEX_NOTEX)m_pVBTriangle->Lock();
		//memcpy( pData, &m_VertexArray[0], vertexs * sizeof(VERTEX_NOTEX) );
		//m_pVBTriangle->Unlock();

		m_pVB->WriteData(0, numVertices * sizeof(VERTEX_NOTEX), &m_meshVertices[0], true);
	}
}
