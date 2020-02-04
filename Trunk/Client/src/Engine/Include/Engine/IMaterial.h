

#pragma once 
#include "Common.h"
#include "EngineResource.h"
#include "Vector4.h"
//#include "IPixelShader.h"
//#include "IGPUProgram.h"
//#include "TextureUnit.h"
//#include "IShader.h"
#include "ColorValue.h"
#include "Color.h"
#include "MarkupSTL.h"
#include "EngineDefination.h"
#include "MemoryAllocatorConfig.h"
#include "FixedString.h"
#include "IGPUProgram.h"
#include "MaterialTemplate.h"
#include "RenderStateObject.h"

class Renderable;
class IRenderer;
class CLight;
class FileData;
//class CTechnique;
class ITexture;
class FixedString;
//class CPass;
class SamplerStateObject;
class ShaderObject;
class BlendStateObject;
class FixedPipeStateObject;
class DepthStencilStateObject;
class RasterizerStateObject;
class MaterialTemplate;

class IIndexBuffer;
class IVertexBuffer;

struct IVertexArrayObject
{
    IVertexArrayObject():m_vb(0),m_ib(0)
    {
        
    }
    virtual ~IVertexArrayObject()
    {
        
    }
    IVertexBuffer* m_vb;
    IIndexBuffer* m_ib;
};

struct RenderPass 
{
    RenderPass() : pShaderObject(NULL),vertexArrayObj(NULL)
    {

    }
    ShaderObject* pShaderObject;
    BlendState blendState;
    RasterizerState rasterizerState;
    DepthStencilState depthStencilState;
    FixedPipeState fixedPipeState;    
    //BlendStateObject* pBlendState;
    //FixedPipeStateObject* pFixedPipeState;
    //DepthStencilStateObject* pDepthStencilState;
    //RasterizerStateObject*   pRasterizerState;
    IVertexArrayObject* vertexArrayObj;
};

struct RenderTechnique
{
    RenderTechnique() : nFlowDesc(0), bIsReady(false)
    {

    }

    ~RenderTechnique()
    {

    }

    AIRVector<RenderPass> m_vtRenderPass;
    size_t nFlowDesc;    //flow，代表当前是哪个flow
    ShaderMarco shaderMarco;
    bool bIsReady;
};

class EngineExport IMaterial : public ResourceListener//, public EngineAllocator(IMaterial)
{
	EngineAllocator(IMaterial)
    //typedef void (IMaterial::*CallBack)(CCamera* pCamera, IMaterial* pMaterial);
public:
    IMaterial();
    virtual ~IMaterial() { }

    //创建材质，包括里面的纹理与shader的创建
    //virtual bool Create() = 0;

    //通过材质脚本创建材质
    virtual bool CreateFromFile(const char* szFilename, const char* szMaterialName);

    //注册回调函数，每帧更新时调用其注册的函数，由renderobject调用
    //virtual void RegisterCallBack(CallBack pFunction)
    //{
    //    m_lpCallbackFunc = pFunction;
    //}

    virtual bool LoadFromXML(CMarkupSTL& xml, const char* szName);

    //调用材质时需要调用的函数
    //virtual void OnSetMaterial() = 0;

    virtual u32 GetID() const { return m_nID; }

    virtual void SetID(u32 nID) { m_nID = nID; }

    virtual void Clear();

    //virtual IEffect* GetEffect() { return NULL; }

    
    virtual void SetEmissive(const CColorValue& emissive);

    virtual void SetAmbient(const CColorValue& ambient);

    virtual void SetSpecular(const CColorValue& specular);

    virtual void SetDifusse(const CColorValue& difusse);

    void SetShiness(float fShiness);

    //float GetShiness() 
    //{
    //    return m_fShiness;
    //}

    //virtual s32  GetTextureCount() const = 0;

    //virtual ITexture* GetTexture(s32 nIndex) = 0;

    //保存到文件
    virtual void SaveToFile(const char* szFilename);

	void SaveToFile(const char* szPath, const char* szFilename);

    //保存
    //virtual void Save() = 0;

    //更新材质参数，主要是更新shader中的参数
    //virtual void Update(CCamera* pCamera, Renderable* pRenderable) = 0;

    //virtual IShader* GetShader() = 0;

    //virtual s32 Release() = 0;

    //获取采样状态
    //virtual u32 GetSamplerState(u32 nSampler, u32 nSamplerState);

    //获得纹理阶段状态
    //virtual u32 GetTextureStageState(u32 nStage, u32 nStageState);

    //是否用到了vertex shader
    //virtual bool HasVertexShader() = 0;

    //是否用到了pixel shader
    //virtual bool HasPixelShader() = 0;

    bool IsTransparent();

    bool IsWireFrame() const;

    void SetFillMode(RenderSystemFillMode fillMode);

    //获得纹理单元数
    virtual u32 GetTextureUnitCount() const
    {
        return m_nTextureCount;
    }

    //设置纹理采样状态
    //virtual void SetSamplerState(u32 nSampler, u32 nSamplerState, u32 nValue);

    //设置纹理阶段状态
    //virtual void SetTextureStageState(u32 nStage, u32 nStageState, u32 nValue);

    virtual void SetTransparent(bool bAlpha);

    const AIRString& GetName() const
    {
        return m_strName;
    }

    //材质克隆
    virtual void DeepCopy(IMaterial* pCopy);

    //设置纹理单元中的纹理
    //@nStage         纹理阶段
    //@szTextureFile  纹理文件
    //virtual void SetTextureUnitTex(int nStage, const char* szTextureFile, int nMipmap = 0);

    //virtual void SetTextureUnitTex(int nStage, ITexture* pTexture);


    //运用材质，渲染时用到
    //virtual void ApplyMaterial(IRenderer* pRenderer);

    //获得所属文件名
    //virtual const AIRString& GetFileName() const
    //{
    //    return m_strFilename;
    //}

    virtual bool IsLightingEnable(); // { return false; }

	virtual bool IsFogEnable();

    typedef AIRList<CLight*> LightList;
    //virtual void UseLights(LightList& lstLights) = 0;

    
    const CColorValue& GetEmissive() const
    {
        return m_Ke;
    }

    const CColorValue& GetSpecular() const
    {
        return m_Ks;
    }

    const CColorValue& GetAmbient() const
    {
        return m_Ka;
    }

    const CColorValue& GetDiffuse() const
    {
        return m_Kd;
    }
    
    float GetShininess() const
    {
        return m_fShiness;
    }

    virtual s32 Release()
    {
        return 0;
    }
	//
	//void SetClipPlaneEnable(bool bEnable);

	//void SetClipPlane(int nIndex, const Plane3Df& clipPlane);

    //设置光照
    void SetLightEnable(bool bEnable);

	void SetFogEnable( bool bEnable );

    //设置深度缓冲
    void SetZBufferEnable(bool bEnable);

    //设置ZWrite
    void SetZWriteEnable(bool bEnable);

    //设置cullmode
    //void SetCullMode(u32 nCullMode);

    //设置z-bias
    //void SetDepthBias(float fBias);

    //void SetSlopeScaleDepthBias(float fBias);

    /*
    CTechnique* GetCurrentTechnique()
    {
        return m_pCurrentTechnique;
    }

    CTechnique* GetTechnique(size_t index)
    {
        if (m_vtTechniques.size() > index)
        {
            return m_vtTechniques[index];
        }
        return NULL;
    }

    CTechnique* GetTechnique(const AIRString& strName);
    

    size_t GetTechniquesNum() const
    {
        return m_vtTechniques.size();
    }

    void AddTechnique(CTechnique* pTechnique);
    */

    void SetBlendSrc(RenderStateBlendValue src);

    void SetBlendDest(RenderStateBlendValue dest);

    void SetBlendOpt(BlendOpt opt);

    //设置alphatest
    void SetAlphaTest(bool bAlphaTest);

    void SetAlphaRef(u32 nRef);

    void SetName(const char* szName);

    void SetCullMode(CullMode cull);

    //void RemoveTechique(int nIndex);

    virtual bool Cache();

    //virtual bool Load();

    //检查资源状态
    void CheckResourceStatus();

    void NeedCheck()
    {
        m_bNeedCheck = true;
    }

    virtual void OnResourceLoaded(Resource* pResource)
    {
        m_bNeedCheck = true;
    }

    virtual void OnResourceReady(Resource* pResource)
    {

    }

    bool IsNeedCheck() const
    {
        return m_bNeedCheck;
    }

    bool IsReady() const
    {
        return m_bReady;
    }

    void SetIsReady(bool bReady)
    {
        m_bReady = bReady;
    }

    void SetMaterialName(const char* szName)
    {
        m_strMaterialName = szName;
    }

    const AIRString& GetMaterialName() const
    {
        return m_strMaterialName;
    }

    void SetStencilEnable(bool bEnable);

    f32 GetVersion() const
    {
        return m_fVersion;
    }

    //从materialtemplate里创建
    bool CreateFromTemplate(const FixedString& strFile);
	bool CreateFromTemplate(const FixedString& strName, CDataStream* pStream);

    RenderTechnique* GetRenderTechnique(const ShaderMarco& marco, size_t nIndex);

    MaterialTemplate* GetMaterialTemplate() 
    {
        return m_pMaterialTemplate;
    }

public:
    
    void SetMaterialParam(const FixedString& strName, const GPUVariant& var);

    bool GetMaterialIndexByName(const FixedString& strName, size_t& index);

    //void UpdateMaterialParams(CPass* pPass);

    //新方法
    void UpdateMaterialParams(RenderPass* pPass);

    void AddMaterialParam(const FixedString& strName, MaterialConstantType type, const GPUVariant& var);

    size_t GetTechniqueIndexByFlow(eRenderFlow flow);

    void SetCurShaderMarco(ShaderMarco marco)
    {
        m_curShaderMarco = marco;
    }

    ShaderMarco GetCurShaderMarco() const
    {
        return m_curShaderMarco;
    }

    void AddSamplerState(const FixedString& strName, const SamplerState& state);

    size_t GetSamplerStateIndex(const FixedString& strName);
protected:
    virtual bool LoadFromXML(CMarkupSTL& xml);
protected:
    //IMaterialCallBack*  m_pCallBack;
    u32 m_nID;
    u32 m_nTextureCount;     //纹理单元数量，并不是真实的纹理数量
    
    //AIRString m_strName;        //材质名
    AIRString m_strMaterialName;   //材质名
    //AIRString m_strFilename; 

    /*
    typedef AIRVector<CTextureUnit*> VECTOR_TEXTUREUNIT;
    typedef VECTOR_TEXTUREUNIT::iterator  TEXTUREUNIT_ITER;
    AIRVector<CTextureUnit*>   m_vtTextureUnit;
    */

    CColorValue m_Ke;       //材质自发射光emissive系数
    CColorValue m_Ka;       //环境反射光ambient系数
    CColorValue m_Ks;       //镜面反射项specular系数
    CColorValue m_Kd;       //漫反射difusse系数
    float       m_fShiness;  //镜面反射光强度系统
    
    
    //CTechnique* m_pCurrentTechnique;    //当前使用的technique
    
    //typedef AIRVector<CTechnique*> TechniqueArray;
    //typedef TechniqueArray::iterator TechniqueArray_Iter;
    //TechniqueArray m_vtTechniques;    //technique

private:
    //u32 m_nCallBack;
    bool m_bNeedCheck;   //是否需要检查，当启动后台线程后，只有在后台线程才会写这个变量。主线程只会读
    FileData*  m_pFileData;
    AIRString m_strName;
    bool m_bReady;
    bool m_bFileLoaded;   //材质文件是否加载完成
    //bool m_bTechniqueReady;   //technique是否已经加载完

    f32  m_fVersion;

    MaterialTemplate* m_pMaterialTemplate;

    AIRVector<stMtlParam> m_vtMaterialParams;

    /*
    struct stSampler
    {
        stSampler() : m_texAddr(TAV_Warp), m_texFilter(TFT_Point), pTexture(NULL), pSamplerStateObject(NULL)
        {

        }
        FixedString strName;
        TextureAddressValue m_texAddr;
        TextureFilterOption m_texFilter;
        ITexture* pTexture;
        SamplerStateObject* pSamplerStateObject;
    };

    AIRVector<stSampler> m_vtSamplers;
    */
    AIRVector<SamplerState> m_vtSamplers;
    AIRVector<FixedString> m_vtSamplerNames;

    //当前正在使用的technique
    AIRVector<RenderTechnique> m_vtCurrentRenderTechs;
    AIRVector<size_t> m_vtTechFlowIndices;

    ShaderMarco m_curShaderMarco;

    bool GetRenderPass(const ShaderMarco& shaderMarco, size_t nTechIndex, size_t nPassIndex, RenderPass& pass);
};
