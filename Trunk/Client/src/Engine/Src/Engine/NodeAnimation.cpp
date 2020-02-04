#include "stdafx.h"
#include "NodeAnimation.h"
#include "AnimationKeyFrame.h"
#include "AnimationTrack.h"
#include "SceneNode.h"
#include "DebugTools.h"
#include "AIRAnimation.h"
#include "AIRAnimationState.h"

struct KeyFrameTimeLess
{
    bool operator() (/*const*/ AnimationKeyFrame* kf, /*const*/ AnimationKeyFrame* kf2) const
    {
        return kf->GetTimePos() < kf2->GetTimePos();
    }
};


NodeAnimation::InterpolationMode			NodeAnimation::msDefaultInterpolationMode			= NodeAnimation::IM_LINEAR;
NodeAnimation::RotationInterpolationMode	NodeAnimation::msDefaultRotationInterpolationMode	= NodeAnimation::RIM_LINEAR;

//#if NODE_ANIM_TYPE == 0 
//NodeAnimation::NodeAnimation(const AIRString& name, float length)
//: mName(name)
//, mLength(length)
//, mInterpolationMode(msDefaultInterpolationMode)
//, mRotationInterpolationMode(msDefaultRotationInterpolationMode)
//, mKeyFrameTimesDirty(false)
//{
//}
//#else
NodeAnimation::NodeAnimation(CSceneNode* pNode, const AIRString& name, float length)
: mInterpolationMode(msDefaultInterpolationMode)
, mRotationInterpolationMode(msDefaultRotationInterpolationMode)
, mKeyFrameTimesDirty(false)
, m_pNode(pNode)
{
	SetName( name.c_str() );
	SetTimeRange( length );
	mLoop = false;
    m_nApplyMask = ANM_Translate | ANM_Rotate | ANM_Scale;
}
//#endif



NodeAnimation::~NodeAnimation()
{
	//DestroyAllTracks();
}

void NodeAnimation::ApplyToNode(CSceneNode* node, const f32 fTimePos, float weight,float scl)
{
    // Nothing to do if no keyframes or zero weight or no node
    if (mKeyFrames.empty() || !weight || !node)
        return;

    AnimationKeyFrame kf(fTimePos);
    GetInterpolatedKeyFrame(fTimePos, &kf);

    // add to existing. Weights are not relative, but treated as absolute multipliers for the animation
    if (m_nApplyMask & ANM_Translate)
    {
        Vector3Df translate = kf.GetTranslate() * weight * scl;
        node->Translate(translate);
        //node->SetPosition( translate );
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
        //node->SetRotation(kf.m_rotation);
        
        node->Rotate(kf.m_rotation);
        /*
        f32 fAngle;
        Vector3Df axis;
        kf.m_rotation.ToAngleAxis(fAngle, axis);
        
        DT_TO_DBGSTR(LT_COMMAND, GetString("keyframe rotation axis: (%.4f, %.4f, %.4f) angle:%.4f\n", axis.x,
            axis.y, axis.z, fAngle));

        node->GetRotation().ToAngleAxis(fAngle, axis);
        DT_TO_DBGSTR(LT_COMMAND, GetString("node rotation axis: (%.4f, %.4f, %.4f) angle:%.4f\n", axis.x,
            axis.y, axis.z, fAngle));

        DT_TO_DBGSTR(LT_COMMAND, GetString("node rotation: (%.4f, %.4f, %.4f, %.4f)\n", node->GetRotation().x,
            node->GetRotation().y, node->GetRotation().z, node->GetRotation().w));
            */
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
        //node->SetScale(scale);
        node->Scale(scale);
    }
}

void NodeAnimation::GetInterpolatedKeyFrame(const f32 fTimePos, AnimationKeyFrame* kf) const
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

    float t = this->GetKeyFramesAtTime(fTimePos, &kBase1, &kBase2, &firstKeyIndex);
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
        NodeAnimation::InterpolationMode im = GetInterpolationMode();
        NodeAnimation::RotationInterpolationMode rim = GetRotationInterpolationMode();
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

            //kret->m_rotation.Slerp(k1->GetRotation(), k2->GetRotation(), t);
            kret->m_rotation = Quaternion::nlerp(t, k1->GetRotation(), k2->GetRotation(), true);
            //kret->m_rotation = Quaternion::Slerp(t, k1->GetRotation(), k2->GetRotation(), true);

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
                BuildInterpolationSplines();
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

void NodeAnimation::SetApplyMask(ApplyNodeMask mask)
{
    m_nApplyMask = mask;
}

/*
NodeAnimationTrack* NodeAnimation::CreateNodeTrack(unsigned short handle)
{
	if (HasNodeTrack(handle))
	{
        return mNodeTrackList[handle];
		DT_TO_MSGBOX_FMT( LT_ERROR, GetString("Node track with the specified handle [%d]  already exists", handle) );
	}

	NodeAnimationTrack* ret = new NodeAnimationTrack(this, handle);

	mNodeTrackList[handle] = ret;
	return ret;
}


NodeAnimationTrack* NodeAnimation::CreateNodeTrack(unsigned short handle, CSceneNode* node)
{
	NodeAnimationTrack* ret = CreateNodeTrack(handle);

	ret->SetAssociatedNode(node);

	return ret;
}


NodeAnimationTrack* NodeAnimation::GetNodeTrack(unsigned short handle) const
{
	NodeTrackList::const_iterator i = mNodeTrackList.find(handle);

	if (i == mNodeTrackList.end())
	{
		//DT_TO_MSGBOX_FMT( LT_WARN, GetString("Cannot find node track with the specified handle [%d]", handle) );
		return NULL;
	}

	return i->second;
}


void NodeAnimation::DestroyNodeTrack(unsigned short handle)
{
	NodeTrackList::iterator i = mNodeTrackList.find(handle);

	if (i != mNodeTrackList.end())
	{
		SafeDelete(i->second);
		mNodeTrackList.erase(i);
		_KeyFrameListChanged();
	}
}

void NodeAnimation::DestroyAllNodeTracks(void)
{
	NodeTrackList::iterator i;
	for (i = mNodeTrackList.begin(); i != mNodeTrackList.end(); ++i)
	{
		SafeDelete(i->second);
	}
	mNodeTrackList.clear();
	_KeyFrameListChanged();
}



void NodeAnimation::DestroyAllTracks(void)
{
	DestroyAllNodeTracks();
}


void NodeAnimation::Apply(float timePos, float weight, float scale)
{
	// Calculate time index for fast keyframe search
	TimeIndex timeIndex = _GetTimeIndex(timePos);

	NodeTrackList::iterator i;
	for (i = mNodeTrackList.begin(); i != mNodeTrackList.end(); ++i)
	{
		i->second->Apply(timeIndex, weight, scale);
	}
}
*/

void NodeAnimation::Optimise(bool discardIdentityNodeTracks)
{
	//OptimiseNodeTracks(discardIdentityNodeTracks);
}

/*
void NodeAnimation::_CollectIdentityNodeTracks(TrackHandleList& tracks) const
{
	NodeTrackList::const_iterator i, iend;
	iend = mNodeTrackList.end();
	for (i = mNodeTrackList.begin(); i != iend; ++i)
	{
		const NodeAnimationTrack* track = i->second;
		if (track->HasNonZeroKeyFrames())
		{
			tracks.erase(i->first);
		}
	}
}


void NodeAnimation::_DestroyNodeTracks(const TrackHandleList& tracks)
{
	TrackHandleList::const_iterator t, tend;
	tend = tracks.end();
	for (t = tracks.begin(); t != tend; ++t)
	{
		DestroyNodeTrack(*t);
	}
}


void NodeAnimation::OptimiseNodeTracks(bool discardIdentityTracks)
{
	// Iterate over the node tracks and identify those with no useful keyframes
	std::list<unsigned short> tracksToDestroy;
	NodeTrackList::iterator i;
	for (i = mNodeTrackList.begin(); i != mNodeTrackList.end(); ++i)
	{
		NodeAnimationTrack* track = i->second;
		if (discardIdentityTracks && !track->HasNonZeroKeyFrames())
		{
			// mark the entire track for destruction
			tracksToDestroy.push_back(i->first);
		}
		else
		{
			track->Optimise();
		}

	}

	// Now destroy the tracks we marked for death
	for(std::list<unsigned short>::iterator h = tracksToDestroy.begin();
		h != tracksToDestroy.end(); ++h)
	{
		DestroyNodeTrack(*h);
	}
}
*/


NodeAnimation* NodeAnimation::Clone(const AIRString& newName) const
{
//#if NODE_ANIM_TYPE == 0
//	NodeAnimation* newAnim = new NodeAnimation(newName, mLength);
//#else
	NodeAnimation* newAnim = new NodeAnimation(m_pNode, newName, this->m_fTimeRange);
//#endif
	newAnim->mInterpolationMode = mInterpolationMode;
	newAnim->mRotationInterpolationMode = mRotationInterpolationMode;
    newAnim->m_nApplyMask = m_nApplyMask;

	// Clone all tracks
    /*
	for (NodeTrackList::const_iterator i = mNodeTrackList.begin();
		i != mNodeTrackList.end(); ++i)
	{
		i->second->_Clone(newAnim);
	}
    */

	newAnim->_KeyFrameListChanged();
	return newAnim;

}



TimeIndex NodeAnimation::_GetTimeIndex(float timePos) const
{
	// Uncomment following statement for work as previous
	//return timePos;

	// Build keyframe time list on demand
	if (mKeyFrameTimesDirty)
	{
		BuildKeyFrameTimeList();
	}

	// Wrap time
//#if NODE_ANIM_TYPE == 0
//	float totalAnimationLength = mLength;
//#else
	float totalAnimationLength = this->m_fTimeRange;
//#endif

	while (timePos > totalAnimationLength && totalAnimationLength > 0.0f)
	{
		timePos -= totalAnimationLength;
	}

	// Search for global index
	KeyFrameTimeList::iterator it =	std::lower_bound(mKeyFrameTimes.begin(), mKeyFrameTimes.end(), timePos);

	return TimeIndex(timePos, std::distance(mKeyFrameTimes.begin(), it));
}



void NodeAnimation::BuildKeyFrameTimeList(void) const
{
    /*
	NodeTrackList::const_iterator i;

	// Clear old keyframe times
	mKeyFrameTimes.clear();

	// Collect all keyframe times from each track
	for (i = mNodeTrackList.begin(); i != mNodeTrackList.end(); ++i)
	{
		i->second->_CollectKeyFrameTimes(mKeyFrameTimes);
	}

	// Build global index to local index map for each track
	for (i = mNodeTrackList.begin(); i != mNodeTrackList.end(); ++i)
	{
		i->second->_BuildKeyFrameIndexMap(mKeyFrameTimes);
	}
    */

	// Reset dirty flag
	mKeyFrameTimesDirty = false;
}

/*
void NodeAnimation::SaveToXML(CMarkupSTL& xml)
{
    xml.AddElem("NodeAnimation");
	xml.AddAttrib("Name", this->GetName().c_str() );
    xml.AddAttrib("TimeLength", GetString("%f", m_fTimeRange));
    xml.AddAttrib("FrameRate", GetString("%f", m_fFrameRate));
    xml.AddAttrib("InterpolationMode", mInterpolationMode == IM_LINEAR ? "IM_LINEAR" : "IM_SPLINE");
	if ( mLoop == true )
	{
		xml.AddAttrib( "Loop", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "Loop", "FALSE" );
	}

    xml.IntoElem();
    NodeTrackList::iterator itEnd = mNodeTrackList.end();
    for (NodeTrackList::iterator it = mNodeTrackList.begin(); it != mNodeTrackList.end(); ++it)
    {
        NodeAnimationTrack* pTrack = it->second;
        xml.AddElem("AnimationTrack");
        xml.AddAttrib("Handle", pTrack->GetHandle());
        xml.IntoElem();
        
        for (int i = 0; i < pTrack->GetNumKeyFrames(); ++i)
        {
            AnimationKeyFrame* pKeyFrame = pTrack->GetNodeKeyFrame(i);
            xml.AddElem("Keyframe");
            xml.AddAttrib("timepos", GetString("%.2f", pKeyFrame->GetTimePos()));
            xml.AddAttrib("translate", GetString("%.4f %.4f %.4f", pKeyFrame->GetTranslate().x,
                pKeyFrame->GetTranslate().y, pKeyFrame->GetTranslate().z));
            xml.AddAttrib("rotation", GetString("%.4f %.4f %.4f %.4f", pKeyFrame->GetRotation().x,
                pKeyFrame->GetRotation().y, pKeyFrame->GetRotation().z, pKeyFrame->GetRotation().w));
            xml.AddAttrib("scale", GetString("%.4f %.4f %.4f", pKeyFrame->GetScale().x, pKeyFrame->GetScale().y, pKeyFrame->GetScale().z));
        }
        
        xml.OutOfElem();
    }
    
    xml.OutOfElem();
}

void NodeAnimation::LoadFromXML(CMarkupSTL& xml)
{
    AIRString strValue = xml.GetAttrib("FrameRate");
    if (strValue.length() > 0)
    {
        m_fFrameRate = atof(strValue.c_str());
    }

    strValue = xml.GetAttrib("InterpolationMode");
    if (strValue.length() > 0)
    {
        mInterpolationMode = strValue == "IM_LINEAR" ? IM_LINEAR : IM_SPLINE;
    }

	strValue = xml.GetAttrib("Loop");
	if ( strValue.length() )
	{
		if ( strValue == "TRUE" )
		{
			mLoop = true;
		}
		else
		{
			mLoop = false;
		}
	}

    xml.IntoElem();
    while (xml.FindElem("AnimationTrack"))
    {
        strValue = xml.GetAttrib("handle");
        unsigned int nHandle = atoi(strValue.c_str());
        xml.IntoElem();
        NodeAnimationTrack* pTrack = CreateNodeTrack(nHandle);
        
        while (xml.FindElem("Keyframe"))
        {
            //AnimationKeyFrame* pKeyframe = new AnimationKeyFrame;
            strValue = xml.GetAttrib("timepos");
            AnimationKeyFrame* pKeyframe = pTrack->CreateKeyFrame(atof(strValue.c_str()));
            strValue = xml.GetAttrib("translate");
            Vector3Df translate;
            sscanf(strValue.c_str(), "%f %f %f", &translate.x, &translate.y, &translate.z);
            pKeyframe->SetTranslate(translate);

            Quaternion rotation;
            strValue = xml.GetAttrib("rotation");
            sscanf(strValue.c_str(), "%f %f %f %f", &rotation.x, &rotation.y, &rotation.z, &rotation.w);
            pKeyframe->SetRotation(rotation);

            Vector3Df scale;
            strValue = xml.GetAttrib("scale");
            sscanf(strValue.c_str(), "%f %f %f", &scale.x, &scale.y, &scale.z);
            pKeyframe->SetScale(scale);

        }
        xml.OutOfElem();
    }
    xml.OutOfElem();
}
*/

float NodeAnimation :: GetKeyFramesAtTime(
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
        float totalAnimationLength = GetTimeRange();
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
        t2 = GetTimeRange() + (*keyFrame2)->GetTimePos();

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

void NodeAnimation::BuildInterpolationSplines(void) const
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

void NodeAnimation::RemoveAllKeyFrames(void)
{
    KeyFrameList::iterator i = mKeyFrames.begin();

    for (; i != mKeyFrames.end(); ++i)
    {
        SafeDelete( *i );
    }

    //_KeyFrameDataChanged();
    _KeyFrameListChanged();

    mKeyFrames.clear();

}

AnimationKeyFrame* NodeAnimation::CreateNodeKeyFrame(float timePos)
{
    AnimationKeyFrame* kf = new AnimationKeyFrame(timePos);

    if (timePos < 0)
    {
        int a = 0;
    }
    // Insert just before upper bound
    KeyFrameList::iterator i =
        std::upper_bound(mKeyFrames.begin(), mKeyFrames.end(), kf, KeyFrameTimeLess());
    mKeyFrames.insert(i, kf);

    //_KeyFrameDataChanged();
    _KeyFrameListChanged();

    return kf;
}




NodeAnimation* AnimationManager::CreateAnimation( CSceneNode* pNode, const AIRString& szName, float fLength )
{
    // Check name not used
    AnimationList::iterator it = mAnimationsList.find(szName);
    if ( it != mAnimationsList.end())
    {
        DT_TO_MSGBOX_FMT( LT_WARN, GetString("An animation with the name [%s]  already exists.", szName.c_str()) );
        return it->second;
    }

    NodeAnimation* pAnim = new NodeAnimation(pNode, szName, fLength);
    mAnimationsList[szName] = pAnim;
    return pAnim;
}

NodeAnimation* AnimationManager::GetAnimation( const AIRString& szName )
{
    AnimationList::iterator it = mAnimationsList.find(szName);
    if ( it != mAnimationsList.end())
    {
        return it->second;
    }
    DT_TO_MSGBOX_FMT( LT_WARN, GetString("Cannot find animation with name [%s].", szName.c_str()) );
    return NULL;
}


bool AnimationManager::HasAnimation(const AIRString& szName) const
{
    return ( mAnimationsList.find( szName ) == mAnimationsList.end() );
}

bool AnimationManager::DestroyAnimation(const AIRString& name)
{
    // Also destroy any animation states referencing this animation
    //mAnimationStates.RemoveAnimationState(name);

    AnimationList::iterator i = mAnimationsList.find(name);
    if (i == mAnimationsList.end())
    {
        DT_TO_MSGBOX_FMT( LT_WARN, GetString("Cannot find animation with name [%s].", name.c_str()) );
        return false;
    }

    // Free memory
    SafeDelete( i->second );
    mAnimationsList.erase(i);
    return true;
}

void AnimationManager::DestroyAllAnimations(void)
{
    // Destroy all states too, since they cannot reference destroyed animations
    //DestroyAllAnimationStates();

    AnimationList::iterator i;
    for (i = mAnimationsList.begin(); i != mAnimationsList.end(); ++i)
    {
        // destroy
        SafeDelete( i->second );
    }
    mAnimationsList.clear();
}

/*
AnimationState* AnimationManager::CreateAnimationState(const AIRString& animName)
{
    // Get animation, this will throw an exception if not found
    NodeAnimation* anim = GetAnimation(animName);

    // Create new state
    //#if NODE_ANIM_TYPE == 0
    //	return mAnimationStates.CreateAnimationState(animName, 0, anim->GetLength());
    //#else
    return mAnimationStates.CreateAnimationState(animName, 0, anim->GetTimeRange());
    //#endif

}

AnimationState* AnimationManager::GetAnimationState(const AIRString& animName) const
{
    return mAnimationStates.GetAnimationState(animName);
}

bool AnimationManager::HasAnimationState(const AIRString& name) const
{
    return mAnimationStates.HasAnimationState(name);
}

void AnimationManager::DestroyAnimationState(const AIRString& name)
{
    mAnimationStates.RemoveAnimationState(name);
}

void AnimationManager::DestroyAllAnimationStates(void)
{
    mAnimationStates.RemoveAllAnimationStates();
}
*/

void AnimationManager::_ApplySceneAnimations(void)
{
    // manual lock over states (extended duration required)
    /*
    ConstEnabledAnimationStateIterator stateIt = mAnimationStates.GetEnabledAnimationStateIterator();

    while (!stateIt.IsEnd())
    {
        const AnimationState* state = stateIt.GetNext();
        NodeAnimation* anim = GetAnimation(state->GetAnimationName());

        // Reset any nodes involved
        // NB this excludes blended animations
        // Apply the animation
        //anim->Apply(state->GetTimePosition(), state->GetWeight());
    }
    */

    AnimationList::iterator itEnd = mAnimationsList.end();
    for (AnimationList::iterator i = mAnimationsList.begin(); i != itEnd; ++i)
    {
        i->second->GetSceneNode()->ResetToInitialState();
    }

    AnimationStateMap::iterator itStateEnd = m_mapAnimationStates.end();
    for (AnimationStateMap::iterator it = m_mapAnimationStates.begin(); it != itStateEnd; ++it)
    {
        CAIRAnimationState* pAnimationState = it->second;
        NodeAnimation* pAnim = GetAnimation(pAnimationState->GetName());
        pAnim->ApplyToNode(pAnim->GetSceneNode(), pAnimationState->GetTimePos(), pAnimationState->GetWeight());
    }
}


void AnimationManager::Update( float fTimeElapse )
{
    // manual lock over states (extended duration required)
    /*
    ConstEnabledAnimationStateIterator stateIt = mAnimationStates.GetEnabledAnimationStateIterator();

    while (!stateIt.IsEnd())
    {
        AnimationState* state = stateIt.GetNext();

        state->AddTime( fTimeElapse );
    }
    */
    AnimationStateMap::iterator itStateEnd = m_mapAnimationStates.end();
    for (AnimationStateMap::iterator it = m_mapAnimationStates.begin(); it != itStateEnd; ++it)
    {
        CAIRAnimationState* pAnimationState = it->second;
        pAnimationState->AddTime(fTimeElapse);
    }

    _ApplySceneAnimations();
}

