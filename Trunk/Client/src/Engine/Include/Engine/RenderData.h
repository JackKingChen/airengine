// ***************************************************************
//  文件: RenderData.h
//  日期: 2010-2-22
//  作者: liangairan
//  说明: 渲染用到的数据
// ***************************************************************

#pragma once 

#include "IVertexBuffer.h"
#include "IIndexBuffer.h"


enum RenderType
{
	/// A list of points, 1 vertex per point
	RT_POINT_LIST = 1,
	/// A list of lines, 2 vertices per line
	RT_LINE_LIST = 2,
	/// A strip of connected lines, 1 vertex per line plus 1 start vertex
	RT_LINE_STRIP = 3,
	/// A list of triangles, 3 vertices per triangle
	RT_TRIANGLE_LIST = 4,
	/// A strip of triangles, 3 vertices for the first triangle, and 1 per triangle after that 
	RT_TRIANGLE_STRIP = 5,
	/// A fan of triangles, 3 vertices for the first triangle, and 1 per triangle after that
	RT_TRIANGLE_FAN = 6
};

class EngineExport CRenderData
{
public:
    CRenderData() : m_pVB(NULL)
        , m_pIB(NULL)
        , m_renderType(RT_TRIANGLE_LIST)
        , m_nPrimCount(0)
    {

    }
    virtual ~CRenderData() {};

    IVertexBuffer*  m_pVB;
    IIndexBuffer*   m_pIB;

    RenderType      m_renderType;
    u32             m_nPrimCount;

    void Clear();

protected:
private:
};
