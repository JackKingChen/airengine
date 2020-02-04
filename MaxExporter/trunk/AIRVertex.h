#pragma once
#include "AirMeshDef.h"
#include "AIRSkeleton.h"
//#include "MaxMesh.h"
#include <vector>
using namespace std;

class CMaxMesh;

class CAIRVertex
{
public:
    typedef struct
    {
        float u, v;
    } TextureCoordinate;

    typedef struct
    {
        int boneId;
        float weight;
    } Influence;

    typedef struct
    {
        float weight;
        int springCount;
        int constraintDistance;
    } PhysicalProperty;
public:
    static float EPSILON;   //���
    CAIRVertex(void);
    virtual ~CAIRVertex(void);

    //************************************
    // Method:    Create
    // FullName:  CAIRVertex::Create
    // Access:    public 
    // Returns:   bool
    // Qualifier: �������㣬ȷ��position��TexCoordinate
    // Parameter: CMaxMesh * pMaxMesh
    // Parameter: int nFaceID               ����MaxMesh�е������±�
    // Parameter: int nFaceVertexIndex      �����ڸ����˳��0-2֮�䣩
    //************************************
    bool Create(CMaxMesh* pMaxMesh, int nFaceID, int nFaceVertexIndex);

    void AddTextureCoordinate(TextureCoordinate& txtCoord);

    void SetPosition(float x, float y, float z);

    void GetPosition(AIRPOSITION& position);

    int  GetTexCoordinateCount() const { return (int)m_vtTexCoordinates.size(); }

    void GetTexCoordinate(int nCoordinateIndex, TextureCoordinate& coordinate);

    bool AddBoneInfluence(CAIRSkeleton* pSkeleton, INode* pINode, float fWeight);

    bool operator == (const CAIRVertex& vertex);

    Influence GetInfluence(int nIndex)
    {
        return m_vtInfluence[nIndex];
    }

    size_t GetInfluenceNum()
    {
        return m_vtInfluence.size();
    }

    void SetWeightOffset(int nOffset)
    {
        m_nWeightOffset = nOffset;
    }

    int GetWeightOffset() { return m_nWeightOffset; }

    DWORD GetColor() const
    {
        return m_dwColor;
    }

    void SetColor(float a, float r, float g, float b)
    {
        unsigned char val8;
        DWORD val32 = 0;

        // Convert to 32bit pattern
        // (ARGB = 8888)

        // Alpha
        val8 = static_cast<unsigned char>(a * 255);
        val32 = val8 << 24;

        // Red
        val8 = static_cast<unsigned char>(r * 255);
        val32 += val8 << 16;

        // Green
        val8 = static_cast<unsigned char>(g * 255);
        val32 += val8 << 8;

        // Blue
        val8 = static_cast<unsigned char>(b * 255);
        val32 += val8;

        m_dwColor = val32;
    }

protected:
    vector<TextureCoordinate> m_vtTexCoordinates;    //�����������飬��Ӧ���������ڸð汾��Ӧ��ֻ��һ��
    vector<Influence>         m_vtInfluence;         //Ӱ��ö����Ȩ��
    AIRPOSITION   m_position;      //���������е�λ��
    int           m_nWeightOffset;
    DWORD         m_dwColor;    //������ɫ
    //AIRBOUNDINGBOX  m_boundingBox;
};
