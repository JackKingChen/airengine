// ***************************************************************
//  AIRSubMesh   version:  1.0   ��  date: 01/01/2010
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

//Ȩֵ����
typedef struct tagWeight 
{
    int   nBoneID;		//��ͷ����
    float fWeight;		//Ȩֵ
}MESHWEIGHT, *LPMESHWEIGHT;



//��Ӳ�����ٵ����񶥵�����
typedef struct tagMeshVertNormal
{
    VERTEX_NORMAL			vertex;         //��������
	u8 boneID[4];
	float fWeights[4];
    //int						nWeightIndex;   //Ȩֵ������
    //int						nWeightCount;   //�������ſ�ʼ��Ȩ�ظ���
}MESHVERTNORMAL, *LPMESHVERTNORMAL;


//����Ӳ�����ٵ����񶥵�����
typedef struct tagHardwareMeshVertex
{
    VERTEXNORMALBONEWEIGHTS vertex;			//��Ȩ�صĶ����ʽ
    //int						nWeightIndex;   //Ȩֵ������
    //int						nWeightCount;   //�������ſ�ʼ��Ȩ�ظ���
}HARDWAREMESHVERTEX, *LPHARDWAREMESHVERTEX;

class CModelInstance;
class AIRModel;
class AIRKeyframeAnimationRes;

//��������
enum MeshAnimationType
{
    eMAT_NoAnimation = 0,						//��̬����
    eMAT_SkeletionAnimation = 1,			//��������
    eMAT_VertexAnimation = 2,				//���㶯��
};



typedef AIRVector<MESHWEIGHT>				SOFTWARE_BLEND_WEIGHT_LIST;
typedef AIRVector<LPMESHVERTNORMAL>		SOFTWARE_BLEND_VERTEX_LIST;
typedef AIRVector<LPHARDWAREMESHVERTEX>	HARDWARE_BLEND_VERTEX_LIST;
typedef AIRVector<AIRMESHTRI>				FACE_LIST;

struct VertexData 
{
	VertexData():m_pMeshVB(NULL), m_pMeshIB(NULL)	{}

	IVertexBuffer*	m_pMeshVB;					// ��ǰlod�µĶ��㻺��
	IIndexBuffer*	m_pMeshIB;					// ��ǰlod�µ���������
};

// �����lod������Ϣ
// ˵��: ��0�㱣�����ԭʼ��������;
//		 �ӵ�1����,ÿ���m_pvtHardwareBlendVertex, m_pvtSoftwareBlendVertex, m_pvtFaces, m_pMeshVB, m_pMeshIB�Ƕ�������������;
//		 ����0��� m_pvtSoftwareBlendWeight ���õ��ǵ�0��� m_pvtSoftwareBlendWeight;
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
	//SOFTWARE_BLEND_WEIGHT_LIST* m_pvtSoftwareBlendWeight;	// ��ǰlod�µĶ���Ȩ���б�
	//SOFTWARE_BLEND_VERTEX_LIST* m_pvtSoftwareBlendVertex;	// ��ǰlod�µ������Ƥ�����б�
	//HARDWARE_BLEND_VERTEX_LIST* m_pvtHardwareBlendVertex;	// ��ǰlod�µ�Ӳ����Ƥ�����б�
	//FACE_LIST* m_pvtFaces;					// ��ǰlod�µ��������б�
	//IVertexBuffer* m_pMeshVB;					// ��ǰlod�µĶ��㻺��
	//IIndexBuffer* m_pMeshIB;					// ��ǰlod�µ���������

	//SOFTWARE_BLEND_WEIGHT_LIST	m_pvtSoftwareBlendWeight;	// ��ǰlod�µĶ���Ȩ���б�
	SOFTWARE_BLEND_VERTEX_LIST	m_pvtSoftwareBlendVertex;	// ��ǰlod�µ������Ƥ�����б�
	HARDWARE_BLEND_VERTEX_LIST	m_pvtHardwareBlendVertex;	// ��ǰlod�µ�Ӳ����Ƥ�����б�
	FACE_LIST					m_pvtFaces;					// ��ǰlod�µ��������б�
	VertexData					m_VertexData;				// ��ǰlod�µĶ�����Ⱦ����
};

typedef AIRVector< LodData* >				MESH_LOD_LIST;



class EngineExport CAIRSubMesh// : public EngineAllocator(CAIRSubMesh)
{
	EngineAllocator(CAIRSubMesh)
public:
    CAIRSubMesh();

	~CAIRSubMesh();

    //virtual void						Render(CCamera* pCamera);

    // �������㻺��
    //@nVertexCount ��������
    void CreateVertices(int nVertexCount);

    // ������������
    //@nFacesCount ����
    void CreateIndices(int nFacesCount);

	// ���ı��ļ�����
    bool LoadFromFileData(CDataStream* pFileStream, const char* szFilename);

    // �Ӷ������ļ�����
    bool LoadFromFileDataBinary(CDataStream* pFileStream, AIRModel* pModel);

	//ֱ�Ӵ�����̬ģ��
	bool Create(LPVERTEX_NORMAL pVertices, u32 nVerticesNum, u16* pIndices, u32 nIndicesNum);

    //����������
	bool Create(LPMESHVERTNORMAL pVertices, u32 nVerticesNum, u16* pIndices, u32 nIndicesNum);

    // ���ù���
    //void SetSkeletonRes(CAIRSkeletonRes* pSkeleton)
    //{
    //    m_pSkeleton = pSkeleton;
    //}

	// �Ƿ�̬����
    bool IsStaticMesh() const
    {
        return m_animationType == eMAT_NoAnimation;
    }

	// ���ö�������
    void SetAnimationType(u8 type)
    {
        m_animationType = type;
    }

	// ���������Ķ�������
	u8 GetAnimationType( void ) const
	{
		return m_animationType;
	}

    // ���㶥������
    void CaculateVertices(const SColor& colorDiffuse, Matrix4f* pBoneMatrix, f32 fTimePos);

	// ���������͸����
    void SetTransparent(bool bAlpha = true, u8 nAlpha = 128);

	// �������Ĳ�������
    const AIRString& GetMaterialName() const
    {
        return m_strMaterialName;
    }

	// ���õ�ǰ�Ƿ���Ⱦ����ķ���
	void SetDrawMeshNormal( bool b );

	// �Ƿ��������
    bool IsPick(const Vector3Df& vOrg, const Vector3Df& vPickDir, const Matrix4f& matTransform);

	bool IsPick(const Vector3Df& vOrg, const Vector3Df& vPickDir, const Matrix4f& matTransform, float& distFromEyeToObject);

    // ���㶥�㶯��
    void CaculateVerticesByVertexAnimation(
		float fInterpolate, 
        VertexAnimationKeyframe* pKeyframe1,
		VertexAnimationKeyframe* pKeyframe2,
        s32 nAnimationMask);

	// ���lod�����б�
	void ClearLodList( void );

	// ���õ�ǰ��lod����
	void SetCurLodLevel( u16 nCurLodLevel );

	// �������ǰ��lod����
	u16 GetCurLodLevel( void )
	{
		return m_nCurLodLevel;
	}

	// ����ģ�͵�lod�����б�
	void CreateLodDataList( void );

	// ��ó�ʼ���������
    u32 GetFacesNum() const
    {
        return m_nFacesCount;
    }

	// ��ó�ʼ����Ķ�����
    u32 GetVerticesNum() const
    {
        return m_nVerticesCount;
    }

	// ��õ�ǰ��lod���������
	LodData* GetCurLodData( void )
	{
		return m_pCurLodData;
	}

	// ��������lod�б�
	MESH_LOD_LIST& GetLodList( void )
	{
		return m_LodList;
	}

	// ��õ�nLod���lod����
    LodData* GetLodData(size_t nLod);

	// ��ӵ�nLod���lod����
    void AddLodData(size_t nLod, LodData* pLodData);

	void Clear();


	////�������Ķ��㻺��
	IVertexBuffer* GetVertexBuffer();

	//����������������
	IIndexBuffer* GetIndexBuffer();

	//// ��������Ƥ��Ȩ���б�
	//SOFTWARE_BLEND_WEIGHT_LIST&		GetSoftwareBlendWeightList( void )
	//{
	//	return m_vtWeights;
	//}

	//// ��������Ƥ�Ķ����б�
	//SOFTWARE_BLEND_VERTEX_LIST&		GetSoftwareBlendVertexList( void )
	//{
	//	return m_vtVertices;
	//}

	//// ���Ӳ����Ƥ�Ķ����б�
	//HARDWARE_BLEND_VERTEX_LIST&		GetHardwareBlendVertexList( void )
	//{
	//	return m_vtHardwareVertices;
	//}

	//// ����������б���Ϣ
	//FACE_LIST&						GetFaceList( void )
	//{
	//	return m_vtFaces;
	//}

	//// ��ó�ʼ������������б�
	//const FACE_LIST&					GetMeshFaces() const
	//{
	//	return m_vtFaces;
	//}

	//// ��ó�ʼ����������Ƥ/��̬����/���㶯���Ķ����б�
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

    //���㶥��λ�ã������Ǵ���̬ģ��
    void ComputeVertexBuffer(AIRVector<VERTEX_NORMAL>* vertices);

    //������Ǵ��������ݵ�ģ��
    void ComputeVertexBuffer(AIRVector<LPMESHVERTNORMAL>* vertices);

    //�������vs�Ķ�������
    void ComputeVertexBuffer(AIRVector<LPHARDWAREMESHVERTEX>* vertices);

	//������������
    void ComputeIndexBuffer(AIRVector<AIRMESHTRI>* vtFaces);

    //���㷨��
    void ComputeVertexNormal(AIRVector<VERTEX_NORMAL>* vertices, AIRVector<AIRMESHTRI>* vtFaces);

	//���㷨��
    void ComputeVertexNormal(AIRVector<LPMESHVERTNORMAL>* vertices, AIRVector<AIRMESHTRI>* vtFaces);

    //�������vertexshader�ķ���
    void ComputeVertexNormal(AIRVector<LPHARDWAREMESHVERTEX>* vertices, AIRVector<AIRMESHTRI>* vtFaces);
         
	//�����Ⱦ����Ϣ
    void ClearRenderableImp( void );
    
	//���������淨��
    void ComputeFaceNormal( const Vector3Df& v1, const Vector3Df& v2, const Vector3Df& v3, Vector3Df& vNormal );

    //��֡���ݼ��㶥��λ��
    void CaculateVertices( const SColor& colorDiffuse, Matrix4f* pBoneMatrix, LPVERTEX_NORMAL* pVertex, f32 fTimePos);

	//�������߶��㻺��
	void GenNormalVB( void );

	//��յ�nLod���lod����
	void ClearLodData( size_t nLod );

	

private:
    //IVertexBuffer*					m_pVertexBuffer;			//���㻺��
    //IIndexBuffer*						m_pIndexBuffer;				//��������
	//SOFTWARE_BLEND_WEIGHT_LIST		m_vtWeights;				//Ӱ���ģ�͵�Ȩ��
	//SOFTWARE_BLEND_VERTEX_LIST		m_vtVertices;				//��ʼ��������
	//HARDWARE_BLEND_VERTEX_LIST		m_vtHardwareVertices;		//����vs�Ķ�������
	//FACE_LIST							m_vtFaces;					//����Ϣ
	//AIRVector<VERTEX_NORMAL>        m_vtNoSkelVertices;         //û�й������ݵĹ�������

    s32									m_nVerticesCount;			//������
    s32									m_nFacesCount;				//����
    AIRString							m_strMaterialName;			//��������
    //CAIRSkeletonRes*					m_pSkeleton;				//Ӱ���mesh�Ĺ�����Դ�����ָ��Ϊ�գ���Ϊ��̬ģ��
    AIRKeyframeAnimationRes*	        m_pVertexAnimationRes;	// ���㶯��
	IVertexBuffer*						m_pNormalVB;				//�������������ݵĶ��㻺��
	bool								m_bGenNormalVB;				//�Ƿ��Ѿ����ɷ���
    u8					m_animationType;			//��������
	MESH_LOD_LIST						m_LodList;					//lod��������
	u16									m_nCurLodLevel;				//��ǰlod����
	LodData*							m_pCurLodData;				//��ǰlod���������
    IMaterial*                          m_pMaterial;                //��Ӧ��material
    AABBBox3Df  m_boundingBox;
	OBBoxf m_obb;
    AIRString m_strName;
};
