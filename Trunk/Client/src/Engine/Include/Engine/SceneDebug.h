//------------------------------------------------------------------------------
/**
    @class	SceneDebug

	@brief	场景渲染调试工具

    @date	2012-7-11

	@author HJJ
*/
//------------------------------------------------------------------------------

#pragma once

#ifndef __SceneDebug_H__
#define __SceneDebug_H__

#include "EngineConfig.h"
#include "AIRTypes.h"
#include "Util.h"
#include "Vector3D.h"
#include "Color.h"
#include "Rect.h"
#include "Matrix4.h"
#include "Ray.h"
#include "AABBBox3D.h"
#include "Obb.h"
#include "VertexDef.h"
#include "MemoryAllocatorConfig.h"
#include "Singleton.h"
#include "EngineDefination.h"
#include "FixedString.h"

class Primitive;
class IMaterial;
class IVertexBuffer;
class CCamera;
class RenderContextQueue;
class CVertexDeclaration;

struct stLine
{
	AIR::Vector3Df	pos1;
	AIR::Vector3Df	pos2;
	SColor		color;
};

struct stTriangle
{
	AIR::Vector3Df	pos1;
	AIR::Vector3Df	pos2;
	AIR::Vector3Df	pos3;
	SColor		color;
};


class EngineExport SceneDebug : public Singleton<SceneDebug>
{
	BE_SINGLETON(SceneDebug);
public:
	~SceneDebug();

	void RenderRay(AIR::Ray &ray,bool zEnabled = false);
	void RenderLine(const AIR::Vector3Df &pos1, const AIR::Vector3Df &pos2, SColor color,bool zEnabled = false);
	void RenderLineScreen(const AIR::Vector3Df &pos1, const AIR::Vector3Df &pos2, SColor color);
	void RenderRectScreen(const AIR::Rect& rect, const SColor& color, bool solid);
	void RenderTriangleScreen(const AIR::Vector3Df &pos1, const AIR::Vector3Df &pos2, const AIR::Vector3Df &pos3, const SColor& color);
	void RenderTriangle(const AIR::Vector3Df &pos1, const AIR::Vector3Df &pos2, const AIR::Vector3Df &pos3, const SColor& color, bool zEnabled = true);
	void RenderCircle(const AIR::Vector3Df& center, const AIR::Vector3Df& normal, AIR::f32 radius, const SColor& color, bool zEnabled = true);
	void RenderSphere(const AIR::Vector3Df& center, AIR::f32 radius, const SColor& color, bool zEnabled = true);
	void RenderAABB(const AIR::AABBBox3Df& aabb, const SColor& color, bool zEnabled = false);
	void RenderAABB(const AIR::AABBBox3Df& aabb, const AIR::Matrix4f& transform, const SColor& color, bool zEnabled = false);
	void RenderOBB(const AIR::AABBBox3Df& aabb, const AIR::Matrix4f& xform, const SColor& color, bool zEnabled = false);
	void RenderOBB(const AIR::OBBoxf& obb, const SColor& color, bool zEnabled = false );
	void RenderBox(const AIR::AABBBox3Df& aabb, const SColor& color, bool zEnabled = true);
	void RenderBox(const AIR::AABBBox3Df& aabb, const AIR::Matrix4f& xform, const SColor& color, bool zEnabled = true);
	void RenderBox(const AIR::Vector3Df points[8], const SColor& color, bool zEnabled = true);
	void RenderCone(const AIR::Vector3Df& pos, AIR::f32 angle, AIR::f32 height, const AIR::Vector3Df& dir, const SColor& color, bool zEnabled = true);
	void RenderCapsule(const AIR::Vector3Df& a, const AIR::Vector3Df& b, AIR::f32 radius, const SColor& color, bool zEnabled = true);
	void RenderCylinder(const AIR::Vector3Df& a, const AIR::Vector3Df& b, AIR::f32 radius, const SColor& color, bool zEnabled = true);

	void GetBoxEdgeLines(AIRVector<stLine>&lines, const AIR::Vector3Df &minpos, const AIR::Vector3Df &maxpos, SColor color);
	void GetCirclePoints(AIRVector<AIR::Vector3Df>& points, const AIR::Vector3Df& center, const AIR::Vector3Df& normal, AIR::f32 radius);
	void GetSphereTriangles(AIRVector<stTriangle>& triangles, AIR::u32 iterations);

	
	void Clear();
	void Init();

	void Render(CCamera* pCam, RenderContextQueue* pQueue, eRenderFlow flow);

private:
	static bool triangle_compare(const stTriangle& t1, const stTriangle& t2);

	void RenderLineImpl(bool bZenable, bool bScreenLine, CCamera* pCam, RenderContextQueue* pQueue, eRenderFlow flow);
	void RenderTriangleImpl(bool bZenable, bool bScreenTriangle, CCamera* pCam, RenderContextQueue* pQueue, eRenderFlow flow);
	void Prepare();

private:
	AIRVector<stLine>		m_LinesWithDepth;
	AIRVector<stLine>		m_LinesWithoutDepth;
	AIRVector<stLine>		m_ScreenLines;
	AIRVector<stTriangle>	m_TriWithDepth;
	AIRVector<stTriangle>	m_TriWithoutDepth;
	AIRVector<stTriangle>	m_TriangleScreen;
	AIRVector<stTriangle>   m_Sphere;

	//AIRVector<VERTEX_NOTEX> m_VertexArray;
	AIRVector<RHWVERTEXNOTEX> m_RHWVertexArray;

	IMaterial* m_pMatCommonWithDepth;
	IMaterial* m_pMatCommonWithoutDepth;
	IMaterial* m_pMatScreenWithDepth;
	IMaterial* m_pMatScreenWithoutDepth;

	Primitive* m_pPrimLine;
	Primitive* m_pPrimTriangle;
	Primitive* m_pPrimLineScreen;
	Primitive* m_pPrimTriangleScreen;

	IVertexBuffer* m_pVBLine;
	IVertexBuffer* m_pVBTriangle;
	IVertexBuffer* m_pVBLineScreen;
	IVertexBuffer* m_pVBTriangleScreen;
	//CVertexDeclaration* m_pVertexDecl;
	bool m_bReady;
	bool m_bInit;

	//FixedString m_strVertexDecl;
};

#define SCENE_DEBUGER SceneDebug::GetInstance()

#endif //__SceneDebug_H__