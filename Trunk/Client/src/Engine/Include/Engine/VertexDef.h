#pragma once 
#include "Common.h"
#include "Color.h"
#include "Vector3D.h"
#include "Matrix4.h"

enum EVertexBufferType
{
	E_VB_STANDARD			= 0,
	E_VB_NORMAL				= 1,
	E_VB_RHW				= 2,	
	E_VB_NOTEX				= 3,
	E_VB_NOCOLOR			= 4,
	E_VB_NORMAL_TANGENT		= 5,
    E_VB_NORMAL_BONE_WEIGHT = 6,
    E_VB_NORMAL_INSTANCE_DATA,
	E_VB_RHW_NOTEX,
	E_VB_USER_DEF			= 10,
};


typedef struct tagVertexStandard 
{
	//tagVertexStandard( void ) : position(0.0f, 0.0f, 0.0f), color(0xffffffff), tu(0.0f), tv(0.0f) {}

    Vector3Df	position;
    CColorValue		color;
    f32			tu, tv;

}VERTEX_STANDARD, *LPVERTEX_STANDARD;



typedef struct tagVertexNormal 
{
    //tagVertexNormal( void ) : vPos(0.0f, 0.0f, 0.0f), normal(0.0f, 0.0f, 0.0f), color(0xffffffff), tu(0.0f), tv(0.0f) {}

    Vector3Df	vPos;
    Vector3Df	normal;
    CColorValue		color;
    f32			tu, tv;
}VERTEX_NORMAL, *LPVERTEX_NORMAL;



typedef struct tagRHWVertex
{
	//tagRHWVertex( void ) : x(0.0f), y(0.0f), z(0.0f), rhw(1.0f), color(0xffffffff), tu(0.0f), tv(0.0f) {}

    f32			x, y, z, rhw; // The transformed position for the vertex.
    //SColor		color;        // The vertex color.
	CColorValue color;
    f32		tu, tv;
}RHWVERTEX, *LPRHWVERTEX;


typedef struct tagRHWVertexNoTex
{
	f32			x, y, z, rhw; // The transformed position for the vertex.
	CColorValue		color;        // The vertex color.
}RHWVERTEXNOTEX, *LPRHWVERTEXNOTEX;



typedef struct tagVertexNoTex
{
	//tagVertexNoTex() : position(0.0f, 0.0f, 0.0f), color(0xffffffff) {}

    Vector3Df	position;
    CColorValue		color;
}VERTEX_NOTEX, *LPVERTEX_NOTEX;



typedef struct tagVertexNoColor
{
	//tagVertexNoColor() : x(0.0f), y(0.0f), z(0.0f), fu(0.0f), fv(0.0f) {}

    f32			x, y, z;
    f32			fu, fv;
}VERTEX_NOCOLOR, *LPVERTEX_NOCOLOR;



typedef struct tagVertexNormalTangent //只有Shader才用到，不需要设置FVF
{
	//tagVertexNormalTangent() : x(0.0f), y(0.0f), z(0.0f), normal(0.0f, 0.0f, 0.0f), tangent(0.0f, 0.0f, 0.0f), color(0xffffffff), tu(0.0f), tv(0.0f) {}

	Vector3Df	position;
    Vector3Df	normal;
    Vector3Df	tangent;
    CColorValue		color;
    f32			tu, tv;
}VERTEX_NORMAL_TANGENT, *LPVERTEX_NORMAL_TANGENT;


//基于vs的骨骼动画顶点数据
typedef struct tagVertexNormalBoneWeights
{
    VERTEX_NORMAL vertex;
    float         weights[4];    //影响该顶点的四个权重
    float         boneIndex[4];  //影响该顶点的骨
}VERTEXNORMALBONEWEIGHTS, *LPVERTEXNORMALBONEWEIGHTS;

//instancing指量渲染数据
typedef struct tagVertexNormalInstanceData
{
    Vector3Df	vPos;
    Vector3Df	normal;
    CColorValue		color;
    f32			tu, tv;
    f32         fInstance;     //第几个实例
}VERTEXNORMAL_INSTANCEDATA, *LPVERTEXNORMAL_INSTANCEDATA;

typedef struct tagPosition4NoTex
{
	//tagVertexNoTex() : position(0.0f, 0.0f, 0.0f), color(0xffffffff) {}

	Vector4f	position;
	CColorValue		color;
}VERTEX_POS4_NOTEX, *LPVERTEX_POS4_NOTEX;
