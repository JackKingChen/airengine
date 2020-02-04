// ***************************************************************
//  文件: GPUDataParam.h
//  日期: 2010-3-10
//  作者: liangairan
//  说明: GPU参数类，当GPU程序需要参数时，从该类中获得
// ***************************************************************

#pragma once

#include "Matrix4.h"
#include "ColorValue.h"
#include "Color.h"
#include "Vector2D.h"
#include "Color.h"
#include <list>
#include <vector>
class CLight;
struct GPUConstantDefinition;

const int MAX_BONE_NUM = 50;
const int MAX_BATCH_MATRIX_NUM = 50;   //最大的批量渲染数量
const int MAX_POSITION_NUM = 50;

class EngineExport CGPUDataParamSource
{
public:
    typedef AIRVector<Vector2Df>  UVScaleArray;
    typedef UVScaleArray::iterator  UVScaleArray_Iter;
    typedef AIRList<CLight*> LightList;
public:
    CGPUDataParamSource();
    virtual ~CGPUDataParamSource();

    void SetWorldMatrix(const Matrix4f& matWorld);

    void SetViewMatrix(const Matrix4f& matView);

    void SetProjectionMatrix(const Matrix4f& matProj);

    const Matrix4f& GetWorldMatrix();

    const Matrix4f& GetViewMatrix();

    const Matrix4f& GetProjectionMatrix();

    const Matrix4f& GetWorldViewProjMatrix();

	const Matrix4f& GetWorldMatrixTransposed() const
	{
		return m_matWorldTransposed;
	}

	const Matrix4f& GetViewMatrixTransposed() const
	{
		return m_matViewTransposed;
	}

	const Matrix4f& GetProjectionMatrixTransposed() const
	{
		return m_matProjTransposed;
	}

    const Matrix4f& GetInvViewProjMatrix();

	const Matrix4f& GetInvWorldViewProjMatrix();

    const Matrix4f& GetWorldViewMatrix();

    const Matrix4f& GetViewProjMatrix();

    const Matrix4f& GetTextureProjMatrix();

    const Matrix4f& GetTextureProjMatrixTransposed() const
    {
        return m_matTextureProjTransposed;
    }

    /*inline*/ void SetCameraPosition(const Vector3Df& position);

    /*inline*/ void SetAmbientColor(const CColorValue& color);

    const CColorValue& GetAmbientColor() const
    {
        return m_clrAmbient;
    }

    /*inline*/ void SetFogColor(const CColorValue& color);

    /*inline*/ void SetFogStart(const float fStart);

    /*inline*/ void SetFogEnd(const float fEnd);

    /*inline*/ void SetFogDensity(const float fDensity);

    /*inline*/ void SetLightList(LightList* pLightList);

    /*inline*/ void SetSunDirection(const Vector3Df& direction);

    /*inline*/ void SetSunLightColor(const CColorValue& color);

    /*inline*/ void SetClipPlane(const Plane3Df& plane);

    //inline void SetUVScaleArray(const UVScaleArray& uvScales, u32 nSize);

    /*inline*/ void SetMaterialAmbient(const CColorValue& color);

    /*inline*/ void SetMaterialSpecular(const CColorValue& color);

    /*inline*/ void SetMaterialDiffuse(const CColorValue& color);

    /*inline*/ void SetMaterialShiness(float fShiness);

    /*inline*/ void SetBoneMatrices(const Matrix4f* pMatrix, int nMatrixNum);

    //设置世界矩阵数组
    /*inline*/ void SetWorldMatrixArray(const Matrix4f* pMatrix, int nMatrixNum);

    /*inline*/ void SetWorldMatrixArray(const Matrix4f& mat, int nMatrixIndex);

    /*inline*/ void SetWorldMatrixNum(int nMatrixNum);

    /*inline*/ void SetPositionArray(const Vector4f* pPositions, int nPositionsNum);

    /*inline*/ void SetPositionArray(const Vector3Df& position, int nPositionIndex);

    /*inline*/ void SetPositionsNum(int nPositionsNum);

    //const UVScaleArray& GetUVScaleArray() const
    //{
    //    return m_vtUVScales;
    //}

	inline void SetFrameTime( float fFrameTime )
	{
		m_fTimeElapse = fFrameTime;
	}

    /*inline*/ void SetLightPosition(const Vector3Df& position);

    /*inline*/ void SetLightColor(const CColorValue& lightColor);

    /*inline*/ void SetLightRange(float fLightRadius);

    inline void SetCameraFarPlane(float fFarPlane)
    {
        m_fCameraFarPlane = fFarPlane;
    }

    inline void SetFarPlaneCorners(Vector3Df* pCorners)
    {
        m_pFarPlaneCorners = pCorners;
    }

    inline void SetHalfPixel(const Vector2Df& halfPixel)
    {
        m_halfPixel = halfPixel;
    }

    inline void SetWorldViewProjectMatrix(const Matrix4f& matWVP)
    {
        m_matWVP = matWVP;
        m_matWVP.GetTransposed(m_matWVPTransposed);
        m_bWorldViewProjMatrixDirty = false;
    }

    inline void SetViewProjectMatrix(const Matrix4f& matViewProj)
    {
        m_matViewProj = matViewProj;
        m_matViewProj.GetTransposed(m_matViewProjTransposed);
        m_bViewProjMatrixDirty = false;
    }

    inline void SetWorldViewProjDirty(bool bDirty)
    {
        m_bWorldViewProjMatrixDirty = bDirty;
    }

    /*inline*/ void SetColorDiffuse(const CColorValue& color);

    /*inline*/ void SetColorDiffuse(const SColor color);

    inline void SetFogType(int nFogType)
    {
        m_nFogType = nFogType;
    }

    //u32 GetUVScaleArraySize() const
    //{
    //    return m_uUVScaleArraySize;
    //}

    //更新数据
    void UpdateParameters();

    //void BuildAutoGPUParameters(GPUConstantDefinition* pGPUDef);

	inline void SetClipEnable(bool bEnable)
	{
		m_nClipEnable = bEnable;
	}

    void SetLightViewProjMatrix(const Matrix4f& matLightViewProj)
    {
        m_matLightViewProj = matLightViewProj;
    }

    void SetJitterScale(const Vector4f& scale)
    {
        m_scaleJitter = scale;
    }

    void SetFilterSize(const Vector4f& size)
    {
        m_filterSize = size;
    }

    void SetJitterDelta(float fDelta)
    {
        m_fJitterDelta = fDelta;
    }
public:
	Matrix4f m_matInvWVP;
    Matrix4f m_matWorld;
    Matrix4f m_matView;
    Matrix4f m_matProj;
    Matrix4f m_matWorldView;
    Matrix4f m_matWVP;
	Matrix4f m_matWorldTransposed;
	Matrix4f m_matViewTransposed;
	Matrix4f m_matProjTransposed;
	Matrix4f m_matWVPTransposed;
    Matrix4f m_matWorldViewTransposed;
    //Matrix4f m_matR;   //投影纹理用到的矩阵
    Matrix4f m_matTextureProj;   //投影纹理矩阵
    Matrix4f m_matTextureProjTransposed;   //投影纹理矩阵
    Matrix4f m_matInvViewProj;   
    Matrix4f m_matInvViewProjTransposed;
    Matrix4f m_matViewProj;
    Matrix4f m_matViewProjTransposed;
    CColorValue  m_clrAmbient;   //环境光颜色
    CColorValue  m_clrFog;       //雾颜色
    CColorValue  m_clrSunLight;      //太阳光颜色
    Vector3Df    m_cameraPosition;   //摄像机颜色
    Vector3Df    m_sunDirection;     //太阳光方向
    float        m_fFogStart;
    float        m_fFogEnd;
    float        m_fFogDensity;      //雾的参数
    float        m_fTimeElapse;      //每帧时间
    Plane3Df     m_clipPlane;        //裁剪面
    LightList*   m_pLightList;       //光照列表，这里暂时不用，因为没有更好的解决方案
    Vector2Df    m_uvScale;

    CColorValue  m_clrMaterialAmbient;  //材质环境光参数
    CColorValue  m_clrMaterialSpecular; //材质镜面反射参数
    CColorValue  m_clrMaterialDiffuse;  //材质漫反射参数
    float        m_fShiness;            //材质反射强度
    
    //UVScaleArray m_vtUVScales;

    Matrix4f     m_boneMatrices[MAX_BONE_NUM];
    u32          m_nBonesNum;

    //用于批量渲染
    Matrix4f     m_worldMatrixArray[MAX_BATCH_MATRIX_NUM];
    u32          m_nWorldMatrixNum;

    Vector4f     m_positionArray[MAX_POSITION_NUM];
    u32          m_nPositionsNum;

    Vector3Df    m_lightPosition;     //光源位置
    CColorValue  m_clrLight;          //光源颜色，一般是diffuse颜色
    float        m_fLightRange;       //光源影响范围，即光照的半径
    float        m_fCameraFarPlane;

    Vector2Df    m_halfPixel;
    Vector3Df*   m_pFarPlaneCorners;

    CColorValue  m_clrDiffuse;   //顶点颜色

    int          m_nFogType;

    //u32  m_uUVScaleArraySize;

	int m_nClipEnable;

    //shadowmap相关
    Matrix4f m_matLightSpaceWVP;
    Matrix4f m_matLightViewProj;
    Vector4f m_scaleJitter;
    Vector4f m_filterSize;
    f32      m_fJitterDelta;
    f32      m_fAlphaRef;
private:
    bool  m_bWorldMatrixDirty;
    bool  m_bViewMatrixDirty;
    bool  m_bProjMatrixDirty;
    bool  m_bWorldViewProjMatrixDirty;
    bool  m_bTextureProjMatrixDirty;
    bool  m_bViewProjMatrixDirty;

public:
    struct ShaderMatrix   //记录所有要用到的矩阵，DX11中是其中一个默认的cbuffer
    {
        Matrix4f matWorld;
        Matrix4f matView;
        Matrix4f matProjection;
        Matrix4f matWVP;
        Matrix4f matInvWVP;
        Matrix4f matWorldView;
        Matrix4f matViewProj;
        Matrix4f matInvViewProj;
    };

    struct ShaderMaterial
    {
        CColorValue  clrAmbient;   //环境光颜色
        CColorValue  clrMaterialAmbient;  //材质环境光参数
        CColorValue  clrMaterialSpecular; //材质镜面反射参数
        CColorValue  clrMaterialDiffuse;  //材质漫反射参数
        f32        fShiness;            //材质反射强度
    };

private:
    ShaderMatrix m_shaderMatrices;
    ShaderMaterial m_shaderMaterial;
    bool m_bMatrixDirty; 
};
