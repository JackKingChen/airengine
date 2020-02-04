// ***************************************************************
//  �ļ�: NavPathFinder.h
//  ����: 2010-7-6
//  ����: huangjunjie
//  ˵��: 
// ***************************************************************

#pragma once

#ifndef __NavPathFinder_H__
#define __NavPathFinder_H__

#include "Navigater.h"
#include "MemoryAlloter.h"
#include "MemoryAllocatorConfig.h"

#define CHILD_COUNT 3



// ��������������
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
	Triangle3Df m_Triangle;	// ��������Ϣ
	u8		m_byAttrib; // 0(�߶��Ե���Ϊ׼), 1(�߶��Ե�����Ϊ׼)
};



// ����������
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



// ·������������
class CTrianglePath
{
public:
	int x;
	int side;
};



// ·���ڵ�
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
	int		idx;		//�ýڵ�ı�־
	double	f,g,h;		//������ֵf = g + h
	int		side;		//��ID
	CNode*	chil[CHILD_COUNT];	//�ӽڵ�
	CNode*	parent;		//���ڵ�
	CNode*	next;		//��һ���ڵ�
	CNode*	prev;		//��һ���ڵ�
};




// ��������Ѱ·��
class NavPathFinder
{
public:
	NavPathFinder( void );

	NavPathFinder( const AIRVector<NavTriangle>& triangleList );

	virtual ~NavPathFinder( void );

	// Ѱ��������·��
	const AIRVector<CTrianglePath>&	FindPathOfTriangle( Vector3Df& vStart, Vector3Df& vEnd );

	// Ѱ�Ҷ���·��
	const AIRVector<Vector3Df>&		FindPathOfLine( Vector3Df& vStart, Vector3Df& vEnd );

	// ���Ѱ·��Ϣ
	bool								Clear();

	// ����Ѱ·����Ϣ
	void								ResetNavPathFinder( const AIRVector<NavTriangle>& triangleList );

protected:
	// �����ѵ�·���ڵ�
	CNode*	GetBestNode();

	// ����һ��·���ڵ�
	CNode*	Insert(CNode* head, CNode* node);

	// �Ƴ�·���ڵ�
	CNode*	Remove(CNode* list, CNode* node);

	// ���ĳ���ڵ��Ƿ��ڿ���
	CNode*	CheckInOpen(int idx);

	// ���ĳ���ڵ��Ƿ��ڱձ�
	CNode*	CheckInClose(int idx);

	// A��Ѱ·
	bool	AStar(int nCurr, int tar, Vector3Df& target);

	// ���ĳ�����Ƿ��ڵ���������������
	int		GetPointIn(Vector3Df& pt);

	// �ɹ��ҵ�
	void	Successor(int cr, int end, int side, Vector3Df& targ);

	// ���Hֵ
	double  GetH(int nT , Vector3Df& pt);

	// ���Gֵ
	double  GetG(int s, int e);

	// ����
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