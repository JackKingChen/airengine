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


// �ж�2���߶��Ƿ��ཻ
bool EngineExport IsLineSegIntersect
( 
float line1pStartX, float line1pStartY, float line1pEndX, float line1pEndY, 
float line2pStartX, float line2pStartY, float line2pEndX, float line2pEndY 
);

// �жϾ������������Ƿ��ཻ
bool EngineExport IsRectIntersectTriangle
( 
Rectf&			rc, 
Triangle2Df&	tri 
);


// �༭ģʽ
enum NavEditorMode
{
	eNEM_EditNavPolygon,	// �༭�Ѿ���ӵ��ϰ���������
	eNEM_AddNavPolygon,		// ����ϰ���������
	eNEM_EditMapBorder,		// �༭��ͼ�߽�����
	eNEM_EditPathStartEnd,	// �༭Ѱ·��ʼ������
	eNEM_FindPath,			// Ѱ·
};

// ��ƽ���Ƶ��ģʽ
enum FlattenMode
{
	eFM_MIN,				// �����п��Ƶ�ĸ߶�����Ϊ���п��Ƶ������С�߶�
	eFM_MAX,				// �����п��Ƶ�ĸ߶�����Ϊ���п��Ƶ�������߶�
	eFM_DEF,				// �����п��Ƶ�ĸ߶�����Ϊ�Զ���߶�
};


// �������񶥵���Ϣ
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
	Vector3Df	m_Pos;		// λ��
	CColorValue		m_Color;	// ��ɫ
};



// �ϰ�����������Ϣ
class PolygonInfo
{
public:
	PolygonInfo( void ) : m_Start(0), m_Len(0) {}

public:
	u32 m_Start;			// ��ʼ��������
	u32 m_Len;			// ����εĶ������
};



// ���������������Ϣ
class CheckPoint
{
public:
	CheckPoint( void ) : m_nTriangleID(-1), m_nVertexID(0) {}

public:
	u32	m_nTriangleID;	// ����������
	int		m_nVertexID;	// �������еĶ���ID
};





// ��������༭��
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


	// ���õ�������༭����ʾ�Ĳ���
	void SetMaterial( const AIRString& szMatName );

	// ���õ�������༭������Ϣ
	void ResetNavEditor( void );

	// �ͷ���Դ
	void Clear( void );

	// ׼����һ���ϰ���������
	void PrepareNextPolygon( void );

	// ����ϰ��������ζ���
	void AddNewNavPolygonPoint( float x, float y, float z );

	// ��ӵ�ͼ�߽����ζ���
	void AddMapBorderPoint( float x, float y, float z );

	// ȡ������ϰ��������ζ�����ͼ�߽����ζ���
	void CancelAddNavPolygonPointOrMapBorderPoint( void );

	// �����༭��ǰ�ϰ���������
	void EndCurPolygon( void );

	// �����༭��ͼ�߽�
	void EndMapBorder( void );

	// ���õ�ͼ�߽�����
	void SetWholeMapPolygon( const Polygon3Df& wholeMapPolygon );

	// ���õ�ǰ������ڵ�����λ��(�༭�ϰ�����ʱ�õ�)
	void SetCandidateEditPoint( const Vector3Df& vEditPoint );

	// ������������
	bool GenNavMesh( void );

	// Ѱ��·��������
	bool FindPathOfTriangle( Vector3Df start, Vector3Df end );

	// Ѱ��·��
	bool FindPathOfLine( Vector3Df start, Vector3Df end );

	// Ѱ��·��
	bool FindPath( float fStartX, float fStartY, float fStartZ, float fEndX, float fEndY, float fEndZ )
	{
		return FindPathOfLine( Vector3Df( fStartX, fStartY, fStartZ ), Vector3Df( fEndX, fEndY, fEndZ ) );
	}

	// ��Ⱦ
	void Render( void );

	// �����Ƿ���ʾ�༭�ϰ�����
	inline void SetShowEditNavPoints( bool b )
	{
		m_bShowEditNavPolygon = b;
	}

	// ����Ƿ���ʾ�༭�ϰ�����
	inline bool GetShowEditNavPoints( void )
	{
		return m_bShowEditNavPolygon;
	}

	// �����Ƿ���ʾ��������
	inline void SetShowNavMesh( bool b )
	{
		m_bShowNavMesh = b;
	}

	// ����Ƿ���ʾ��������
	inline bool GetShowNavMesh( void )
	{
		return m_bShowNavMesh;
	}

	// �����Ƿ���ʾ·��������
	inline void SetShowPathTriangle( bool b )
	{
		m_bShowPathTriangle = b;
	}

	// ����Ƿ���ʾ·��������
	inline bool GetShowPathTriangle( void )
	{
		return m_bShowPathTriangle;
	}

	// �����Ƿ���ʾ·����
	inline void SetShowPathLine( bool b )
	{
		m_bShowPathLine = b;
	}

	// ����Ƿ���ʾ·����
	inline bool GetShowPathLine( void )
	{
		return m_bShowPathLine;
	}

	// ����Ѱ·��ʼ��
	inline void SetEditPathStart( const Vector3Df& vStart )
	{
		m_vEditPathStart = vStart;
	}

	// ����Ѱ·Ŀ���
	inline void SetEditPathEnd( const Vector3Df& vEnd )
	{
		m_vEditPathEnd = vEnd;
	}

	// �����Ƿ���ʾѰ··���˵�
	inline void SetShowPathEndPoint( bool b )
	{
		m_bShowPathEndPoint = b;
	}

	// ����Ƿ���ʾѰ··���˵�
	inline bool GetShowPathEndPoint( void )
	{
		return m_bShowPathEndPoint;
	}

	// ���õ�ǰNAV_EDITOR�Ĳ���ģʽ
	inline void SetNavEditMode( NavEditorMode mode )
	{
		m_EditMode = mode;
	}

	// ��õ�ǰNAV_EDITOR�Ĳ���ģʽ
	inline NavEditorMode GetNavEditMode( void )
	{
		return m_EditMode; 
	}

	// ����Ƿ��Ѿ������˵�ͼ�߽�����
	inline bool GetHasGenMapBorder( void )
	{
		return m_bHasGenMapBorder;
	}

	// �����ڻ����������ʱ���Ƿ����Ȳ���
	inline void SetEnableDepthTest( bool b )
	{
		m_bEnableDepthTest = b;
	}

	// ����ڻ����������ʱ���Ƿ����Ȳ���
	inline bool GetEnableDepthTest( void )
	{
		return m_bEnableDepthTest;
	}

	// ���õ�ͼ�߽�������Ϣ
	// �����ǰ�Ѿ������˵�������,���ô˺�����ɾ���Ѵ����ĵ�������
	void ResetMapBorder( void );

	// �����ϰ�����������Ϣ
	// �����ǰ�Ѿ������˵�������,���ô˺�����ɾ���Ѵ����ĵ�������
	void ResetEditNavPolygon( void );

	// ���õ���������Ϣ
	void ResetNavMesh( void );

	// �����Ƿ���ʾѰ·������Ϣ
	void DebugFindPath( void );

	// �����Ƿ���ʾNAV_EDITOR
	inline void SetVisable( bool b )
	{
		m_bIsVisable = b;
	}

	// ����Ƿ���ʾNAV_EDITOR
	inline bool GetVisable( void )
	{
		return m_bIsVisable;
	}

	// ������ͶӰģʽ��,����ĳ�������ˮƽ���������Ƿ����ĳ�������������ϵĶ���
	bool CalCheckPoints( float fRX, float fRZ, Vector3Df& vOut, float fErrTor = 1.0f );

	// ͨ�����߼���Ƿ����ĳ�������������ϵĶ���
	bool CalCheckPoints( Line3Df& line, Vector3Df& vOut, float fErrTor = 1.0f );

	// ����ĳ�������еĵ�����Ķ��������
	void MoveCheckPoints( const Vector3Df& vNewPos );

	// ������ͶӰģʽ��,����ĳ�������ˮƽ���������Ƿ���ĳ����������������
	bool CalPickTriangle( float fRX, float fRZ, NavTriangle& nav_tri );

	// ͨ�����߼���Ƿ����ĳ������������
	bool CalPickTriangle( const Line3Df& line, NavTriangle& nav_tri );

	// ���õ��еĵ��������������
	void SetPickNavTriangleAttrib( u8 attrib );

	// �����ѡ�ĵ�������������
	void ClearPickNavTriangle( void );

	// �����ѡ�ĵ������񶥵�
	void ClearCurCheckPoint( void );

	// ��õ�ǰ�Ƿ��Ѿ�������������
	inline bool GetHasGenNavMesh( void )
	{
		return ( m_vtTriangle.size() > 0 );
	}

	// ���Ѱ�ҵ���·��
	inline const AIRVector< Vector3Df >& GetPathLine( void ) const 
	{
		return m_vtPathLine;
	}

	// ��ӵ�������ı༭���Ƶ�
	void AddOperatPoint( Vector3Df& vPos, const AIRVector< CheckPoint >& vtOperatPointSet );

	// ������еĵ�������༭���Ƶ�
	void ClearOperatPointsAndInfo( void );

	// �ƶ����еĵ�������༭���Ƶ�
	void MoveOperatPoints( float fOffset );

	// ��ƽ���б༭���Ƶ�ĸ߶�
	void FlattenOperatPoints( FlattenMode fm, float height = 100.0f );

	// ��õ�ǰ���еĵ�������༭���Ƶ�
	const AIRVector< CheckPoint >& GetCurCheckPoint( void )
	{
		return m_vtCurCheckPoints;
	}

	// ��ѯ��ǰ�Ƿ��е�������ı༭���Ƶ�
	bool GetHasOperatPoints( void )
	{
		return (m_lstOperatPoints.size() > 0);
	}

	// ��ô��������ĵ�������
	AIRVector< NavTriangle >& GetNavMesh( void )
	{
		return m_vtTriangle;
	}

	// ������߻��е�������������Լ����е�
	bool GetNearestEyeRayIntersetPointOfNavMesh( Line3Df& eyeRay, Vector3Df& vOut, NavTriangle& NavTri );

	// ��ѡ�༭�ϰ��������ζ���
	bool PickEditNavPolygonAndPoint( float fRX, float fRZ, int& nPolygonID, float fErrTor = 10.0f );

	// ��ѡ�༭�ϰ��������ζ���
	bool PickEditNavPolygonAndPoint( Line3Df& line, int& nPolygonID, float fErrTor = 10.0f );

	// ɾ����ǰ��ѡ�����ϰ���������
	void DelCurPickEditNavPolygon( int nEditNavPolygon );

	// �����ѡ�ı༭�ϰ����������
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
	// ��ȡ��������xml�ļ�
	bool LoadFromXML( const char* szNavFile );

	bool LoadFromBinFile( const char* szNavFile );

	// ��xml�ļ�����
	bool CreateFromXML( CMarkupSTL& xml );

	// �����༭����Ϣ
	bool CreateEditInfo( CMarkupSTL& xml );

	// ��������������Ϣ
	bool CreateNavInfo( CMarkupSTL& xml );

	// ���浽xml�ļ�
	bool SaveToXML( const char* szNavFile );

	bool SaveToBinFile( const char* szNavFile );

	// ����༭����Ϣ
	bool SaveEditInfo( CMarkupSTL& xml );

	// ���浼��������Ϣ
	bool SaveNavInfo( CMarkupSTL& xml );

	// ���ñ�����ļ���
	void	SetNavFile( const AIRString& szFile )
	{
		m_szNavFile = szFile;
	}

	// ��ñ�����ļ���
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
	// �����༭���ϰ�������Ⱦ����
	void PrepareVBEditNavPoints( void );

	// ���������������Ⱦ����
	void PreparemVBNavMesh( void );

	// ����Ѱ·������·������Ⱦ����
	void PrepareVBPathTriangle( void );

	// ����Ѱ··������Ⱦ����
	void PrepareVBPathLine( void );


	// ��Ⱦ�༭���ϰ�����
	void RenderEditNavPoints( void );

	// ��Ⱦ��������
	void RenderNavMesh( void );

	// ��ȾѰ·������·��
	void RenderPathTriangle( void );

	// ��ȾѰ··��
	void RenderPathLine( void );

	// ��ȾѰ·�Ķ˵�
	void RenderPathEndPoint( void );

	// ��Ⱦ��ͼ�߽�
	void RenderMapBorder( void );

	// ��Ⱦѡ�еı༭�ϰ�����
	void RenderPickEditNavPolygon( void );


	// ��鲢������ͼ�߽����εĶ���˳��
	void CheckMapBorderValid( void );

	// ˢ�µ���������Ϣ
	void RefreshNavInfo( void );

    void _GetPrimitive();

private:
	// �ϰ����������ߵ���ɫ
	u32						m_colEditPolygon;
	// �����������ɫ(�Ե��θ߶�Ϊ׼)
	u32						m_colNavMeshTerrain;
	// �����������ɫ(������߶�Ϊ׼)
	u32						m_colNavMeshSelf;
	// ��������ı���ɫ
	u32						m_colNavMeshBack;
	// ��ͼ�߽����ɫ
	u32						m_colMapBorder;
	// ������·������ɫ
	u32						m_colPathTriangle;
	// ����·������ɫ
	u32						m_colPathLine;
	// ·����ʼ�����ɫ
	u32						m_colPathStart;
	// ·�����������ɫ
	u32						m_colPathEnd;
	// ���ڱ༭�ĵ������񶥵���ɫ
	u32						m_colCheckPoint;
	// ���ڱ༭�ĵ����������������ɫ
	u32						m_colEditNavTriangle;

	// ������ͼ�Ķ���ΰ�Χ��
	Polygon3Df					m_WholeMapPolygon;

	// ��ǰ��ѡ������
	Vector3Df					m_vCandidateEditPoint;

	// ��������Ķ�������
	//CVertexDeclaration*			m_pVertexDecl;

	// ��ǰ�༭��������������
	size_t						m_nEditNavPointsCurMaxVertexCount;
	// �༭������Ķ��㻺��
	IVertexBuffer*				m_pVBEditNavPoints;
	// �༭������Ķ�����Ϣ
	AIRVector< NAV_VERTEX >	m_vtEditingPolygonVertex;

	// ��ǰ����ʼ����ID
	int							m_nStart;
	// ��ǰ���ڱ༭�Ķ����
	int							m_nCurPolygon;

	
	// �������񶥵�
	AIRVector< NAV_VERTEX >	m_vtNavVertex;
	// ��������������Ϣ
	AIRVector< PolygonInfo >	m_vtNavPolygonInfo;


	// ���������ĵ��������������б�
	AIRVector< NavTriangle >	m_vtTriangle;

	// �������񶥵�����
	AIRVector< NAV_VERTEX >	m_vtTVertex;
	// �������񶥵㻺��
	IVertexBuffer*				m_pVBNavMesh;

	// ���������Ե��������
	AIRVector< NAV_VERTEX >	m_vtTVertexBackLine;
	// ���������Ե���㻺��
	IVertexBuffer*				m_pVBNavMeshBackLine;
	

	// Ѱ··����������
	AIRVector< NAV_VERTEX >	m_vtPathLineVertex;
	// Ѱ··�����㻺��
	IVertexBuffer*				m_pVBPathLine;
	// �Ѿ��ҵ���·������
	AIRVector <Vector3Df>		m_vtPathLine;

	// ������·����Ϣ
	AIRVector <CTrianglePath> m_vtTPath;
	// ������·��������Ϣ
	AIRVector <NAV_VERTEX>	m_vtTPVertex;
	// ������·�����㻺��
	IVertexBuffer*				m_pVBPathTriangle;

	// ��������Ѱ·��
	NavPathFinder*				m_pNavPathFinder; 

	// ��ǰ�Ƿ���ʾ�༭�ĵ��������
	bool						m_bShowEditNavPolygon;
	// ��ǰ�Ƿ���ʾ��������
	bool						m_bShowNavMesh;
	// ��ǰ�Ƿ���ʾѰ·��������
	bool						m_bShowPathTriangle;
	// ��ǰ�Ƿ���ʾѰ··��
	bool						m_bShowPathLine;

	// �����༭������ʾ����
	IMaterial*					m_pMaterial;

	// Ѱ·��ʼ��
	Vector3Df					m_vEditPathStart;
	// Ѱ·������
	Vector3Df					m_vEditPathEnd;
	// �Ƿ���ʾ·���Ķ˵�
	bool						m_bShowPathEndPoint;

	// ��ǰ�ı༭ģʽ
	NavEditorMode				m_EditMode;

	// �����ֶ��༭�ĵ�ͼ�߽����εĵ�
	AIRVector< NAV_VERTEX >	m_vtMapBoarderPoints;
	// ��ǰ�Ƿ�����˵�ͼ�߽�ı༭
	bool						m_bEndEditMapBoarder;
	// ��ǰ�Ƿ��Ѿ������˵�ͼ�߽�
	bool						m_bHasGenMapBorder;

	// ��ǰ����ͼģʽ
	bool						m_bEnableDepthTest;

	// �Ƿ���ʾNAV��������
	bool						m_bIsVisable;

	// ��ǰѡ�в����������ζ���
	AIRVector<CheckPoint>		m_vtCurCheckPoints;
	// �������������ζ���
	Vector3Df					m_vCheckPointPos;
	bool						m_bPickCheckPoint;

	// ��ǰ���еĵ�������������
	NavTriangle					m_CurPickTriangle;
	// ��ǰ�Ƿ��е��еĵ�������������
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