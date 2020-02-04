// ***************************************************************
//  文件: Polygon3D.h
//  日期: 2010-7-7
//  作者: huangjunjie
//  说明: 多边形类
// ***************************************************************

#pragma once

#ifndef __Polygon3D_H__
#define __Polygon3D_H__

#include "MyMath.h"
#include "Vector3D.h"
#include "AABBBox3D.h"
#include <vector>

namespace AIR
{
	class EngineExport Polygon3D
	{
	public:
		inline Polygon3D( void ) { }

		inline Polygon3D( const AIRVector< Vector3Df >& vtVertexs )
		{
			m_vtVertex = vtVertexs;
		}

		inline Polygon3D( const Polygon3D& other )
		{
			m_vtVertex = other.m_vtVertex;
		}

		~Polygon3D( void ) {m_vtVertex.clear();}


		inline void operator = (const Polygon3D& other)
		{
			m_vtVertex = other.m_vtVertex;
		}

		inline const AIRVector< Vector3Df >& GetPoints( void ) const
		{
			return m_vtVertex;
		}

		//取得多边形的边表
		inline AIRVector< Line3D >& GetEdges( void )
		{
			m_vtEdges.clear();
			for (size_t i = 0 ; i < m_vtVertex.size()-1; i++)
			{
				m_vtEdges.push_back( Line3D( m_vtVertex[i],m_vtVertex[i+1] ) );
			}
			m_vtEdges.push_back( Line3D(m_vtVertex[m_vtVertex.size()-1], m_vtVertex[0]) );
			return m_vtEdges;
		}

		//取得多边形的AABB
		inline AABBBox3Df GetAABB( void ) 
		{
			double xmin = 1<<23;
			double ymin = 1<<23;
			double zmin = 1<<23;
			double xmax = -xmin;
			double ymax = -ymin;
			double zmax = -zmin;
			for (size_t i = 0 ; i < m_vtVertex.size(); i++)
			{
				if (m_vtVertex[i].x > xmax)
				{
					xmax = m_vtVertex[i].x;
				}
				if (m_vtVertex[i].x < xmin)
				{
					xmin = m_vtVertex[i].x;
				}
				if (m_vtVertex[i].y > ymax)
				{
					ymax = m_vtVertex[i].y;
				}
				if (m_vtVertex[i].y < ymin)
				{
					ymin = m_vtVertex[i].y;
				}
				if (m_vtVertex[i].z > zmax)
				{
					zmax = m_vtVertex[i].z;
				}
				if (m_vtVertex[i].z < zmin)
				{
					zmin = m_vtVertex[i].z;
				}
			}
			return AABBBox3Df( xmin, ymin, zmin, xmax, ymax, zmax );
		}



	public:
		AIRVector< Vector3Df > m_vtVertex;
		AIRVector< Line3D >	m_vtEdges;

	};//! end class Polygon3D



	typedef Polygon3D	Polygon3Df;



};



#endif //! __Polygon3D_H__