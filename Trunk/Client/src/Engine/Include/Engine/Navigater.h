// ***************************************************************
//  �ļ�: Navigater.h
//  ����: 2010-7-6
//  ����: huangjunjie
//  ˵��: 
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

#define  PRECISION 0.00000001 //��Χ

using namespace AIR;


// ��������������
class EngineExport Navigater
{
public:
	Navigater( void );

	virtual ~Navigater( void );

	// ��ʼ������
	// vtPolygons : ������б�
	bool							InitData( AIRVector<Polygon3Df>& vtPolygons );

	// ������������
	const AIRVector<Triangle3Df>& CreateNavigater( AIRVector<Polygon3Df>& vtPolygons );

	// ���ĳ���ĳ���Ƿ�ɼ�
	bool							IsVisible( Line3Df& line, const Vector3Df& pt );

	// Ѱ��ĳ�ߵĿɼ���
	bool							FindDT( Line3Df& line, Vector3Df& outDTPoint );

	// ���ĳ�����Ƿ���������ı���
	bool							IsEdge( Line3Df& line );

	// ���
	void							Clear( void );


private:
	AIRVector<Polygon3D>			m_vPolygon;		//������б�
	AIRVector<Vector3D>			m_ptVertex;		//����ζ����
	AIRVector<Line3D>				m_vEdge;		//�߱�
	std::queue <Line3D>			m_qLine;		//�߶���
	AIRVector<Triangle3D>			m_vTriangle;	//���������������б�


};//! end class Navigater

#endif //! __Navigater_H__