#include "stdafx.h"
#include "NavEditor.h"
#include "Engine.h"
#include "MaterialManager.h"
#include "VertexDeclarationManager.h"
#include "ISceneManager.h"
#include "OTSceneManager.h"
#include "Camera.h"
#include "FileSystem.h"
#include "Terrain.h"
#include "HeightMap.h"
#include "MemoryDataStream.h"
#include <fstream>
#include "Primitive.h"
//#include "Technique.h"

// ���3�����е����ֵ
template< class T >
T GetMax( T a, T b, T c )
{
	T t = AIR::Max( a, b );
	t = AIR::Max( t, c );
	return t;
}

// ���3�����е���Сֵ
template< class T >
T GetMin( T a, T b, T c )
{
	T t = AIR::Min( a, b );
	t = AIR::Min( t, c );
	return t;
}


// �ж�2���߶��Ƿ��ཻ
bool IsLineSegIntersect( 
	float line1pStartX, 
	float line1pStartY, 
	float line1pEndX, 
	float line1pEndY,
	float line2pStartX,
	float line2pStartY,
	float line2pEndX,
	float line2pEndY )
{
	
	Rectf re1( Vector2Df(line1pStartX, line1pStartY), Vector2Df(line1pEndX, line1pEndY) );
	Rectf re2( Vector2Df(line2pStartX, line2pStartY), Vector2Df(line2pEndX, line2pEndY) );
	re1.Normalize();
	re2.Normalize();

	if( !re1.IsCross(re2) )
	{
		return false;
	}

	// �໥����
	Vector2Df ve1 ( Vector2Df(line1pStartX, line1pStartY), Vector2Df(line1pEndX,   line1pEndY) );
	Vector2Df ve11( Vector2Df(line1pStartX, line1pStartY), Vector2Df(line2pStartX, line2pStartY) );
	Vector2Df ve12( Vector2Df(line1pStartX, line1pStartY), Vector2Df(line2pEndX,   line2pEndY) );
	double cro1 = ve1.CrossProduct(ve12) * ve1.CrossProduct(ve11);

	Vector2Df ve2 ( Vector2Df(line2pStartX, line2pStartY), Vector2Df(line2pEndX,   line2pEndY) );
	Vector2Df ve21( Vector2Df(line2pStartX, line2pStartY), Vector2Df(line1pStartX, line1pStartY) );
	Vector2Df ve22( Vector2Df(line2pStartX, line2pStartY), Vector2Df(line1pEndX,   line1pEndY) );
	double cro2 = ve2.CrossProduct(ve22) * ve2.CrossProduct(ve21);

	if ( cro1 <= 0 && cro2 <= 0)
	{
		return true;
	}
	return false;
}



// �жϾ������������Ƿ��ཻ
bool IsRectIntersectTriangle( Rectf& rc, Triangle2Df& tri )
{
	// ���������εľ��ΰ�Χ��
	Rectf triBB;
	triBB.left   = GetMin( tri.m_pt0.x, tri.m_pt1.x, tri.m_pt2.x );
	triBB.top    = GetMin( tri.m_pt0.y, tri.m_pt1.y, tri.m_pt2.y );
	triBB.right  = GetMax( tri.m_pt0.x, tri.m_pt1.x, tri.m_pt2.x );
	triBB.bottom = GetMax( tri.m_pt0.y, tri.m_pt1.y, tri.m_pt2.y );

	// ��������εľ��ΰ�Χ�к;�������,ֱ�ӷ���false
	if ( !rc.IsCross( triBB ) )
	{
		return false;
	}

	// �������������һ�������ھ�����,ֱ�ӷ���true
	if ( rc.IsIn( tri.m_pt0 ) || rc.IsIn( tri.m_pt1 ) || rc.IsIn( tri.m_pt2 ) )
	{
		return true;
	}

	// ������ε�����һ����������������,ֱ�ӷ���true
	if ( tri.IsInTriangle( Vector2Df(rc.left,  rc.top ) ) ||
		 tri.IsInTriangle( Vector2Df(rc.right, rc.top ) ) ||
		 tri.IsInTriangle( Vector2Df(rc.right, rc.bottom ) ) ||
		 tri.IsInTriangle( Vector2Df(rc.left,  rc.bottom ) ) )
	{
		return true;
	}

	// ���ε�4���߶�
	Vector2Df rcLineSeg[8] = 
	{
		Vector2Df( rc.left,  rc.top ),    Vector2Df( rc.right, rc.top ),
		Vector2Df( rc.right, rc.top ),    Vector2Df( rc.right, rc.bottom ),
		Vector2Df( rc.right, rc.bottom ), Vector2Df( rc.left,  rc.bottom ),
		Vector2Df( rc.left,  rc.bottom ), Vector2Df( rc.left,  rc.top ), 
	};

	// �����ε�3���߶�
	Vector2Df triLineSeg[6] = 
	{
		tri.m_pt0, tri.m_pt1,
		tri.m_pt1, tri.m_pt2,
		tri.m_pt2, tri.m_pt0,
	};

	bool bIntersect = false;
	// ������ε�4���ߵ�����һ�����������3���ߵ�����һ���ཻ�Ļ�,��ô�����ξ�������ཻ
	for ( int i = 0; i < 8; i += 2 )
	{
		for ( int j = 0; j < 6; j += 2 )
		{
			bIntersect = IsLineSegIntersect( 
				rcLineSeg[i].x, rcLineSeg[i].y, rcLineSeg[i+1].x, rcLineSeg[i+1].y,
				triLineSeg[j].x, triLineSeg[j].y, triLineSeg[j+1].x, triLineSeg[j+1].y );
			if ( bIntersect )
			{
				return true;
			}
		}
	}

	return false;
}



NavEditor::NavEditor( void ) : m_pTrianglePrimitive(NULL)
, m_pLinePrimitive(NULL)
, m_pPointPrimitive(NULL)
{
	m_colEditPolygon = 0xffff00ff;
	m_colNavMeshTerrain = 0x8000ff00;
	m_colNavMeshSelf = 0x80ffff80;
	m_colNavMeshBack = 0xff000000;
	m_colMapBorder = 0xff00ffff;
	m_colPathTriangle = 0x80ff0000;
	m_colPathLine = 0xffffff00;
	m_colPathStart = 0xffffffff;
	m_colPathEnd = 0xff000000;
	m_colCheckPoint = 0xff0000ff;
	m_colEditNavTriangle = 0x8000ffff;

    /*
	m_pVertexDecl = ENGINE_INST->GetRenderer()->CreateVertexDeclaration();
	size_t offset = 0;
	m_pVertexDecl->AddVertexElement( 0, offset, eVET_FLOAT3, eVEU_POSITION, 0 );
	offset += sizeof(float) * 3;
	m_pVertexDecl->AddVertexElement( 0, offset, eVET_COLOUR, eVEU_DIFFUSE, 0 );
	offset += sizeof(u32);
	m_pVertexDecl->Build();
	VERTEXDECL_MANAGER->AddVertexDeclaration( m_pVertexDecl );
    */

	m_nStart = 0;
	m_nCurPolygon = -1;

	m_pVBEditNavPoints = NULL;
	m_nEditNavPointsCurMaxVertexCount = C_PAGE_VERTEX_COUNT;

	m_pVBNavMesh = NULL;

	m_pVBNavMeshBackLine = NULL;

	m_pVBPathTriangle = NULL;

	m_pVBPathLine = NULL;

	m_bShowEditNavPolygon = false;
	m_bShowNavMesh = false;
	m_bShowPathLine = false;
	m_bShowPathTriangle = false;

	m_bShowPathEndPoint = false;

	m_EditMode = eNEM_AddNavPolygon;

	m_bHasGenMapBorder = false;
	m_bEndEditMapBoarder = false;

	m_bEnableDepthTest = false;

	m_bIsVisable = false;

	m_nCurPickPolygon = -1;
	m_bPickPolygon = false;

	m_bNeedLoadEditData = true;
}


NavEditor::~NavEditor( void )
{
    SafeDelete(m_pTrianglePrimitive);
    SafeDelete(m_pLinePrimitive);
    SafeDelete(m_pPointPrimitive);
}


void NavEditor::SetMaterial( const AIRString& szMatName )
{
	if ( m_pMaterial )
	{
		MATERIAL_MANAGER->ClearMaterial(m_pMaterial);
	}
	m_pMaterial = MATERIAL_MANAGER->CreateMaterial( szMatName.c_str() );
}


void NavEditor::Clear( void )
{
	ResetNavEditor();

	MATERIAL_MANAGER->ClearMaterial( m_pMaterial );
}


void NavEditor::ResetNavEditor( void )
{
	IRenderer* pRenderer = ENGINE_INST->GetRenderer();

	pRenderer->ClearVertexBuffer(m_pVBEditNavPoints);
	m_pVBEditNavPoints = NULL;

	pRenderer->ClearVertexBuffer(m_pVBNavMesh );
	m_pVBNavMesh = NULL;

	pRenderer->ClearVertexBuffer(m_pVBNavMeshBackLine);
	m_pVBNavMeshBackLine = NULL;

	pRenderer->ClearVertexBuffer(m_pVBPathTriangle);
	m_pVBPathTriangle = NULL;

	pRenderer->ClearVertexBuffer(m_pVBPathLine);
	m_pVBPathLine = NULL;

	SafeDelete( m_pNavPathFinder );

	m_bShowEditNavPolygon = false;
	m_bShowNavMesh = false;
	m_bShowPathLine = false;
	m_bShowPathTriangle = false;

	m_nEditNavPointsCurMaxVertexCount = C_PAGE_VERTEX_COUNT;
	m_vtEditingPolygonVertex.clear();
	m_nStart = 0;
	m_nCurPolygon = -1;
	m_vtNavVertex.clear();
	m_vtNavPolygonInfo.clear();
	m_vtTriangle.clear();
	m_vtTVertex.clear();
	m_vtTVertexBackLine.clear();
	m_vtPathLineVertex.clear();
	m_vtPathLine.clear();
	m_vtTPath.clear();
	m_vtTPVertex.clear();

	m_bShowPathEndPoint = false;

	m_EditMode = eNEM_AddNavPolygon;

	m_vtMapBoarderPoints.clear();
	m_bEndEditMapBoarder = false;
	m_bHasGenMapBorder = false;

	m_bHasPickTriangle = false;

	m_vtCurCheckPoints.clear();

	m_lstOperatPoints.clear();
	m_lstOperatPointsInfo.clear();

	m_lstEdittingMapBorderVertex.clear();
	m_lstEdittingNavPolygonVertex.clear();

	m_nCurPolygon = -1;
	m_bPickPolygon = false;
}


void NavEditor::PrepareNextPolygon( void )
{
	// ����һ���������Ϣ�ṹ��
	PolygonInfo polyinfo;

	// ���õ�ǰ����ε���ʼ����ID
	if ( m_nCurPolygon == -1 )
	{
		m_nStart = 0;
	}
	else
	{
		m_nStart = m_vtNavPolygonInfo[m_nCurPolygon].m_Start + m_vtNavPolygonInfo[m_nCurPolygon].m_Len;  
	}

	polyinfo.m_Start = m_nStart;
	polyinfo.m_Len = 0;
	m_vtNavPolygonInfo.push_back(polyinfo);
	m_nCurPolygon++; 
}


void NavEditor::AddNewNavPolygonPoint( float x, float y, float z )
{
	// ����һ���������㵽�༭���б�
	NAV_VERTEX vertex;
	vertex.m_Pos.Set( x, y, z );
	vertex.m_Color = SColor(m_colEditPolygon);
	m_vtNavVertex.push_back( vertex );
	m_vtNavPolygonInfo[m_nCurPolygon].m_Len++;

	// ���±༭������Ķ��㻺��
	PrepareVBEditNavPoints();
}


void NavEditor::AddMapBorderPoint( float x, float y, float z )
{
	if ( m_bEndEditMapBoarder )
	{
		m_vtMapBoarderPoints.clear();
		m_bEndEditMapBoarder = false;
	}
	NAV_VERTEX vertex;
	vertex.m_Color = SColor(m_colMapBorder);
	vertex.m_Pos.Set( x, y, z );
	m_vtMapBoarderPoints.push_back( vertex );
}


// ȡ������ϰ��������ζ�����ͼ�߽����ζ���
void NavEditor::CancelAddNavPolygonPointOrMapBorderPoint( void )
{
	if ( m_EditMode == eNEM_AddNavPolygon )
	{
		if ( m_vtNavPolygonInfo[m_nCurPolygon].m_Len > 0 )
		{
			m_vtNavPolygonInfo[m_nCurPolygon].m_Len--;
			m_vtNavVertex.pop_back();
		}
		else
		{
			if ( m_vtNavPolygonInfo.size() == 1 )
			{
				return;
			}
			for ( size_t i = 0; i < m_vtNavPolygonInfo[m_nCurPolygon].m_Len; ++i )
			{
				m_vtNavVertex.pop_back();
			}
			m_vtNavPolygonInfo[m_nCurPolygon-1] = m_vtNavPolygonInfo[m_nCurPolygon];
			m_vtNavPolygonInfo.pop_back();
			--m_nCurPolygon;
			m_nStart = m_vtNavPolygonInfo[m_nCurPolygon].m_Start;
		}
		PrepareVBEditNavPoints();
	}
	else if ( m_EditMode == eNEM_EditMapBorder )
	{
		if ( m_vtMapBoarderPoints.size() > 0 )
		{
			m_vtMapBoarderPoints.pop_back();
		}
	}
}


void NavEditor::EndCurPolygon( void )
{
	if ( m_vtNavPolygonInfo[ m_nCurPolygon ].m_Len < 1 )
	{
		return;
	}

	// ���ӵ�ǰ�����,����ǰ����ε�β�ڵ���뵽������������
	m_vtNavVertex.push_back( m_vtNavVertex[m_nStart] );
	m_vtNavPolygonInfo[ m_nCurPolygon ].m_Len++;

	// ���±༭������Ķ��㻺��
	PrepareVBEditNavPoints();

	// ׼���༭��һ�������
	PrepareNextPolygon();
}


void NavEditor::EndMapBorder( void )
{
	if ( m_vtMapBoarderPoints.size() < 3 )
	{
		DT_TO_MSGBOX_FMT( LT_WARN, "The count of map border points must be > 2!" );
		return;
	}

	if ( m_vtMapBoarderPoints[0].m_Pos == m_vtMapBoarderPoints[ m_vtMapBoarderPoints.size()-1 ].m_Pos )
	{
		return;
	}

	m_vtMapBoarderPoints.push_back( m_vtMapBoarderPoints[0] );
	m_bEndEditMapBoarder = true;

	AIRVector< Vector3Df > tmpVecMapBorder;
	for ( size_t i = 0; i < m_vtMapBoarderPoints.size(); ++i )
	{
		tmpVecMapBorder.push_back( m_vtMapBoarderPoints[i].m_Pos );
	}
	SetWholeMapPolygon( Polygon3Df(tmpVecMapBorder) );
}


void NavEditor::SetWholeMapPolygon( const Polygon3Df& wholeMapPolygon )
{
	// ���õ�ǰ������ͼ������Χ�����
	m_WholeMapPolygon = wholeMapPolygon;
	m_bHasGenMapBorder = true;
}


void NavEditor::SetCandidateEditPoint( const Vector3Df& vEditPoint )
{
	m_vCandidateEditPoint = vEditPoint;
}


void NavEditor::CheckMapBorderValid( void )
{
	AIRVector<Line3Df> mapBorders = m_WholeMapPolygon.GetEdges();
	if ( mapBorders.size() && m_vtNavVertex.size() )
	{
		if ( mapBorders[0].HorzRelationShipOfLineAndPoint( m_vtNavVertex[0].m_Pos ) != -1 )
		{
			AIRVector<Vector3Df> tmpVec = m_WholeMapPolygon.GetPoints();
			u32 size = tmpVec.size();

			for ( size_t i = 0; i < size/2; ++i )
			{
				Vector3Df tmp = tmpVec[i];
				tmpVec[i] = tmpVec[size-i-1];
				tmpVec[size-i-1] = tmp;
			}

			m_WholeMapPolygon = Polygon3Df( tmpVec );
		}
	}
}


bool NavEditor::GenNavMesh( void )
{
	// ����Ƿ��Ѿ�����������ͼ������Χ�����
	if ( m_WholeMapPolygon.GetPoints().size() == 0 )
	{
		DT_TO_MSGBOX_FMT( LT_WARN, "Whole map polygon is empty!" );
		return false;
	}

	// ��鲢������ͼ�߽�����
	CheckMapBorderValid();

	Navigater nav;
	AIRVector<Polygon3Df> allPolygon;

	Polygon3Df po;
	AIRVector<Vector3Df>  allVertexOfThePolygon;

	// ��������ͼ������Χ����ηŽ����еĶ�����б���
	allPolygon.push_back( m_WholeMapPolygon ); 

	// �������еĶ������Ϣ
	for (size_t i = 0 ; i < m_vtNavPolygonInfo.size()-1; i++)
	{
		allVertexOfThePolygon.clear();              
		for (	u32 j = m_vtNavPolygonInfo[i].m_Start; 
			j < m_vtNavPolygonInfo[i].m_Start + m_vtNavPolygonInfo[i].m_Len - 1; 
			j++	)
		{
			// ȡ�õ�ǰ����ε����ж���
			allVertexOfThePolygon.push_back( m_vtNavVertex[j].m_Pos );
		}
		// ��¼��ǰ�����
		allPolygon.push_back( Polygon3Df(allVertexOfThePolygon) );
	}

	// ������������
	//m_vtTriangle = nav.CreateNavigater(allPolygon);
	AIRVector<Triangle3Df> vtTriangle = nav.CreateNavigater(allPolygon);

	NavTriangle nav_tri;
	nav_tri.m_byAttrib = 0;
	m_vtTriangle.clear();
	for ( size_t i = 0; i < vtTriangle.size(); ++i )
	{
		nav_tri.m_dwID = i;
		nav_tri.m_Triangle = vtTriangle[i];
		m_vtTriangle.push_back( nav_tri );
	}

	// ���������������ʾ����
	PreparemVBNavMesh();

	// ������������Ѱ·��
	m_pNavPathFinder = new NavPathFinder( m_vtTriangle );

    return true;
}


bool NavEditor::FindPathOfTriangle( Vector3Df start, Vector3Df end )
{
	// ����Ƿ��Ѿ�����NAVѰ·��
	if ( !m_pNavPathFinder )
	{
		DT_TO_MSGBOX_FMT( LT_WARN, GetString("NavPathFinder is not exist!") );
		return false;
	}

	// ���NAVѰ·���ϴ�Ѱ·�Ľ��
	m_pNavPathFinder->Clear();
	// ���������·���洢��
	m_vtTPath.clear();
	// Ѱ��������·��
	m_vtTPath = m_pNavPathFinder->FindPathOfTriangle( start, end );

	// ���������·���洢����Ϊ��,׼����ʾ������·������ʾ����
	if ( m_vtTPath.size() > 0 )
	{
		PrepareVBPathTriangle();
		return true;
	}
	return false;
}


bool NavEditor::FindPathOfLine( Vector3Df start, Vector3Df end )
{
	// ����Ƿ��Ѿ�����NAVѰ·��
	if ( !m_pNavPathFinder )
	{
		DT_TO_MSGBOX_FMT( LT_WARN, GetString("NavPathFinder is not exist!") );
		return false;
	}

	// ���NAVѰ·���ϴ�Ѱ·�Ľ��
	m_pNavPathFinder->Clear();
	// ��ն���·���洢��
	m_vtPathLine.clear();

	// �������������·���Ƿ����
	if ( !FindPathOfTriangle( end, start ) )
	{
		m_vtPathLine.clear();
		return false;
	}	

	// ���ݲ��ҵ���������·�����������·��
	m_vtPathLine = m_pNavPathFinder->FindPathOfLine( start, end );

	// ׼������·������ʾ��Ϣ
	if ( m_vtPathLine.size() > 0 )
	{
		PrepareVBPathLine();
		return true; 
	}
	return false;
}

void NavEditor::Render( void )
{
	if ( !this->m_bReady )
	{
		if ( this->IsLoaded() )
		{
			if ( !this->IsPrepared() )
			{
				m_bReady = this->Cache();
			}

			if ( m_bReady )
			{
				if ( m_pMaterial && !m_pMaterial->IsReady() )
				{
					m_bReady = m_pMaterial->Cache();
				}
			}
		}
	}

	if ( !m_bIsVisable )
	{
		return;
	}

	IRenderer* pRenderer = ENGINE_INST->GetRenderer();
	pRenderer->SetWorldTransform( &(Matrix4f::IDENTITY) );

    _GetPrimitive();

	if (m_pMaterial)
	{
        /*
		CTechnique* pTechnique = m_pMaterial->GetCurrentTechnique();
		if (pTechnique)
		{
			size_t nPassNum = pTechnique->GetPassesNum();
			for (size_t i = 0; i < nPassNum; ++i)
			{
				CPass* pPass = pTechnique->GetPass(i);
                //IShader* pShader = pPass->GetShader();
                pPass->SetZBufferEnable(m_bEnableDepthTest);
				pPass->ApplyPass(pRenderer);

				//pRenderer->SetVertexDeclaration( m_pVertexDecl );

				// ��ʾ�༭�е��ϰ�������������
				if ( m_bShowEditNavPolygon )
				{
					// ���ϰ�����
					RenderEditNavPoints();
					// ����ͼ�߽�
					RenderMapBorder();
					// ��ѡ�е��ϰ�����
					RenderPickEditNavPolygon();
				}

				// ��ʾ���ɵĵ�������
				if ( m_bShowNavMesh )
				{
					RenderNavMesh();
				}

				// ��ʾѰ�ҵ���������·��
				if ( m_bShowPathTriangle )
				{
					RenderPathTriangle();
				}

				// ��ʾѰ�ҵ��Ķ���·��
				if ( m_bShowPathLine )
				{
					RenderPathLine();
				}

				// ��ʾѰ·����ʼ���Լ�������
				if ( m_bShowPathEndPoint )
				{
					RenderPathEndPoint();
				}
			}
		}
        */
	}
}



void NavEditor::PrepareVBEditNavPoints( void )
{
	// ����༭�ĵ�����������Ϊ0������
	if ( m_vtNavVertex.size() == 0 )
	{
		return;
	}
	IRenderer* pRender = ENGINE_INST->GetRenderer();

	// �����༭�ĵ������㶥�㻺��
	if ( !m_pVBEditNavPoints )
	{
		//m_pVBEditNavPoints = pRender->CreateVertexBuffer( m_pVertexDecl, m_nEditNavPointsCurMaxVertexCount );
        m_pVBEditNavPoints = pRender->CreateVertexBuffer( m_nEditNavPointsCurMaxVertexCount, sizeof(NAV_VERTEX) );
	}

	// �����������������������Ĵ�С,�ؽ�����
	if ( m_vtNavVertex.size() >= m_nEditNavPointsCurMaxVertexCount )
	{
		m_nEditNavPointsCurMaxVertexCount += C_PAGE_VERTEX_COUNT;
		pRender->ClearVertexBuffer( m_pVBEditNavPoints );				
		//m_pVBEditNavPoints = pRender->CreateVertexBuffer( m_pVertexDecl, m_nEditNavPointsCurMaxVertexCount );
         m_pVBEditNavPoints = pRender->CreateVertexBuffer( m_nEditNavPointsCurMaxVertexCount, sizeof(NAV_VERTEX) );
	}

	// �����ݱ��浽���㻺����
	void* pVertex = m_pVBEditNavPoints->Lock();

	u32 size = m_vtNavVertex.size() * sizeof(NAV_VERTEX);
	memcpy( pVertex, &(m_vtNavVertex[0]), size );

	m_pVBEditNavPoints->Unlock();
}

void NavEditor::RenderEditNavPoints( void )
{
	// ����༭�ĵ�����������Ϊ0������
	if ( m_vtNavVertex.size() == 0 )
	{
		return;
	}

	IRenderer* pRender = ENGINE_INST->GetRenderer();

	//pRender->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
    pRender->SetFillMode(RSFM_Solid);

	// ���õ�ǰҪ���Ķ��㻺����
	pRender->SetVertexBuffer( 0, m_pVBEditNavPoints );

	size_t i = 0;
	for ( ; i < m_vtNavPolygonInfo.size()-1; ++i )
	{
		// �������
		if ( m_vtNavPolygonInfo[i].m_Len > 0 )
		{
			//pRender->DrawLineStrip( m_vtNavPolygonInfo[i].m_Start, m_vtNavPolygonInfo[i].m_Len-1 );
            //m_pLinePrimitive->m_pVertexData = &(m_vtEditingPolygonVertex[0]);
            m_pLinePrimitive->m_nVertexStart = m_vtNavPolygonInfo[i].m_Start;
            m_pLinePrimitive->SetPrimitiveCount(m_vtNavPolygonInfo[i].m_Len - 1);
            m_pLinePrimitive->m_pVertexBuffer = m_pVBEditNavPoints;
            //m_pLinePrimitive->Draw(NULL);
		}
	}

	// �������һ�������
	PolygonInfo pi = m_vtNavPolygonInfo[ i ];

	// ������һ������εĶ���������0
	if ( pi.m_Len > 0 )
	{
		m_vtEditingPolygonVertex.clear();
		u32 nEndID = pi.m_Start + pi.m_Len;
		for ( u32 j = pi.m_Start; j < nEndID; ++j )
		{
			m_vtEditingPolygonVertex.push_back( m_vtNavVertex[j] );
		}
		// ����һ����ѡ���
		NAV_VERTEX vertex;
		vertex.m_Pos = m_vCandidateEditPoint;
		vertex.m_Color = m_colEditPolygon;
		m_vtEditingPolygonVertex.push_back( vertex );

		// �����һ�������
		//pRender->DrawLineStrip( &(m_vtEditingPolygonVertex[0]), m_vtEditingPolygonVertex.size()-1, sizeof(NAV_VERTEX) );
        m_pLinePrimitive->m_pVertexData = &(m_vtEditingPolygonVertex[0]);
        m_pLinePrimitive->m_pVertexBuffer = NULL;
        m_pLinePrimitive->SetPrimitiveCount(m_vtEditingPolygonVertex.size() - 1);
        //m_pLinePrimitive->Draw(NULL);
	}
}


void NavEditor::PreparemVBNavMesh( void )
{
	// ����༭�ĵ�����������Ϊ0������
	if ( m_vtTriangle.size() == 0 )
	{
		m_vtTVertex.clear();
		m_vtTVertexBackLine.clear();
		return;
	}
	IRenderer* pRender = ENGINE_INST->GetRenderer();
	
	u32 nCurMaxCount = m_vtTriangle.size() * 3;
	// �����������񶥵㻺��
	if ( m_pVBNavMesh )
	{
		if ( m_pVBNavMesh->GetVertexCount() < nCurMaxCount )
		{
			pRender->ClearVertexBuffer( m_pVBNavMesh );
			//m_pVBNavMesh = pRender->CreateVertexBuffer( m_pVertexDecl, nCurMaxCount );
            m_pVBNavMesh = pRender->CreateVertexBuffer( nCurMaxCount, sizeof(NAV_VERTEX) );
		}		
	}
	else
	{
		//m_pVBNavMesh = pRender->CreateVertexBuffer( m_pVertexDecl, nCurMaxCount );
        m_pVBNavMesh = pRender->CreateVertexBuffer( nCurMaxCount, sizeof(NAV_VERTEX) );
	}

	nCurMaxCount = m_vtTriangle.size() * 3;
	// ���������������񶥵㻺��
	if ( m_pVBNavMeshBackLine )
	{
		if ( m_pVBNavMeshBackLine->GetVertexCount() < nCurMaxCount )
		{
			pRender->ClearVertexBuffer( m_pVBNavMeshBackLine );
			//m_pVBNavMeshBackLine = pRender->CreateVertexBuffer( m_pVertexDecl, nCurMaxCount );
            m_pVBNavMeshBackLine = pRender->CreateVertexBuffer( nCurMaxCount, sizeof(NAV_VERTEX) );
		}		
	}
	else
	{
		//m_pVBNavMeshBackLine = pRender->CreateVertexBuffer( m_pVertexDecl, nCurMaxCount );
        m_pVBNavMeshBackLine = pRender->CreateVertexBuffer( nCurMaxCount, sizeof(NAV_VERTEX) );
	}

	// ȡ�õ������������������ε����ж���
	NAV_VERTEX nav_vec;
	m_vtTVertex.clear();
	m_vtTVertexBackLine.clear();
	SColor col;
	for ( size_t i = 0; i < m_vtTriangle.size(); ++i )
	{
		if ( m_vtTriangle[i].m_byAttrib == 0 )
		{
			col = m_colNavMeshTerrain;
		}
		else
		{
			col = m_colNavMeshSelf;
		}
		
		nav_vec.m_Color = col;
		nav_vec.m_Pos = m_vtTriangle[i].m_Triangle.pointA;
		m_vtTVertex.push_back( nav_vec );
		nav_vec.m_Color = SColor(m_colNavMeshBack);
		m_vtTVertexBackLine.push_back( nav_vec );

		nav_vec.m_Color = col;
		nav_vec.m_Pos = m_vtTriangle[i].m_Triangle.pointB;
		m_vtTVertex.push_back( nav_vec );
		nav_vec.m_Color = SColor( m_colNavMeshBack );
		m_vtTVertexBackLine.push_back( nav_vec );

		nav_vec.m_Color = col;
		nav_vec.m_Pos = m_vtTriangle[i].m_Triangle.pointC;
		m_vtTVertex.push_back( nav_vec );
		nav_vec.m_Color = SColor( m_colNavMeshBack );
		m_vtTVertexBackLine.push_back( nav_vec );
	}

	// �����ݱ��浽���㻺����
	NAV_VERTEX* pVertex = (NAV_VERTEX*)(m_pVBNavMesh->Lock());
	u32 size = m_vtTVertex.size() * sizeof(NAV_VERTEX);
	memcpy( pVertex, &(m_vtTVertex[0]), size );
	m_pVBNavMesh->Unlock();

	pVertex = (NAV_VERTEX*)(m_pVBNavMeshBackLine->Lock());
	size = m_vtTVertexBackLine.size() * sizeof(NAV_VERTEX);
	memcpy( pVertex, &(m_vtTVertexBackLine[0]), size );
	m_pVBNavMeshBackLine->Unlock();
}


void NavEditor::RenderNavMesh( void )
{
	IRenderer* pRender = ENGINE_INST->GetRenderer();

	if ( m_pVBNavMesh )
	{
		//pRender->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
		pRender->SetFillMode(RSFM_Solid);
		//pRender->SetVertexBuffer( 0, m_pVBNavMesh );
		//pRender->DrawTriangleList( 0, m_vtTriangle.size() );
		m_pTrianglePrimitive->m_pVertexBuffer = m_pVBNavMesh;
		//m_pTrianglePrimitive->m_pVertexDecl = m_pVBNavMesh->GetVertexDeclaration();
		m_pTrianglePrimitive->SetPrimitiveCount(m_vtTriangle.size());
		//m_pTrianglePrimitive->Draw(NULL);
	}
	
	if ( m_pVBNavMeshBackLine )
	{
		//pRender->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
		pRender->SetFillMode(RSFM_WireFrame);
		//pRender->SetVertexBuffer( 0, m_pVBNavMeshBackLine );
		//pRender->DrawTriangleList( 0, m_vtTriangle.size() );
		m_pTrianglePrimitive->m_pVertexBuffer = m_pVBNavMeshBackLine;
		//m_pTrianglePrimitive->m_pVertexDecl = m_pVBNavMeshBackLine->GetVertexDeclaration();
		m_pTrianglePrimitive->SetPrimitiveCount(m_vtTriangle.size());
		//m_pTrianglePrimitive->Draw(NULL);
	}
}


void NavEditor::PrepareVBPathTriangle( void )
{
	// ����༭�ĵ�����������Ϊ0������
	if ( m_vtTPath.size() == 0 )
	{
		m_vtTPVertex.clear();
		return;
	}
	IRenderer* pRender = ENGINE_INST->GetRenderer();

	u32 nCurMaxCount = m_vtTPath.size() * 3;
	if ( m_pVBPathTriangle )
	{		
		if ( m_pVBPathTriangle->GetVertexCount() < nCurMaxCount )
		{
			pRender->ClearVertexBuffer( m_pVBPathTriangle );
			//m_pVBPathTriangle = pRender->CreateVertexBuffer( m_pVertexDecl, nCurMaxCount );
            m_pVBPathTriangle = pRender->CreateVertexBuffer( nCurMaxCount, sizeof(NAV_VERTEX) );
		}
	}
	else
	{
		//m_pVBPathTriangle = pRender->CreateVertexBuffer( m_pVertexDecl, nCurMaxCount );
        m_pVBPathTriangle = pRender->CreateVertexBuffer( nCurMaxCount, sizeof(NAV_VERTEX) );
	}

	m_vtTPVertex.clear();
	NAV_VERTEX nav_vec;
	nav_vec.m_Color = m_colPathTriangle;
	for ( size_t i = 0; i < m_vtTPath.size(); ++i )
	{
		Triangle3Df tr = m_vtTriangle[ m_vtTPath[i].x ].m_Triangle; 

		nav_vec.m_Pos = tr.pointA;
		m_vtTPVertex.push_back( nav_vec );

		nav_vec.m_Pos = tr.pointB;
		m_vtTPVertex.push_back( nav_vec );

		nav_vec.m_Pos = tr.pointC;
		m_vtTPVertex.push_back( nav_vec );
	}

	// �����ݱ��浽���㻺����
	NAV_VERTEX* pVertex = (NAV_VERTEX*)m_pVBPathTriangle->Lock();

	u32 size = m_vtTPVertex.size() * sizeof(NAV_VERTEX);
	memcpy( pVertex, &(m_vtTPVertex[0]), size );

	m_pVBPathTriangle->Unlock();
}


void NavEditor::RenderPathTriangle( void )
{
	if ( m_vtTPath.size() )
	{
		IRenderer* pRender = ENGINE_INST->GetRenderer();
		//pRender->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
        pRender->SetFillMode(RSFM_Solid);
		//pRender->SetVertexBuffer( 0, m_pVBPathTriangle );
		//pRender->DrawTriangleList( 0, m_vtTPath.size() );
        m_pTrianglePrimitive->m_pVertexBuffer = m_pVBPathTriangle;
        //m_pTrianglePrimitive->m_pVertexDecl = m_pVBPathTriangle->GetVertexDeclaration();
        m_pTrianglePrimitive->SetPrimitiveCount(m_vtTPath.size());
        //m_pTrianglePrimitive->Draw(NULL);
	}
}


void NavEditor::PrepareVBPathLine( void )
{
	// ����༭�ĵ�����������Ϊ0������
	if ( m_vtPathLine.size() == 0 )
	{
		m_vtPathLineVertex.clear();
		return;
	}
	IRenderer* pRender = ENGINE_INST->GetRenderer();

	u32 nCurMaxCount = m_vtPathLine.size();
	if ( m_pVBPathLine )
	{
		if ( m_pVBPathLine->GetVertexCount() < nCurMaxCount )
		{
			pRender->ClearVertexBuffer( m_pVBPathLine );
			//m_pVBPathLine = pRender->CreateVertexBuffer( m_pVertexDecl, nCurMaxCount );
            m_pVBPathLine = pRender->CreateVertexBuffer( nCurMaxCount, sizeof(NAV_VERTEX) );
		}		
	}
	else
	{
		//m_pVBPathLine = pRender->CreateVertexBuffer( m_pVertexDecl, nCurMaxCount );
        m_pVBPathLine = pRender->CreateVertexBuffer( nCurMaxCount, sizeof(NAV_VERTEX) );
	}

	m_vtPathLineVertex.clear();
	NAV_VERTEX nav_vec;
	nav_vec.m_Color = m_colPathLine;
	for ( size_t i = 0; i < m_vtPathLine.size(); ++i )
	{
		nav_vec.m_Pos = m_vtPathLine[i];
		m_vtPathLineVertex.push_back( nav_vec );
	}

	// �����ݱ��浽���㻺����
	NAV_VERTEX* pVertex = (NAV_VERTEX*)m_pVBPathLine->Lock();

	u32 size = m_vtPathLineVertex.size() * sizeof(NAV_VERTEX);
	memcpy( pVertex, &(m_vtPathLineVertex[0]), size );

	m_pVBPathLine->Unlock();
}


void NavEditor::RenderPathLine( void )
{
	if ( m_vtPathLine.size() )
	{
		IRenderer* pRender = ENGINE_INST->GetRenderer();
		//pRender->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
        pRender->SetFillMode(RSFM_Solid);
		//pRender->SetVertexBuffer( 0, m_pVBPathLine );
		//pRender->DrawLineStrip( 0, m_vtPathLineVertex.size()-1 );
        m_pLinePrimitive->m_pVertexBuffer = m_pVBPathLine;
        //m_pLinePrimitive->m_pVertexDecl = m_pVBPathLine->GetVertexDeclaration();
        m_pLinePrimitive->SetPrimitiveCount(m_vtPathLineVertex.size() - 1);
        //m_pLinePrimitive->Draw(NULL);
	}
}


void NavEditor::RenderPathEndPoint( void )
{
	IRenderer* pRender = ENGINE_INST->GetRenderer();

	NAV_VERTEX pathPoint[2];
	pathPoint[0].m_Pos = m_vEditPathStart;
	pathPoint[0].m_Color = m_colPathStart;

	pathPoint[1].m_Pos = m_vEditPathEnd;
	pathPoint[1].m_Color = m_colPathEnd;

	float psize = 10.0f;
	//pRender->SetRenderState( D3DRS_POINTSIZE, *((u32*)(&psize)) );
	//pRender->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
	//pRender->SetRenderState( D3DRS_POINTSCALEENABLE, FALSE );
    pRender->SetPointSizeState(psize);
    pRender->SetPointSpriteEnable(false);
    pRender->SetPointScale(false);
	//pRender->DrawPointList( sizeof(NAV_VERTEX), 0, 2, pathPoint );
    m_pPointPrimitive->m_pVertexBuffer = NULL;
    m_pPointPrimitive->SetPrimitiveCount(2);
    m_pPointPrimitive->m_pVertexData = &pathPoint[0];
    m_pPointPrimitive->m_nVerticesStride = sizeof(NAV_VERTEX);
    m_pPointPrimitive->m_nVertexStart = 0;
    //m_pPointPrimitive->Draw(NULL);

	if ( m_lstOperatPoints.size() )
	{
		NAV_VERTEX vertex[C_PAGE_VERTEX_COUNT];
		AIRList< Vector3Df >::iterator it = m_lstOperatPoints.begin();
		for ( size_t i = 0; i < m_lstOperatPoints.size(); ++i )
		{
			vertex[i].m_Color = m_colCheckPoint;
			vertex[i].m_Pos = *it;
			++it;
		}
		//pRender->DrawPointList( sizeof(NAV_VERTEX), 0, m_lstOperatPoints.size(), &vertex[0] );
        m_pPointPrimitive->m_pVertexBuffer = NULL;
        m_pPointPrimitive->SetPrimitiveCount(m_lstOperatPoints.size());
        m_pPointPrimitive->m_pVertexData = &vertex[0];
        m_pPointPrimitive->m_nVerticesStride = sizeof(NAV_VERTEX);
        m_pPointPrimitive->m_nVertexStart = 0;
        //m_pPointPrimitive->Draw(NULL);
	}
	else if ( m_vtCurCheckPoints.size() )
	{
		NAV_VERTEX vertex;
		vertex.m_Color = m_colCheckPoint;
		vertex.m_Pos = m_vCheckPointPos;
		//pRender->DrawPointList( sizeof(NAV_VERTEX), 0, 1, &vertex );
        m_pPointPrimitive->m_pVertexBuffer = NULL;
        m_pPointPrimitive->m_pVertexData = &vertex;
        m_pPointPrimitive->SetPrimitiveCount(1);
        m_pPointPrimitive->m_nVerticesStride = sizeof(NAV_VERTEX);
        m_pPointPrimitive->m_nVertexStart = 0;
        //m_pPointPrimitive->Draw(NULL);
	}
	else if ( m_bHasPickTriangle )
	{
		NAV_VERTEX vTri[3];
		vTri[0].m_Pos = m_CurPickTriangle.m_Triangle.pointA;
		vTri[0].m_Color = m_colEditNavTriangle;
		vTri[1].m_Pos = m_CurPickTriangle.m_Triangle.pointB;
		vTri[1].m_Color = m_colEditNavTriangle;
		vTri[2].m_Pos = m_CurPickTriangle.m_Triangle.pointC;
		vTri[2].m_Color = m_colEditNavTriangle;
		//pRender->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
        pRender->SetFillMode(RSFM_Solid);
		//pRender->DrawTriangleList( sizeof(NAV_VERTEX), 0, 1, &vTri[0] );
        m_pTrianglePrimitive->m_pVertexBuffer = NULL;
        m_pTrianglePrimitive->m_pVertexData = &vTri[0];
        m_pTrianglePrimitive->SetPrimitiveCount(1);
        m_pTrianglePrimitive->m_nVerticesStride = sizeof(NAV_VERTEX);
        m_pTrianglePrimitive->m_nVertexStart = 0;
        //m_pTrianglePrimitive->Draw(NULL);
	}
}


void NavEditor::RenderMapBorder( void )
{
	IRenderer* pRender = ENGINE_INST->GetRenderer();

	if ( m_vtMapBoarderPoints.size() == 0 )
	{
		return;
	}

	if ( m_bEndEditMapBoarder )
	{
		//pRender->DrawLineStrip( &m_vtMapBoarderPoints[0], m_vtMapBoarderPoints.size()-1, sizeof(NAV_VERTEX) );
        m_pLinePrimitive->m_pVertexBuffer = NULL;
        m_pLinePrimitive->m_pVertexData = &m_vtMapBoarderPoints[0];
        m_pLinePrimitive->SetPrimitiveCount(m_vtMapBoarderPoints.size()-1);
        m_pLinePrimitive->m_nVertexStart = 0;
        m_pLinePrimitive->m_nVerticesStride = sizeof(NAV_VERTEX);
        //m_pLinePrimitive->Draw(NULL);
	}
	else
	{
		NAV_VERTEX vertex;
		vertex.m_Pos = m_vCandidateEditPoint;
		vertex.m_Color = m_colMapBorder;
		m_vtMapBoarderPoints.push_back( vertex );
		//pRender->DrawLineStrip( &m_vtMapBoarderPoints[0], m_vtMapBoarderPoints.size()-1, sizeof(NAV_VERTEX) );
        m_pLinePrimitive->m_pVertexBuffer = NULL;
        m_pLinePrimitive->m_pVertexData = &m_vtMapBoarderPoints[0];
        m_pLinePrimitive->SetPrimitiveCount(m_vtMapBoarderPoints.size() - 1);
        m_pLinePrimitive->m_nVertexStart = 0;
        m_pLinePrimitive->m_nVerticesStride = sizeof(NAV_VERTEX);
        //m_pLinePrimitive->Draw(NULL);
		m_vtMapBoarderPoints.pop_back();
	}
}



void NavEditor::RenderPickEditNavPolygon( void )
{
	this->ClearOperatPointsAndInfo();
	this->ClearPickNavTriangle();
	this->ClearCurCheckPoint();

	IRenderer* pRender = ENGINE_INST->GetRenderer();
	if ( m_bPickPolygon && m_nCurPickPolygon != -1 )
	{
		PolygonInfo pi = m_vtNavPolygonInfo[m_nCurPickPolygon];
		AIRVector<NAV_VERTEX> tmpVertex;
		NAV_VERTEX vertex;
		vertex.m_Color = m_colMapBorder;
		for ( u32 i = pi.m_Start; i < pi.m_Start+pi.m_Len; ++i )
		{
			vertex.m_Pos = m_vtNavVertex[i].m_Pos;
			tmpVertex.push_back( vertex );
		}
		//pRender->DrawLineStrip( &tmpVertex[0], tmpVertex.size()-1, sizeof(NAV_VERTEX) );
        m_pLinePrimitive->m_pVertexBuffer = NULL;
        m_pLinePrimitive->m_pVertexData = &tmpVertex[0];
        m_pLinePrimitive->SetPrimitiveCount(tmpVertex.size() - 1);
        m_pLinePrimitive->m_nVertexStart = 0;
        m_pLinePrimitive->m_nVerticesStride = sizeof(NAV_VERTEX);
        //m_pLinePrimitive->Draw(NULL);
	}
}



void NavEditor::ResetMapBorder( void )
{
	m_vtMapBoarderPoints.clear();
	m_bEndEditMapBoarder = false;
}



void NavEditor::ResetEditNavPolygon( void )
{
	IRenderer* pRenderer = ENGINE_INST->GetRenderer();

	m_nEditNavPointsCurMaxVertexCount = C_PAGE_VERTEX_COUNT;
	m_vtEditingPolygonVertex.clear();
	m_nStart = 0;
	m_nCurPolygon = -1;
	m_vtNavVertex.clear();
	m_vtNavPolygonInfo.clear();

	m_vtTriangle.clear();
	m_vtTVertexBackLine.clear();
	m_vtTVertex.clear();

	PrepareNextPolygon();
}


// ���õ���������Ϣ
void NavEditor::ResetNavMesh( void )
{
	IRenderer* pRenderer = ENGINE_INST->GetRenderer();
	m_vtTriangle.clear();
	SafeDelete( m_pNavPathFinder );
}


void NavEditor::DebugFindPath( void )
{
	SetShowPathTriangle( true );
	SetShowPathLine( true );
	FindPathOfLine( m_vEditPathStart, m_vEditPathEnd );
}


bool NavEditor::CalCheckPoints( float fRX, float fRZ, Vector3Df& vOut, float fErrTor /* = 1.0f */ )
{
	m_bPickCheckPoint = false;
	m_vtCurCheckPoints.clear();

	Vector2Df vDst( fRX, fRZ );
	Vector2Df vCheck;
	CheckPoint cp;
	for ( size_t i = 0; i < m_vtTriangle.size(); ++i )
	{
		Triangle3Df tri = m_vtTriangle[i].m_Triangle;

		vCheck.Set( tri.pointA.x, tri.pointA.z );
		if ( vCheck.GetDistanceFrom( vDst ) < fErrTor )
		{
			cp.m_nTriangleID = i;
			cp.m_nVertexID = 0;
			m_vtCurCheckPoints.push_back( cp );
		}

		vCheck.Set( tri.pointB.x, tri.pointB.z );
		if ( vCheck.GetDistanceFrom( vDst ) < fErrTor )
		{
			cp.m_nTriangleID = i;
			cp.m_nVertexID = 1;
			m_vtCurCheckPoints.push_back( cp );
		}

		vCheck.Set( tri.pointC.x, tri.pointC.z );
		if ( vCheck.GetDistanceFrom( vDst ) < fErrTor )
		{
			cp.m_nTriangleID = i;
			cp.m_nVertexID = 2;
			m_vtCurCheckPoints.push_back( cp );
		}
	}

	int count = m_vtCurCheckPoints.size();
	if ( count == 0 )
	{
		m_bPickCheckPoint = false;
		return false;
	}

	cp = m_vtCurCheckPoints[0];
	if ( cp.m_nVertexID == 0 )
	{
		vOut = m_vtTriangle[cp.m_nTriangleID].m_Triangle.pointA;
	}
	else if ( cp.m_nVertexID == 1 )
	{
		vOut = m_vtTriangle[cp.m_nTriangleID].m_Triangle.pointB;
	}
	else if ( cp.m_nVertexID == 2 )
	{
		vOut = m_vtTriangle[cp.m_nTriangleID].m_Triangle.pointC;
	}
	m_vCheckPointPos = vOut;
	m_bPickCheckPoint = true;
	return true;
}


bool NavEditor::CalCheckPoints( Line3Df& line, Vector3Df& vOut, float fErrTor/* = 1.0f*/ )
{
	m_vtCurCheckPoints.clear();

	CheckPoint cp;
	for ( size_t i = 0; i < m_vtTriangle.size(); ++i )
	{
		Triangle3Df tri = m_vtTriangle[i].m_Triangle;

		vOut = tri.pointA;
		if ( line.GetDistanceFromPointToLine( vOut ) < fErrTor )
		{
			cp.m_nTriangleID = i;
			cp.m_nVertexID = 0;
			m_vtCurCheckPoints.push_back( cp );
		}

		vOut = tri.pointB;
		if ( line.GetDistanceFromPointToLine( vOut ) < fErrTor )
		{
			cp.m_nTriangleID = i;
			cp.m_nVertexID = 1;
			m_vtCurCheckPoints.push_back( cp );
		}

		vOut = tri.pointC;
		if ( line.GetDistanceFromPointToLine( vOut ) < fErrTor )
		{
			cp.m_nTriangleID = i;
			cp.m_nVertexID = 2;
			m_vtCurCheckPoints.push_back( cp );
		}
	}

	int count = m_vtCurCheckPoints.size();
	if ( count == 0 )
	{
		return false;
	}

	cp = m_vtCurCheckPoints[0];
	if ( cp.m_nVertexID == 0 )
	{
		vOut = m_vtTriangle[cp.m_nTriangleID].m_Triangle.pointA;
	}
	else if ( cp.m_nVertexID == 1 )
	{
		vOut = m_vtTriangle[cp.m_nTriangleID].m_Triangle.pointB;
	}
	else if ( cp.m_nVertexID == 2 )
	{
		vOut = m_vtTriangle[cp.m_nTriangleID].m_Triangle.pointC;
	}
	m_vCheckPointPos = vOut;
	return true;
}



void NavEditor::MoveCheckPoints( const Vector3Df& vNewPos )
{
	if ( m_vtCurCheckPoints.size() )
	{
		for ( size_t i = 0; i < m_vtCurCheckPoints.size(); ++i )
		{
			CheckPoint cp = m_vtCurCheckPoints[i];
			switch ( cp.m_nVertexID )
			{
			case 0:
				m_vtTriangle[ cp.m_nTriangleID ].m_Triangle.pointA = vNewPos;
				break;
			case 1:
				m_vtTriangle[ cp.m_nTriangleID ].m_Triangle.pointB = vNewPos;
				break;
			case 2:
				m_vtTriangle[ cp.m_nTriangleID ].m_Triangle.pointC = vNewPos;
				break;
			}
		}

		m_vCheckPointPos = vNewPos;

		RefreshNavInfo();
	}
}



// ˢ�µ���������Ϣ
void NavEditor::RefreshNavInfo( void )
{
	if ( m_pNavPathFinder )
	{
		m_pNavPathFinder->ResetNavPathFinder( m_vtTriangle );
	}
	else
	{
		m_pNavPathFinder = new NavPathFinder( m_vtTriangle );
	}
	PreparemVBNavMesh();
	PrepareVBPathTriangle();
	PrepareVBPathLine();
}



bool NavEditor::CalPickTriangle( float fRX, float fRZ, NavTriangle& nav_tri )
{
	Vector2Df vCheck( fRX, fRZ );

	// ���Ƚ��о�ȷ�ز���
	for ( size_t i = 0; i < m_vtTriangle.size(); ++i )
	{
		m_CurPickTriangle = m_vtTriangle[i];
		//if ( m_CurPickTriangle.m_Triangle.GetHorzProjTriangle2D().IsInTriangle( vCheck ) )
		if ( m_CurPickTriangle.m_Triangle.Is2DPointInHorzProjTriangle( fRX, fRZ ) )
		{
			nav_tri = m_CurPickTriangle;
			m_bHasPickTriangle = true;
			return true;
		}
	}

	// ��ȷ�����Ҳ���,�ͽ���ģ������,ֻҪ�㵽�����α߽�֮һ�ľ�����5����λ֮��,���Ѹõ㵱����������֮��
	for ( size_t i = 0; i < m_vtTriangle.size(); ++i )
	{
		m_CurPickTriangle = m_vtTriangle[i];
		//if ( m_CurPickTriangle.m_Triangle.GetHorzProjTriangle2D().IsInTriangle( vCheck, 5.0f ) )
		if ( m_CurPickTriangle.m_Triangle.Is2DPointInHorzProjTriangle( fRX, fRZ, 5.0 ) )
		{
			nav_tri = m_CurPickTriangle;
			m_bHasPickTriangle = true;
			return true;
		}
	}

	m_bHasPickTriangle = false;
	return false;
}



bool NavEditor::CalPickTriangle( const Line3Df& line, NavTriangle& nav_tri )
{
	Vector3Df vout;
	for ( size_t i = 0; i < m_vtTriangle.size(); ++i )
	{
		m_CurPickTriangle = m_vtTriangle[i];
		if ( m_CurPickTriangle.m_Triangle.GetIntersectionWithLimitedLine( line, vout ) )
		{
			nav_tri = m_CurPickTriangle;
			m_bHasPickTriangle = true;
			return true;
		}
	}
	m_bHasPickTriangle = false;
	return false;
}


void NavEditor::SetPickNavTriangleAttrib( u8 attrib )
{
	if ( m_bHasPickTriangle )
	{
		m_vtTriangle[ m_CurPickTriangle.m_dwID ].m_byAttrib = attrib;
		RefreshNavInfo();
	}
}


bool NavEditor::LoadFromXML( const char* szNavFile )
{
	if ( !szNavFile )
	{
		return false;
	}
	CDataStream* pStream = FILESYSTEM->GetFileStream( szNavFile );
	if ( !pStream )
	{
		DT_TO_MSGBOX_FMT( LT_WARN, GetString("Load navigation file [%s] failed!", szNavFile) );
		return false;
	}

	AIRString strDoc = pStream->GetAsString();
	CMarkupSTL xml(strDoc.c_str());

	//��һ������������
	if (!xml.FindElem())
	{
		SafeDelete( pStream );
		return false;
	}

	xml.IntoElem();

	if ( !CreateFromXML(xml) )
	{
		SafeDelete( pStream );
		return false;
	}

	xml.OutOfElem();

	SafeDelete( pStream );
	return true;
}

bool NavEditor::Cache()
{
    if (m_pStream)
    {
        CMemoryDataStream memDataStream( *m_pStream, true );
        u8* pDataPtr = memDataStream.GetPtr();

        // ������С
        float fSceneSize = *(float*)(pDataPtr);
        pDataPtr += sizeof(float);

        // ��ǰ�༭���ϰ����񶥵�����ֵ
        m_nEditNavPointsCurMaxVertexCount = *(size_t*)(pDataPtr);
        pDataPtr += sizeof(size_t);

        // ��һ���ϰ����񶥵����ʼ����
        m_nStart = *(size_t*)(pDataPtr);
        pDataPtr += sizeof(size_t);

        // ��ǰ���ڱ༭�ڼ����ϰ���������
        m_nCurPolygon = *(size_t*)(pDataPtr);
        pDataPtr += sizeof(size_t);

        // �༭ģʽ
        m_EditMode = NavEditorMode( *(int*)(pDataPtr) );
        pDataPtr += sizeof(int);

        // ��ǰ�Ƿ���ʾ�ϰ����������
        m_bShowEditNavPolygon = bool( *(int*)(pDataPtr) );
        pDataPtr += sizeof(int);

        // ��ǰ�Ƿ���ʾ��������
        m_bShowNavMesh = bool( *(int*)(pDataPtr) );
        pDataPtr += sizeof(int);

        // ��ǰ�Ƿ���ʾ������·��
        m_bShowPathTriangle = bool( *(int*)(pDataPtr) );
        pDataPtr += sizeof(int);

        // ��ǰ�Ƿ���ʾѰ··��
        m_bShowPathLine = bool( *(int*)(pDataPtr) );
        pDataPtr += sizeof(int);

        // ��ǰ�Ƿ���ʾ·���˵�
        m_bShowPathEndPoint = bool( *(int*)(pDataPtr) );
        pDataPtr += sizeof(int);

        // ��ǰ�Ƿ����Ȳ���,��������ģʽ��ʾ��������
        m_bEnableDepthTest = bool( *(int*)(pDataPtr) );
        pDataPtr += sizeof(int);

        // �Ƿ���ʾ�������������Ϣ]
        m_bIsVisable = bool( *(int*)(pDataPtr) );
        pDataPtr += sizeof(int);

        // ��¼��ǰ�Ƿ��Ѿ�������ͼ�߽�
        m_bHasGenMapBorder = bool( *(int*)(pDataPtr) );
        pDataPtr += sizeof(int);

        // ��¼��ǰ�Ƿ��Ѿ�������ͼ�߽����εı༭
        m_bEndEditMapBoarder = bool( *(int*)(pDataPtr) );
        pDataPtr += sizeof(int);

        // ��ͼ�߽����εĸ���������Ϣ
        u32 nMapBorderVertexCount = *(u32*)(pDataPtr);
        pDataPtr += sizeof(u32);
        if ( nMapBorderVertexCount > 0 )
        {
            u32 dwDataSize = sizeof( Vector3Df ) * nMapBorderVertexCount;
            if ( m_bNeedLoadEditData )
            {
                AIRVector< Vector3Df > vtMapBorderVertex;
                vtMapBorderVertex.resize( nMapBorderVertexCount );
                memcpy( &vtMapBorderVertex[0], pDataPtr, dwDataSize );
                m_WholeMapPolygon = Polygon3Df( vtMapBorderVertex );
            }
            pDataPtr += dwDataSize;		
        }

        // �༭�õ��ϰ����񶥵���Ϣ
        u32 nNavVertexCount = *(u32*)(pDataPtr);
        pDataPtr += sizeof(u32);
        if ( nNavVertexCount > 0 )
        {
            u32 dwDataSize = sizeof( NAV_VERTEX ) * nNavVertexCount;
            if ( m_bNeedLoadEditData )
            {
                m_vtNavVertex.clear();
                m_vtNavVertex.resize( nNavVertexCount );
                memcpy( &m_vtNavVertex[0], pDataPtr, dwDataSize );
            }
            pDataPtr += dwDataSize;
        }

        // �༭�õ��ϰ�����������Ϣ
        u32 nNavPolygonInfoCount = *(u32*)(pDataPtr);
        pDataPtr += sizeof(u32);
        if ( nNavPolygonInfoCount > 0 )
        {
            u32 dwDataSize = sizeof( PolygonInfo ) * nNavPolygonInfoCount;
            if ( m_bNeedLoadEditData )
            {
                m_vtNavPolygonInfo.clear();
                m_vtNavPolygonInfo.resize( nNavPolygonInfoCount );
                memcpy( &m_vtNavPolygonInfo[0], pDataPtr, dwDataSize );
            }
            pDataPtr += dwDataSize;
        }

        // ��¼��������
        u32 nNavMeshCount = *(u32*)(pDataPtr);
        pDataPtr += sizeof(u32);
        if ( nNavMeshCount > 0 )
        {
            m_vtTriangle.clear();
            m_vtTriangle.resize( nNavMeshCount );
            u32 dwDataSize = sizeof( NavTriangle ) * nNavMeshCount;
            memcpy( &m_vtTriangle[0], pDataPtr, dwDataSize );
            pDataPtr += dwDataSize;
        }

        // ������������༭����ʾ����
        PrepareVBEditNavPoints();

        if ( m_vtTriangle.size() )
        {
            // ���������������ʾ����
            PreparemVBNavMesh();

            // ������������Ѱ·��
            if ( !m_pNavPathFinder )
            {
                m_pNavPathFinder = new NavPathFinder( m_vtTriangle );
            }
            else
            {
                m_pNavPathFinder->ResetNavPathFinder( m_vtTriangle );
            }

            if ( !OptimiseNavMesh( 50 ) )
            {
                DT_TO_MSGBOX_FMT( LT_WARN, "Optimise nav mesh failed!" );
            }
        }
        m_bIsVisable = false;

        SafeDelete(m_pStream);
    }

    m_bReady = true;
    NotifyAllListenerReady();
    return m_bReady;
}

bool NavEditor::LoadFromBinFile( const char* szNavFile )
{
    /*
	CDataStream* pStream = FILESYSTEM->GetFileStream( szNavFile );
	if (pStream == NULL)
	{
		return false;
	}

	CMemoryDataStream memDataStream( *pStream, true );
	u8* pDataPtr = memDataStream.GetPtr();

	// ������С
	float fSceneSize = *(float*)(pDataPtr);
	pDataPtr += sizeof(float);

	// ��ǰ�༭���ϰ����񶥵�����ֵ
	m_nEditNavPointsCurMaxVertexCount = *(size_t*)(pDataPtr);
	pDataPtr += sizeof(size_t);

	// ��һ���ϰ����񶥵����ʼ����
	m_nStart = *(size_t*)(pDataPtr);
	pDataPtr += sizeof(size_t);

	// ��ǰ���ڱ༭�ڼ����ϰ���������
	m_nCurPolygon = *(size_t*)(pDataPtr);
	pDataPtr += sizeof(size_t);

	// �༭ģʽ
	m_EditMode = NavEditorMode( *(int*)(pDataPtr) );
	pDataPtr += sizeof(int);

	// ��ǰ�Ƿ���ʾ�ϰ����������
	m_bShowEditNavPolygon = bool( *(int*)(pDataPtr) );
	pDataPtr += sizeof(int);

	// ��ǰ�Ƿ���ʾ��������
	m_bShowNavMesh = bool( *(int*)(pDataPtr) );
	pDataPtr += sizeof(int);

	// ��ǰ�Ƿ���ʾ������·��
	m_bShowPathTriangle = bool( *(int*)(pDataPtr) );
	pDataPtr += sizeof(int);

	// ��ǰ�Ƿ���ʾѰ··��
	m_bShowPathLine = bool( *(int*)(pDataPtr) );
	pDataPtr += sizeof(int);

	// ��ǰ�Ƿ���ʾ·���˵�
	m_bShowPathEndPoint = bool( *(int*)(pDataPtr) );
	pDataPtr += sizeof(int);

	// ��ǰ�Ƿ����Ȳ���,��������ģʽ��ʾ��������
	m_bEnableDepthTest = bool( *(int*)(pDataPtr) );
	pDataPtr += sizeof(int);

	// �Ƿ���ʾ�������������Ϣ]
	m_bIsVisable = bool( *(int*)(pDataPtr) );
	pDataPtr += sizeof(int);

	// ��¼��ǰ�Ƿ��Ѿ�������ͼ�߽�
	m_bHasGenMapBorder = bool( *(int*)(pDataPtr) );
	pDataPtr += sizeof(int);

	// ��¼��ǰ�Ƿ��Ѿ�������ͼ�߽����εı༭
	m_bEndEditMapBoarder = bool( *(int*)(pDataPtr) );
	pDataPtr += sizeof(int);

	// ��ͼ�߽����εĸ���������Ϣ
	u32 nMapBorderVertexCount = *(u32*)(pDataPtr);
	pDataPtr += sizeof(u32);
	if ( nMapBorderVertexCount > 0 )
	{
		u32 dwDataSize = sizeof( Vector3Df ) * nMapBorderVertexCount;
		if ( m_bNeedLoadEditData )
		{
			AIRVector< Vector3Df > vtMapBorderVertex;
			vtMapBorderVertex.resize( nMapBorderVertexCount );
			memcpy( &vtMapBorderVertex[0], pDataPtr, dwDataSize );
			m_WholeMapPolygon = Polygon3Df( vtMapBorderVertex );
		}
		pDataPtr += dwDataSize;		
	}
	
	// �༭�õ��ϰ����񶥵���Ϣ
	u32 nNavVertexCount = *(u32*)(pDataPtr);
	pDataPtr += sizeof(u32);
	if ( nNavVertexCount > 0 )
	{
		u32 dwDataSize = sizeof( NAV_VERTEX ) * nNavVertexCount;
		if ( m_bNeedLoadEditData )
		{
			m_vtNavVertex.clear();
			m_vtNavVertex.resize( nNavVertexCount );
			memcpy( &m_vtNavVertex[0], pDataPtr, dwDataSize );
		}
		pDataPtr += dwDataSize;
	}

	// �༭�õ��ϰ�����������Ϣ
	u32 nNavPolygonInfoCount = *(u32*)(pDataPtr);
	pDataPtr += sizeof(u32);
	if ( nNavPolygonInfoCount > 0 )
	{
		u32 dwDataSize = sizeof( PolygonInfo ) * nNavPolygonInfoCount;
		if ( m_bNeedLoadEditData )
		{
			m_vtNavPolygonInfo.clear();
			m_vtNavPolygonInfo.resize( nNavPolygonInfoCount );
			memcpy( &m_vtNavPolygonInfo[0], pDataPtr, dwDataSize );
		}
		pDataPtr += dwDataSize;
	}

	// ��¼��������
	u32 nNavMeshCount = *(u32*)(pDataPtr);
	pDataPtr += sizeof(u32);
	if ( nNavMeshCount > 0 )
	{
		m_vtTriangle.clear();
		m_vtTriangle.resize( nNavMeshCount );
		u32 dwDataSize = sizeof( NavTriangle ) * nNavMeshCount;
		memcpy( &m_vtTriangle[0], pDataPtr, dwDataSize );
		pDataPtr += dwDataSize;
	}

	// ������������༭����ʾ����
	PrepareVBEditNavPoints();

	if ( m_vtTriangle.size() )
	{
		// ���������������ʾ����
		PreparemVBNavMesh();

		// ������������Ѱ·��
		if ( !m_pNavPathFinder )
		{
			m_pNavPathFinder = new NavPathFinder( m_vtTriangle );
		}
		else
		{
			m_pNavPathFinder->ResetNavPathFinder( m_vtTriangle );
		}

		if ( !OptimiseNavMesh( 50 ) )
		{
			DT_TO_MSGBOX_FMT( LT_WARN, "Optimise nav mesh failed!" );
		}
	}
	m_bIsVisable = false;

	SafeDelete( pStream );
    */

	return Resource::LoadFromFile(szNavFile, ENGINE_INST->GetConfig()->m_bBackgroundThread);
}


bool NavEditor::CreateFromXML( CMarkupSTL& xml )
{
	CreateEditInfo( xml );
	PrepareVBEditNavPoints();

	CreateNavInfo(xml);
	if ( m_vtTriangle.size() )
	{
		// ���������������ʾ����
		PreparemVBNavMesh();

		// ������������Ѱ·��
		if ( !m_pNavPathFinder )
		{
			m_pNavPathFinder = new NavPathFinder( m_vtTriangle );
		}
		else
		{
			m_pNavPathFinder->ResetNavPathFinder( m_vtTriangle );
		}

		if ( !OptimiseNavMesh( 50 ) )
		{
			DT_TO_MSGBOX_FMT( LT_WARN, "Optimise nav mesh failed!" );
		}
	}
	m_bIsVisable = false;
	return true;
}


bool NavEditor::CreateEditInfo( CMarkupSTL& xml )
{
	AIRString szValue;
	if ( xml.FindElem("EditInfo") )
	{
		szValue = xml.GetAttrib( "SceneSize" ).c_str();
		float fSceneSize = atof( szValue.c_str() );

		szValue = xml.GetAttrib( "EditNavPointsCurMaxVertexCount" ).c_str();
		m_nEditNavPointsCurMaxVertexCount = atoi( szValue.c_str() );
		m_nStart = atoi( xml.GetAttrib("Start").c_str() );
		m_nCurPolygon = atoi( xml.GetAttrib("CurPolygon").c_str() );
		szValue = xml.GetAttrib( "EditMode" ).c_str();
		if ( szValue == "eNEM_EditMapBorder" )
		{
			m_EditMode = eNEM_EditMapBorder;
		}
		else if ( szValue == "eNEM_AddNavPolygon" )
		{
			m_EditMode = eNEM_AddNavPolygon;
		}
		else if ( szValue == "eNEM_EditPathStartEnd" )
		{
			m_EditMode = eNEM_EditPathStartEnd;
		}
		else if ( szValue == "eNEM_FindPath" )
		{
			m_EditMode = eNEM_FindPath;
		}

		szValue = xml.GetAttrib( "ShowEditNavPolygon" ).c_str();
		if ( szValue == "TRUE" )
		{
			m_bShowEditNavPolygon = true;
		}
		else
		{
			m_bShowEditNavPolygon = false;
		}

		szValue = xml.GetAttrib( "ShowNavMesh" ).c_str();
		if ( szValue == "TRUE" )
		{
			m_bShowNavMesh = true;
		}
		else
		{
			m_bShowNavMesh = false;
		}

		szValue = xml.GetAttrib( "ShowPathTriangle" ).c_str();
		if ( szValue == "TRUE" )
		{
			m_bShowPathTriangle = true;
		}
		else
		{
			m_bShowPathTriangle = false;
		}

		szValue = xml.GetAttrib( "ShowPathLine" ).c_str();
		if ( szValue == "TRUE" )
		{
			m_bShowPathLine = true;
		}
		else
		{
			m_bShowPathLine = false;
		}

		szValue = xml.GetAttrib( "ShowPathEndPoint" ).c_str();
		if ( szValue == "TRUE" )
		{
			m_bShowPathEndPoint = true;
		}
		else
		{
			m_bShowPathEndPoint = false;
		}

		szValue = xml.GetAttrib( "EnableDepthTest" ).c_str();
		if ( szValue == "TRUE" )
		{
			m_bEnableDepthTest = true;
		}
		else
		{
			m_bEnableDepthTest = false;
		}

		szValue = xml.GetAttrib( "IsVisable" ).c_str();
		if ( szValue == "TRUE" )
		{
			m_bIsVisable = true;
		}
		else
		{
			m_bIsVisable = false;
		}

		xml.IntoElem();

		//--------------------------------------
		// add by hjj (2010-10-23)
		if ( m_bNeedLoadEditData )
		//--------------------------------------
		{
			if ( xml.FindElem("MapBorderPolygon") )
			{
				szValue = xml.GetAttrib( "HasGenMapBorder" ).c_str();
				if ( szValue == "TRUE" )
				{
					m_bHasGenMapBorder = true;
				}
				else
				{
					m_bHasGenMapBorder = false;
				}

				szValue = xml.GetAttrib( "EndEditMapBoarder" ).c_str();
				if ( szValue == "TRUE" )
				{
					m_bEndEditMapBoarder = true;
				}
				else
				{
					m_bEndEditMapBoarder = false;
				}

				xml.IntoElem();

				AIRVector< Vector3Df > vtMapBorderVertex;
				while( xml.FindElem() )
				{
					szValue = xml.GetAttrib( "Position" ).c_str();
					if ( szValue.length() )
					{
						Vector3Df vpos;
						sscanf( szValue.c_str(), "(%f,%f,%f)", &vpos.x, &vpos.y, &vpos.z );
						vtMapBorderVertex.push_back( vpos );
					}
					m_WholeMapPolygon = Polygon3Df( vtMapBorderVertex );
				}

				xml.OutOfElem();
			}
		}

		//--------------------------------------
		// add by hjj (2010-10-23)
		if ( m_bNeedLoadEditData )
		//--------------------------------------
		{
			if ( xml.FindElem("EditPolygonVertex") )
			{
				m_vtNavVertex.clear();

				xml.IntoElem();
				
				while ( xml.FindElem() )
				{
					NAV_VERTEX vertex;
					szValue = xml.GetAttrib("Position").c_str();
					if ( szValue.length() )
					{
						sscanf( szValue.c_str(), "(%f,%f,%f)", &vertex.m_Pos.x, &vertex.m_Pos.y, &vertex.m_Pos.z );
					}

					szValue = xml.GetAttrib("Color").c_str();
					if ( szValue.length() )
					{
						u32 a,r,g,b;
						sscanf( szValue.c_str(), "(%d,%d,%d,%d)", &a, &r, &g, &b );
						vertex.m_Color.Set( a, r, g, b );
					}
					m_vtNavVertex.push_back( vertex );
				}

				xml.OutOfElem();
			}
		}

		//--------------------------------------
		// add by hjj (2010-10-23)
		if ( m_bNeedLoadEditData )
		//--------------------------------------
		{
			if ( xml.FindElem("EditPolygonInfo") )
			{
				xml.IntoElem();

				while ( xml.FindElem() )
				{
					PolygonInfo pi;
					pi.m_Start = atoi( xml.GetAttrib( "Start" ).c_str() );
					pi.m_Len = atoi( xml.GetAttrib( "Len" ).c_str() );
					m_vtNavPolygonInfo.push_back( pi );
				}

				xml.OutOfElem();
			}
		}

		xml.OutOfElem();
	}
	return true;
}

bool NavEditor::CreateNavInfo( CMarkupSTL& xml )
{
	AIRString szValue;

	if ( xml.FindElem("NavInfo") )
	{
		xml.IntoElem();

		if ( xml.FindElem("NavMesh") )
		{
			xml.IntoElem();

			m_vtTriangle.clear();
			NavTriangle tri;
			while( xml.FindElem() )
			{
				tri.m_dwID = u32( atoi(xml.GetAttrib("ID").c_str()) );
				tri.m_byAttrib = u8( atoi(xml.GetAttrib("Attribute").c_str()) );
				xml.IntoElem();

				while( xml.FindElem() )
				{
					Vector3Df vpos;
					szValue = xml.GetAttrib( "Pos" ).c_str();
					sscanf( szValue.c_str(), "(%f,%f,%f)", &vpos.x, &vpos.y, &vpos.z );
					if ( xml.GetTagName() == "VertexA" )
					{
						tri.m_Triangle.pointA = vpos;
					}
					else if ( xml.GetTagName() == "VertexB" )
					{
						tri.m_Triangle.pointB = vpos;
					}
					else if ( xml.GetTagName() == "VertexC" )
					{
						tri.m_Triangle.pointC = vpos;
					}
				}
				m_vtTriangle.push_back( tri );

				xml.OutOfElem();
			}

			xml.OutOfElem();
		}

		xml.OutOfElem();
	}

	return true;
}



bool NavEditor::SaveToXML( const char* szNavFile )
{
	CMarkupSTL xml;
	xml.SetDoc("<?xml version=\"1.0\" encoding=\"gb2312\"?>\r\n");
	xml.AddElem("Navigator");

	xml.IntoElem();
	xml.AddNode(CMarkupSTL::MNT_TEXT, "\r\n    ");

	SaveEditInfo(xml);
	SaveNavInfo(xml);

	xml.OutOfElem();

	return xml.Save(szNavFile);
}



bool NavEditor::SaveToBinFile( const char* szNavFile )
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	std::locale loc1 = std::locale::global(std::locale(".936"));
#endif

	std::ofstream fout;
	fout.open( szNavFile, std::ios::out | std::ios::binary);
	fout.flush();

	// ��¼������С
	float fSceneSize = TERRAIN->GetCurHeightmap()->GetWidth();
	fout.write( (char*)(&fSceneSize), sizeof(float) );

	// ��ǰ�༭���ϰ����񶥵�����ֵ
	fout.write( (char*)(&m_nEditNavPointsCurMaxVertexCount), sizeof(size_t) );

	// ��һ���ϰ����񶥵����ʼ����
	fout.write( (char*)(&m_nStart), sizeof(size_t) );

	// ��ǰ���ڱ༭�ڼ����ϰ���������
	fout.write( (char*)(&m_nCurPolygon), sizeof(size_t) );

	// �༭ģʽ
	int nEditMode = (int)m_EditMode;
	fout.write( (char*)(&nEditMode), sizeof(int) );

	// ��ǰ�Ƿ���ʾ�ϰ����������
	int nShowEditNavPolygon = (int)m_bShowEditNavPolygon;
	fout.write( (char*)(&nShowEditNavPolygon), sizeof(int) );

	// ��ǰ�Ƿ���ʾ��������
	int nShowNavMesh = (int)m_bShowNavMesh;
	fout.write( (char*)(&nShowNavMesh), sizeof(int) );

	// ��ǰ�Ƿ���ʾ������·��
	int nShowPathTriangle = (int)m_bShowPathTriangle;
	fout.write( (char*)(&nShowPathTriangle), sizeof(int) );

	// ��ǰ�Ƿ���ʾѰ··��
	int nShowPathLine = (int)m_bShowPathLine;
	fout.write( (char*)(&nShowPathLine), sizeof(int) );

	// ��ǰ�Ƿ���ʾ·���˵�,
	int nShowPathEndPoint = (int)m_bShowPathEndPoint;
	fout.write( (char*)(&nShowPathEndPoint), sizeof(int) );

	// ��ǰ�Ƿ����Ȳ���,��������ģʽ��ʾ��������
	int nEnableDepthTest = (int)m_bEnableDepthTest;
	fout.write( (char*)(&nEnableDepthTest), sizeof(int) );

	// �Ƿ���ʾ�������������Ϣ
	int nIsVisable = (int)m_bIsVisable;
	fout.write( (char*)(&nIsVisable), sizeof(int) );

	// ��¼��ǰ�Ƿ��Ѿ�������ͼ�߽�
	int nHasGenMapBorder = (int)m_bHasGenMapBorder;
	fout.write( (char*)(&nHasGenMapBorder), sizeof(int) );

	// ��¼��ǰ�Ƿ��Ѿ�������ͼ�߽����εı༭
	int nEndEditMapBoarder = (int)m_bEndEditMapBoarder;
	fout.write( (char*)(&nEndEditMapBoarder), sizeof(int) );

	// �����ͼ�߽����εĸ���������Ϣ
	AIRVector< Vector3Df > vtMapBorderVertex = m_WholeMapPolygon.GetPoints();
	u32 nMapBorderVertexCount = vtMapBorderVertex.size();
	fout.write( (char*)(&nMapBorderVertexCount), sizeof(u32) );
	if ( nMapBorderVertexCount > 0 )
	{
		fout.write( (char*)(&vtMapBorderVertex[0]), sizeof(Vector3Df) * nMapBorderVertexCount );
	}

	// ��¼�༭�õ��ϰ����񶥵���Ϣ
	u32 nNavVertexCount = m_vtNavVertex.size();
	fout.write( (char*)(&nNavVertexCount), sizeof(u32) );
	if ( nNavVertexCount > 0 )
	{
		fout.write( (char*)(&m_vtNavVertex[0]), sizeof(NAV_VERTEX) * nNavVertexCount );
	}

	// ��¼�༭�õ��ϰ�����������Ϣ
	u32 nNavPolygonInfoCount = m_vtNavPolygonInfo.size();
	fout.write( (char*)(&nNavPolygonInfoCount), sizeof(u32) );
	if ( nNavPolygonInfoCount > 0 )
	{
		fout.write( (char*)(&m_vtNavPolygonInfo[0]), sizeof(PolygonInfo) * nNavPolygonInfoCount );
	}

	// ��¼��������
	u32 nNavMeshCount = m_vtTriangle.size();
	fout.write( (char*)(&nNavMeshCount), sizeof(u32) );
	if ( nNavMeshCount > 0 )
	{
		fout.write( (char*)(&m_vtTriangle[0]), sizeof(NavTriangle) * nNavMeshCount );
	}

	fout.flush();
	fout.close();
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	std::locale::global(std::locale(loc1));
#endif

	return true;
}



bool NavEditor::SaveEditInfo( CMarkupSTL& xml )
{
	// ��¼�༭������Ϣ
	xml.AddElem( "EditInfo" );

	// ��¼������С
	float fSceneSize = TERRAIN->GetCurHeightmap()->GetWidth();
	xml.AddAttrib( "SceneSize", GetString("%f", fSceneSize) );

	// ��ǰ�༭���ϰ����񶥵�����ֵ
	xml.AddAttrib( "EditNavPointsCurMaxVertexCount", m_nEditNavPointsCurMaxVertexCount );
	// ��һ���ϰ����񶥵����ʼ����
	xml.AddAttrib( "Start", m_nStart );
	// ��ǰ���ڱ༭�ڼ����ϰ���������
	xml.AddAttrib( "CurPolygon", m_nCurPolygon );

	// ��ǰ���ڱ༭��ͼ�߽�����
	if ( m_EditMode == eNEM_EditMapBorder )
	{
		xml.AddAttrib( "EditMode", "eNEM_EditMapBorder" );
	}
	// ��ǰ���ڱ༭�����ϰ������
	else if ( m_EditMode == eNEM_AddNavPolygon )
	{
		xml.AddAttrib( "EditMode", "eNEM_AddNavPolygon" );
	}
	// ��ǰ��������·���Ķ˵㲢Ѱ·
	else if ( m_EditMode == eNEM_EditPathStartEnd )
	{
		xml.AddAttrib( "EditMode", "eNEM_EditPathStartEnd" );
	}
	// Ѱ·ģʽ
	else if ( m_EditMode == eNEM_FindPath )
	{
		xml.AddAttrib( "EditMode", "eNEM_FindPath" );
	}

	// ��ǰ�Ƿ���ʾ�ϰ����������
	if ( m_bShowEditNavPolygon )
	{
		xml.AddAttrib( "ShowEditNavPolygon", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "ShowEditNavPolygon", "FALSE" );
	}

	// ��ǰ�Ƿ���ʾ��������
	if ( m_bShowNavMesh )
	{
		xml.AddAttrib( "ShowNavMesh", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "ShowNavMesh", "FALSE" );
	}

	// ��ǰ�Ƿ���ʾ������·��
	if ( m_bShowPathTriangle )
	{
		xml.AddAttrib( "ShowPathTriangle", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "ShowPathTriangle", "FALSE" );
	}

	// ��ǰ�Ƿ���ʾѰ··��
	if ( m_bShowPathLine )
	{
		xml.AddAttrib( "ShowPathLine", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "ShowPathLine", "FALSE" );
	}

	// ��ǰ�Ƿ���ʾ·���˵�,
	if ( m_bShowPathEndPoint )
	{
		xml.AddAttrib( "ShowPathEndPoint", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "ShowPathEndPoint", "FALSE" );
	}

	// ��ǰ�Ƿ����Ȳ���,��������ģʽ��ʾ��������
	if ( m_bEnableDepthTest )
	{
		xml.AddAttrib( "EnableDepthTest", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "EnableDepthTest", "FALSE" );
	}

	// �Ƿ���ʾ�������������Ϣ
	if ( m_bIsVisable )
	{
		xml.AddAttrib( "IsVisable", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "IsVisable", "FALSE" );
	}
	
	xml.IntoElem();

	// ��¼��ͼ�߽�������Ϣ
	{
		xml.AddElem( "MapBorderPolygon" );

		// ��¼��ǰ�Ƿ��Ѿ��ɳɵ�ͼ�߽�
		if ( m_bHasGenMapBorder )
		{
			xml.AddAttrib( "HasGenMapBorder", "TRUE" );
		}
		else
		{
			xml.AddAttrib( "HasGenMapBorder", "FALSE" );
		}

		// ��¼��ǰ�Ƿ��Ѿ�������ͼ�߽����εı༭
		if ( m_bEndEditMapBoarder )
		{
			xml.AddAttrib( "EndEditMapBoarder", "TRUE" );
		}
		else
		{
			xml.AddAttrib( "EndEditMapBoarder", "FALSE" );
		}

		// �����ͼ�߽����εĸ���������Ϣ
		AIRVector< Vector3Df > vtMapBorderVertex = m_WholeMapPolygon.GetPoints();
		for ( size_t i = 0; i < vtMapBorderVertex.size(); ++i )
		{
			xml.AddChildElem( "PolygonVertex" );
			xml.AddChildAttrib( "Position", GetString("(%f,%f,%f)", vtMapBorderVertex[i].x, vtMapBorderVertex[i].y, vtMapBorderVertex[i].z) );
		}
	}

	// ��¼�༭�õ��ϰ����񶥵���Ϣ
	{
		xml.AddElem( "EditPolygonVertex" );
		// �������е��ϰ�����ζ���
		for ( size_t i = 0; i < m_vtNavVertex.size(); ++i )
		{
			xml.AddChildElem( "PolygonVertex" );
			xml.AddChildAttrib( "Position", GetString("(%f,%f,%f)", m_vtNavVertex[i].m_Pos.x, m_vtNavVertex[i].m_Pos.y, m_vtNavVertex[i].m_Pos.z) );
			SColor col = m_vtNavVertex[i].m_Color.GetAsARGB();
			xml.AddChildAttrib( "Color", GetString("(%d,%d,%d,%d)",col.GetAlpha(), col.GetRed(), col.GetGreen(), col.GetBlue()) );
		}
	}

	// ��¼�༭�õ��ϰ�����������Ϣ
	{
		xml.AddElem( "EditPolygonInfo" );
		// �������е��ϰ������
		for ( size_t i = 0; i < m_vtNavPolygonInfo.size(); ++i )
		{
			xml.AddChildElem( "PolygonInfo" );
			xml.AddChildAttrib( "Start", m_vtNavPolygonInfo[i].m_Start );
			xml.AddChildAttrib( "Len", m_vtNavPolygonInfo[i].m_Len );
		}
	}

	xml.OutOfElem();

	return true;
}

bool NavEditor::SaveNavInfo( CMarkupSTL& xml )
{
	// ��¼�Ѿ����������ĵ���������Ϣ
	xml.AddElem( "NavInfo" );

	xml.IntoElem();

	// ��¼��������
	{
		xml.AddElem( "NavMesh" );
		xml.IntoElem();
		NavTriangle tri;
		// �������еĵ�������������
		for ( size_t i = 0; i < m_vtTriangle.size(); ++i )
		{
			tri = m_vtTriangle[i];
			xml.AddElem( "NavTriangle" );
			// ���������ε�ID
			xml.AddAttrib( "ID", int(tri.m_dwID) );
			// ���������ε�����
			xml.AddAttrib( "Attribute", int(tri.m_byAttrib) );
			// ���������ε�3������
			xml.AddChildElem( "VertexA" );
			xml.AddChildAttrib( "Pos", GetString("(%f,%f,%f)", tri.m_Triangle.pointA.x, tri.m_Triangle.pointA.y, tri.m_Triangle.pointA.z ) );
			xml.AddChildElem( "VertexB" );
			xml.AddChildAttrib( "Pos", GetString("(%f,%f,%f)", tri.m_Triangle.pointB.x, tri.m_Triangle.pointB.y, tri.m_Triangle.pointB.z ) );
			xml.AddChildElem( "VertexC" );
			xml.AddChildAttrib( "Pos", GetString("(%f,%f,%f)", tri.m_Triangle.pointC.x, tri.m_Triangle.pointC.y, tri.m_Triangle.pointC.z ) );
		}
		xml.OutOfElem();
	}

	xml.OutOfElem();

	return true;
}






void NavEditor::AddOperatPoint( Vector3Df& vPos, const AIRVector< CheckPoint >& vtOperatPointSet )
{
	AIRList< Vector3Df >::iterator it_pos = m_lstOperatPoints.begin();
	int id = 0;
	for ( ; it_pos != m_lstOperatPoints.end(); ++it_pos )
	{
		if ( *it_pos == vPos )
		{
			break;
		}
		++id;
	}
	if ( it_pos == m_lstOperatPoints.end() )
	{
		m_lstOperatPoints.push_back( vPos );
		m_lstOperatPointsInfo.push_back( vtOperatPointSet );
	}
	else
	{
		m_lstOperatPoints.erase( it_pos );
		AIRList< AIRVector<CheckPoint> >::iterator it_point_info = m_lstOperatPointsInfo.begin();
		std::advance( it_point_info, id );
		m_lstOperatPointsInfo.erase( it_point_info );
	}

	//--------------------------------------------------
	//! add by hjj 2010-10-20
	AIRList< u32 >::iterator itb = m_lstEdittingNavPolygonVertex.begin(), ite = m_lstEdittingNavPolygonVertex.end(), itt;
	u32 count = m_vtNavVertex.size();
	bool bExist = false;
	for ( ; itb != ite;  )
	{
		if ( *itb < count && m_vtNavVertex[ *itb ].m_Pos == vPos )
		{
			itt = itb;
			++itt;
			m_lstEdittingNavPolygonVertex.erase( itb );
			itb = itt;
			ite = m_lstEdittingNavPolygonVertex.end();
			bExist = true;
		}
		else
		{
			++itb;
		}
	}
	if ( !bExist )
	{
		for ( size_t i = 0; i < count; ++i )
		{
			if ( m_vtNavVertex[i].m_Pos == vPos )
			{
				m_lstEdittingNavPolygonVertex.push_back( i );
			}
		}
	}

	itb = m_lstEdittingMapBorderVertex.begin();
	ite = m_lstEdittingMapBorderVertex.end();
	bExist = false;
	count = m_vtMapBoarderPoints.size();
	for ( ; itb != ite;  )
	{
		if ( *itb < count && m_vtMapBoarderPoints[ *itb ].m_Pos == vPos )
		{
			itt = itb;
			++itt;
			m_lstEdittingMapBorderVertex.erase( itb );
			itb = itt;
			ite = m_lstEdittingMapBorderVertex.end();
			bExist = true;
		}
		else
		{
			++itb;
		}
	}
	if ( !bExist )
	{
		for ( u32 i = 0; i < count; ++i )
		{
			if ( m_vtMapBoarderPoints[i].m_Pos == vPos )
			{
				m_lstEdittingMapBorderVertex.push_back( i );
			}
		}
	}
	//--------------------------------------------------

}

void NavEditor::ClearOperatPointsAndInfo( void )
{
	m_lstOperatPoints.clear();
	m_lstOperatPointsInfo.clear();

	//--------------------------------------------------
	//! add by hjj 2010-10-20
	m_lstEdittingNavPolygonVertex.clear();
	m_lstEdittingMapBorderVertex.clear();
	//--------------------------------------------------
}

void NavEditor::MoveOperatPoints( float fOffset )
{
	AIRList< Vector3Df >::iterator it_pos = m_lstOperatPoints.begin();
	AIRList< AIRVector<CheckPoint> >::iterator it_point_info = m_lstOperatPointsInfo.begin();
	for ( ; it_point_info != m_lstOperatPointsInfo.end() && it_pos != m_lstOperatPoints.end(); ++it_point_info, ++it_pos )
	{
		AIRVector<CheckPoint>& vtCheckPoints = *it_point_info;
		for ( size_t i = 0; i < vtCheckPoints.size(); ++i )
		{
			CheckPoint cp = vtCheckPoints[i];
			switch ( cp.m_nVertexID )
			{
			case 0:
				m_vtTriangle[ cp.m_nTriangleID ].m_Triangle.pointA.y += fOffset;
				*it_pos = m_vtTriangle[ cp.m_nTriangleID ].m_Triangle.pointA;
				break;
			case 1:
				m_vtTriangle[ cp.m_nTriangleID ].m_Triangle.pointB.y += fOffset;
				*it_pos = m_vtTriangle[ cp.m_nTriangleID ].m_Triangle.pointB;
				break;
			case 2:
				m_vtTriangle[ cp.m_nTriangleID ].m_Triangle.pointC.y += fOffset;
				*it_pos = m_vtTriangle[ cp.m_nTriangleID ].m_Triangle.pointC;
				break;
			}
		}
	}
	RefreshNavInfo();

	//--------------------------------------------------
	//! add by hjj 2010-10-20
	AIRList< u32 >::iterator itb = m_lstEdittingNavPolygonVertex.begin(), ite = m_lstEdittingNavPolygonVertex.end();
	for ( ; itb != ite; ++itb )
	{
		m_vtNavVertex[ *itb ].m_Pos.y += fOffset;
	}
	PrepareVBEditNavPoints();

	itb = m_lstEdittingMapBorderVertex.begin();
	ite = m_lstEdittingMapBorderVertex.end();
	for ( ; itb != ite; ++itb )
	{
		m_vtMapBoarderPoints[ *itb ].m_Pos.y += fOffset;
	}
	//--------------------------------------------------

}




bool NavEditor::GetNearestEyeRayIntersetPointOfNavMesh( Line3Df& eyeRay, Vector3Df& vOut, NavTriangle& NavTri )
{
	// ��ѯ���
	bool		bFindIntersetPoint = false;
	Vector3Df	vTemp;
	float		fDistance = float( 0xfffffffe );
	float		fTempDistance = 0.0f;
	NavTriangle nav_tri;
	Vector3Df	vEyePos = ENGINE_INST->GetSceneMgr()->GetCamera()->GetPosition();

	// �������еĵ���������
	for ( size_t i = 0; i < m_vtTriangle.size(); ++i )
	{
		// ȡ�õ�ǰ��������Ϣ
		nav_tri = m_vtTriangle[i];
		// ���������ཻ����
		if ( nav_tri.m_Triangle.GetIntersectionWithLimitedLine( eyeRay, vTemp ) )
		{
			// ����н���,���ò��Խ��Ϊtrue
			bFindIntersetPoint = true;
			// ���㽻�㵽������ľ���
			vOut = vTemp - vEyePos;
			fTempDistance = vOut.GetLength();
			// ����ý��������������
			if ( fTempDistance < fDistance )
			{
				// ���õ�ǰ����С����
				fDistance = fTempDistance;
				// ��¼����
				vOut = vTemp;
				// ��¼��ǰ����������Ϣ
				NavTri = nav_tri;
			}
		}
	}
	return bFindIntersetPoint;
}


void NavEditor::FlattenOperatPoints( FlattenMode fm, float height )
{
	if ( m_lstOperatPoints.size() == 0 )
	{
		return;
	}

	if ( fm == eFM_MIN )
	{
		AIRList< Vector3Df >::iterator it_pos = m_lstOperatPoints.begin();
		height = (*it_pos).y;
		++it_pos;
		for ( ; it_pos != m_lstOperatPoints.end(); ++it_pos )
		{
			if ( height > (*it_pos).y )
			{
				height = (*it_pos).y;
			}
		}
	}
	else if ( fm == eFM_MAX )
	{
		AIRList< Vector3Df >::iterator it_pos = m_lstOperatPoints.begin();
		height = (*it_pos).y;
		++it_pos;
		for ( ; it_pos != m_lstOperatPoints.end(); ++it_pos )
		{
			if ( height < (*it_pos).y )
			{
				height = (*it_pos).y;
			}
		}
	}

	AIRList< Vector3Df >::iterator it_pos = m_lstOperatPoints.begin();
	AIRList< AIRVector<CheckPoint> >::iterator it_point_info = m_lstOperatPointsInfo.begin();
	for ( ; it_point_info != m_lstOperatPointsInfo.end() && it_pos != m_lstOperatPoints.end(); ++it_point_info, ++it_pos )
	{
		AIRVector<CheckPoint>& vtCheckPoints = *it_point_info;
		for ( size_t i = 0; i < vtCheckPoints.size(); ++i )
		{
			CheckPoint cp = vtCheckPoints[i];
			switch ( cp.m_nVertexID )
			{
			case 0:
				m_vtTriangle[ cp.m_nTriangleID ].m_Triangle.pointA.y = height;
				*it_pos = m_vtTriangle[ cp.m_nTriangleID ].m_Triangle.pointA;
				break;
			case 1:
				m_vtTriangle[ cp.m_nTriangleID ].m_Triangle.pointB.y = height;
				*it_pos = m_vtTriangle[ cp.m_nTriangleID ].m_Triangle.pointB;
				break;
			case 2:
				m_vtTriangle[ cp.m_nTriangleID ].m_Triangle.pointC.y = height;
				*it_pos = m_vtTriangle[ cp.m_nTriangleID ].m_Triangle.pointC;
				break;
			}
		}
	}
	RefreshNavInfo();

	//--------------------------------------------------
	//! add by hjj 2010-10-20
	AIRList< u32 >::iterator itb = m_lstEdittingNavPolygonVertex.begin(), ite = m_lstEdittingNavPolygonVertex.end();
	for ( ; itb != ite; ++itb )
	{
		m_vtNavVertex[ *itb ].m_Pos.y = height;
	}
	PrepareVBEditNavPoints();

	itb = m_lstEdittingMapBorderVertex.begin();
	ite = m_lstEdittingMapBorderVertex.end();
	for ( ; itb != ite; ++itb )
	{
		m_vtMapBoarderPoints[ *itb ].m_Pos.y = height;
	}
	//--------------------------------------------------
}



bool NavEditor::PickEditNavPolygonAndPoint( float fRX, float fRZ, int& nPolygonID, float fErrTor )
{
	Vector2Df vTest;
	Vector2Df vPick( fRX, fRZ );
	for ( nPolygonID = 0; nPolygonID < m_vtNavPolygonInfo.size(); ++nPolygonID )
	{
		PolygonInfo pi = m_vtNavPolygonInfo[nPolygonID];
		for ( u32 j = pi.m_Start; j < pi.m_Start+pi.m_Len; ++j )
		{
			if ( j >= m_vtNavVertex.size() )
			{
				continue;
			}
			vTest.Set( m_vtNavVertex[j].m_Pos.x, m_vtNavVertex[j].m_Pos.z );
			if ( vTest.GetDistanceFrom(vPick) <= fErrTor )
			{
				m_nCurPickPolygon = nPolygonID;
				m_bPickPolygon = true;
				return true;
			}
		}
	}

	nPolygonID = -1;
	m_nCurPickPolygon = -1;
	m_bPickPolygon = false;
	return false;
}



bool NavEditor::PickEditNavPolygonAndPoint( Line3Df& line, int& nPolygonID, float fErrTor /*= 10.0f*/ )
{
	for ( nPolygonID = 0; nPolygonID < m_vtNavPolygonInfo.size(); ++nPolygonID )
	{
		PolygonInfo pi = m_vtNavPolygonInfo[nPolygonID];
		for ( u32 j = pi.m_Start; j <= pi.m_Start+pi.m_Len && j < m_vtNavVertex.size(); ++j )
		{
			if ( line.GetDistanceFromPointToLine( m_vtNavVertex[j].m_Pos ) <= fErrTor )
			{
				m_nCurPickPolygon = nPolygonID;
				m_bPickPolygon = true;
				return true;
			}
		}
	}

	nPolygonID = -1;
	m_nCurPickPolygon = -1;
	m_bPickPolygon = false;
	return false;
}



// ɾ����ǰ��ѡ�����ϰ���������
void NavEditor::DelCurPickEditNavPolygon( int nEditNavPolygon )
{
	if ( nEditNavPolygon == -1 || nEditNavPolygon >= m_vtNavPolygonInfo.size() )
	{
		return;
	}
	// ȡ��Ҫɾ���ĵ�nEditNavPolygon���༭�������Ϣ
	PolygonInfo pi = m_vtNavPolygonInfo[nEditNavPolygon];
	// ����Ҫ�ƶ���Ŀ����ʼ����
	int nDstStart = pi.m_Start + pi.m_Len;
	// ����Ҫ�ŵ�����ʼ����
	int nSrcStart = pi.m_Start;
	// ����Ҫ�ƶ���Ŀ���������
	int nDstEnd = m_vtNavVertex.size();
	// Ҫɾ���Ķ������
	u32 nCount = pi.m_Len;

	// �������ƶ���nDstStart��nDstEnd�����ж���
	for ( ; nDstStart < nDstEnd; ++nDstStart, ++nSrcStart )
	{
		m_vtNavVertex[nSrcStart] = m_vtNavVertex[nDstStart];
	}
	// ɾ�������nCount���ڵ�
	for ( u32 i = 0; i < nCount; ++i )
	{
		m_vtNavVertex.pop_back();
	}

	// ɾ���������Ϣ
	for ( size_t i = nEditNavPolygon; i < m_vtNavPolygonInfo.size()-1; ++i )
	{
		m_vtNavPolygonInfo[i] = m_vtNavPolygonInfo[i+1];
		m_vtNavPolygonInfo[i].m_Start -= nCount;
	}
	m_vtNavPolygonInfo.pop_back();
	m_nCurPolygon--;
	m_nStart = m_vtNavPolygonInfo[m_nCurPolygon].m_Start + m_vtNavPolygonInfo[m_nCurPolygon].m_Len;  

	ClearPickEditNavPolygon();

	PrepareVBEditNavPoints();
}


void NavEditor::ClearPickEditNavPolygon( void )
{
	m_nCurPickPolygon = -1;
	m_bPickPolygon = false;
}

void NavEditor::ClearPickNavTriangle( void )
{
	m_bHasPickTriangle = false;
}

void NavEditor::ClearCurCheckPoint( void )
{
	m_vtCurCheckPoints.clear();
	m_bPickCheckPoint = false;
}



// �����ɵĵ����������¼��nGridCountPerSide * nGridCountPerSide��������������
bool NavEditor::OptimiseNavMesh( int nGridCountPerSide )
{
	if ( !TERRAIN->GetCurHeightmap() )
	{
		DT_TO_MSGBOX_FMT( LT_WARN, "Did not create terrain!" );
		return false;
	}
	if ( m_vtTriangle.size() == 0 )
	{
		DT_TO_MSGBOX_FMT( LT_WARN, "Did not create nav mesh triangle!" );
		return false;
	}
	// ȡ������Ĵ�С
	float fTerrainSize = TERRAIN->GetCurHeightmap()->GetWidth();
	// ȡ������Ĵ�С��һ��
	float fHalfSize = fTerrainSize / 2.0f;
	// ȡ��ÿ����������������С
	float fGridSize = fTerrainSize / float(nGridCountPerSide);
	m_fGridSize = fGridSize;
	m_nGridCountPerSide = nGridCountPerSide;

	// ��������ľ�������
	m_rcWorld.SetRect( -fHalfSize, -fHalfSize, fHalfSize, fHalfSize );

	// ��¼ÿ�������������������Ϣ������
	m_vtOptimistNavMeshInfo.clear();
	for ( int r = 0; r < nGridCountPerSide; ++r )
	{
		for ( int c = 0; c < nGridCountPerSide; ++c )
		{
			AIRVector< int > vtTmpTriID;
			m_vtOptimistNavMeshInfo.push_back( vtTmpTriID );
		}
	}

	Triangle2Df tri;
	float left, top, right, bottom;
	float rcl, rct, rcr, rcb;
	int rs, re, cs, ce;
	// ����ÿ������������
	for ( size_t i = 0; i < m_vtTriangle.size(); ++i )
	{
		// ȡ�øõ������������
		Triangle3Df tri = m_vtTriangle[i].m_Triangle;

		// ������������ˮƽ���ϵ�ͶӰ
		Triangle2Df triProj = tri.GetHorzProjTriangle2D(); 

		// �������������ˮƽ���ϵľ��ΰ�Χ��
		left   = GetMin( tri.pointA.x, tri.pointB.x, tri.pointC.x );
		top    = GetMin( tri.pointA.z, tri.pointB.z, tri.pointC.z );
		right  = GetMax( tri.pointA.x, tri.pointB.x, tri.pointC.x );
		bottom = GetMax( tri.pointA.z, tri.pointB.z, tri.pointC.z );

		// ������ΰ�Χ������Խ�ĸ�������
		// ��ʼ������
		cs = int( left + fHalfSize )   / int(fGridSize);
		// ����������
		ce = int( right + fHalfSize )  / int(fGridSize);
		// ��ʼ������
		rs = int( fHalfSize - bottom ) / int(fGridSize);
		// ����������
		re = int( fHalfSize - top )    / int(fGridSize);

		// ���к��е���ֵ������������Χ��
		cs = (cs >= m_nGridCountPerSide ? m_nGridCountPerSide-1 : cs);
		ce = (ce >= m_nGridCountPerSide ? m_nGridCountPerSide-1 : ce);
		rs = (rs >= m_nGridCountPerSide ? m_nGridCountPerSide-1 : rs);
		re = (re >= m_nGridCountPerSide ? m_nGridCountPerSide-1 : re);

		// ��������Խ�ĸ�������
		for ( int r = rs; r <= re; ++r )
		{
			for ( int c = cs; c <= ce; ++c )
			{
				//--------------------------------------------------
				//! add by hjj 2010-8-4
				// ��������ӵ������С
				// ������߽�
				rcl = c * m_fGridSize - fHalfSize;
				// �����ϱ߽�
				rct = fHalfSize - (r+1) * m_fGridSize;
				// �����ұ߽�
				rcr = rcl + m_fGridSize;
				// �����±߽�
				rcb = rct + m_fGridSize;

				Rectf rcGrid( rcl, rct, rcr, rcb );
				rcGrid.Normalize();

				// �жϸ��Ӿ����뵼���������Ƿ��ཻ
				bool bIntersect = IsRectIntersectTriangle( rcGrid, triProj );
				//--------------------------------------------------

				if ( bIntersect )
				{
					// ���������ε�ID��¼����
					int id = r * nGridCountPerSide + c;
					m_vtOptimistNavMeshInfo[ id ].push_back( i );
				}
			}
		}

	}

	return true;
}



// ��������(fWorldX,fWorldZ)�ж��Ƿ��л���ĳ������������
bool NavEditor::OptimiseFindIntersectNavMesh( float fWorldX, float fWorldZ, NavTriangle& NavTri )
{
	if ( m_vtOptimistNavMeshInfo.size() == 0 )
	{
		m_bHasPickTriangle = false;
		return false;
	}
	if ( !m_rcWorld.IsInRect( fWorldX, fWorldZ ) )
	{
		m_bHasPickTriangle = false;
		return false;
	}
	int c = int(fWorldX - m_rcWorld.left) / int(m_fGridSize);
	int r = int(m_rcWorld.bottom - fWorldZ) / int(m_fGridSize);
	int id = r * m_nGridCountPerSide + c;
	AIRVector< int > vtTmpNavMesh = m_vtOptimistNavMeshInfo[ id ];

	// ���Ƚ��о�ȷ�ز���
	for ( size_t i = 0; i < vtTmpNavMesh.size(); ++i )
	{
		NavTri = m_vtTriangle[ vtTmpNavMesh[i] ];
		if ( NavTri.m_Triangle.Is2DPointInHorzProjTriangle(fWorldX, fWorldZ) )
		{
			m_CurPickTriangle = NavTri;
			m_bHasPickTriangle = true;
			return true;
		}
	}

	// ��ȷ�����Ҳ���,�ͽ���ģ������,ֻҪ�㵽�����α߽�ľ�����5����λ֮��,���Ѹõ㵱����������֮��
	for ( size_t i = 0; i < vtTmpNavMesh.size(); ++i )
	{
		NavTri = m_vtTriangle[ vtTmpNavMesh[i] ];
		if ( NavTri.m_Triangle.Is2DPointInHorzProjTriangle(fWorldX, fWorldZ, 5.0) )
		{
			m_CurPickTriangle = NavTri;
			m_bHasPickTriangle = true;
			return true;
		}
	}

	// ģ������Ҳ�Ҳ���,�ͷ���false,��ʾ�㲻�ڵ�������֮��
	m_bHasPickTriangle = false;
	return false;
}


bool NavEditor::IsPointValid( float fWorldX, float fWorldZ )
{
	NavTriangle tri;
	return OptimiseFindIntersectNavMesh( fWorldX, fWorldZ, tri );
}


float NavEditor::GetWorldPosHeight( float fWorldX, float fWorldZ )
{
	COTSceneManager* pSceneMgr = (COTSceneManager*)ENGINE_INST->GetSceneMgr();
	float fHeight = 0;

	if ( GetHasGenNavMesh() )
	{
		NavTriangle nav_tri;
		// ��ý�ɫ���ڵĵ���������
		if ( OptimiseFindIntersectNavMesh( fWorldX, fWorldZ, nav_tri ) )
		{
			// ������������������Ϊ0����ʾ��ɫ��վ�ڵ�������
			if ( nav_tri.m_byAttrib == 0 )
			{
				// ��ý�ɫ���ڵ���ĸ߶�
				fHeight = pSceneMgr->GetHeightMap()->GetHeight( fWorldX, fWorldZ );
			}
			// ����,��ʾ��ɫվ�ڵ�����������
			else
			{
				// �����ɫ���ڵ�Ĵ�ֱ���뵼������Ľ���
				fHeight = -float( 0xffffff );
				Vector3Df out;
				if ( nav_tri.m_Triangle.GetIntersectionWithLine( Vector3Df(fWorldX, 0, fWorldZ ), Vector3Df::UNIT_Y, out ) )
				{
					fHeight = out.y;
				}						
			}
		}
	}
	else
	{
		// ��ý�ɫ���ڵ���ĸ߶�
		if( pSceneMgr->GetHeightMap() )
		{
			fHeight = pSceneMgr->GetHeightMap()->GetHeight( fWorldX, fWorldZ );
		}
	}
	return fHeight;
}

void NavEditor::_GetPrimitive()
{
    if (m_pTrianglePrimitive == NULL)
    {
        m_pTrianglePrimitive = new Primitive(Primitive::Primitive_TriangleList);
        m_pTrianglePrimitive->SetUseUserPrimitive(true);
    }
    m_pTrianglePrimitive->m_pVertexBuffer = m_pVBNavMeshBackLine;
    //if (m_pVBNavMeshBackLine)
    //    m_pTrianglePrimitive->m_pVertexDecl = m_pVBNavMeshBackLine->GetVertexDeclaration();
    m_pTrianglePrimitive->SetPrimitiveCount(m_vtTriangle.size());

    if (m_pLinePrimitive == NULL)
    {
        m_pLinePrimitive = new Primitive(Primitive::Primitive_ListStrip);
        m_pLinePrimitive->SetUseUserPrimitive(true);
    }
    if (m_vtEditingPolygonVertex.size() > 0)
    {
        m_pLinePrimitive->m_pVertexData = &(m_vtEditingPolygonVertex[0]);
    }
    
    if (m_pVBEditNavPoints)
        //m_pLinePrimitive->m_pVertexDecl = m_pVBEditNavPoints->GetVertexDeclaration();
    m_pLinePrimitive->SetPrimitiveCount(m_vtEditingPolygonVertex.size() - 1);
    m_pLinePrimitive->m_nVerticesStride = sizeof(NAV_VERTEX);

    if (m_pPointPrimitive == NULL)
    {
        m_pPointPrimitive = new Primitive(Primitive::Primitive_Point);
        m_pPointPrimitive->SetUseUserPrimitive(true);
		//m_pPointPrimitive->m_pVertexDecl = m_pVertexDecl;
    }
    m_pPointPrimitive->SetPrimitiveCount(m_lstOperatPoints.size());
}
