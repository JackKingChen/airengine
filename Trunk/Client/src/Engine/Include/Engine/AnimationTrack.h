// ***************************************************************
//  �ļ�: AnimationTrack.h
//  ����: 2010-5-22
//  ����: huangjunjie
//  ˵��: ��������
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


    // ʱ������
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
		float				mTimePos;					// ʱ���
		u32					mKeyIndex;					// ����
		static const u32	INVALID_KEY_INDEX = (u32)-1;// �Ƿ��Ĺؼ�֡����ֵ
    };


    // ��������
	class EngineExport AnimationTrack
    {
    public:
		// �������м�����
		class EngineExport Listener
		{
		public:
			virtual ~Listener() {}
			// ��ò�ֵ�Ĺؼ�֡
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

		// ��ùؼ�֡������
		unsigned short GetHandle(void) const 
		{ 
			return mHandle; 
		}

        // ��ö����Ĺؼ�֡����
        virtual unsigned short GetNumKeyFrames(void) const
		{
			return (unsigned short)mKeyFrames.size();
		}

        // ��ùؼ�֡
        virtual AnimationKeyFrame* GetKeyFrame(unsigned short index) const;

        // ��ò�ֵ�ؼ�֡
        virtual float GetKeyFramesAtTime(
			const TimeIndex& timeIndex, 
			AnimationKeyFrame** keyFrame1, 
			AnimationKeyFrame** keyFrame2,
            unsigned short* firstKeyIndex = 0) const;

        // �����ؼ�֡
        virtual AnimationKeyFrame* CreateKeyFrame(float timePos);

        // �Ƴ��ؼ�֡
        virtual void RemoveKeyFrame(unsigned short index);

        // �Ƴ����еĹؼ�֡
        virtual void RemoveAllKeyFrames(void);


        // ��ò�ֵ�ؼ�֡
        virtual void GetInterpolatedKeyFrame(const TimeIndex& timeIndex, AnimationKeyFrame* kf) const = 0;

        // ÿ֡���º��ύ�����任
        virtual void Apply(const TimeIndex& timeIndex, float weight = 1.0, float scale = 1.0f) = 0;

		// �ڲ�����,�����ؼ�֡�����б�,��Ҫ�ؽ�
		virtual void _KeyFrameDataChanged(void) const {}

		// �������еĹؼ�֡���ǺϷ����õ�
		virtual bool HasNonZeroKeyFrames(void) const { return true; }

		// �Ż��ؼ�֡
		virtual void Optimise(void) {}

		// �ռ��ؼ�֡
		virtual void _CollectKeyFrameTimes(AIRVector<float>& keyFrameTimes);

		// �����ؼ�֡������
		virtual void _BuildKeyFrameIndexMap(const AIRVector<float>& keyFrameTimes);

		// ���ü�����
		virtual void SetListener(Listener* l) { mListener = l; }

		// ��ø���������
		NodeAnimation *GetParent() const { return mParent; }


	protected:
		// �����ؼ�֡
		virtual AnimationKeyFrame* CreateKeyFrameImpl(float time) = 0;

		// ��¡
		virtual void PopulateClone(AnimationTrack* clone) const;

    protected:
		typedef AIRVector<AnimationKeyFrame*> KeyFrameList;
		typedef AIRVector<unsigned short>		KeyFrameIndexMap;

        KeyFrameList		mKeyFrames;			// �ؼ�֡����
        NodeAnimation*		mParent;			// ��������
		unsigned short		mHandle;			// ����
		Listener*			mListener;			// ������
        KeyFrameIndexMap	mKeyFrameIndexMap;	// �ؼ�֡������

	};







	// �ڵ㶯������
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

        // �����ڵ㶯���ؼ�֡����
        virtual AnimationKeyFrame* CreateNodeKeyFrame(float timePos);

		// ��ñ任�Ľڵ����
		virtual CSceneNode* GetAssociatedNode(void) const
		{
			return mTargetNode;
		}

		// ���ñ任�Ľڵ����
		virtual void SetAssociatedNode(CSceneNode* node)
		{
			mTargetNode = node;
		}

		// ÿ֡���º��ύ�ı任
		virtual void ApplyToNode(CSceneNode* node, const TimeIndex& timeIndex, float weight = 1.0, float scale = 1.0f);

		// ������ת�ļ��㷽ʽ
		virtual void SetUseShortestRotationPath(bool useShortestPath)
		{
			mUseShortestRotationPath = useShortestPath ;
		}

		// ��ȡ��ת�ļ��㷽ʽ
		virtual bool GetUseShortestRotationPath() const
		{
			return mUseShortestRotationPath ;
		}

		// ��ò�ֵ�ؼ�֡
        virtual void GetInterpolatedKeyFrame(const TimeIndex& timeIndex, AnimationKeyFrame* kf) const;

		// ÿ֡���º��ύ�ı任
		virtual void Apply(const TimeIndex& timeIndex, float weight = 1.0, float scale = 1.0f);

		// �����ؼ�֡���ݷ����仯
		void _KeyFrameDataChanged(void) const
		{
			mSplineBuildNeeded = true;
		}

		// ��ýڵ㶯���ؼ�֡
		virtual AnimationKeyFrame* GetNodeKeyFrame(unsigned short index) const;


		// �Ƿ����йؼ�֡���Ϸ���Ч
		virtual bool HasNonZeroKeyFrames(void) const;

		// �Ż��ؼ�֡
		virtual void Optimise(void);

		// ��¡
		NodeAnimationTrack* _Clone(NodeAnimation* newParent) const;

        //����Ӱ��mask
        void  SetApplyMask(unsigned int nMask);
		
	protected:
		// �����ؼ�֡
		AnimationKeyFrame* CreateKeyFrameImpl(float time);

		// �´��Ƿ���Ҫ�ؽ���ֵ����
		virtual void buildInterpolationSplines(void) const;


	protected:
        // �������߶���
        struct Splines
        {
		    VertexSpline		positionSpline;
		    VertexSpline		scaleSpline;
		    RotationalSpline	rotationSpline;
        };

		CSceneNode*				mTargetNode;				// �����ڵ�
		mutable Splines*		mSplines;					// ��ֵ���߶���
		mutable bool			mSplineBuildNeeded;			// �Ƿ���Ҫ�ؽ���ֵ����
		mutable bool			mUseShortestRotationPath;	// ��ת���㷽ʽ

        unsigned int m_nApplyMask;    //Ӱ��mask
	};



#endif //! __AnimationTrack_H__