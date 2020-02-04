/********************************************************************
	created:	2009/10/03
	created:	3:10:2009   16:37
	filename: 	d:\My Codes\MyExport\AIRAnimBone.h
	file path:	d:\My Codes\MyExport
	file base:	AIRAnimBone
	file ext:	h
	author:		liangairan
	
	purpose:	�����еĹ�������
*********************************************************************/
#pragma once

#include <vector>
#include "max.h"

//ÿһ֡������
typedef struct tagFrameBoneData 
{
    float posX, posY, posZ;         //ע�⣬�����ǹ���ÿһ֡������丸�׵�λ��
    float orientX, orientY, orientZ, orientW;    //ÿһ֡������丸�׵���ת��Ԫ��
	Point3 vScale;                  //����ڸ��׵�����
    Point3 vAbsolutePos;            //��֡�ľ���λ�� GetNodeTM���
    Quat   qAbsoluteRotation;       //��֡�ľ�����ת
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
