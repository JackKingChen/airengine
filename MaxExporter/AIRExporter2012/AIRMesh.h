// ***************************************************************
//  AIRMesh   version:  1.0   ·  date: 06/04/2009
//  -------------------------------------------------------------
//  代表一整个mesh文件
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
    // Qualifier: 创建一个mesh
    // Parameter: int maxBoneCount     影响该mesh的骨骼数
    // Parameter: float weightThreshold
    //************************************
    bool Create(CAIRSkeleton* pSkeleton, CMaxNode* pMaxNode, int maxBoneCount, float weightThreshold);

    bool Create(CMaxNode* pMaxNode, TimeValue tTime, bool bVertexAnimation = false);

    bool SaveToFile(FILE* f);

    bool SaveToFileBinary(std::ofstream& fout);

    //保存顶点动画信息
    bool SaveVertexAnimation(FILE* f);

    bool SaveVertexAnimationBin(std::fstream& fout);

    //只保存顶点信息，不保存三角网格
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
    //CMaxNode*    m_pNode;  //对应的Max结点
    //CMaxMesh*    m_pMesh;  //对应的Max中的mesh
    bool m_bStatic;     //是否为静态模型（不受骨骼影响的模型）
    CMaxMesh* m_pMaxMesh;

    int  m_nFacesCount;
    typedef vector<CAIRSubMesh*>::iterator SUBMESH_ITER;
    std::vector<CAIRSubMesh*> m_vtSubMeshes;

    std::string  m_strSkeleton;   //影响该模型的骨骼文件名

    //CAIRSkeleton* m_pSkeleton;   //影响该模型的骨骼
};
