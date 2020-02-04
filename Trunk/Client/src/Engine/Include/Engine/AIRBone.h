/********************************************************************
	created:	2009/10/13
	created:	13:10:2009   16:06
	filename: 	d:\My Codes\3DGraphic\Engine\src\Scene\AIRBone.h
	file path:	d:\My Codes\3DGraphic\Engine\src\Scene
	file base:	AIRBone
	file ext:	h
	author:		liangairan
	
	purpose:	��������е�һ���ǣ�ʵ�����ǹؼ�֡�е���Ϣ
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
		//�������û�и��ף����λ�þ���ģ�Ϳռ��е�λ��
		Vector3Df  m_vPosToParent;        

		//��Ը��׵���ת�����û�и��ף�����ģ�Ϳռ��еľ�����ת
		Quaternion m_qRotationToParent;
	};
public:
    //CAIRBone(int nID, int nParentID, const Vector3Df& vPos2Parent, const Quaternion& qRotation2Parent, CAIRSkeleton* pSkeleton);
	CAIRBone(s32 nID, s32 nParentID, size_t nFrames, CAIRSkeletonAnimation* pAnimation);
    virtual ~CAIRBone();

    //LPAIRBONE GetCoreBonePtr() { return &m_coreBone; }

    void CaculateTranslation();

    //������ģ�Ϳռ�ı任λ��
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

    //������ģ�Ϳռ����ת
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

    //����������һ�ؼ�֡��λ��
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
    int        m_nID;         //�Լ���ID
    int        m_nParentID;   //��������ID
    //Vector3Df  m_vTranslateBS;  //Bone Space��ƽ��
    //Quaternion m_qRotationBS;   //Bone Space����ת

    //Vector3Df  m_vTranslateAbsolute;   //����λ�ƣ�ʵ�����������ģ�Ϳռ��λ��
    //Quaternion m_qRotationAbsolute;    //������ת
    //AIRBONE    m_coreBone;   //���ļ����������

    //��Ը��׵�λ�� ��max������GetNodeTM * Inverse(GetParentTM)��õ�
    //�������û�и��ף����λ�þ���ģ�Ϳռ��е�λ��
    Vector3Df  m_vPosToParent;        

    //��Ը��׵���ת�����û�и��ף�����ģ�Ϳռ��еľ�����ת
    Quaternion m_qRotationToParent;   

    //Vector3Df  m_posToLastFrame;     //�ù�ͷ�������һ�ؼ�֡��λ��
    //Quaternion  m_rotateToLastFrame;  //�ù�ͷ�������һ�ؼ�֡����ת

    //AIRString  m_strName;       //��������

    //CAIRSkeleton*  m_pSkeleton;   //�����ڵ�ĳһ֡�Ĺ���

	AIRVector<BoneFrameData> m_vtFrameDatas;
	CAIRSkeletonAnimation* m_pSkeletonAnimation;
};
