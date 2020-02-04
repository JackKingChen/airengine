#pragma once 
#include "EngineResource.h"
//#include "IShaderConstantTable.h"
//#include "GPUParameters.h"
//#include "ShaderParameter.h"
#include "EngineDefination.h"
#include "GPUDataParam.h"
#include "FixedString.h"
#include "ShaderMarco.h"
#include "GPUVariant.h"

class Renderable;
class IMaterial;
class ManualGPUDataParamSource;
class GPUParameters;

class CVertexDeclaration;


class EngineExport IGPUProgram
{
	EngineAllocator(IGPUProgram)
public:
    IGPUProgram(const char* szName, const char* szVersion, const char* szFunction);
    virtual ~IGPUProgram();

    virtual bool Create(CDataStream* pStream) = 0;

    virtual void Clear() = 0;

    virtual bool Use() = 0;

    //virtual IShaderConstantTable* GetConstantTable() = 0;

    //virtual const AIRString& GetName() const { return m_strName; }

    virtual void* GetInterface() = 0;

    //更新渲染物自定义的shader
    //virtual void UpdateParameters(CCamera* pCamera, Renderable* pRenderable, IMaterial* pMaterial) = 0;

    //更新系统中存在的shader
    virtual void AutoUpdateParameters(CGPUDataParamSource* pGPUDataParam) = 0;

    //
    //virtual void UpdateParameters(std::map<AIRString, CShaderParameter*>& mapShaderParams) = 0;

    //设置手动更新参数源
    //void SetManualGPUDataParamSource(ManualGPUDataParamSource* pSource);

    //const AIRString& GetFilename() const
    //{
    //    return m_strFilename;
    //}

    //void AddManualGPUDataParamSource(ManualGPUDataParamSource* pSource);

	//void RemoveAllManualGPUDataParamSource();

    //添加编译的宏
    void AddMarco(const char* szName, const char* szDefination);

	//DX11下的dynamic shader linkage
	virtual void AddClassName(const char* szClassName) {};

    void ClearMarcos()
    {
        m_vtMarcos.clear();
    }

    const AIRString& GetName() const
    {
        return m_strName;
    }

	const AIRString& GetFileName() const
	{
		return m_strFile;
	}

    virtual bool CreateVertexDeclaration(const FixedString& strVertexDeclaration);

    CVertexDeclaration* GetVertexDeclaration()
    {
        return m_pVertexDeclaration;
    }

	virtual bool SetClassInterface(const char* szClassName, const char* szClassUseName) 
	{
		return false;
	}

	GPUShaderType GetShaderType() const
	{
		return m_shaderType;
	}

	const char* GetShaderTypeStr() const
	{
		switch( m_shaderType )
		{
		case eGST_Vertex: return "VS";
		case eGST_Pixel: return "PS";
		case eGST_Geometry: return "GS";
		}
		return "Unknown";
	}

    virtual void SetGPUVariant(s32 nLocation, const GPUVariant& var) {}

    //for opengles renderer
    virtual void AddSamplerIndex(const char* szName, u32 nIndex) {}

    virtual void SetGPUVariant(const FixedString& strName, GPUVariant* pVar, MaterialConstantType csType) {}

    virtual void SetGPUVariantArray(const FixedString& strName, GPUVariantArray* varArray, MaterialConstantType csType) {}

    virtual bool GetSamplerIndex(const FixedString& strName, u32& nIndex) 
    {
        return false;
    }

    ShaderMarco GetShaderMarco()
    {
        return m_shaderMarco;
    }

    virtual void SetDefaultValues() {}

	size_t GetDataSize() const { return m_DataSize; }

	const char* GetEntryFunction() const { return m_strFunction.c_str(); }

	const char* GetVersion() const { return m_strVersion.c_str(); }
protected:
    //AIRString m_strName;

    //ManualGPUDataParamSource*  m_pManualGPUDataSource;

    //typedef AIRVector<ManualGPUDataParamSource*> ManualGPUDataSourceList;
    //typedef ManualGPUDataSourceList::iterator      ManualGPUDataSourceListIter;
    //ManualGPUDataSourceList  m_vtManualGPUDataSources;

    GPUShaderType m_shaderType;

    //GPUParameters  m_gpuParameters;    //GPU参数
    GPUParameters*   m_pGPUParameters;

    //typedef AIRVector<GPUConstantDefinition> GolbalGPUParameters;   //有defaultvalue的constant
    //typedef GolbalGPUParameters::iterator GolbalGPUParametersIter;
    //GolbalGPUParameters m_vtGolbalParameters;

    //AIRString m_strFilename; 

    AIRString m_strFunction;
    AIRString m_strVersion;

    int   m_nID;

    AIRVector<ShaderMarcoDef> m_vtMarcos;

    //void BuildDefaultConstants(GPUConstantDefinition& def);
    //AIRVector<f32>   m_vtDefaultFloatValues;
    //AIRVector<s32>   m_vtDefaultIntValues;

    AIRString m_strName;

    CVertexDeclaration* m_pVertexDeclaration;

    ShaderMarco m_shaderMarco;

	AIRString m_strFile;

	size_t m_DataSize;
protected:
    //virtual void CreateParameters() = 0;

    //virtual void SetDefaultValues() = 0;
   
private:
};
