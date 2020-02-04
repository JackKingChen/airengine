/********************************************************************
	created:	28:3:2011   9:59
	filename: 	GPUProgramManager.h
	author:		liangairan
	
	purpose:	gpuprogram�����������ڶ�gpu����Ĺ�������������
*********************************************************************/

#pragma once 

#include "EngineConfig.h"

class IGPUProgram;
class CDataStream;

typedef bool (*GPUProgramCreateCallback)(CDataStream*);

class EngineExport GPUProgramManager : public Singleton<GPUProgramManager>
{
    BE_SINGLETON(GPUProgramManager);
public:
    ~GPUProgramManager();

    void AddShaderFile(const char* szFilename, GPUProgramCreateCallback pCallback);

    IGPUProgram* GetGPUProgram(const AIRString& strName);

    //�������gpuprogram
    void Clear();

    void AddGPUProgram(IGPUProgram* pGPUProgram);

    //��������Ĭ�ϵ�shader
    void CreateDefaultShaders(const char* szShaderPatch);

	
protected:
private:

    AIRMap<AIRString, IGPUProgram*> m_mapGPUProgram;
    typedef AIRMap<AIRString, IGPUProgram*>::iterator GPUProgram_Iter;

    typedef AIRVector<AIRString> ShaderDefines;
    ShaderDefines m_vtShaderDefines;

    typedef AIRMap<AIRString, GPUProgramCreateCallback> ShaderFiles;
    //typedef std::map<AIRString, GPUProgramCreateCallback> PixelShaderFiles;

    typedef ShaderFiles::iterator ShaderFileIter;

    ShaderFiles m_mapShaderFiles;

    bool CreateDefaultShadersThroughPatchFile( const char* szShaderPatchFile = "ShaderPatch.xml" );
};

#define GPUPROGRAM_MANAGER GPUProgramManager::GetInstance()
