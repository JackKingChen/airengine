#pragma once

#ifndef __NavEditor_H__
#define __NavEditor_H__


#include "Navigater.h"
#include "NavPathFinder.h"
#include "Color.h"
#include "VertexDeclaration.h"
#include "IVertexBuffer.h"
#include "IRenderer.h"
#include "IMaterial.h"
#include "Renderable.h"


const size_t C_PAGE_VERTEX_COUNT = 4096;


// 判断2条线段是否相交
bool EngineExport IsLineSegIntersect
( 
float line1pStartX, float line1pStartY, float line1pEndX, float line1pEndY, 
float line2pStartX, float line2pStartY, float line2pEndX, float line2pEndY 
);

// 判断矩形与三角形是否相交
bool EngineExport IsRectIntersectTriangle
( 
Rectf&			rc, 
Triangle2Df&	tri 
);


// 编辑模式
enum NavEditorMode
{
	eNEM_EditNavPolygon,	// 编辑已经添加的障碍导航网格
	eNEM_AddNavPolygon,		// 添加障碍导航网格
	eNEM_EditMapBorder,		// 编辑地图边界网格
	eNEM_EditPathStartEnd,	// 编辑寻路起始结束点
	eNEM_FindPath,			// 寻路
};

// 推平控制点的模式
enum FlattenMode
{
	eFM_MIN,				// 将所有控制点的高度设置为所有控制点里的最小高度
	eFM_MAX,				// 将所有控制点的高度设置为所有控制点里的最大高度
	eFM_DEF,				// 将所有控制点的高度设置为自定义高度
};


// 导航网格顶点信息
class NAV_VERTEX
{
public:
	NAV_VERTEX( void ) : m_Pos(0,0,0), m_Color(0xffffffff) {}

	NAV_VERTEX( const NAV_VERTEX& other ) : m_Pos(other.m_Pos), m_Color(other.m_Color) {}

	void operator = ( const NAV_VERTEX& other )
	{
		m_Pos = other.m_Pos;
		m_Color = other.m_Color;
	}

public:
	Vector3Df	m_Pos;		// 位置
	CColorValue		m_Color;	// 颜色
};



// 障碍网格多边形信息
class PolygonInfo
{
public:
	PolygonInfo( void ) : m_Start(0), m_Len(0) {}

public:
	u32 m_Start;			// 起始顶点索引
	u32 m_Len;			// 多边形的顶点个数
};



// 导航网格操作点信息
class CheckPoint
{
public:
	CheckPoint( void ) : m_nTriangleID(-1), m_nVertexID(0) {}

public:
	u32	m_nTriangleID;	// 三角形索引
	int		m_nVertexID;	// 三角形中的顶点ID
};





// 导航网格编辑器
class EngineExport NavEditor : public Resource, public Renderable//, public EngineAllocator(NavEditor)
{
	EngineAllocator(NavEditor)
	NavEditor( void );

public:
	static NavEditor* GetSingleton( void )
	{
		static NavEditor inst;
		return &inst;
	}


	~NavEditor( void );


	// 设置导航网格编辑器显示的材质
	void SetMaterial( const AIRString& szMatName );

	// 重置导航网格编辑器的信息
	void ResetNavEditor( void );

	// 释放资源
	void Clear( void );

	// 准备下一个障碍网格多边形
	void PrepareNextPolygon( void );

	// 添加障碍网格多边形顶点
	void AddNewNavPolygonPoint( float x, float y, float z );

	// 添加地图边界多边形顶点
	void AddMapBorderPoint( float x, float y, float z );

	// 取消添加障碍网格多边形顶点或地图边界多边形顶点
	void CancelAddNavPolygonPointOrMapBorderPoint( void );

	// 结束编辑当前障碍网格多边形
	void EndCurPolygon( void );

	// 结束编辑地图边界
	void EndMapBorder( void );

	// 设置地图边界多边形
	void SetWholeMapPolygon( const Polygon3Df& wholeMapPolygon );

	// 设置当前鼠标所在的世界位置(编辑障碍网格时用到)
	void SetCandidateEditPoint( const Vector3Df& vEditPoint );

	// 创建导航网格
	bool GenNavMesh( void );

	// 寻找路径三角面
	bool FindPathOfTriangle( Vector3Df start, Vector3Df end );

	// 寻找路径
	bool FindPathOfLine( Vector3Df start, Vector3Df end );

	// 寻找路径
	bool FindPath( float fStartX, float fStartY, float fStartZ, float fEndX, float fEndY, float fEndZ )
	{
		return FindPathOfLine( Vector3Df( fStartX, fStartY, fStartZ ), Vector3Df( fEndX, fEndY, fEndZ ) );
	}

	// 渲染
	void Render( void );

	// 设置是否显示编辑障碍网格
	inline void SetShowEditNavPoints( bool b )
	{
		m_bShowEditNavPolygon = b;
	}

	// 获得是否显示编辑障碍网格
	inline bool GetShowEditNavPoints( void )
	{
		return m_bShowEditNavPolygon;
	}

	// 设置是否显示导航网格
	inline void SetShowNavMesh( bool b )
	{
		m_bShowNavMesh = b;
	}

	// 获得是否显示导航网格
	inline bool GetShowNavMesh( void )
	{
		return m_bShowNavMesh;
	}

	// 设置是否显示路径三角面
	inline void SetShowPathTriangle( bool b )
	{
		m_bShowPathTriangle = b;
	}

	// 获得是否显示路径三角面
	inline bool GetShowPathTriangle( void )
	{
		return m_bShowPathTriangle;
	}

	// 设置是否显示路径线
	inline void SetShowPathLine( bool b )
	{
		m_bShowPathLine = b;
	}

	// 获得是否显示路径线
	inline bool GetShowPathLine( void )
	{
		return m_bShowPathLine;
	}

	// 设置寻路起始点
	inline void SetEditPathStart( const Vector3Df& vStart )
	{
		m_vEditPathStart = vStart;
	}

	// 设置寻路目标点
	inline void SetEditPathEnd( const Vector3Df& vEnd )
	{
		m_vEditPathEnd = vEnd;
	}

	// 设置是否显示寻路路径端点
	inline void SetShowPathEndPoint( bool b )
	{
		m_bShowPathEndPoint = b;
	}

	// 获得是否显示寻路路径端点
	inline bool GetShowPathEndPoint( void )
	{
		return m_bShowPathEndPoint;
	}

	// 设置当前NAV_EDITOR的操作模式
	inline void SetNavEditMode( NavEditorMode mode )
	{
		m_EditMode = mode;
	}

	// 获得当前NAV_EDITOR的操作模式
	inline NavEditorMode GetNavEditMode( void )
	{
		return m_EditMode; 
	}

	// 检查是否已经创建了地图边界网格
	inline bool GetHasGenMapBorder( void )
	{
		return m_bHasGenMapBorder;
	}

	// 设置在画导航网格的时候是否打开深度测试
	inline void SetEnableDepthTest( bool b )
	{
		m_bEnableDepthTest = b;
	}

	// 获得在画导航网格的时候是否打开深度测试
	inline bool GetEnableDepthTest( void )
	{
		return m_bEnableDepthTest;
	}

	// 重置地图边界多变形信息
	// 如果当前已经创建了导航网格,调用此函数将删除已创建的导航网格
	void ResetMapBorder( void );

	// 重置障碍网格多边形信息
	// 如果当前已经创建了导航网格,调用此函数将删除已创建的导航网格
	void ResetEditNavPolygon( void );

	// 重置导航网格信息
	void ResetNavMesh( void );

	// 设置是否显示寻路调试信息
	void DebugFindPath( void );

	// 设置是否显示NAV_EDITOR
	inline void SetVisable( bool b )
	{
		m_bIsVisable = b;
	}

	// 获得是否显示NAV_EDITOR
	inline bool GetVisable( void )
	{
		return m_bIsVisable;
	}

	// 在正交投影模式下,根据某点的世界水平面坐标检测是否点中某个导航三角面上的顶点
	bool CalCheckPoints( float fRX, float fRZ, Vector3Df& vOut, float fErrTor = 1.0f );

	// 通过射线检测是否点中某个导航三角面上的顶点
	bool CalCheckPoints( Line3Df& line, Vector3Df& vOut, float fErrTor = 1.0f );

	// 设置某个被点中的导航面的顶点的坐标
	void MoveCheckPoints( const Vector3Df& vNewPos );

	// 在正交投影模式下,根据某点的世界水平面坐标检测是否在某个导航三角面里面
	bool CalPickTriangle( float fRX, float fRZ, NavTriangle& nav_tri );

	// 通过射线检测是否点中某个导航三角面
	bool CalPickTriangle( const Line3Df& line, NavTriangle& nav_tri );

	// 设置点中的导航三角面的属性
	void SetPickNavTriangleAttrib( u8 attrib );

	// 清除点选的导航网格三角面
	void ClearPickNavTriangle( void );

	// 清除点选的导航网格顶点
	void ClearCurCheckPoint( void );

	// 获得当前是否已经创建导航网格
	inline bool GetHasGenNavMesh( void )
	{
		return ( m_vtTriangle.size() > 0 );
	}

	// 获得寻找到的路径
	inline const AIRVector< Vector3Df >& GetPathLine( void ) const 
	{
		return m_vtPathLine;
	}

	// 添加导航网格的编辑控制点
	void AddOperatPoint( Vector3Df& vPos, const AIRVector< CheckPoint >& vtOperatPointSet );

	// 清空所有的导航网格编辑控制点
	void ClearOperatPointsAndInfo( void );

	// 移动所有的导航网格编辑控制点
	void MoveOperatPoints( float fOffset );

	// 推平所有编辑控制点的高度
	void FlattenOperatPoints( FlattenMode fm, float height = 100.0f );

	// 获得当前所有的导航网格编辑控制点
	const AIRVector< CheckPoint >& GetCurCheckPoint( void )
	{
		return m_vtCurCheckPoints;
	}

	// 查询当前是否有导航网格的编辑控制点
	bool GetHasOperatPoints( void )
	{
		return (m_lstOperatPoints.size() > 0);
	}

	// 获得创建出来的导航网格
	AIRVector< NavTriangle >& GetNavMesh( void )
	{
		return m_vtTriangle;
	}

	// 获得射线击中的最近的三角面以及击中点
	bool GetNearestEyeRayIntersetPointOfNavMesh( Line3Df& eyeRay, Vector3Df& vOut, NavTriangle& NavTri );

	// 点选编辑障碍网格多边形顶点
	bool PickEditNavPolygonAndPoint( float fRX, float fRZ, int& nPolygonID, float fErrTor = 10.0f );

	// 点选编辑障碍网格多边形顶点
	bool PickEditNavPolygonAndPoint( Line3Df& line, int& nPolygonID, float fErrTor = 10.0f );

	// 删除当前点选到的障碍网格多边形
	void DelCurPickEditNavPolygon( int nEditNavPolygon );

	// 清除点选的编辑障碍导航多边形
	void ClearPickEditNavPolygon( void );

	bool OptimiseNavMesh( int nGridCountPerSide );

	bool OptimiseFindIntersectNavMesh( float fWorldX, float fWorldZ, NavTriangle& NavTri );

	bool IsPointValid( float fWorldX, float fWorldZ );

	void SetNeedLoadEditData( bool b )
	{
		m_bNeedLoadEditData = b;
	}

	float GetWorldPosHeight( float fWorldX, float fWorldZ );

    virtual s32 Release()
    {
        return 0;
    }
public:
	// 读取导航网格xml文件
	bool LoadFromXML( const char* szNavFile );

	bool LoadFromBinFile( const char* szNavFile );

	// 从xml文件创建
	bool CreateFromXML( CMarkupSTL& xml );

	// 创建编辑器信息
	bool CreateEditInfo( CMarkupSTL& xml );

	// 创建导航网格信息
	bool CreateNavInfo( CMarkupSTL& xml );

	// 保存到xml文件
	bool SaveToXML( const char* szNavFile );

	bool SaveToBinFile( const char* szNavFile );

	// 保存编辑器信息
	bool SaveEditInfo( CMarkupSTL& xml );

	// 保存导航网格信息
	bool SaveNavInfo( CMarkupSTL& xml );

	// 设置保存的文件名
	void	SetNavFile( const AIRString& szFile )
	{
		m_szNavFile = szFile;
	}

	// 获得保存的文件名
	const AIRString& GetNavFile( void )
	{
		return m_szNavFile;
	}

    // add by liangairan  at 20101031
    size_t GetNavMeshsNum() const
    {
        return m_vtOptimistNavMeshInfo.size();
    }

    virtual bool Cache();
protected:
	// 创建编辑的障碍网格渲染数据
	void PrepareVBEditNavPoints( void );

	// 创建导航网格的渲染数据
	void PreparemVBNavMesh( void );

	// 创建寻路三角形路径的渲染数据
	void PrepareVBPathTriangle( void );

	// 创建寻路路径的渲染数据
	void PrepareVBPathLine( void );


	// 渲染编辑的障碍网格
	void RenderEditNavPoints( void );

	// 渲染导航网格
	void RenderNavMesh( void );

	// 渲染寻路三角形路径
	void RenderPathTriangle( void );

	// 渲染寻路路径
	void RenderPathLine( void );

	// 渲染寻路的端点
	void RenderPathEndPoint( void );

	// 渲染地图边界
	void RenderMapBorder( void );

	// 渲染选中的编辑障碍网格
	void RenderPickEditNavPolygon( void );


	// 检查并调整地图边界多边形的顶点顺序
	void CheckMapBorderValid( void );

	// 刷新导航网格信息
	void RefreshNavInfo( void );

    void _GetPrimitive();

private:
	// 障碍导航网格线的颜色
	u32						m_colEditPolygon;
	// 导航网格的颜色(以地形高度为准)
	u32						m_colNavMeshTerrain;
	// 导航网格的颜色(以网格高度为准)
	u32						m_colNavMeshSelf;
	// 导航网格的背景色
	u32						m_colNavMeshBack;
	// 地图边界的颜色
	u32						m_colMapBorder;
	// 三角形路径的颜色
	u32						m_colPathTriangle;
	// 顶点路径的颜色
	u32						m_colPathLine;
	// 路径起始点的颜色
	u32						m_colPathStart;
	// 路径结束点的颜色
	u32						m_colPathEnd;
	// 正在编辑的导航网格顶点颜色
	u32						m_colCheckPoint;
	// 正在编辑的导航网格三角面的颜色
	u32						m_colEditNavTriangle;

	// 整个地图的多边形包围框
	Polygon3Df					m_WholeMapPolygon;

	// 当前候选导航点
	Vector3Df					m_vCandidateEditPoint;

	// 导航网格的顶点声明
	//CVertexDeclaration*			m_pVertexDecl;

	// 当前编辑导航点的最大数量
	size_t						m_nEditNavPointsCurMaxVertexCount;
	// 编辑导航点的顶点缓冲
	IVertexBuffer*				m_pVBEditNavPoints;
	// 编辑导航点的顶点信息
	AIRVector< NAV_VERTEX >	m_vtEditingPolygonVertex;

	// 当前的起始顶点ID
	int							m_nStart;
	// 当前正在编辑的多边形
	int							m_nCurPolygon;

	
	// 导航网格顶点
	AIRVector< NAV_VERTEX >	m_vtNavVertex;
	// 导航网格多边形信息
	AIRVector< PolygonInfo >	m_vtNavPolygonInfo;


	// 创建出来的导航网格三角形列表
	AIRVector< NavTriangle >	m_vtTriangle;

	// 导航网格顶点数据
	AIRVector< NAV_VERTEX >	m_vtTVertex;
	// 导航网格顶点缓冲
	IVertexBuffer*				m_pVBNavMesh;

	// 导航网格边缘顶点数据
	AIRVector< NAV_VERTEX >	m_vtTVertexBackLine;
	// 导航网格边缘顶点缓冲
	IVertexBuffer*				m_pVBNavMeshBackLine;
	

	// 寻路路径顶点数据
	AIRVector< NAV_VERTEX >	m_vtPathLineVertex;
	// 寻路路径顶点缓冲
	IVertexBuffer*				m_pVBPathLine;
	// 已经找到的路径过程
	AIRVector <Vector3Df>		m_vtPathLine;

	// 三角形路径信息
	AIRVector <CTrianglePath> m_vtTPath;
	// 三角形路径顶点信息
	AIRVector <NAV_VERTEX>	m_vtTPVertex;
	// 三角形路径顶点缓冲
	IVertexBuffer*				m_pVBPathTriangle;

	// 导航网格寻路器
	NavPathFinder*				m_pNavPathFinder; 

	// 当前是否显示编辑的导航多边形
	bool						m_bShowEditNavPolygon;
	// 当前是否显示导航网格
	bool						m_bShowNavMesh;
	// 当前是否显示寻路三角网格
	bool						m_bShowPathTriangle;
	// 当前是否显示寻路路线
	bool						m_bShowPathLine;

	// 导航编辑器的显示材质
	IMaterial*					m_pMaterial;

	// 寻路起始点
	Vector3Df					m_vEditPathStart;
	// 寻路结束点
	Vector3Df					m_vEditPathEnd;
	// 是否显示路径的端点
	bool						m_bShowPathEndPoint;

	// 当前的编辑模式
	NavEditorMode				m_EditMode;

	// 保存手动编辑的地图边界多边形的点
	AIRVector< NAV_VERTEX >	m_vtMapBoarderPoints;
	// 当前是否结束了地图边界的编辑
	bool						m_bEndEditMapBoarder;
	// 当前是否已经生成了地图边界
	bool						m_bHasGenMapBorder;

	// 当前的视图模式
	bool						m_bEnableDepthTest;

	// 是否显示NAV调试网格
	bool						m_bIsVisable;

	// 当前选中操作的三角形顶点
	AIRVector<CheckPoint>		m_vtCurCheckPoints;
	// 被操作的三角形顶点
	Vector3Df					m_vCheckPointPos;
	bool						m_bPickCheckPoint;

	// 当前点中的导航网格三角形
	NavTriangle					m_CurPickTriangle;
	// 当前是否有点中的导航网格三角形
	bool						m_bHasPickTriangle;

	AIRString								m_szNavFile;

	AIRList< Vector3Df >					m_lstOperatPoints;
	AIRList< AIRVector<CheckPoint> >	m_lstOperatPointsInfo;

	int										m_nCurPickPolygon;
	bool									m_bPickPolygon;

	AIRVector< AIRVector<int> >			m_vtOptimistNavMeshInfo;
	Rectf									m_rcWorld;
	float									m_fGridSize;
	int										m_nGridCountPerSide;

	AIRList< u32 >						m_lstEdittingNavPolygonVertex;

	AIRList< u32 >						m_lstEdittingMapBorderVertex;

	bool									m_bNeedLoadEditData;

    Primitive*  m_pTrianglePrimitive;
    Primitive*  m_pLinePrimitive;
    Primitive*  m_pPointPrimitive;

};//! end class NavEditor


#define NAV_EDITOR (NavEditor::GetSingleton())


#endif //! __NavEditor_H__