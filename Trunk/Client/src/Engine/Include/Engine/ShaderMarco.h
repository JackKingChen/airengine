#pragma once 
#include "InternalFixedStrings.h"
#include "MemoryAllocatorConfig.h"

typedef struct ShaderMarcoDef 
{
    ShaderMarcoDef()
    {
        memset(szName, 0, sizeof(szName));
        memset(szDefination, 0, sizeof(szDefination));
    }
    char szName[512];
    char szDefination[512];

	bool operator == (const ShaderMarcoDef& other)
	{
		return strcmp(szName, other.szName) == 0 && strcmp(szDefination, other.szDefination) == 0;
	}
}ShaderMarcoDef;

struct ShaderMarco
{
    union
    {
        struct
        {
            u32 ShadowmapOn : 1;
            u32 LightPrePassOn : 1;
            u32 ReflectionOn : 1;
            u32 RefractionOn : 1;
            u32 RenderShadowmap : 1;
            u32 RenderToGBuffer : 1;
            u32 DepthmapSoftedge : 1;
            u32 Ldr : 1;
            u32 Starfield : 1;
            u32 TexLumiance : 1;
			u32 HardwareSkin : 1;
        };
        u32 m_uMarcoValue;
    };

    ShaderMarco() : m_uMarcoValue(0)
    {

    }

    bool operator == (const ShaderMarco& right) const
    {
        return m_uMarcoValue == right.m_uMarcoValue;
    }

    bool operator != (const ShaderMarco& right) const
    {
        return m_uMarcoValue != right.m_uMarcoValue;
    }

    void SetShaderMarco(const FixedString& strName, u32 uValue)
    {
        if (strName == *FS_SHADOWMAP)
        {
            ShadowmapOn = uValue;
        }
        else if (strName == *FS_RENDER_TO_GBUFFER)
        {
            RenderToGBuffer = uValue;
        }
        else if (strName == *FS_LIGHT_PRE_PASS)
        {
            LightPrePassOn = uValue;
        }
        else if (strName == *FS_RENDER_TO_SHADOWMAP)
        {
            RenderShadowmap = uValue;
        }
        else if (strName == *FS_TEX_LUMINANCE)
        {
            TexLumiance = uValue;
        }
        else if (strName == *FS_HAS_REFLECT)
        {
            ReflectionOn = uValue;
        }
        else if (strName == *FS_HAS_REFRACT)
        {
            RefractionOn = uValue;
        }
        else if (strName == *FS_DEPTHMAP_SOFT_EDGE)
        {
            DepthmapSoftedge = uValue;
        }
		else if (strName == *FS_HARDWARE_SKIN)
		{
			HardwareSkin = uValue;
		}
    }

    void GetMarcoStrings(AIRVector<ShaderMarcoDef>& vtMarcos)
    {
        ShaderMarcoDef def;
        memset(&def, 0, sizeof(ShaderMarcoDef));
        if (ShadowmapOn == 1)
        {
            strcpy_s(def.szName, (*FS_SHADOWMAP).AsCharPtr());
            vtMarcos.push_back(def);
        }
        
        if (RenderToGBuffer == 1)
        {
            strcpy_s(def.szName, (*FS_RENDER_TO_GBUFFER).AsCharPtr());
            vtMarcos.push_back(def);
        }

        if (LightPrePassOn == 1)
        {
            strcpy_s(def.szName, (*FS_LIGHT_PRE_PASS).AsCharPtr());
            vtMarcos.push_back(def);
        }

        if (RenderShadowmap == 1)
        {
            strcpy_s(def.szName, (*FS_RENDER_TO_SHADOWMAP).AsCharPtr());
            vtMarcos.push_back(def);
        }

        if (TexLumiance == 1)
        {
            strcpy_s(def.szName, (*FS_TEX_LUMINANCE).AsCharPtr());
            vtMarcos.push_back(def);
        }

        if (ReflectionOn == 1)
        {
            strcpy_s(def.szName, (*FS_HAS_REFLECT).AsCharPtr());
            vtMarcos.push_back(def);
        }

        if (RefractionOn == 1)
        {
            strcpy_s(def.szName, (*FS_HAS_REFRACT).AsCharPtr());
            vtMarcos.push_back(def);
        }

        if (DepthmapSoftedge == 1)
        {
            strcpy_s(def.szName, (*FS_DEPTHMAP_SOFT_EDGE).AsCharPtr());
            vtMarcos.push_back(def);
        }

        if (Ldr == 1)
        {
            strcpy_s(def.szName, (*FS_LDR).AsCharPtr());
            vtMarcos.push_back(def);
        }

        if (Starfield == 1)
        {
            strcpy_s(def.szName, (*FS_STARFIELD).AsCharPtr());
            vtMarcos.push_back(def);
        }

		if (HardwareSkin == 1)
		{
			strcpy_s(def.szName, (*FS_HARDWARE_SKIN).AsCharPtr());
			vtMarcos.push_back(def);
		}
    }

    void Reset()
    {
        m_uMarcoValue = 0;
    }
};


/*
void GetShaderMarcoValue(ShaderMarco& marco, const FixedString& strMacro, const char* szDefination)
{
    if (strMacro == *FS_SHADOWMAP)
    {
        marco.ShadowmapOn = 1;
    }
    else if (strMacro == *FS_SHADOWMAP)
    {
        marco.RenderToGBuffer = 1;
    }
}
*/
