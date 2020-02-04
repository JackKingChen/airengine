#include "stdafx.h"
#include "GPUDataParam.h"
#include "Engine.h"
#include "GPUParameters.h"

CGPUDataParamSource::CGPUDataParamSource() : m_fShiness(0)
, m_bWorldMatrixDirty(true)
, m_bViewMatrixDirty(true)
, m_bProjMatrixDirty(true)
, m_bWorldViewProjMatrixDirty(true)
, m_bTextureProjMatrixDirty(true) 
, m_bViewProjMatrixDirty(true)
, m_nBonesNum(0)
, m_nWorldMatrixNum(0)
, m_fCameraFarPlane(0)
//, m_uUVScaleArraySize(0)
, m_bMatrixDirty(false)
, m_nClipEnable(0)
, m_fAlphaRef(0.0f)
{
 //   m_matR[0] = 0.5; m_matR[12] = 0.5;
 //   m_matR[5] = -0.5; m_matR[13] = 0.5;
 //   m_matR[10] = 1; m_matR[14] = 1;
	//m_matR[15] = 1;
	//m_matR.GetTransposed(m_matR);
    //m_vtUVScales.resize(8);
    m_shaderMaterial.fShiness = 0.0f;
}

CGPUDataParamSource::~CGPUDataParamSource()
{

}

void CGPUDataParamSource::SetWorldMatrix(const Matrix4f& matWorld)
{
    m_matWorld = matWorld;
	m_matWorld.GetTransposed(m_matWorldTransposed);
    m_bWorldMatrixDirty = false;
    m_bWorldViewProjMatrixDirty = true;
    m_bMatrixDirty = true;
    m_bTextureProjMatrixDirty = true;
}

void CGPUDataParamSource::SetViewMatrix(const Matrix4f& matView)
{
    m_matView = matView;
	m_matView.GetTransposed(m_matViewTransposed);
    m_bViewMatrixDirty = false;
    m_bWorldViewProjMatrixDirty = true;
    m_bViewProjMatrixDirty = true;
    m_bMatrixDirty = true;
    m_bTextureProjMatrixDirty = true;
}

void CGPUDataParamSource::SetProjectionMatrix(const Matrix4f& matProj)
{
    m_matProj = matProj;
	m_matProj.GetTransposed(m_matProjTransposed);
    m_bProjMatrixDirty = false;
    m_bViewProjMatrixDirty = true;
    m_bMatrixDirty = true;
    m_bTextureProjMatrixDirty = true;
}

const Matrix4f& CGPUDataParamSource::GetWorldMatrix()
{
    if (m_bWorldMatrixDirty)
    {
		//IRenderer* pRenderer = ENGINE_INST->GetRenderer();
		//pRenderer->GetWorldMatrix(&m_matWorld);
		//m_matWorld.GetTransposed(m_matWorldTransposed);
		//m_bWorldMatrixDirty = false;
    }
    return m_matWorld;
}

const Matrix4f& CGPUDataParamSource::GetViewMatrix()
{
    if (m_bViewMatrixDirty)
    {
        IRenderer* pRenderer = ENGINE_INST->GetRenderer();
        pRenderer->GetViewMatrix(&m_matView);
		m_matView.GetTransposed(m_matViewTransposed);
        m_bViewMatrixDirty = false;
    }
    return m_matView;
}

const Matrix4f& CGPUDataParamSource::GetProjectionMatrix()
{
    if (m_bProjMatrixDirty)
    {
        IRenderer* pRenderer = ENGINE_INST->GetRenderer();
        pRenderer->GetProjectionMatrix(&m_matProj);
		m_matProj.GetTransposed(m_matProjTransposed);
        m_bProjMatrixDirty = false;
    }
    return m_matProj;
}

const Matrix4f& CGPUDataParamSource::GetWorldViewProjMatrix()
{
    if (m_bWorldViewProjMatrixDirty)
    {
        m_matWVP = GetWorldMatrix() * GetViewMatrix() * GetProjectionMatrix();
        //for test
        //Vector3Df vPos;
        //m_matWVP.TransformCoord(Vector3Df(10.0f, 10.0f, -20.0f), vPos);
        //----
        m_matWVP.GetTransposed(m_matWVPTransposed);
        m_bWorldViewProjMatrixDirty = false;
    }
    return m_matWVPTransposed;
}

const Matrix4f& CGPUDataParamSource::GetInvWorldViewProjMatrix()
{
	if (m_bWorldViewProjMatrixDirty)
	{
		m_matWVP = GetWorldMatrix() * GetViewMatrix() * GetProjectionMatrix();
		m_matWVP.GetTransposed(m_matWVPTransposed);
		m_matWVPTransposed.GetInverse( m_matInvWVP );
		m_bWorldViewProjMatrixDirty = false;
	}
	return m_matInvWVP;
}

const Matrix4f& CGPUDataParamSource::GetInvViewProjMatrix()
{
    if (m_bViewProjMatrixDirty)
    {
        m_matViewProj = GetViewMatrix() * GetProjectionMatrix();

        //Matrix4f matInv;
        //m_matViewProj.GetInverse(matInv);
        //Vector3Df vec(0, 0, 1);
        //matInv.TransformCoord(vec, vec);
        //m_matViewProj.TransformCoord(vec, vec);

        m_matViewProj.GetInverse(m_matInvViewProj); 
        m_matInvViewProj.GetTransposed(m_matInvViewProjTransposed);
        
        m_bViewProjMatrixDirty = false;
    }
    return m_matInvViewProjTransposed;
}

const Matrix4f& CGPUDataParamSource::GetWorldViewMatrix()
{
    m_matWorldView = GetWorldMatrix() * GetViewMatrix();
    //m_matWorldView = m_matWorldView.GetTransposed();
    //for test
    //-----
    m_matWorldView.GetTransposed(m_matWorldViewTransposed);
    return m_matWorldViewTransposed;
}

const Matrix4f& CGPUDataParamSource::GetViewProjMatrix()
{
    if (m_bViewProjMatrixDirty)
    {
        m_matViewProj = GetViewMatrix() * GetProjectionMatrix();
        m_matViewProj.GetTransposed(m_matViewProjTransposed);
        m_bViewProjMatrixDirty = false;
    }
    return m_matViewProjTransposed;
}

const Matrix4f& CGPUDataParamSource::GetTextureProjMatrix()
{
	if (m_bTextureProjMatrixDirty)
	{
        m_matTextureProj = GetWorldMatrix() * GetViewMatrix() * GetProjectionMatrix() * Matrix4f::CLIPSPACE2DTOIMAGESPACE;
        //Matrix4f matClipSpace2D = Matrix4f::CLIPSPACE2DTOIMAGESPACE;
        //matClipSpace2D(3, 0) = RENDERER->GetPixelOffset();
        //matClipSpace2D(3, 1) = RENDERER->GetPixelOffset();
        //Matrix4f matTexProj = GetWorldMatrix() * GetViewMatrix() * GetProjectionMatrix() * matClipSpace2D;
        //m_matTextureProj = m_matTextureProj.GetTransposed();
        m_matTextureProj.GetTransposed(m_matTextureProjTransposed);
        m_bTextureProjMatrixDirty = false;
	}
    
    return m_matTextureProj;
}

void CGPUDataParamSource::SetCameraPosition(const Vector3Df& position)
{
    m_cameraPosition = position;
}

void CGPUDataParamSource::SetAmbientColor(const CColorValue& color)
{
    m_clrAmbient = color;
}

void CGPUDataParamSource::SetFogColor(const CColorValue& color)
{
    m_clrFog = color;
}

void CGPUDataParamSource::SetFogStart(const float fStart)
{
    m_fFogStart = fStart;
}

void CGPUDataParamSource::SetFogEnd(const float fEnd)
{
    m_fFogEnd = fEnd;
}

void CGPUDataParamSource::SetFogDensity(const float fDensity)
{
    m_fFogDensity = fDensity;
}

void CGPUDataParamSource::SetLightList(LightList* pLightList)
{
    m_pLightList = pLightList;
}

void CGPUDataParamSource::SetClipPlane(const Plane3Df& plane)
{
    m_clipPlane = plane;
}

void CGPUDataParamSource::SetSunDirection(const Vector3Df& direction)
{
    m_sunDirection = -direction;
}

//void CGPUDataParamSource::SetUVScaleArray(const UVScaleArray& uvScales, u32 nSize)
//{
//    for (u32 i = 0; i < nSize; ++i)
//    {
//        m_vtUVScales[i] = uvScales[i];
//    }
//    m_uUVScaleArraySize = nSize;
//}

void CGPUDataParamSource::SetMaterialAmbient(const CColorValue& color)
{
    m_clrMaterialAmbient = color;
    //m_shaderMaterial.clrMaterialAmbient = color;
}

void CGPUDataParamSource::SetMaterialDiffuse(const CColorValue& color)
{
    m_clrMaterialDiffuse = color;
    //m_shaderMaterial.clrMaterialDiffuse = color;
}

void CGPUDataParamSource::SetMaterialSpecular(const CColorValue& color)
{
    m_clrMaterialSpecular = color;
    //m_shaderMaterial.clrMaterialSpecular = color;
}

void CGPUDataParamSource::SetMaterialShiness(float fShiness)
{
    m_fShiness = fShiness;
    //m_shaderMaterial.fShiness = fShiness;
}

void CGPUDataParamSource::SetSunLightColor(const CColorValue& color)
{
    m_clrSunLight = color;
}

void CGPUDataParamSource::SetBoneMatrices(const Matrix4f* pMatrix, int nMatrixNum)
{
    //memcpy(m_boneMatrices, pMatrix, nMatrixNum * sizeof(Matrix4f));
	for ( int i = 0; i < nMatrixNum; ++i )
	{
		m_boneMatrices[i] = pMatrix[i].GetTransposed();
	}
    m_nBonesNum = nMatrixNum;
}

void CGPUDataParamSource::SetWorldMatrixArray(const Matrix4f* pMatrix, int nMatrixNum)
{
    for (int i = 0; i < nMatrixNum; ++i)
    {
        pMatrix[i].GetTransposed(m_worldMatrixArray[i]);
    }
    m_nWorldMatrixNum = nMatrixNum;
}

void CGPUDataParamSource::SetWorldMatrixArray(const AIR::Matrix4f &mat, int nMatrixIndex)
{
    mat.GetTransposed(m_worldMatrixArray[nMatrixIndex]);
}

void CGPUDataParamSource::SetWorldMatrixNum(int nMatrixNum)
{
    m_nWorldMatrixNum = nMatrixNum;
}

void CGPUDataParamSource::SetPositionArray(const Vector4f* pPositions, int nPositionsNum)
{
    memcpy(m_positionArray, pPositions, nPositionsNum * sizeof(Vector4f));
    m_nPositionsNum = nPositionsNum;
}

void CGPUDataParamSource::SetPositionsNum(int nPositionsNum)
{
    m_nPositionsNum = nPositionsNum;
}

void CGPUDataParamSource::SetPositionArray(const Vector3Df& position, int nPositionIndex)
{
    m_positionArray[nPositionIndex].x = position.x;
    m_positionArray[nPositionIndex].y = position.y;
    m_positionArray[nPositionIndex].z = position.z;
    m_positionArray[nPositionIndex].w = 1;
}

void CGPUDataParamSource::SetLightPosition(const Vector3Df& position)
{
    m_lightPosition = position;
}

void CGPUDataParamSource::SetLightColor(const CColorValue& lightColor)
{
    m_clrLight = lightColor;
}

void CGPUDataParamSource::SetLightRange(float fLightRadius)
{
    m_fLightRange = fLightRadius;
}

void CGPUDataParamSource::SetColorDiffuse(const SColor color)
{
    m_clrDiffuse.Set((float)color.GetAlpha() / 255, (float)color.GetRed() / 255, (float)color.GetGreen() / 255, (float)color.GetBlue() / 255);
}

void CGPUDataParamSource::SetColorDiffuse(const CColorValue& color)
{
    m_clrDiffuse = color;
}

void CGPUDataParamSource::UpdateParameters()
{
    if (m_bMatrixDirty)
    {
        m_matWorld.GetTransposed(m_shaderMatrices.matWorld);
        m_matView.GetTransposed(m_shaderMatrices.matView);
        m_matProj.GetTransposed(m_shaderMatrices.matProjection);
        Matrix4f matWorldView = m_matWorld * m_matView;
        matWorldView.GetTransposed(m_shaderMatrices.matWorldView);
        Matrix4f matWVP = matWorldView * m_matProj;
        matWVP.GetTransposed(m_shaderMatrices.matWVP);
        Matrix4f matViewProj = m_matView * m_matProj;
        matViewProj.GetTransposed(m_shaderMatrices.matViewProj);

        m_shaderMatrices.matWVP.GetInverse(m_shaderMatrices.matInvWVP);
        m_shaderMatrices.matViewProj.GetInverse(m_shaderMatrices.matInvViewProj);
        m_bMatrixDirty = false;
    }
}

