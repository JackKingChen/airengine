#include "stdafx.h"
#include "AIRSkeletonAnimation.h"
#include "FileSystem.h"
//#include "AIRSkeletonRes.h"
//#include "AIRSkeleton.h"
#include <algorithm>
#include <functional>
#include "ModelInstance.h"
#include "DebugTools.h"

IMPLEMENT_RTTI(CAIRSkeletonAnimation, CAIRAnimation)

CAIRSkeletonAnimation::CAIRSkeletonAnimation(AIRModel* pModel) : m_pModelRes(pModel)
{

}

CAIRSkeletonAnimation::~CAIRSkeletonAnimation()
{
    Clear();
}

void CAIRSkeletonAnimation::Clear()
{
    /*for (int i = 0; i < (int)m_vtFrameSkeleton.size(); i++)
    {
		if (m_vtFrameSkeleton[i])
		{
			m_vtFrameSkeleton[i]->Clear();

		}
        SafeDelete(m_vtFrameSkeleton[i]);
    }*/

    m_vtKeyFrameTime.clear();

    //m_vtFrameSkeleton.clear();

	for (size_t i = 0; i < m_vtFrameBones.size(); ++i)
	{
		SafeDelete(m_vtFrameBones[i]);
	}

	m_vtFrameBones.clear();
}


bool CAIRSkeletonAnimation::LoadFromFileData(CDataStream* pFileStream, AIRModel* pBaseModel)
{
    //AIRString strFilename = FILESYSTEM->GetFileAbsolutePath(szFilename);

    //FILE* fp = fopen(strFilename.c_str(), "r");
    Clear();
	m_pModelRes = pBaseModel;

    //if (fp == NULL)
    //{
    //    return false;
    //}

    //char buff[512];
    //float fVersion = 0;
    AIRString lineBuff;
    int numBones = 0;
    int nBoneID = 0;
    
    int nFrameIndex = 0;

    FRAMEDATA frame;
	AIRVector<FRAMEDATA> vtFrameData;   //每一帧的骨数据
    vtFrameData.resize(pBaseModel->GetBoneInstanceCount());

    char buff[256] = { 0 };

    while (buff[0] != '}' && !pFileStream->Eof())
    {
        /* Read whole line */
        //fgets (buff, 512, fp);
        lineBuff = pFileStream->GetLine();
        strncpy(buff, lineBuff.c_str(), 256);

        if (sscanf(/*buff*/lineBuff.c_str(), " numFrames %d\n", &m_nFrames) == 1)
        {
			//m_vtFrameSkeleton.resize(m_nFrames);
            
        }
        else if (sscanf(/*buff*/lineBuff.c_str(), " frameRate %f\n", &m_fFrameRate) == 1)
        {
            
        }
        else if (sscanf(/*buff*/lineBuff.c_str(), " frame %d {\n", &nFrameIndex) == 1)
        {
            char frameBuf[256] = { 0 };
            //char c = frameBuf[0];
            AIRString frameLine;
            while(frameBuf[0] != '}' && !pFileStream->Eof())
            {
                //fgets(frameBuf, 512, fp);
                
                frameLine = pFileStream->GetLine();
                strncpy(frameBuf, frameLine.c_str(), 256);
                if (sscanf(frameLine.c_str(), " bone %d (%f %f %f) (%f %f %f %f)", &nBoneID, &frame.vPosition.x, &frame.vPosition.y, &frame.vPosition.z, 
                    &frame.qOrient.x, &frame.qOrient.y, &frame.qOrient.z, &frame.qOrient.w) == 8)
                {
                    //frame.qOrient.w = -frame.qOrient.w;
                    //frame.qOrient.Inverse();
					vtFrameData[nBoneID] = frame;
                    //pAniBone->SetFrameData(nFrameIndex, frame);
                }
                else if (sscanf(frameLine.c_str(), " }"))
                {
                    break;
                }
            }
            BuildFrameSkeleton(nFrameIndex, vtFrameData);
        }
        
    }

    m_fTimeRange = 1.0f / m_fFrameRate * m_nFrames;

    float fKeyFrameTime = 0.0f;
    for (size_t i = 0; i < m_nFrames; ++i)
    {
        m_vtKeyFrameTime.push_back(1.0f / m_fFrameRate * i);
    }

    //计算出所有帧的骨骼位置
    CaculateFrameData();

    return true;
}

bool CAIRSkeletonAnimation::LoadFromFileDataBinary(CDataStream* pStream, AIRModel* pBaseModel)
{
    Clear();
	m_pModelRes = pBaseModel;
    FRAMEDATA frame;
    AIRVector<FRAMEDATA> vtFrameData;   //每条骨的每一帧信息
    //vtFrameData.resize(pBaseSkeleton->GetBoneInstanceCount());

    char szName[256];
    pStream->Read(szName, sizeof(szName));

    if (pStream->Eof())
    {
        return false;
    }
    m_strName = szName;

    pStream->Read(&m_nFrames, sizeof(int));
    //m_vtFrameSkeleton.resize(m_nFrames);

    pStream->Read(&m_fFrameRate, sizeof(int));

    //size_t nSizeFrameData = sizeof(float) * 7;
    //size_t nTest = sizeof(FRAMEDATA);
	vtFrameData.resize(m_nFrames);

    for (size_t i = 0; i < pBaseModel->GetBoneInstanceCount(); ++i)
    {
        //vtFrameData.clear();
        pStream->Read(&vtFrameData[0], vtFrameData.size() * sizeof(FRAMEDATA));
        BuildFrameSkeleton( i, vtFrameData);
    }
    

    m_fTimeRange = 1.0f / m_fFrameRate * m_nFrames;

    float fKeyFrameTime = 0.0f;
    for (size_t i = 0; i < m_nFrames; ++i)
    {
        m_vtKeyFrameTime.push_back(1.0f / m_fFrameRate * i);
    }
    //计算出所有帧的骨骼位置
    CaculateFrameData();

    return true;
}

void CAIRSkeletonAnimation::SaveToFileStream(std::fstream& fout, AIRModel* pBaseModel) const
{
    FRAMEDATA frame;
    AIRVector<FRAMEDATA> vtFrameData;   //每一帧的骨数据
    vtFrameData.resize(pBaseModel->GetBoneInstanceCount());

    fout.write(m_strName.c_str(), 256);
    fout.write((const char*)&m_nFrames, sizeof(int));
    fout.write((const char*)&m_fFrameRate, sizeof(float));
	/*
    for (size_t i = 0; i < m_vtFrameSkeleton.size(); ++i)
    {
        CAIRSkeleton* pSkeleton = m_vtFrameSkeleton[i];
        for (size_t j = 0; j < vtFrameData.size(); ++j)
        {
            CAIRBone* pFrameBone = pSkeleton->GetBone(j);
            vtFrameData[j].vPosition = pFrameBone->GetTranslate2Parent();
            vtFrameData[j].qOrient = pFrameBone->GetRotation2Parent();
            vtFrameData[j].qOrient.Inverse();   //左手坐标，所以要取反
        }

        fout.write((const char*)&vtFrameData[0], sizeof(FRAMEDATA) * vtFrameData.size());
    }
	*/
}

void CAIRSkeletonAnimation::BuildFrameSkeleton(int nBoneIndex, AIRVector<FRAMEDATA>& vtFrameData)
{
    size_t nBonesNum = m_pModelRes->GetBoneInstanceCount();
  
	//{
 //   CAIRSkeleton* pSkeleton = new CAIRSkeleton(pBaseSkeleton);
 //   //pSkeleton->Create(pBaseSkeleton->GetBoneInstanceCount());

	//LPAIRBONE pBone = NULL;
	//CAIRBone* pFrameBone = NULL;
 //   for (size_t i = 0; i < nBonesNum; i++)
 //   {
 //       pBone = pBaseSkeleton->GetBone(i);
 //       pFrameBone = new CAIRBone(i, pBone->nParentID, vtFrameData[i].vPosition, vtFrameData[i].qOrient, pSkeleton);
 //       pSkeleton->AddBone(pFrameBone);
 //   }

 //   m_vtFrameSkeleton[nFrame] = pSkeleton;
	//}

	if (m_vtFrameBones.empty())
	{
		LPAIRBONE pBone = NULL;

		for (size_t i = 0; i < nBonesNum; ++i)
		{
			pBone = m_pModelRes->GetBone(i);
			CAIRBone* pFrameBone = new CAIRBone(i, pBone->nParentID, vtFrameData.size(), this);
			//pFrameBone->AddFrameData(nFrame, vtFrameData[i].vPosition, vtFrameData[i].qOrient);
			m_vtFrameBones.push_back(pFrameBone);
		}
		
	}

	for (size_t i = 0; i < vtFrameData.size(); ++i)
	{
		m_vtFrameBones[nBoneIndex]->AddFrameData(i, vtFrameData[i].vPosition, vtFrameData[i].qOrient);
	}
	
	m_nFrames = vtFrameData.size();
	/*
	else
	{
		for (size_t i = 0; i < nBonesNum; ++i)
		{
			CAIRBone* pFrameBone = m_vtFrameBones[i];
			pFrameBone->AddFrameData(nFrame, vtFrameData[i].vPosition, vtFrameData[i].qOrient);
		}
		
	}
	*/
}



void CAIRSkeletonAnimation::CaculateFrameData()
{
    //DT_ADD_LOG("skeleton animation", "D:/log.txt");
    //if (m_strName == "idle")
    //{
    //    DT_TO_LOG(ML_COMMON, GetString("Animation:%s\n", m_strName.c_str()));
    //}
/*
	CAIRSkeleton* pSkeleton = NULL;

	u32 dwFrameCount = m_vtFrameSkeleton.size();
	
    for (u32 i = 0; i < dwFrameCount; i++)
    {
        m_vtFrameSkeleton[i]->CaculateBones();
    }
	*/

    //计算骨骼中相对上一帧的位置
	/*
    for (u32 i = 0; i < dwFrameCount; i++)
    {
        //pSkeleton = m_vtFrameSkeleton[i];
        if (i == 0)
        {
            m_vtFrameSkeleton[i]->CaculateRelatedLastFrame(NULL);
        }
        else
		{
            m_vtFrameSkeleton[i]->CaculateRelatedLastFrame(m_vtFrameSkeleton[i - 1]);
		}
    }
	*/
}

void CAIRSkeletonAnimation::SetTimeRange(float fTime)
{
    m_fTimeRange = fTime;
    m_fFrameRate = m_vtKeyFrameTime.size() / fTime;

    //m_vtKeyFrameTime.clear();
    //float fKeyFrameTime = 0.0f;
    //for (int i = 0; i < m_vtFrameSkeleton.size(); ++i)
    //{
    //    m_vtKeyFrameTime.push_back(1.0f / m_fFrameRate * i);
    //}
}

void CAIRSkeletonAnimation::SetFrameRate(float fFrameRate)
{
    m_fFrameRate = fFrameRate;
	float fDeltaFrameTime = 1.0f / m_fFrameRate;
	m_fTimeRange = 1.0f / m_fFrameRate * m_nFrames;
    //m_fTimeRange = fDeltaFrameTime * m_vtKeyFrameTime.size();
    
	//u32 dwFrameCount = m_vtKeyFrameTime.size();
	m_vtKeyFrameTime.clear();
    for (size_t i = 0; i < m_nFrames; ++i)
    {
        m_vtKeyFrameTime.push_back( fDeltaFrameTime * i );
    }
}
/*
void CAIRSkeletonAnimation::AddFrameSkeleton(CAIRSkeleton* pFrameSkel)
{
    m_vtFrameSkeleton.push_back(pFrameSkel);
}
*/
void CAIRSkeletonAnimation::RemoveAllFrames(bool bDelete /* = false */)
{
    if (bDelete)
    {
        Clear();
    }
    else
    {
        //m_vtFrameSkeleton.clear();
        m_vtKeyFrameTime.clear();
		//m_vtFrameBones.clear();
    }
}

bool CAIRSkeletonAnimation::Cache()
{
	if (!m_bReady)
	{
		if (m_pStream)
		{
			char szName[256] = { 0 };
			m_pStream->Read(szName, sizeof(szName));
			m_strName = szName;
			s32 nFrames = 0;
			m_pStream->Read(&nFrames, sizeof(nFrames));
			m_nFrames = nFrames;
			m_pStream->Read(&m_fFrameRate, sizeof(float));

			u32 nBonesNum = 0;
			m_pStream->Read(&nBonesNum, sizeof(nBonesNum));

			FRAMEDATA frame;
			AIRVector<FRAMEDATA> vtFrameData;
			vtFrameData.resize(m_nFrames);

			for (size_t i = 0; i < nBonesNum; ++i)
			{
				//vtFrameData.clear();
				m_pStream->Read(&vtFrameData[0], vtFrameData.size() * sizeof(FRAMEDATA));
				BuildFrameSkeleton(i, vtFrameData);
			}


			m_fTimeRange = 1.0f / m_fFrameRate * m_nFrames;

			float fKeyFrameTime = 0.0f;
			for (size_t i = 0; i < m_nFrames; ++i)
			{
				m_vtKeyFrameTime.push_back(1.0f / m_fFrameRate * i);
			}
			//计算出所有帧的骨骼位置
			CaculateFrameData();

			SafeDelete(m_pStream);
			m_bReady = true;
		}
	}

	return true;
}

float CAIRSkeletonAnimation::GetKeyFramesAtTime(f32 fTimePos, u32& nFrame1, u32& nFrame2)
{
	AIRVector<float>::iterator it = std::lower_bound(m_vtKeyFrameTime.begin(), m_vtKeyFrameTime.end(), fTimePos);

	float t1 = 0.0f;
	float t2 = 0.0f;

	if (it == m_vtKeyFrameTime.end())
	{
		//it = m_vtKeyFrameTime.front();
		nFrame1 = m_vtKeyFrameTime.size() - 1;
		nFrame2 = 0;
		t2 = GetTimeRange();
	}
	else
	{
		if (fTimePos == *it)
		{
			nFrame1 = std::distance(m_vtKeyFrameTime.begin(), it);
		}
		else
		{
			nFrame1 = std::distance(m_vtKeyFrameTime.begin(), --it);
		}

		if (nFrame1 == m_vtKeyFrameTime.size() - 1)
		{
			nFrame2 = 0;
		}
		else
			nFrame2 = nFrame1 + 1;

		t2 = m_vtKeyFrameTime[nFrame2];
	}
	t1 = m_vtKeyFrameTime[nFrame1];

	return (fTimePos - t1) / (t2 - t1);
}

void CAIRSkeletonAnimation::AddKeyFrameTime(float fTime)
{
    m_vtKeyFrameTime.push_back(fTime);
}

void CAIRSkeletonAnimation::Apply(CModelInstance* pModelInst, float fTimePos, float fWeight)
{
	u32 dwBoneInstCount = pModelInst->GetBoneInstanceCount();
    for (size_t i = 0; i < dwBoneInstCount; ++i)
    {
        //CAIRBoneInstance* pBone = pModelInst->GetBoneInst(i);
        //ApplyToNode(pBone, i, fTimePos, fWeight);
		ApplyToNode( pModelInst->GetBoneInst(i), i, fTimePos, fWeight );
    }
}

void CAIRSkeletonAnimation::GetInterpolatedKeyFrame(float fTimePos, int nBoneIndex, CKeyFrame& keyFrame)
{
	/*
    CAIRSkeleton* pCurKeyFrame;
    CAIRSkeleton* pNextKeyFrame;

    float fInterpolate = GetKeyFramesAtTime(fTimePos, &pCurKeyFrame, &pNextKeyFrame);

    //获得该骨在该动画中的当前帧的数据
    

	CAIRBone* pKeyFrameBone1 = pCurKeyFrame->GetBone(nBoneIndex);
    //获得该骨在该动画中的下一帧的数据
    CAIRBone* pKeyFrameBone2 = pNextKeyFrame->GetBone(nBoneIndex);

    if (fInterpolate == 0.0)
    {
        keyFrame.m_translate = pKeyFrameBone1->m_vPosToParent;
        keyFrame.m_rotation = pKeyFrameBone1->m_qRotationToParent;
        //keyFrame.m_translate = pKeyFrameBone1->m_posToLastFrame;
        //keyFrame.m_rotation = pKeyFrameBone1->m_rotateToLastFrame;
    }
    else
    {
        
        keyFrame.m_translate.x = pKeyFrameBone1->m_vPosToParent.x + fInterpolate * (pKeyFrameBone2->m_vPosToParent.x - pKeyFrameBone1->m_vPosToParent.x);
        keyFrame.m_translate.y = pKeyFrameBone1->m_vPosToParent.y + fInterpolate * (pKeyFrameBone2->m_vPosToParent.y - pKeyFrameBone1->m_vPosToParent.y);
        keyFrame.m_translate.z = pKeyFrameBone1->m_vPosToParent.z + fInterpolate * (pKeyFrameBone2->m_vPosToParent.z - pKeyFrameBone1->m_vPosToParent.z);

        //keyFrame.m_rotation.Slerp(pKeyFrameBone1->m_qRotationToParent, pKeyFrameBone2->m_qRotationToParent, fInterpolate);
        keyFrame.m_rotation = Quaternion::Slerp(fInterpolate, pKeyFrameBone1->m_qRotationToParent, pKeyFrameBone2->m_qRotationToParent, true);

    }
	*/

	u32 nFrame1 = 0;
	u32 nFrame2 = 0;

	f32 fInterpolate = GetKeyFramesAtTime(fTimePos, nFrame1, nFrame2);

	CAIRBone* pKeyFrameBone = m_vtFrameBones[nBoneIndex];

	if (fInterpolate == 0.0)
	{
		const CAIRBone::BoneFrameData* pFrameData = pKeyFrameBone->GetFrameData(nFrame1);
		keyFrame.m_translate = pFrameData->m_vPosToParent;
		keyFrame.m_rotation = pFrameData->m_qRotationToParent;
		//keyFrame.m_translate = pKeyFrameBone1->m_posToLastFrame;
		//keyFrame.m_rotation = pKeyFrameBone1->m_rotateToLastFrame;
	}
	else
	{
		const CAIRBone::BoneFrameData* pFrameData1 = pKeyFrameBone->GetFrameData(nFrame1);
		const CAIRBone::BoneFrameData* pFrameData2 = pKeyFrameBone->GetFrameData(nFrame2);
		keyFrame.m_translate.x = pFrameData1->m_vPosToParent.x + fInterpolate * (pFrameData2->m_vPosToParent.x - pFrameData1->m_vPosToParent.x);
		keyFrame.m_translate.y = pFrameData1->m_vPosToParent.y + fInterpolate * (pFrameData2->m_vPosToParent.y - pFrameData1->m_vPosToParent.y);
		keyFrame.m_translate.z = pFrameData1->m_vPosToParent.z + fInterpolate * (pFrameData2->m_vPosToParent.z - pFrameData1->m_vPosToParent.z);

		//keyFrame.m_rotation.Slerp(pKeyFrameBone1->m_qRotationToParent, pKeyFrameBone2->m_qRotationToParent, fInterpolate);
		keyFrame.m_rotation = Quaternion::Slerp(fInterpolate, pFrameData1->m_qRotationToParent, pFrameData2->m_qRotationToParent, true);

	}
}

void CAIRSkeletonAnimation::ApplyToNode(CSceneNode* pNode, int nBoneIndex, float fTimePos, float fWeight)
{
    //if (!fWeight)
    //{
    //    return;
    //}
    //首先获得插值后的关键帧数据kf
    GetInterpolatedKeyFrame(fTimePos, nBoneIndex, m_keyFrameTransform);

    //权重X kf的所有值
    if (fWeight != 1.0f)
    {
        m_keyFrameTransform.m_translate.x *= fWeight;
		m_keyFrameTransform.m_translate.y *= fWeight;
		m_keyFrameTransform.m_translate.z *= fWeight;
        //static Quaternion rotationIdentity;
        //m_keyFrameTransform.m_rotation.Slerp(rotationIdentity, m_keyFrameTransform.m_rotation, fWeight);
    }

    //改变pNode的数据
    pNode->SetPosition(m_keyFrameTransform.m_translate);
    pNode->SetRotation(m_keyFrameTransform.m_rotation);

    //pNode->Rotate(m_keyFrameTransform.m_rotation);
    //pNode->Translate(m_keyFrameTransform.m_translate);
    
}

void CAIRSkeletonAnimation::Blend(CModelInstance* pModelInst, float fTimePos, float fWeight)
{
    //OutputDebugString(GetString("blending! timepos:%f\n", fTimePos));
	u32 dwBoneInstCount = pModelInst->GetBoneInstanceCount();
    for (size_t i = 0; i < dwBoneInstCount; ++i)
    {
        //CAIRBoneInstance* pBone = pModelInst->GetBoneInst(i);
        //BlendToNode(pBone, i, fTimePos, fWeight);
		BlendToNode(pModelInst->GetBoneInst(i), i, fTimePos, fWeight);
    }
}

void CAIRSkeletonAnimation::BlendToNode(CSceneNode* pNode, int nBoneIndex, float fTimePos, float fWeight)
{
    //首先获得插值后的关键帧数据kf
    GetInterpolatedKeyFrame(fTimePos, nBoneIndex, m_keyFrameTransform);

    //权重X kf的所有值
    if (fWeight != 1.0f)
    {
        m_keyFrameTransform.m_translate.x *= fWeight;
		m_keyFrameTransform.m_translate.y *= fWeight;
		m_keyFrameTransform.m_translate.z *= fWeight;
        //static Quaternion rotationIdentity;
        //m_keyFrameTransform.m_rotation.Slerp(rotationIdentity, m_keyFrameTransform.m_rotation, fWeight);
    }

    const Quaternion& rotateInNode = pNode->GetRotation();
    Quaternion rotate = Quaternion::Slerp(fWeight, rotateInNode, m_keyFrameTransform.m_rotation, true);
    //rotate.Slerp(rotateInNode, m_keyFrameTransform.m_rotation, fWeight);

    //改变pNode的数据
    pNode->Translate(m_keyFrameTransform.m_translate);
    pNode->SetRotation(rotate);
    
}
