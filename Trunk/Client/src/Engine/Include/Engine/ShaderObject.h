#pragma once 

#include "AIRTypes.h"
#include "MemoryAlloter.h"
#include "MemoryAllocatorConfig.h"
#include "EngineDefination.h"
#include "GPUParameters.h"
#include "FixedString.h"
using namespace AIR;

class IGPUProgram;
class IRenderer;
class GPUVariant;
class GPUVariantArray;

class EngineExport ShaderObject //: public EngineAllocator(ShaderObject)
{
	EngineAllocator(ShaderObject)
public:
	ShaderObject();
	virtual ~ShaderObject();

	virtual void Apply(IRenderer* pRenderer);

	virtual bool Create(IGPUProgram* pGpuPrograms[], u32 nProgramsNum);

	bool HasVertexShader() const;

	bool HasPixelShader() const;

	IGPUProgram* GetGPUProgram(GPUShaderType shaderType);

	//目的是适应DX11的dynamic shader linkage
	bool SetShaderClassInstance(GPUShaderType shaderType, const char* szInterface, const char* szClassInstance);

    virtual void SetConstantValue(s32 nLocation, u32 nCount, const void* pValue, GpuConstantType type) {}

    virtual void SetConstantValue(s32 nLocation, f32 fValue, GpuConstantType type) {}

    virtual void SetConstantValue(s32 nLocation, s32 nValue, GpuConstantType type) {}

    virtual void SetGPUVariant(s32 nLocation, const GPUVariant& var);

    virtual void UpdateMaterialParam(const FixedString& strName, GPUVariant* pVar, MaterialConstantType csType);

    virtual void UpdateMaterialParam(const FixedString& strName, GPUVariantArray* varArray, MaterialConstantType csType);

    //以后要放在materialtemplate里
    virtual void GetSamplerIndex(const FixedString& strName, u32& nIndex);

    //为了适应opengles2
    virtual void BeginUse();
protected:
	AIRVector<IGPUProgram*> m_vtGPUPrograms;

	virtual void UpdateGPUProgramParameters(IRenderer* pRenderer);
private:
};
