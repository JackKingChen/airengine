// ***************************************************************
//  文件: Navigater.h
//  日期: 2010-7-6
//  作者: huangjunjie
//  说明: 
// ***************************************************************

#pragma once

#ifndef __Navigater_H__
#define __Navigater_H__

#include <math.h>
#include <vector>
#include <queue>
#include "EngineConfig.h"
#include "Polygon2D.h"
#include "Vector2D.h"
#include "Line2D.h"
#include "Rect.h"
#include "Triangle2D.h"
#include "Circle2D.h"
#include "Polygon3D.h"
#include "Vector3D.h"
#include "Line3D.h"
#include "Triangle3D.h"

#define  PRECISION 0.00000001 //误差范围

using namespace AIR;


// 导航网格生成器
class EngineExport Navigater
{
public:
	Navigater( void );

	virtual ~Navigater( void );

	// 初始化数据
	// vtPolygons : 多边形列表
	bool							InitData( AIRVector<Polygon3Df>& vtPolygons );

	// 创建导航网格
	const AIRVector<Triangle3Df>& CreateNavigater( AIRVector<Polygon3Df>& vtPolygons );

	// 检查某点对某线是否可见
	bool							IsVisible( Line3Df& line, const Vector3Df& pt );

	// 寻找某线的可见点
	bool							FindDT( Line3Df& line, Vector3Df& outDTPoint );

	// 检查某条线是否多边形网格的边线
	bool							IsEdge( Line3Df& line );

	// 清空
	void							Clear( void );


private:
	AIRVector<Polygon3D>			m_vPolygon;		//多边形列表
	AIRVector<Vector3D>			m_ptVertex;		//多边形顶点表
	AIRVector<Line3D>				m_vEdge;		//边表
	std::queue <Line3D>			m_qLine;		//边队列
	AIRVector<Triangle3D>			m_vTriangle;	//导航网格三角形列表


};//! end class Navigater

#endif //! __Navigater_H__