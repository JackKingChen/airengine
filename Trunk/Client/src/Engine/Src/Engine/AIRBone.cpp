#include "stdafx.h"
#include "AIRBone.h"
//#include "AIRSkeleton.h"
//#include "AIRSkeletonRes.h"
#include "DebugTools.h"
#include "AIRSkeletonAnimation.h"

CAIRBone::CAIRBone(s32 nID, s32 nParentID, size_t nFrames, CAIRSkeletonAnimation* pAnimation) : m_nID(nID),
	m_nParentID(nParentID), m_pSkeletonAnimation(pAnimation)
{
	m_vtFrameDatas.resize(nFrames);
}

CAIRBone::~CAIRBone()
{

}

void CAIRBone::CaculateTranslation()
{
	/*
    if (m_nParentID == -1)
    {
        m_vTranslateAbsolute = m_vPosToParent;
        m_qRotationAbsolute = m_qRotationToParent;
        //m_vPosToParent = Vector3Df::ZERO;
        //m_qRotationToParent = Quaternion(0, 0, 0, 1);
        //return;
    }
	else
	{
        CAIRBone* pParent = m_pSkeleton->GetBone(m_nParentID);
        m_vTranslateAbsolute = m_vPosToParent;  //获得相对父亲的位置
        
		//这里要认真思考一下为什么要取反才能计算成功
        //m_vTranslateAbsolute = pParent->GetRotationAbsolute() * m_vTranslateAbsolute;  
        //m_vTranslateAbsolute += pParent->GetTranslateAbsolute();
		QuaternionMultiplyVector( pParent->GetRotationAbsolute(), m_vTranslateAbsolute, m_vTranslateAbsolute );
		m_vTranslateAbsolute.x += pParent->GetTranslateAbsolute().x;
		m_vTranslateAbsolute.y += pParent->GetTranslateAbsolute().y;
		m_vTranslateAbsolute.z += pParent->GetTranslateAbsolute().z;
           
        m_qRotationAbsolute = m_qRotationToParent;   
        //m_qRotationAbsolute = pParent->GetRotationAbsolute() * m_qRotationAbsolute;
		MultiplyQuaternion( pParent->GetRotationAbsolute(), m_qRotationAbsolute, m_qRotationAbsolute );

        //for test
        //Quaternion rotateTest = m_qRotationToParent.Inverse();
        //rotateTest = pParent->GetRotationAbsolute().Inverse() * rotateTest;
        //int a = 0;
        //-----
    }
    //m_qRotationAbsolute.Inverse();

    LPAIRBONE pCoreBone = m_pSkeleton->GetSkeletonRes()->GetBone(m_nID);
    m_vTranslateBS = pCoreBone->vPosition;

    //测试用
    //Quaternion qRotation2BS = m_qRotationAbsolute;
    //qRotation2BS.Inverse();
    //Vector3Df vTest = qTest * pCoreBone->vPosition;
    //------

    //m_vTranslateBS = m_qRotationAbsolute * m_vTranslateBS;
    //m_vTranslateBS += m_vTranslateAbsolute;
	QuaternionMultiplyVector( m_qRotationAbsolute, m_vTranslateBS, m_vTranslateBS );
	m_vTranslateBS.x += m_vTranslateAbsolute.x;
	m_vTranslateBS.y += m_vTranslateAbsolute.y;
	m_vTranslateBS.z += m_vTranslateAbsolute.z;
    
   
    m_qRotationBS = pCoreBone->qRotation;
	MultiplyQuaternion( m_qRotationAbsolute, m_qRotationBS, m_qRotationBS );
    //m_qRotationBS = m_qRotationAbsolute * m_qRotationBS;

    //char* pMsg = GetString("bone id:%d, translateBS(%f, %f, %f), rotationBS(%f, %f, %f, %f)\n", 
    //    m_nID,
    //    m_vTranslateBS.x, m_vTranslateBS.y, m_vTranslateBS.z, 
    //    m_qRotationBS.x, m_qRotationBS.y, m_qRotationBS.z, m_qRotationBS.w);
    //DT_TO_LOG(ML_COMMON, pMsg);

    //m_qRotationAbsolute.Inverse();
    //m_qRotationToParent.Inverse();
    //m_qRotationBS.Inverse();
	*/
}


void CAIRBone::AddFrameData(u32 nFrame, const Vector3Df& vPos2Parent, const Quaternion& qRotation2Parent)
{
	//LPAIRBONE pCoreBone = m_pSkeletonAnimation->GetSkeletonRes()->GetBone(m_nID);
	//m_strName = pCoreBone->szName;

	BoneFrameData frameData;
	frameData.m_qRotationToParent = qRotation2Parent;
	frameData.m_qRotationToParent.Inverse();
	frameData.m_vPosToParent = vPos2Parent;
	m_vtFrameDatas[nFrame] = frameData;
}
