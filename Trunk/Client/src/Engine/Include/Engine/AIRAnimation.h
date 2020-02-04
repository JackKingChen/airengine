/********************************************************************
	created:	2009/10/06
	created:	6:10:2009   11:37
	filename: 	d:\My Codes\3DGraphic\Engine\src\Scene\AIRAnimation.h
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

//关键帧数据
typedef struct tagFrameData 
{
    Vector3Df  vPosition;    //相对父亲的位置
    Quaternion qOrient;      //相对父亲的旋转
}FRAMEDATA, *LPFRAMEDATA;


class EngineExport CKeyFrame// : public EngineAllocator(CKeyFrame)
{
	EngineAllocator(CKeyFrame)

public:
	CKeyFrame() : m_scale(Vector3Df::UNIT_SCALE), m_translate(Vector3Df::ZERO), m_rotation(Quaternion::IDENEITY) {}
    virtual ~CKeyFrame() {}

	// 设置位置
	virtual void SetTranslate(const Vector3Df& trans)
	{
		m_translate = trans;
	}

	// 获得位置
	const Vector3Df& GetTranslate(void) const
	{
		return m_translate;
	}

	// 设置缩放
	virtual void SetScale(const Vector3Df& scale)
	{
		m_scale = scale;
	}

	// 获得缩放
	virtual const Vector3Df& GetScale(void) const
	{
		return m_scale;
	}

	// 设置旋转
	virtual void SetRotation(const Quaternion& rot)
	{
		m_rotation = rot;
	}

	// 获得旋转
	virtual const Quaternion& GetRotation(void) const
	{
		return m_rotation;
	}

public:
    Vector3Df	m_translate;	//相对上一关键帧的变换
    Quaternion	m_rotation;		//相对上一关键帧的旋转
    Vector3Df	m_scale;		//相对上一关键帧的缩放
};

class CModelInstance;

class EngineExport CAIRAnimation// : public EngineAllocator(CAIRAnimation)
{
	DECLARE_ROOT_RTTI;
	EngineAllocator(CAIRAnimation)
public:
    CAIRAnimation();
    virtual ~CAIRAnimation();

    // 从文件中加载
    //bool LoadFromFileData(/*FILE* fp, char* buff*/CDataStream* pStream, CAIRSkeletonRes* pBaseSkeleton);

    //void Update(float fTime);

    //virtual s32  Release();

    // 清除动画数据
    virtual void Clear() = 0;

    const AIRString& GetName() const
    {
        return m_strName;
    }

    void SetName(const char* szName)
    {
        m_strName = szName;
    }

	// 获得帧的数量
    int GetFramesNum() const { return m_nFrames; }

    //获得帧速
    float GetFrameRate() const { return m_fFrameRate; }


    //获得动画的播放时间
    float GetTimeRange() const
    {
        return m_fTimeRange;
    }

    //设置动画时间
    virtual void SetTimeRange(float fTime)
    {
        m_fTimeRange = fTime;
    }

    //设置帧速（每秒放多少帧）
    virtual void SetFrameRate(float fFrameRate);

    //删除帧数据，在过渡动画中用到
	virtual void RemoveAllFrames(bool bDelete = false) {}

	virtual void AddKeyFrameTime(float fTime) {}

	virtual void Apply(float timePos, float weight, float scale) {}

    //作用到模型上
	virtual void Apply(CModelInstance* pModelInst, float fTimePos, float fWeight) {}

    //作用到结点上
	virtual void ApplyToNode(CSceneNode* pNode, int nBoneIndex, float fTimePos, float fWeight) {}

    //混合到模型
	virtual void Blend(CModelInstance* pModelInst, float fTimePos, float fWeight) {}

    //混合到结点上
	virtual void BlendToNode(CSceneNode* pNode, int nBoneIndex, float fTimePos, float fWeight) {}

    //获得关键骨，返回两骨的时间间隔，只用在骨骼动画上
    //virtual float GetKeyFramesAtTime(float fTimePos, CAIRSkeleton** pKeyFrame1, CAIRSkeleton** pKeyFrame2)
    //{
    //    return 0;
    //}

protected:
    //获得某一条骨的插值后的关键帧数据
    //virtual void GetInterpolatedKeyFrame(float fTimePos, int nBoneIndex, CKeyFrame& keyFrame) = 0;

protected:
    
    u16							m_nFrames;			//该动画的关键帧数
    f32						m_fFrameRate;		//帧速，每秒多少帧
    float						m_fTimeRange;		//时间单位秒
    AIRString					m_strName;			//动画名字
private:

    
    //MyVector<CAIRAnibone*> m_vtAniBones;
};
