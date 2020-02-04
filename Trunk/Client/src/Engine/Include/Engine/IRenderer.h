#pragma once

//#include "TextureMgr.h"
#include "Color.h"
#include "Common.h"
//#include "IFontBase.h"
#include "Matrix4.h"
#include "IVertexBuffer.h"
#include "ColorValue.h"
#include "Color.h"
#include "IEffect.h"
#include "IIndexBuffer.h"
#include "IGPUProgram.h"
//#include "IPixelShader.h"
//#include "IMaterial.h"
//#include "RenderData.h"
#include "GPUDataParam.h"
#include "EngineDefination.h"
//#include "Dimension2D.h"
#include "RenderStateObject.h"
#include "RTTI.h"
#include "IMaterial.h"
#include "DepthStencil.h"

class CRenderData;
class GPUParameters;
class DepthStencil;
class ScreenQuad;
// ��Ⱦ״̬MAP��
//typedef std::map< AIRString, u32 > MAP_RENDER_PARM;
// ���������Ⱦ״̬����
//typedef std::map< u32, AIRString > MAP_RENDER_NAME;

typedef AIRVector< Plane3Df > VEC_CLIPPLANE;

class RenderTarget;
class Primitive;
class ITexture;
struct TextureParam;
struct tagGPUDeviceRule;
class IMaterial;
//class CPass;
//class TextureUnitBlend;
class IOcclusionCull;
//class CTechnique;
class TextureRenderTarget;
class ShaderObject;
class MaterialTemplate;
class CVertexDeclaration;
class RenderWindow;


class EngineExport IRenderer //: public EngineAllocator(IRenderer)
{
	EngineAllocator(IRenderer)
	DECLARE_ROOT_RTTI;
    
public:
	IRenderer(); 
    virtual ~IRenderer();

    //virtual bool Initialize(HWND hWnd) = 0;

    virtual void SetTexture(int nStage, ITexture* pTex) = 0;

    //virtual bool Draw2DImage(const AIR::Rect& rectScreen, const AIR::Rectf& rectUV, ITexture* pTex, const CColorValue& color = CColorValue::White) = 0;

    virtual bool Draw2DImage(const AIR::Rectf& rectScreen, const AIR::Rectf& rectUV, ITexture* pTex, const CColorValue& color = CColorValue::White) = 0;

    //��2Dͼ��������RHW
    //virtual bool Draw2DImageOH(const AIR::Rectf& rectScreen, const AIR::Rectf& rectUV, ITexture* pTex, f32 fDepth, const SColor& color = SColor(255, 255, 255, 255)) = 0;

    //������
    //virtual void DrawRect(const AIR::Rect& rectScreen, f32 fLineWidth, const SColor& color = 0xffffffff) = 0;

    virtual void DrawRect(const AIR::Rectf& rectScreen, f32 fLineWidth, const SColor& color = 0xffffffff) = 0;

	virtual void DrawScreenQuad(const AIR::Rectf& rectScreen, const AIR::Rectf& rectUV, f32 fUVScale, const CColorValue& color, IMaterial* pMaterial);

    virtual void Release();

    virtual void BeginScene() = 0;

    virtual void EndScene() = 0;

    virtual void ClearScene(const SColor& clrBackground, bool bClearDepth, bool bClearStencil) = 0;

    //virtual void Present(const RECT* pRectSource, const RECT* pRectDest, HWND hDestWnd) = 0;

    //����Ĭ��״̬
    virtual void Reset() = 0;

    //���û��
    //virtual void EnableColorBlend() = 0;

    //ȡ�����
    //virtual void DisableColorBlend() = 0;

    //�����������
    virtual bool SetTextureFilterPoint(s32 nStage, u32 nType) = 0;

    virtual bool SetTextureFilterLinear(s32 nStage, u32 nType) = 0;


    //������Ⱦ����ص�����
    virtual ITexture* CreateTexture() = 0;

    virtual IVertexBuffer* CreateVertexBuffer(u32 nVertexCount, u32 nUnitSize, bool bDynamic = false) = 0;

    //�������㻺��
    //@type ��������
    //@lpBuffer �����ڴ�����
    //@nVertexCount �������
    //bDynamic���Ƿ�Ϊ��̬����
    virtual IVertexBuffer* CreateVertexBuffer( void* lpBuffer, u32 nVertexCount, u32 nUnitSize, bool bDynamic = false ) = 0;

	//************************************
	// Method:    CreateVertexDeclaration
	// FullName:  IRenderer::CreateVertexDeclaration
	// Access:    virtual public 
	// Returns:   CVertexDeclaration*
	// Qualifier: ���������ʽ��������
	// Parameter: void
	//************************************
	virtual CVertexDeclaration* CreateVertexDeclaration( void ) = 0;

    virtual void ClearVertexBuffer(IVertexBuffer*& pVB) = 0;

    virtual void ClearIndexBuffer( IIndexBuffer*& pIB) = 0;

	virtual void SetVertexDeclaration( CVertexDeclaration* pVD ) = 0;

    //���ö��㻺��
    virtual void SetVertexBuffer(u32 nStream, IVertexBuffer* pVertexBuffer) = 0;

    //ֻ���ö��㻺�壬�����ö�������
    //virtual void SetVertexStream(u32 nStream, IVertexBuffer* pVertexBuffer) = 0;

    //����instancing����
    //virtual void SetStreamSourceFreq(u32 nStream, u32 nInstancesNum, bool bInstanceData) = 0;

    //����instancing����
    //virtual void FreeStreamSourceFreq(u32 nStream) = 0;

    virtual void SetIndexBuffer(IIndexBuffer* pIndexBuffer) = 0;


    virtual void SetWorldTransform(const Matrix4f* pMat) = 0;

    virtual void SetViewTransform(const Matrix4f* pMat) = 0;

    virtual void SetProjectionTransform(const Matrix4f* pMat) = 0;

    //��������Ѱַģʽ
    virtual void SetTextureAddressMode(u32 nStage, TextureAddressValue nAddressMode) = 0;

    //�����������ģʽ
    virtual void SetTextureFilter(u32 nStage, TextureFilterOption nFilter) = 0;

    virtual u32 GetRenderTargetWidth() const = 0;

    virtual u32 GetRenderTargetHeight() const = 0;

    //virtual void SetType(u8 nType) { m_nAPIType = nType; }

    //virtual u8 GetType() { return m_nAPIType; };

    //virtual IEffect* CreateEffect(const char* szFilename) = 0;

    //virtual void ClearEffect(IEffect* pEffect) = 0;

    //virtual void SetRenderTarget(RenderTarget* pRenderTarget) = 0;

    //virtual void SetRenderTarget(u32 nIndex, RenderTarget* pRenderTarget) = 0;

	virtual void SetRenderTargets(u32 nRenderTargetsNum, RenderTarget** pRenderTargets) = 0;

	virtual void SetRenderTargetsEx(u32 nRenderTargetsNum, TextureRenderTarget** pRenderTargets, DepthStencil* pDepthStencil) = 0;

	virtual void SetDepthStencil(DepthStencil* pDepthStencil) = 0;

    virtual void GetViewMatrix(Matrix4f* mat) const = 0;

    virtual void GetWorldMatrix(Matrix4f* mat) const = 0;

    virtual void GetProjectionMatrix(Matrix4f* mat) const = 0;

    //����IndexBuffer��
    //@nIndexCount    ��������
    virtual IIndexBuffer* CreateIndexBuffer(u32 nIndexCount/*nSize*/, bool bDynamic = false) = 0;

	virtual IIndexBuffer* CreateIndexBuffer(u32 nIndexCount, bool bDynamic, IndexType type, bool bUseSystemMemory = false ) = 0;


    virtual IGPUProgram* CreateVertexShader(const char* szName, const char* szVersion, const char* szFunction = "VSMain") = 0;

    virtual IGPUProgram* CreatePixelShader(const char* szName, const char* szVersion, const char* szFunction = "PSMain") = 0;

    virtual void SetVertexShader(IGPUProgram* pShader) = 0;

    virtual void SetPixelShader(IGPUProgram* pShader) = 0;

    //virtual void ClearShader(IGPUProgram* pShader) = 0;

    //��ÿ֡��Ⱦǰ�ļ��
    virtual void CheckBeforeRender() = 0;

    //virtual IMaterial* GetMaterial(const char* szName) = 0;

    //virtual ITexture*  GetRenderTarget(const AIRString& strName) = 0;

    virtual void SetFillMode(bool bWireFrame) = 0;

    virtual void GetBackBufferSize(Vector2D& dimension) = 0;

    virtual void GetBackBufferSize(int& nWidth, int& nHeight) const = 0;

    //������Ⱦ״̬��ʵ����������shader֮���
    //virtual void ApplyRenderState(IShader& shader) = 0;

    //���þ���ĳ����Ⱦ״̬
    virtual void SetRenderState(u32 nState, u32 nValue) = 0;

    //����Ĭ����Ⱦ״̬
    virtual void SetDefaultRenderState() = 0;

    //����scissor
    virtual void SetScissorEnable(bool bEnable) = 0;

    //����scissor�ľ���
    virtual void SetScissorRect(const RECT* pRect) = 0;

    //��2Dֱ��
    virtual void Draw2DLine(int nStartX, int nStartY, int nEndX, int nEndY, float fWidth = 1.0f, const SColor& color = 0xffffffff) = 0;

	virtual void SetViewPort( f32 l, f32 t, f32 r, f32 b, f32 n = 0, f32 f = 1 ) = 0;

	//���ò���״̬
	virtual void SetSamplerState(u32 nSampler, u32 Type, u32 Value) = 0;

	//��������׶�״̬
	virtual void SetTextureStageState(u32 nStage, u32 Type, u32 Value) = 0;

	//����豸��״̬
	virtual EDeviceState GetDeviceState() = 0;

	//�豸��ʧ�Ĵ���
	//virtual void	OnLostDevice( void ) = 0;

	//���������豸�Ĵ���
	//virtual void	OnResetDevice( void ) = 0;

    //��Ⱦ����
    virtual void    Render(CRenderData& renderData) = 0;

    //���û�����
    virtual void    SetAmbientLight(const CColorValue& color) = 0;

    //������
    virtual void    SetFog(FogMode mode, const CColorValue& fogColor, float fDensity, float fStart, float fEnd) = 0;
    
    //������Ⱦ����ʲ���
    virtual void    SetSurfaceMaterialParam(const CColorValue& emissive, const CColorValue& specular, const CColorValue& diffuse,
        const CColorValue& ambient, float fShiness) = 0;

    virtual unsigned short  GetCurrentTexturesNum() const
    {
        return m_nCurrentTextures;
    }

    virtual void SetCurrentTexturesNum(unsigned int nCount) = 0;

    CGPUDataParamSource*  GetGPUDataParam()
    {
        return &m_gpuDataParam;
    }

    virtual void BindGPUParameter(GPUShaderType shaderType, GPUParameters* pGPUParameters) = 0;

    virtual void SetTextureTransform(int nTexture, const Matrix4f& mat, bool bDisable) = 0;

    //ʹ�ù��գ��������Թ̶����ߵ�
    //@lights   ��������
    //@nLimits  ��������
    virtual void    UseLights(AIRList<CLight*>& lights, unsigned short nLimits) = 0;
    
    //������Ⱦ״̬
    //virtual IShader* CreateShader(CPass* pPass) = 0;

    //���vertexshader�İ汾
    //virtual u32 GetVertexShaderVersion(const AIRString& strVertexShaderVersion) = 0;

    //���pixelshader�İ汾
    //virtual u32 GetPixelShaderVersion(const AIRString& strPixelShaderVerion) = 0;

    //���������˵�cap
    virtual u32 GetTextureFilterCap(u32 nFilter) = 0;

	//virtual ITexture* GetTexture( const AIRString& szName )  = 0;

    //�ж��Ƿ�֧��
    virtual bool IsGPURuleSupported(const tagGPUDeviceRule* pRule) = 0;

    /*
    inline void SetCurrentPass(CPass* pPass)
    {
        m_pCurrentPass = pPass;
    }

    CPass* GetCurrentPass() 
    {
        return m_pCurrentPass;
    }
    */

    virtual void NeedResetDevice()
    {
       
    }

	virtual bool GetNeedResetDevice( void ) = 0;

	virtual void AddClipPlane( const Plane3Df& clipPlane );

	virtual void SetClipPlanes( const VEC_CLIPPLANE& vtPlanes );

	virtual void ResetClipPlanes( void );

	virtual void ApplyClipPlanes( bool bVertexProgramBound );

	virtual void SetClipPlanesImpl( const VEC_CLIPPLANE& vtPlanes, bool bVertexProgramBound ) = 0;

    virtual bool    CheckCurrentDeviceFormat(RenderSystemUsage usage, RenderSystemResourceType resType, RendererPixelFormat format) = 0;

    //�ж��Ƿ�֧��multirendertarget
    virtual bool IsSupportMultiRenderTarget() const = 0;

    //�������mrt����
    virtual int GetMaxMultiRTTsNum() const = 0;
    //���ú󻺳��С
    //virtual void SetBackBufferSize(int nWidth, int nHeight) = 0;

    virtual bool SaveBackbufferToFile(const char* szFilename) = 0;

	//lbh ���òü�״̬
	virtual void SetCullMode(CullMode nCM) = 0;

    //����Ũ��״̬
    virtual void SetDepthEnable(bool bEnable) = 0;

    virtual void SetZWriteEnable(bool bEnable) = 0;

    virtual void SetFillMode(RenderSystemFillMode fillMode) = 0;

    //���õ����Ⱦ״̬
    virtual void SetPointSizeState(float fPointSize, float fPointSizeMin = 1.0f, float fPointSizeMax = 64.0f) = 0;

    virtual void SetPointScale(bool bPointScaleEnable, float fPointScaleA = 1.0f, float fPointScaleB = 0.0f, float fPointScaleC = 0.0f) = 0;

    virtual void SetPointSpriteEnable(bool bEnable) = 0;

    //RendererPixelFormat GetPixelFormatByName(const char* szName);

	//virtual bool TestCompileShader( const char* szFilename, const char* szVersion, const char* szFunction ) = 0;


    virtual void DrawPrimitive(Primitive* pPrimitive) = 0;

    //��������rendertargets
    //void RenderAllRenderTargets();

    //����rendertarget
    //void AttachRenderTarget(RenderTarget* pRenderTarget);

    //void DetachRenderTarget(RenderTarget* pRenderTarget);

    //void DestroyRenderTarget(RenderTarget* pRenderTarget);

    //void DestroyAllRenderTargets();

    //RenderTarget* GetRenderTarget(const AIRString& strName);

    //void DetachAllRenderTargets();

    virtual void SetBackGroundColor(SColor color) = 0;

    //����������
    //virtual void SetTextureBlendMode(u32 nStage, const TextureUnitBlend& textureBlend) = 0;

    //��������border��ɫ
    virtual void SetTextureBorderColor(u32 nStage, u32 color) = 0;

    virtual void SetDepthBias(const RasterizerState& rasterizer, f32 fDepthBias, f32 fScoleScaleBias) = 0;

    virtual RenderWindow* CreateRenderWindow(const char* szName, u32 nHandle, bool bSwapChain) = 0;

	RenderWindow* GetRenderWindow(const char* szName);

	virtual IOcclusionCull* CreateOcclusionCull() = 0;

    virtual BlendStateObject* CreateBlendStateObject() = 0;

    virtual DepthStencilStateObject* CreateDepthStencilStateObject() = 0;

    virtual RasterizerStateObject* CreateRasterizerStateObject() = 0;

    virtual FixedPipeStateObject* CreateFixedPipeStateObject() = 0;

    virtual SamplerStateObject* CreateSamplerStateObject(u32 nStage = 0) = 0;

    virtual GPUParameters* CreateGPUParameters() = 0;

	virtual f32 GetPixelOffset()
	{
		return 0.0f;
	}

	//��Ⱦinstancing
	//virtual void DrawInstances(IVertexBuffer* pVertexBuffer, IVertexBuffer* pInstanceVB, Primitive* pPrimitive, u32 nInstanceNum, CTechnique* pTechnique, u32 nIndexCount) = 0;

	virtual RenderWindow* GetCurrentRenderWindow()
	{
		return m_pRenderWindow;
	}

	void SetCurrentRenderWindow(RenderWindow* pRenderWindow)
	{
		m_pRenderWindow = pRenderWindow;
	}

	//RenderTarget* CreateRenderTarget(const char* szName);

	//void DestroyRenderTarget(RenderTarget* pRenderTarget); 

	virtual ITexture* CreateTextureRenderTarget(const char* szName, u32 nWidth, u32 nHeight, RendererPixelFormat pixelFormat) = 0;

	virtual DepthStencil* CreateDepthStencil(u32 nWidth, u32 nHeight, RendererPixelFormat pixelFormat, bool bBindShaderResource) = 0;

	virtual void DestroyAllDepthStencil();

	virtual void SetDepthStencilAsTexture(u32 nStage, DepthStencil* pDepthStencil) = 0;

	virtual void DestroyDepthStencil(DepthStencil* pDepthStencil);

	virtual void DestroyTextureRenderTarget(ITexture* pRTT);

	virtual ShaderObject* CreateShaderObject(IGPUProgram* pGPUPrograms[], u32 nGPUProgramsNum);

    inline virtual ShaderObject* CreateShaderObject();

	void DestroyAllShaderObjects();

    MaterialTemplate* CreateMaterialTemplate(const FixedString& strFile);

	MaterialTemplate* CreateMaterialTemplate(const FixedString& strName, CDataStream* pDataStream);

    virtual void SetRenderState(const BlendState& blendState, const RasterizerState& rasterState, const DepthStencilState& depthState, const FixedPipeState& fixedState) = 0;

    virtual void SetSamplerState(u32 nSampler, const SamplerState& samplerState) = 0;

    virtual void RenameShaderFile(const char* szShader, AIRString& strOut, bool bIsVertexShader) {}

    inline virtual void SetDepthBiasEnable(bool bEnable, f32 fDepthBias, f32 fSlopeScaledDepthBias)
    {
        m_bDepthBiasEnable = bEnable;
        m_fDepthBias = fDepthBias;
        m_fSlopeScaledDepthBias = fSlopeScaledDepthBias;
    }
    
    virtual void CreateVAO( RenderPass* pass, IVertexBuffer* pVB, IIndexBuffer* pIB ) {}
    virtual void SetVAO( IVertexArrayObject* pVAO ) {}
    

	CVertexDeclaration* GetCurVertexDecl() 
	{
		return m_pCurInputLayout;
	}
protected:
    //virtual void    SetVertexDeclaration(CVertexDeclaration* pVertexDecl) = 0;

    virtual ShaderObject* CreateShaderObjectImpl();

protected:
    //CTextureMgr		m_textureMgr;		//���������
	//u8					m_nAPIType;			//0 - dx9, 1 - opengl
	//MAP_RENDER_PARM		m_mapRenderParm;	//��Ⱦ����MAP�����ַ�����������Ӧ����Ⱦ������ֵ

    //MAP_RENDER_NAME     m_mapTexStageStateTypeName;   //����׶�״̬���Ͷ�Ӧ������
    //MAP_RENDER_NAME     m_mapSamplerStateTypeName;    //�������״̬���Ͷ�Ӧ������

    CGPUDataParamSource       m_gpuDataParam;

    //CPass* m_pCurrentPass;    //��ǰ��pass

	VEC_CLIPPLANE		m_vtClipPlanes;
	bool				m_bClipPlanesDirty;
	bool				m_bVertexProgramBound;
    unsigned short      m_nCurrentTextures; //��ǰ�Ѿ��õ�������

    //typedef AIRMap<AIRString, RenderTarget*> RenderTargetMap;
    //typedef RenderTargetMap::iterator RenderTargetMapIter;

    //typedef std::multimap<u8, RenderTarget*> PriorityRenderTargets;
    //typedef PriorityRenderTargets::iterator PriorityRenderTargetsIter;

	typedef AIRMap<AIRString, ITexture*> TextureRenderTargetMap;

    //RenderTargetMap m_mapRenderTargets;
    //PriorityRenderTargets m_priorityRenderTargets;

	TextureRenderTargetMap m_mapTexRenderTargets;

	//typedef AIRMap<AIRString, DepthStencil*> DepthStencilMap;
	//DepthStencilMap m_mapDepthStencils;

	typedef AIRMap<AIRString, ShaderObject*> ShaderObjectMap;
	ShaderObjectMap m_mapShaderObjects;

    typedef AIRMap<FixedString, MaterialTemplate*> MaterialTemplateMap;
    MaterialTemplateMap m_mapMaterialTemplates;

    f32 m_fDepthBias;
    f32 m_fSlopeScaledDepthBias;
    bool m_bDepthBiasEnable;

	CVertexDeclaration* m_pCurInputLayout;
	RenderWindow* m_pRenderWindow;

	AIRMap<FixedString, RenderWindow*> m_mapRenderWindows;
	typedef AIRMap<DepthStencilKey, DepthStencil*> DepthStencilMap;
	AIRMap<DepthStencilKey, DepthStencil*> m_mapDepthStencils;
private:
	ScreenQuad* m_pScreenQuad; //������Ⱦ��Ļ����
    
};

