/********************************************************************
	created:	2009/10/06
	created:	6:10:2009   11:37
	filename: 	d:\My Codes\3DGraphic\Engine\src\Scene\AIRSkeletonAnimation.h
	file path:	d:\My Codes\3DGraphic\Engine\src\Scene
	file base:	AIRAnimation
	file ext:	h
	author:		liangairan
	
	purpose:	骨骼动画类，用于骨骼动画
*********************************************************************/

#pragma once

#include "IReference.h"
//#include "AIRSkeleton.h"
//#include "MyVector.h"
#include "DataStream.h"
#include "SceneNode.h"
#include "AIRAnimation.h"
#include "EngineResource.h"
class CAIRBone;

//关键帧数据
/*
typedef struct tagFrameData 
{
    Vector3Df  vPosition;    //相对父亲的位置
    Quaternion qOrient;      //相对父亲的旋转
}FRAMEDATA, *LPFRAMEDATA;


class CAIRSkeletonRes;
class CModelInstance;

class CKeyFrame
{
public:
    CKeyFrame() : m_scale(Vector3Df::UNIT_SCALE) {}
    virtual ~CKeyFrame() {}

public:
    Vector3Df	m_translate;	//相对上一关键帧的变换
    Quaternion	m_rotation;		//相对上一关键帧的旋转
    Vector3Df	m_scale;		//相对上一关键帧的缩放
};
*/

class AIRModel;

class EngineExport CAIRSkeletonAnimation : public CAIRAnimation, public Resource
{
	DECLARE_RTTI;
	EngineAllocator(CAIRSkeletonAnimation)
public:
    CAIRSkeletonAnimation(AIRModel* pModel);
    virtual ~CAIRSkeletonAnimation();

    // 从文件中加载
    bool LoadFromFileData(/*FILE* fp, char* buff*/CDataStream* pStream, AIRModel* pBaseModel);

    //从二进制文件读取
    bool LoadFromFileDataBinary(CDataStream* pStream, AIRModel* pBaseModel);

    void SaveToFileStream(std::fstream& fout, AIRModel* pBaseModel) const;

    // 清除动画数据
    virtual void Clear();

    //const AIRString& GetName() const
    //{
    //    return m_strName;
    //}

    //void SetName(const char* szName)
    //{
    //    m_strName = szName;
    //}

	//************************************
	// Method:    BuildFrameSkeletons
	// FullName:  CAIRSkeletonAnimation::BuildFrameSkeleton
	// Access:    public 
	// Returns:   void
	// Qualifier: 创建每一帧的骨骼
	// Parameter: CAIRSkeleton * pBaseSkeleton   基础骨骼
	// Parameter: int nFrame                     帧索引，即第几帧
	// Parameter: AIRVector<FRAMEDATA> & vtFrameData  一帧里面所有骨的位置和旋转信息
	//************************************
	void BuildFrameSkeleton(int nBoneIndex, AIRVector<FRAMEDATA>& vtFrameData);

	virtual bool Cache();

    //这里计算出每帧的数据
    void CaculateFrameData();

    //添加骨骼的帧
    //virtual void AddFrameSkeleton(CAIRSkeleton* pFrameSkel);

    //设置动画时间
    virtual void SetTimeRange(float fTime);

    //设置帧速（每秒放多少帧）
    virtual void SetFrameRate(float fFrameRate);

    //删除帧数据，在过渡动画中用到
    virtual void RemoveAllFrames(bool bDelete = false);

    //根据时间获得两个关键帧
    //return 返回插值的值
    //virtual float GetKeyFramesAtTime(float fTimePos, CAIRSkeleton** pKeyFrame1, CAIRSkeleton** pKeyFrame2);

	virtual float GetKeyFramesAtTime(f32 fTimePos, u32& nFrame1, u32& nFrame2);

    virtual void AddKeyFrameTime(float fTime);

    //作用到模型上
    virtual void Apply(CModelInstance* pModelInst, float fTimePos, float fWeight);

    //作用到结点上
    virtual void ApplyToNode(CSceneNode* pNode, int nBoneIndex, float fTimePos, float fWeight);

    //混合到模型
    virtual void Blend(CModelInstance* pModelInst, float fTimePos, float fWeight);

    //混合到结点上
    virtual void BlendToNode(CSceneNode* pNode, int nBoneIndex, float fTimePos, float fWeight);

	//CAIRSkeletonRes* GetSkeletonRes()
	//{
	//	return m_pSkeletonRes;
	//}

protected:
    //获得某一条骨的插值后的关键帧数据
    virtual void GetInterpolatedKeyFrame(float fTimePos, int nBoneIndex, CKeyFrame& keyFrame);

protected:
    
    //int							m_nFrames;			//该动画的关键帧数
    //float						m_fFrameRate;		//帧速，每秒多少帧
    //float						m_fTimeRange;		//时间单位秒
    //AIRString					m_strName;			//动画名字
    //AIRVector<CAIRSkeleton*>  m_vtFrameSkeleton;	//每一帧的骨骼
    AIRVector<float>			m_vtKeyFrameTime;	//关键帧时间点
    CKeyFrame					m_keyFrameTransform;//变换的关键帧
	//CAIRSkeletonRes* m_pSkeletonRes;
	AIRModel* m_pModelRes;
	AIRVector<CAIRBone*>      m_vtFrameBones;
private:

    
    //MyVector<CAIRAnibone*> m_vtAniBones;
};
