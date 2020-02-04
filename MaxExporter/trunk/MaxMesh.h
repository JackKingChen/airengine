#pragma once

#include "Max.h"
#include "stdmat.h"
#include "AirMeshDef.h"
#include "AIRStaticMesh.h"
#include "MaxMaterial.h"
#include <string>
#include <vector>

class CAIRVertex;

typedef struct tagMaxFace 
{
    int vert[3];
    int uv[3];
    int color[3];
    int normal[3];
    int matID;       //材质ID
    int faceIndex;
}MAXFACE, *LPMAXFACE;

class NullView : public View 
{
public:
    Point2 ViewToScreen(Point3 p)
    { 
        return Point2(p.x,p.y); 
    }

    NullView() 
    {
        worldToView.IdentityMatrix();
        screenW=640.0f; screenH = 480.0f;
    }

};


class CMaxMesh
{
public:
    typedef enum
    {
        MODIFIER_NONE,
        MODIFIER_SKIN,
        MODIFIER_PHYSIQUE
    } ModifierType;

    CMaxMesh();
    virtual ~CMaxMesh();

    bool Create(INode *pINode, unsigned int iMaxTime);

    bool CreateAnimatedMesh(INode* pINode, unsigned int iMaxTime);
    int GetFaceCount() const;
    Mesh *GetIMesh() { return m_pMesh; }
    INode* GetINode() { return m_pNode; }

    Matrix3 GetWorldMatrix() const { return m_tm; };
    int GetMaterialCount() const;
    int GetFaceMaterialId(int faceId);

    int GetSubMeshMapCount(int nMaterialIndex);

    StdMat* GetStdMtl(int nMatID);

    //************************************
    // Method:    GetAIRVertex
    // FullName:  CMaxMesh::GetAIRVertex
    // Access:    public 
    // Returns:   CAIRVertex*
    // Qualifier: 获得要导出的顶点数据
    // Parameter: int nFaceID          顶点所在面的ID，即mesh中的面的数组下标
    // Parameter: int nFaceVertexIndex
    //************************************
    CAIRVertex*  GetAIRVertex(CAIRSkeleton* pSkeleton, int nFaceID, int nFaceVertexIndex);

    //获得顶点数据
    bool GetVertexData(AIRTEXCOORD& texCoord, AIRPOSITION& position, DWORD& dwColor, int nFaceID, int nFaceVertexIndex);

    CMaxMaterial* GetMaterial(int nMaterialID);

    bool IsMirrored() { return m_bMirrored; }

    void  Clear();

    std::string GetName()
    {
        return m_strName;
    }

    //std::vector<CAIRMaterial*>* GetUsedMaterials() const
    //{
    //    return &m_vtUsedMaterial;
    //}
    static int s_nNewCount;

    //设置当前mesh的时间
    bool SetCurrentTime(unsigned int iMaxTime);
protected:
    INode* m_pNode;
    Mesh*  m_pMesh;
    Modifier* m_pModifier;
    TriObject*    m_triObjNeedDelete;    //需要删除，否则有内存泄
    bool          m_bDelete;
    bool          m_bMirrored;
    ModifierType  m_modifierType;
    Matrix3 m_tm;
    std::vector<CMaxMaterial*>  m_vtUsedMaterial;  //使用到的材质列表 
    std::vector<StdMat*> m_vectorStdMat;    //材质列表
    std::vector<MAXFACE> m_vtFaces;          //面信息

    CAIRStaticMesh       m_exportMesh;       //要导出的数据结构

    std::string m_strName;
    //std::vector<CAIRStaticMesh*>  m_vtExportMeshes;   //分解后要导出的mesh

    //创建MAX中的Mesh指针
    bool BuildIMesh(unsigned int iMaxTime);

    bool BuildAnimatedIMesh(unsigned int iMaxTime);
    //bool AddBoneInfluence(CSkeletonCandidate *pSkeletonCandidate, CVertexCandidate *pVertexCandidate, INode *pNode, float weight);
    //bool CreateMaterial(Mtl *pMtl);
    //创建面信息，实际上是创建索引缓冲
    bool BuildFaceInfo();
    //创建所有材质，并添加到材质列表中
    bool BuildMaterials(Mtl *pMtl);

    //根据stdmat在mesh中的数组下标，找出对应的maxmaterial的下标
    int FindMaxMatBySTDMatID(int nID);

    Modifier* FindPhysiqueModifier();
    Modifier* FindSkinModifier();

    bool HasVertexColor() const
    {
        return m_bHasVertexColor;
    }
private:
    bool m_bHasVertexColor;
};
