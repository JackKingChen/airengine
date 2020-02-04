/********************************************************************
	created:	2009/10/05
	created:	5:10:2009   12:02
	filename: 	d:\My Codes\3DGraphic\Engine\src\Scene\AIRSkeleton.h
	file path:	d:\My Codes\3DGraphic\Engine\src\Scene
	file base:	AIRSkeleton
	file ext:	h
	author:		liangairan
	
	purpose:	����������ʵ�����Ƕ�������ÿһ�ؼ�֡�Ĺ�������
*********************************************************************/

#pragma once
#include "AIRBone.h"

class CAIRSkeletonRes;




class EngineExport CAIRSkeleton// : public EngineAllocator(CAIRSkeleton)
{
	EngineAllocator(CAIRSkeleton)
public:
    CAIRSkeleton();
    CAIRSkeleton(CAIRSkeletonRes* pSkeletonRes);
    //CAIRSkeleton()
    virtual ~CAIRSkeleton();

    //bool CreateFromFile(const char* szFilename);

    //virtual s32 Release();

    //��չ���
    void Clear();

    //void Create(AIRVector<AIRBONE>& vtBones, int nBonesNum);

    //void Create(int nBonesNum);

    CAIRBone* GetBone(int nIndex) const
    {
        return m_vtBones[ nIndex ];
    }

    //CAIRBone* GetBoneByName(const char* szName);

	//���ع�������
	int GetBoneInstanceCount() const { return m_nBoneNum; }

    //void SetBoneData(int nBoneIndex, const AIRBONE& bone);

    //const Vector3Df& GetBoneTranslate(int nBoneID);

    //const Quaternion& GetBoneRotation(int nBoneID);

    void AddBone(CAIRBone* pBone);

    //�������йǵ�ʵ��λ��
    //void CaculateBones();

    //�����֡�Ĺ����Ĺؼ�֡�����һ�ؼ�֡��λ��
    //void CaculateRelatedLastFrame(CAIRSkeleton* pLastFrame);

    CAIRSkeletonRes* GetSkeletonRes()
    {
        return m_pSkeletonRes;
    }
protected:
private:

    AIRVector<CAIRBone*> m_vtBones;
    //LPAIRBONE m_vtBones; //�ǵ�����
    int       m_nBoneNum;   //�ǵ�����

    CAIRSkeletonRes* m_pSkeletonRes;  
};
