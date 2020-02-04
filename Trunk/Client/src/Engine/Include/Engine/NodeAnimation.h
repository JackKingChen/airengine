// ***************************************************************
//  �ļ�: NodeAnimation.h
//  ����: 2010-5-22
//  ����: huangjunjie
//  ˵��: ������
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


	// ������
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
		// �ؼ�֡ID����
		typedef std::set<unsigned short>						TrackHandleList;


        // ������ֵ��ʽ
        enum InterpolationMode
        {
            // ���Բ�ֵ
            IM_LINEAR,
            // �������߲�ֵ
            IM_SPLINE
        };

        // 4Ԫ����ת�Ĳ�ֵ��ʽ
        enum RotationInterpolationMode
        {
            // ���Բ�ֵ
            RIM_LINEAR,
            // �����ֵ
            RIM_SPHERICAL
        };

        enum ApplyNodeMask
        {
            ANM_Translate = 0x0001,
            ANM_Rotate = 0x0002,
            ANM_Scale  = 0x0004,
        };
    public:
        
		// ���캯��, ��AnimationManager����������
		NodeAnimation(CSceneNode* pNode, const AIRString& name, float length);

        virtual ~NodeAnimation();
        // ÿ֡���º��ύ�ı任
        virtual void ApplyToNode(CSceneNode* node, const f32 fTimePos, float weight = 1.0, float scale = 1.0f);

        // ��ò�ֵ�ؼ�֡
        virtual void GetInterpolatedKeyFrame(const f32 fTimePos, AnimationKeyFrame* kf) const;

        // ��ò�ֵ�ؼ�֡
        virtual float GetKeyFramesAtTime(
            const TimeIndex& timeIndex, 
            AnimationKeyFrame** keyFrame1, 
            AnimationKeyFrame** keyFrame2,
            unsigned short* firstKeyIndex = 0) const;

//#if NODE_ANIM_TYPE == 0
//		// ��ö���������
//		const AIRString& GetName(void) const
//		{
//			return mName;
//		}
//
//		// ��ö����Ĳ���ʱ�䳤��
//		float GetLength(void) const
//		{
//			return mLength;
//		}
//
//		// ���ö����Ĳ���ʱ�䳤��
//		void SetLength(float len)
//		{
//			mLength = len;
//		}
//#endif

        /*
		// �����ڵ㶯�����ж���
        NodeAnimationTrack* CreateNodeTrack(unsigned short handle);

		// �����ڵ㶯�����ж���
        NodeAnimationTrack* CreateNodeTrack(unsigned short handle, CSceneNode* node);


		// ��ýڵ㶯�����ж��������
        unsigned short GetNumNodeTracks(void) const
		{
			return (unsigned short)mNodeTrackList.size();
		}

        // ��ýڵ㶯�����ж���
        NodeAnimationTrack* GetNodeTrack(unsigned short handle) const;

		// �Ƿ����idΪhandle�Ľڵ㶯�����ж���
		bool HasNodeTrack(unsigned short handle) const
		{
			return (mNodeTrackList.find(handle) != mNodeTrackList.end());
		}
		
		// ���ٽڵ㶯�����ж���
        void DestroyNodeTrack(unsigned short handle);

		// �������еĶ������ж���
        void DestroyAllTracks(void);

		// �������еĽڵ㶯�����ж���
		void DestroyAllNodeTracks(void);
        */

        // �������ļ��������µ��ڵ�任
        //void Apply(float timePos, float weight = 1.0, float scale = 1.0f);

        // ���������Ĳ�ֵ��ʽ
        void SetInterpolationMode(InterpolationMode im)
		{
			mInterpolationMode = im;
		}

        // ��������Ĳ�ֵ��ʽ
        InterpolationMode GetInterpolationMode(void) const
		{
			return mInterpolationMode;
		}

        // ����4Ԫ����ֵ��ʽ
        void SetRotationInterpolationMode(RotationInterpolationMode im)
		{
			mRotationInterpolationMode = im;
		}

        // ���4Ԫ����ֵ��ʽ
        RotationInterpolationMode GetRotationInterpolationMode(void) const
		{
			return mRotationInterpolationMode;
		}

        // ����Ĭ��������ֵ��ʽ
        static void SetDefaultInterpolationMode(InterpolationMode im)
		{
			msDefaultInterpolationMode = im;
		}

        // ���Ĭ��������ֵ��ʽ
        static InterpolationMode GetDefaultInterpolationMode(void)
		{
			return msDefaultInterpolationMode;
		}

        // ����Ĭ��4Ԫ����ֵ��ʽ
        static void SetDefaultRotationInterpolationMode(RotationInterpolationMode im)
		{
			msDefaultRotationInterpolationMode = im;
		}

        // ���Ĭ��4Ԫ����ֵ��ʽ
        static RotationInterpolationMode GetDefaultRotationInterpolationMode(void)
		{
			return msDefaultRotationInterpolationMode;
		}

        /*
		// ��ýڵ㶯�������б�
        const NodeTrackList& _GetNodeTrackList(void) const
		{
			return mNodeTrackList;
		}

        // ��ýڵ㶯�����е�����
        NodeTrackIterator GetNodeTrackIterator(void) const
        { 
			return NodeTrackIterator(mNodeTrackList.begin(), mNodeTrackList.end()); 
		}
        */

		// �Զ������н����Ż�,ɾ���Ǳ���Ķ��������Լ��ؼ�֡
		void Optimise(bool discardIdentityNodeTracks = true);

		// �ռ�ָ���Ľڵ㶯������
        //void _CollectIdentityNodeTracks(TrackHandleList& tracks) const;

        // ����ָ���Ķ�������
        //void _DestroyNodeTracks(const TrackHandleList& tracks);

		// ��¡��������
		NodeAnimation* Clone(const AIRString& newName) const;
		
        // ֪ͨ�ؼ�֡�Ƿ����˸ı�
        void _KeyFrameListChanged(void) { mKeyFrameTimesDirty = true; }

		// ��ʱ���ת��Ϊʱ������
        TimeIndex _GetTimeIndex(float timePos) const;

		virtual void Clear( void )
		{

		}

        //void SaveToXML(CMarkupSTL& xml);

        //��xml�м���
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

        // �Ƴ����еĹؼ�֡
        virtual void RemoveAllKeyFrames(void);

        // �����ڵ㶯���ؼ�֡����
        virtual AnimationKeyFrame* CreateNodeKeyFrame(float timePos);

        CSceneNode* GetSceneNode()
        {
            return m_pNode;
        }
	protected:
		// �Ż��ڵ㶯������
		//void OptimiseNodeTracks(bool discardIdentityTracks);

		// ����ȫ�ֵĹؼ�֡ʱ���б�
		void BuildKeyFrameTimeList(void) const;

        // �´��Ƿ���Ҫ�ؽ���ֵ����
        virtual void BuildInterpolationSplines(void) const;
    protected:
		//NodeTrackList						mNodeTrackList;							// �ڵ㶯������
//#if NODE_ANIM_TYPE == 0
//		AIRString							mName;									// ��������
//		float								mLength;								// ����ʱ�䳤��
//#endif
		InterpolationMode					mInterpolationMode;						// ������ֵ��ʽ
		RotationInterpolationMode			mRotationInterpolationMode;				// 4Ԫ����ֵ��ʽ
		static InterpolationMode			msDefaultInterpolationMode;				// Ĭ�ϵ�������ֵ��ʽ
		static RotationInterpolationMode	msDefaultRotationInterpolationMode;		// Ĭ�ϵ�4Ԫ����ֵ��ʽ
		mutable KeyFrameTimeList			mKeyFrameTimes;							// ȫ�ֹؼ�֡ʱ����б�
		mutable bool						mKeyFrameTimesDirty;					// �����Ƿ���Ҫ�ؽ�ʱ����б�
		bool								mLoop;

        typedef AIRVector<AnimationKeyFrame*> KeyFrameList;
        typedef AIRVector<unsigned short>		KeyFrameIndexMap;

        KeyFrameList		mKeyFrames;			// �ؼ�֡����

        unsigned int m_nApplyMask;    //Ӱ��mask

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

        KeyFrameIndexMap	mKeyFrameIndexMap;	// �ؼ�֡������

        CSceneNode*  m_pNode;
    };


    // ����������
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

        // ������������
        NodeAnimation* CreateAnimation( CSceneNode* pNode, const AIRString& szName, float fLength );

        // ��ö�������
        NodeAnimation* GetAnimation( const AIRString& szName );

        // �Ƿ���ڶ���
        bool HasAnimation(const AIRString& szName) const;

        // ���ٶ���
        bool DestroyAnimation(const AIRString& name);

        // �������ж���
        void DestroyAllAnimations(void);

        // ��������״̬
        //AnimationState* CreateAnimationState(const AIRString& animName);

        // ��ö���״̬
        //AnimationState* GetAnimationState(const AIRString& animName) const;

        // �Ƿ���ڶ���״̬
        //bool HasAnimationState(const AIRString& name) const;

        // ���ٶ���״̬
        //void DestroyAnimationState(const AIRString& name);

        // �������ж���״̬
        //void DestroyAllAnimationStates(void);

        // ÿ֡�������д򿪵Ķ���
        void Update( float fTimeElapse );

    protected:
        // �ύ�����任
        void _ApplySceneAnimations(void);


    protected:
        typedef std::map<AIRString, NodeAnimation*>	AnimationList;
        AnimationList		mAnimationsList;	// �����б�
        //AnimationStateSet	mAnimationStates;	// ����״̬��

        typedef std::map<AIRString, CAIRAnimationState*> AnimationStateMap;
        AnimationStateMap m_mapAnimationStates;
    };


#define ANIMATION_MANAGER (AnimationManager::GetSingleton())
