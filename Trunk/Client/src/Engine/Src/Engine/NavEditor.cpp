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

// 获得3个数中的最大值
template< class T >
T GetMax( T a, T b, T c )
{
	T t = AIR::Max( a, b );
	t = AIR::Max( t, c );
	return t;
}

// 获得3个数中的最小值
template< class T >
T GetMin( T a, T b, T c )
{
	T t = AIR::Min( a, b );
	t = AIR::Min( t, c );
	return t;
}


// 判断2条线段是否相交
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

	// 相互跨立
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



// 判断矩形与三角形是否相交
bool IsRectIntersectTriangle( Rectf& rc, Triangle2Df& tri )
{
	// 计算三角形的矩形包围盒
	Rectf triBB;
	triBB.left   = GetMin( tri.m_pt0.x, tri.m_pt1.x, tri.m_pt2.x );
	triBB.top    = GetMin( tri.m_pt0.y, tri.m_pt1.y, tri.m_pt2.y );
	triBB.right  = GetMax( tri.m_pt0.x, tri.m_pt1.x, tri.m_pt2.x );
	triBB.bottom = GetMax( tri.m_pt0.y, tri.m_pt1.y, tri.m_pt2.y );

	// 如果三角形的矩形包围盒和矩形相离,直接返回false
	if ( !rc.IsCross( triBB ) )
	{
		return false;
	}

	// 如果三角形任意一个顶点在矩形内,直接返回true
	if ( rc.IsIn( tri.m_pt0 ) || rc.IsIn( tri.m_pt1 ) || rc.IsIn( tri.m_pt2 ) )
	{
		return true;
	}

	// 如果矩形的任意一个顶点在三角形内,直接返回true
	if ( tri.IsInTriangle( Vector2Df(rc.left,  rc.top ) ) ||
		 tri.IsInTriangle( Vector2Df(rc.right, rc.top ) ) ||
		 tri.IsInTriangle( Vector2Df(rc.right, rc.bottom ) ) ||
		 tri.IsInTriangle( Vector2Df(rc.left,  rc.bottom ) ) )
	{
		return true;
	}

	// 矩形的4条线段
	Vector2Df rcLineSeg[8] = 
	{
		Vector2Df( rc.left,  rc.top ),    Vector2Df( rc.right, rc.top ),
		Vector2Df( rc.right, rc.top ),    Vector2Df( rc.right, rc.bottom ),
		Vector2Df( rc.right, rc.bottom ), Vector2Df( rc.left,  rc.bottom ),
		Vector2Df( rc.left,  rc.bottom ), Vector2Df( rc.left,  rc.top ), 
	};

	// 三角形的3条线段
	Vector2Df triLineSeg[6] = 
	{
		tri.m_pt0, tri.m_pt1,
		tri.m_pt1, tri.m_pt2,
		tri.m_pt2, tri.m_pt0,
	};

	bool bIntersect = false;
	// 如果矩形的4条边的任意一条与三角面的3条边的任意一条相交的话,那么三角形就与矩形相交
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
	// 创建一个多边形信息结构体
	PolygonInfo polyinfo;

	// 设置当前多边形的起始顶点ID
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
	// 插入一个导航网点到编辑点列表
	NAV_VERTEX vertex;
	vertex.m_Pos.Set( x, y, z );
	vertex.m_Color = SColor(m_colEditPolygon);
	m_vtNavVertex.push_back( vertex );
	m_vtNavPolygonInfo[m_nCurPolygon].m_Len++;

	// 更新编辑导航点的顶点缓冲
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


// 取消添加障碍网格多边形顶点或地图边界多边形顶点
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

	// 连接当前多边形,将当前多边形的尾节点加入到导航点序列中
	m_vtNavVertex.push_back( m_vtNavVertex[m_nStart] );
	m_vtNavPolygonInfo[ m_nCurPolygon ].m_Len++;

	// 更新编辑导航点的顶点缓冲
	PrepareVBEditNavPoints();

	// 准备编辑下一个多边形
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
	// 设置当前整个地图的最大包围多边形
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
	// 检查是否已经生成整个地图的最大包围多边形
	if ( m_WholeMapPolygon.GetPoints().size() == 0 )
	{
		DT_TO_MSGBOX_FMT( LT_WARN, "Whole map polygon is empty!" );
		return false;
	}

	// 检查并调整地图边界多边形
	CheckMapBorderValid();

	Navigater nav;
	AIRVector<Polygon3Df> allPolygon;

	Polygon3Df po;
	AIRVector<Vector3Df>  allVertexOfThePolygon;

	// 将整个地图的最大包围多边形放进所有的多边形列表中
	allPolygon.push_back( m_WholeMapPolygon ); 

	// 遍历所有的多边形信息
	for (size_t i = 0 ; i < m_vtNavPolygonInfo.size()-1; i++)
	{
		allVertexOfThePolygon.clear();              
		for (	u32 j = m_vtNavPolygonInfo[i].m_Start; 
			j < m_vtNavPolygonInfo[i].m_Start + m_vtNavPolygonInfo[i].m_Len - 1; 
			j++	)
		{
			// 取得当前多边形的所有顶点
			allVertexOfThePolygon.push_back( m_vtNavVertex[j].m_Pos );
		}
		// 记录当前多边形
		allPolygon.push_back( Polygon3Df(allVertexOfThePolygon) );
	}

	// 创建导航网格
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

	// 创建导航网格的显示数据
	PreparemVBNavMesh();

	// 创建导航网格寻路器
	m_pNavPathFinder = new NavPathFinder( m_vtTriangle );

    return true;
}


bool NavEditor::FindPathOfTriangle( Vector3Df start, Vector3Df end )
{
	// 检查是否已经创建NAV寻路器
	if ( !m_pNavPathFinder )
	{
		DT_TO_MSGBOX_FMT( LT_WARN, GetString("NavPathFinder is not exist!") );
		return false;
	}

	// 清空NAV寻路器上次寻路的结果
	m_pNavPathFinder->Clear();
	// 清空三角形路径存储器
	m_vtTPath.clear();
	// 寻找三角形路径
	m_vtTPath = m_pNavPathFinder->FindPathOfTriangle( start, end );

	// 如果三角形路径存储器不为空,准备显示三角形路径的显示数据
	if ( m_vtTPath.size() > 0 )
	{
		PrepareVBPathTriangle();
		return true;
	}
	return false;
}


bool NavEditor::FindPathOfLine( Vector3Df start, Vector3Df end )
{
	// 检查是否已经创建NAV寻路器
	if ( !m_pNavPathFinder )
	{
		DT_TO_MSGBOX_FMT( LT_WARN, GetString("NavPathFinder is not exist!") );
		return false;
	}

	// 清空NAV寻路器上次寻路的结果
	m_pNavPathFinder->Clear();
	// 清空顶点路径存储器
	m_vtPathLine.clear();

	// 逆向查找三角形路径是否存在
	if ( !FindPathOfTriangle( end, start ) )
	{
		m_vtPathLine.clear();
		return false;
	}	

	// 根据查找到的三角形路径计算出顶点路径
	m_vtPathLine = m_pNavPathFinder->FindPathOfLine( start, end );

	// 准备顶点路径的显示信息
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

				// 显示编辑中的障碍导航网格多边形
				if ( m_bShowEditNavPolygon )
				{
					// 画障碍网格
					RenderEditNavPoints();
					// 画地图边界
					RenderMapBorder();
					// 画选中的障碍网格
					RenderPickEditNavPolygon();
				}

				// 显示生成的导航网格
				if ( m_bShowNavMesh )
				{
					RenderNavMesh();
				}

				// 显示寻找到的三角形路径
				if ( m_bShowPathTriangle )
				{
					RenderPathTriangle();
				}

				// 显示寻找到的顶点路径
				if ( m_bShowPathLine )
				{
					RenderPathLine();
				}

				// 显示寻路的起始点以及结束点
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
	// 如果编辑的导航网点数量为0，返回
	if ( m_vtNavVertex.size() == 0 )
	{
		return;
	}
	IRenderer* pRender = ENGINE_INST->GetRenderer();

	// 创建编辑的导航网点顶点缓冲
	if ( !m_pVBEditNavPoints )
	{
		//m_pVBEditNavPoints = pRender->CreateVertexBuffer( m_pVertexDecl, m_nEditNavPointsCurMaxVertexCount );
        m_pVBEditNavPoints = pRender->CreateVertexBuffer( m_nEditNavPointsCurMaxVertexCount, sizeof(NAV_VERTEX) );
	}

	// 如果导航网点数量超过缓冲的大小,重建缓冲
	if ( m_vtNavVertex.size() >= m_nEditNavPointsCurMaxVertexCount )
	{
		m_nEditNavPointsCurMaxVertexCount += C_PAGE_VERTEX_COUNT;
		pRender->ClearVertexBuffer( m_pVBEditNavPoints );				
		//m_pVBEditNavPoints = pRender->CreateVertexBuffer( m_pVertexDecl, m_nEditNavPointsCurMaxVertexCount );
         m_pVBEditNavPoints = pRender->CreateVertexBuffer( m_nEditNavPointsCurMaxVertexCount, sizeof(NAV_VERTEX) );
	}

	// 将数据保存到顶点缓冲区
	void* pVertex = m_pVBEditNavPoints->Lock();

	u32 size = m_vtNavVertex.size() * sizeof(NAV_VERTEX);
	memcpy( pVertex, &(m_vtNavVertex[0]), size );

	m_pVBEditNavPoints->Unlock();
}

void NavEditor::RenderEditNavPoints( void )
{
	// 如果编辑的导航网点数量为0，返回
	if ( m_vtNavVertex.size() == 0 )
	{
		return;
	}

	IRenderer* pRender = ENGINE_INST->GetRenderer();

	//pRender->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
    pRender->SetFillMode(RSFM_Solid);

	// 设置当前要画的顶点缓冲区
	pRender->SetVertexBuffer( 0, m_pVBEditNavPoints );

	size_t i = 0;
	for ( ; i < m_vtNavPolygonInfo.size()-1; ++i )
	{
		// 画多边形
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

	// 处理最后一个多边形
	PolygonInfo pi = m_vtNavPolygonInfo[ i ];

	// 如果最后一个多变形的顶点数大于0
	if ( pi.m_Len > 0 )
	{
		m_vtEditingPolygonVertex.clear();
		u32 nEndID = pi.m_Start + pi.m_Len;
		for ( u32 j = pi.m_Start; j < nEndID; ++j )
		{
			m_vtEditingPolygonVertex.push_back( m_vtNavVertex[j] );
		}
		// 插入一个候选活动点
		NAV_VERTEX vertex;
		vertex.m_Pos = m_vCandidateEditPoint;
		vertex.m_Color = m_colEditPolygon;
		m_vtEditingPolygonVertex.push_back( vertex );

		// 画最后一个多边形
		//pRender->DrawLineStrip( &(m_vtEditingPolygonVertex[0]), m_vtEditingPolygonVertex.size()-1, sizeof(NAV_VERTEX) );
        m_pLinePrimitive->m_pVertexData = &(m_vtEditingPolygonVertex[0]);
        m_pLinePrimitive->m_pVertexBuffer = NULL;
        m_pLinePrimitive->SetPrimitiveCount(m_vtEditingPolygonVertex.size() - 1);
        //m_pLinePrimitive->Draw(NULL);
	}
}


void NavEditor::PreparemVBNavMesh( void )
{
	// 如果编辑的导航网点数量为0，返回
	if ( m_vtTriangle.size() == 0 )
	{
		m_vtTVertex.clear();
		m_vtTVertexBackLine.clear();
		return;
	}
	IRenderer* pRender = ENGINE_INST->GetRenderer();
	
	u32 nCurMaxCount = m_vtTriangle.size() * 3;
	// 创建导航网格顶点缓冲
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
	// 创建背景导航网格顶点缓冲
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

	// 取得导航网格里所有三角形的所有顶点
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

	// 将数据保存到顶点缓冲区
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
	// 如果编辑的导航网点数量为0，返回
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

	// 将数据保存到顶点缓冲区
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
	// 如果编辑的导航网点数量为0，返回
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

	// 将数据保存到顶点缓冲区
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


// 重置导航网格信息
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



// 刷新导航网格信息
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

	// 首先进行精确地测试
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

	// 精确测试找不到,就进行模糊测试,只要点到三角形边界之一的距离在5个单位之内,都把该点当做在三角形之内
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

	//第一步，进入根结点
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

        // 场景大小
        float fSceneSize = *(float*)(pDataPtr);
        pDataPtr += sizeof(float);

        // 当前编辑的障碍网格顶点的最大值
        m_nEditNavPointsCurMaxVertexCount = *(size_t*)(pDataPtr);
        pDataPtr += sizeof(size_t);

        // 下一个障碍网格顶点的起始索引
        m_nStart = *(size_t*)(pDataPtr);
        pDataPtr += sizeof(size_t);

        // 当前正在编辑第几个障碍网格多边形
        m_nCurPolygon = *(size_t*)(pDataPtr);
        pDataPtr += sizeof(size_t);

        // 编辑模式
        m_EditMode = NavEditorMode( *(int*)(pDataPtr) );
        pDataPtr += sizeof(int);

        // 当前是否显示障碍导航多边形
        m_bShowEditNavPolygon = bool( *(int*)(pDataPtr) );
        pDataPtr += sizeof(int);

        // 当前是否显示导航网格
        m_bShowNavMesh = bool( *(int*)(pDataPtr) );
        pDataPtr += sizeof(int);

        // 当前是否显示三角形路径
        m_bShowPathTriangle = bool( *(int*)(pDataPtr) );
        pDataPtr += sizeof(int);

        // 当前是否显示寻路路径
        m_bShowPathLine = bool( *(int*)(pDataPtr) );
        pDataPtr += sizeof(int);

        // 当前是否显示路径端点
        m_bShowPathEndPoint = bool( *(int*)(pDataPtr) );
        pDataPtr += sizeof(int);

        // 当前是否打开深度测试,以正常的模式显示导航网格
        m_bEnableDepthTest = bool( *(int*)(pDataPtr) );
        pDataPtr += sizeof(int);

        // 是否显示导航网格调试信息]
        m_bIsVisable = bool( *(int*)(pDataPtr) );
        pDataPtr += sizeof(int);

        // 记录当前是否已经创建地图边界
        m_bHasGenMapBorder = bool( *(int*)(pDataPtr) );
        pDataPtr += sizeof(int);

        // 记录当前是否已经结束地图边界多边形的编辑
        m_bEndEditMapBoarder = bool( *(int*)(pDataPtr) );
        pDataPtr += sizeof(int);

        // 地图边界多边形的各个顶点信息
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

        // 编辑好的障碍网格顶点信息
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

        // 编辑好的障碍网格多边形信息
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

        // 记录导航网格
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

        // 创建导航网格编辑的显示数据
        PrepareVBEditNavPoints();

        if ( m_vtTriangle.size() )
        {
            // 创建导航网格的显示数据
            PreparemVBNavMesh();

            // 创建导航网格寻路器
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

	// 场景大小
	float fSceneSize = *(float*)(pDataPtr);
	pDataPtr += sizeof(float);

	// 当前编辑的障碍网格顶点的最大值
	m_nEditNavPointsCurMaxVertexCount = *(size_t*)(pDataPtr);
	pDataPtr += sizeof(size_t);

	// 下一个障碍网格顶点的起始索引
	m_nStart = *(size_t*)(pDataPtr);
	pDataPtr += sizeof(size_t);

	// 当前正在编辑第几个障碍网格多边形
	m_nCurPolygon = *(size_t*)(pDataPtr);
	pDataPtr += sizeof(size_t);

	// 编辑模式
	m_EditMode = NavEditorMode( *(int*)(pDataPtr) );
	pDataPtr += sizeof(int);

	// 当前是否显示障碍导航多边形
	m_bShowEditNavPolygon = bool( *(int*)(pDataPtr) );
	pDataPtr += sizeof(int);

	// 当前是否显示导航网格
	m_bShowNavMesh = bool( *(int*)(pDataPtr) );
	pDataPtr += sizeof(int);

	// 当前是否显示三角形路径
	m_bShowPathTriangle = bool( *(int*)(pDataPtr) );
	pDataPtr += sizeof(int);

	// 当前是否显示寻路路径
	m_bShowPathLine = bool( *(int*)(pDataPtr) );
	pDataPtr += sizeof(int);

	// 当前是否显示路径端点
	m_bShowPathEndPoint = bool( *(int*)(pDataPtr) );
	pDataPtr += sizeof(int);

	// 当前是否打开深度测试,以正常的模式显示导航网格
	m_bEnableDepthTest = bool( *(int*)(pDataPtr) );
	pDataPtr += sizeof(int);

	// 是否显示导航网格调试信息]
	m_bIsVisable = bool( *(int*)(pDataPtr) );
	pDataPtr += sizeof(int);

	// 记录当前是否已经创建地图边界
	m_bHasGenMapBorder = bool( *(int*)(pDataPtr) );
	pDataPtr += sizeof(int);

	// 记录当前是否已经结束地图边界多边形的编辑
	m_bEndEditMapBoarder = bool( *(int*)(pDataPtr) );
	pDataPtr += sizeof(int);

	// 地图边界多边形的各个顶点信息
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
	
	// 编辑好的障碍网格顶点信息
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

	// 编辑好的障碍网格多边形信息
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

	// 记录导航网格
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

	// 创建导航网格编辑的显示数据
	PrepareVBEditNavPoints();

	if ( m_vtTriangle.size() )
	{
		// 创建导航网格的显示数据
		PreparemVBNavMesh();

		// 创建导航网格寻路器
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
		// 创建导航网格的显示数据
		PreparemVBNavMesh();

		// 创建导航网格寻路器
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

	// 记录场景大小
	float fSceneSize = TERRAIN->GetCurHeightmap()->GetWidth();
	fout.write( (char*)(&fSceneSize), sizeof(float) );

	// 当前编辑的障碍网格顶点的最大值
	fout.write( (char*)(&m_nEditNavPointsCurMaxVertexCount), sizeof(size_t) );

	// 下一个障碍网格顶点的起始索引
	fout.write( (char*)(&m_nStart), sizeof(size_t) );

	// 当前正在编辑第几个障碍网格多边形
	fout.write( (char*)(&m_nCurPolygon), sizeof(size_t) );

	// 编辑模式
	int nEditMode = (int)m_EditMode;
	fout.write( (char*)(&nEditMode), sizeof(int) );

	// 当前是否显示障碍导航多边形
	int nShowEditNavPolygon = (int)m_bShowEditNavPolygon;
	fout.write( (char*)(&nShowEditNavPolygon), sizeof(int) );

	// 当前是否显示导航网格
	int nShowNavMesh = (int)m_bShowNavMesh;
	fout.write( (char*)(&nShowNavMesh), sizeof(int) );

	// 当前是否显示三角形路径
	int nShowPathTriangle = (int)m_bShowPathTriangle;
	fout.write( (char*)(&nShowPathTriangle), sizeof(int) );

	// 当前是否显示寻路路径
	int nShowPathLine = (int)m_bShowPathLine;
	fout.write( (char*)(&nShowPathLine), sizeof(int) );

	// 当前是否显示路径端点,
	int nShowPathEndPoint = (int)m_bShowPathEndPoint;
	fout.write( (char*)(&nShowPathEndPoint), sizeof(int) );

	// 当前是否打开深度测试,以正常的模式显示导航网格
	int nEnableDepthTest = (int)m_bEnableDepthTest;
	fout.write( (char*)(&nEnableDepthTest), sizeof(int) );

	// 是否显示导航网格调试信息
	int nIsVisable = (int)m_bIsVisable;
	fout.write( (char*)(&nIsVisable), sizeof(int) );

	// 记录当前是否已经创建地图边界
	int nHasGenMapBorder = (int)m_bHasGenMapBorder;
	fout.write( (char*)(&nHasGenMapBorder), sizeof(int) );

	// 记录当前是否已经结束地图边界多边形的编辑
	int nEndEditMapBoarder = (int)m_bEndEditMapBoarder;
	fout.write( (char*)(&nEndEditMapBoarder), sizeof(int) );

	// 保存地图边界多边形的各个顶点信息
	AIRVector< Vector3Df > vtMapBorderVertex = m_WholeMapPolygon.GetPoints();
	u32 nMapBorderVertexCount = vtMapBorderVertex.size();
	fout.write( (char*)(&nMapBorderVertexCount), sizeof(u32) );
	if ( nMapBorderVertexCount > 0 )
	{
		fout.write( (char*)(&vtMapBorderVertex[0]), sizeof(Vector3Df) * nMapBorderVertexCount );
	}

	// 记录编辑好的障碍网格顶点信息
	u32 nNavVertexCount = m_vtNavVertex.size();
	fout.write( (char*)(&nNavVertexCount), sizeof(u32) );
	if ( nNavVertexCount > 0 )
	{
		fout.write( (char*)(&m_vtNavVertex[0]), sizeof(NAV_VERTEX) * nNavVertexCount );
	}

	// 记录编辑好的障碍网格多边形信息
	u32 nNavPolygonInfoCount = m_vtNavPolygonInfo.size();
	fout.write( (char*)(&nNavPolygonInfoCount), sizeof(u32) );
	if ( nNavPolygonInfoCount > 0 )
	{
		fout.write( (char*)(&m_vtNavPolygonInfo[0]), sizeof(PolygonInfo) * nNavPolygonInfoCount );
	}

	// 记录导航网格
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
	// 记录编辑器的信息
	xml.AddElem( "EditInfo" );

	// 记录场景大小
	float fSceneSize = TERRAIN->GetCurHeightmap()->GetWidth();
	xml.AddAttrib( "SceneSize", GetString("%f", fSceneSize) );

	// 当前编辑的障碍网格顶点的最大值
	xml.AddAttrib( "EditNavPointsCurMaxVertexCount", m_nEditNavPointsCurMaxVertexCount );
	// 下一个障碍网格顶点的起始索引
	xml.AddAttrib( "Start", m_nStart );
	// 当前正在编辑第几个障碍网格多边形
	xml.AddAttrib( "CurPolygon", m_nCurPolygon );

	// 当前正在编辑地图边界多边形
	if ( m_EditMode == eNEM_EditMapBorder )
	{
		xml.AddAttrib( "EditMode", "eNEM_EditMapBorder" );
	}
	// 当前正在编辑导航障碍多边形
	else if ( m_EditMode == eNEM_AddNavPolygon )
	{
		xml.AddAttrib( "EditMode", "eNEM_AddNavPolygon" );
	}
	// 当前可以设置路径的端点并寻路
	else if ( m_EditMode == eNEM_EditPathStartEnd )
	{
		xml.AddAttrib( "EditMode", "eNEM_EditPathStartEnd" );
	}
	// 寻路模式
	else if ( m_EditMode == eNEM_FindPath )
	{
		xml.AddAttrib( "EditMode", "eNEM_FindPath" );
	}

	// 当前是否显示障碍导航多边形
	if ( m_bShowEditNavPolygon )
	{
		xml.AddAttrib( "ShowEditNavPolygon", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "ShowEditNavPolygon", "FALSE" );
	}

	// 当前是否显示导航网格
	if ( m_bShowNavMesh )
	{
		xml.AddAttrib( "ShowNavMesh", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "ShowNavMesh", "FALSE" );
	}

	// 当前是否显示三角形路径
	if ( m_bShowPathTriangle )
	{
		xml.AddAttrib( "ShowPathTriangle", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "ShowPathTriangle", "FALSE" );
	}

	// 当前是否显示寻路路径
	if ( m_bShowPathLine )
	{
		xml.AddAttrib( "ShowPathLine", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "ShowPathLine", "FALSE" );
	}

	// 当前是否显示路径端点,
	if ( m_bShowPathEndPoint )
	{
		xml.AddAttrib( "ShowPathEndPoint", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "ShowPathEndPoint", "FALSE" );
	}

	// 当前是否打开深度测试,已正常的模式显示导航网格
	if ( m_bEnableDepthTest )
	{
		xml.AddAttrib( "EnableDepthTest", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "EnableDepthTest", "FALSE" );
	}

	// 是否显示导航网格调试信息
	if ( m_bIsVisable )
	{
		xml.AddAttrib( "IsVisable", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "IsVisable", "FALSE" );
	}
	
	xml.IntoElem();

	// 记录地图边界多边形信息
	{
		xml.AddElem( "MapBorderPolygon" );

		// 记录当前是否已经成成地图边界
		if ( m_bHasGenMapBorder )
		{
			xml.AddAttrib( "HasGenMapBorder", "TRUE" );
		}
		else
		{
			xml.AddAttrib( "HasGenMapBorder", "FALSE" );
		}

		// 记录当前是否已经结束地图边界多边形的编辑
		if ( m_bEndEditMapBoarder )
		{
			xml.AddAttrib( "EndEditMapBoarder", "TRUE" );
		}
		else
		{
			xml.AddAttrib( "EndEditMapBoarder", "FALSE" );
		}

		// 保存地图边界多边形的各个顶点信息
		AIRVector< Vector3Df > vtMapBorderVertex = m_WholeMapPolygon.GetPoints();
		for ( size_t i = 0; i < vtMapBorderVertex.size(); ++i )
		{
			xml.AddChildElem( "PolygonVertex" );
			xml.AddChildAttrib( "Position", GetString("(%f,%f,%f)", vtMapBorderVertex[i].x, vtMapBorderVertex[i].y, vtMapBorderVertex[i].z) );
		}
	}

	// 记录编辑好的障碍网格顶点信息
	{
		xml.AddElem( "EditPolygonVertex" );
		// 遍历所有的障碍多边形顶点
		for ( size_t i = 0; i < m_vtNavVertex.size(); ++i )
		{
			xml.AddChildElem( "PolygonVertex" );
			xml.AddChildAttrib( "Position", GetString("(%f,%f,%f)", m_vtNavVertex[i].m_Pos.x, m_vtNavVertex[i].m_Pos.y, m_vtNavVertex[i].m_Pos.z) );
			SColor col = m_vtNavVertex[i].m_Color.GetAsARGB();
			xml.AddChildAttrib( "Color", GetString("(%d,%d,%d,%d)",col.GetAlpha(), col.GetRed(), col.GetGreen(), col.GetBlue()) );
		}
	}

	// 记录编辑好的障碍网格多边形信息
	{
		xml.AddElem( "EditPolygonInfo" );
		// 遍历所有的障碍多边形
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
	// 记录已经创建出来的导航网格信息
	xml.AddElem( "NavInfo" );

	xml.IntoElem();

	// 记录导航网格
	{
		xml.AddElem( "NavMesh" );
		xml.IntoElem();
		NavTriangle tri;
		// 遍历所有的导航网格三角形
		for ( size_t i = 0; i < m_vtTriangle.size(); ++i )
		{
			tri = m_vtTriangle[i];
			xml.AddElem( "NavTriangle" );
			// 保存三角形的ID
			xml.AddAttrib( "ID", int(tri.m_dwID) );
			// 保存三角形的属性
			xml.AddAttrib( "Attribute", int(tri.m_byAttrib) );
			// 保存三角形的3个顶点
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
	// 查询结果
	bool		bFindIntersetPoint = false;
	Vector3Df	vTemp;
	float		fDistance = float( 0xfffffffe );
	float		fTempDistance = 0.0f;
	NavTriangle nav_tri;
	Vector3Df	vEyePos = ENGINE_INST->GetSceneMgr()->GetCamera()->GetPosition();

	// 遍历所有的导航三角面
	for ( size_t i = 0; i < m_vtTriangle.size(); ++i )
	{
		// 取得当前三角面信息
		nav_tri = m_vtTriangle[i];
		// 与视线做相交测试
		if ( nav_tri.m_Triangle.GetIntersectionWithLimitedLine( eyeRay, vTemp ) )
		{
			// 如果有交点,设置测试结果为true
			bFindIntersetPoint = true;
			// 计算交点到摄像机的距离
			vOut = vTemp - vEyePos;
			fTempDistance = vOut.GetLength();
			// 如果该交点里摄像机更近
			if ( fTempDistance < fDistance )
			{
				// 设置当前的最小距离
				fDistance = fTempDistance;
				// 记录交点
				vOut = vTemp;
				// 记录当前的三角面信息
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



// 删除当前点选到的障碍网格多边形
void NavEditor::DelCurPickEditNavPolygon( int nEditNavPolygon )
{
	if ( nEditNavPolygon == -1 || nEditNavPolygon >= m_vtNavPolygonInfo.size() )
	{
		return;
	}
	// 取出要删除的第nEditNavPolygon个编辑多边形信息
	PolygonInfo pi = m_vtNavPolygonInfo[nEditNavPolygon];
	// 计算要移动的目标起始顶点
	int nDstStart = pi.m_Start + pi.m_Len;
	// 计算要放到的起始顶点
	int nSrcStart = pi.m_Start;
	// 计算要移动的目标结束顶点
	int nDstEnd = m_vtNavVertex.size();
	// 要删除的顶点个数
	u32 nCount = pi.m_Len;

	// 遍历并移动从nDstStart到nDstEnd的所有顶点
	for ( ; nDstStart < nDstEnd; ++nDstStart, ++nSrcStart )
	{
		m_vtNavVertex[nSrcStart] = m_vtNavVertex[nDstStart];
	}
	// 删除后面的nCount个节点
	for ( u32 i = 0; i < nCount; ++i )
	{
		m_vtNavVertex.pop_back();
	}

	// 删除多边形信息
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



// 将生成的导航三角面记录到nGridCountPerSide * nGridCountPerSide个格子区域里面
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
	// 取得世界的大小
	float fTerrainSize = TERRAIN->GetCurHeightmap()->GetWidth();
	// 取得世界的大小的一半
	float fHalfSize = fTerrainSize / 2.0f;
	// 取得每个格子区域的世界大小
	float fGridSize = fTerrainSize / float(nGridCountPerSide);
	m_fGridSize = fGridSize;
	m_nGridCountPerSide = nGridCountPerSide;

	// 设置世界的矩形区域
	m_rcWorld.SetRect( -fHalfSize, -fHalfSize, fHalfSize, fHalfSize );

	// 记录每个格子区域的三角面信息的容器
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
	// 遍历每个导航三角面
	for ( size_t i = 0; i < m_vtTriangle.size(); ++i )
	{
		// 取得该导航面的三角形
		Triangle3Df tri = m_vtTriangle[i].m_Triangle;

		// 导航三角形在水平面上的投影
		Triangle2Df triProj = tri.GetHorzProjTriangle2D(); 

		// 计算该三角形在水平面上的矩形包围盒
		left   = GetMin( tri.pointA.x, tri.pointB.x, tri.pointC.x );
		top    = GetMin( tri.pointA.z, tri.pointB.z, tri.pointC.z );
		right  = GetMax( tri.pointA.x, tri.pointB.x, tri.pointC.x );
		bottom = GetMax( tri.pointA.z, tri.pointB.z, tri.pointC.z );

		// 计算矩形包围盒所跨越的格子阵列
		// 起始格子列
		cs = int( left + fHalfSize )   / int(fGridSize);
		// 结束格子列
		ce = int( right + fHalfSize )  / int(fGridSize);
		// 起始格子行
		rs = int( fHalfSize - bottom ) / int(fGridSize);
		// 结束格子行
		re = int( fHalfSize - top )    / int(fGridSize);

		// 将行和列的数值控制在正常范围内
		cs = (cs >= m_nGridCountPerSide ? m_nGridCountPerSide-1 : cs);
		ce = (ce >= m_nGridCountPerSide ? m_nGridCountPerSide-1 : ce);
		rs = (rs >= m_nGridCountPerSide ? m_nGridCountPerSide-1 : rs);
		re = (re >= m_nGridCountPerSide ? m_nGridCountPerSide-1 : re);

		// 遍历所跨越的格子阵列
		for ( int r = rs; r <= re; ++r )
		{
			for ( int c = cs; c <= ce; ++c )
			{
				//--------------------------------------------------
				//! add by hjj 2010-8-4
				// 计算出格子的区域大小
				// 格子左边界
				rcl = c * m_fGridSize - fHalfSize;
				// 格子上边界
				rct = fHalfSize - (r+1) * m_fGridSize;
				// 格子右边界
				rcr = rcl + m_fGridSize;
				// 格子下边界
				rcb = rct + m_fGridSize;

				Rectf rcGrid( rcl, rct, rcr, rcb );
				rcGrid.Normalize();

				// 判断格子矩形与导航三角形是否相交
				bool bIntersect = IsRectIntersectTriangle( rcGrid, triProj );
				//--------------------------------------------------

				if ( bIntersect )
				{
					// 将该三角形的ID记录下来
					int id = r * nGridCountPerSide + c;
					m_vtOptimistNavMeshInfo[ id ].push_back( i );
				}
			}
		}

	}

	return true;
}



// 根据坐标(fWorldX,fWorldZ)判断是否有击中某个导航三角面
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

	// 首先进行精确地测试
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

	// 精确测试找不到,就进行模糊测试,只要点到三角形边界的距离在5个单位之内,都把该点当做在三角形之内
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

	// 模糊测试也找不到,就返回false,表示点不在导航网格之内
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
		// 获得角色所在的导航三角面
		if ( OptimiseFindIntersectNavMesh( fWorldX, fWorldZ, nav_tri ) )
		{
			// 如果导航三角面的属性为0，表示角色是站在地形面上
			if ( nav_tri.m_byAttrib == 0 )
			{
				// 获得角色所在地面的高度
				fHeight = pSceneMgr->GetHeightMap()->GetHeight( fWorldX, fWorldZ );
			}
			// 否则,表示角色站在导航三角面上
			else
			{
				// 计算角色所在点的垂直线与导航网格的交点
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
		// 获得角色所在地面的高度
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
