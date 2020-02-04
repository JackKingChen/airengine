#include "stdafx.h"
#include "NavPathFinder.h"
#include "Util.h"
#include "DebugTools.h"




NavPathFinder::NavPathFinder( void )
{
	m_pOpen = NULL;
	m_pClose = NULL;
	m_pBestNode = NULL;
}

NavPathFinder::NavPathFinder( const AIRVector<NavTriangle>& triangleList ) : 
m_vTriangle(triangleList),
//m_nLink(triangleList.size()),
m_pOpen(NULL),
m_pClose(NULL),
m_pBestNode(NULL)
{
	m_nLink.resize( triangleList.size() );
	for ( size_t i = 0; i < m_vTriangle.size(); i++ )
	{
		NavTriangle ti = m_vTriangle[i];
		for (size_t j = 0 ; j < m_vTriangle.size(); j++)
		{
			NavTriangle tj = m_vTriangle[j];
			int n = ti.m_Triangle.SharingSide(tj.m_Triangle);
			if (n != -1)
			{
				m_nLink[i].m_TriLink[n] = j;
			}
		}
	}
}


NavPathFinder:: ~NavPathFinder( void )
{
	Clear();
}


void	NavPathFinder:: ResetNavPathFinder( const AIRVector<NavTriangle>& triangleList )
{
	Clear();

	m_vTriangle.clear();
	m_nLink.reserve( m_vTriangle.size() );

	m_vTriangle = triangleList;
	m_pOpen		= NULL;
	m_pClose	= NULL;
	m_pBestNode = NULL;

	for ( size_t i = 0; i < m_vTriangle.size(); i++ )
	{
		NavTriangle ti = m_vTriangle[i];
		for (size_t j = 0 ; j < m_vTriangle.size(); j++)
		{
			NavTriangle tj = m_vTriangle[j];
			int n = ti.m_Triangle.SharingSide(tj.m_Triangle);
			if (n != -1)
			{
				m_nLink[i].m_TriLink[n] = j;
			}
		}
	}
}

const AIRVector<CTrianglePath>& NavPathFinder::FindPathOfTriangle( Vector3Df& vStart, Vector3Df& vEnd )
{
	m_vTPath.clear();
	int nStart = GetPointIn(vStart);
	int nEnd = GetPointIn(vEnd);

	if ( nStart == -1 )
	{
		//DT_TO_DBGSTR( LT_WARN, GetString("Path start point [%f,%f,%f] is invalid!\n", vStart.x, vStart.y, vStart.z) );
		return m_vTPath;
	}

	if ( nEnd == -1 )
	{
		//DT_TO_DBGSTR( LT_WARN, GetString("Path end point [%f,%f,%f] is invalid!\n", vEnd.x, vEnd.y, vEnd.z) );
		return m_vTPath;
	}

	AStar(nStart,nEnd,vEnd);

	//获得路径并储存在m_vTPath中
	CNode* tmp = m_pBestNode;
	CTrianglePath tpath;
	while (tmp->parent != NULL )
	{
		tpath.x = tmp->idx;
		tpath.side = tmp->side;
		m_vTPath.push_back(tpath); 
		tmp = tmp->parent; 
	}
	tpath.x = tmp->idx;
	tpath.side = -1;
	m_vTPath.push_back(tpath);

	return m_vTPath;
}

const AIRVector<Vector3Df>& NavPathFinder::FindPathOfLine( Vector3Df& vStart, Vector3Df& vEnd )
{
	m_vtVPath.clear();

	if ( m_vTPath.size() == 0 )
	{
		return m_vtVPath;
	}

	m_vtVPath.push_back(vStart);
	Vector3Df ptLside, ptRside;
	Vector3Df veL, veR;
	Vector3Df tmpveL,  tmpveR;
	Vector3Df mid = vStart;
	bool flag = false;
	int idL,idR;
	AIRVector <Vector3Df> vpLPoint,vpRPoint;

	for (size_t i = 1 ; i < m_vTPath.size(); i++)
	{
		Triangle3Df tr = m_vTriangle[m_vTPath[i].x].m_Triangle;//获得下一个三角形
		int side = m_vTPath[i-1].side; //获得穿过边
		Vector3Df pt1,pt2;
		//获得点的传入边pt1,pt2
		if ( side == 0 )
		{
			pt1 = tr.pointA;
			pt2 = tr.pointB;
		}
		else if ( side == 1 )
		{
			pt1 = tr.pointB;
			pt2 = tr.pointC;
		}
		else if ( side == 2 )
		{
			pt1 = tr.pointC;
			pt2 = tr.pointA;
		}
		Vector3Df ve1(mid,pt1);
		Vector3Df ve2(mid,pt2);
		mid.x = (pt1.x + pt2.x)/2;
		mid.y = (pt1.y + pt2.y)/2;
		mid.z = (pt1.z + pt2.z)/2;

		Vector2Df projVe1( ve1.x, ve1.z );
		Vector2Df projVe2( ve2.x, ve2.z );

		double d1 = projVe1.CrossProduct(projVe2);

		if( d1 > 0 )
		{
			vpLPoint.push_back(pt1);
			vpRPoint.push_back(pt2);
		}
		else 
		{
			vpLPoint.push_back(pt2);
			vpRPoint.push_back(pt1);
		}
	}

	for (size_t i = 0 ; i <= vpLPoint.size(); i++)
	{
		if ( i == vpLPoint.size())
		{
			if (!flag)
			{
				continue;
			}

			Vector3Df vend(vStart,vEnd);
			Vector2Df projVeR( veR.x, veR.z );
			Vector2Df projVend( vend.x, vend.z );
			Vector2Df projVeL( veL.x, veL.z );

			if ( projVeR.CrossProduct(projVend) > 0 )
			{
				m_vtVPath.push_back(ptRside);
				vStart = ptRside;
				flag = false;
				i = idR;
			}
			else if (projVeL.CrossProduct(projVend) < 0)
			{
				m_vtVPath.push_back(ptLside);
				vStart = ptLside;
				flag = false;
				i = idL;
			}
			continue;
		}
		Vector3Df ptL,ptR;
		ptL = vpLPoint[i];
		ptR = vpRPoint[i];
		tmpveL = Vector3Df(vStart,ptL);
		tmpveR = Vector3Df(vStart,ptR);
		if (flag)
		{
			Vector2Df projVeR( veR.x, veR.z );
			Vector2Df projVeL( veL.x, veL.z );
			Vector2Df projTmpVeL( tmpveL.x, tmpveL.z );
			Vector2Df projTmpVeR( tmpveR.x, tmpveR.z );

			if ( projVeR.CrossProduct(projTmpVeL) > 0 ) //右拐点
			{
				vStart = ptRside;
				m_vtVPath.push_back(vStart);
				flag = false;
				i = idR;
				continue;
			}
			else if ( projVeL.CrossProduct(projTmpVeR) < 0 )//左拐点
			{
				vStart = ptLside;
				m_vtVPath.push_back(vStart);
				i = idL;
				flag = false;
				continue;
			}

			double l = projVeL.CrossProduct(projTmpVeL);
			if ( l >= 0 )
			{
				veL = tmpveL;
				ptLside = ptL;
				idL = i;
			}

			double r = projVeR.CrossProduct(projTmpVeR);
			if (r <= 0)
			{
				veR = tmpveR;
				ptRside = ptR;
				idR = i;
			}
		}
		else if (!flag)
		{
			veL = tmpveL;
			veR = tmpveR;
			ptLside = ptL;
			ptRside = ptR;
			flag = true;
			idL = idR = i;
		}

	}

	m_vtVPath.push_back(vEnd);
	return m_vtVPath;
}




bool NavPathFinder::Clear()
{
	CNode * tmp,*tmp1;
	if (m_pOpen != NULL)
	{
		tmp = m_pOpen;
		tmp1 = m_pOpen->next;
		while ( tmp != NULL )
		{   
			SafeDelete(tmp);
			tmp = tmp1;
			if (tmp1 == NULL)
			{
				break;
			}
			tmp1 = tmp1->next;
		}
	}

	if (m_pClose != NULL)
	{
		tmp = m_pClose;
		tmp1 = m_pClose->next;
		while ( tmp != NULL )
		{
			SafeDelete(tmp);
			tmp = tmp1;
			if (tmp1 == NULL)
			{
				break;
			}
			tmp1 = tmp1->next;
		}
	}

	m_pOpen = NULL;
	m_pClose = NULL;
	m_pBestNode = NULL;
	return true;
}


CNode* NavPathFinder::GetBestNode()
{
	CNode* BestNode;
	if (m_pOpen->next == NULL)
	{
		return NULL;
	}
	//获得第一个节点并且把其从Open列表中删除
	BestNode = m_pOpen->next;
	Remove(m_pOpen , BestNode);   

	//把节点加入到Close列表中
	Insert(m_pClose,BestNode);
	return BestNode;
}

CNode* NavPathFinder::Insert(CNode* head, CNode* node)
{
	if ( head->next == NULL )
	{
		head->next = node;
		node->prev = head;
	}
	else
	{   
		node->next = head->next ;
		head->next->prev = node;
		head->next = node;
		node->prev = head;
	}
	return node;
}

CNode* NavPathFinder::Remove(CNode* list, CNode* node)
{
	node->prev->next = node->next;
	if ( node->next!=NULL )
	{
		node->next->prev = node->prev ;
	}
	node->next = node->prev = NULL;
	return node;
}

CNode* NavPathFinder::CheckInOpen(int idx)
{
	CNode* tmp;
	tmp = m_pOpen;
	while ( tmp->next != NULL )
	{
		tmp = tmp->next;
		if ( tmp->idx == idx )
		{
			return tmp;
		}

	}
	return NULL;
}

CNode* NavPathFinder::CheckInClose(int idx)
{
	CNode* tmp;
	tmp = m_pClose;
	while ( tmp->next != NULL )
	{
		tmp = tmp->next;
		if ( tmp->idx == idx )
		{
			return tmp;
		}

	}
	return NULL;
}

bool NavPathFinder::AStar(int nCurr, int tar, Vector3Df& target)
{
	//建立Open和Close列表
	m_pOpen = new CNode();
	m_pClose = new CNode();

	CNode* node = new CNode();
	//当前代价
	node->g = 0;
	//评估值
	node->h = GetH(nCurr,target);
	node->f = node->g + node->h;
	node->idx = nCurr;

	//加入第一个节点到Open队列中
	Insert(m_pOpen,node);
	for (;;)
	{
		m_pBestNode = GetBestNode();

		if ( m_pBestNode == NULL )
		{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
			MessageBox(NULL, "shit","Debug", MB_OK);
#endif
		}

		//已经找到了目标
		if ( m_pBestNode->idx == tar)
		{
			break;
		}
		//三角形的三个方向寻找
		for (int i = 0 ; i < 3 ; i++)
		{
			if ( m_nLink[m_pBestNode->idx].m_TriLink[i] != -1 )
			{
				Successor(m_nLink[m_pBestNode->idx].m_TriLink[i],
					tar,i,target);
			}

		}

	}
	return true;
}

int NavPathFinder::GetPointIn(Vector3Df& pt)
{
	for (size_t i = 0 ; i < m_vTriangle.size() ; i++)
	{
		if ( m_vTriangle[i].m_Triangle.GetHorzProjTriangle2D().IsInTriangle( Vector2Df(pt.x, pt.z) ) )
		{
			return i;
		}
	}
	return -1;
}

void NavPathFinder::Successor(int cr, int end, int side, Vector3Df& targ)
{
	CNode* old;
	CNode* pSu;
	int i; 
	double g = m_pBestNode->g + GetG(m_pBestNode->idx,cr);
	old = CheckInOpen(cr);
	if ( old != NULL )//在Open列表中
	{
		for (i = 0 ; i < CHILD_COUNT ; i++)
		{
			if( m_pBestNode->chil[i] == NULL)
				break;
		}
		m_pBestNode->chil[i] = old;
		if (g < old->g)
		{
			old->parent = m_pBestNode;
			old->g = g;
			old->side = side;
			old->f = old->g + old->h;
			Sort(m_pOpen,old);
		}
	}
	//在Close列表中
	else if ( (old = CheckInClose(cr) ) != NULL )
	{
		for (i = 0 ; i < CHILD_COUNT ; i++)
		{
			if (m_pBestNode->chil[i] == NULL)
				break;
		}
		m_pBestNode->chil[i] = old;
		if (g < old->g)
		{
			old->parent = m_pBestNode;
			old->g = g;
			old->side = side;
			old->f = old->g + old->h; 
			Remove(m_pClose,old);
			Insert(m_pOpen,old);
			Sort(m_pOpen,old);
		}

	}
	else//都不在
	{
		pSu = new CNode();

		pSu->g = g;
		pSu->h = GetH(cr,targ);
		pSu->f = g+ pSu->h;
		pSu->parent = m_pBestNode;
		pSu->idx = cr;
		pSu->side = side;
		Insert(m_pOpen, pSu);
		for (i = 0 ; i < CHILD_COUNT ; i++)
		{
			if (m_pBestNode->chil[i] == NULL)
				break;
		}
		m_pBestNode->chil[i] = pSu;
		Sort(m_pOpen,pSu);
	}
}

double  NavPathFinder::GetH(int nT , Vector3Df& pt)
{
	Vector3Df cen = m_vTriangle[nT].m_Triangle.GetCenterOfTriangle();
	Vector3Df ve(cen,pt);
	return ve.GetLength(); 
}

double  NavPathFinder::GetG(int s, int e)
{
	Vector3Df cs = m_vTriangle[s].m_Triangle.GetCenterOfTriangle();
	Vector3Df ce = m_vTriangle[e].m_Triangle.GetCenterOfTriangle();

	Vector3Df ve(cs,ce);
	return ve.GetLength();
}

void NavPathFinder::Sort(CNode* list, CNode* node)
{
	//从List中删除node;
	node->prev->next = node->next;
	if ( node->next!=NULL )
	{
		node->next->prev = node->prev ;
	}
	node->next = node->prev = NULL;

	//找到node所在的位置并将其插入到该位置上
	CNode* temp;
	temp = list;
	while ( temp->next != NULL )
	{

		if ( temp->next->f >= node->f )//找到了位置在temp后面面插入
		{
			break;
		}
		temp = temp->next;
	}

	//将node 加入到list的temp之后
	if ( temp ->next == NULL )
	{
		temp->next = node;
		node->prev = temp;
	}
	else
	{
		node->next = temp->next;
		temp->next->prev = node;
		temp->next = node;
		node->prev = temp;
	}
}

