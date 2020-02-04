#include "stdafx.h"
#include "IGPUProgram.h"
#include "Engine.h"
//#include "ManualGPUDataParamSource.h"
#include "GPUParameters.h"
#include "VertexDeclarationManager.h"
#include "DebugTools.h"
#include "GPUVariant.h"

int g_gpuPrograms = 0;




IGPUProgram::IGPUProgram(const char* szName, const char* szVersion, const char* szFunction) : m_strName(szName)
, m_strVersion(szVersion)
, m_strFunction(szFunction)
, m_pGPUParameters(NULL)
, m_pVertexDeclaration(0)
, m_DataSize(0)
{
    m_pGPUParameters = ENGINE_INST->GetRenderer()->CreateGPUParameters();
}

IGPUProgram::~IGPUProgram()
{
    //m_vtManualGPUDataSources.clear();
    SafeDelete(m_pGPUParameters);
    //if (m_pVertexDeclaration)
    //{
    //    m_pVertexDeclaration->Clear();
    //    SafeDelete(m_pVertexDeclaration);
    //}
    //SafeDelete(m_pVertexDeclaration);
}

/*
void IGPUProgram::AddManualGPUDataParamSource(ManualGPUDataParamSource* pSource)
{
    ManualGPUDataSourceListIter it = std::find(m_vtManualGPUDataSources.begin(), m_vtManualGPUDataSources.end(), pSource);
    if (it == m_vtManualGPUDataSources.end())
    {
        m_vtManualGPUDataSources.push_back(pSource);
    }    
}
*/
//void IGPUProgram::RemoveAllManualGPUDataParamSource()
//{
//	m_vtManualGPUDataSources.clear();
//	//m_gpuParameters.SetManualConstantTypeBuilded( false );
//}

void IGPUProgram::AddMarco(const char* szName, const char* szDefination)
{
    ShaderMarcoDef marco;
    strcpy(marco.szName, szName);
    if (szDefination)
    {
        strcpy(marco.szDefination, szDefination);
    }
    
    m_vtMarcos.push_back(marco);

    m_shaderMarco.SetShaderMarco(szName, 1);
}

bool IGPUProgram::CreateVertexDeclaration(const FixedString& strVertexDeclaration)
{
	if (m_pVertexDeclaration == NULL)
	{
		m_pVertexDeclaration = VERTEXDECL_MANAGER->CreateVertexDeclaration(strVertexDeclaration, this);
	}
    //VertexDeclarationManager::ElementVector& elements = VERTEXDECL_MANAGER->GetElementVector(strVertexDeclaration);
    //m_pVertexDeclaration = ENGINE_INST->GetRenderer()->CreateVertexDeclaration();

    //if (m_pVertexDeclaration)
    //{
    //    m_pVertexDeclaration->Create(elements);
    //}
    
    return m_pVertexDeclaration != NULL;
}

