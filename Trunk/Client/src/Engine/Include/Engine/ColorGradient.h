/********************************************************************
	author:		HJJ
	created:	2011:10:10
	purpose:	
*********************************************************************/

#pragma once

#ifndef __ColorGradient_H__
#define __ColorGradient_H__


#include "Common.h"
#include <utility>
#include "Vector3D.h"
#include "EngineConfig.h"

class EngineExport ColorGradient
{
public:
	//颜色帧
	//ColorFrame.first : 颜色值
	//ColorFrame.second: 梯度,范围是 [0,1]
	typedef std::pair<AIR::Vector3Df, f32> ColorFrame;

	ColorGradient();

	~ColorGradient();

	//插入颜色帧
	void AddCFrame(const ColorFrame& CFrame);

	//清空所有的颜色帧
	inline void Clear()
	{
		CFrameVector.clear();
	}

	//根据梯度取颜色
	const AIR::Vector3Df GetColor(f32 p) const;

private:
	//检查颜色表是否正确设置
	bool _CheckBounds();

	bool mMalFormed;

	AIRVector<ColorFrame> CFrameVector;

};

#endif //__ColorGradient_H__