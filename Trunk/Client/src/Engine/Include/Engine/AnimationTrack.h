// ***************************************************************
//  文件: AnimationTrack.h
//  日期: 2010-5-22
//  作者: huangjunjie
//  说明: 动画序列
// ***************************************************************

#pragma once

#ifndef __AnimationTrack_H__
#define __AnimationTrack_H__


#include "EngineConfig.h"
#include <vector>
#include <list>
#include <map>
#include <string>
#include "Spline.h"


class NodeAnimation;
class AnimationKeyFrame;
//class TransformAnimationKeyFrame;
class AnimationTrack;
class NodeAnimationTrack;
class CSceneNode;


    // 时间索引
    class EngineExport TimeIndex
    {
    public:
        TimeIndex(float timePos)
            : mTimePos(timePos)
            , mKeyIndex(INVALID_KEY_INDEX)
        {
        }

        TimeIndex(float timePos, u32 keyIndex)
            : mTimePos(timePos)
            , mKeyIndex(keyIndex)
        {
        }

        bool HasKeyIndex(void) const
        {
            return mKeyIndex != INVALID_KEY_INDEX;
        }

        float GetTimePos(void) const
        {
            return mTimePos;
        }

        u32 GetKeyIndex(void) const
        {
            return mKeyIndex;
        }

	protected:
		float				mTimePos;					// 时间点
		u32					mKeyIndex;					// 索引
		static const u32	INVALID_KEY_INDEX = (u32)-1;// 非法的关键帧索引值
    };


    // 动画序列
	class EngineExport AnimationTrack
    {
    public:
		// 动画序列监听器
		class EngineExport Listener
		{
		public:
			virtual ~Listener() {}
			// 获得插值的关键帧
			virtual bool GetInterpolatedKeyFrame(const AnimationTrack* t, const TimeIndex& timeIndex, AnimationKeyFrame* kf) = 0;
		};

		AnimationTrack(NodeAnimation* parent, unsigned short handle) :
			mParent(parent), 
			mHandle(handle), 
			mListener(0)
		{

		}

        virtual ~AnimationTrack()
		{
			RemoveAllKeyFrames();
		}

		// 获得关键帧的索引
		unsigned short GetHandle(void) const 
		{ 
			return mHandle; 
		}

        // 获得动画的关键帧数量
        virtual unsigned short GetNumKeyFrames(void) const
		{
			return (unsigned short)mKeyFrames.size();
		}

        // 获得关键帧
        virtual AnimationKeyFrame* GetKeyFrame(unsigned short index) const;

        // 获得插值关键帧
        virtual float GetKeyFramesAtTime(
			const TimeIndex& timeIndex, 
			AnimationKeyFrame** keyFrame1, 
			AnimationKeyFrame** keyFrame2,
            unsigned short* firstKeyIndex = 0) const;

        // 创建关键帧
        virtual AnimationKeyFrame* CreateKeyFrame(float timePos);

        // 移除关键帧
        virtual void RemoveKeyFrame(unsigned short index);

        // 移除所有的关键帧
        virtual void RemoveAllKeyFrames(void);


        // 获得插值关键帧
        virtual void GetInterpolatedKeyFrame(const TimeIndex& timeIndex, AnimationKeyFrame* kf) const = 0;

        // 每帧更新后提交动画变换
        virtual void Apply(const TimeIndex& timeIndex, float weight = 1.0, float scale = 1.0f) = 0;

		// 内部函数,标明关键帧数据有变,需要重建
		virtual void _KeyFrameDataChanged(void) const {}

		// 标明所有的关键帧都是合法有用的
		virtual bool HasNonZeroKeyFrames(void) const { return true; }

		// 优化关键帧
		virtual void Optimise(void) {}

		// 收集关键帧
		virtual void _CollectKeyFrameTimes(AIRVector<float>& keyFrameTimes);

		// 创建关键帧索引表
		virtual void _BuildKeyFrameIndexMap(const AIRVector<float>& keyFrameTimes);

		// 设置监听器
		virtual void SetListener(Listener* l) { mListener = l; }

		// 获得父动画对象
		NodeAnimation *GetParent() const { return mParent; }


	protected:
		// 创建关键帧
		virtual AnimationKeyFrame* CreateKeyFrameImpl(float time) = 0;

		// 克隆
		virtual void PopulateClone(AnimationTrack* clone) const;

    protected:
		typedef AIRVector<AnimationKeyFrame*> KeyFrameList;
		typedef AIRVector<unsigned short>		KeyFrameIndexMap;

        KeyFrameList		mKeyFrames;			// 关键帧序列
        NodeAnimation*		mParent;			// 动画对象
		unsigned short		mHandle;			// 索引
		Listener*			mListener;			// 监听器
        KeyFrameIndexMap	mKeyFrameIndexMap;	// 关键帧索引表

	};







	// 节点动画序列
	class EngineExport NodeAnimationTrack : public AnimationTrack
	{
    public:
        enum ApplyNodeMask
        {
            ANM_Translate = 0x0001,
            ANM_Rotate = 0x0002,
            ANM_Scale  = 0x0004,
        };
	public:
		NodeAnimationTrack(NodeAnimation* parent, unsigned short handle);
		NodeAnimationTrack(NodeAnimation* parent, unsigned short handle, CSceneNode* targetNode);
        virtual ~NodeAnimationTrack();

        // 创建节点动画关键帧对象
        virtual AnimationKeyFrame* CreateNodeKeyFrame(float timePos);

		// 获得变换的节点对象
		virtual CSceneNode* GetAssociatedNode(void) const
		{
			return mTargetNode;
		}

		// 设置变换的节点对象
		virtual void SetAssociatedNode(CSceneNode* node)
		{
			mTargetNode = node;
		}

		// 每帧更新后提交的变换
		virtual void ApplyToNode(CSceneNode* node, const TimeIndex& timeIndex, float weight = 1.0, float scale = 1.0f);

		// 设置旋转的计算方式
		virtual void SetUseShortestRotationPath(bool useShortestPath)
		{
			mUseShortestRotationPath = useShortestPath ;
		}

		// 获取旋转的计算方式
		virtual bool GetUseShortestRotationPath() const
		{
			return mUseShortestRotationPath ;
		}

		// 获得插值关键帧
        virtual void GetInterpolatedKeyFrame(const TimeIndex& timeIndex, AnimationKeyFrame* kf) const;

		// 每帧更新后提交的变换
		virtual void Apply(const TimeIndex& timeIndex, float weight = 1.0, float scale = 1.0f);

		// 声明关键帧数据发生变化
		void _KeyFrameDataChanged(void) const
		{
			mSplineBuildNeeded = true;
		}

		// 获得节点动画关键帧
		virtual AnimationKeyFrame* GetNodeKeyFrame(unsigned short index) const;


		// 是否所有关键帧都合法有效
		virtual bool HasNonZeroKeyFrames(void) const;

		// 优化关键帧
		virtual void Optimise(void);

		// 克隆
		NodeAnimationTrack* _Clone(NodeAnimation* newParent) const;

        //设置影响mask
        void  SetApplyMask(unsigned int nMask);
		
	protected:
		// 创建关键帧
		AnimationKeyFrame* CreateKeyFrameImpl(float time);

		// 下次是否需要重建插值曲线
		virtual void buildInterpolationSplines(void) const;


	protected:
        // 样条曲线对象
        struct Splines
        {
		    VertexSpline		positionSpline;
		    VertexSpline		scaleSpline;
		    RotationalSpline	rotationSpline;
        };

		CSceneNode*				mTargetNode;				// 场景节点
		mutable Splines*		mSplines;					// 插值曲线对象
		mutable bool			mSplineBuildNeeded;			// 是否需要重建插值曲线
		mutable bool			mUseShortestRotationPath;	// 旋转计算方式

        unsigned int m_nApplyMask;    //影响mask
	};



#endif //! __AnimationTrack_H__