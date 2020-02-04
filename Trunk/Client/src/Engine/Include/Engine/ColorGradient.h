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
	//��ɫ֡
	//ColorFrame.first : ��ɫֵ
	//ColorFrame.second: �ݶ�,��Χ�� [0,1]
	typedef std::pair<AIR::Vector3Df, f32> ColorFrame;

	ColorGradient();

	~ColorGradient();

	//������ɫ֡
	void AddCFrame(const ColorFrame& CFrame);

	//������е���ɫ֡
	inline void Clear()
	{
		CFrameVector.clear();
	}

	//�����ݶ�ȡ��ɫ
	const AIR::Vector3Df GetColor(f32 p) const;

private:
	//�����ɫ���Ƿ���ȷ����
	bool _CheckBounds();

	bool mMalFormed;

	AIRVector<ColorFrame> CFrameVector;

};

#endif //__ColorGradient_H__