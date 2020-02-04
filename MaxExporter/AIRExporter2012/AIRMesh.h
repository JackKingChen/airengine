// ***************************************************************
//  AIRMesh   version:  1.0   ��  date: 06/04/2009
//  -------------------------------------------------------------
//  ����һ����mesh�ļ�
//  -------------------------------------------------------------
//  Copyright (C) 2009 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************
#pragma once
#include <string>
#include "AIRSubMesh.h"
#include "MaxNode.h"
#include "MaxMesh.h"
#include "AIRSkeleton.h"
#include <fstream>

class CAIRMesh
{
public:
    CAIRMesh(void);
    virtual ~CAIRMesh(void);


    //************************************
    // Method:    Create
    // FullName:  CAIRMesh::Create
    // Access:    public 
    // Returns:   bool
    // Qualifier: ����һ��mesh
    // Parameter: int maxBoneCount     Ӱ���mesh�Ĺ�����
    // Parameter: float weightThreshold
    //************************************
    bool Create(CAIRSkeleton* pSkeleton, CMaxNode* pMaxNode, int maxBoneCount, float weightThreshold);

    bool Create(CMaxNode* pMaxNode, TimeValue tTime, bool bVertexAnimation = false);

    bool SaveToFile(FILE* f);

    bool SaveToFileBinary(std::ofstream& fout);

    //���涥�㶯����Ϣ
    bool SaveVertexAnimation(FILE* f);

    bool SaveVertexAnimationBin(std::fstream& fout);

    //ֻ���涥����Ϣ����������������
    bool SavePositionToFile(FILE* f);

    bool SavePositionToFile(std::fstream& fout);

    void Clear();

    int  GetSubMeshCount() { return (int)m_vtSubMeshes.size(); }

    CAIRSubMesh* GetSubMesh(size_t nIndex)
    {
        if (nIndex >= m_vtSubMeshes.size() || m_vtSubMeshes.size() == 0)
        {
            return NULL;
        }
        return m_vtSubMeshes[nIndex];
    }

    const std::string& GetSkeletonFile() const
    {
        return m_strSkeleton;
    }

    int GetFaceNum() const
    {
        return m_nFacesCount;
    }

    std::string GetName() const
    {
        if (m_pMaxMesh)
        {
            return m_pMaxMesh->GetName();
        }
        return "";
    }

    bool CreateVertexAnimation(int nFrame);

    CMaxMesh* GetMaxMesh()
    {
        return m_pMaxMesh;
    }

protected:
    //CMaxNode*    m_pNode;  //��Ӧ��Max���
    //CMaxMesh*    m_pMesh;  //��Ӧ��Max�е�mesh
    bool m_bStatic;     //�Ƿ�Ϊ��̬ģ�ͣ����ܹ���Ӱ���ģ�ͣ�
    CMaxMesh* m_pMaxMesh;

    int  m_nFacesCount;
    typedef vector<CAIRSubMesh*>::iterator SUBMESH_ITER;
    std::vector<CAIRSubMesh*> m_vtSubMeshes;

    std::string  m_strSkeleton;   //Ӱ���ģ�͵Ĺ����ļ���

    //CAIRSkeleton* m_pSkeleton;   //Ӱ���ģ�͵Ĺ���
};
