#pragma once 

#include "MemoryAlloter.h"
#include "EngineDefination.h"
#include "ColorValue.h"
#include "Color.h"

class IRenderer;
class ITexture;

struct BlendState 
{
    size_t blendEnable : 1;
    size_t alphaToCoverageEnable : 1;
    size_t independentBlendEnable : 1;
    size_t nBlendOpColor : 3;
    size_t nSrcColor : 5;
    size_t nDestColor : 5;
    size_t nBlendOpAlpha : 3;
    size_t nSrcAlpha : 5;
    size_t nDestAlpha : 5;
    size_t seperateAlphaBlendEnable : 1;

    BlendState& operator = (size_t value)
    {
        size_t* pMarco = (size_t*)this;
        *pMarco = value;
        return *this;
    }

    bool operator == (const BlendState& state)
    {
        size_t* pMarco = (size_t*)this;
        size_t* pOther = (size_t*)(&state);
        return (*pMarco == *pOther);
    }

    bool operator != (const BlendState& state)
    {
        size_t* pMarco = (size_t*)this;
        size_t* pOther = (size_t*)(&state);
        return (*pMarco != *pOther);
    }

    bool operator < (const BlendState& state)
    {
        size_t* pMarco = (size_t*)this;
        size_t* pOther = (size_t*)(&state);
        return (*pMarco < *pOther);
    }

    BlendState& operator = (const BlendState& state)
    {
        size_t* pMarco = (size_t*)this;
        size_t* pOther = (size_t*)(&state);
        *pMarco = *pOther;
        return *this;
    }
};

struct RasterizerState 
{
    size_t fillMode : 2;
    size_t cullMode : 2;
    size_t MultiSampleAntialiasEnable : 1;
    size_t AntialiasedLineEnable : 1;
    size_t ScissorEnable : 1;
    size_t depthBiasEnable : 1;
    size_t depthClipEnable : 1;

    RasterizerState& operator = (size_t value)
    {
        size_t* pMarco = (size_t*)this;
        *pMarco = value;
        return *this;
    }

    bool operator == (const RasterizerState& state)
    {
        size_t* pMarco = (size_t*)this;
        size_t* pOther = (size_t*)(&state);
        return (*pMarco == *pOther);
    }

    bool operator != (const RasterizerState& state)
    {
        size_t* pMarco = (size_t*)this;
        size_t* pOther = (size_t*)(&state);
        return (*pMarco != *pOther);
    }

    bool operator < (const RasterizerState& state)
    {
        size_t* pMarco = (size_t*)this;
        size_t* pOther = (size_t*)(&state);
        return (*pMarco < *pOther);
    }

    RasterizerState& operator = (const RasterizerState& state)
    {
        size_t* pMarco = (size_t*)this;
        size_t* pOther = (size_t*)(&state);
        *pMarco = *pOther;
        return *this;
    }
};

struct DepthStencilState
{
    size_t zEnable : 1;
    size_t zWriteEnable : 1;
    size_t DepthCompareFunc : 3;
    size_t stencilEnable : 1;
    size_t opFailed : 3;
    size_t opZFailed : 3;
    size_t opPassed : 3;
    size_t StencilCompareFunc : 3;

    DepthStencilState& operator = (size_t value)
    {
        size_t* pMarco = (size_t*)this;
        *pMarco = value;
        return *this;
    }

    bool operator == (const DepthStencilState& state)
    {
        size_t* pMarco = (size_t*)this;
        size_t* pOther = (size_t*)(&state);
        return (*pMarco == *pOther);
    }

    bool operator != (const DepthStencilState& state)
    {
        size_t* pMarco = (size_t*)this;
        size_t* pOther = (size_t*)(&state);
        return (*pMarco != *pOther);
    }

    bool operator < (const DepthStencilState& state)
    {
        size_t* pMarco = (size_t*)this;
        size_t* pOther = (size_t*)(&state);
        return (*pMarco < *pOther);
    }

    DepthStencilState& operator = (const DepthStencilState& state)
    {
        size_t* pMarco = (size_t*)this;
        size_t* pOther = (size_t*)(&state);
        *pMarco = *pOther;
        return *this;
    }
};

struct FixedPipeState
{
    size_t alphaTestEnable : 1;
    size_t alphaCompareFunc : 3;
    size_t fogEnable : 1;
    size_t lightEnable : 1;
    size_t clipPlaneEnable : 1;
    size_t alphaRef : 8;

    FixedPipeState& operator = (size_t value)
    {
        size_t* pMarco = (size_t*)this;
        *pMarco = value;
        return *this;
    }

    bool operator == (const FixedPipeState& state)
    {
        size_t* pMarco = (size_t*)this;
        size_t* pOther = (size_t*)(&state);
        return (*pMarco == *pOther);
    }

    bool operator != (const FixedPipeState& state)
    {
        size_t* pMarco = (size_t*)this;
        size_t* pOther = (size_t*)(&state);
        return (*pMarco != *pOther);
    }

    bool operator < (const FixedPipeState& state)
    {
        size_t* pMarco = (size_t*)this;
        size_t* pOther = (size_t*)(&state);
        return (*pMarco < *pOther);
    }

    FixedPipeState& operator = (const FixedPipeState& state)
    {
        size_t* pMarco = (size_t*)this;
        size_t* pOther = (size_t*)(&state);
        *pMarco = *pOther;
        return *this;
    }
};

struct SamplerState
{
    SamplerState()// : m_texAddr(TAV_Warp), m_texFilter(TFT_Point)//, pTexture(NULL), pSamplerStateObject(NULL)
    {

    }
    //FixedString strName;
    //TextureAddressValue m_texAddr;
    //TextureFilterOption m_texFilter;

    //ITexture* pTexture;
    //SamplerStateObject* pSamplerStateObject;

    size_t texAddr : 3;
    size_t texFilter : 4;
    size_t stage : 4;

    SamplerState& operator = (size_t value)
    {
        size_t* pMarco = (size_t*)this;
        *pMarco = value;
        return *this;
    }

    bool operator == (const SamplerState& state)
    {
        size_t* pMarco = (size_t*)this;
        size_t* pOther = (size_t*)(&state);
        return (*pMarco == *pOther);
    }

    bool operator != (const SamplerState& state)
    {
        size_t* pMarco = (size_t*)this;
        size_t* pOther = (size_t*)(&state);
        return (*pMarco != *pOther);
    }

    bool operator < (const SamplerState& state)
    {
        size_t* pMarco = (size_t*)this;
        size_t* pOther = (size_t*)(&state);
        return (*pMarco < *pOther);
    }

    SamplerState& operator = (const SamplerState& state)
    {
        size_t* pMarco = (size_t*)this;
        size_t* pOther = (size_t*)(&state);
        *pMarco = *pOther;
        return *this;
    }
};

class EngineExport BlendStateObject// : public EngineAllocator(BlendStateObject)
{
	EngineAllocator(BlendStateObject)
public:
    BlendStateObject() : m_bBlendEnable(false)
        , m_blendOpt(BO_Add)
        , m_nSrc(RSRV_One)
        , m_nDest(RSRV_Zero)
        , m_blendOptAlpha(BO_Add)
        , m_nSrcAlpha(RSRV_One)
        , m_nDestAlpha(RSRV_Zero)
        , m_bSeperateAlphaBlendEnable(false)
        , m_bDirty(false)
    {}
    virtual ~BlendStateObject() {}

    virtual void Apply(IRenderer* pRenderer) = 0;

    void SetBlendEnable(bool bEnable)
    {
        if (bEnable != m_bBlendEnable)
        {
            m_bDirty = true;
        }
        m_bBlendEnable = bEnable;
    }

    void SetBlendOp(BlendOpt op)
    {
        if (m_blendOpt != op)
        {
            m_bDirty = true;
        }
        m_blendOpt = op;
    }

    void SetBlendValueSrc(RenderStateBlendValue src)
    {
        if (src != m_nSrc)
        {
            m_bDirty = true;
        }
        m_nSrc = src;
    }

    void SetBlendValueDest(RenderStateBlendValue dest)
    {
        if (dest != m_nDest)
        {
            m_bDirty = true;
        }
        m_nDest = dest;
    }

    void SetBlendOpAlpha(BlendOpt op)
    {
        if (m_blendOptAlpha != op)
        {
            m_bDirty = true;
        }

        m_blendOptAlpha = op;
    }

    void SetBlendValueSrcAlpha(RenderStateBlendValue src)
    {
        if (src != m_nSrcAlpha)
        {
            m_bDirty = true;
        }
        m_nSrcAlpha = src;
    }

    void SetBlendValueDestAlpha(RenderStateBlendValue dest)
    {
        if (dest != m_nDestAlpha)
        {
            m_bDirty = true;
        }
        m_nDestAlpha = dest;
    }

    bool IsBlendEnable() const
    {
        return m_bBlendEnable;
    }

    BlendOpt GetBlendOp() const
    {
        return m_blendOpt;
    }

    RenderStateBlendValue GetBlendSrc() const
    {
        return m_nSrc;
    }

    RenderStateBlendValue GetBlendDest() const
    {
        return m_nDest;
    }

    RenderStateBlendValue GetBlendSrcAlpha() const
    {
        return m_nSrcAlpha;
    }

    RenderStateBlendValue GetBlendDestAlpha() const
    {
        return m_nDestAlpha;
    }

    BlendOpt GetBlendOpAlpha() const
    {
        return m_blendOptAlpha;
    }

    bool IsSeperateAlphaBlendEnable() const
    {
        return m_bSeperateAlphaBlendEnable;
    }

    void SetSeperateAlphaBlendEnable(bool bEnable)
    {
        m_bSeperateAlphaBlendEnable = bEnable;
    }

protected:
    bool m_bBlendEnable;
    BlendOpt m_blendOpt;
    RenderStateBlendValue  m_nSrc;          //blend如果是alpha或modulate的操作
    RenderStateBlendValue  m_nDest;			//blend如果是alpha或modulate的操作

    BlendOpt m_blendOptAlpha;
    RenderStateBlendValue  m_nSrcAlpha;          //blend如果是alpha或modulate的操作
    RenderStateBlendValue  m_nDestAlpha;			//blend如果是alpha或modulate的操作
    bool m_bSeperateAlphaBlendEnable;     

    bool m_bDirty;
};

class EngineExport RasterizerStateObject //: public EngineAllocator(RasterizerStateObject)
{
	EngineAllocator(RasterizerStateObject)
public:
    RasterizerStateObject() : m_fillMode(RSFM_Solid), m_cullMode(CM_CCW)
    , m_bMultiSampleAntialiasEnable(false), m_bAntialiasedLineEnable(false)
    , m_bScissorEnable(false), m_fDepthBias(0), m_fDepthBiasClamp(0), m_fSlopeScaledDepthBias(0), m_bDirty(true) {}
    virtual ~RasterizerStateObject() {}

    virtual void Apply(IRenderer* pRenderer) = 0;

    void SetFillMode(RenderSystemFillMode fillMode)
    {
        if (fillMode != m_fillMode)
        {
            m_bDirty = true;
        }
        m_fillMode = fillMode;
    }

    void SetCullMode(CullMode cullMode)
    {
        if (m_cullMode != cullMode)
        {
            m_bDirty = true;
        }
        m_cullMode = cullMode;
    }

    void SetMultiSampleAntialiasEnable(bool bEnable)
    {
        if (m_bMultiSampleAntialiasEnable != bEnable)
        {
            m_bDirty = true;
        }
        m_bMultiSampleAntialiasEnable = bEnable;
    }

    void SetAntialiasedLineEnable(bool bEnable)
    {
        if (bEnable != m_bAntialiasedLineEnable)
        {
            m_bDirty = true;
        }
        m_bAntialiasedLineEnable = bEnable;
    }

    void SetScissorEnable(bool bEnable)
    {
        if (bEnable != m_bScissorEnable)
        {
            m_bDirty = true;
        }
        m_bScissorEnable = bEnable;
    }

    RenderSystemFillMode GetFillMode() const
    {
        return m_fillMode;
    }

    CullMode GetCullMode() const
    {
        return m_cullMode;
    }

    bool IsMultiSampleAntialiasEnable() const
    {
        return m_bMultiSampleAntialiasEnable;
    }

    bool IsAntialiasedLineEnable() const
    {
        return m_bAntialiasedLineEnable;
    }

    bool IsScissorEnable() const
    {
        return m_bScissorEnable;
    }

	void SetDepthBias(f32 fDepthBias)
	{
		if (fDepthBias != m_fDepthBias)
		{
			m_bDirty = true;
		}
		m_fDepthBias = fDepthBias;
	}

	void SetDepthBiasClamp(f32 fDepthBiasClamp)
	{
		if (fDepthBiasClamp != m_fDepthBiasClamp)
		{
			m_bDirty = true;
		}
		m_fDepthBiasClamp = fDepthBiasClamp;
	}

	void SetSlopeScaledDepthBias(f32 fSlopeScaledDepthBias)
	{
		if (fSlopeScaledDepthBias != m_fSlopeScaledDepthBias)
		{
			m_bDirty = true;
		}
		m_fSlopeScaledDepthBias = fSlopeScaledDepthBias;
	}

protected:
    RenderSystemFillMode m_fillMode;
    CullMode m_cullMode;
    bool  m_bMultiSampleAntialiasEnable;
    bool  m_bAntialiasedLineEnable;   //用于dx11
    bool  m_bScissorEnable;

	f32   m_fDepthBias;
	f32   m_fDepthBiasClamp;
	f32   m_fSlopeScaledDepthBias;

    bool  m_bDirty;
private:
};

class EngineExport DepthStencilStateObject// : public EngineAllocator(DepthStencilStateObject)
{
	EngineAllocator(DepthStencilStateObject)
public:
    DepthStencilStateObject() : m_bZEnable(true)
    , m_bZWriteEnable(true)
    , m_DepthCompareFunc(eCF_LESS_EQUAL)
    , m_bStencilEnable(false)
    , m_opFailed(eSOP_KEEP)
    , m_opZFailed(eSOP_KEEP)
    , m_opPass(eSOP_KEEP)
    , m_StencilCompareFunc(eCF_ALWAYS_PASS)
    , m_nStencilRef(0)
    , m_nStencilMask(0xffffffff)
    , m_nStencilWriteMask(0xffffffff)
    , m_bDirty(false)
    {}
    virtual ~DepthStencilStateObject() {}

    virtual void Apply(IRenderer* pRenderer) = 0;

    void SetDepthEnable(bool bEnable)
    {
        if (m_bZEnable != bEnable)
        {
            m_bDirty = true;
        }
        m_bZEnable = bEnable;
    }

    void SetDepthWriteEnable(bool bEnable)
    {
        if (m_bZWriteEnable != bEnable)
        {
            m_bDirty = true;
        }

        m_bZWriteEnable = bEnable;
    }

    void SetStencilEnable(bool bEnable)
    {
        if (m_bStencilEnable != bEnable)
        {
            m_bDirty = true;
        }
        m_bStencilEnable = bEnable;
    }

    void  SetDepthCompareFunc(CompareFunction func)
    {
        if (m_DepthCompareFunc != func)
        {
            m_bDirty = true;
        }
        m_DepthCompareFunc = func;
    }

    void SetStencilOpFailed(StencilOperation opFailed)
    {
        if (opFailed != m_opFailed)
        {
            m_bDirty = true;
        }
        m_opFailed = opFailed;
    }

    void SetStencilOpDepthFailed(StencilOperation opFailed)
    {
        if (opFailed != m_opZFailed)
        {
            m_bDirty = true;
        }
        m_opZFailed = opFailed;
    }

    void SetStencilOpPass(StencilOperation opPass)
    {
        if (opPass != m_opPass)
        {
            m_bDirty = true;
        }
        m_opPass = opPass;
    }

    void SetStencilCompareFunc(CompareFunction func)
    {
        if (m_StencilCompareFunc != func)
        {
            m_bDirty = true;
        }
        m_StencilCompareFunc = func;
    }

    void SetStencilRef(u32 nRef)
    {
        if (m_nStencilRef != nRef)
        {
            m_bDirty = true;
        }
        m_nStencilRef = nRef;
    }

    void SetStencilMask(u32 nMask)
    {
        if (m_nStencilMask != nMask)
        {
            m_bDirty = true;
        }
        m_nStencilMask = nMask;
    }

    void SetStencilWriteMask(u32 nMask)
    {
        if (m_nStencilWriteMask != nMask)
        {
            m_bDirty = true;
        }
        m_nStencilWriteMask = nMask;
    }

    bool IsDepthEnable() const
    {
        return m_bZEnable;
    }

    bool IsDepthWriteEnable() const
    {
        return m_bZWriteEnable;
    }

    CompareFunction GetDepthCompareFunc() const
    {
        return m_DepthCompareFunc;
    }

    bool IsStencilEnable() const
    {
        return m_bStencilEnable;
    }

    StencilOperation GetStencilOpFailed() const
    {
        return m_opFailed;
    }

    StencilOperation GetStencilOpDepthFailed() const
    {
        return m_opZFailed;
    }

    StencilOperation GetStencilOpPass() const
    {
        return m_opPass;
    }

    CompareFunction GetStencilCompareFunc() const
    {
        return m_StencilCompareFunc;
    }

    u32 GetStencilRef() const
    {
        return m_nStencilRef;
    }

    u32 GetStencilMask() const
    {
        return m_nStencilMask;
    }

    u32 GetStencilWriteMask() const
    {
        return m_nStencilWriteMask;
    }

protected:
    bool m_bZEnable;
    bool m_bZWriteEnable;
    CompareFunction m_DepthCompareFunc;
    bool m_bStencilEnable;
    StencilOperation m_opFailed;
    StencilOperation m_opZFailed;
    StencilOperation m_opPass;
    CompareFunction  m_StencilCompareFunc;
    u32 m_nStencilRef;
    u32 m_nStencilMask;
    u32 m_nStencilWriteMask;

    bool m_bDirty;
};

class EngineExport FixedPipeStateObject //: public EngineAllocator(FixedPipeStateObject)
{
	EngineAllocator(FixedPipeStateObject)
public:
    FixedPipeStateObject() : m_bAlphaTestEnable(false)
    , m_nAlphaRef(0)
    , m_AlphaCompareFunc(eCF_ALWAYS_PASS)
    , m_bFogEnable(true)
    , m_bLightEnable(false)
    , m_bClipPlaneEnable(false)
    {}
    virtual ~FixedPipeStateObject() {}

    virtual void Apply(IRenderer* pRenderer) = 0;

    bool m_bAlphaTestEnable;
    u32  m_nAlphaRef;
    CompareFunction m_AlphaCompareFunc;

    bool m_bFogEnable;
    bool m_bLightEnable;
    bool m_bClipPlaneEnable;
};

class EngineExport SamplerStateObject //: public EngineAllocator(SamplerStateObject)
{
	EngineAllocator(SamplerStateObject)
public:
    SamplerStateObject(u32 nStage = 0) : m_nStage(nStage)
    , m_texAddrU(TAV_Warp) 
    , m_texAddrV(TAV_Warp)
    , m_texAddrW(TAV_Warp)
    , m_texFilter(TFT_Linear) 
	, m_funCompare(eCF_ALWAYS_FAIL)
	{
		memset(&m_colorBorder, 0, sizeof(m_colorBorder));
	}
    virtual ~SamplerStateObject() {}

    virtual void Apply(IRenderer* pRenderer, ITexture* pTexture) = 0;

    void SetTextureAddressU(TextureAddressValue value)
    {
        if (m_texAddrU != value)
        {
            m_bDirty = true;
        }

        m_texAddrU = value;
    }

    void SetTextureAddressV(TextureAddressValue value)
    {
        if (m_texAddrV != value)
        {
            m_bDirty = true;
        }

        m_texAddrV = value;
    }

    void SetTextureAddressW(TextureAddressValue value)
    {
        if (m_texAddrW != value)
        {
            m_bDirty = true;
        }

        m_texAddrW = value;
    }

    void SetTextureFilterOp(TextureFilterOption op)
    {
        if (op != m_texFilter)
        {
            m_bDirty = true;
        }
        m_texFilter = op;
    }

    void SetTextureBorderColor(const CColorValue& color)
    {
        if (m_colorBorder != color)
        {
            m_bDirty = true;
        }
        m_colorBorder = color;
    }

	void SetCompareFunc(CompareFunction func)
	{
		if (m_funCompare != func)
		{
			m_bDirty = true;
		}
		m_funCompare = func;
	}

    TextureAddressValue GetTextureAddressU() const
    {
        return m_texAddrU;
    }

    TextureAddressValue GetTextureAddressV() const
    {
        return m_texAddrV;
    }

    TextureAddressValue GetTextureAddressW() const
    {
        return m_texAddrW;
    }

    TextureFilterOption GetTextureFilter() const
    {
        return m_texFilter;
    }

    const CColorValue& GetBorderColor() const
    {
        return m_colorBorder;
    }

    void SetStage(u32 nStage)
    {
        m_nStage = nStage;
    }

    u32 m_nStage;
protected:
    TextureAddressValue m_texAddrU;
    TextureAddressValue m_texAddrV;
    TextureAddressValue m_texAddrW;
    TextureFilterOption m_texFilter;
    CColorValue  m_colorBorder;
	CompareFunction m_funCompare;

    bool m_bDirty;
};
