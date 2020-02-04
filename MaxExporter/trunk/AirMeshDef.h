#pragma once
//#include <Windows.h>
//#include "Math/Vector4.h"
#include <math.h>

const char STR_NUMMESH[] = "numMeshes";

const char STR_MESH_HEADER[] = "mesh";

const char STR_MESH_VERT[] = "mesh_vertex";

const char STR_NUM_VERTS[] = "num_vertices";

const char STR_NUM_TRIANGLES[] = "num_triangles";

const char STR_MESH_TRIANGLE[] = "mesh_triangle";

const char STR_MESH_BOUNDINGBOX[] = "mesh_boundingbox";

const char STR_CAMERA[] = "camera";

const char STR_MATERIAL[] = "mesh_material";

const char STR_COORDINATE_RIGHT[] = "right_hand";

const char STR_COORDINATE_LEFT[] = "left_hand";

const char STR_AXIS_Z[] = "z_axis";

const char STR_AXIS_Y[] = "y_axis";

const float FILE_VERSION = 0.2f;    //0.2版本，添加了顶点颜色，顶点动画改成索引形式

const float fEPSILON = 0.000001f;

typedef struct tagAIRPosition 
{
    float x, y, z;
    inline bool operator == (const tagAIRPosition& other) const
    {
        //return x == other.x && y == other.y && z == other.z;
        if((fabs(x - other.x) >= fEPSILON)
            || (fabs(y - other.y) >= fEPSILON)
            || (fabs(z - other.z) >= fEPSILON)) 
            return false;
        return true;
    }

    inline bool operator != (const tagAIRPosition& other) const
    {
        return x != other.x || y != other.y || z != other.z;
    }
}AIRPOSITION, *LPAIRPOSITION;

typedef struct tagAIRTexcoord
{
    inline bool operator == (const tagAIRTexcoord& other) const
    {
        //return u == other.u && v == other.v;
        if((fabs(u - other.u) >= fEPSILON)
            || (fabs(v - other.v) >= fEPSILON))
            return false;
        return true;
    }

    inline bool operator != (const tagAIRTexcoord& other) const
    {
        return u != other.u || v != other.v;
    }
    float u, v;
}AIRTEXCOORD;

typedef struct tagAirMeshVert 
{
    int nIndex;
    float x, y, z;
    float tu, tv;
}AIRMESHVERT, *LPAIRMESHVERT;

//实际上是一个面的数据
typedef struct tagAirMeshTri
{
    int nIndex[3];
}AIRMESHTRI, *LPAIRMESHTRI;

typedef struct tagAirMesh 
{
    int nVertices;     //顶点数
    int nTriangles;    //面数

    LPAIRMESHVERT pVertices;   
    LPAIRMESHTRI  pTriangles;
}AIRMESH, *LPAIRMESH;

//bounding box 
typedef struct tagAirBoundingBox
{
    float minX, minY, minZ;
    float maxX, maxY, maxZ;
    
}AIRBOUNDINGBOX, *LPAIRBOUNDINGBOX;

typedef struct tagAirMaterial
{
    unsigned int dwColor;
    float Ambient[3];
    float Diffuse[3];
    float Specular[3];
    float Shiness;
}AIRMATERIAL, *LPAIRMATERIAL;

