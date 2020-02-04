// ***************************************************************
//  文件: PageStaticGeometory.h
//  日期: 2010-5-27
//  作者: huangjunjie
//  说明: 静态几何体页
// ***************************************************************

#pragma once

#ifndef __PageStaticGeometory_H__
#define __PageStaticGeometory_H__

//#include "MemoryAllocatorConfig.h"
#include "Renderable.h"
#include "Engine.h"
//#include "PlantObject.h"
#include "GPUDataParam.h"
//#include "ManualGPUDataParamSource.h"
#include "IteratorWrappers.h"


// 物体初始化信息
class EngineExport ObjectInitInfo
{
public:
	ObjectInitInfo() : m_vPosition(Vector3Df::ZERO), m_vScale(1.0f, 1.0f, 1.0f), m_fAngle(0.0f), m_fBaseWidth(1.0f), m_fBaseHeight(1.0f)	{}

	inline void operator = ( const ObjectInitInfo& other )
	{
		m_vScale = other.m_vScale;
		m_vPosition = other.m_vPosition;
		m_fAngle = other.m_fAngle;
		m_fBaseWidth = other.m_fBaseWidth;
		m_fBaseHeight = other.m_fBaseHeight;
	}

	void normalize()
	{
		m_vScale.x *= m_fBaseWidth;
		m_vScale.y *= m_fBaseHeight;
		m_vScale.z *= m_fBaseWidth;
		m_fBaseWidth = m_fBaseHeight = 1.0f;
	}

	Vector3Df	m_vPosition;	// 世界坐标
	Vector3Df	m_vScale;		// 缩放
	float		m_fAngle;		// 绕Y轴旋转的角度
	float		m_fBaseWidth;	// 基准宽度
	float		m_fBaseHeight;	// 基准高度
};

typedef AIRVector< Vector3Df >		VEC_OBJECT_OFFSET;
typedef AIRVector< ObjectInitInfo >		LST_OBJECT_INIT_INFO;

enum ePlantFaceType
{
    ePFT_Always_Face_Camera = 0,	// 以billboard的形式,总是面向摄像机
    ePFT_Horz_Face_Camera = 1,		// 以billboard的形式,垂直于地面并面向摄像机 
    ePFT_Never_Face_Camera = 2,		// 植物体的顶点不会随视角的变化而变
};


enum PSG_ANIM_TYPE
{
	ePsgAnimNone,			// 没有动画
	ePsgAnimSameOffset,		// 所有的物体使用相同的偏移量 (CPU操作,锁缓冲,慢)
	ePsgAnimDiffOffset,		// 每个物体使用各自的偏移量 (CPU操作,锁缓冲,慢)
	ePsgAnimShearMatrix,	// 使用切变矩阵进行操作(只适用于放在Y=0平面上的物体) (CPU操作,不锁缓冲,快)
	ePsgAnimUseVS,			// 使用VS进行的动画操作 (GPU操作,不锁缓冲,快)
};



// 静态页几何体
class EngineExport PageStaticGeometory : public Renderable
{
public:
	PageStaticGeometory();

	virtual ~PageStaticGeometory( void );

	// 渲染
	//virtual void Render(CCamera* pCamera);

	// 清除
	virtual void Clear();

	virtual const Matrix4f& GetWorldMatrix() const;

	// 每帧更新
	//virtual void Update( float fTimeElapse );

	// 生成页几何体数据
	//virtual void GenPageStaticGeometryData( const LST_OBJECT_INIT_INFO& vtObjectInitInfo, PlantObject* pObjRef );

	// 对所有的对象进行动画操作
	virtual void AnimAllObjects( const Vector3Df& vOffset );

	// 对所有的对象进行动画操作
	virtual void AnimAllObjects( const VEC_OBJECT_OFFSET& vtOffset );

	// 对所有的对象进行动画操作
	virtual void AnimAllObjects( const Matrix4f& matShear );

	// 对所有的对象进行动画操作
	virtual void AnimAllObjects( CCamera* pCamera );

	// 设置材质
	virtual void SetMaterial(IMaterial* pMaterial);

	// 设置物体的动画类型
	virtual void SetPsgAnimType( PSG_ANIM_TYPE pat );

	virtual void SetMaxXOffset( float fMaxXOffset )
	{
		m_fXMaxOffset = fMaxXOffset;
	}

	virtual void SetMaxZOffset( float fMaxZOffset )
	{
		m_fZMaxOffset = fMaxZOffset;
	}

	virtual float GetMaxXOffset( void )
	{
		return m_fXMaxOffset;
	}

	virtual float GetMaxZOffset( void )
	{
		return m_fZMaxOffset;
	}

	void SetOffsetFreq( float fFreq )
	{
		m_fOffsetFreq = fFreq;
	}

	float GetOffsetFreq( void )
	{
		return m_fOffsetFreq;
	}

	void SetObjectWidth( float width )
	{
		m_fObjectWidth = width;
	}

	float GetObjectWidth( void )
	{
		return m_fObjectWidth;
	}

	void SetObjectHeight( float height )
	{
		m_fObjectHeight = height;
	}

	float GetObjectHeight( void )
	{
		return m_fObjectHeight;
	}

	void SetQuadFaceCountPerObject( int nQuadFaceCountPerObj )
	{
		m_nQuadFaceCountPerObj	= nQuadFaceCountPerObj;
	}

    //渲染到GBuffer的technique，用于延迟渲染
    //virtual void GenRenderToGBufferTechnique();

    //virtual void RenderToShadowMap(CCamera* pCamera);

    virtual void SetAcceptLighting(bool bAccept);

    virtual void ResetCurrentTechnique();

    //PSGManualGPUDataParamSource* GetGPUDataParamSource()
    //{
    //    return m_gpuDataParamSource;
    //}
public:
	// for editor

	// 添加物体
	virtual void AddPlantObject( const LST_OBJECT_INIT_INFO& vtObjectInitInfo, float fWidth, float fHeight, int nQuadFaceCountPerObj, ePlantFaceType ePFT );

	void CreateInternal();

	// 删除物体
	virtual void DelPlantObject( const Vector3Df& vCenterPos, float fRadius );

	// 设置页几何体所能容纳的最大物体数
	virtual void SetMaxObjectCount( size_t nMaxObjectCount );

	// 重建几何体数据
	virtual void RebuildGeometryData( void );

	virtual void BuildGeometryDataFromBin( u8**pData );

	virtual bool GetIsCurPage( float fWidth, float fHeight, int nQuadFaceCountPerObj, ePlantFaceType ePFT, AIRString& szTexFileName );

	// 获得当前植物体的数量
	size_t		 GetObjectCount( void )
	{
		return m_nObjectCount;
	}

	// 保存到XML
	virtual bool SaveToXML( CMarkupSTL& xml );

	virtual bool SaveToBinFile( std::ofstream& fout );

	// 从XML加载
	virtual bool CreateFromXML( CMarkupSTL& xml );

	virtual bool CreateFromBinData( u8** pData );

	// 设置对象的纹理
	virtual void SetObjectTexture( const AIRString& szTex );

	// 获得对象的纹理
	virtual const AIRString& GetObjectTexture( void ) const
	{
		return m_szPlantTexFile;
	}

	// 复制对象初始化创建信息
	virtual void CopyObjectInitInfo( LST_OBJECT_INIT_INFO& vtDstObjectInitInfo, const Vector3Df& vNewCenterPosY );

    virtual bool Prepare();

	VectorIterator<LST_OBJECT_INIT_INFO> GetObjectInitInfo()
	{
		return VectorIterator<LST_OBJECT_INIT_INFO>(m_vtObjectInitInfo);
	}

	const AIRString& GetPlantTexFile() const
	{
		return m_szPlantTexFile;
	}

	void ClearAllObjectInitInfo()
	{
		m_vtObjectInitInfo.clear();
	}

	void AddCluster( const ObjectInitInfo& objInitInfo )
	{
		m_vtObjectInitInfo.push_back( objInitInfo );
	}

	int GetClusterCount( void )
	{
		return m_vtObjectInitInfo.size();
	}

	const Vector3Df& GetObjectInitCenter() const
	{
		return m_vObjectInitCenter;
	}

	void SetObjectInitCenter( const Vector3Df& v )
	{
		m_vObjectInitCenter = v;
		//m_worldPosition = v;
	}

	AIRString& GetNodeName()
	{
		return m_szNodeName;
	}

    //const Vector3Df& GetInitWorldPosition() const
    //{
    //    return m_worldPosition;
    //}

protected:
    // 渲染预处理
    virtual void PreRender(CCamera* pCamera);

    virtual void _GetPrimitive();




protected:
	LST_OBJECT_INIT_INFO			m_vtObjectInitInfo;			// 记录植物体的位置,大小,朝向的信息的容器
	size_t							m_nVertexCountPerObject;	// 每个植物体的顶点数量
	size_t							m_nIndexCountPerObject;		// 每个植物体的索引数量
	size_t							m_nObjectCount;				// 植物体数量
	bool							m_bHasGenVertexData;		// 是否已经创建顶点数据
	Matrix4f						m_matShear;					// 物体切变矩阵
	//PSGManualGPUDataParamSource*    m_gpuDataParamSource;		// gpu参数
	float							m_fXMaxOffset;				// x坐标的最大偏移量
	float							m_fZMaxOffset;				// z坐标的最大偏移量
	VEC_OBJECT_OFFSET				m_vtObjectOffsetInit;		// 
	VEC_OBJECT_OFFSET				m_vtObjectOffsetCur;		// 
	PSG_ANIM_TYPE					m_PsgAnimType;				// 动画操作类型
	float							m_fObjectHeight;			// 植物体的高度
	float							m_fObjectWidth;				// 植物体的宽度
	ePlantFaceType					m_PlantFaceType;			// 植物体的朝向方式
	size_t							m_nMaxObjectCount;			// 最大的物体数量
	size_t							m_nQuadFaceCountPerObj;		// 每个物体的四边面数
	AIRString						m_szPlantTexFile;
    ITexture*                       m_pPlantTexture;
	float							m_fOffsetFreq;
private:
	float							m_fXPos;
	float							m_fZPos;
	//bool							m_bHasUpdated;
	Vector3Df						m_vObjectInitCenter;
	AIRString						m_szNodeName;
    //Vector3Df                       m_worldPosition;     //新增变量

};//! end class PageStaticGeometory


typedef AIRList<PageStaticGeometory*>				LST_PSG;
typedef std::map<AIRString, LST_PSG*>				MAP_MATERIAL_PSG_LIST;
typedef std::map<int, MAP_MATERIAL_PSG_LIST*>		MAP_TRUNK_MATERIAL_PSG_LIST;
//
//class EngineExport PsgManager
//{
//public:
//	PsgManager( void ) : m_nID(0), m_bShowAllPSG(true), m_bIsTrunkPSG(false), m_nTrunkRow(0), m_nTrunkCol(0)
//	{
//
//	}
//
//	virtual ~PsgManager( void )
//	{
//
//	}
//
//	static PsgManager* GetSingleton( void )
//	{
//		static PsgManager inst;
//		return &inst;
//	}
//
//	PageStaticGeometory* CreatePageStaticGeometry( void );
//
//	void ClearPageStaticGeometry( PageStaticGeometory* pPsg );
//
//	void ClearAllPageStaticGeometry( void );
//
//	PageStaticGeometory* GetPageStaticGeometry( const char* szName );
//
//	bool SaveToXML( const char* szFile );
//
//	bool SaveToBinFile( const char* szFile );
//
//	bool CreateFromXML( CMarkupSTL& xml );
//
//	bool LoadFromFile( const char* szFile );
//
//	bool LoadFromBinFile( const char* szFile );
//
//	const AIRString& GetEditFileName( void ) const
//	{
//		return m_szFile;
//	}
//
//	void SetEditFileName( const AIRString& szFile )
//	{
//		m_szFile = szFile;
//	}
//
//	bool IsEmpty( void )
//	{
//		return (m_lstPSG.size() == 0);
//	}
//
//	void SetShowBoundingBox( bool b );
//
//	void SetShowAllPSG( bool bShow );
//
//	bool GetShowAllPSG( void )
//	{
//		return m_bShowAllPSG;
//	}
//
//	void SetParmToAllPlant( float fXMaxOffset, float fZMaxOffset, float fOffsetFreq );
//
//	void UpdateAllPageStaticGeometory( float fFrameTime );
//
//	//void GenerateOptimiseTrunkPSG( int nRowCount, int nColCount );
//
//	void DestroyAllPSG();
//
//	int GetTrunkRow()
//	{
//		return m_nTrunkRow;
//	}
//
//	int GetTrunkCol()
//	{
//		return m_nTrunkCol;
//	}
//
//    void ResetAllPlantsTechnique();
//
//protected:
//	LST_PSG			m_lstPSG;
//	AIRString		m_szFile;
//	u32			m_nID;
//	bool			m_bShowAllPSG;
//	bool			m_bIsTrunkPSG;
//	int				m_nTrunkRow;
//	int				m_nTrunkCol;
//};
//
//
////#define PSG_MANAGER (PsgManager::GetSingleton())
//










#endif //! __PageStaticGeometory_H__