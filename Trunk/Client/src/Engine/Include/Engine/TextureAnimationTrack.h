/********************************************************************
	created:	2010/07/07
	created:	7:7:2010   9:28
	filename: 	TextureAnimationTrack.h
	author:		liangairan
	
	purpose:	纹理动画
*********************************************************************/

#pragma once 

#include "AnimationTrack.h"
#include "AnimationKeyFrame.h"

class EngineExport TextureAnimationTrack : public AnimationTrack
{
public:
    TextureAnimationTrack();
    virtual ~TextureAnimationTrack();

    // 获得插值关键帧
    virtual void GetInterpolatedKeyFrame(const TimeIndex& timeIndex, AnimationKeyFrame* kf) const;

    // 每帧更新后提交动画变换
    virtual void Apply(const TimeIndex& timeIndex, float weight = 1.0, float scale = 1.0f) {};

    //apply to the matrix
    virtual void ApplyToMatrix(Matrix4f& matApply, const TimeIndex& timeIndex, float weight = 1.0, float scale = 1.0f);

    float GetTimeLength() const
    {
        return mKeyFrames[mKeyFrames.size() - 1]->GetTimePos();
    }
protected:
    Vector3Df m_position;
    Vector3Df m_scale;
    Quaternion  m_rotation;
private:
};
