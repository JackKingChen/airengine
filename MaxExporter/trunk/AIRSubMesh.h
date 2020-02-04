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
    int m_nFrameIndex;  //帧号
};

/*
* 子网络类，代表一个mesh文件中的其中一个mesh
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

    //添加position，返回该position的位置
    int AddPosition(const AIRPOSITION& position);

    int AddTexcoord(const AIRTEXCOORD& texCoord);

    CMeshKeyFrameData* CreateAnimationKeyframe(int nFrame);

    bool SaveVertexAnimation(FILE* f);

    bool SaveVertexAnimationBin(std::fstream& out);
protected:
    int      m_nMaterialID;
    int      m_nMapCount;    //用到的纹理数量
    CMaxMesh*  m_pMaxMesh;   //所对应的max中的mesh
    CAIRMaterial*  m_pMaterial;    //对应的material
    AIRBOUNDINGBOX       m_boundingBox;   //包围盒
    vector<CAIRVertex*>  m_vertices;
    vector<AIRMESHTRI>   m_vtFaces;
    int      m_nTotalWeightsNum;
    std::string  m_strName;

    //下面是应用在vertexanimation中
    bool m_bVertexAnimate;   //是否为顶点动画
    std::vector<AIRPOSITION> m_vtPositions;
    std::vector<AIRTEXCOORD> m_vtTexcoords;

    //这个是vertexanimation的数据
    std::vector<CMeshKeyFrameData*> m_vtAnimationDatas;

public:
    //增加面
    bool    AddFace(int nVertexid1, int nVertexid2, int nVertexid3);

    //增加顶点，返回顶点在数组中的位置
    int     AddVertex(CAIRVertex* pVertex);

    //返回顶点数量
    size_t     GetVerticesCount() const { return m_vertices.size(); }

    //返回面的数量
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
    // Qualifier: 计算boundingbox
    //************************************
    void CalculateBoundingBox();

    void Clear();

    bool SaveToFile(CAIRMesh* pMesh, FILE* f);

    bool SaveToFile(std::ofstream& fout);

    bool SavePositionToFile(CAIRMesh* pMesh, FILE* f);

    bool SavePositionToFile(CAIRMesh* pMesh, std::fstream& fout);
};
