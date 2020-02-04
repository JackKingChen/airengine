// ***************************************************************
//  AIRSubMesh   version:  1.0   ·  date: 01/01/2010
//  -------------------------------------------------------------
//  Author liangairan
//  -------------------------------------------------------------
//  Copyright (C) 2010 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************
#pragma once

//#include "IReference.h"
#include "AIRStaticMesh.h"
#include "Camera.h"
#include "IVertexBuffer.h"
#include "IIndexBuffer.h"
//#include "Renderable.h"
#include "ITexture.h"
#include "VertexDef.h"
//#include "AIRSkeletonRes.h"
#include "DataStream.h"
#include "VertexAnimation.h"
#include "OBB.h"

//权值数据
typedef struct tagWeight 
{
    int   nBoneID;		//骨头索引
    float fWeight;		//权值
}MESHWEIGHT, *LPMESHWEIGHT;



//非硬件加速的网格顶点数据
typedef struct tagMeshVertNormal
{
    VERTEX_NORMAL			vertex;         //顶点数据
	u8 boneID[4];
	float fWeights[4];
    //int						nWeightIndex;   //权值索引号
    //int						nWeightCount;   //从索引号开始的权重个数
}MESHVERTNORMAL, *LPMESHVERTNORMAL;


//用于硬件加速的网格顶点数据
typedef struct tagHardwareMeshVertex
{
    VERTEXNORMALBONEWEIGHTS vertex;			//带权重的顶点格式
    //int						nWeightIndex;   //权值索引号
    //int						nWeightCount;   //从索引号开始的权重个数
}HARDWAREMESHVERTEX, *LPHARDWAREMESHVERTEX;

class CModelInstance;
class AIRModel;
class AIRKeyframeAnimationRes;

//动画类型
enum MeshAnimationType
{
    eMAT_NoAnimation = 0,						//静态网格
    eMAT_SkeletionAnimation = 1,			//骨骼动画
    eMAT_VertexAnimation = 2,				//顶点动画
};



typedef AIRVector<MESHWEIGHT>				SOFTWARE_BLEND_WEIGHT_LIST;
typedef AIRVector<LPMESHVERTNORMAL>		SOFTWARE_BLEND_VERTEX_LIST;
typedef AIRVector<LPHARDWAREMESHVERTEX>	HARDWARE_BLEND_VERTEX_LIST;
typedef AIRVector<AIRMESHTRI>				FACE_LIST;

struct VertexData 
{
	VertexData():m_pMeshVB(NULL), m_pMeshIB(NULL)	{}

	IVertexBuffer*	m_pMeshVB;					// 当前lod下的顶点缓冲
	IIndexBuffer*	m_pMeshIB;					// 当前lod下的索引缓冲
};

// 网格的lod数据信息
// 说明: 第0层保存的是原始网格数据;
//		 从第1层起,每层的m_pvtHardwareBlendVertex, m_pvtSoftwareBlendVertex, m_pvtFaces, m_pMeshVB, m_pMeshIB是独立创建的数据;
//		 而非0层的 m_pvtSoftwareBlendWeight 引用的是第0层的 m_pvtSoftwareBlendWeight;
class LodData// : public EngineAllocator(LodData)
{
	EngineAllocator(LodData)
public:
	//LodData( void ) : 
	//m_pvtSoftwareBlendWeight(NULL), 
	//m_pvtSoftwareBlendVertex(NULL),
	//m_pvtHardwareBlendVertex(NULL),
	//m_pvtFaces(NULL),
	//m_pMeshVB(NULL),
	//m_pMeshIB(NULL)
	//{}

	LodData( void )
	{

	}

	~LodData( void )
	{

	}

public:
	//SOFTWARE_BLEND_WEIGHT_LIST* m_pvtSoftwareBlendWeight;	// 当前lod下的顶点权重列表
	//SOFTWARE_BLEND_VERTEX_LIST* m_pvtSoftwareBlendVertex;	// 当前lod下的软件蒙皮顶点列表
	//HARDWARE_BLEND_VERTEX_LIST* m_pvtHardwareBlendVertex;	// 当前lod下的硬件蒙皮顶点列表
	//FACE_LIST* m_pvtFaces;					// 当前lod下的三角面列表
	//IVertexBuffer* m_pMeshVB;					// 当前lod下的顶点缓冲
	//IIndexBuffer* m_pMeshIB;					// 当前lod下的索引缓冲

	//SOFTWARE_BLEND_WEIGHT_LIST	m_pvtSoftwareBlendWeight;	// 当前lod下的顶点权重列表
	SOFTWARE_BLEND_VERTEX_LIST	m_pvtSoftwareBlendVertex;	// 当前lod下的软件蒙皮顶点列表
	HARDWARE_BLEND_VERTEX_LIST	m_pvtHardwareBlendVertex;	// 当前lod下的硬件蒙皮顶点列表
	FACE_LIST					m_pvtFaces;					// 当前lod下的三角面列表
	VertexData					m_VertexData;				// 当前lod下的顶点渲染数据
};

typedef AIRVector< LodData* >				MESH_LOD_LIST;



class EngineExport CAIRSubMesh// : public EngineAllocator(CAIRSubMesh)
{
	EngineAllocator(CAIRSubMesh)
public:
    CAIRSubMesh();

	~CAIRSubMesh();

    //virtual void						Render(CCamera* pCamera);

    // 创建顶点缓冲
    //@nVertexCount 顶点数量
    void CreateVertices(int nVertexCount);

    // 创建索引数据
    //@nFacesCount 面数
    void CreateIndices(int nFacesCount);

	// 从文本文件加载
    bool LoadFromFileData(CDataStream* pFileStream, const char* szFilename);

    // 从二进制文件读入
    bool LoadFromFileDataBinary(CDataStream* pFileStream, AIRModel* pModel);

	//直接创建静态模型
	bool Create(LPVERTEX_NORMAL pVertices, u32 nVerticesNum, u16* pIndices, u32 nIndicesNum);

    //带骨骼动画
	bool Create(LPMESHVERTNORMAL pVertices, u32 nVerticesNum, u16* pIndices, u32 nIndicesNum);

    // 设置骨骼
    //void SetSkeletonRes(CAIRSkeletonRes* pSkeleton)
    //{
    //    m_pSkeleton = pSkeleton;
    //}

	// 是否静态网格
    bool IsStaticMesh() const
    {
        return m_animationType == eMAT_NoAnimation;
    }

	// 设置动画类型
    void SetAnimationType(u8 type)
    {
        m_animationType = type;
    }

	// 获得子网格的动画类型
	u8 GetAnimationType( void ) const
	{
		return m_animationType;
	}

    // 计算顶点数据
    void CaculateVertices(const SColor& colorDiffuse, Matrix4f* pBoneMatrix, f32 fTimePos);

	// 设置网格的透明度
    void SetTransparent(bool bAlpha = true, u8 nAlpha = 128);

	// 获得网格的材质名称
    const AIRString& GetMaterialName() const
    {
        return m_strMaterialName;
    }

	// 设置当前是否渲染网格的法线
	void SetDrawMeshNormal( bool b );

	// 是否点中网格
    bool IsPick(const Vector3Df& vOrg, const Vector3Df& vPickDir, const Matrix4f& matTransform);

	bool IsPick(const Vector3Df& vOrg, const Vector3Df& vPickDir, const Matrix4f& matTransform, float& distFromEyeToObject);

    // 计算顶点动画
    void CaculateVerticesByVertexAnimation(
		float fInterpolate, 
        VertexAnimationKeyframe* pKeyframe1,
		VertexAnimationKeyframe* pKeyframe2,
        s32 nAnimationMask);

	// 清空lod数据列表
	void ClearLodList( void );

	// 设置当前的lod级别
	void SetCurLodLevel( u16 nCurLodLevel );

	// 获得网格当前的lod级别
	u16 GetCurLodLevel( void )
	{
		return m_nCurLodLevel;
	}

	// 创建模型的lod数据列表
	void CreateLodDataList( void );

	// 获得初始网格的面数
    u32 GetFacesNum() const
    {
        return m_nFacesCount;
    }

	// 获得初始网格的顶点数
    u32 GetVerticesNum() const
    {
        return m_nVerticesCount;
    }

	// 获得当前的lod级别的数据
	LodData* GetCurLodData( void )
	{
		return m_pCurLodData;
	}

	// 获得网格的lod列表
	MESH_LOD_LIST& GetLodList( void )
	{
		return m_LodList;
	}

	// 获得第nLod层的lod数据
    LodData* GetLodData(size_t nLod);

	// 添加第nLod层的lod数据
    void AddLodData(size_t nLod, LodData* pLodData);

	void Clear();


	////获得网格的顶点缓冲
	IVertexBuffer* GetVertexBuffer();

	//获得网格的索引缓冲
	IIndexBuffer* GetIndexBuffer();

	//// 获得软件蒙皮的权重列表
	//SOFTWARE_BLEND_WEIGHT_LIST&		GetSoftwareBlendWeightList( void )
	//{
	//	return m_vtWeights;
	//}

	//// 获得软件蒙皮的顶点列表
	//SOFTWARE_BLEND_VERTEX_LIST&		GetSoftwareBlendVertexList( void )
	//{
	//	return m_vtVertices;
	//}

	//// 获得硬件蒙皮的顶点列表
	//HARDWARE_BLEND_VERTEX_LIST&		GetHardwareBlendVertexList( void )
	//{
	//	return m_vtHardwareVertices;
	//}

	//// 获得网格面列表信息
	//FACE_LIST&						GetFaceList( void )
	//{
	//	return m_vtFaces;
	//}

	//// 获得初始网格的三角面列表
	//const FACE_LIST&					GetMeshFaces() const
	//{
	//	return m_vtFaces;
	//}

	//// 获得初始网格的软件蒙皮/静态网格/顶点动画的顶点列表
	//SOFTWARE_BLEND_VERTEX_LIST&		GetVerticesData()
	//{
	//	return m_vtVertices;
	//}

    void SetName(const char* szName)
    {
        m_strName = szName;
    }

    const AIRString& GetName() const
    {
        return m_strName;
    }

    const AABBBox3Df& GetBoundingBox() const
    {
        return m_boundingBox;
    }

    IMaterial* GetMaterial()
    {
        return m_pMaterial;
    }

    void CaculateKeyframeVertices(f32 fTimePos);

    void SetKeyframeAnimationRes(AIRKeyframeAnimationRes* pAnimationRes)
    {
        m_pVertexAnimationRes = pAnimationRes;
    }
	inline const OBBoxf& GetObb()
	{
		return m_obb;
	}

    void CacheVertexBuffer();

    void SetMaterial(IMaterial* pMaterial)
    {
        m_pMaterial = pMaterial;
    }

protected:
    //virtual void						PreRender(CCamera* pCamera);

    //计算顶点位置，这里是处理静态模型
    void ComputeVertexBuffer(AIRVector<VERTEX_NORMAL>* vertices);

    //处理的是带骨骼数据的模型
    void ComputeVertexBuffer(AIRVector<LPMESHVERTNORMAL>* vertices);

    //处理基于vs的顶点数据
    void ComputeVertexBuffer(AIRVector<LPHARDWAREMESHVERTEX>* vertices);

	//计算索引缓冲
    void ComputeIndexBuffer(AIRVector<AIRMESHTRI>* vtFaces);

    //计算法线
    void ComputeVertexNormal(AIRVector<VERTEX_NORMAL>* vertices, AIRVector<AIRMESHTRI>* vtFaces);

	//计算法线
    void ComputeVertexNormal(AIRVector<LPMESHVERTNORMAL>* vertices, AIRVector<AIRMESHTRI>* vtFaces);

    //计算基于vertexshader的法线
    void ComputeVertexNormal(AIRVector<LPHARDWAREMESHVERTEX>* vertices, AIRVector<AIRMESHTRI>* vtFaces);
         
	//清空渲染物信息
    void ClearRenderableImp( void );
    
	//计算三角面法线
    void ComputeFaceNormal( const Vector3Df& v1, const Vector3Df& v2, const Vector3Df& v3, Vector3Df& vNormal );

    //用帧数据计算顶点位置
    void CaculateVertices( const SColor& colorDiffuse, Matrix4f* pBoneMatrix, LPVERTEX_NORMAL* pVertex, f32 fTimePos);

	//创建法线顶点缓冲
	void GenNormalVB( void );

	//清空第nLod层的lod数据
	void ClearLodData( size_t nLod );

	

private:
    //IVertexBuffer*					m_pVertexBuffer;			//顶点缓冲
    //IIndexBuffer*						m_pIndexBuffer;				//索引缓冲
	//SOFTWARE_BLEND_WEIGHT_LIST		m_vtWeights;				//影响该模型的权重
	//SOFTWARE_BLEND_VERTEX_LIST		m_vtVertices;				//初始顶点数据
	//HARDWARE_BLEND_VERTEX_LIST		m_vtHardwareVertices;		//基于vs的顶点数据
	//FACE_LIST							m_vtFaces;					//面信息
	//AIRVector<VERTEX_NORMAL>        m_vtNoSkelVertices;         //没有骨骼数据的骨骼动画

    s32									m_nVerticesCount;			//顶点数
    s32									m_nFacesCount;				//面数
    AIRString							m_strMaterialName;			//材质名称
    //CAIRSkeletonRes*					m_pSkeleton;				//影响该mesh的骨骼资源，如果指针为空，则为静态模型
    AIRKeyframeAnimationRes*	        m_pVertexAnimationRes;	// 顶点动画
	IVertexBuffer*						m_pNormalVB;				//保存网格法线数据的顶点缓冲
	bool								m_bGenNormalVB;				//是否已经生成法线
    u8					m_animationType;			//动画类型
	MESH_LOD_LIST						m_LodList;					//lod数据容器
	u16									m_nCurLodLevel;				//当前lod级别
	LodData*							m_pCurLodData;				//当前lod级别的数据
    IMaterial*                          m_pMaterial;                //对应的material
    AABBBox3Df  m_boundingBox;
	OBBoxf m_obb;
    AIRString m_strName;
};
