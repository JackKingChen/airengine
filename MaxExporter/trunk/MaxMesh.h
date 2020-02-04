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
    int matID;       //����ID
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
    // Qualifier: ���Ҫ�����Ķ�������
    // Parameter: int nFaceID          �����������ID����mesh�е���������±�
    // Parameter: int nFaceVertexIndex
    //************************************
    CAIRVertex*  GetAIRVertex(CAIRSkeleton* pSkeleton, int nFaceID, int nFaceVertexIndex);

    //��ö�������
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

    //���õ�ǰmesh��ʱ��
    bool SetCurrentTime(unsigned int iMaxTime);
protected:
    INode* m_pNode;
    Mesh*  m_pMesh;
    Modifier* m_pModifier;
    TriObject*    m_triObjNeedDelete;    //��Ҫɾ�����������ڴ�й
    bool          m_bDelete;
    bool          m_bMirrored;
    ModifierType  m_modifierType;
    Matrix3 m_tm;
    std::vector<CMaxMaterial*>  m_vtUsedMaterial;  //ʹ�õ��Ĳ����б� 
    std::vector<StdMat*> m_vectorStdMat;    //�����б�
    std::vector<MAXFACE> m_vtFaces;          //����Ϣ

    CAIRStaticMesh       m_exportMesh;       //Ҫ���������ݽṹ

    std::string m_strName;
    //std::vector<CAIRStaticMesh*>  m_vtExportMeshes;   //�ֽ��Ҫ������mesh

    //����MAX�е�Meshָ��
    bool BuildIMesh(unsigned int iMaxTime);

    bool BuildAnimatedIMesh(unsigned int iMaxTime);
    //bool AddBoneInfluence(CSkeletonCandidate *pSkeletonCandidate, CVertexCandidate *pVertexCandidate, INode *pNode, float weight);
    //bool CreateMaterial(Mtl *pMtl);
    //��������Ϣ��ʵ�����Ǵ�����������
    bool BuildFaceInfo();
    //�������в��ʣ�����ӵ������б���
    bool BuildMaterials(Mtl *pMtl);

    //����stdmat��mesh�е������±꣬�ҳ���Ӧ��maxmaterial���±�
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
