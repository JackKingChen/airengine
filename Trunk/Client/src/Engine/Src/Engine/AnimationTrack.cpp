#include "stdafx.h"
#include "NodeAnimation.h"
#include "AnimationKeyFrame.h"
#include "AnimationTrack.h"
#include "SceneNode.h"
#include "DebugTools.h"


struct KeyFrameTimeLess
{
	bool operator() (/*const*/ AnimationKeyFrame* kf, /*const*/ AnimationKeyFrame* kf2) const
	{
		return kf->GetTimePos() < kf2->GetTimePos();
	}
};




AnimationKeyFrame* AnimationTrack::GetKeyFrame(unsigned short index) const
{
	// If you hit this assert, then the keyframe index is out of bounds
	assert( index < (unsigned short)mKeyFrames.size() );

	return mKeyFrames[index];
}



float AnimationTrack :: GetKeyFramesAtTime(
	 const TimeIndex& timeIndex, 
	 AnimationKeyFrame** keyFrame1, 
	 AnimationKeyFrame** keyFrame2,
	 unsigned short* firstKeyIndex /*= 0*/) const
{
	// Parametric time
	// t1 = time of previous keyframe
	// t2 = time of next keyframe
	float t1, t2;

	float timePos = timeIndex.GetTimePos();

	// Find first keyframe after or on current time
    
	KeyFrameList::const_iterator i;
	if (timeIndex.HasKeyIndex())
	{
		// Global keyframe index available, map to local keyframe index directly.
		assert(timeIndex.GetKeyIndex() < mKeyFrameIndexMap.size());
		i = mKeyFrames.begin() + mKeyFrameIndexMap[timeIndex.GetKeyIndex()];
#ifdef _DEBUG
		AnimationKeyFrame timeKey(timePos);
		if (i != std::lower_bound(mKeyFrames.begin(), mKeyFrames.end(), &timeKey, KeyFrameTimeLess()))
		{
			DT_TO_MSGBOX_FMT( LT_ERROR, "Optimised key frame search failed!" );
		}
#endif
	}
	else
	{
		// Wrap time
//#if NODE_ANIM_TYPE == 0
//		float totalAnimationLength = mParent->GetLength();
//#else
		float totalAnimationLength = mParent->GetTimeRange();
//#endif
		assert(totalAnimationLength > 0.0f && "Invalid animation length!");

		while (timePos > totalAnimationLength && totalAnimationLength > 0.0f)
		{
			timePos -= totalAnimationLength;
		}

		// No global keyframe index, need to search with local keyframes.
		AnimationKeyFrame timeKey(timePos);
		i = std::lower_bound(mKeyFrames.begin(), mKeyFrames.end(), &timeKey, KeyFrameTimeLess());
	}

	if (i == mKeyFrames.end())
	{
		// There is no keyframe after this time, wrap back to first
		
//#if NODE_ANIM_TYPE == 0
//		t2 = mParent->GetLength() + (*keyFrame2)->GetTimePos();
//#else
        //++i;
        
        //*keyFrame2 = mKeyFrames.rbegin();
        *keyFrame2 = mKeyFrames.front();
        //if ((*keyFrame2)->GetTimePos() == 0.0f)
        //{
        //    int a = 0;
        //}
		t2 = mParent->GetTimeRange() + (*keyFrame2)->GetTimePos();

        --i;
        *keyFrame1 = *i;
//#endif
		// Use last keyframe as previous keyframe
		
	}
	else
	{
		//--------------------------------------------------
		//! add by hjj 2010-8-31
		//放置编辑器模式下崩溃
		if ( mKeyFrames.size() == 1 )
		{
			*keyFrame1 = *i;
			*keyFrame2 = *i;
		}
		else
		{
			if ( i == mKeyFrames.begin() )
			{
				*keyFrame1 = *i;
				*keyFrame2 = *(i + 1);
			}
			else
			{
				*keyFrame1 = *(i - 1);
				*keyFrame2 = *i;
			}
		}
		//--------------------------------------------------

		t2 = (*keyFrame2)->GetTimePos();

		// Find last keyframe before or on current time
		if (i != mKeyFrames.begin() && timePos < (*i)->GetTimePos())
		{
			--i;
		}
	}

	// Fill index of the first key
	if (firstKeyIndex)
	{
		*firstKeyIndex = std::distance(mKeyFrames.begin(), i);
	}

	

	t1 = (*keyFrame1)->GetTimePos();

	if (t1 == t2)
	{
		// Same KeyFrame (only one)
		return 0.0;
	}
	else
	{
		return (timePos - t1) / (t2 - t1);
	}
    
}



AnimationKeyFrame* AnimationTrack :: CreateKeyFrame(float timePos)
{
	AnimationKeyFrame* kf = CreateKeyFrameImpl(timePos);

    if (timePos < 0)
    {
        int a = 0;
    }
	// Insert just before upper bound
	KeyFrameList::iterator i =
		std::upper_bound(mKeyFrames.begin(), mKeyFrames.end(), kf, KeyFrameTimeLess());
	mKeyFrames.insert(i, kf);

	_KeyFrameDataChanged();
	mParent->_KeyFrameListChanged();

	return kf;
}



void AnimationTrack::RemoveAllKeyFrames(void)
{
	KeyFrameList::iterator i = mKeyFrames.begin();

	for (; i != mKeyFrames.end(); ++i)
	{
		SafeDelete( *i );
	}

	_KeyFrameDataChanged();
	mParent->_KeyFrameListChanged();

	mKeyFrames.clear();

}



void AnimationTrack::RemoveKeyFrame(unsigned short index)
{
	// If you hit this assert, then the keyframe index is out of bounds
	assert( index < (unsigned short)mKeyFrames.size() );

	KeyFrameList::iterator i = mKeyFrames.begin();

	i += index;

	SafeDelete( *i );

	mKeyFrames.erase(i);

	_KeyFrameDataChanged();
	mParent->_KeyFrameListChanged();


}




void AnimationTrack::_CollectKeyFrameTimes(AIRVector<float>& keyFrameTimes)
{
	for (KeyFrameList::const_iterator i = mKeyFrames.begin(); i != mKeyFrames.end(); ++i)
	{
		float timePos = (*i)->GetTimePos();
        if (timePos < 0)  //add by liangairan for debug
        {
            int a = 0;
        }

		AIRVector<float>::iterator it = std::lower_bound(keyFrameTimes.begin(), keyFrameTimes.end(), timePos);
		if (it == keyFrameTimes.end() || *it != timePos)
		{
			keyFrameTimes.insert(it, timePos);
		}
	}
}



void AnimationTrack::_BuildKeyFrameIndexMap(const AIRVector<float>& keyFrameTimes)
{
	// Pre-allocate memory
	mKeyFrameIndexMap.resize(keyFrameTimes.size() + 1);

	size_t i = 0, j = 0;
	while (j <= keyFrameTimes.size())
	{
		mKeyFrameIndexMap[j] = static_cast<unsigned short>(i);
		while (i < mKeyFrames.size() && mKeyFrames[i]->GetTimePos() <= keyFrameTimes[j])
		{
			++i;
		}
		++j;
	}
}


void AnimationTrack::PopulateClone(AnimationTrack* clone) const
{
	for (KeyFrameList::const_iterator i = mKeyFrames.begin(); i != mKeyFrames.end(); ++i)
	{
		AnimationKeyFrame* clonekf = (*i)->_Clone(clone);
		clone->mKeyFrames.push_back(clonekf);
	}
}





//------------------------------------------------------------------------------------

NodeAnimationTrack::NodeAnimationTrack(NodeAnimation* parent, unsigned short handle)
: AnimationTrack(parent, handle)
, mTargetNode(0)
, mSplines(0)
, mSplineBuildNeeded(false)
, mUseShortestRotationPath(true)
{
    m_nApplyMask = ANM_Translate | ANM_Rotate | ANM_Scale;
}


NodeAnimationTrack::NodeAnimationTrack(NodeAnimation* parent, unsigned short handle, CSceneNode* targetNode)
: AnimationTrack(parent, handle)
, mTargetNode(targetNode)
, mSplines(0)
, mSplineBuildNeeded(false)
, mUseShortestRotationPath(true)
{
}


NodeAnimationTrack::~NodeAnimationTrack()
{
	SafeDelete( mSplines );
}


void NodeAnimationTrack::GetInterpolatedKeyFrame(const TimeIndex& timeIndex, AnimationKeyFrame* kf) const
{
	//if (mListener)
	//{
	//	if (mListener->GetInterpolatedKeyFrame(this, timeIndex, kf))
	//		return;
	//}

	AnimationKeyFrame* kret = static_cast<AnimationKeyFrame*>(kf);

	// Keyframe pointers
	AnimationKeyFrame *kBase1, *kBase2;
	AnimationKeyFrame *k1, *k2;
	unsigned short firstKeyIndex;

	float t = this->GetKeyFramesAtTime(timeIndex, &kBase1, &kBase2, &firstKeyIndex);
	k1 = static_cast<AnimationKeyFrame*>(kBase1);
	k2 = static_cast<AnimationKeyFrame*>(kBase2);

	if (t == 0.0)
	{
		// Just use k1
		kret->SetRotation(k1->GetRotation());
		kret->SetTranslate(k1->GetTranslate());
		kret->SetScale(k1->GetScale());
	}
	else
	{
		// Interpolate by t
		NodeAnimation::InterpolationMode im = mParent->GetInterpolationMode();
		NodeAnimation::RotationInterpolationMode rim = mParent->GetRotationInterpolationMode();
		Vector3Df base;
		switch(im)
		{
		case NodeAnimation::IM_LINEAR:
			// Interpolate linearly
			// Rotation
			// Interpolate to nearest rotation if mUseShortestRotationPath set
            
			//if (rim == NodeAnimation::RIM_LINEAR)
			//{
			//	kret->SetRotation( Quaternion::nlerp(t, k1->GetRotation(), k2->GetRotation(), mUseShortestRotationPath) );
			//}
			//else //if (rim == Animation::RIM_SPHERICAL)
			//{
			//	kret->SetRotation( Quaternion::Slerp(t, k1->GetRotation(), k2->GetRotation(), mUseShortestRotationPath) );
			//}
            
            kret->m_rotation.Slerp(k1->GetRotation(), k2->GetRotation(), t);
            

			// Translation
			base = k1->GetTranslate();
			kret->SetTranslate( base + ((k2->GetTranslate() - base) * t) );

			// Scale
			base = k1->GetScale();
			kret->SetScale( base + ((k2->GetScale() - base) * t) );
			break;

		case NodeAnimation::IM_SPLINE:
			// Spline interpolation

			// Build splines if required
			if (mSplineBuildNeeded)
			{
				buildInterpolationSplines();
			}

			// Rotation, take mUseShortestRotationPath into account
			kret->SetRotation( mSplines->rotationSpline.Interpolate(firstKeyIndex, t, mUseShortestRotationPath) );

			// Translation
			kret->SetTranslate( mSplines->positionSpline.Interpolate(firstKeyIndex, t) );

			// Scale
			kret->SetScale( mSplines->scaleSpline.Interpolate(firstKeyIndex, t) );

			break;
		}

	}
}


void NodeAnimationTrack::Apply(const TimeIndex& timeIndex, float weight, float scale)
{
	ApplyToNode(mTargetNode, timeIndex, weight, scale);

}


void NodeAnimationTrack::ApplyToNode(CSceneNode* node, const TimeIndex& timeIndex, float weight,float scl)
{
	// Nothing to do if no keyframes or zero weight or no node
	if (mKeyFrames.empty() || !weight || !node)
		return;

	AnimationKeyFrame kf(timeIndex.GetTimePos());
	GetInterpolatedKeyFrame(timeIndex, &kf);

	// add to existing. Weights are not relative, but treated as absolute multipliers for the animation
    if (m_nApplyMask & ANM_Translate)
    {
        Vector3Df translate = kf.GetTranslate() * weight * scl;
		//node->Translate(translate);
		node->SetPosition( translate );
    }
	

	// interpolate between no-rotation and full rotation, to point 'weight', so 0 = no rotate, 1 = full
    if (m_nApplyMask & ANM_Rotate)
    {
        /*
        Quaternion rotate;
        NodeAnimation::RotationInterpolationMode rim = mParent->GetRotationInterpolationMode();
        if (rim == NodeAnimation::RIM_LINEAR)
        {
            rotate = Quaternion::nlerp(weight, Quaternion::IDENEITY, kf.GetRotation(), mUseShortestRotationPath);
        }
        else //if (rim == Animation::RIM_SPHERICAL)
        {
            rotate = Quaternion::Slerp(weight, Quaternion::IDENEITY, kf.GetRotation(), mUseShortestRotationPath);
        }
        */
        //node->Rotate(kf.m_rotation, CSceneNode::TS_LOCAL);
        node->SetRotation(kf.m_rotation);
        //DT_TO_DBGSTR(LT_COMMAND, GetString("node rotation : (%.4f, %.4f, %.4f, %.4f)\n", kf.m_rotation.x,
        //    kf.m_rotation.y, kf.m_rotation.z, kf.m_rotation.w));
    }
	
    if (m_nApplyMask & ANM_Scale)
    {
        Vector3Df scale = kf.GetScale();
        // Not sure how to modify scale for cumulative anims... leave it alone
        //scale = ((Vector3::UNIT_SCALE - kf.getScale()) * weight) + Vector3::UNIT_SCALE;
        if (scl != 1.0f && scale != Vector3Df::UNIT_SCALE)
        {
            scale = Vector3Df::UNIT_SCALE + (scale - Vector3Df::UNIT_SCALE) * scl;
        }
        node->SetScale(scale);
    }
	

}

void NodeAnimationTrack::SetApplyMask(unsigned int nMask)
{
    m_nApplyMask = nMask;
}

void NodeAnimationTrack::buildInterpolationSplines(void) const
{
	// Allocate splines if not exists
	if (!mSplines)
	{
		mSplines = new Splines();
	}

	// Cache to register for optimisation
	Splines* splines = mSplines;

	// Don't calc automatically, do it on request at the end
	splines->positionSpline.SetAutoCalculate(false);
	splines->rotationSpline.SetAutoCalculate(false);
	splines->scaleSpline.SetAutoCalculate(false);

	splines->positionSpline.Clear();
	splines->rotationSpline.Clear();
	splines->scaleSpline.Clear();

	KeyFrameList::const_iterator i, iend;
	iend = mKeyFrames.end(); // precall to avoid overhead
	for (i = mKeyFrames.begin(); i != iend; ++i)
	{
		AnimationKeyFrame* kf = static_cast<AnimationKeyFrame*>(*i);
		splines->positionSpline.AddPoint(kf->GetTranslate());
		splines->rotationSpline.AddPoint(kf->GetRotation());
		splines->scaleSpline.AddPoint(kf->GetScale());
	}

	splines->positionSpline.RecalcTangents();
	splines->rotationSpline.RecalcTangents();
	splines->scaleSpline.RecalcTangents();


	mSplineBuildNeeded = false;
}



bool NodeAnimationTrack::HasNonZeroKeyFrames(void) const
{
	KeyFrameList::const_iterator i = mKeyFrames.begin();
	for (; i != mKeyFrames.end(); ++i)
	{
		// look for keyframes which have any component which is non-zero
		// Since exporters can be a little inaccurate sometimes we use a
		// tolerance value rather than looking for nothing
		AnimationKeyFrame* kf = static_cast<AnimationKeyFrame*>(*i);
		Vector3Df trans = kf->GetTranslate();
		Vector3Df scale = kf->GetScale();
		Vector3Df axis;
		float angle;
		kf->GetRotation().ToAngleAxis(angle, axis);
		float tolerance = 1e-3f;
		if (!trans.Equals(Vector3Df::ZERO, tolerance) ||
			!scale.Equals(Vector3Df::UNIT_SCALE, tolerance) ||
			!AIR::Equals(angle, 0.0f, tolerance))
		{
			return true;
		}

	}

	return false;
}



void NodeAnimationTrack::Optimise(void)
{
	// Eliminate duplicate keyframes from 2nd to penultimate keyframe
	// NB only eliminate middle keys from sequences of 5+ identical keyframes
	// since we need to preserve the boundary keys in place, and we need
	// 2 at each end to preserve tangents for spline interpolation
	Vector3Df lasttrans;
	Vector3Df lastscale;
	Quaternion lastorientation;
	KeyFrameList::iterator i = mKeyFrames.begin();
	float quatTolerance(1e-3f);
	std::list<unsigned short> removeList;
	unsigned short k = 0;
	unsigned short dupKfCount = 0;
	for (; i != mKeyFrames.end(); ++i, ++k)
	{
		AnimationKeyFrame* kf = static_cast<AnimationKeyFrame*>(*i);
		Vector3Df newtrans = kf->GetTranslate();
		Vector3Df newscale = kf->GetScale();
		Quaternion neworientation = kf->GetRotation();
		// Ignore first keyframe; now include the last keyframe as we eliminate
		// only k-2 in a group of 5 to ensure we only eliminate middle keys
		if (i != mKeyFrames.begin() &&
			newtrans.Equals(lasttrans) &&
			newscale.Equals(lastscale) &&
			neworientation.Equals(lastorientation, quatTolerance))
		{
			++dupKfCount;

			// 4 indicates this is the 5th duplicate keyframe
			if (dupKfCount == 4)
			{
				// remove the 'middle' keyframe
				removeList.push_back(k-2);
				--dupKfCount;
			}
		}
		else
		{
			// reset
			dupKfCount = 0;
			lasttrans = newtrans;
			lastscale = newscale;
			lastorientation = neworientation;
		}
	}

	// Now remove keyframes, in reverse order to avoid index revocation
	std::list<unsigned short>::reverse_iterator r = removeList.rbegin();
	for (; r!= removeList.rend(); ++r)
	{
		RemoveKeyFrame(*r);
	}


}



AnimationKeyFrame* NodeAnimationTrack::CreateKeyFrameImpl(float time)
{
	return new AnimationKeyFrame(time);
}


AnimationKeyFrame* NodeAnimationTrack::CreateNodeKeyFrame(float timePos)
{
	return static_cast<AnimationKeyFrame*>(CreateKeyFrame(timePos));
}


AnimationKeyFrame* NodeAnimationTrack::GetNodeKeyFrame(unsigned short index) const
{
	return static_cast<AnimationKeyFrame*>(GetKeyFrame(index));
}


NodeAnimationTrack* NodeAnimationTrack::_Clone(NodeAnimation* newParent) const
{
    /*
	NodeAnimationTrack* newTrack = newParent->CreateNodeTrack(mHandle, mTargetNode);
	newTrack->mUseShortestRotationPath = mUseShortestRotationPath;
	PopulateClone(newTrack);
    */
	return 0; //newTrack;
}