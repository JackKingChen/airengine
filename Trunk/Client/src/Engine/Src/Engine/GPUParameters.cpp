#include "stdafx.h"
#include "GPUParameters.h"
#include "Engine.h"
//#include "ShaderParameter.h"
//#include "ManualGPUDataParamSource.h"
#include "ISceneManager.h"
#include "InternalFixedStrings.h"

GPUParameters::GPUParameters()// : m_bManualConstantTypeBuilded(false)
{

}

GPUParameters::~GPUParameters()
{

}

AutoConstantType GPUParameters::GetAutoConstantTypeByName(const FixedString& strName)
{
    if (strName == *FS_WVPMatrix/*"wvpMatrix"*/)
    {
        return eACT_WVPMATRIX;
    }
    if (strName == *FS_InvWVPMatrix/*"invWvpMatrix"*/)
    {
        return eACT_INVWVPMATRIX;
    }
    else if (strName == *FS_WorldMatrix/*"worldMatrix"*/)
    {
        return eACT_WORLDMATRIX;
    }
    else if (strName == *FS_ViewMatrix/*"viewMatrix"*/)
    {
        return eACT_VIEWMATRIX;
    }
    else if (strName == *FS_WorldViewMatrix/*"WorldViewMatrix"*/)
    {
        return eACT_WORLDVIEWMATIRX;
    }
    else if (strName == *FS_ProjectionMatrix/*"projectionMatrix"*/)
    {
        return eACT_PROJECTIONMATRIX;
    }
    else if (strName == *FS_fogColor/*"fogColor"*/)
    {
        return eACT_FOG_COLOR;
    }
    else if (strName == *FS_fogStart/*"fogStart"*/)
    {
        return eACT_FOG_START;
    }
    else if (strName == *FS_fogEnd/*"fogEnd"*/)
    {
        return eACT_FOG_END;
    }
    else if (strName == *FS_fogDensity/*"fogDensity"*/)
    {
        return eACT_FOG_DENSITY;
    }
    else if (strName == *FS_lightDiffuse/*"lightDiffuse"*/)
    {
        return eACT_LIGHT_DIFFUSE_COLOR;
    }
    else if (strName == *FS_lightAmbient/*"lightAmbient"*/)
    {
        return eACT_LIGHT_AMBIENT_COLOR;
    }
    else if (strName == *FS_lightSpecular/*"lightSpecular"*/)
    {
        return eACT_LIGHT_SPECULAR_COLOR;
    }
    else if (strName == *FS_ambientColor/*"ambientColor"*/)
    {
        return eACT_AMBIENT_COLOR;
    }
    else if (strName == *FS_textureProjMatrix/*"textureProjMatrix"*/)
    {
        return eACT_TEXPROJMATRIX;
    }
    else if (strName == *FS_clipPlane/*"clipPlane"*/)
    {
        return eACT_CLIPPLANE;
    }
    else if (strName == *FS_sunLight/*"sunLight"*/)
    {
        return eACT_SUN_LIGHT_DIRECTION;
    }
    else if (strName == *FS_sunColor/*"sunColor"*/)
    {
        return eACT_SUN_LIGHT_COLOR;
    }
    else if (strName == *FS_cameraPos/*"cameraPos"*/)
    {
        return eACT_CAMERAPOSITION;
    }
    //else if (strName == *FS_uvScales/*"uvScales"*/ || strName == *FS_uvScales_1/*"uvScales[0]"*/)
    //{
    //    return eACT_UVSCALE_ARRAY;
    //}
    //else if (strName == *FS_uvScale/*"uvScale"*/)
    //{
    //    return eACT_UVSCALE;
    //}
    else if (strName == *FS_shininess/*"shininess"*/)
    {
        return eACT_MATERIAL_SHINESS;
    }
    else if (strName == *FS_ka/*"ka"*/)
    {
        return eACT_MATERIAL_AMBIENT;
    }
    else if (strName == *FS_ks/*"ks"*/)
    {
        return eACT_MATERIAL_SPECULAR;
    }
    else if (strName == *FS_kd/*"kd"*/)
    {
        return eACT_MATERIAL_DIFFUSE;
    }
    else if (strName == *FS_frameTime/*"frameTime"*/)
    {
        return eACT_FRAME_TIME;
    }
    else if (strName == *FS_boneMatrix/*"boneMatrix"*/ || strName == *FS_boneMatrix_1/*"boneMatrix[0]"*/)
    {
        return eACT_BONE_MATRIX;
    }
    else if (strName == *FS_worldMatrixArray/*"worldMatrixArray"*/)
    {
        return eACT_WORLDMATRIX_ARRAY;
    }
    else if (strName == *FS_positionArray/*"positionArray"*/)
    {
        return eACT_POSITION_ARRAY;
    }
    else if (strName == *FS_LightPosition/*"LightPosition"*/)
    {
        return eACT_LIGHT_POSITION;
    }
    else if (strName == *FS_LightRadius/*"LightRadius"*/)
    {
        return eACT_LIGHT_RADIUS;
    }
    else if (strName == "ViewPortWidth")
    {
        return eACT_VIEWPORT_WIDTH;
    }
    else if (strName == "ViewPortHeight")
    {
        return eACT_VIEWPORT_HEIGHT;
    }
    else if (strName == *FS_LightPositionInView/*"LightPositionInView"*/)
    {
        return eACT_LIGHTPOS_INVIEW;
    }
    else if (strName == *FS_InvViewProjMatrix/*"InvViewProjMatrix"*/)
    {
        return eACT_INVVIEWPROJ_MATRIX;
    }
    else if (strName == *FS_farPlane/*"farPlane"*/)
    {
        return eACT_CAMERA_FARPLANE;
    }
    else if (strName == *FS_FrustumCorners/*"FrustumCorners"*/)
    {
        return eACT_FARPLANE_CORNERS;
    }
    else if (strName == *FS_halfPixel/*"halfPixel"*/)
    {
        return eACT_HALFPIXEL;
    }
    else if (strName == *FS_colorDiffuse/*"colorDiffuse"*/)
    {
        return eACT_COLOR_DIFFUSE;
    }
    else if (strName == *FS_ViewProjMatrix/*"ViewProjMatrix"*/)
    {
        return eACT_VIEWPROJMATRIX;
    }
    else if (strName == "fogType")
    {
        return eACT_FOG_TYPE;
    }
    else if (strName == "cameraRight")
    {
        return eACT_CAMERA_RIGHT;
    }
    else if (strName == "cameraDir")
    {
        return eACT_CAMERA_LOOK;
    }
    else if (strName == "cameraUp")
    {
        return eACT_CAMERA_UP;
    }
	else if (strName == *FS_nClipEnable/*"nClipEnable"*/)
	{
		return eACT_CLIPENABLE;
	}
    else if (strName == *FS_JitterScale)
    {
        return eACT_JITTERSCALE;
    }
    else if (strName == *FS_FilterSize)
    {
        return eACT_FILTTERSIZE;
    }
    else if (strName == *FS_jitterDelta)
    {
        return eACT_JITTERDELTA;
    }
    else if (strName == *FS_LightSpaceProjMatrix)
    {
        return eACT_LightSpaceProjMatrix;
    }
    else if (strName == *FS_LightSpaceViewProjMatrix)
    {
        return eACT_LightSpaceViewProjMatrix;
    }
    //else if (strName == *FS_AlphaTest)
    //{
    //    return eACT_AlphaRef;
    //}

    return eACT_UNKNOWN;
}

/*
void GPUParameters::BuildManualConstantType(ManualGPUDataParamSource* pSource)
{
	//if (!m_bManualConstantTypeBuilded)
	{
		int nBuildFailedCount = 0;
		GPUConstDef_Vector_Iter itEnd = m_vtManualGPUConstDef.end();
		GPUConstantDefinition* pConstantDef = NULL;
		for (GPUConstDef_Vector_Iter it = m_vtManualGPUConstDef.begin(); it != itEnd; ++it)
		{
			pConstantDef = &(*it);
			//if (pConstantDef->pManaulGPUSource == NULL)
			if (pConstantDef->pManaulGPUSource != pSource)
			{
				if (pSource->BuildManualConstantDefType(pConstantDef))
				{
					pConstantDef->pManaulGPUSource = pSource;
				}
				else
				{
					++nBuildFailedCount;
				}
			}

		}
		//m_bManualConstantTypeBuilded = (nBuildFailedCount == 0);
	}
}
*/

s32 GPUParameters::GetGPUVariantIndex(const FixedString& strName) const
{
    for (size_t i = 0; i < m_vtManualGPUConstDef.size(); ++i)
    {
        if (m_vtManualGPUConstDef[i].strName == strName)
        {
            return (s32)i;
        }
    }
    return -1;
}
