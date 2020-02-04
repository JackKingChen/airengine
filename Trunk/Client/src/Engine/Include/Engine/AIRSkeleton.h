/********************************************************************
	created:	2009/10/05
	created:	5:10:2009   12:02
	filename: 	d:\My Codes\3DGraphic\Engine\src\Scene\AIRSkeleton.h
	file path:	d:\My Codes\3DGraphic\Engine\src\Scene
	file base:	AIRSkeleton
	file ext:	h
	author:		liangairan
	
	purpose:	骨骼容器，实际上是动画里面每一关键帧的骨骼数据
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

    //清空骨骼
    void Clear();

    //void Create(AIRVector<AIRBONE>& vtBones, int nBonesNum);

    //void Create(int nBonesNum);

    CAIRBone* GetBone(int nIndex) const
    {
        return m_vtBones[ nIndex ];
    }

    //CAIRBone* GetBoneByName(const char* szName);

	//返回骨骼数量
	int GetBoneInstanceCount() const { return m_nBoneNum; }

    //void SetBoneData(int nBoneIndex, const AIRBONE& bone);

    //const Vector3Df& GetBoneTranslate(int nBoneID);

    //const Quaternion& GetBoneRotation(int nBoneID);

    void AddBone(CAIRBone* pBone);

    //计算所有骨的实际位置
    //void CaculateBones();

    //计算该帧的骨骼的关键帧相对上一关键帧的位置
    //void CaculateRelatedLastFrame(CAIRSkeleton* pLastFrame);

    CAIRSkeletonRes* GetSkeletonRes()
    {
        return m_pSkeletonRes;
    }
protected:
private:

    AIRVector<CAIRBone*> m_vtBones;
    //LPAIRBONE m_vtBones; //骨的数组
    int       m_nBoneNum;   //骨的数量

    CAIRSkeletonRes* m_pSkeletonRes;  
};
