// ***************************************************************
//  文件: NodeAnimation.h
//  日期: 2010-5-22
//  作者: huangjunjie
//  说明: 动画类
// ***************************************************************

#pragma once


#include "EngineConfig.h"
#include "IteratorWrappers.h"
#include "AIRAnimation.h"
#include <vector>
#include <list>
#include <map>
#include <string>
#include <set>
#include "Spline.h"

class TimeIndex;
class NodeAnimation;
class AnimationKeyFrame;
//class TransformAnimationKeyFrame;
class AnimationTrack;
class NodeAnimationTrack;

class CSceneNode;



#define NODE_ANIM_TYPE 0


	// 动画类
//#if NODE_ANIM_TYPE == 0
//	class EngineExport NodeAnimation
//#else
	class EngineExport NodeAnimation : public CAIRAnimation
//#endif
    {
	public:
		typedef AIRVector<float>								KeyFrameTimeList;
		typedef std::map<unsigned short, NodeAnimationTrack*>	NodeTrackList;
		typedef ConstMapIterator<NodeTrackList>					NodeTrackIterator;
		// 关键帧ID序列
		typedef std::set<unsigned short>						TrackHandleList;


        // 向量插值方式
        enum InterpolationMode
        {
            // 线性插值
            IM_LINEAR,
            // 样条曲线插值
            IM_SPLINE
        };

        // 4元数旋转的插值方式
        enum RotationInterpolationMode
        {
            // 线性插值
            RIM_LINEAR,
            // 球面插值
            RIM_SPHERICAL
        };

        enum ApplyNodeMask
        {
            ANM_Translate = 0x0001,
            ANM_Rotate = 0x0002,
            ANM_Scale  = 0x0004,
        };
    public:
        
		// 构造函数, 给AnimationManager创建动画用
		NodeAnimation(CSceneNode* pNode, const AIRString& name, float length);

        virtual ~NodeAnimation();
        // 每帧更新后提交的变换
        virtual void ApplyToNode(CSceneNode* node, const f32 fTimePos, float weight = 1.0, float scale = 1.0f);

        // 获得插值关键帧
        virtual void GetInterpolatedKeyFrame(const f32 fTimePos, AnimationKeyFrame* kf) const;

        // 获得插值关键帧
        virtual float GetKeyFramesAtTime(
            const TimeIndex& timeIndex, 
            AnimationKeyFrame** keyFrame1, 
            AnimationKeyFrame** keyFrame2,
            unsigned short* firstKeyIndex = 0) const;

//#if NODE_ANIM_TYPE == 0
//		// 获得动画的名称
//		const AIRString& GetName(void) const
//		{
//			return mName;
//		}
//
//		// 获得动画的播放时间长度
//		float GetLength(void) const
//		{
//			return mLength;
//		}
//
//		// 设置动画的播放时间长度
//		void SetLength(float len)
//		{
//			mLength = len;
//		}
//#endif

        /*
		// 创建节点动画序列对象
        NodeAnimationTrack* CreateNodeTrack(unsigned short handle);

		// 创建节点动画序列对象
        NodeAnimationTrack* CreateNodeTrack(unsigned short handle, CSceneNode* node);


		// 获得节点动画序列对象的数量
        unsigned short GetNumNodeTracks(void) const
		{
			return (unsigned short)mNodeTrackList.size();
		}

        // 获得节点动画序列对象
        NodeAnimationTrack* GetNodeTrack(unsigned short handle) const;

		// 是否存在id为handle的节点动画序列对象
		bool HasNodeTrack(unsigned short handle) const
		{
			return (mNodeTrackList.find(handle) != mNodeTrackList.end());
		}
		
		// 销毁节点动画序列对象
        void DestroyNodeTrack(unsigned short handle);

		// 销毁所有的动画序列对象
        void DestroyAllTracks(void);

		// 销毁所有的节点动画序列对象
		void DestroyAllNodeTracks(void);
        */

        // 将动画的计算结果更新到节点变换
        //void Apply(float timePos, float weight = 1.0, float scale = 1.0f);

        // 设置向量的插值方式
        void SetInterpolationMode(InterpolationMode im)
		{
			mInterpolationMode = im;
		}

        // 获得向量的插值方式
        InterpolationMode GetInterpolationMode(void) const
		{
			return mInterpolationMode;
		}

        // 设置4元数插值方式
        void SetRotationInterpolationMode(RotationInterpolationMode im)
		{
			mRotationInterpolationMode = im;
		}

        // 获得4元数插值方式
        RotationInterpolationMode GetRotationInterpolationMode(void) const
		{
			return mRotationInterpolationMode;
		}

        // 设置默认向量插值方式
        static void SetDefaultInterpolationMode(InterpolationMode im)
		{
			msDefaultInterpolationMode = im;
		}

        // 获得默认向量插值方式
        static InterpolationMode GetDefaultInterpolationMode(void)
		{
			return msDefaultInterpolationMode;
		}

        // 设置默认4元数插值方式
        static void SetDefaultRotationInterpolationMode(RotationInterpolationMode im)
		{
			msDefaultRotationInterpolationMode = im;
		}

        // 获得默认4元数插值方式
        static RotationInterpolationMode GetDefaultRotationInterpolationMode(void)
		{
			return msDefaultRotationInterpolationMode;
		}

        /*
		// 获得节点动画序列列表
        const NodeTrackList& _GetNodeTrackList(void) const
		{
			return mNodeTrackList;
		}

        // 获得节点动画序列迭代器
        NodeTrackIterator GetNodeTrackIterator(void) const
        { 
			return NodeTrackIterator(mNodeTrackList.begin(), mNodeTrackList.end()); 
		}
        */

		// 对动画序列进行优化,删除非必须的动画序列以及关键帧
		void Optimise(bool discardIdentityNodeTracks = true);

		// 收集指定的节点动画序列
        //void _CollectIdentityNodeTracks(TrackHandleList& tracks) const;

        // 销毁指定的动画序列
        //void _DestroyNodeTracks(const TrackHandleList& tracks);

		// 克隆动画对象
		NodeAnimation* Clone(const AIRString& newName) const;
		
        // 通知关键帧是否发生了改变
        void _KeyFrameListChanged(void) { mKeyFrameTimesDirty = true; }

		// 将时间点转化为时间索引
        TimeIndex _GetTimeIndex(float timePos) const;

		virtual void Clear( void )
		{

		}

        //void SaveToXML(CMarkupSTL& xml);

        //从xml中加载
        //void LoadFromXML(CMarkupSTL& xml);

		void SetLoop( bool b )
		{
			mLoop = b;
		}

		bool GetLoop( void )
		{
			return mLoop;
		}

        void SetApplyMask(ApplyNodeMask mask);

        // 移除所有的关键帧
        virtual void RemoveAllKeyFrames(void);

        // 创建节点动画关键帧对象
        virtual AnimationKeyFrame* CreateNodeKeyFrame(float timePos);

        CSceneNode* GetSceneNode()
        {
            return m_pNode;
        }
	protected:
		// 优化节点动画序列
		//void OptimiseNodeTracks(bool discardIdentityTracks);

		// 建立全局的关键帧时间列表
		void BuildKeyFrameTimeList(void) const;

        // 下次是否需要重建插值曲线
        virtual void BuildInterpolationSplines(void) const;
    protected:
		//NodeTrackList						mNodeTrackList;							// 节点动画序列
//#if NODE_ANIM_TYPE == 0
//		AIRString							mName;									// 动画名称
//		float								mLength;								// 动画时间长度
//#endif
		InterpolationMode					mInterpolationMode;						// 向量插值方式
		RotationInterpolationMode			mRotationInterpolationMode;				// 4元数插值方式
		static InterpolationMode			msDefaultInterpolationMode;				// 默认的向量插值方式
		static RotationInterpolationMode	msDefaultRotationInterpolationMode;		// 默认的4元数插值方式
		mutable KeyFrameTimeList			mKeyFrameTimes;							// 全局关键帧时间点列表
		mutable bool						mKeyFrameTimesDirty;					// 标明是否需要重建时间点列表
		bool								mLoop;

        typedef AIRVector<AnimationKeyFrame*> KeyFrameList;
        typedef AIRVector<unsigned short>		KeyFrameIndexMap;

        KeyFrameList		mKeyFrames;			// 关键帧序列

        unsigned int m_nApplyMask;    //影响mask

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

        KeyFrameIndexMap	mKeyFrameIndexMap;	// 关键帧索引表

        CSceneNode*  m_pNode;
    };


    // 动画管理器
    class EngineExport AnimationManager
    {
    public:
        AnimationManager( void )
        {
            //mAnimationsList.clear();
        }

        ~AnimationManager( void )
        {
            DestroyAllAnimations();
        }

        static AnimationManager* GetSingleton( void )
        {
            static AnimationManager inst;
            return &inst;
        }

        // 创建动画对象
        NodeAnimation* CreateAnimation( CSceneNode* pNode, const AIRString& szName, float fLength );

        // 获得动画对象
        NodeAnimation* GetAnimation( const AIRString& szName );

        // 是否存在动画
        bool HasAnimation(const AIRString& szName) const;

        // 销毁动画
        bool DestroyAnimation(const AIRString& name);

        // 销毁所有动画
        void DestroyAllAnimations(void);

        // 创建动画状态
        //AnimationState* CreateAnimationState(const AIRString& animName);

        // 获得动画状态
        //AnimationState* GetAnimationState(const AIRString& animName) const;

        // 是否存在动画状态
        //bool HasAnimationState(const AIRString& name) const;

        // 销毁动画状态
        //void DestroyAnimationState(const AIRString& name);

        // 销毁所有动画状态
        //void DestroyAllAnimationStates(void);

        // 每帧更新所有打开的动画
        void Update( float fTimeElapse );

    protected:
        // 提交动画变换
        void _ApplySceneAnimations(void);


    protected:
        typedef std::map<AIRString, NodeAnimation*>	AnimationList;
        AnimationList		mAnimationsList;	// 动画列表
        //AnimationStateSet	mAnimationStates;	// 动画状态集

        typedef std::map<AIRString, CAIRAnimationState*> AnimationStateMap;
        AnimationStateMap m_mapAnimationStates;
    };


#define ANIMATION_MANAGER (AnimationManager::GetSingleton())
