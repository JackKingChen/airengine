#include "stdafx.h"
#include "TextureAnimationTrack.h"

TextureAnimationTrack::TextureAnimationTrack() : AnimationTrack(NULL, 0), m_scale(Vector3Df::UNIT_SCALE)
{

}

TextureAnimationTrack::~TextureAnimationTrack()
{

}

void TextureAnimationTrack::GetInterpolatedKeyFrame(const TimeIndex& timeIndex, AnimationKeyFrame* kf) const
{
    AnimationKeyFrame *kBase1, *kBase2;
    AnimationKeyFrame *k1, *k2;
    unsigned short firstKeyIndex;

    float t = this->GetKeyFramesAtTime(timeIndex, &kBase1, &kBase2, &firstKeyIndex);
    k1 = static_cast<AnimationKeyFrame*>(kBase1);
    k2 = static_cast<AnimationKeyFrame*>(kBase2);

    if (t == 0.0)
    {
        // Just use k1
        kf->SetRotation(k1->GetRotation());
        kf->SetTranslate(k1->GetTranslate());
        kf->SetScale(k1->GetScale());
    }
    else
    {
        // Interpolate by t
        //NodeAnimation::InterpolationMode im = mParent->GetInterpolationMode();
        //NodeAnimation::RotationInterpolationMode rim = mParent->GetRotationInterpolationMode();
        Vector3Df base;


        kf->m_rotation.Slerp(k1->GetRotation(), k2->GetRotation(), t);


        // Translation
        base = k1->GetTranslate();
        kf->SetTranslate( base + ((k2->GetTranslate() - base) * t) );

        // Scale
        base = k1->GetScale();
        kf->SetScale( base + ((k2->GetScale() - base) * t) );
    }
}

void TextureAnimationTrack::ApplyToMatrix(Matrix4f& matApply, const TimeIndex& timeIndex, float weight /* = 1.0 */, float scale /* = 1.0f */)
{
    if (mKeyFrames.empty() || !weight)
        return;

    AnimationKeyFrame kf(timeIndex.GetTimePos());
    GetInterpolatedKeyFrame(timeIndex, &kf);

    // add to existing. Weights are not relative, but treated as absolute multipliers for the animation
    //if (m_nApplyMask & ANM_Translate)
    {
        Vector3Df translate = kf.GetTranslate() * weight * scale;
        m_position += translate;
        //node->Translate(translate);
    }


    // interpolate between no-rotation and full rotation, to point 'weight', so 0 = no rotate, 1 = full
    //if (m_nApplyMask & ANM_Rotate)
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
        m_rotation = kf.m_rotation;
        //DT_TO_DBGSTR(LT_COMMAND, GetString("node rotation : (%.4f, %.4f, %.4f, %.4f)\n", kf.m_rotation.x,
        //    kf.m_rotation.y, kf.m_rotation.z, kf.m_rotation.w));
    }

    //if (m_nApplyMask & ANM_Scale)
    {
        Vector3Df vScale = kf.GetScale();
        // Not sure how to modify scale for cumulative anims... leave it alone
        //scale = ((Vector3::UNIT_SCALE - kf.getScale()) * weight) + Vector3::UNIT_SCALE;
        if (scale != 1.0f && vScale != Vector3Df::UNIT_SCALE)
        {
            vScale = Vector3Df::UNIT_SCALE + (scale - Vector3Df::UNIT_SCALE) * scale;
        }
        m_scale = vScale;
    }

    Matrix4f rot4x4, scale4x4;
    m_rotation.GetMatrix(rot4x4);
    scale4x4.Identity();;
    scale4x4(0, 0) = m_scale.x;
    scale4x4(1, 1) = m_scale.y;
    scale4x4(2, 2) = m_scale.z;

    // Set up final matrix with scale, rotation and translation
    matApply = rot4x4 * scale4x4;
    matApply.SetTranslation(m_position);

    // No projection term
    matApply(0, 3) = 0; 
    matApply(1, 3) = 0; 
    matApply(2, 3) = 0; 
    matApply(3, 3) = 1;
}
