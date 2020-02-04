/********************************************************************
	created:	2009/10/03
	created:	3:10:2009   16:37
	filename: 	d:\My Codes\MyExport\AIRAnimBone.h
	file path:	d:\My Codes\MyExport
	file base:	AIRAnimBone
	file ext:	h
	author:		liangairan
	
	purpose:	动画中的骨骼数据
*********************************************************************/
#pragma once

#include <vector>
#include "max.h"

//每一帧的数据
typedef struct tagFrameBoneData 
{
    float posX, posY, posZ;         //注意，这里是骨骼每一帧相对于其父亲的位置
    float orientX, orientY, orientZ, orientW;    //每一帧相对于其父亲的旋转四元数
	Point3 vScale;                  //相对于父亲的缩放
    Point3 vAbsolutePos;            //该帧的绝对位置 GetNodeTM获得
    Quat   qAbsoluteRotation;       //该帧的绝对旋转
}FRAMEDATA, *LPFRAMEDATA;

class CAIRAnimBone
{
public:
    CAIRAnimBone(int nBoneID);
    virtual ~CAIRAnimBone(void);

    void AddFrameData(FRAMEDATA frame)
    {
        m_vtFrames.push_back(frame);
    }

    FRAMEDATA GetFrameData(int nIndex)
    {
        return m_vtFrames[nIndex];
    }

    int GetBoneID() const { return m_nBoneID; }

private:
    int  m_nBoneID;

    std::vector<FRAMEDATA> m_vtFrames;
};
