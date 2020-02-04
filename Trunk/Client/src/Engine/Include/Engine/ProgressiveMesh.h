#pragma once

#ifndef __ProgressiveMesh_H__
#define __ProgressiveMesh_H__


#include "AIRSubMesh.h"
#include "Engine.h"
#include "Vector3D.h"
#include <set>


#define NEVER_COLLAPSE_COST 99999.9f


enum VertexReductionType
{
	VRT_CONSTANT,
	VRT_PROPORTIONAL,
};


class	PMFaceVertex;
class	PMVertex;
class	PMTriangle;
struct	PMWorkingData;


typedef std::set<PMVertex *>		NeighborList;		// 邻居点列表
typedef std::set<PMVertex *>		DuplicateList;		// 丢弃点列表
typedef std::set<PMTriangle *>		FaceList;			// 三角面列表
typedef AIRVector<PMTriangle>		TriangleList;		
typedef AIRVector<PMFaceVertex>	FaceVertexList;
typedef AIRVector<PMVertex>		CommonVertexList;
typedef AIRVector<float>			WorstCostList;
typedef AIRVector<PMWorkingData>	WorkingDataList;



// 用于创建独立顶点数据的结构体
struct VertexInfo
{
	size_t	m_nRealID;	 // 原始网格顶点序列中的ID
	size_t	m_nLatestID; // 当前lod级别下的顶点序列中的ID
	bool	m_bRemoved;	 // 该顶点当前是否是删除的
};
typedef AIRVector<VertexInfo>		VEC_VERTEX_INFO;



// 三角面的顶点信息. 
// 记录三角面用到的实际顶点的索引以及指向用于表面估值的公共顶点
class PMFaceVertex 
{
public:
	size_t		realIndex;		// 顶点的ID
	PMVertex*	commonVertex;	// 记录评估信息的顶点
};



// 渐进网格中的三角面, 保存一些额外的信息(如面法线)
class PMTriangle 
{
public:
	PMTriangle();

	// 设置三角形细节信息
	void			SetDetails(size_t newindex, PMFaceVertex *v0, PMFaceVertex *v1, PMFaceVertex *v2);

	// 计算三角形法线
	void			ComputeNormal(void);

	// 置换三角面顶点
	void			ReplaceVertex(PMFaceVertex *vold, PMFaceVertex *vnew);

	// 是否有共同顶点
	bool			HasCommonVertex(PMVertex *v) const;

	// 是否有面顶点
	bool			HasFaceVertex(PMFaceVertex *v) const;

	// 从共同顶点中获得面顶点
	PMFaceVertex*	GetFaceVertexFromCommon(PMVertex* commonVert);

	// 声明当前可以移除三角面
	void			NotifyRemoved(void);

public:
	
	PMFaceVertex*	vertex[3];	// 三角形的3个点
	Vector3Df		normal;		// 三角形的法线
	bool			removed;	// 当前是否可以移除该三角形
	size_t			index;		// 三角形索引
};



// 渐进网格的顶点
class PMVertex 
{
public:
    PMVertex();

	// 设置顶点细节信息
    void			SetDetails(const Vector3Df& v, size_t newindex);

	// 如果n不是邻居顶点,就移除之
    void			RemoveIfNonNeighbor(PMVertex *n);

	// 是否几何体的边界
	bool			IsBorder(void);

	// 检查该点与v组成的边是否只有一个邻接三角形
	bool			IsManifoldEdgeWith(PMVertex* v);

	// 声明是否可以移除
	void			NotifyRemoved(void);

public:
	
    Vector3Df		position;		// 欧式空间的坐标
    size_t			index;			// 初始网格顶点列表中的顶点索引
    NeighborList	neighbor;		// 邻接顶点列表
	FaceList		face;			// 邻接三角面列表
    float			collapseCost;	// 边折叠的代价
    PMVertex *		collapseTo;		// 边折叠的候选顶点
    bool			removed;		// 当前是否可以移除该顶点
	bool			toBeRemoved;	// 即将删除
	bool			seam;			// 是否接缝

};


// 工作数据集
struct PMWorkingData
{
	TriangleList		mTriList;		// 初始网格的三角形列表
	FaceVertexList		mFaceVertList;	// 初始网格的三角面顶点列表
	CommonVertexList	mVertList;		// 初始网格的顶点信息列表
};



// 容器将按从小到大排列
struct vectorLess
{
	bool operator()(const Vector3Df& v1, const Vector3Df& v2) const
    {
		if (v1.x < v2.x) return true;
		if (v1.x == v2.x && v1.y < v2.y) return true;
		if (v1.x == v2.x && v1.y == v2.y && v1.z < v2.z) return true;

		return false;
	}
};



// 渐进网格
class ProgressiveMesh
{
public:
	ProgressiveMesh( CAIRSubMesh* pSrcSubMesh );

	~ProgressiveMesh( void );

	// 添加工作数据集
	void AddWorkingData();

	// 初始化所有边折叠花费值信息
	void InitialiseEdgeCollapseCosts(void);

	// 计算顶点src变为dest的花费值
	float ComputeEdgeCollapseCost(PMVertex *src, PMVertex *dest);

	// 计算数据集中第vertIndex个顶点的折叠信息
	float ComputeEdgeCostAtVertexForBuffer(WorkingDataList::iterator idata, size_t vertIndex);

	// 计算第vertIndex个顶点的折叠信息
	void ComputeEdgeCostAtVertex(size_t vertIndex);

	// 计算所有的折叠信息
	void ComputeAllCosts(void);

	// 获得下一个可折叠顶点
	size_t GetNextCollapser(void);

	// 折叠顶点src
	void Collapse( PMVertex *src );

	// 建立网格的lod信息
	void Build( int numLevels, MESH_LOD_LIST& outList, VertexReductionType quota = VRT_PROPORTIONAL, float reductionValue = 0.5f );

	// 创建共享顶点数据的lod信息
	void BakeNewLODShareVB( LodData* pLodData, LodData* pLod0 );

	// 记录创建信息到文件
	void DumpContent( const AIRString& log );

	// 创建独立顶点数据的lod信息
	void BakeNewLODIndependentVB( LodData* pLodData, LodData* pLod0  );
	

private:
	CAIRSubMesh *	m_pOriginSubMesh;		// 原始网格
	size_t			mCurrNumIndexes;		// 当前的索引数量
	size_t			mNumCommonVertices;		// 顶点数
	WorkingDataList mWorkingData;			// 工作数据集
	WorstCostList	mWorstCosts;			// 折叠花费信息容器

};//! end class ProgressiveMesh






#endif //! __ProgressiveMesh_H__