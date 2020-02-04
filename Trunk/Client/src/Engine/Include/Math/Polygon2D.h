// ***************************************************************
//  文件: Polygon2D.h
//  日期: 2010-7-6
//  作者: huangjunjie
//  说明: 
// ***************************************************************

#pragma once

#ifndef __Polygon2D_H__
#define __Polygon2D_H__

#include "MyMath.h"
#include "Vector2D.h"
#include <vector>
#include "Rect.h"
#include "Line2D.h"



namespace AIR
{

	class EngineExport Polygon2D
	{
	public:
		inline Polygon2D( void )
		{

		}

		inline Polygon2D( const AIRVector< Vector2D >& other )
		{
			m_vtVertex.clear();
			m_vtVertex.insert( m_vtVertex.end(), other.begin(), other.end() );
		}

		~Polygon2D( void )
		{ 
			m_vtVertex.clear(); 
		}

		inline const AIRVector< Vector2D >& GetPoints( void ) const
		{
			return m_vtVertex;
		}

		inline AIRVector< Line2D >& GetEdges( void )
		{
			m_vtEdges.clear();
			for (size_t i = 0 ; i < m_vtVertex.size()-1; i++)
			{
				m_vtEdges.push_back( Line2D( m_vtVertex[i],m_vtVertex[i+1] ) );
			}
			m_vtEdges.push_back( Line2D(m_vtVertex[m_vtVertex.size()-1], m_vtVertex[0]) );
			return m_vtEdges;
		}

		inline Rect GetBoundingBox( void )
		{
			double xmin = 1<<23;
			double ymin = 1<<23;
			double xmax = -xmin;
			double ymax = -ymin;
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
			}
			return Rect( xmin, ymin, xmax, ymax );
		}

	private:
		AIRVector< Vector2D > m_vtVertex;
		AIRVector< Line2D > m_vtEdges;

	};//! end class Polygon2D


	typedef Polygon2D	Polygon2Df;
	
};


#endif //! __Polygon2D_H__