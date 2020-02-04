/********************************************************************
	created:	2010/09/08
	created:	8:9:2010   18:07
	filename: 	AIRAnimationMixRes.h
	author:		liangairan
	
	purpose:	代表一个动作混合文件的资源，注意，所有混合文件的后缀名为animix
                暂时动作混合只限两个，一个代表action，另一个代表circle
*********************************************************************/

#pragma once 

#include "EngineResource.h"
#include "IteratorWrappers.h"

class CAIRSkeletonAnimation;
class AIRModel;
class AIRAnimationMixer;
class AIRAnimationMixRes;
class CDataStream;

class EngineExport AIRAnimationMixData
{
public:
    typedef AIRVector<float> WeightVector;
    typedef std::map<CAIRSkeletonAnimation*, WeightVector> MapAnimationWeights;
    typedef MapAnimationWeights::iterator MapAnimationWeightsIter;
public:
    AIRAnimationMixData(const char* szName, AIRAnimationMixRes* pOwner);
    ~AIRAnimationMixData();

    WeightVector* GetActionWeightVector(CAIRSkeletonAnimation* pAnimation);
    WeightVector* GetCircleWeightVector(CAIRSkeletonAnimation* pAnimation);

    WeightVector* GetWeightVector(CAIRSkeletonAnimation* pAnimation);

    bool LoadFromFileData(CDataStream* pStream);

    const AIRString& GetName() const
    {
        return m_strName;
    }

    inline int GetBonesNum() const;

    size_t GetActionAnimationsNum() const
    {
        return m_mapAnimationActionWeights.size();
    }

    size_t GetCircleAnimationsNum() const
    {
        return m_mapAnimationCircleWeights.size();
    }

    MapIterator<MapAnimationWeights> GetAnimationCircleWeightsIter()
    {
        return MapIterator<MapAnimationWeights>(m_mapAnimationCircleWeights);
    }

    MapIterator<MapAnimationWeights> GetAnimationActionWeightsIter()
    {
        return MapIterator<MapAnimationWeights>(m_mapAnimationActionWeights);
    }

    //设置某动画的骨的权重
    void SetActionAnimationBoneWeight(CAIRSkeletonAnimation* pAnimation, int nBoneIndex, float fWeight);

    void SetAnimationBoneWeight(CAIRSkeletonAnimation* pAnimation, int nBoneIndex, float fWeight);

    //删除动画
    void DeleteAnimationAction(CAIRSkeletonAnimation* pAnimation);

    void DeleteAnimationAction(const char* szAnimationName);

    void DeleteAnimationCircle(CAIRSkeletonAnimation* pAnimation);

    void DeleteAnimationCircle(const char* szAnimationName);

    void DeleteAnimation(CAIRSkeletonAnimation* pAnimation);

    void DeleteAnimation(const char* szAnimationName);

    //添加动画
    void AddAnimationAction(CAIRSkeletonAnimation* pAnimation);

    void AddAnimationCircle(CAIRSkeletonAnimation* pAnimation);

private:
    MapAnimationWeights  m_mapAnimationCircleWeights;
    MapAnimationWeights  m_mapAnimationActionWeights;

    AIRString m_strName;

    AIRAnimationMixRes* m_pOwner;
};

class EngineExport AIRAnimationMixRes : public Resource
{
	DECLARE_RTTI;
public:
    typedef std::map<AIRString, AIRAnimationMixData*> MapAnimationMixData;
public:
    AIRAnimationMixRes();

    virtual ~AIRAnimationMixRes();

    virtual int Release();

    //从文件中加载
    //bool LoadFromFile(const char* szFilename);

    

    void Clear();

    int GetBonesNum() const
    {
        return m_nBonesNum;
    }

    const AIRString& GetFilename() const
    {
        return m_strFilename;
    }

    //保存到文件
    bool SaveToFile(const char* szFilename);

    AIRAnimationMixData* GetAnimationMixData(const AIRString& strName);

    //创建数据
    AIRAnimationMixData* CreateAnimationMixData(const char* szName);

    AIRModel* GetModelRes()
    {
        return m_pModelRes;
    }

    MapIterator<MapAnimationMixData> GetAnimationMixDataIter()
    {
        return MapIterator<MapAnimationMixData>(m_mapAnimationMixData);
    }

    void SetModelRes(AIRModel* pSkeletonRes);

    virtual bool Cache();
protected:
private:
    

    //AIRString  m_strFilename;

    float  m_fFileVersion;
    int    m_nBonesNum;

    AIRModel*  m_pModelRes;   //骨骼文件资源
	AIRString m_strName;
    
    MapAnimationMixData m_mapAnimationMixData;
    typedef MapAnimationMixData::iterator ANIMATIONMIXDATA_ITER;
};

