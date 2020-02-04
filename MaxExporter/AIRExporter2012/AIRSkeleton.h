/********************************************************************
	created:	2009/10/09
	created:	9:10:2009   11:16
	filename: 	d:\My Codes\MyExport\AIRSkeleton.h
	file path:	d:\My Codes\MyExport
	file base:	AIRSkeleton
	file ext:	h
	author:		liangairan
	
	purpose:	导出的骨骼动画类，结构为一个骨骼并包含所有属于它的动画在内
*********************************************************************/
#pragma once

#include "AIRBone.h"
#include "AIRAnimation.h"
#include <vector>

class CAIRSkeleton
{
public:
    CAIRSkeleton(void);
    virtual ~CAIRSkeleton(void);

    bool BuildBones(CMaxNode* pNode, int nParentID);

    bool SaveToFile(const char* szFilename);

    bool SaveToFileBinary(const char* szFilename);

	bool SaveToFileBinary(std::ofstream& fout);

    void Clear();

    int  GetBoneID(const char* szNodeName);

    size_t  GetBoneNum() { return m_vtBones.size(); }

    //取得每一帧中相对于父亲的位置与旋转
    void GetTranslationAndRotation(int nBoneID, float fTime, Point3& Pos, Quat& Rotation);

    //获得每一帧的绝对位置
    void GetAbsoluteTranslationAndRotation(int nBoneID, float fTime, Point3& Pos, Quat& Rotation);

    //取得每一帧中的相对坐标空间的位置
    //void GetBoneSpaceTranslation(int nBoneID, float fTime, Point3& Pos, Quat& Rotation);

	bool LoadFromFile(const char* szFilename);

    //从二进制文件读取
    bool LoadFromBinaryFile(const char* szFilename);

	//添加动画
	void AddAnimation(CAIRAnimation* pAnimation);

	//追加动画
	void SaveAnimation(CAIRAnimation* pAnimation);

	//获得某动画
	CAIRAnimation* GetAnimation(const char* szName);

    void SetFileName(const char* szFilename)
    {
        m_strFilename = szFilename;
    }

    std::string GetFileName() const
    {
        return m_strFilename;
    }

	bool HasBones() const
	{
		return !m_vtBones.empty();
	}
    //bool Create(CMaxNode* pNode, int nParentId, int nID);
private:
    void GetTranslationAndRotation(CMaxNode* pNode, CMaxNode* pParentNode, float fTime, Point3& Pos, Quat& Rotation);
    std::vector<CAIRBone*> m_vtBones;

	std::vector<CAIRAnimation*>  m_vtAnimation;

	std::string  m_strFilename;
};
