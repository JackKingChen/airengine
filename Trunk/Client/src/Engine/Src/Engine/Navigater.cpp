#include "stdafx.h"
#include "Navigater.h"
#include "DebugTools.h"


Navigater::Navigater( void )
{
}

Navigater::~Navigater( void )
{
	Clear();
}

bool Navigater::InitData( AIRVector<Polygon3Df>& vtPolygons )
{
	// ��¼�������Ϣ
	m_vPolygon = vtPolygons;

	// �������еĶ����
	for (size_t i = 0 ; i < vtPolygons.size(); i++)
	{
		// ��õ�ǰ����εĶ�������
		AIRVector<Vector3Df> tpv = vtPolygons[i].GetPoints();
		if ( tpv.size() == 0 )
		{
			continue;
		}
		m_ptVertex.insert( m_ptVertex.end(), tpv.begin(), tpv.end() );

		// ��õ�ǰ����εı�����
		AIRVector<Line3Df>   tpe = vtPolygons[i].GetEdges();
		m_vEdge.insert( m_vEdge.end(),tpe.begin(),tpe.end() );
	}
	return true;
}


const AIRVector<Triangle3Df>& Navigater::CreateNavigater( AIRVector<Polygon3Df>& vtPolygons )
{
	//���ݳ�ʼ��
	InitData( vtPolygons );

	Line3Df line= m_vEdge[2];

	//���Ա��б�
	AIRVector< Line3Df > stLine;
	stLine.push_back(line);

	while (!stLine.empty())
	{
		// �Ӷ����ҵ�һ���߶�
		line = stLine[stLine.size()-1];
		// �Ѹ��߶δӶ���ɾ��
		stLine.pop_back();

		// Ѱ�Ҹ��߶ε�dt�� 
		Vector3Df dtPoint;
		bool bFind = FindDT(line, dtPoint);
		
		// ����Ҳ������߶ε�DT��, ��ִ����һ��
		if ( !bFind )
		{
			continue;
		}

		m_vTriangle.push_back( Triangle3Df(line.start, line.end, dtPoint) );
		Line3Df l1(line.start, dtPoint);
		Line3Df l2(dtPoint, line.end);

		AIRVector<Line3Df>::iterator it = stLine.begin();

		//�ж��Ƿ���Լ����
		bool flag = false;
		if ( !IsEdge(l1) )
		{
			flag = false;

			//�жϱ��Ƿ��Ѿ��ڶ���
			while (it!=stLine.end())
			{   
				if (l1.IsEqualNotDetectDirection(*it))
				{
					stLine.erase(it);
					flag = true;
					break;
				}
				else
				{
					it++;
				}
			}
			//���ڶ��оͰѸñ߲��뵽����
			if (!flag)
			{
				stLine.push_back(l1);
			}
		}
		it = stLine.begin();
		if (!IsEdge(l2))
		{
			flag = false;
			while(it != stLine.end())
			{
				if (l2.IsEqualNotDetectDirection(*it))
				{
					stLine.erase(it);
					flag = true;
					break;
				}
				else
				{
					it++;
				}
			}
			if (!flag)
			{
				stLine.push_back(l2);
			}
		}

	}
	return m_vTriangle;

}

//����ĳ����߶��Ƿ�ɼ�
bool Navigater::IsVisible( Line3Df& line, const Vector3Df& pt )
{
	//������߶εĶ˵�,��õ㲻�ɼ�
	if( (line.start.x == pt.x && line.start.z == pt.z) || (line.end.x == pt.x && line.end.z == pt.z) )
	{
		return false;
	}
	//�����߶�֮��
	if ( line.HorzRelationShipOfLineAndPoint(pt) != -1 )
	{
		return false;
	}

	Line3Df l1( line.start, pt );
	Line3Df l2( line.end,   pt );
	for (size_t i = 0 ; i < m_vEdge.size(); i++)
	{
		bool bl1 = l1.HorzIsIntersect(m_vEdge[i]);
		if ( bl1 )
		{
			return false;
		}
		if ( l2.HorzIsIntersect(m_vEdge[i]) )
		{
			return false;
		}
	}
	return true;
}


bool Navigater::FindDT( Line3Df& line, Vector3Df& outDTPoint )
{
	AIRVector<Vector3Df> visibleV;
	for (size_t i = 0 ; i < m_ptVertex.size(); i++)
	{  
		Vector3Df tpde = m_ptVertex[i];
		bool isv = IsVisible(line,tpde);
		if ( isv )
		{
			visibleV.push_back(m_ptVertex[i]);
		}
	}

	if ( visibleV.size() == 0)
	{
		DT_TO_MSGBOX_FMT( LT_WARN, GetString("Did not find DT Point for line [(%f, %f, %f), (%f, %f, %f)]", line.start.x, line.start.y, line.start.z, line.end.x, line.end.y, line.end.z) );
		return false;
	}

	Vector3Df ptA	= line.start;
	Vector3Df ptB	= line.end;
	Vector3Df ptC	= visibleV[0];
	bool flag		= true;
	double crp		= 0.0 ;

	while(flag)
	{
		flag = false;
		Triangle3Df tr(ptA,ptB,ptC);
		Circle2Df ci = tr.GetHorzProjTriangle2D().GetCircumCircle();
		Rectf boundingBox = ci.GetBoundingBox();
		Vector2Df veAC( Vector2Df(ptC.x, ptC.z), Vector2Df(ptA.x, ptA.z) );
		Vector2Df veBC( Vector2Df(ptC.x, ptC.z), Vector2Df(ptB.x, ptB.z) );
		crp = veAC.DotProduct(veBC) / (veAC.GetLength() * veBC.GetLength());

		for (size_t i = 0 ; i < visibleV.size(); i++)
		{
			Vector3Df pt = visibleV[i];
			if ( pt.Equals(ptA) || pt.Equals(ptB) || pt.Equals(ptC) )
			{
				continue;
			}

			if ( boundingBox.IsIn( Vector2Df(pt.x, pt.z) ) )
			//if ( ci.IsPointInCircle( Vector2Df(pt.x, pt.z) ) )
			{
				Vector2Df veA( Vector2Df(pt.x, pt.z), Vector2Df(ptA.x, ptA.z) );
				Vector2Df veB( Vector2Df(pt.x, pt.z), Vector2Df(ptB.x, ptB.z) );
				double tmp = (veA.DotProduct(veB)) / (veA.GetLength() * veB.GetLength());
				if (tmp < crp)
				{
					ptC	  = pt;
					crp   = tmp;
					flag  = true;
					break;
				}
			}   
		}
	}
	outDTPoint = ptC;
	return true;  
}


bool Navigater::IsEdge( Line3Df& line )
{
	for (size_t i = 0 ; i < m_vEdge.size(); i++)
	{
		if ( line.IsEqualNotDetectDirection( m_vEdge[i] ) )
		{
			return true;
		}
	}
	return false;
}


void Navigater::Clear( void )
{
	m_vPolygon.clear();
	m_ptVertex.clear();
	m_vEdge.clear();
	while ( !m_qLine.empty() )
	{
		m_qLine.pop();
	}
	m_vTriangle.clear();
}


