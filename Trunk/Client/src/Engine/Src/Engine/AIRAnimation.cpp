#include "stdafx.h"
#include "AIRAnimation.h"
#include "FileSystem.h"
//#include "AIRSkeletonRes.h"
//#include "AIRSkeleton.h"
#include <algorithm>
#include <functional>
#include "ModelInstance.h"
#include "DebugTools.h" 

IMPLEMENT_ROOT_RTTI(CAIRAnimation)

CAIRAnimation::CAIRAnimation() : m_nFrames(0), m_fFrameRate(0), m_fTimeRange(0.0f)
{

}

CAIRAnimation::~CAIRAnimation()
{

}

//void CAIRAnimation::SetAnimationTime(float fTime)
//{
//    m_fTimeRange = fTime;
//}

void CAIRAnimation::SetFrameRate(float fFrameRate)
{
    m_fFrameRate = fFrameRate;
}
