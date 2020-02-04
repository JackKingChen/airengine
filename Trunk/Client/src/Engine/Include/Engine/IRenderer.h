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
// 渲染状态MAP表
//typedef std::map< AIRString, u32 > MAP_RENDER_PARM;
// 反向查找渲染状态名字
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

    //画2D图画，不用RHW
    //virtual bool Draw2DImageOH(const AIR::Rectf& rectScreen, const AIR::Rectf& rectUV, ITexture* pTex, f32 fDepth, const SColor& color = SColor(255, 255, 255, 255)) = 0;

    //画矩形
    //virtual void DrawRect(const AIR::Rect& rectScreen, f32 fLineWidth, const SColor& color = 0xffffffff) = 0;

    virtual void DrawRect(const AIR::Rectf& rectScreen, f32 fLineWidth, const SColor& color = 0xffffffff) = 0;

	virtual void DrawScreenQuad(const AIR::Rectf& rectScreen, const AIR::Rectf& rectUV, f32 fUVScale, const CColorValue& color, IMaterial* pMaterial);

    virtual void Release();

    virtual void BeginScene() = 0;

    virtual void EndScene() = 0;

    virtual void ClearScene(const SColor& clrBackground, bool bClearDepth, bool bClearStencil) = 0;

    //virtual void Present(const RECT* pRectSource, const RECT* pRectDest, HWND hDestWnd) = 0;

    //设置默认状态
    virtual void Reset() = 0;

    //设置混合
    //virtual void EnableColorBlend() = 0;

    //取消混合
    //virtual void DisableColorBlend() = 0;

    //设置纹理过滤
    virtual bool SetTextureFilterPoint(s32 nStage, u32 nType) = 0;

    virtual bool SetTextureFilterLinear(s32 nStage, u32 nType) = 0;


    //创建渲染器相关的纹理
    virtual ITexture* CreateTexture() = 0;

    virtual IVertexBuffer* CreateVertexBuffer(u32 nVertexCount, u32 nUnitSize, bool bDynamic = false) = 0;

    //创建顶点缓冲
    //@type 顶点类型
    //@lpBuffer 顶点内存数据
    //@nVertexCount 顶点个数
    //bDynamic，是否为动态创建
    virtual IVertexBuffer* CreateVertexBuffer( void* lpBuffer, u32 nVertexCount, u32 nUnitSize, bool bDynamic = false ) = 0;

	//************************************
	// Method:    CreateVertexDeclaration
	// FullName:  IRenderer::CreateVertexDeclaration
	// Access:    virtual public 
	// Returns:   CVertexDeclaration*
	// Qualifier: 创建顶点格式声明对象
	// Parameter: void
	//************************************
	virtual CVertexDeclaration* CreateVertexDeclaration( void ) = 0;

    virtual void ClearVertexBuffer(IVertexBuffer*& pVB) = 0;

    virtual void ClearIndexBuffer( IIndexBuffer*& pIB) = 0;

	virtual void SetVertexDeclaration( CVertexDeclaration* pVD ) = 0;

    //设置顶点缓冲
    virtual void SetVertexBuffer(u32 nStream, IVertexBuffer* pVertexBuffer) = 0;

    //只设置顶点缓冲，不设置顶点声明
    //virtual void SetVertexStream(u32 nStream, IVertexBuffer* pVertexBuffer) = 0;

    //设置instancing数据
    //virtual void SetStreamSourceFreq(u32 nStream, u32 nInstancesNum, bool bInstanceData) = 0;

    //清理instancing数据
    //virtual void FreeStreamSourceFreq(u32 nStream) = 0;

    virtual void SetIndexBuffer(IIndexBuffer* pIndexBuffer) = 0;


    virtual void SetWorldTransform(const Matrix4f* pMat) = 0;

    virtual void SetViewTransform(const Matrix4f* pMat) = 0;

    virtual void SetProjectionTransform(const Matrix4f* pMat) = 0;

    //设置纹理寻址模式
    virtual void SetTextureAddressMode(u32 nStage, TextureAddressValue nAddressMode) = 0;

    //设置纹理过滤模式
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

    //创建IndexBuffer，
    //@nIndexCount    索引数量
    virtual IIndexBuffer* CreateIndexBuffer(u32 nIndexCount/*nSize*/, bool bDynamic = false) = 0;

	virtual IIndexBuffer* CreateIndexBuffer(u32 nIndexCount, bool bDynamic, IndexType type, bool bUseSystemMemory = false ) = 0;


    virtual IGPUProgram* CreateVertexShader(const char* szName, const char* szVersion, const char* szFunction = "VSMain") = 0;

    virtual IGPUProgram* CreatePixelShader(const char* szName, const char* szVersion, const char* szFunction = "PSMain") = 0;

    virtual void SetVertexShader(IGPUProgram* pShader) = 0;

    virtual void SetPixelShader(IGPUProgram* pShader) = 0;

    //virtual void ClearShader(IGPUProgram* pShader) = 0;

    //在每帧渲染前的检查
    virtual void CheckBeforeRender() = 0;

    //virtual IMaterial* GetMaterial(const char* szName) = 0;

    //virtual ITexture*  GetRenderTarget(const AIRString& strName) = 0;

    virtual void SetFillMode(bool bWireFrame) = 0;

    virtual void GetBackBufferSize(Vector2D& dimension) = 0;

    virtual void GetBackBufferSize(int& nWidth, int& nHeight) const = 0;

    //设置渲染状态，实际上是设置shader之类的
    //virtual void ApplyRenderState(IShader& shader) = 0;

    //设置具体某个渲染状态
    virtual void SetRenderState(u32 nState, u32 nValue) = 0;

    //设置默认渲染状态
    virtual void SetDefaultRenderState() = 0;

    //设置scissor
    virtual void SetScissorEnable(bool bEnable) = 0;

    //设置scissor的矩形
    virtual void SetScissorRect(const RECT* pRect) = 0;

    //画2D直线
    virtual void Draw2DLine(int nStartX, int nStartY, int nEndX, int nEndY, float fWidth = 1.0f, const SColor& color = 0xffffffff) = 0;

	virtual void SetViewPort( f32 l, f32 t, f32 r, f32 b, f32 n = 0, f32 f = 1 ) = 0;

	//设置采样状态
	virtual void SetSamplerState(u32 nSampler, u32 Type, u32 Value) = 0;

	//设置纹理阶段状态
	virtual void SetTextureStageState(u32 nStage, u32 Type, u32 Value) = 0;

	//获得设备的状态
	virtual EDeviceState GetDeviceState() = 0;

	//设备丢失的处理
	//virtual void	OnLostDevice( void ) = 0;

	//重新设置设备的处理
	//virtual void	OnResetDevice( void ) = 0;

    //渲染数据
    virtual void    Render(CRenderData& renderData) = 0;

    //设置环境光
    virtual void    SetAmbientLight(const CColorValue& color) = 0;

    //设置雾
    virtual void    SetFog(FogMode mode, const CColorValue& fogColor, float fDensity, float fStart, float fEnd) = 0;
    
    //设置渲染物材质参数
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

    //使用光照，这个是针对固定管线的
    //@lights   光照数组
    //@nLimits  最大光照数
    virtual void    UseLights(AIRList<CLight*>& lights, unsigned short nLimits) = 0;
    
    //创建渲染状态
    //virtual IShader* CreateShader(CPass* pPass) = 0;

    //获得vertexshader的版本
    //virtual u32 GetVertexShaderVersion(const AIRString& strVertexShaderVersion) = 0;

    //获得pixelshader的版本
    //virtual u32 GetPixelShaderVersion(const AIRString& strPixelShaderVerion) = 0;

    //获得纹理过滤的cap
    virtual u32 GetTextureFilterCap(u32 nFilter) = 0;

	//virtual ITexture* GetTexture( const AIRString& szName )  = 0;

    //判断是否支持
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

    //判断是否支持multirendertarget
    virtual bool IsSupportMultiRenderTarget() const = 0;

    //获得最大的mrt数量
    virtual int GetMaxMultiRTTsNum() const = 0;
    //设置后缓冲大小
    //virtual void SetBackBufferSize(int nWidth, int nHeight) = 0;

    virtual bool SaveBackbufferToFile(const char* szFilename) = 0;

	//lbh 设置裁剪状态
	virtual void SetCullMode(CullMode nCM) = 0;

    //设置浓度状态
    virtual void SetDepthEnable(bool bEnable) = 0;

    virtual void SetZWriteEnable(bool bEnable) = 0;

    virtual void SetFillMode(RenderSystemFillMode fillMode) = 0;

    //设置点的渲染状态
    virtual void SetPointSizeState(float fPointSize, float fPointSizeMin = 1.0f, float fPointSizeMax = 64.0f) = 0;

    virtual void SetPointScale(bool bPointScaleEnable, float fPointScaleA = 1.0f, float fPointScaleB = 0.0f, float fPointScaleC = 0.0f) = 0;

    virtual void SetPointSpriteEnable(bool bEnable) = 0;

    //RendererPixelFormat GetPixelFormatByName(const char* szName);

	//virtual bool TestCompileShader( const char* szFilename, const char* szVersion, const char* szFunction ) = 0;


    virtual void DrawPrimitive(Primitive* pPrimitive) = 0;

    //处理所有rendertargets
    //void RenderAllRenderTargets();

    //加载rendertarget
    //void AttachRenderTarget(RenderTarget* pRenderTarget);

    //void DetachRenderTarget(RenderTarget* pRenderTarget);

    //void DestroyRenderTarget(RenderTarget* pRenderTarget);

    //void DestroyAllRenderTargets();

    //RenderTarget* GetRenderTarget(const AIRString& strName);

    //void DetachAllRenderTargets();

    virtual void SetBackGroundColor(SColor color) = 0;

    //设置纹理混合
    //virtual void SetTextureBlendMode(u32 nStage, const TextureUnitBlend& textureBlend) = 0;

    //设置纹理border颜色
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

	//渲染instancing
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
    //CTextureMgr		m_textureMgr;		//纹理管理器
	//u8					m_nAPIType;			//0 - dx9, 1 - opengl
	//MAP_RENDER_PARM		m_mapRenderParm;	//渲染参数MAP表，用字符串来索引对应的渲染参数的值

    //MAP_RENDER_NAME     m_mapTexStageStateTypeName;   //纹理阶段状态类型对应的名字
    //MAP_RENDER_NAME     m_mapSamplerStateTypeName;    //纹理采样状态类型对应的名字

    CGPUDataParamSource       m_gpuDataParam;

    //CPass* m_pCurrentPass;    //当前的pass

	VEC_CLIPPLANE		m_vtClipPlanes;
	bool				m_bClipPlanesDirty;
	bool				m_bVertexProgramBound;
    unsigned short      m_nCurrentTextures; //当前已经用到的纹理

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
	ScreenQuad* m_pScreenQuad; //用于渲染屏幕数据
    
};

