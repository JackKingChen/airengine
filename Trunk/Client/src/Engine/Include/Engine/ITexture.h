#pragma once

#include "EngineResource.h"
#include "Color.h"
#include "EngineDefination.h"

class IRenderer;


typedef struct tagTextureInfo 
{
    AIRString m_strName;
    AIRString m_strFilename;
    bool		m_bRenderTarget;
}TEXTUREINFO;


enum TEXTURE_TYPE
{
	TT_TEXTURE	= 1,
	TT_VOLUMN	= 2,
	TT_CUBE		= 3,
	TT_UNKNOWN	= 0xffffffff,
};

enum CUBE_FACE_TYPE
{
	CFT_POS_X	= 0,
	CFT_NEG_X	= 1,
	CFT_POS_Y	= 2,
	CFT_NEG_Y	= 3,
	CFT_POS_Z	= 4,
	CFT_NEG_Z	= 5,
};

struct TextureParam 
{
    TextureParam() : nWidth(0), nHeight(0), nDepth(1), nMipmaps(0), usage(eRSU_Default)
        , pixelFormat(RPF_A8R8G8B8)
        , type(TT_TEXTURE)
        , bUseVideoMemory(false)
		, nBitsPerPixel(0)
    {

    }
    TextureParam(int width, int height, int depth, int miplevels, RenderSystemUsage _usage,
        RendererPixelFormat _format, TEXTURE_TYPE _type, bool bUseVideoMem = false, bool bDepthSurface = false) : nWidth(width),
        nHeight(height)
        , nDepth(depth)
        , nMipmaps(miplevels)
        , usage(_usage)
        , pixelFormat(_format)
        , type(_type)
        , bUseVideoMemory(bUseVideoMem)
		, nBitsPerPixel(0)
    {

    }

	void operator = (const TextureParam& other )
	{
		nWidth = other.nWidth;
		nHeight = other.nHeight;
		nDepth = other.nDepth;
		nMipmaps = other.nMipmaps;
		usage = other.usage;
		pixelFormat = other.pixelFormat;
		type = other.type;
		bUseVideoMemory = other.bUseVideoMemory;
		nBitsPerPixel = other.nBitsPerPixel;
	}

    int nWidth;
    int nHeight;
    int nDepth;
    int nMipmaps;
    RenderSystemUsage usage;
    RendererPixelFormat pixelFormat;
    TEXTURE_TYPE  type;
    bool bUseVideoMemory;
	u32 nBitsPerPixel;

    static TextureParam GetRenderTarget(int _width, int _height, RendererPixelFormat format)
    {
        return TextureParam(_width, _height, 1, 1,
            eRSU_RenderTarget, format, TT_TEXTURE, true, false);
    }
    static TextureParam GetRenderTargetFloat32(int _width, int _height)
    {
        return TextureParam(_width, _height, 1, 1,
            eRSU_RenderTarget, RPF_R32F, TT_TEXTURE, true, false);
    }
};

struct tagBufferBox
{
	u32 nLeft;
	u32 nTop;
	u32 nFront;
	u32 nRight;
	u32 nBottom;
	u32 nBack;
};

class EngineExport TextureTrunk
{
public:
	TextureTrunk();
	~TextureTrunk();

	TextureParam mTexParam;
	u32 mPixelDataSize;
	u8* mPixels;

	bool ReadFromStream( CDataStream* pStream );

	bool SaveToFile( const char* szPath );

	bool ConvertImageToTextureTrunk(const char* szSrcImageFullPath, bool coverIfExist);
};

class EngineExport ITexture : public Resource
{
	DECLARE_RTTI;
public:
    ITexture() : 
		//m_nWidth(0), 
		//m_nHeight(0), 
		//m_nDepth(0), 
		//m_nOrgWidth(0), 
		//m_nOrgHeight(0), 
		//m_nOrgDepth(0), 
		m_nID(-1), 
		m_bResizeWithScreenSizeWhenResetDevice(false), 
		m_fRttScale(1.0f),
		m_LoadFromImage(true)
		//m_TextureType(TT_UNKNOWN)
    {

    }

    virtual ~ITexture() 
	{

	}

	// 通过引用计数释放资源
    virtual s32 Release() = 0;

	// 获得宽度
    virtual u32 GetWidth() const
    {
        return m_textureParam.nWidth;
    }

	// 获得高度
    virtual u32 GetHeight() const
    {
        return m_textureParam.nHeight;
    }

	// 获得深度
	virtual u32 GetDepth() const
	{
		return m_textureParam.nDepth;
	}

    /*
	// 获得原始宽度
    virtual u32			GetOriginalWidth()
    {
        return m_nOrgWidth;
    }

	// 获得原始高度
    virtual u32			GetOriginalHeight()
    {
        return m_nOrgHeight;
    }

	// 获得原始深度
	virtual u32			GetOriginalDepth()
	{
		return m_nOrgDepth;
	}
    */

	// 获得纹理的ID
    virtual s32			GetID() 
	{ 
		return m_nID; 
	}

	// 设置纹理的ID
    virtual void		SetID(s32 nID) 
	{ 
		m_nID = nID; 
	}

	// 加锁RTT,操作RTT纹理像素数据
	virtual void*		Lock( int level = 0 ) = 0;

	// 加锁体积纹理,操作体积纹理数据
    //@nRowPitch
    //@nSlicePitch
	virtual void*		LockVolumn(int& nRowPitch, int& nSlicePitch, int level = 0 ) = 0;

	// 加锁立方纹理,操作立方纹理数据
	virtual void*		LockCubeMap( int surfaceType, int level = 0 ) = 0;

	// 解锁RTT
    virtual void		Unlock() = 0;

	// 获得D3D纹理对象
    //virtual void*		GetInterface() = 0;

    // 强行清空纹理
    virtual void		Clear() = 0;

	// 保存纹理到文件
    virtual bool		SaveToFile(const char* szFilename) = 0;

	// 获得纹理的名称
    const AIRString&	GetName() const 
	{ 
		return m_strName; 
	}

	// 查询纹理是否RTT]
    virtual bool		IsRenderTarget() const = 0;

    // 获得mipmap数量
    int GetMipmap()
    {
        return m_textureParam.nMipmaps;
    }

    
	// 读取文件, 创建平面纹理/体积纹理/立方纹理
    virtual bool CreateFromFile(const char* szFilename, const TextureParam& param) = 0;
	bool CreateFromFile(const char* szFilename);

	// 根据给定格式创建平面纹理/体积纹理/立方纹理
	//virtual bool		Create( const AIRString& strName, TEXTURE_TYPE textureType, u32 nWidth, u32 nHeight, u32 nDepth, u32 nMipMap, bool bRenderTarget, RendererPixelFormat nPixelFormat ) = 0;

    virtual bool Create(const char* szName, const TextureParam& param) = 0;
    //************************************
    // Method:    CopyPixelToTexture
    // FullName:  CDXTexture::CopyPixelToTexture
    // Access:    public 
    // Returns:   bool
    // Qualifier: 填充纹量的一部分，区域为(nPosX, nPosY) (nPosX + nOffsetX, nPosY + nOffsetY)
    // Parameter: const u8 * pPixel     填充的内容
    // Parameter: int nPosX       纹理的当前位置X坐标
    // Parameter: int nPosY       纹理的当前位置Y坐标
    // Parameter: int nOffsetX    纹理的X夸度
    // Parameter: int nOffsetY    纹理的Y夸度
    // Parameter: int nBits    像素的位数
    //************************************
    virtual bool		CopyPixelToTexture(const u8* pPixel, int nPosX, int nPosY, int nOffsetX, int nOffsetY, int nBits = 1) = 0;

	// 复制字体像素数据到纹理
    virtual bool		CopyMonoFontToTexture(const u8* pPixel, int nPosX, int nPosY, int nWidth, int nHeight, int nGlyWidth, SColor color = SColor(0xffffffff)) = 0;

	//
	virtual bool CopyPixelBoxToTexture(const u8* pPixel, const tagBufferBox* pBox, u32 nSrcOffset, u32 nSrcSize, u32 nRowPitch, u32 nDepthPitch) = 0;

	// 丢失Device的事件动作
	//virtual bool		OnLostDevice( void ) = 0;

	// 重置Device的事件动作
	//virtual bool		OnResetDevice( void ) = 0;

	// 获得纹理的资源文件名
    const AIRString&	GetFileName() const
    {
        return m_strFilename;
    }

	// 设置重建RTT的参数
	virtual void		SetResetOperation( bool bResizeWithScreenSizeWhenResetDevice, float fRttScale )
	{
		m_bResizeWithScreenSizeWhenResetDevice = bResizeWithScreenSizeWhenResetDevice;
		m_fRttScale = fRttScale;
	}


	// 重置纹理的大小
    virtual void		ResetSize(int nWidth, int nHeight) = 0;

    // 获得像素格式
    RendererPixelFormat GetPixelFormat() const
    {
        return m_textureParam.pixelFormat;
    }

	// 获得纹理的类型
	TEXTURE_TYPE GetTextureType( void ) const
	{
		return m_textureParam.type;
	}

    void SetFilename(const char* szFilename)
    {
        m_strFilename = szFilename;
    }

    virtual void* GetPixelBuffer() = 0;

	virtual void* RetriveRenderTargetPixelData( bool flip = false ) = 0;

	virtual void Apply(u32 nStage) = 0;

	const TextureParam& GetTextureParam() const
	{
		return m_textureParam;
	}

protected:
    //u32					m_nWidth;								// 纹理宽度
    //u32					m_nHeight;								// 纹理高度
	//u32					m_nDepth;
    //u32					m_nOrgWidth;							// 原始宽度
    //u32					m_nOrgHeight;							// 原始高度 
	//u32					m_nOrgDepth;
    //u32					m_nPixelFormat;							// 像素格式
    s32					m_nID;									// 资源索引
    //AIRString			m_strFilename;							// 文件名称
	//TEXTURE_TYPE		m_TextureType;							// 纹理类型
    TextureParam        m_textureParam;
	bool				m_LoadFromImage;

protected:
	// 重建rtt所用的参数
	bool				m_bResizeWithScreenSizeWhenResetDevice;	// 重置设备时是否需要按屏幕大小重建纹理(用于RTT)
	float				m_fRttScale;							// 纹理与屏幕大小的缩放比率(用于RTT)
	AIRString m_strName;
};
