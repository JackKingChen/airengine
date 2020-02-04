// ***************************************************************
//  文件: AnimationKeyFrame.h
//  日期: 2010-5-22
//  作者: huangjunjie
//  说明: 动画关键帧
// ***************************************************************

#pragma once

#ifndef __AnimationKeyFrame_H__
#define __AnimationKeyFrame_H__

//#include "EngineConfig.h"
#include <vector>
#include <list>
#include <map>
#include <string>

#include "Vector3D.h"
#include "Quaternion.h"
#include "AIRAnimation.h"

class NodeAnimation;
class AnimationKeyFrame;
class TransformAnimationKeyFrame;
class AnimationTrack;
class NodeAnimationTrack;

using namespace AIR;



// 变换动画关键帧
class EngineExport AnimationKeyFrame : public CKeyFrame
{
public:
	// 构造函数, 通过 AnimationTrack::CreateKeyFrame()调用
	AnimationKeyFrame(/*const AnimationTrack* parent, */float time):
	//m_pAnimationTrack( parent ),
	m_fTimePos( time ),
	CKeyFrame()
	{
	}

	~AnimationKeyFrame() 
	{

	}

	// 获得时间点
	float GetTimePos( void ) 
	{ 
		return m_fTimePos; 
	}

	// 克隆变换关键帧
	AnimationKeyFrame* _Clone(AnimationTrack* newParent) const
	{
		AnimationKeyFrame* newKf = new AnimationKeyFrame(/*newParent,*/ m_fTimePos);
		newKf->m_translate = m_translate;
		newKf->m_scale = m_scale;
		newKf->m_rotation = m_rotation;
		return newKf;
	}

protected:
	float					m_fTimePos;				// 时间点
	//const AnimationTrack*	m_pAnimationTrack;		// 动画序列对象

};//! end class AnimationKeyFrame


//// 动画关键帧
//class EngineExport AnimationKeyFrame
//{
//public:
//	AnimationKeyFrame( const AnimationTrack* pAnimTrack, float fTimePos ) :
//	  m_pAnimationTrack( pAnimTrack ),
//	  m_fTimePos( fTimePos )
//	{
//
//	}
//
//	virtual ~AnimationKeyFrame( void )
//	{
//
//	}
//
//	// 获得时间点
//	float GetTimePos( void ) 
//	{ 
//		return m_fTimePos; 
//	}
//
//	// 克隆动画关键帧对象
//	AnimationKeyFrame* _Clone(AnimationTrack* newParent) const
//	{
//		return new AnimationKeyFrame(newParent, m_fTimePos);
//	}
//
//protected:
//	float					m_fTimePos;				// 时间点
//	const AnimationTrack*	m_pAnimationTrack;		// 动画序列对象
//
//};//! end class AnimationKeyFrame
//


//// 变换动画关键帧
//class EngineExport TransformAnimationKeyFrame : public AnimationKeyFrame
//{
//public:
//	// 构造函数, 通过 AnimationTrack::CreateKeyFrame()调用
//	TransformAnimationKeyFrame(const AnimationTrack* parent, float time):
//		AnimationKeyFrame(parent, time),
//		mTranslate(Vector3Df::ZERO), 
//		mScale(Vector3Df::UNIT_SCALE), 
//		mRotate(Quaternion::IDENEITY) 
//	{
//
//	}
//
//	~TransformAnimationKeyFrame() 
//	{
//
//	}
//
//	// 设置位置
//	virtual void SetTranslate(const Vector3Df& trans)
//	{
//		mTranslate = trans;
//	}
//
//	// 获得位置
//	const Vector3Df& GetTranslate(void) const
//	{
//		return mTranslate;
//	}
//
//	// 设置缩放
//	virtual void SetScale(const Vector3Df& scale)
//	{
//		mScale = scale;
//	}
//
//	// 获得缩放
//	virtual const Vector3Df& GetScale(void) const
//	{
//		return mScale;
//	}
//
//	// 设置旋转
//	virtual void SetRotation(const Quaternion& rot)
//	{
//		mRotate = rot;
//	}
//
//	// 获得旋转
//	virtual const Quaternion& GetRotation(void) const
//	{
//		return mRotate;
//	}
//
//	// 克隆变换关键帧
//	AnimationKeyFrame* _Clone(AnimationTrack* newParent) const
//	{
//		TransformAnimationKeyFrame* newKf = new TransformAnimationKeyFrame(newParent, m_fTimePos);
//		newKf->mTranslate = mTranslate;
//		newKf->mScale = mScale;
//		newKf->mRotate = mRotate;
//		return newKf;
//	}
//
//protected:	
//	Vector3Df	mTranslate;	// 位置
//	Vector3Df	mScale;		// 缩放
//	Quaternion	mRotate;	// 朝向
//
//
//};

#endif //! __AnimationKeyFrame_H__