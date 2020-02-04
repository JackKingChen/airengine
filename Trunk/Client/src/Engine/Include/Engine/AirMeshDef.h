#pragma once
//#include "Math/Vector4.h"

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


typedef struct tagAirMeshVert 
{
    int nIndex;
    float x, y, z;
    float tu, tv;
}AIRMESHVERT, *LPAIRMESHVERT;

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
    float maxX, maxY, maxZ;
    float minX, minY, minZ;
}AIRBOUNDINGBOX, *LPAIRBOUNDINGBOX;

typedef struct tagAirMaterial
{
    u32 dwColor;
    float Ambient[3];
    float Diffuse[3];
    float Specular[3];
    float Shiness;
}AIRMATERIAL, *LPAIRMATERIAL;

