#include "stdafx.h"
#include "AIRAnimationMixRes.h"
#include "FileSystem.h"
#include "Engine.h"
#include "AIRSkeletonAnimation.h"
#include "AIRAnimationMixer.h"
//#include "AIRSkeletonRes.h"
#include "AIRModel.h"

IMPLEMENT_RTTI(AIRAnimationMixRes, Resource);

AIRAnimationMixData::AIRAnimationMixData(const char* szName, AIRAnimationMixRes* pOwner) : m_strName(szName)
, m_pOwner(pOwner)
{

}

AIRAnimationMixData::~AIRAnimationMixData()
{

}

bool AIRAnimationMixData::LoadFromFileData(CDataStream* pStream)
{
    int nAnimationsNum;
    pStream->Read(&nAnimationsNum, sizeof(int));

    char szAnimationName[256] = { 0 };

    AIRModel* pModelRes = m_pOwner->GetModelRes();

    for (int i = 0; i < nAnimationsNum; ++i)
    {
        pStream->Read(szAnimationName, 256);
        //根据名字获得骨骼动画
        CAIRSkeletonAnimation* pAnimation = pModelRes->GetAnimation(szAnimationName);
        WeightVector vtWeights;
        vtWeights.resize(m_pOwner->GetBonesNum());
        int nType;
        pStream->Read(&nType, sizeof(int));
        pStream->Read(&vtWeights[0], sizeof(float) * m_pOwner->GetBonesNum());

        if (nType == 0)
        {
            m_mapAnimationActionWeights.insert(std::make_pair(pAnimation, vtWeights));
        }
        else
            m_mapAnimationCircleWeights.insert(std::make_pair(pAnimation, vtWeights));
    }
    return true;
}

AIRAnimationMixData::WeightVector* AIRAnimationMixData::GetActionWeightVector(CAIRSkeletonAnimation* pAnimation)
{
    MapAnimationWeightsIter it = m_mapAnimationActionWeights.find(pAnimation);
    if (it != m_mapAnimationActionWeights.end())
    {
        return &it->second;
    }
    return NULL;
}

AIRAnimationMixData::WeightVector* AIRAnimationMixData::GetCircleWeightVector(CAIRSkeletonAnimation* pAnimation)
{
    MapAnimationWeightsIter it = m_mapAnimationCircleWeights.find(pAnimation);
    if (it != m_mapAnimationCircleWeights.end())
    {
        return &it->second;
    }
    return NULL;
}

AIRAnimationMixData::WeightVector* AIRAnimationMixData::GetWeightVector(CAIRSkeletonAnimation* pAnimation)
{
    MapAnimationWeightsIter it = m_mapAnimationActionWeights.find(pAnimation);
    if (it != m_mapAnimationActionWeights.end())
    {
        return &it->second;
    }

    it = m_mapAnimationCircleWeights.find(pAnimation);
    if (it != m_mapAnimationCircleWeights.end())
    {
        return &it->second;
    }
    return NULL;
}

inline int AIRAnimationMixData::GetBonesNum() const
{
    return m_pOwner->GetBonesNum();
}

void AIRAnimationMixData::SetActionAnimationBoneWeight(CAIRSkeletonAnimation* pAnimation, int nBoneIndex, float fWeight)
{
    MapAnimationWeightsIter it = m_mapAnimationActionWeights.find(pAnimation);
    if (it != m_mapAnimationActionWeights.end())
    {
        WeightVector* pWeights = &it->second;
        if (pWeights->size() == 0)
        {
            pWeights->resize(GetBonesNum());
        }

        (*pWeights)[nBoneIndex] = fWeight;
    }
}

void AIRAnimationMixData::SetAnimationBoneWeight(CAIRSkeletonAnimation* pAnimation, int nBoneIndex, float fWeight)
{
    MapAnimationWeightsIter it = m_mapAnimationActionWeights.find(pAnimation);
    if (it != m_mapAnimationActionWeights.end())
    {
        WeightVector* pWeights = &it->second;
        if (pWeights->size() == 0)
        {
            pWeights->resize(GetBonesNum());
        }

        (*pWeights)[nBoneIndex] = fWeight;
        return;
    }

    it = m_mapAnimationCircleWeights.find(pAnimation);
    if (it != m_mapAnimationCircleWeights.end())
    {
        WeightVector* pWeights = &it->second;
        if (pWeights->size() == 0)
        {
            pWeights->resize(GetBonesNum());
        }

        (*pWeights)[nBoneIndex] = fWeight;
        return;
    }
}

void AIRAnimationMixData::DeleteAnimationAction(CAIRSkeletonAnimation* pAnimation)
{
    MapAnimationWeightsIter it = m_mapAnimationActionWeights.find(pAnimation);
    if (it != m_mapAnimationActionWeights.end())
    {
        m_mapAnimationActionWeights.erase(it);
    }
}

void AIRAnimationMixData::DeleteAnimationAction(const char* szAnimationName)
{
    MapAnimationWeightsIter itEnd = m_mapAnimationActionWeights.end();
    for (MapAnimationWeightsIter it = m_mapAnimationActionWeights.begin(); it != itEnd; ++it)
    {
        CAIRSkeletonAnimation* pAnimation = it->first;
        if (strcmp(pAnimation->GetName().c_str(), szAnimationName) == 0)
        {
            m_mapAnimationActionWeights.erase(it);
            break;
        }
    }
}

void AIRAnimationMixData::DeleteAnimationCircle(CAIRSkeletonAnimation* pAnimation)
{
    MapAnimationWeightsIter it = m_mapAnimationCircleWeights.find(pAnimation);
    if (it != m_mapAnimationCircleWeights.end())
    {
        m_mapAnimationCircleWeights.erase(it);
    }
}

void AIRAnimationMixData::DeleteAnimationCircle(const char* szAnimationName)
{
    MapAnimationWeightsIter itEnd = m_mapAnimationCircleWeights.end();
    for (MapAnimationWeightsIter it = m_mapAnimationCircleWeights.begin(); it != itEnd; ++it)
    {
        CAIRSkeletonAnimation* pAnimation = it->first;
        if (strcmp(pAnimation->GetName().c_str(), szAnimationName) == 0)
        {
            m_mapAnimationCircleWeights.erase(it);
            break;
        }
    }
}

void AIRAnimationMixData::DeleteAnimation(CAIRSkeletonAnimation* pAnimation)
{
    MapAnimationWeightsIter it = m_mapAnimationActionWeights.find(pAnimation);
    if (it != m_mapAnimationActionWeights.end())
    {
        m_mapAnimationActionWeights.erase(it);
        return;
    }

    it = m_mapAnimationCircleWeights.find(pAnimation);
    if (it != m_mapAnimationCircleWeights.end())
    {
        m_mapAnimationCircleWeights.erase(it);
    }
}

void AIRAnimationMixData::DeleteAnimation(const char* szAnimationName)
{
    MapAnimationWeightsIter itEnd = m_mapAnimationActionWeights.end();
    for (MapAnimationWeightsIter it = m_mapAnimationActionWeights.begin(); it != itEnd; ++it)
    {
        CAIRSkeletonAnimation* pAnimation = it->first;
        if (strcmp(pAnimation->GetName().c_str(), szAnimationName) == 0)
        {
            m_mapAnimationActionWeights.erase(it);
            return;
        }
    }

    itEnd = m_mapAnimationCircleWeights.end();
    for (MapAnimationWeightsIter it = m_mapAnimationCircleWeights.begin(); it != itEnd; ++it)
    {
        CAIRSkeletonAnimation* pAnimation = it->first;
        if (strcmp(pAnimation->GetName().c_str(), szAnimationName) == 0)
        {
            m_mapAnimationCircleWeights.erase(it);
            break;
        }
    }
}

void AIRAnimationMixData::AddAnimationAction(CAIRSkeletonAnimation* pAnimation)
{
    MapAnimationWeightsIter it = m_mapAnimationActionWeights.find(pAnimation);
    if (it == m_mapAnimationActionWeights.end())
    {
        assert(m_pOwner->GetModelRes()->GetAnimation(pAnimation->GetName().c_str()) != NULL);
        WeightVector weight_vector;
        weight_vector.resize(GetBonesNum());
        m_mapAnimationActionWeights.insert(std::make_pair(pAnimation, weight_vector));
    }
}

void AIRAnimationMixData::AddAnimationCircle(CAIRSkeletonAnimation* pAnimation)
{
    MapAnimationWeightsIter it = m_mapAnimationCircleWeights.find(pAnimation);
    if (it == m_mapAnimationCircleWeights.end())
    {
        assert(m_pOwner->GetModelRes()->GetAnimation(pAnimation->GetName().c_str()) != NULL);
        WeightVector weight_vector;
        weight_vector.resize(GetBonesNum());
        m_mapAnimationCircleWeights.insert(std::make_pair(pAnimation, weight_vector));
    }
}


AIRAnimationMixRes::AIRAnimationMixRes() : m_fFileVersion(0.1f)
, m_nBonesNum(0)
, m_pModelRes(NULL)
{
    
}

AIRAnimationMixRes::~AIRAnimationMixRes()
{
    DT_TO_DBGSTR(LT_NONE, GetString("----AIRAnimationMixRes[%s] delete----\n", m_strFilename.c_str()));
}

int AIRAnimationMixRes::Release()
{
    if (DropRef() == 0)
    {
        Clear();
    }
    return GetRef();
}

void AIRAnimationMixRes::Clear()
{
    ANIMATIONMIXDATA_ITER itEnd = m_mapAnimationMixData.end();
    for (ANIMATIONMIXDATA_ITER it = m_mapAnimationMixData.begin(); it != itEnd; ++it)
    {
        AIRAnimationMixData* pData = it->second;
        SafeDelete(pData);
    }

    m_mapAnimationMixData.clear();
}

/*
bool AIRAnimationMixRes::LoadFromFile(const char* szFilename)
{
    SetFilename(szFilename);
    m_strName = szFilename;
    //pStream->Seek(0);
    
    return Load();
}
*/

/*
AIRAnimationMixRes::WeightVector* AIRAnimationMixRes::GetWeightVector(CAIRSkeletonAnimation* pAnimation)
{
    MapAnimationWeightsIter it = m_mapAnimationWeights.find(pAnimation);
    if (it != m_mapAnimationWeights.end())
    {
        return &it->second;
    }
    return NULL;
}
*/

bool AIRAnimationMixRes::SaveToFile(const char* szFilename)
{
    if (m_pModelRes == NULL)
    {
        return false;
    }
    std::fstream fout;

    fout.open(szFilename, std::ios::out | std::ios::binary);

    //输出版本号
    //fout<<(float)0.1f;
    float fVersion = 0.1f;
    fout.write((const char*)&fVersion, sizeof(float));

    fout.write(m_strName.c_str(), 256);

    fout.write(m_pModelRes->GetName().c_str(), 256);

    int nBonesNum = m_pModelRes->GetBoneInstanceCount();
    fout.write((const char*)&nBonesNum, sizeof(int));

    int nMixNum = m_mapAnimationMixData.size();
    fout.write((const char*)&nMixNum, sizeof(int));

    ANIMATIONMIXDATA_ITER itEnd = m_mapAnimationMixData.end();
    for (ANIMATIONMIXDATA_ITER it = m_mapAnimationMixData.begin(); it != itEnd; ++it)
    {
        AIRAnimationMixData* pData = it->second;
        fout.write(pData->GetName().c_str(), 256);
        int nActionAnimationsNum = pData->GetActionAnimationsNum() + pData->GetCircleAnimationsNum();
        fout.write((const char*)&nActionAnimationsNum, sizeof(int));

        MapIterator<AIRAnimationMixData::MapAnimationWeights> iter = pData->GetAnimationActionWeightsIter();
        int nType = 0;
        while (!iter.IsEnd())
        {
            fout.write(iter.PeekNextKey()->GetName().c_str(), 256);
            AIRAnimationMixData::WeightVector vtWeight = iter.GetNext();
            fout.write((const char*)&nType, sizeof(nType));
            fout.write((const char*)&(vtWeight[0]), vtWeight.size() * sizeof(float));
        }

        iter = pData->GetAnimationCircleWeightsIter();
        nType = 1;
        while (!iter.IsEnd())
        {
            fout.write(iter.PeekNextKey()->GetName().c_str(), 256);
            AIRAnimationMixData::WeightVector vtWeight = iter.GetNext();
            fout.write((const char*)&nType, sizeof(nType));
            fout.write((const char*)&(vtWeight[0]), vtWeight.size() * sizeof(float));
        }
    }
    /*
    int nAnimationsNum = m_mapAnimationWeights.size();
    fout.write((char*)&nAnimationsNum, sizeof(int));

    MapAnimationWeightsIter itEnd = m_mapAnimationWeights.end();

    for (MapAnimationWeightsIter it = m_mapAnimationWeights.begin(); it != itEnd; ++it)
    {
        CAIRSkeletonAnimation* pAnimation = it->first;
        WeightVector* pWeights = &it->second;
        fout.write(pAnimation->GetName().c_str(), 256);
        fout.write((char*)&((*pWeights)[0]), pWeights->size() * sizeof(float));
    }
    */

    fout.close();
    return true;
}

AIRAnimationMixData* AIRAnimationMixRes::GetAnimationMixData(const AIRString& strName)
{
    ANIMATIONMIXDATA_ITER it = m_mapAnimationMixData.find(strName);

    if (it != m_mapAnimationMixData.end())
    {
        return it->second;
    }

    return NULL;
}

AIRAnimationMixData* AIRAnimationMixRes::CreateAnimationMixData(const char* szName)
{
    ANIMATIONMIXDATA_ITER it = m_mapAnimationMixData.find(szName);
    AIRAnimationMixData* pData = NULL;
    if (it != m_mapAnimationMixData.end())
    {
        pData = it->second;
    }
    else
    {
        pData = new AIRAnimationMixData(szName, this);
        m_mapAnimationMixData.insert(std::make_pair(pData->GetName(), pData));
    }

    return pData;
}

void AIRAnimationMixRes::SetModelRes(AIRModel* pModelRes)
{
    m_pModelRes = pModelRes; 
    m_nBonesNum = pModelRes->GetBoneInstanceCount();
}

bool AIRAnimationMixRes::Cache()
{
    m_pStream->Read(&m_fFileVersion, sizeof(float));
    if (m_fFileVersion < ENGINE_INST->GetMeshFileVersion())
    {
        SafeDelete(m_pStream);
        return false;
    }

    char szName[256] = { 0 };

    m_pStream->Read(szName, 256);
    m_strName = szName;

    char szSkeletonFile[256] = { 0 };
    m_pStream->Read(szSkeletonFile, 256);
    AIRModel* pModelRes = RESOURCE_MANAGER->CreateResource<AIRModel>(szSkeletonFile); //MODELMGR->GetSkeletonRes(szSkeletonFile);

    if (pModelRes == NULL)
    {
        SafeDelete(m_pStream);
        return false;
    }

    m_pModelRes = pModelRes;

    m_pStream->Read(&m_nBonesNum, sizeof(int));

    int nMixersNum = 0;
    m_pStream->Read(&nMixersNum, sizeof(int));

    char szMixerName[256] = { 0 };

    for (int i = 0; i < nMixersNum; ++i)
    {
        m_pStream->Read(szMixerName, 256);
        AIRAnimationMixData* pAnimationMixData = new AIRAnimationMixData(szMixerName, this);

        if (pAnimationMixData->LoadFromFileData(m_pStream))
        {
            m_mapAnimationMixData.insert(std::make_pair(pAnimationMixData->GetName(), pAnimationMixData));
        }

    }



    SafeDelete(m_pStream);
    m_bReady = true;
    return m_bReady;
}
