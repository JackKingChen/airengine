#pragma once 

#include "EngineConfig.h"
#include "MemoryAlloter.h"
#include "EngineDefination.h"
using namespace AIR;

struct DepthStencilKey 
{
	u64 nWidth : 16;
	u64 nHeight : 16;
	u64 format : 16;
	u64 bBindShader : 1;

	DepthStencilKey(u16 width, u16 height, u16 nFormat, bool bBind) : nWidth(width), nHeight(height), format(nFormat)
		, bBindShader(bBind)
	{

	}

	DepthStencilKey& operator = (u64 value)
	{
		u64* pMarco = (u64*)this;
		*pMarco = value;
		return *this;
	}

	bool operator == (const DepthStencilKey& state)
	{
		u64* pMarco = (u64*)this;
		u64* pOther = (u64*)(&state);
		return (*pMarco == *pOther);
	}

	bool operator == (u64 nValue)
	{
		u64* pMarco = (u64*)this;
		u64* pOther = (u64*)(&nValue);
		return (*pMarco == *pOther);
	}

	bool operator != (const DepthStencilKey& state)
	{
		u64* pMarco = (u64*)this;
		u64* pOther = (u64*)(&state);
		return (*pMarco != *pOther);
	}

	bool operator < (const DepthStencilKey& state)
	{
		u64* pMarco = (u64*)this;
		u64* pOther = (u64*)(&state);
		return (*pMarco < *pOther);
	}

	DepthStencilKey& operator = (const DepthStencilKey& state)
	{
		u64* pMarco = (u64*)this;
		u64* pOther = (u64*)(&state);
		*pMarco = *pOther;
		return *this;
	}
};

inline bool operator < (const DepthStencilKey& left, const DepthStencilKey& right)
{
	u64* uLeftValue = (u64*)&left;
	u64* uRightValue = (u64*)&right;
	return *uLeftValue < *uRightValue;
}

inline bool operator > (const DepthStencilKey& left, const DepthStencilKey& right)
{
	u64* uLeftValue = (u64*)&left;
	u64* uRightValue = (u64*)&right;
	return *uLeftValue > *uRightValue;
}

inline bool operator == (const DepthStencilKey& left, const DepthStencilKey& right)
{
	u64* uLeftValue = (u64*)&left;
	u64* uRightValue = (u64*)&right;
	return *uLeftValue == *uRightValue;
}

class EngineExport DepthStencil// : public EngineAllocator(DepthStencil)
{
	EngineAllocator(DepthStencil)
public:
	DepthStencil() : m_nWidth(0), m_nHeight(0), m_pixelFormat(RPF_D24S8), m_bBindShaderResource(false), m_bFullScreen(true), m_fScreenRatio(1.0f) {}
	DepthStencil(u32 nWidth, u32 nHeight, RendererPixelFormat pixelFormat, bool bBindShaderResource) : m_nWidth(nWidth)
	, m_nHeight(nHeight)
	, m_pixelFormat(pixelFormat)
	, m_bBindShaderResource(bBindShaderResource) {}

	virtual ~DepthStencil() {}

	virtual void Clear() {}

	bool IsBindShaderResource() const
	{
		return m_bBindShaderResource;
	}

	void SetBindShaderResource(bool bBind)
	{
		m_bBindShaderResource = bBind;
	}

	RendererPixelFormat GetPixelFormat() const
	{
		return m_pixelFormat;
	}

	virtual void OnLostDevice() {}

	virtual void OnResetDevice() {}

	virtual bool Create(u32 nWidth, u32 nHeight, RendererPixelFormat pixelFormat, bool bBindShaderResource) = 0;

	virtual void Resize(u32 nWidth, u32 nHeight) = 0;

	bool IsFullScreen() const
	{
		return m_bFullScreen;
	}

	u32 GetWidth() const
	{
		return m_nWidth;
	}

	u32 GetHeight() const
	{
		return m_nHeight;
	}

	DepthStencilKey GetKey() const
	{
		DepthStencilKey key((u16)m_nWidth, (u16)m_nHeight, (u16)m_pixelFormat, m_bBindShaderResource);
		return key;
	}
protected:
	//AIRString m_strName;
	bool m_bBindShaderResource;   //是否用于纹理显示
	RendererPixelFormat m_pixelFormat;
	u32 m_nWidth;
	u32 m_nHeight;
	bool m_bFullScreen;
	float m_fScreenRatio;   //与屏幕大小的比例
private:
	
};
