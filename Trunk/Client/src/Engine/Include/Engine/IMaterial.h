

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
    size_t nFlowDesc;    //flow������ǰ���ĸ�flow
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

    //�������ʣ����������������shader�Ĵ���
    //virtual bool Create() = 0;

    //ͨ�����ʽű���������
    virtual bool CreateFromFile(const char* szFilename, const char* szMaterialName);

    //ע��ص�������ÿ֡����ʱ������ע��ĺ�������renderobject����
    //virtual void RegisterCallBack(CallBack pFunction)
    //{
    //    m_lpCallbackFunc = pFunction;
    //}

    virtual bool LoadFromXML(CMarkupSTL& xml, const char* szName);

    //���ò���ʱ��Ҫ���õĺ���
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

    //���浽�ļ�
    virtual void SaveToFile(const char* szFilename);

	void SaveToFile(const char* szPath, const char* szFilename);

    //����
    //virtual void Save() = 0;

    //���²��ʲ�������Ҫ�Ǹ���shader�еĲ���
    //virtual void Update(CCamera* pCamera, Renderable* pRenderable) = 0;

    //virtual IShader* GetShader() = 0;

    //virtual s32 Release() = 0;

    //��ȡ����״̬
    //virtual u32 GetSamplerState(u32 nSampler, u32 nSamplerState);

    //�������׶�״̬
    //virtual u32 GetTextureStageState(u32 nStage, u32 nStageState);

    //�Ƿ��õ���vertex shader
    //virtual bool HasVertexShader() = 0;

    //�Ƿ��õ���pixel shader
    //virtual bool HasPixelShader() = 0;

    bool IsTransparent();

    bool IsWireFrame() const;

    void SetFillMode(RenderSystemFillMode fillMode);

    //�������Ԫ��
    virtual u32 GetTextureUnitCount() const
    {
        return m_nTextureCount;
    }

    //�����������״̬
    //virtual void SetSamplerState(u32 nSampler, u32 nSamplerState, u32 nValue);

    //��������׶�״̬
    //virtual void SetTextureStageState(u32 nStage, u32 nStageState, u32 nValue);

    virtual void SetTransparent(bool bAlpha);

    const AIRString& GetName() const
    {
        return m_strName;
    }

    //���ʿ�¡
    virtual void DeepCopy(IMaterial* pCopy);

    //��������Ԫ�е�����
    //@nStage         ����׶�
    //@szTextureFile  �����ļ�
    //virtual void SetTextureUnitTex(int nStage, const char* szTextureFile, int nMipmap = 0);

    //virtual void SetTextureUnitTex(int nStage, ITexture* pTexture);


    //���ò��ʣ���Ⱦʱ�õ�
    //virtual void ApplyMaterial(IRenderer* pRenderer);

    //��������ļ���
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

    //���ù���
    void SetLightEnable(bool bEnable);

	void SetFogEnable( bool bEnable );

    //������Ȼ���
    void SetZBufferEnable(bool bEnable);

    //����ZWrite
    void SetZWriteEnable(bool bEnable);

    //����cullmode
    //void SetCullMode(u32 nCullMode);

    //����z-bias
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

    //����alphatest
    void SetAlphaTest(bool bAlphaTest);

    void SetAlphaRef(u32 nRef);

    void SetName(const char* szName);

    void SetCullMode(CullMode cull);

    //void RemoveTechique(int nIndex);

    virtual bool Cache();

    //virtual bool Load();

    //�����Դ״̬
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

    //��materialtemplate�ﴴ��
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

    //�·���
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
    u32 m_nTextureCount;     //����Ԫ��������������ʵ����������
    
    //AIRString m_strName;        //������
    AIRString m_strMaterialName;   //������
    //AIRString m_strFilename; 

    /*
    typedef AIRVector<CTextureUnit*> VECTOR_TEXTUREUNIT;
    typedef VECTOR_TEXTUREUNIT::iterator  TEXTUREUNIT_ITER;
    AIRVector<CTextureUnit*>   m_vtTextureUnit;
    */

    CColorValue m_Ke;       //�����Է����emissiveϵ��
    CColorValue m_Ka;       //���������ambientϵ��
    CColorValue m_Ks;       //���淴����specularϵ��
    CColorValue m_Kd;       //������difusseϵ��
    float       m_fShiness;  //���淴���ǿ��ϵͳ
    
    
    //CTechnique* m_pCurrentTechnique;    //��ǰʹ�õ�technique
    
    //typedef AIRVector<CTechnique*> TechniqueArray;
    //typedef TechniqueArray::iterator TechniqueArray_Iter;
    //TechniqueArray m_vtTechniques;    //technique

private:
    //u32 m_nCallBack;
    bool m_bNeedCheck;   //�Ƿ���Ҫ��飬��������̨�̺߳�ֻ���ں�̨�̲߳Ż�д������������߳�ֻ���
    FileData*  m_pFileData;
    AIRString m_strName;
    bool m_bReady;
    bool m_bFileLoaded;   //�����ļ��Ƿ�������
    //bool m_bTechniqueReady;   //technique�Ƿ��Ѿ�������

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

    //��ǰ����ʹ�õ�technique
    AIRVector<RenderTechnique> m_vtCurrentRenderTechs;
    AIRVector<size_t> m_vtTechFlowIndices;

    ShaderMarco m_curShaderMarco;

    bool GetRenderPass(const ShaderMarco& shaderMarco, size_t nTechIndex, size_t nPassIndex, RenderPass& pass);
};
