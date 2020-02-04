/********************************************************************
	created:	2010/01/27
	created:	27:1:2010   16:39
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\AIRModel.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	AIRModel
	file ext:	h
	author:		liangairan
	
	purpose:	ģ����Դ������һ��ģ���ļ�
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

//����ԭʼ���ݣ���δ�������仯ǰ������
typedef struct tagAirBone 
{
	tagAirBone()
	{
		memset(szName, 0, 256);
	}
	char        szName[256];
	int         nParentID;   //������ID
	Vector3Df   vPosition;   //���ڳ�ʼ֡���ģ�Ϳռ��ԭ����Թǵ�λ�� ��������MAX�е�Inverse(GetNodeTM)��õ�
	Quaternion  qRotation;   //ģ�Ϳռ��ԭ����Թǵ���ת ��������MAX�е�Inverse(GetNodeTM)��õ�
	//Vector3Df   positionAbsolute;    //�����ڳ�ʼ�еľ���λ��
	//Quaternion  rotationAbsolute;    //�����ڳ�ʼ�еľ�����ת
	//Vector3Df   vPosToParent;  //����ڸ��׵�ƽ��
	//Quaternion  qRotationToParent;  //����ڸ��׵���ת
	AIRString strName;     //��������
}AIRBONE, *LPAIRBONE;

class EngineExport AIRModel : public Mesh
{
	DECLARE_RTTI;
public:
    AIRModel();
    virtual ~AIRModel();

    bool CreateFromFile(const char* szFilename);

    //�Ӷ������ļ�����
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

    //�Ƿ�̬ģ��
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
    //bool       m_bStatic;      //�Ƿ�Ϊ��̬ģ��
    bool       m_bLeftCoord;
    f32      m_fFileVersion;    //�ļ��汾��
    //Vector3Df  m_upVec;     //���ϵ�����
    //Vector3Df  m_vCenter;   //ģ�����ĵ�
    AABBBox3Df m_boundingBox;  //AABB

	AIRVector<OBBInf> m_obbinfs;

    u8  m_animationType;

    //CAIRSkeletonRes*    m_pSkeletalRes;    //Ӱ���ģ�͵Ĺ���
    AIRKeyframeAnimationRes*  m_pVertexAnimationRes;     //���㶯����Դ
    AIRAnimationMixRes*       m_pAnimationMixRes;        //��������ļ���Դ

	AIRVector<AIRBONE>     m_vtBoneHierarchy;
	AIRMap<AIRString, AIRString> m_mapAnimationNames;
	AIRMap<AIRString, CAIRSkeletonAnimation*> m_mapAnimation;

	AIRString m_strName;
};

