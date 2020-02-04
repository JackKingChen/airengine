#pragma once

#include "AIRVertex.h"
#include "MaxMesh.h"
#include "AIRMaterial.h"
class CAIRMesh;

class CAIRSubMesh;

class CMeshKeyFrameData
{
public:
    CMeshKeyFrameData(int nFrame);
    ~CMeshKeyFrameData();

    //void Create(TimeValue time, CAIRSubMesh* pSubMesh);

    void Clear();

    bool SaveToFile(FILE* f);

    bool SaveToFile(std::fstream& fout);

    void AddPositionIndex(WORD nPosIndex, WORD nVertexIndex);

    void AddTexcoordIndex(WORD nTexcoordIndex, WORD nVertexIndex);

    void AddVertexColor(DWORD dwColor, WORD nVertexIndex);

    void SetVerticesNum(int nNum);
private:
    std::vector<WORD> m_vtPositionIndices;
    std::vector<WORD> m_vtTexcoordIndices;
    std::vector<DWORD> m_vtColors;
    int m_nFrameIndex;  //֡��
};

/*
* �������࣬����һ��mesh�ļ��е�����һ��mesh
*
*
*/
class CAIRSubMesh
{
public:
    CAIRSubMesh(void);
    virtual ~CAIRSubMesh(void);

    bool IsVertexAnimation() const
    {
        return m_bVertexAnimate;
    }

    void SetVertexAnimate(bool bVertexAnimate)
    {
        m_bVertexAnimate = bVertexAnimate;
    }

    //���position�����ظ�position��λ��
    int AddPosition(const AIRPOSITION& position);

    int AddTexcoord(const AIRTEXCOORD& texCoord);

    CMeshKeyFrameData* CreateAnimationKeyframe(int nFrame);

    bool SaveVertexAnimation(FILE* f);

    bool SaveVertexAnimationBin(std::fstream& out);
protected:
    int      m_nMaterialID;
    int      m_nMapCount;    //�õ�����������
    CMaxMesh*  m_pMaxMesh;   //����Ӧ��max�е�mesh
    CAIRMaterial*  m_pMaterial;    //��Ӧ��material
    AIRBOUNDINGBOX       m_boundingBox;   //��Χ��
    vector<CAIRVertex*>  m_vertices;
    vector<AIRMESHTRI>   m_vtFaces;
    int      m_nTotalWeightsNum;
    std::string  m_strName;

    //������Ӧ����vertexanimation��
    bool m_bVertexAnimate;   //�Ƿ�Ϊ���㶯��
    std::vector<AIRPOSITION> m_vtPositions;
    std::vector<AIRTEXCOORD> m_vtTexcoords;

    //�����vertexanimation������
    std::vector<CMeshKeyFrameData*> m_vtAnimationDatas;

public:
    //������
    bool    AddFace(int nVertexid1, int nVertexid2, int nVertexid3);

    //���Ӷ��㣬���ض����������е�λ��
    int     AddVertex(CAIRVertex* pVertex);

    //���ض�������
    size_t     GetVerticesCount() const { return m_vertices.size(); }

    //�����������
    int     GetFaceCount() const { return (int)m_vtFaces.size(); }

    CAIRVertex*  GetVertex(int nIndex);

    bool    Create(int nMaterialID, int nMapCount, CMaxMesh* pMesh, const char* szMeshName);

    const AIRMESHTRI& GetFace(int i);

    CAIRMaterial* GetAIRMaterial() const 
    {
        return m_pMaterial;
    }

    //************************************
    // Method:    CalculateBoundingBox
    // FullName:  CAIRSubMesh::CalculateBoundingBox
    // Access:    public 
    // Returns:   void
    // Qualifier: ����boundingbox
    //************************************
    void CalculateBoundingBox();

    void Clear();

    bool SaveToFile(CAIRMesh* pMesh, FILE* f);

    bool SaveToFile(std::ofstream& fout);

    bool SavePositionToFile(CAIRMesh* pMesh, FILE* f);

    bool SavePositionToFile(CAIRMesh* pMesh, std::fstream& fout);
};
