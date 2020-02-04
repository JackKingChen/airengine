/********************************************************************
	created:	2009/10/13
	created:	13:10:2009   16:06
	filename: 	d:\My Codes\3DGraphic\Engine\src\Scene\AIRBone.h
	file path:	d:\My Codes\3DGraphic\Engine\src\Scene
	file base:	AIRBone
	file ext:	h
	author:		liangairan
	
	purpose:	代表骨骼中的一条骨，实际上是关键帧中的信息
*********************************************************************/
#pragma once

#include "Common.h"
#include "Quaternion.h"
#include "EngineConfig.h"
#include "MemoryAlloter.h"

class CAIRSkeletonAnimation;

class EngineExport CAIRBone// : public EngineAllocator(CAIRBone)
{
	EngineAllocator(CAIRBone)
public:
	struct BoneFrameData
	{
		//如果骨骼没有父亲，则该位置就是模型空间中的位置
		Vector3Df  m_vPosToParent;        

		//相对父亲的旋转，如果没有父亲，则是模型空间中的绝对旋转
		Quaternion m_qRotationToParent;
	};
public:
    //CAIRBone(int nID, int nParentID, const Vector3Df& vPos2Parent, const Quaternion& qRotation2Parent, CAIRSkeleton* pSkeleton);
	CAIRBone(s32 nID, s32 nParentID, size_t nFrames, CAIRSkeletonAnimation* pAnimation);
    virtual ~CAIRBone();

    //LPAIRBONE GetCoreBonePtr() { return &m_coreBone; }

    void CaculateTranslation();

    //获得相对模型空间的变换位置
    /*const Vector3Df& GetTranslateBoneSpace() const
    {
        return m_vTranslateBS;
    }

	void GetTranslateBoneSpace( Vector3Df& vOut ) const
	{
		vOut.x = m_vTranslateBS.x;
		vOut.y = m_vTranslateBS.y;
		vOut.z = m_vTranslateBS.z;
	}

    //获得相对模型空间的旋转
    const Quaternion& GetRotationBoneSpace() const
    {
        return m_qRotationBS;
    }

	void GetRotationBoneSpace( Quaternion& qOut ) const
	{
		qOut.x = m_qRotationBS.x;
		qOut.y = m_qRotationBS.y;
		qOut.z = m_qRotationBS.z;
		qOut.w = m_qRotationBS.w;
	}*/

    const Vector3Df& GetTranslate2Parent() const
    {
        return m_vPosToParent;
    }

    const Quaternion& GetRotation2Parent() const
    {
        return m_qRotationToParent;
    }

    /*const Vector3Df& GetTranslateAbsolute() const
    {
        return m_vTranslateAbsolute;
    }

    const Quaternion& GetRotationAbsolute() const
    {
        return m_qRotationAbsolute;
    }*/

    //计算出相对上一关键帧的位置
    //void CaculateRelatedToLastFrame(CAIRBone* pLastFrameBone);

	void AddFrameData(u32 nFrame, const Vector3Df& vPos2Parent, const Quaternion& qRotation2Parent);

	const BoneFrameData* GetFrameData(u32 nFrame) const
	{
		if (nFrame < m_vtFrameDatas.size())
		{
			return &m_vtFrameDatas[nFrame];
		}

		return NULL;
	}
protected:
public:
    int        m_nID;         //自己的ID
    int        m_nParentID;   //父骨骼的ID
    //Vector3Df  m_vTranslateBS;  //Bone Space的平移
    //Quaternion m_qRotationBS;   //Bone Space的旋转

    //Vector3Df  m_vTranslateAbsolute;   //绝对位移，实际上是相对于模型空间的位移
    //Quaternion m_qRotationAbsolute;    //绝对旋转
    //AIRBONE    m_coreBone;   //从文件读入的数据

    //相对父亲的位置 在max中是由GetNodeTM * Inverse(GetParentTM)求得的
    //如果骨骼没有父亲，则该位置就是模型空间中的位置
    Vector3Df  m_vPosToParent;        

    //相对父亲的旋转，如果没有父亲，则是模型空间中的绝对旋转
    Quaternion m_qRotationToParent;   

    //Vector3Df  m_posToLastFrame;     //该骨头相对于上一关键帧的位置
    //Quaternion  m_rotateToLastFrame;  //该骨头相对于上一关键帧的旋转

    //AIRString  m_strName;       //骨骼名字

    //CAIRSkeleton*  m_pSkeleton;   //它所在的某一帧的骨骼

	AIRVector<BoneFrameData> m_vtFrameDatas;
	CAIRSkeletonAnimation* m_pSkeletonAnimation;
};
