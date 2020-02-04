// ***************************************************************
//  文件: NavPathFinder.h
//  日期: 2010-7-6
//  作者: huangjunjie
//  说明: 
// ***************************************************************

#pragma once

#ifndef __NavPathFinder_H__
#define __NavPathFinder_H__

#include "Navigater.h"
#include "MemoryAlloter.h"
#include "MemoryAllocatorConfig.h"

#define CHILD_COUNT 3



// 导航网格三角形
class NavTriangle
{
public:
	NavTriangle( void ) : m_byAttrib(0), m_dwID(-1) {}

	NavTriangle( const Triangle3Df& tri, u8 attrib, u32 id ) : m_Triangle(tri), m_byAttrib(attrib), m_dwID(id) {}

	NavTriangle( const NavTriangle& other ) : m_Triangle(other.m_Triangle), m_byAttrib(other.m_byAttrib), m_dwID(other.m_dwID) {}

	void operator = (const NavTriangle& other) { m_Triangle = other.m_Triangle; m_byAttrib = other.m_byAttrib; m_dwID = other.m_dwID; }

	bool operator == (const NavTriangle& other) { return (m_Triangle == other.m_Triangle && m_byAttrib == other.m_byAttrib && m_dwID == other.m_dwID); }

    //bool operator < (const NavTriangle& other)
    //{
    //    CCamera* pCamera = ENGINE_INST->GetSceneMgr()->GetCamera();

    //    return (m_Triangle.GetCenterOfTriangle() - pCamera->GetPosition()).GetLength() < (other.m_Triangle.GetCenterOfTriangle() - pCamera->GetPosition()).GetLength();
    //}
public:
	u32		m_dwID;		// id
	Triangle3Df m_Triangle;	// 三角形信息
	u8		m_byAttrib; // 0(高度以地面为准), 1(高度以导航面为准)
};



// 导航面链表
class CTriangleLink
{
public:
	CTriangleLink(void)
	{
		m_TriLink[0] = -1;
		m_TriLink[1] = -1;
		m_TriLink[2] = -1;
	}

public:
	int m_TriLink[3];
};



// 路径三角面链表
class CTrianglePath
{
public:
	int x;
	int side;
};



// 路径节点
class CNode// : public EngineAllocator(CNode)
{
	EngineAllocator(CNode)
public:
	CNode(void)
	{
		idx = side = -1;
		f = g = h = 0;
		for (int i = 0 ; i < CHILD_COUNT; i++)
		{
			chil[i]=NULL;
		}
		parent = next = prev = NULL;
	}

public:
	int		idx;		//该节点的标志
	double	f,g,h;		//评估价值f = g + h
	int		side;		//边ID
	CNode*	chil[CHILD_COUNT];	//子节点
	CNode*	parent;		//父节点
	CNode*	next;		//下一个节点
	CNode*	prev;		//上一个节点
};




// 导航网格寻路器
class NavPathFinder
{
public:
	NavPathFinder( void );

	NavPathFinder( const AIRVector<NavTriangle>& triangleList );

	virtual ~NavPathFinder( void );

	// 寻找三角面路径
	const AIRVector<CTrianglePath>&	FindPathOfTriangle( Vector3Df& vStart, Vector3Df& vEnd );

	// 寻找顶点路径
	const AIRVector<Vector3Df>&		FindPathOfLine( Vector3Df& vStart, Vector3Df& vEnd );

	// 清空寻路信息
	bool								Clear();

	// 重置寻路器信息
	void								ResetNavPathFinder( const AIRVector<NavTriangle>& triangleList );

protected:
	// 获得最佳的路径节点
	CNode*	GetBestNode();

	// 插入一个路径节点
	CNode*	Insert(CNode* head, CNode* node);

	// 移除路径节点
	CNode*	Remove(CNode* list, CNode* node);

	// 检查某个节点是否在开表
	CNode*	CheckInOpen(int idx);

	// 检查某个节点是否在闭表
	CNode*	CheckInClose(int idx);

	// A星寻路
	bool	AStar(int nCurr, int tar, Vector3Df& target);

	// 检查某个点是否在导航网格里三角形
	int		GetPointIn(Vector3Df& pt);

	// 成功找到
	void	Successor(int cr, int end, int side, Vector3Df& targ);

	// 获得H值
	double  GetH(int nT , Vector3Df& pt);

	// 获得G值
	double  GetG(int s, int e);

	// 排列
	void	Sort(CNode* list, CNode* node);

protected:
	CNode *						m_pOpen;
	CNode *						m_pClose;
	CNode *						m_pBestNode;
	AIRVector<NavTriangle>	m_vTriangle;
	AIRVector<CTrianglePath>	m_vTPath;
	AIRVector<CTriangleLink>	m_nLink;
	AIRVector<Vector3Df>		m_vtVPath;

};//! end class NavPathFinder

#endif //! __NavPathFinder_H__