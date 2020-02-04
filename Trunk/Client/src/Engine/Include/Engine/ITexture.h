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

	// ͨ�����ü����ͷ���Դ
    virtual s32 Release() = 0;

	// ��ÿ��
    virtual u32 GetWidth() const
    {
        return m_textureParam.nWidth;
    }

	// ��ø߶�
    virtual u32 GetHeight() const
    {
        return m_textureParam.nHeight;
    }

	// ������
	virtual u32 GetDepth() const
	{
		return m_textureParam.nDepth;
	}

    /*
	// ���ԭʼ���
    virtual u32			GetOriginalWidth()
    {
        return m_nOrgWidth;
    }

	// ���ԭʼ�߶�
    virtual u32			GetOriginalHeight()
    {
        return m_nOrgHeight;
    }

	// ���ԭʼ���
	virtual u32			GetOriginalDepth()
	{
		return m_nOrgDepth;
	}
    */

	// ��������ID
    virtual s32			GetID() 
	{ 
		return m_nID; 
	}

	// ���������ID
    virtual void		SetID(s32 nID) 
	{ 
		m_nID = nID; 
	}

	// ����RTT,����RTT������������
	virtual void*		Lock( int level = 0 ) = 0;

	// �����������,���������������
    //@nRowPitch
    //@nSlicePitch
	virtual void*		LockVolumn(int& nRowPitch, int& nSlicePitch, int level = 0 ) = 0;

	// ������������,����������������
	virtual void*		LockCubeMap( int surfaceType, int level = 0 ) = 0;

	// ����RTT
    virtual void		Unlock() = 0;

	// ���D3D�������
    //virtual void*		GetInterface() = 0;

    // ǿ���������
    virtual void		Clear() = 0;

	// ���������ļ�
    virtual bool		SaveToFile(const char* szFilename) = 0;

	// ������������
    const AIRString&	GetName() const 
	{ 
		return m_strName; 
	}

	// ��ѯ�����Ƿ�RTT]
    virtual bool		IsRenderTarget() const = 0;

    // ���mipmap����
    int GetMipmap()
    {
        return m_textureParam.nMipmaps;
    }

    
	// ��ȡ�ļ�, ����ƽ������/�������/��������
    virtual bool CreateFromFile(const char* szFilename, const TextureParam& param) = 0;
	bool CreateFromFile(const char* szFilename);

	// ���ݸ�����ʽ����ƽ������/�������/��������
	//virtual bool		Create( const AIRString& strName, TEXTURE_TYPE textureType, u32 nWidth, u32 nHeight, u32 nDepth, u32 nMipMap, bool bRenderTarget, RendererPixelFormat nPixelFormat ) = 0;

    virtual bool Create(const char* szName, const TextureParam& param) = 0;
    //************************************
    // Method:    CopyPixelToTexture
    // FullName:  CDXTexture::CopyPixelToTexture
    // Access:    public 
    // Returns:   bool
    // Qualifier: ���������һ���֣�����Ϊ(nPosX, nPosY) (nPosX + nOffsetX, nPosY + nOffsetY)
    // Parameter: const u8 * pPixel     ��������
    // Parameter: int nPosX       ����ĵ�ǰλ��X����
    // Parameter: int nPosY       ����ĵ�ǰλ��Y����
    // Parameter: int nOffsetX    �����X���
    // Parameter: int nOffsetY    �����Y���
    // Parameter: int nBits    ���ص�λ��
    //************************************
    virtual bool		CopyPixelToTexture(const u8* pPixel, int nPosX, int nPosY, int nOffsetX, int nOffsetY, int nBits = 1) = 0;

	// ���������������ݵ�����
    virtual bool		CopyMonoFontToTexture(const u8* pPixel, int nPosX, int nPosY, int nWidth, int nHeight, int nGlyWidth, SColor color = SColor(0xffffffff)) = 0;

	//
	virtual bool CopyPixelBoxToTexture(const u8* pPixel, const tagBufferBox* pBox, u32 nSrcOffset, u32 nSrcSize, u32 nRowPitch, u32 nDepthPitch) = 0;

	// ��ʧDevice���¼�����
	//virtual bool		OnLostDevice( void ) = 0;

	// ����Device���¼�����
	//virtual bool		OnResetDevice( void ) = 0;

	// ����������Դ�ļ���
    const AIRString&	GetFileName() const
    {
        return m_strFilename;
    }

	// �����ؽ�RTT�Ĳ���
	virtual void		SetResetOperation( bool bResizeWithScreenSizeWhenResetDevice, float fRttScale )
	{
		m_bResizeWithScreenSizeWhenResetDevice = bResizeWithScreenSizeWhenResetDevice;
		m_fRttScale = fRttScale;
	}


	// ��������Ĵ�С
    virtual void		ResetSize(int nWidth, int nHeight) = 0;

    // ������ظ�ʽ
    RendererPixelFormat GetPixelFormat() const
    {
        return m_textureParam.pixelFormat;
    }

	// ������������
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
    //u32					m_nWidth;								// ������
    //u32					m_nHeight;								// ����߶�
	//u32					m_nDepth;
    //u32					m_nOrgWidth;							// ԭʼ���
    //u32					m_nOrgHeight;							// ԭʼ�߶� 
	//u32					m_nOrgDepth;
    //u32					m_nPixelFormat;							// ���ظ�ʽ
    s32					m_nID;									// ��Դ����
    //AIRString			m_strFilename;							// �ļ�����
	//TEXTURE_TYPE		m_TextureType;							// ��������
    TextureParam        m_textureParam;
	bool				m_LoadFromImage;

protected:
	// �ؽ�rtt���õĲ���
	bool				m_bResizeWithScreenSizeWhenResetDevice;	// �����豸ʱ�Ƿ���Ҫ����Ļ��С�ؽ�����(����RTT)
	float				m_fRttScale;							// ��������Ļ��С�����ű���(����RTT)
	AIRString m_strName;
};
