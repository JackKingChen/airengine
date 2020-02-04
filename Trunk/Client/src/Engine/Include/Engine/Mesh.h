#pragma once


#include "EngineConfig.h"
#include "Common.h"
#include "RTTI.h"
#include "EngineResource.h"
#include "VertexDef.h"

class IVertexBuffer;
class IIndexBuffer;

enum CustomMeshType
{
	eNone,
	eBox,
	eSphere,
	ePlane,
	eCylinder,
	eCustom,  //通过文件加载
};

class EngineExport Mesh : public Resource
{
	DECLARE_RTTI
public:
	Mesh(CustomMeshType meshType);
	virtual ~Mesh();

	static Mesh* CreateMesh(CustomMeshType type);

	void Clear();
private:
	CustomMeshType m_Type;

	IVertexBuffer* m_pVB;
	IIndexBuffer* m_pIB;

	void BuildVertexData();

	void BuildBoxVertexData();

	void BuildSphereVertexData();

	void BuildPlaneVertexData();

	void BuildCylinderVertexData();

	AIRVector<VERTEX_NOTEX> m_meshVertices;
};

