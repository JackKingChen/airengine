/********************************************************************
	created:	2009/10/03
	created:	3:10:2009   10:28
	filename: 	d:\My Codes\MyExport\AIRAnimation.h
	file path:	d:\My Codes\MyExport
	file base:	AIRAnimation
	file ext:	h
	author:		liangairan
	
	purpose:	����������
*********************************************************************/
#pragma once

#include "AIRAnimBone.h"
#include "AirMeshDef.h"

class CAIRSkeleton;
class CMaxMesh;

class CAIRAnimation
{
public:
    
public:
    CAIRAnimation(void);
    virtual ~CAIRAnimation(void);

    bool Create(CAIRSkeleton& skeleton);

    float GetDuration() { return m_fDuration; }

    void SetDuration(float fDuration)
    {
        m_fDuration = fDuration;
    }

    bool SaveToFile(const char* szFilename);

    bool SaveToFileBinary(const char* szFilename);

	bool AppendToFile(FILE* fp);

    void Clear();

	void SetName(const char* szName);

	std::string GetName() { return m_strName; }

    int GetFramesNum() const
    {
        return m_nFrames;
    }

    void BuildBoundingBoxes(CMaxMesh* pNode, int nNodeIndex);

private:
    float m_fDuration;

    int   m_nFrames;  //֡��

	std::string  m_strName;   //������

    std::vector<CAIRAnimBone*> m_vtAnimBones;

    typedef std::vector<AIRBOUNDINGBOX> BOUNDINGBOX_VECTOR;   //ÿ֡�İ�Χ��

    std::vector<BOUNDINGBOX_VECTOR> m_vtBoundingBoxes;
};
