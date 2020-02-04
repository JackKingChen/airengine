#include "stdafx.h"
#include "ShaderObject.h"
#include "IRenderer.h"

ShaderObject::ShaderObject()
{

}

ShaderObject::~ShaderObject()
{
	
}

void ShaderObject::Apply(IRenderer* pRenderer)
{
	for (size_t i = 0; i < m_vtGPUPrograms.size(); ++i)
	{
		switch (m_vtGPUPrograms[i]->GetShaderType())
		{
		case eGST_Vertex:
			pRenderer->SetVertexShader(m_vtGPUPrograms[i]);
			break;
		case eGST_Pixel:
			pRenderer->SetPixelShader(m_vtGPUPrograms[i]);
			break;
		}

        
	}

	UpdateGPUProgramParameters(pRenderer);
}

bool ShaderObject::Create(IGPUProgram* pGpuPrograms[], u32 nProgramsNum)
{
	m_vtGPUPrograms.clear();
	for (u32 i = 0; i < nProgramsNum; ++i)
	{
		m_vtGPUPrograms.push_back(pGpuPrograms[i]);
	}

	return true;
}

void ShaderObject::UpdateGPUProgramParameters(IRenderer* pRenderer)
{
	for (size_t i = 0; i < m_vtGPUPrograms.size(); ++i)
	{
		m_vtGPUPrograms[i]->AutoUpdateParameters(pRenderer->GetGPUDataParam());
	}
}

bool ShaderObject::HasVertexShader() const
{
	for (size_t i = 0; i < m_vtGPUPrograms.size(); ++i)
	{
		if (m_vtGPUPrograms[i]->GetShaderType() == eGST_Vertex)
		{
			return true;
		}
	}

	return false;
}

bool ShaderObject::HasPixelShader() const
{
	for (size_t i = 0; i < m_vtGPUPrograms.size(); ++i)
	{
		if (m_vtGPUPrograms[i]->GetShaderType() == eGST_Pixel)
		{
			return true;
		}
	}

	return false;
}

IGPUProgram* ShaderObject::GetGPUProgram(GPUShaderType shaderType)
{
    IGPUProgram* pGpuProgram = NULL;
	for (size_t i = 0; i < m_vtGPUPrograms.size(); ++i)
	{
        pGpuProgram = m_vtGPUPrograms[i];
		if (pGpuProgram && pGpuProgram->GetShaderType() == shaderType)
		{
			return pGpuProgram;
		}
	}

	return NULL;
}

bool ShaderObject::SetShaderClassInstance(GPUShaderType shaderType, const char* szInterface, const char* szClassInstance)
{
	for (size_t i = 0; i < m_vtGPUPrograms.size(); ++i)
	{
		if (m_vtGPUPrograms[i]->GetShaderType() == shaderType)
		{
			if (m_vtGPUPrograms[i]->SetClassInterface(szInterface, szClassInstance))
			    break;
		}
	}

	return false;
}

void ShaderObject::SetGPUVariant(s32 nLocation, const GPUVariant& var)
{
    for (size_t i = 0; i < m_vtGPUPrograms.size(); ++i)
    {
        
    }
}

void ShaderObject::UpdateMaterialParam(const FixedString& strName, GPUVariant* pVar, MaterialConstantType csType)
{
    for (size_t i = 0; i < m_vtGPUPrograms.size(); ++i)
    {
        m_vtGPUPrograms[i]->SetGPUVariant(strName, pVar, csType);
    }
}

void ShaderObject::GetSamplerIndex(const FixedString& strName, u32& nIndex)
{
    for (size_t i = 0; i < m_vtGPUPrograms.size(); ++i)
    {
        if (m_vtGPUPrograms[i]->GetSamplerIndex(strName, nIndex))
        {
            break;
        }
    }
}

void ShaderObject::UpdateMaterialParam(const FixedString& strName, GPUVariantArray* varArray, MaterialConstantType csType)
{
    for (size_t i = 0; i < m_vtGPUPrograms.size(); ++i)
    {
        m_vtGPUPrograms[i]->SetGPUVariantArray(strName, varArray, csType);
    }
}

void ShaderObject::BeginUse()
{
    for (size_t i = 0; i < m_vtGPUPrograms.size(); ++i)
    {
        m_vtGPUPrograms[i]->SetDefaultValues();
    }
}
