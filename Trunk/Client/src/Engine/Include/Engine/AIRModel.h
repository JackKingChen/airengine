/********************************************************************
	created:	2010/01/27
	created:	27:1:2010   16:39
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\AIRModel.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	AIRModel
	file ext:	h
	author:		liangairan
	
	purpose:	模型资源，代表一个模型文件
*********************************************************************/
#pragma once

#include "Mesh.h"
#include "AIRSubMesh.h"
#include "AABBBox3D.h"
#include "Obb.h"
//#include "AIRSkeletonRes.h"
#include "AIRKeyframeAnimationRes.h"
#include "AIRAnimationMixRes.h"
class CAIRSkeletonAnimation;

struct EngineExport  OBBInf
{
	OBBInf(Vector3Df  OBBAxisX,
	Vector3Df  OBBAxisY,
	Vector3Df  OBBAxisZ,
	Vector3Df  OBBCenter)
	{
		m_OBBAxisX = OBBAxisX ;
		m_OBBAxisY = OBBAxisY;
		m_OBBAxisZ = OBBAxisZ;
		m_OBBCenter = OBBCenter;
	}
	Vector3Df  m_OBBAxisX;
	Vector3Df  m_OBBAxisY;
	Vector3Df  m_OBBAxisZ;
	Vector3Df  m_OBBCenter;
};

//骨骼原始数据，即未经动画变化前的数据
typedef struct tagAirBone 
{
	tagAirBone()
	{
		memset(szName, 0, 256);
	}
	char        szName[256];
	int         nParentID;   //父骨骼ID
	Vector3Df   vPosition;   //骨在初始帧里的模型空间的原点相对骨的位置 这里是由MAX中的Inverse(GetNodeTM)求得的
	Quaternion  qRotation;   //模型空间的原点相对骨的旋转 都是由由MAX中的Inverse(GetNodeTM)求得的
	//Vector3Df   positionAbsolute;    //骨骼在初始中的绝对位置
	//Quaternion  rotationAbsolute;    //骨骼在初始中的绝对旋转
	//Vector3Df   vPosToParent;  //相对于父亲的平移
	//Quaternion  qRotationToParent;  //相对于父亲的旋转
	AIRString strName;     //骨骼名字
}AIRBONE, *LPAIRBONE;

class EngineExport AIRModel : public Mesh
{
	DECLARE_RTTI;
public:
    AIRModel();
    virtual ~AIRModel();

    bool CreateFromFile(const char* szFilename);

    //从二进制文件创建
    virtual bool CreateFromBinaryFile(const char* szFilename);

    virtual s32 Release();

    virtual void Clear();

    bool IsLeftHandCoord() { return m_bLeftCoord; }

    u32  GetMeshCount() const { return (u32)m_mapSubMeshes.size(); }

    //CAIRSubMesh* GetMesh(u32 nIndex) { return m_vtSubMeshes[nIndex]; }
    CAIRSubMesh* GetMesh(const AIRString& strName);

    void GetMeshVector(AIRVector<CAIRSubMesh*>& meshVector);

    const AABBBox3Df& Getboundingbox() const { return m_boundingBox; }

    //const Vector3Df& GetCenter() const { return m_vCenter; }

    CAIRSkeletonAnimation* GetAnimation(const char* szAnimationName);

    //CAIRSkeletonRes* GetSkeletonRes()
    //{
    //    return m_pSkeletalRes;
    //}

    AIRKeyframeAnimationRes* GetVertexAnimationRes()
    {
        return m_pVertexAnimationRes;
    }

    //是否静态模型
    virtual bool IsStatic()
    {
        return m_animationType == 0;
        //return m_bStatic;
    }

    AIRAnimationMixRes* GetAnimationMixRes()
    {
        return m_pAnimationMixRes;
    }

    void SetAnimationMixRes(AIRAnimationMixRes* pAnimationMixRes)
    {
        m_pAnimationMixRes = pAnimationMixRes;
    }

    virtual bool Cache();

    //virtual bool Load();

    u8 GetAnimationType() const
    {
        return m_animationType;
    }

    f32 GetFileVersion() const
    {
        return m_fFileVersion;
    }

	void AddSubMesh(const AIRString& strName, CAIRSubMesh* pSubmesh);

	//void SetSkeletonRes(CAIRSkeletonRes* pSkeletonRes);

	LPAIRBONE GetBone(size_t nIndex)
	{
		return &m_vtBoneHierarchy[nIndex];
		//return m_baseSkeleton.GetBone(nIndex);
	}

	size_t GetBoneInstanceCount() const
	{
		return m_vtBoneHierarchy.size();
	}

	int GetBoneIndexByName(const char* szName);

	void CreateAnimationStates(AIRMap<AIRString, CAIRAnimationState*>& mapAnimations);

	CAIRSkeletonAnimation* GetAnimation( int nIndex );

	bool HasSkeletons() const
	{
		return !m_vtBoneHierarchy.empty();
	}

	void SetBoneHierarchy(const AIRVector<AIRBONE>& vtBoneHierarchy)
	{
		m_vtBoneHierarchy = vtBoneHierarchy;
	}

	void AddSkeletonAnimation(const AIRString& strName, CAIRSkeletonAnimation* pAnimation);

	const AIRString& GetName() const
	{
		return m_strName;
	}

	CAIRSkeletonAnimation* CreateSkeletonAnimation(const char* szName);

	void LoadAnimationConnects(const char* szFilename);
protected:
//private:
    void ComputeBoundingBox();
    //template class EngineExport vector<CAIRSubMesh*>;
    AIRMap<AIRString, CAIRSubMesh*>  m_mapSubMeshes;
    typedef AIRMap<AIRString, CAIRSubMesh*>::iterator MESH_ITER;
    //bool       m_bStatic;      //是否为静态模型
    bool       m_bLeftCoord;
    f32      m_fFileVersion;    //文件版本号
    //Vector3Df  m_upVec;     //向上的向量
    //Vector3Df  m_vCenter;   //模型中心点
    AABBBox3Df m_boundingBox;  //AABB

	AIRVector<OBBInf> m_obbinfs;

    u8  m_animationType;

    //CAIRSkeletonRes*    m_pSkeletalRes;    //影响该模型的骨骼
    AIRKeyframeAnimationRes*  m_pVertexAnimationRes;     //顶点动画资源
    AIRAnimationMixRes*       m_pAnimationMixRes;        //动作混合文件资源

	AIRVector<AIRBONE>     m_vtBoneHierarchy;
	AIRMap<AIRString, AIRString> m_mapAnimationNames;
	AIRMap<AIRString, CAIRSkeletonAnimation*> m_mapAnimation;

	AIRString m_strName;
};

