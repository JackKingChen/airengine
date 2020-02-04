/********************************************************************
	created:	2009/10/09
	created:	9:10:2009   11:16
	filename: 	d:\My Codes\MyExport\AIRSkeleton.h
	file path:	d:\My Codes\MyExport
	file base:	AIRSkeleton
	file ext:	h
	author:		liangairan
	
	purpose:	�����Ĺ��������࣬�ṹΪһ�����������������������Ķ�������
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

    //ȡ��ÿһ֡������ڸ��׵�λ������ת
    void GetTranslationAndRotation(int nBoneID, float fTime, Point3& Pos, Quat& Rotation);

    //���ÿһ֡�ľ���λ��
    void GetAbsoluteTranslationAndRotation(int nBoneID, float fTime, Point3& Pos, Quat& Rotation);

    //ȡ��ÿһ֡�е��������ռ��λ��
    //void GetBoneSpaceTranslation(int nBoneID, float fTime, Point3& Pos, Quat& Rotation);

	bool LoadFromFile(const char* szFilename);

    //�Ӷ������ļ���ȡ
    bool LoadFromBinaryFile(const char* szFilename);

	//��Ӷ���
	void AddAnimation(CAIRAnimation* pAnimation);

	//׷�Ӷ���
	void SaveAnimation(CAIRAnimation* pAnimation);

	//���ĳ����
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
