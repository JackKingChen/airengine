#pragma once 
#include "Common.h"
#include "AirMeshDef.h"
#include <vector>
#include <string>
#include "EngineConfig.h"

using namespace std;

class EngineExport CAIRStaticMesh
{
public:
    CAIRStaticMesh() : m_bHasMaterial(false) {}
    ~CAIRStaticMesh() {}

    void SetVerticesSize(int nVerticesCount)
    {
        m_vtMeshVertices.clear();
        m_vtMeshVertices.reserve(nVerticesCount);
    }
    void SetFaceSize(int nFaceCount)
    {
        m_vtFaces.clear();
        m_vtFaces.reserve(nFaceCount);
    }

    void AddVertex(const AIRMESHVERT& meshVert)
    {
        m_vtMeshVertices.push_back(meshVert);
    }
    void AddFace(const AIRMESHTRI& face)
    {
        m_vtFaces.push_back(face);
    }

    void SetName(const string& name)
    {
        m_strName = name;
    }

    const string& GetName() const
    {
        return m_strName;
    }

    void SetMaterialName(const string& materialName)
    {
        m_strMaterial = materialName;
    }

    const string& GetMaterialName() const
    {
        return m_strMaterial;
    }

    void SetBoundingBox(const AIRBOUNDINGBOX& boundingBox)
    {
        m_boundingBox = boundingBox;
    }

    const AIRBOUNDINGBOX& GetBoundingBox() const
    {
        return m_boundingBox;
    }

    void SetMaterial(const AIRMATERIAL& material)
    {
        //m_material = m_material;
        m_bHasMaterial = true;
    }

    /*
    const AIRMATERIAL& GetMaterial() const
    {
        return m_material;
    }
    */

    //设置顶点格式
    void SetWireframeColor(u32 dwColor)
    {
        //m_material.dwColor = dwColor;
    }

    u32 GetWireframeColor() const
    {
        return 0;
        //return m_material.dwColor;
    }

    int GetVerticesCount() const { return (int)m_vtMeshVertices.size(); }

    int GetFaceCount() const { return (int)m_vtFaces.size(); }

    const AIRMESHVERT& GetMeshVert(int nIndex) const
    {
        //AIRMESHVERT_Iter it = m_vtMeshVertices.begin() + nIndex;
        //return *it;
        return m_vtMeshVertices[nIndex];
    }

    const AIRMESHTRI& GetMeshFace(int nIndex) const
    {
        return m_vtFaces[nIndex];
    }

//protected:
//private:
    AIRVector<AIRMESHVERT>     m_vtMeshVertices;
    typedef AIRVector<AIRMESHVERT>::iterator AIRMESHVERT_Iter;
    AIRVector<AIRMESHTRI>  m_vtFaces;
    typedef AIRVector<AIRMESHTRI>::iterator AIRMESHTRI_Iter;

    AIRBOUNDINGBOX  m_boundingBox;
    //AIRMATERIAL     m_material;

    string m_strName;
    string m_strMaterial;
    bool   m_bHasMaterial;
};