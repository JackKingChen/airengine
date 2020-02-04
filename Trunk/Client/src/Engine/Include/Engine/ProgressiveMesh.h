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


typedef std::set<PMVertex *>		NeighborList;		// �ھӵ��б�
typedef std::set<PMVertex *>		DuplicateList;		// �������б�
typedef std::set<PMTriangle *>		FaceList;			// �������б�
typedef AIRVector<PMTriangle>		TriangleList;		
typedef AIRVector<PMFaceVertex>	FaceVertexList;
typedef AIRVector<PMVertex>		CommonVertexList;
typedef AIRVector<float>			WorstCostList;
typedef AIRVector<PMWorkingData>	WorkingDataList;



// ���ڴ��������������ݵĽṹ��
struct VertexInfo
{
	size_t	m_nRealID;	 // ԭʼ���񶥵������е�ID
	size_t	m_nLatestID; // ��ǰlod�����µĶ��������е�ID
	bool	m_bRemoved;	 // �ö��㵱ǰ�Ƿ���ɾ����
};
typedef AIRVector<VertexInfo>		VEC_VERTEX_INFO;



// ������Ķ�����Ϣ. 
// ��¼�������õ���ʵ�ʶ���������Լ�ָ�����ڱ����ֵ�Ĺ�������
class PMFaceVertex 
{
public:
	size_t		realIndex;		// �����ID
	PMVertex*	commonVertex;	// ��¼������Ϣ�Ķ���
};



// ���������е�������, ����һЩ�������Ϣ(���淨��)
class PMTriangle 
{
public:
	PMTriangle();

	// ����������ϸ����Ϣ
	void			SetDetails(size_t newindex, PMFaceVertex *v0, PMFaceVertex *v1, PMFaceVertex *v2);

	// ���������η���
	void			ComputeNormal(void);

	// �û������涥��
	void			ReplaceVertex(PMFaceVertex *vold, PMFaceVertex *vnew);

	// �Ƿ��й�ͬ����
	bool			HasCommonVertex(PMVertex *v) const;

	// �Ƿ����涥��
	bool			HasFaceVertex(PMFaceVertex *v) const;

	// �ӹ�ͬ�����л���涥��
	PMFaceVertex*	GetFaceVertexFromCommon(PMVertex* commonVert);

	// ������ǰ�����Ƴ�������
	void			NotifyRemoved(void);

public:
	
	PMFaceVertex*	vertex[3];	// �����ε�3����
	Vector3Df		normal;		// �����εķ���
	bool			removed;	// ��ǰ�Ƿ�����Ƴ���������
	size_t			index;		// ����������
};



// ��������Ķ���
class PMVertex 
{
public:
    PMVertex();

	// ���ö���ϸ����Ϣ
    void			SetDetails(const Vector3Df& v, size_t newindex);

	// ���n�����ھӶ���,���Ƴ�֮
    void			RemoveIfNonNeighbor(PMVertex *n);

	// �Ƿ񼸺���ı߽�
	bool			IsBorder(void);

	// ���õ���v��ɵı��Ƿ�ֻ��һ���ڽ�������
	bool			IsManifoldEdgeWith(PMVertex* v);

	// �����Ƿ�����Ƴ�
	void			NotifyRemoved(void);

public:
	
    Vector3Df		position;		// ŷʽ�ռ������
    size_t			index;			// ��ʼ���񶥵��б��еĶ�������
    NeighborList	neighbor;		// �ڽӶ����б�
	FaceList		face;			// �ڽ��������б�
    float			collapseCost;	// ���۵��Ĵ���
    PMVertex *		collapseTo;		// ���۵��ĺ�ѡ����
    bool			removed;		// ��ǰ�Ƿ�����Ƴ��ö���
	bool			toBeRemoved;	// ����ɾ��
	bool			seam;			// �Ƿ�ӷ�

};


// �������ݼ�
struct PMWorkingData
{
	TriangleList		mTriList;		// ��ʼ������������б�
	FaceVertexList		mFaceVertList;	// ��ʼ����������涥���б�
	CommonVertexList	mVertList;		// ��ʼ����Ķ�����Ϣ�б�
};



// ����������С��������
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



// ��������
class ProgressiveMesh
{
public:
	ProgressiveMesh( CAIRSubMesh* pSrcSubMesh );

	~ProgressiveMesh( void );

	// ��ӹ������ݼ�
	void AddWorkingData();

	// ��ʼ�����б��۵�����ֵ��Ϣ
	void InitialiseEdgeCollapseCosts(void);

	// ���㶥��src��Ϊdest�Ļ���ֵ
	float ComputeEdgeCollapseCost(PMVertex *src, PMVertex *dest);

	// �������ݼ��е�vertIndex��������۵���Ϣ
	float ComputeEdgeCostAtVertexForBuffer(WorkingDataList::iterator idata, size_t vertIndex);

	// �����vertIndex��������۵���Ϣ
	void ComputeEdgeCostAtVertex(size_t vertIndex);

	// �������е��۵���Ϣ
	void ComputeAllCosts(void);

	// �����һ�����۵�����
	size_t GetNextCollapser(void);

	// �۵�����src
	void Collapse( PMVertex *src );

	// ���������lod��Ϣ
	void Build( int numLevels, MESH_LOD_LIST& outList, VertexReductionType quota = VRT_PROPORTIONAL, float reductionValue = 0.5f );

	// �������������ݵ�lod��Ϣ
	void BakeNewLODShareVB( LodData* pLodData, LodData* pLod0 );

	// ��¼������Ϣ���ļ�
	void DumpContent( const AIRString& log );

	// ���������������ݵ�lod��Ϣ
	void BakeNewLODIndependentVB( LodData* pLodData, LodData* pLod0  );
	

private:
	CAIRSubMesh *	m_pOriginSubMesh;		// ԭʼ����
	size_t			mCurrNumIndexes;		// ��ǰ����������
	size_t			mNumCommonVertices;		// ������
	WorkingDataList mWorkingData;			// �������ݼ�
	WorstCostList	mWorstCosts;			// �۵�������Ϣ����

};//! end class ProgressiveMesh






#endif //! __ProgressiveMesh_H__