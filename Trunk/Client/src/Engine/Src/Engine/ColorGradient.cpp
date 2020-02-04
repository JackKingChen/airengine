#include "stdafx.h"
#include "ColorGradient.h"
#include "DebugTools.h"


ColorGradient::ColorGradient()
: mMalFormed(true)
{
}

ColorGradient::~ColorGradient()
{
}

//≤Â»Î—’…´÷°
void ColorGradient::AddCFrame(const ColorFrame& CFrame)
{
	if ( CFrame.second < 0 || CFrame.second > 1 )
	{
		DT_TO_DBGSTR( LT_WARN, "CFrame.second out of range!" );
		return;
	}
	CFrameVector.push_back(CFrame);
	mMalFormed = !_CheckBounds();
}

const AIR::Vector3Df ColorGradient::GetColor(f32 p) const
{
	if (mMalFormed)
	{
		return AIR::Vector3Df::ZERO;
	}

    size_t nFrameSize = CFrameVector.size();

	if (CFrameVector.empty())
	{
		return AIR::Vector3Df::ZERO;
	}
	else if (nFrameSize == 1)
	{
		return CFrameVector.at(0).first;
	}

	std::pair<int, f32> minBound, maxBound;

	// Min value
	minBound.first = 0;
	minBound.second = -1;
	for (unsigned int k = 0; k < nFrameSize; k++)
	{
		if (CFrameVector.at(k).second == p )
		{
			return CFrameVector.at(k).first;
		}
		else if (CFrameVector.at(k).second < p && CFrameVector.at(k).second > minBound.second)
		{
			minBound.first = k;
			minBound.second = CFrameVector.at(k).second;
		}
	}

	// Max value
	maxBound.first = 0;
	maxBound.second = 2;
	for (unsigned int k = 0; k < nFrameSize; k++)
	{
		if (CFrameVector.at(k).second > p && CFrameVector.at(k).second < maxBound.second)
		{
			maxBound.first = k;
			maxBound.second = CFrameVector.at(k).second;
		}
	}

	float range = maxBound.second - minBound.second,
		rangepoint = (p - minBound.second) / range;

	return CFrameVector.at(minBound.first).first*(1-rangepoint) + CFrameVector.at(maxBound.first).first*rangepoint;
}

bool ColorGradient::_CheckBounds()
{
	std::pair<bool, bool> existbounds;
	existbounds.first = false; existbounds.second = false;

	for (unsigned int k = 0; k < CFrameVector.size(); k++)
	{
		if (CFrameVector.at(k).second == 0)
		{
			// More than one min bound
			if (existbounds.first)
			{
				return false;
			}

			existbounds.first = true;
		}

		if (CFrameVector.at(k).second < 0 || CFrameVector.at(k).second > 1)
		{
			return false;
		}
	}

	for (unsigned int k = 0; k < CFrameVector.size(); k++)
	{
		if (CFrameVector.at(k).second == 1)
		{
			// More than one min bound
			if (existbounds.second)
			{
				return false;
			}

			existbounds.second = true;
		}
	}

	if (!existbounds.first || !existbounds.second)
	{
		return false;
	}

	return true;
}