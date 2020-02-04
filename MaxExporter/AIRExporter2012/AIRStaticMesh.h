// ***************************************************************
//  AIRStaticMesh   version:  1.0   ·  date: 06/03/2009
//  -------------------------------------------------------------
//  导出的静态模型文件 
//    必须是已经分解好的mesh
//  -------------------------------------------------------------
//  Copyright (C) 2009 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************
//

#pragma once 

#include "AIRVertex.h"
#include <vector>
#include <string>

using namespace std;

class CAIRStaticMesh
{
public:

    //typedef LPAIRMESHTRI vIndexBuffer;


    CAIRStaticMesh() : m_bHasMaterial(false) {}
    ~CAIRStaticMesh() {}

    void SetVerticesSize(int nVerticesCount)
    {
        m_vertices.clear();
        m_vertices.reserve(nVerticesCount);
    }

    /*
    void SetFaceSize(int nFaceCount)
    {
        m_vtFaces.clear();
        m_vtFaces.reserve(nFaceCount);
    }
    */

    void AddVertex(const CAIRVertex& meshVert)
    {
        m_vertices.push_back(meshVert);
    }
    

    void SetName(const string& name)
    {
        m_strName = name;
    }

    const string& GetName() const
    {
        return m_strName;
    }

    void SetTextureName(const string& texName)
    {
        m_strTexture = texName;
    }

    const string& GetTextureName() const
    {
        return m_strTexture;
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
        m_material = m_material;
        m_bHasMaterial = true;
    }

    const AIRMATERIAL& GetMaterial() const
    {
        return m_material;
    }

    //设置顶点格式
    void SetWireframeColor(DWORD dwColor)
    {
        m_material.dwColor = dwColor;
    }

    DWORD GetWireframeColor() const
    {
        return m_material.dwColor;
    }

    /*
    bool ReadFromFile(const char* szFilename)
    {
        
        return true;
    }

    bool SaveToFile(const char* szFilename)
    {
        FILE* f = fopen(szFilename, "wt");
        
        if (f == NULL)
        {
            return false;
        }

        

        fclose(f);
        return true;
    }
    */

    size_t GetVerticesCount() const { return m_vertices.size(); }

    //int GetFaceCount() const { return m_vtFaces.size(); }

    const CAIRVertex& GetMeshVert(int nIndex) const
    {
        //AIRMESHVERT_Iter it = m_vtMeshVertices.begin() + nIndex;
        //return *it;
        return m_vertices[nIndex];
    }

    /*
    const AIRMESHTRI& GetMeshFace(int nIndex) const
    {
        return m_vtFaces[nIndex];
    }
    */

    void AddIndexBuffer(LPAIRMESHTRI pIndexBuffer)
    {
        m_vtIndexBuffers.push_back(pIndexBuffer);
    };

//protected:
//private:
    //vector<AIRMESHVERT>     m_vtMeshVertices;
    typedef vector<AIRMESHVERT>::iterator AIRMESHVERT_Iter;

    vector<CAIRVertex>  m_vertices;
    //vIndexBuffer  m_vtFaces;

    typedef vector<LPAIRMESHTRI> vIndexBuffers;
    vIndexBuffers  m_vtIndexBuffers;    //索引缓冲数组
    
    typedef vector<AIRMESHTRI>::iterator AIRMESHTRI_Iter;

    AIRBOUNDINGBOX  m_boundingBox;
    AIRMATERIAL     m_material;

    string m_strName;
    string m_strTexture;
    bool   m_bHasMaterial;
};