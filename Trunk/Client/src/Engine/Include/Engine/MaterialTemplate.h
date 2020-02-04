#pragma once 

#include "EngineConfig.h"
#include "ShaderMarco.h"
#include "MarkupSTL.h"
#include "FixedString.h"
#include "MemoryAllocatorConfig.h"
#include "DataStream.h"
#include "EngineDefination.h"
#include "EngineResource.h"
#include "RenderStateObject.h"
#include "GPUVariant.h"

class IGPUProgram;
class FileData;
//class GPUVariant;
class ShaderObject;

struct PassDesc 
{
    PassDesc()
    {

    }
    FixedString strVSFunctionName;
    
    //FixedString strVertexDeclaration;
    size_t      nVSIndex;
    size_t      nVSMarcoMask;
    FixedString strPSFunctionName;
    
    size_t      nPSIndex;
    size_t      nPSMarcoMask;
    BlendState blendState;
    RasterizerState rasterizerState;
    DepthStencilState depthStencilState;
    FixedPipeState fixedPipeState;    
};

struct TechniqueDesc
{
    TechniqueDesc() : nFlowDesc(FLOW_SCENE)
    {

    }

    ~TechniqueDesc()
    {

    }

    
    FixedString  strTechniqueName;
    FixedString strVSVersion;
    FixedString strPSVersion;
    //std::vector<PassDesc> m_vtPassDesces;
	AIRVector<PassDesc> m_vtPassDesces;
    int nFlowDesc;    //flow，代表当前是哪个flow
    AIRVector<ShaderMarcoDef> m_vtShaderMarcos;
    //AIRVector<ShaderMarcoDef> m_vtShaderMarcosPS;

	void AddShaderMarco(const ShaderMarcoDef& def)
	{
		for (size_t i = 0; i < m_vtShaderMarcos.size(); ++i)
		{
			if (m_vtShaderMarcos[i] == def)
			{
				return;
			}
		}
		m_vtShaderMarcos.push_back(def);
	}
};

struct stMtlParam 
{
    FixedString strName;
    GPUVariant* pVar;
    MaterialConstantType csType;
    char nSamplerIndex;

    stMtlParam() : pVar(NULL), csType(eMCT_FLOAT), nSamplerIndex(-1)
    {

    }

	stMtlParam(const stMtlParam& other) : strName(other.strName), pVar(NULL), 
		csType(other.csType), nSamplerIndex(other.nSamplerIndex)
	{
		if (other.pVar)
		{
			pVar = new GPUVariant(*other.pVar);
			//*pVar = *other.pVar;
		}
	}

	stMtlParam& operator = (const stMtlParam& other)
	{
		strName = other.strName;
		csType = other.csType;
		nSamplerIndex = other.nSamplerIndex;
		if (pVar == NULL)
		{
			SafeDelete(pVar);
			pVar = NULL;
		}
		if (other.pVar)
		{
			pVar = new GPUVariant(*other.pVar);	
			//*pVar = *other.pVar;
		}
		return *this;
	}
};

class EngineExport MaterialTemplate : public ResourceListener//, public EngineAllocator(MaterialTemplate)
{
	EngineAllocator(MaterialTemplate)

public:
    MaterialTemplate();
    virtual ~MaterialTemplate();

    virtual bool Create(CDataStream* pStream);

    bool GetShader(ShaderMarco marco, u32 nTechniqueIndex, u32 nPassIndex, IGPUProgram** pVertexShader, IGPUProgram** pPixelShader);

    void Clear();

    bool LoadFromXML(CMarkupSTL& xml);

	bool LoadFromXML(CMarkupSTL& xml, const char* szName);

	bool LoadFromStream(CDataStream* pStream, const char* szName);

    bool IsReady() const
    {
        return m_bReady;
    }

    virtual void OnResourceLoaded(Resource* pResource);
    virtual void OnResourceReady(Resource* pResource) {};

    size_t GetTechniquesNum() const
    {
        return m_vtTechniqueDesces.size();
    }

    const TechniqueDesc& GetTechniqueDesc(size_t index) const
    {
        return m_vtTechniqueDesces[index];
    }

    void GetRenderState(size_t nTechIndex, size_t nPassIndex, BlendState& blend, RasterizerState& rasterizerState, DepthStencilState& depthStencil, FixedPipeState& fixedPipe);

    ShaderObject* CreateShaderObject(IGPUProgram* pVertexShader, IGPUProgram* pPixelShader);

	size_t GetMaterialParamsNum() const
	{
		return m_vtMaterialParams.size();
	}

	void GetMaterialParam(size_t nIndex, stMtlParam& mtlParam);

	size_t GetSamplersNum() const
	{
		return m_vtSamplers.size();
	}

	bool GetSamplerObject(size_t index, FixedString& name, SamplerState& state);
protected:
private:
    typedef AIRVector<FixedString> SHADERFUNCTION_VECTOR;

    typedef AIRMap<u32, IGPUProgram*> GPUPROGRAM_MAP;

    AIRVector<GPUPROGRAM_MAP> m_vtFunctionVSPrograms; 
    AIRVector<GPUPROGRAM_MAP> m_vtFunctionPSPrograms;

    //AIRVector<ShaderObject*> m_vtShaderObjects;

	AIRVector<TechniqueDesc>  m_vtTechniqueDesces;
    //std::vector<TechniqueDesc>  m_vtTechniqueDesces;

    SHADERFUNCTION_VECTOR  m_vtVSFunctions;
    SHADERFUNCTION_VECTOR  m_vtPSFunctions;

    f32  m_fVersion;

    bool m_bReady;

    AIRVector<stMtlParam> m_vtMaterialParams;

    AIRString m_strVertexShaderFile;
    AIRString m_strPixelShaderFile;
    FileData* m_pVertexShaderFile;
    FileData* m_pPixelShaderFile;

    typedef AIRMap<u64, ShaderObject*> ShaderObject_Map;

    ShaderObject_Map m_mapShaderObjects;

	AIRVector<SamplerState> m_vtSamplers;
	AIRVector<FixedString> m_vtSamplerNames;
private:
    size_t GetVSFunctionIndex(const FixedString& strFunction);
    size_t GetPSFunctionIndex(const FixedString& strFunction);

    void AddSamplerIndex(const char* szName, u32 nIndex)
    {
        m_mapSamplerIndex.insert(std::make_pair(szName, nIndex));
    }

    AIRMap<FixedString, s32> m_mapSamplerIndex;  //只用于gles2
};
