#include "stdafx.h"
#include "VertexAnimation.h"
#include "AIRSubMesh.h"
#include "ModelInstance.h"
#include "MemoryAllocatorConfig.h"


VertexAnimationKeyframe::VertexAnimationKeyframe()
{

}

VertexAnimationKeyframe::~VertexAnimationKeyframe()
{
    m_vtVertexKeyframePositions.clear();
    m_vtVertexKeyframeUVs.clear();
    m_vtVertexKeyframeColors.clear();
}

/*
void VertexAnimationKeyframe::AddKeyframeData(const AIRString& strName, const VertexKeyframeData& data)
{
    VertexKeyframeDataArrayMap_Iter it = m_mapMeshKeyframes.find(strName);
    if (it == m_mapMeshKeyframes.end())
    {
        VertexKeyframeDataArray array;
        m_mapMeshKeyframes.insert(std::make_pair(strName, array));
    }
    it = m_mapMeshKeyframes.find(strName);
    (it->second).push_back(data);
}

bool VertexAnimationKeyframe::LoadFromFileDataBinary(CDataStream* pStream)
{
    int nMeshNum = 0;
    int nVertexNum = 0;
    char szMeshName[256] = { 0 };
    pStream->Read(&nMeshNum, sizeof(int));
    for (int i = 0; i < nMeshNum; ++i)
    {
        pStream->Read(szMeshName, sizeof(szMeshName));
        pStream->Read(&nVertexNum, sizeof(int));
        if (nVertexNum > 0)
        {
            VertexKeyframeDataArray vertexArray;
            vertexArray.resize(nVertexNum);
            pStream->Read(&vertexArray[0], sizeof(VertexKeyframeData) * nVertexNum);
            m_mapMeshKeyframes.insert(std::make_pair(szMeshName, vertexArray));
        }
        
    }

    return true;
}
*/

MeshVertexAnimation::MeshVertexAnimation(VertexAnimation* pAnimation) : m_pVertexAnimation(pAnimation)
{
    
}

MeshVertexAnimation::~MeshVertexAnimation()
{
    size_t keyFramesNum = m_vtKeyframes.size();
    for (size_t i = 0; i < keyFramesNum; ++i)
    {
        SafeDelete(m_vtKeyframes[i]);
    }
    m_vtKeyframes.clear();
    m_vtAllVertices.clear();
    m_vtAllTexcoords.clear();
}

bool MeshVertexAnimation::LoadFromFileDataBinary(CDataStream* pFileStream)
{
    char szName[256] = { 0 };
    pFileStream->Read(szName, sizeof(szName));
    m_strName = szName;

    u32 nRealVerticesNum = 0;
    pFileStream->Read(&nRealVerticesNum, sizeof(u32));

    u32 nVertexNum = 0;
    pFileStream->Read(&nVertexNum, sizeof(u32));
    if (nVertexNum)
    {
        m_vtAllVertices.resize(nVertexNum);
        pFileStream->Read(&m_vtAllVertices[0], sizeof(Vector3Df) * nVertexNum);
    }

    u32 nTexcoordsNum = 0;
    pFileStream->Read(&nTexcoordsNum, sizeof(u32));
    if (nTexcoordsNum)
    {
        m_vtAllTexcoords.resize(nTexcoordsNum);
        pFileStream->Read(&m_vtAllTexcoords[0], sizeof(Vector2Df) * nTexcoordsNum);
    }
    

    VertexInfo* pVertexInfo = AIR_NEW_ARRAY_T(VertexInfo, nRealVerticesNum);

    for (int i = 0; i < m_pVertexAnimation->GetFramesNum(); ++i)
    {
        pFileStream->Read(pVertexInfo, nRealVerticesNum * sizeof(VertexInfo));
        VertexAnimationKeyframe* pKeyframe = new VertexAnimationKeyframe;
        //pKeyframe->m_vtVertexKeyframeDatas.reserve(nRealVerticesNum);

        if (nVertexNum)
        {
            pKeyframe->m_vtVertexKeyframePositions.reserve(nRealVerticesNum);
            for (size_t v = 0; v < nRealVerticesNum; ++v)
            {
                pKeyframe->m_vtVertexKeyframePositions.push_back(m_vtAllVertices[pVertexInfo[v].nPositionIndex]);
            }
        }

        if (nTexcoordsNum)
        {
            pKeyframe->m_vtVertexKeyframeUVs.reserve(nRealVerticesNum);
            for (size_t v = 0; v < nRealVerticesNum; ++v)
            {
                pKeyframe->m_vtVertexKeyframeUVs.push_back(m_vtAllTexcoords[pVertexInfo[v].nTexcoordIndex]);
            }
        }

        if (m_pVertexAnimation->GetVertexAnimationMask() & eVAM_Color)
        {
            for (size_t v = 0; v < nRealVerticesNum; ++v)
            {
                pKeyframe->m_vtVertexKeyframeColors.push_back(pVertexInfo[v].color);
            }
        }
        

        m_vtKeyframes.push_back(pKeyframe);
    }
    
    AIR_DELETE_ARRAY_T(pVertexInfo, VertexInfo, nRealVerticesNum);

    return true;
}


VertexAnimation::VertexAnimation() : m_nVertexAnimationMask(0)
{

}

VertexAnimation::~VertexAnimation()
{

}

void VertexAnimation::Apply(CModelInstance* pModelInst, float fTimePos, float fWeight)
{
    VertexAnimationKeyframe* pKeyframe1, *pKeyframe2;

    for (size_t i = 0; i < pModelInst->GetSubMeshCount(); ++i)
    {
        CAIRSubMesh* pSubmesh = pModelInst->GetSubMesh(i);
        float fInterpolate = GetKeyFramesAtTime(pSubmesh, fTimePos, &pKeyframe1, &pKeyframe2);
        pSubmesh->CaculateVerticesByVertexAnimation(fInterpolate, pKeyframe1,
            pKeyframe2, m_nVertexAnimationMask);
    }
}

void VertexAnimation::Blend(CModelInstance* pModelInst, float fTimePos, float fWeight)
{

}

void VertexAnimation::Clear()
{
    /*
    for (size_t i = 0; i < m_vtKeyframes.size(); ++i)
    {
        SafeDelete(m_vtKeyframes[i]);
    }
    m_vtKeyframes.clear();
    */
    std::map<AIRString, MeshVertexAnimation*>::iterator itEnd = m_mapMeshVertexAnimations.end();
    for (std::map<AIRString, MeshVertexAnimation*>::iterator it = m_mapMeshVertexAnimations.begin(); it != itEnd; ++it)
    {
        SafeDelete(it->second);
    }
    m_mapMeshVertexAnimations.clear();
}

bool VertexAnimation::LoadFromFileData(CDataStream* pFileStream)
{
    Clear();

    //if (fp == NULL)
    //{
    //    return false;
    //}

    //char buff[512];
    //float fVersion = 0;
    AIRString lineBuff;

    char szTemp[512];

    int nFrameIndex = 0;


    char buff[256] = { 0 };

    float fdata[4];
    float uv[2];

    int  nVertexIndex = 0;
    int  weight_index = 0;
    int  nWeightCount = 0;

    AIRString strName;

    while (buff[0] != '}' && !pFileStream->Eof())
    {
        /* Read whole line */
        //fgets (buff, 512, fp);
        lineBuff = pFileStream->GetLine();
        strncpy(buff, lineBuff.c_str(), 256);

        if (sscanf(/*buff*/lineBuff.c_str(), " frames %d\n", &m_nFrames) == 1)
        {
            //m_vtFrameSkeleton.resize(m_nFrames);

        }
        else if (sscanf(/*buff*/lineBuff.c_str(), " frameRate %f\n", &m_fFrameRate) == 1)
        {

        }
        else if (sscanf(/*buff*/lineBuff.c_str(), " frame %d {\n", &nFrameIndex) == 1)
        {
            char frameBuf[256] = { 0 };
            char frameBuf1[256] = { 0 };
            //char c = frameBuf[0];
            AIRString frameLine;

            VertexAnimationKeyframe* pKeyframe = new VertexAnimationKeyframe;
            while(frameBuf[0] != '}' && !pFileStream->Eof())
            {
                //fgets(frameBuf, 512, fp);
                frameLine = pFileStream->GetLine();
                //int nNum = pFileStream->GetLine()
                strncpy(frameBuf, frameLine.c_str(), 256);
                if (strncmp (frameLine.c_str(), "mesh {", 6) == 0)
                {
                    while(frameBuf1[0] != '}' && !pFileStream->Eof())
                    {
                        frameLine = pFileStream->GetLine();
                        strncpy(frameBuf1, frameLine.c_str(), 256);
                        if (sscanf(frameLine.c_str(), "name %s", szTemp) == 1)
                        {
                            strName = szTemp;
                        }
                        else if (sscanf(frameLine.c_str(), " vertex %d position (%f, %f, %f) tex_coordinate (%f, %f) %d %d", &nVertexIndex,
                            &fdata[0], &fdata[1], &fdata[2], &uv[0], &uv[1], &weight_index, &nWeightCount) == 8)
                        {
                            //VertexKeyframeData data;
                            //memcpy(&data.position, fdata, sizeof(Vector3Df));
                            //data.tu = uv[0];
                            //data.tv = uv[1];
                            //pKeyframe->AddKeyframeData(strName, data);
                        }
                        else if (strcmp(frameLine.c_str(), "}") == 0)
                        {
                            break;
                        }

                        if (nVertexIndex == 120)
                        {
                            int a = 0;
                        }
                    }
                }
                else if (strcmp(frameLine.c_str(), " }") == 0)
                {
                    break;
                }
            }

            //m_vtKeyframes.push_back(pKeyframe);
            //BuildFrameSkeleton(pBaseSkeleton, nFrameIndex, vtFrameData);
        }

    }

    m_fTimeRange = 1.0f / m_fFrameRate * m_nFrames;

    float fKeyFrameTime = 0.0f;
    for (int i = 0; i < m_nFrames; ++i)
    {
        m_vtKeyFrameTime.push_back(1.0f / m_fFrameRate * i);
    }

    //计算出所有帧的骨骼位置
    //CaculateFrameData();
    return true;
}

bool VertexAnimation::LoadFromFileDataBinary(CDataStream* pFileStream)
{
    char szName[256] = { 0 };
    pFileStream->Read(szName, sizeof(szName));

    m_strName = szName;

    if (pFileStream->Eof())
    {
        return false;
    }
    m_strName = szName;

    pFileStream->Read(&m_nFrames, sizeof(u16));

    pFileStream->Read(&m_fFrameRate, sizeof(f32));

    pFileStream->Read(&m_nVertexAnimationMask, sizeof(s32));

    u16 nMeshsNum = 0;
    pFileStream->Read(&nMeshsNum, sizeof(u16));

    for (u16 i = 0; i < nMeshsNum; ++i)
    {
        MeshVertexAnimation* pAnimation = new MeshVertexAnimation(this);
        if (pAnimation->LoadFromFileDataBinary(pFileStream))
        {
            m_mapMeshVertexAnimations.insert(std::make_pair(pAnimation->GetName(), pAnimation));
        }
        else
        {
            SafeDelete(pAnimation);
        }
    }

    /*
    for (int i = 0; i < m_nFrames; ++i)
    {
        //读取帧信息
        VertexAnimationKeyframe* pKeyframe = new VertexAnimationKeyframe;
        //pFileStream->Read(&nMeshNum, sizeof(int));
        if (pKeyframe->LoadFromFileDataBinary(pFileStream))
        {
            m_vtKeyframes.push_back(pKeyframe);
        }
        else
        {
            SafeDelete(pKeyframe);
        }
    }
    */

    m_fTimeRange = 1.0f / m_fFrameRate * m_nFrames;

    float fKeyFrameTime = 0.0f;
    for (int i = 0; i < m_nFrames; ++i)
    {
        m_vtKeyFrameTime.push_back(1.0f / m_fFrameRate * i);
    }
    return true;
}

void VertexAnimation::ApplyToSubmesh(CAIRSubMesh* pMesh, float fTimePos)
{
    VertexAnimationKeyframe* pKeyframe1, *pKeyframe2;
    float fInterpolate = GetKeyFramesAtTime(pMesh, fTimePos, &pKeyframe1, &pKeyframe2);
    pMesh->CaculateVerticesByVertexAnimation(fInterpolate, pKeyframe1,
        pKeyframe2, m_nVertexAnimationMask);
}

float VertexAnimation::GetKeyFramesAtTime(CAIRSubMesh* pMesh, float fTimePos, VertexAnimationKeyframe **pKeyFrame1, VertexAnimationKeyframe **pKeyFrame2)
{
    AIRVector<float>::iterator it = std::lower_bound(m_vtKeyFrameTime.begin(), m_vtKeyFrameTime.end(), fTimePos);

    int nFrame1 = 0;
    int nFrame2 = 0;

    float t1 = 0.0f;
    float t2 = 0.0f;

    MeshVertexAnimationMap_Iter itMesh = m_mapMeshVertexAnimations.find(pMesh->GetName());
    if (itMesh == m_mapMeshVertexAnimations.end())
    {
        return 0;
    }
    MeshVertexAnimation* pMeshVertexAnimation = itMesh->second;

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


    *pKeyFrame1 = pMeshVertexAnimation->GetKeyframe(nFrame1); //m_vtKeyframes[nFrame1];

    *pKeyFrame2 = pMeshVertexAnimation->GetKeyframe(nFrame2);

    //char szDebug[256] = { 0 };
    //sprintf(szDebug, "now the animation is %s, timepos : %.4f, keyframe1:%d, keyframe2:%d\n", 
    //    m_strName.c_str(), fTimePos, nFrame1, nFrame2);
    //OutputDebugString(szDebug);
    return (fTimePos - t1) / (t2 - t1);
}

void VertexAnimation::SetFrameRate(float fFrameRate)
{
    m_fFrameRate = fFrameRate;
    m_fTimeRange = 1.0f / m_fFrameRate * m_nFrames;
    m_vtKeyFrameTime.clear();
    float fKeyFrameTime = 0.0f;
    for (int i = 0; i < m_nFrames; ++i)
    {
        m_vtKeyFrameTime.push_back(1.0f / m_fFrameRate * i);
    }
}

void VertexAnimation::ApplyToVertices(CAIRSubMesh* pMesh, LPVERTEX_NORMAL pVertices, u32 nVerticesNum, f32 fTimePos)
{
    VertexAnimationKeyframe* pKeyframe1, *pKeyframe2;
    float fInterpolate = GetKeyFramesAtTime(pMesh, fTimePos, &pKeyframe1, &pKeyframe2);
    if (m_nVertexAnimationMask & eVAM_Position)
    {
        for (u32 i = 0; i < nVerticesNum; i++)
        {
            pVertices[i].vPos.x = pKeyframe1->m_vtVertexKeyframePositions[i].x + fInterpolate * (pKeyframe2->m_vtVertexKeyframePositions[i].x - pKeyframe1->m_vtVertexKeyframePositions[i].x);
            pVertices[i].vPos.y = pKeyframe1->m_vtVertexKeyframePositions[i].y + fInterpolate * (pKeyframe2->m_vtVertexKeyframePositions[i].y - pKeyframe1->m_vtVertexKeyframePositions[i].y);
            pVertices[i].vPos.z = pKeyframe1->m_vtVertexKeyframePositions[i].z + fInterpolate * (pKeyframe2->m_vtVertexKeyframePositions[i].z - pKeyframe1->m_vtVertexKeyframePositions[i].z);
        }
    }

    if (m_nVertexAnimationMask & eVAM_UV)
    {
        for (u32 i = 0; i < nVerticesNum; i++)
        {
            pVertices[i].tu = fInterpolate < 0.5f ? pKeyframe1->m_vtVertexKeyframeUVs[i].x : pKeyframe2->m_vtVertexKeyframeUVs[i].x; //pKeyframe1->m_vtVertexKeyframeDatas[i].tu * (1.0f - fInterpolate) + pKeyframe2->m_vtVertexKeyframeDatas[i].tu * fInterpolate;
            pVertices[i].tv = fInterpolate < 0.5f ? pKeyframe1->m_vtVertexKeyframeUVs[i].y : pKeyframe2->m_vtVertexKeyframeUVs[i].y; //pKeyframe1->m_vtVertexKeyframeDatas[i].tv * (1.0f - fInterpolate) + pKeyframe2->m_vtVertexKeyframeDatas[i].tv * fInterpolate;
            pVertices[i].color = fInterpolate < 0.5f ? pKeyframe1->m_vtVertexKeyframeColors[i] : pKeyframe2->m_vtVertexKeyframeColors[i];
        }
    }
    
}
