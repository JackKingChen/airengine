// ***************************************************************
//  �ļ�: AnimationKeyFrame.h
//  ����: 2010-5-22
//  ����: huangjunjie
//  ˵��: �����ؼ�֡
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



// �任�����ؼ�֡
class EngineExport AnimationKeyFrame : public CKeyFrame
{
public:
	// ���캯��, ͨ�� AnimationTrack::CreateKeyFrame()����
	AnimationKeyFrame(/*const AnimationTrack* parent, */float time):
	//m_pAnimationTrack( parent ),
	m_fTimePos( time ),
	CKeyFrame()
	{
	}

	~AnimationKeyFrame() 
	{

	}

	// ���ʱ���
	float GetTimePos( void ) 
	{ 
		return m_fTimePos; 
	}

	// ��¡�任�ؼ�֡
	AnimationKeyFrame* _Clone(AnimationTrack* newParent) const
	{
		AnimationKeyFrame* newKf = new AnimationKeyFrame(/*newParent,*/ m_fTimePos);
		newKf->m_translate = m_translate;
		newKf->m_scale = m_scale;
		newKf->m_rotation = m_rotation;
		return newKf;
	}

protected:
	float					m_fTimePos;				// ʱ���
	//const AnimationTrack*	m_pAnimationTrack;		// �������ж���

};//! end class AnimationKeyFrame


//// �����ؼ�֡
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
//	// ���ʱ���
//	float GetTimePos( void ) 
//	{ 
//		return m_fTimePos; 
//	}
//
//	// ��¡�����ؼ�֡����
//	AnimationKeyFrame* _Clone(AnimationTrack* newParent) const
//	{
//		return new AnimationKeyFrame(newParent, m_fTimePos);
//	}
//
//protected:
//	float					m_fTimePos;				// ʱ���
//	const AnimationTrack*	m_pAnimationTrack;		// �������ж���
//
//};//! end class AnimationKeyFrame
//


//// �任�����ؼ�֡
//class EngineExport TransformAnimationKeyFrame : public AnimationKeyFrame
//{
//public:
//	// ���캯��, ͨ�� AnimationTrack::CreateKeyFrame()����
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
//	// ����λ��
//	virtual void SetTranslate(const Vector3Df& trans)
//	{
//		mTranslate = trans;
//	}
//
//	// ���λ��
//	const Vector3Df& GetTranslate(void) const
//	{
//		return mTranslate;
//	}
//
//	// ��������
//	virtual void SetScale(const Vector3Df& scale)
//	{
//		mScale = scale;
//	}
//
//	// �������
//	virtual const Vector3Df& GetScale(void) const
//	{
//		return mScale;
//	}
//
//	// ������ת
//	virtual void SetRotation(const Quaternion& rot)
//	{
//		mRotate = rot;
//	}
//
//	// �����ת
//	virtual const Quaternion& GetRotation(void) const
//	{
//		return mRotate;
//	}
//
//	// ��¡�任�ؼ�֡
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
//	Vector3Df	mTranslate;	// λ��
//	Vector3Df	mScale;		// ����
//	Quaternion	mRotate;	// ����
//
//
//};

#endif //! __AnimationKeyFrame_H__