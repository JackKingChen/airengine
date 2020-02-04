/********************************************************************
	created:	2010/07/15
	created:	15:7:2010   15:34
	filename: 	AIRKeyframeAnimation.h
	author:		liangairan
	
	purpose:	
*********************************************************************/

#pragma once

#include "MaxNode.h"
#include "AIRMesh.h"

class CAIRKeyFrameData
{
public:
    CAIRKeyFrameData(int nFrame);
    virtual ~CAIRKeyFrameData();

    void Create(TimeValue time, std::vector<CMaxNode*>& vtMaxNodes);

    void Clear();

    bool SaveToFile(FILE* f);

    bool SaveToFile(std::fstream& fout);
private:
    std::vector<CAIRMesh*> m_vtMeshes;
    int m_nFrameIndex;  //帧号
};

class CAIRKeyframeAnimation
{
public:
    CAIRKeyframeAnimation(void);
    virtual ~CAIRKeyframeAnimation(void);

    bool Create(std::vector<CMaxNode*>& vtMaxNodes);

    bool SaveToFile(const char* szFilename);

    bool SaveToFileBinary(const char* szFilename);

    //清空
    void Clear();

    void SetName(const char* szName);

private:
    int m_nFrames;   //帧数

    std::string  m_strName;    //动画名字

    std::vector<CAIRKeyFrameData*> m_vtKeyFrameData;
};
