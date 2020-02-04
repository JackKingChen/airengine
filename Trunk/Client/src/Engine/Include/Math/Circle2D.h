// ***************************************************************
//  文件: Circle2D.h
//  日期: 2010-7-6
//  作者: huangjunjie
//  说明: 
// ***************************************************************

#pragma once

#ifndef __Circle2D_H__
#define __Circle2D_H__

#include "MyMath.h"
#include "Vector2D.h"
#include "Rect.h"


namespace AIR
{

	class EngineExport Circle2D
	{
	public:
		inline Circle2D( void )
		{

		}

		inline Circle2D( const Vector2D & vCenter, f32 radius )
		{
			m_vCenter = vCenter;
			m_Radius = radius;
		}

		~Circle2D( void )
		{

		}


		inline bool IsPointInCircle( const Vector2D & vCenter )
		{
			double dx = vCenter.x - m_vCenter.x;
			double dy = vCenter.y - m_vCenter.y;
			double dis = dx * dx + dy * dy;
			if ( (dis - m_Radius * m_Radius) > ROUNDING_ERROR_64 )
			{
				return false;
			}
			return true;
		}


		inline Rect GetBoundingBox( void )
		{
			return Rect(
				m_vCenter.x - m_Radius,
				m_vCenter.y - m_Radius,
				m_vCenter.x + m_Radius,
				m_vCenter.y + m_Radius ); 
		}

	public:
		f32				m_Radius;
		Vector2D		m_vCenter;

	};//! end class Circle2D


	//typedef Circle2D<int>	Circle2Di;
	typedef Circle2D Circle2Df;


};



#endif //! __Circle2D_H__