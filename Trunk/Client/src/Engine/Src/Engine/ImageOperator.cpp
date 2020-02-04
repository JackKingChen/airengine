#include "stdafx.h"

#ifdef _WIN32
#ifdef _MSC_VER
#ifdef USE_DEVIL

#include "ImageOperator.h"
#include "Engine.h"
#include "FileSystem.h"
#include <il/il.h>
#include <il/ilu.h>
#include <il/ilut.h>
#include "StringUtil.h"
#include "Color.h"



ImageOperator::ImageOperator()
{
    IMAGEOPERATORMGR;
    size_t size = sizeof(ImageOperator);
	ZeroMemory( this, size );
	ilGenImages(1, &m_dwImgName);
	m_ScaleValueFilter = eSPV_BILINEAR;
	m_ScaleValuePlacement = eSPV_CENTER;
    m_szFile.clear();
}


ImageOperator::~ImageOperator()
{
	ilDeleteImages( 1, &m_dwImgName );
	SafeDeleteArray( m_pData );
	SafeDeleteArray( m_pPelette );
    m_szFile.clear();
}

// 从文件加载图像
bool	ImageOperator::LoadImageFromFile( const char* szFile, bool bFullPath /* = false */ )
{

    // 如果已经加载了图像文件,就直接返回true
    AIRString strFullFile = szFile;
	if( !bFullPath )
	{
		strFullFile = FILESYSTEM->GetFileFullPath(szFile);
	}
    if ( m_szFile == strFullFile && m_pData )
    {
        return true;
    }
	char buf[2048];
	::GetCurrentDirectoryA(1024, buf);
	strFullFile = buf + strFullFile;
	strFullFile = CStringUtil::Replace( strFullFile, "\\", "/" );

    // 释放已经加载的资源
    ResetForDynamicImageOpt();

    // 记录所加载的文件
	m_szFile = strFullFile;

	//ILuint imgName;
	ilBindImage(m_dwImgName);
	if ( iluLoadImage(m_szFile.c_str()) == IL_FALSE )
	{
		DT_TO_MSGBOX_FMT( LT_ERROR, GetString("Get Image File [%s] Failed!",m_szFile.c_str()) );
		return false;
	}

	ILinfo imgInfo;
	memset(&imgInfo, 0, sizeof(ILinfo));
	iluGetImageInfo(&imgInfo);
	m_nWidth		= imgInfo.Width;
	m_nHeight		= imgInfo.Height;
	m_nDepth		= imgInfo.Depth;
	m_byBPP			= imgInfo.Bpp;
	m_dwSizeOfData	= imgInfo.SizeOfData;
	m_PixelFormat	= (PixelFormatDevil)imgInfo.Format;
	m_CodecType		= (CodecType)imgInfo.Type;
	m_OriginType	= (OriginType)imgInfo.Origin;
	m_CubeFlag		= (CubeFlag)imgInfo.CubeFlags;
	m_nNumMips		= imgInfo.NumMips;
	m_nNumLayers	= imgInfo.NumLayers;
	m_nNumNext		= imgInfo.NumNext;
	m_pData			= new u8[m_dwSizeOfData];
	int res			= ilCopyPixels(0, 0, 0, m_nWidth, m_nHeight, m_nDepth, m_PixelFormat, IL_UNSIGNED_BYTE, m_pData);
	u8* pPalette	= ilGetPalette();
	if ( imgInfo.PalSize > 0 && pPalette )
	{
		m_PaletteType = (PaletteType)imgInfo.PalType;
		m_PalSize = imgInfo.PalSize;
		m_pPelette = new u8[ m_PalSize ];
		memcpy( m_pPelette, pPalette, m_PalSize );
	}	

	return true;
}


void	ImageOperator::ResetForDynamicImageOpt( void )
{
	SafeDeleteArray( m_pData );
	SafeDeleteArray( m_pPelette );
	m_nWidth		= 0;
	m_nHeight		= 0;
	m_nDepth		= 0;
	m_byBPP			= 0;
	m_dwSizeOfData	= 0;
	m_PixelFormat	= (PixelFormatDevil::ePF_BGRA);
	m_CodecType		= (CodecType::eCT_UNSIGNED_BYTE);
	m_OriginType	= (OriginType::eOT_ORIGIN_UPPER_LEFT);
	m_CubeFlag		= (CubeFlag)0;
	m_nNumMips		= 0;
	m_nNumLayers	= 0;
	m_PaletteType	= ePAL_NONE;
	m_PalSize		= 0;
	m_nNumNext		= 0;
    m_szFile.clear();
}


// 保存图像到文件
bool	ImageOperator::SaveImageToFile( const char* szFile )
{
	AIRString strFile(szFile);
	AIRString strExt("");
	int i = strFile.size()-1;
	for ( ; i >= 0; --i )
	{
		if ( strFile[i] == '.' )
		{
			break;
		}
	}
	++i;
	for ( ; i < strFile.size(); ++i )
	{
		strExt.push_back( strFile[i] );
	}
	CStringUtil::ToUpperCase( strExt );

	ilPushAttrib( IL_ORIGIN_SET );
	ilSetInteger( IL_ORIGIN_SET, m_OriginType );
	ilEnable( IL_ORIGIN_MODE );

	// 获得要保存的图像格式编号
	ImgExtType iet = IMAGEOPERATORMGR->GetImgExtType( strExt );
	if ( iet == eIET_UNKNOWN )
	{
		DT_TO_MSGBOX_FMT( LT_ERROR, "ERROR: unknown img extend format!" );
		return false;
	}

	ilBindImage( m_dwImgName );

	// 设置图像数据格式
	if( !ilConvertImage( m_PixelFormat, IL_UNSIGNED_BYTE ) )
	{
		DT_TO_MSGBOX_FMT( LT_ERROR, GetString("Save File [%s] Failed! - ilConvertImage() failed",szFile) );
		return false;
	}

	// 调整图像大小
	//if( !iluEnlargeCanvas( m_nWidth, m_nHeight, m_nDepth ) )
	//{
	//	DT_TO_MSGBOX_FMT( LT_ERROR, GetString("Save File [%s] Failed! - iluEnlargeCanvas() failed",szFile) );
	//	return false;
	//}

	// 设置数据
	//if( !ilSetData(m_pData) )
	//{
	//	DT_TO_MSGBOX_FMT( LT_ERROR, GetString("Save File [%s] Failed! - ilSetData() failed",szFile) );
	//	return false;
	//}

	// 如果文件已经存在要先删除文件
	if ( IsFileExist( szFile ) )
	{
		DeleteFileA( szFile );
	}

	// 保存文件
	if( !ilSave( iet, szFile ) )
	{
		DT_TO_MSGBOX_FMT( LT_ERROR, GetString("Save File [%s] Failed! - ilSave() failed",szFile) );
		return false;
	}

	ilDisable( IL_ORIGIN_MODE );
	ilPopAttrib();
	return true;
}

u32	ImageOperator::GetPixelAsInt( int nX, int nY, int nZ /*= 0*/ )
{
	if ( !m_pData )
	{
		DT_TO_DBGSTR( LT_ERROR, "no pixel data!" );
		return 0;
	}
	if ( nX < 0 || nX >= m_nWidth || nY < 0 || nY >= m_nHeight || nZ < 0 || nZ >= m_nDepth )
	{
		DT_TO_DBGSTR( LT_ERROR, "pixel position out of bound!" );
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	if ( m_OriginType == eOT_ORIGIN_UPPER_LEFT )
	{
		//nY = m_nHeight - nY;
	}
	//////////////////////////////////////////////////////////////////////////
	u32 id = (m_nWidth*m_nHeight*nZ + m_nWidth*nY + nX)*m_byBPP; 
    u8* pData = (u8*)m_pData;
	switch ( m_PixelFormat )
	{
		//索引颜色
	case ePF_COLOR_INDEX:		
		{
			//像素对应的索引号
			int nPalID = pData[id];
			//索引颜色数据表中的id
			int nCurID = 0;
			switch ( m_PaletteType )
			{
			case ePAL_NONE:
				return 0;
				// RGB 24 位色, 3个字节全部用上
			case ePAL_RGB24:
				{
					nCurID = nPalID * 3;
					u32 r = m_pPelette[nCurID] << 16;
					u32 g = m_pPelette[nCurID + 1] << 8;
					u32 b = m_pPelette[nCurID + 2];
					return r + g + b;
				}
				// RGB 32 位色, 前3个字节分别对应RGB, 第4个字节没用
			case ePAL_RGB32:
				{
					nCurID = nPalID * 4;
					u32 r = m_pPelette[nCurID] << 16;
					u32 g = m_pPelette[nCurID + 1] << 8;
					u32 b = m_pPelette[nCurID + 2];
					return r + g + b;
				}
				// RGBA 32 位色, 4个字节分别对应RGBA
			case ePAL_RGBA32:
				{
					nCurID = nPalID * 4;
					u32 r = m_pPelette[nCurID] << 24;
					u32 g = m_pPelette[nCurID + 1] << 16;
					u32 b = m_pPelette[nCurID + 2] << 8;
					u32 a = m_pPelette[nCurID + 3];
					return r + g + b + a;
				}
				// BGR 24 位色, 3个字节全部用上
			case ePAL_BGR24:
				{
					nCurID = nPalID * 3;
					u32 b = m_pPelette[nCurID] << 16;
					u32 g = m_pPelette[nCurID + 1] << 8;
					u32 r = m_pPelette[nCurID + 2];
					return b + g + r;
				}
				// BGR 32 位色, 前3个字节分别对应BGR, 第4个字节没用
			case ePAL_BGR32:
				{
					nCurID = nPalID * 4;
					u32 b = m_pPelette[nCurID] << 16;
					u32 g = m_pPelette[nCurID + 1] << 8;
					u32 r = m_pPelette[nCurID + 2];
					return b + g + r;
				}
				// BGRA 32 位色, 4个字节分别对应BGRA
			case ePAL_BGRA32:
				{
					nCurID = nPalID * 4;
					u32 b = m_pPelette[nCurID] << 24;
					u32 g = m_pPelette[nCurID + 1] << 16;
					u32 r = m_pPelette[nCurID + 2] << 8;
					u32 a = m_pPelette[nCurID + 3];
					return b + g + r + a;
				}
			}
			return 0;
		}

		// 单通道ALPHA
	case ePF_ALPHA:				return (pData[id]);

		// 单通道灰度
	case ePF_LUMINANCE:			return (pData[id]);

		// 灰度+ALPHA
	case ePF_LUMINANCE_ALPHA:	return (pData[id] << 8 | pData[id+1]);

		// | R | G | B | ...| R | G | B |
		//   0   1   2   ...  k  k+1 k+2
	case ePF_RGB:				//return (m_pData[id]<<16 + m_pData[id+1]<<8 + m_pData[id+2]);
        {
            u32 r = pData[id] << 16;
            u32 g = pData[id + 1] << 8;
            u32 b = pData[id + 2];
            return b + g + r;
        }

		// | R | G | B | A | ...| R | G | B | A |
		//   0   1   2   3   ...  k  k+1 k+2 k+3
	case ePF_RGBA:				
        {
            u32 r = pData[id] << 24;
            u32 g = pData[id + 1] << 16;
            u32 b = pData[id + 2] << 8;
            u32 a = pData[id + 3];
            return b + g + r + a;
        }

    // add by liangiaran BGR格式
    case ePF_BGR:
        {
            u32 b = pData[id] << 16;
            u32 g = pData[id + 1] << 8;
            u32 r = pData[id + 2];
            return b + g + r;
        }

	case ePF_BGRA:
		{
			u32 b = pData[id] << 24;
			u32 g = pData[id + 1] << 16;
			u32 r = pData[id + 2] << 8;
			u32 a = pData[id + 3];
			return b + g + r + a;
		}
        
	}
	return (0);
}

u32 ImageOperator::GetPixelAsRGBA(int nX, int nY, int nZ /* = 0 */) const
{
    if ( !m_pData )
    {
        DT_TO_DBGSTR( LT_ERROR, "no pixel data!" );
        return 0;
    }
    if ( nX < 0 || nX >= m_nWidth || nY < 0 || nY >= m_nHeight || nZ < 0 || nZ >= m_nDepth )
    {
        DT_TO_DBGSTR( LT_ERROR, "pixel position out of bound!" );
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    if ( m_OriginType == eOT_ORIGIN_UPPER_LEFT )
    {
        nY = m_nHeight - nY;
    }
    //////////////////////////////////////////////////////////////////////////
    u32 id = (m_nWidth*m_nHeight*nZ + m_nWidth*nY + nX)*m_byBPP; 
    u8* pData = (u8*)m_pData;

    u32 r = 0;
    u32 g = 0;
    u32 b = 0;
    u32 a = 255;
    switch ( m_PixelFormat )
    {
		//索引颜色
	case ePF_COLOR_INDEX:
		{
			//像素对应的索引号
			int nPalID = pData[id];
			//索引颜色数据表中的id
			int nCurID = 0;
			switch ( m_PaletteType )
			{
			case ePAL_NONE:
				break;

				// RGB 24 位色, 3个字节全部用上
			case ePAL_RGB24:
				{
					nCurID = nPalID * 3;
					r = m_pPelette[nCurID] << 16;
					g = m_pPelette[nCurID + 1] << 8;
					b = m_pPelette[nCurID + 2];
				}
				break;

				// RGB 32 位色, 前3个字节分别对应RGB, 第4个字节没用
			case ePAL_RGB32:
				{
					nCurID = nPalID * 4;
					r = m_pPelette[nCurID] << 16;
					g = m_pPelette[nCurID + 1] << 8;
					b = m_pPelette[nCurID + 2];
				}
				break;

				// RGBA 32 位色, 4个字节分别对应RGBA
			case ePAL_RGBA32:
				{
					nCurID = nPalID * 4;
					r = m_pPelette[nCurID] << 24;
					g = m_pPelette[nCurID + 1] << 16;
					b = m_pPelette[nCurID + 2] << 8;
					a = m_pPelette[nCurID + 3];
				}
				break;

				// BGR 24 位色, 3个字节全部用上
			case ePAL_BGR24:
				{
					nCurID = nPalID * 3;
					b = m_pPelette[nCurID] << 16;
					g = m_pPelette[nCurID + 1] << 8;
					r = m_pPelette[nCurID + 2];
				}
				break;

				// BGR 32 位色, 前3个字节分别对应BGR, 第4个字节没用
			case ePAL_BGR32:
				{
					nCurID = nPalID * 4;
					b = m_pPelette[nCurID] << 16;
					g = m_pPelette[nCurID + 1] << 8;
					r = m_pPelette[nCurID + 2];
				}
				break;

				// BGRA 32 位色, 4个字节分别对应BGRA
			case ePAL_BGRA32:
				{
					nCurID = nPalID * 4;
					b = m_pPelette[nCurID] << 24;
					g = m_pPelette[nCurID + 1] << 16;
					r = m_pPelette[nCurID + 2] << 8;
					a = m_pPelette[nCurID + 3];
				}
				break;
			}
		}
		break;

	case ePF_ALPHA:
		a = pData[id];
		break;

    case ePF_LUMINANCE:		
        r = g = b = pData[id];
        break;

    case ePF_LUMINANCE_ALPHA:	
		{
			r = g = b = pData[id] << 8;
			a = pData[id+1];
		}
        break;

        // | R | G | B | ...| R | G | B |
        //   0   1   2   ...  k  k+1 k+2
    case ePF_RGB:				//return (m_pData[id]<<16 + m_pData[id+1]<<8 + m_pData[id+2]);
        {
            r = pData[id] << 16;
            g = pData[id + 1] << 8;
            b = pData[id + 2];
        }
        break;

        // | R | G | B | A | ...| R | G | B | A |
        //   0   1   2   3   ...  k  k+1 k+2 k+3
    case ePF_RGBA:				//return (m_pData[id]<<24 + m_pData[id+1]<<16 + m_pData[id+2]<<8 + m_pData[id+3]);
        {
            r = pData[id] << 24;
            g = pData[id + 1] << 16;
            b = pData[id + 2] << 8;
            a = pData[id + 3];
        }
        break;

        // add by liangiaran BGR格式
    case ePF_BGR:
        {
            b = pData[id] << 16;
            g = pData[id + 1] << 8;
            r = pData[id + 2];
        }
        break;

	case ePF_BGRA:
		{
			b = pData[id] << 24;
			g = pData[id + 1] << 16;
			r = pData[id + 2] << 8;
			a = pData[id + 3];
		}
		break;
    }
    return r | g | b | a;
}

u32 ImageOperator::GetPixelAsARGB(int nX, int nY, int nZ /* = 0 */) const
{
    if ( !m_pData )
    {
        DT_TO_DBGSTR( LT_ERROR, "no pixel data!" );
        return 0;
    }
    if ( nX < 0 || nX >= m_nWidth || nY < 0 || nY >= m_nHeight || nZ < 0 || nZ >= m_nDepth )
    {
        DT_TO_DBGSTR( LT_ERROR, "pixel position out of bound!" );
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    if ( m_OriginType == eOT_ORIGIN_UPPER_LEFT )
    {
        nY = m_nHeight - nY;
    }
    //////////////////////////////////////////////////////////////////////////
    u32 id = (m_nWidth*m_nHeight*nZ + m_nWidth*nY + nX)*m_byBPP; 
    u8* pData = (u8*)m_pData;

    u32 r = 0;
    u32 g = 0;
    u32 b = 0;
    u32 a = 255;
    switch ( m_PixelFormat )
    {
		//索引颜色
	case ePF_COLOR_INDEX:
		{
			//像素对应的索引号
			int nPalID = pData[id];
			//索引颜色数据表中的id
			int nCurID = 0;
			switch ( m_PaletteType )
			{
			case ePAL_NONE:
				break;

				// RGB 24 位色, 3个字节全部用上
			case ePAL_RGB24:
				{
					nCurID = nPalID * 3;
					r = m_pPelette[nCurID] << 16;
					g = m_pPelette[nCurID + 1] << 8;
					b = m_pPelette[nCurID + 2];
				}
				break;

				// RGB 32 位色, 前3个字节分别对应RGB, 第4个字节没用
			case ePAL_RGB32:
				{
					nCurID = nPalID * 4;
					r = m_pPelette[nCurID] << 16;
					g = m_pPelette[nCurID + 1] << 8;
					b = m_pPelette[nCurID + 2];
				}
				break;

				// RGBA 32 位色, 4个字节分别对应RGBA
			case ePAL_RGBA32:
				{
					nCurID = nPalID * 4;
					r = m_pPelette[nCurID] << 24;
					g = m_pPelette[nCurID + 1] << 16;
					b = m_pPelette[nCurID + 2] << 8;
					a = m_pPelette[nCurID + 3];
				}
				break;

				// BGR 24 位色, 3个字节全部用上
			case ePAL_BGR24:
				{
					nCurID = nPalID * 3;
					b = m_pPelette[nCurID] << 16;
					g = m_pPelette[nCurID + 1] << 8;
					r = m_pPelette[nCurID + 2];
				}
				break;

				// BGR 32 位色, 前3个字节分别对应BGR, 第4个字节没用
			case ePAL_BGR32:
				{
					nCurID = nPalID * 4;
					b = m_pPelette[nCurID] << 16;
					g = m_pPelette[nCurID + 1] << 8;
					r = m_pPelette[nCurID + 2];
				}
				break;

				// BGRA 32 位色, 4个字节分别对应BGRA
			case ePAL_BGRA32:
				{
					nCurID = nPalID * 4;
					b = m_pPelette[nCurID] << 24;
					g = m_pPelette[nCurID + 1] << 16;
					r = m_pPelette[nCurID + 2] << 8;
					a = m_pPelette[nCurID + 3];
				}
				break;
			}
		}
		break;

	case ePF_ALPHA:
		a = pData[id];
		break;

    case ePF_LUMINANCE:		
        r = g = b = pData[id];
        break;

    case ePF_LUMINANCE_ALPHA:	
        r = g = b = pData[id] << 8;
        a = pData[id+1];
        break;

        // | R | G | B | ...| R | G | B |
        //   0   1   2   ...  k  k+1 k+2
    case ePF_RGB:				
        {
            r = pData[id] << 16;
            g = pData[id + 1] << 8;
            b = pData[id + 2];
        }
        break;
        // | R | G | B | A | ...| R | G | B | A |
        //   0   1   2   3   ...  k  k+1 k+2 k+3
    case ePF_RGBA:				
        {
            r = pData[id] << 24;
            g = pData[id + 1] << 16;
            b = pData[id + 2] << 8;
            a = pData[id + 3];
        }
        break;
        // add by liangiaran BGR格式
    case ePF_BGR:
        {
            b = pData[id] << 16;
            g = pData[id + 1] << 8;
            r = pData[id + 2];
        }
        break;
	case ePF_BGRA:
		{
			b = pData[id] << 24;
			g = pData[id + 1] << 16;
			r = pData[id + 2] << 8;
			a = pData[id + 3];
		}
		break;
    }
    return a | r | g | b;
}

float ImageOperator::GetPixelAsFloat(int nX, int nY, int nZ /* = 0 */)
{
    if ( !m_pData )
    {
        DT_TO_DBGSTR( LT_ERROR, "no pixel data!" );
        return 0;
    }
    if ( nX < 0 || nX >= m_nWidth || nY < 0 || nY >= m_nHeight || nZ < 0 || nZ >= m_nDepth )
    {
        DT_TO_DBGSTR( LT_ERROR, "pixel position out of bound!" );
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    if ( m_OriginType == eOT_ORIGIN_UPPER_LEFT )
    {
        nY = m_nHeight - nY;
    }
    //////////////////////////////////////////////////////////////////////////
    u32 id = (m_nWidth * m_nHeight * nZ + m_nWidth * nY + nX); // * m_byBPP; 
    float* pData = (float*)m_pData;

    return pData[id];
}

u8* ImageOperator::GetPixelAsByteArray( int nX, int nY, int nZ /*= 0*/ )
{
	if ( !m_pData )
	{
		DT_TO_DBGSTR( LT_ERROR, "no pixel data!" );
		return NULL;
	}
	if ( nX < 0 || nX >= m_nWidth || nY < 0 || nY >= m_nHeight || nZ < 0 || nZ >= m_nDepth )
	{
		DT_TO_DBGSTR( LT_ERROR, "pixel position out of bound!" );
		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	if ( m_OriginType == eOT_ORIGIN_UPPER_LEFT )
	{
		nY = m_nHeight - nY;
	}
	//////////////////////////////////////////////////////////////////////////
	u32 id = (m_nWidth*m_nHeight*nZ + m_nWidth*nY + nX)*m_byBPP; 
	return &((u8*)m_pData)[id];
}

u8* ImageOperator::GetPixelAsByteArray( u32 id )
{
	if ( !m_pData )
	{
		DT_TO_DBGSTR( LT_ERROR, "no pixel data!" );
		return NULL;
	}
	if ( id >= m_dwSizeOfData )
	{
		DT_TO_DBGSTR( LT_ERROR, "pixel position out of bound!" );
		return NULL;
	}
	return &((u8*)m_pData)[id];
}

void ImageOperator::SetPixel( u32 value, int nX, int nY, int nZ /*= 0 */)
{
	if ( !m_pData )
	{
		DT_TO_DBGSTR( LT_ERROR, "no pixel data!" );
		return;
	}
	if ( nX < 0 || nX >= m_nWidth || nY < 0 || nY >= m_nHeight || nZ < 0 || nZ >= m_nDepth )
	{
		DT_TO_DBGSTR( LT_ERROR, "pixel position out of bound!" );
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	if ( m_OriginType == eOT_ORIGIN_UPPER_LEFT )
	{
		nY = m_nHeight - nY;
	}
	//////////////////////////////////////////////////////////////////////////
	u32 id = (m_nWidth*m_nHeight*nZ + m_nWidth*nY + nX)*m_byBPP; 
    u8* pData = (u8*)m_pData;
	switch ( m_PixelFormat )
	{
	case ePF_ALPHA:
		{
			pData[id]   = u8(  value ); 
		}
		break;

	case ePF_LUMINANCE:			
		{
			// Luminance
			pData[id]   = u8(  value ); 
		}
		break;

	case ePF_LUMINANCE_ALPHA:	
		{
			// Luminance
			pData[id]   = u8(  value & 0xff);
			// A
			pData[id+1] = u8( (value & 0xff00) >> 8 );
		}
		break;

	case ePF_RGB:
		{
			// R
			pData[id]   = u8( (value & 0xff0000) >> 16 );
			// G
			pData[id+1] = u8( (value & 0xff00) >> 8 );
			// B
			pData[id+2] = u8(  value & 0xff );
		}
		break;

	case ePF_RGBA:	
		{
			// R
			pData[id]   = u8( (value & 0xff0000) >> 16 );
			// G
			pData[id+1] = u8( (value & 0xff00) >> 8 );
			// B
			pData[id+2] = u8(  value & 0xff );
			// A
			pData[id+3] = u8( (value & 0xff000000) >> 24 );
		}
		break;

	case ePF_BGR:
		{
			// B
			pData[id]   = u8(  value & 0xff );
			// G
			pData[id+1] = u8( (value & 0xff00) >> 8 );
			// R
			pData[id+2] = u8( (value & 0xff0000) >> 16 );
		}
		break;

	case ePF_BGRA:
		{
			// B
			pData[id]   = u8(  value & 0xff );
			// G
			pData[id+1] = u8( (value & 0xff00) >> 8 );
			// R
			pData[id+2] = u8( (value & 0xff0000) >> 16 );
			// A
			pData[id+3] = u8( (value & 0xff000000) >> 24 );
		}
		break;
	}
}


void		ImageOperator::SetPixel( u8* pValue, int nX, int nY, int nZ /*= 0*/ )
{
	if ( !m_pData || !pValue )
	{
		DT_TO_DBGSTR( LT_ERROR, "no pixel data!" );
		return;
	}
	if ( nX < 0 || nX >= m_nWidth || nY < 0 || nY >= m_nHeight || nZ < 0 || nZ >= m_nDepth )
	{
		DT_TO_DBGSTR( LT_ERROR, "pixel position out of bound!" );
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	if ( m_OriginType == eOT_ORIGIN_UPPER_LEFT )
	{
		nY = m_nHeight - nY;
	}
	//////////////////////////////////////////////////////////////////////////
	u32 id = (m_nWidth*m_nHeight*nZ + m_nWidth*nY + nX)*m_byBPP; 
    u8* pData = (u8*)m_pData;
	switch ( m_PixelFormat )
	{
	case ePF_ALPHA:
		{
			pData[id]   = *(pValue); 
		}
		break;

	case ePF_LUMINANCE:			
		{
			pData[id]   = *(pValue); 
		}
		break;

	case ePF_LUMINANCE_ALPHA:	
		{
			pData[id]   = *(pValue);
			pData[id+1] = *(pValue+1);
		}
		break;

	case ePF_RGB:
		{
			pData[id]   = *(pValue);
			pData[id+1] = *(pValue+1);
			pData[id+2] = *(pValue+2);
		}
		break;

	case ePF_RGBA:	
		{
			pData[id]   = *(pValue+1);
			pData[id+1] = *(pValue+2);
			pData[id+2] = *(pValue+3);
			pData[id+3] = *(pValue);
		}
		break;

	case ePF_BGR:
		{
			pData[id]   = *(pValue+2);
			pData[id+1] = *(pValue+1);
			pData[id+2] = *(pValue);
		}
		break;

	case ePF_BGRA:	
		{
			pData[id]   = *(pValue+3);
			pData[id+1] = *(pValue+2);
			pData[id+2] = *(pValue+1);
			pData[id+3] = *(pValue);
		}
		break;
	}
}


void		ImageOperator::SetPixel( const SColor& col, int nX, int nY, int nZ /*= 0*/ )
{
	if ( !m_pData )
	{
		DT_TO_DBGSTR( LT_ERROR, "no pixel data!" );
		return;
	}
	if ( nX < 0 || nX >= m_nWidth || nY < 0 || nY >= m_nHeight || nZ < 0 || nZ >= m_nDepth )
	{
		DT_TO_DBGSTR( LT_ERROR, "pixel position out of bound!" );
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	if ( m_OriginType == eOT_ORIGIN_UPPER_LEFT )
	{
		nY = m_nHeight - nY;
	}
	//////////////////////////////////////////////////////////////////////////
	u32 id = (m_nWidth*m_nHeight*nZ + m_nWidth*nY + nX)*m_byBPP; 

    u8* pData = (u8*)m_pData;
	// R
	pData[id]   = u8( (col.color & 0xff0000) >> 16 );
	// G
	pData[id+1] = u8( (col.color & 0xff00) >> 8 );
	// B
	pData[id+2] = u8(  col.color & 0xff );
	// A
	pData[id+3] = u8( (col.color & 0xff000000) >> 24 );
}



size_t		ImageOperator::GetCodeTypeByteSize( CodecType ct )
{
	size_t nTurnelSize = 0;
	switch( ct )
	{
	case eCT_BYTE:
		nTurnelSize = sizeof( char );
		break;
	case eCT_UNSIGNED_BYTE:
		nTurnelSize = sizeof( unsigned char );
		break;
	case eCT_SHORT:
		nTurnelSize = sizeof( short );
		break;
	case eCT_UNSIGNED_SHORT:
		nTurnelSize = sizeof( unsigned short );
		break;
	case eCT_INT:
		nTurnelSize = sizeof( int );
		break;
	case eCT_UNSIGNED_INT:
		nTurnelSize = sizeof( unsigned int );
		break;
	case eCT_FLOAT:
		nTurnelSize = sizeof( float );
		break;
	case eCT_DOUBLE:
		nTurnelSize = sizeof( double );
		break;
	case eCT_HALF:
		nTurnelSize = 2;
		break;
	}
	return nTurnelSize;
}


// 拷贝矩形像素
size_t		ImageOperator::CopyPixelDataFromImage( int nOffsetX, int nOffsetY, int nWidth, int nHeight, void** ppData )
{
	if( !m_pData )
	{
		DT_TO_MSGBOX_FMT( LT_ERROR, "复制图像数据之前请先加载图像!" );
		return 0;
	}

	size_t size = nWidth * m_byBPP * GetCodeTypeByteSize( m_CodecType ) * nHeight;
	(*ppData) = new u8[size];
	ZeroMemory( ppData, 0 );
	ilBindImage( m_dwImgName );
	ilCopyPixels( nOffsetX,nOffsetY, 0, nWidth, nHeight, 1, m_PixelFormat, IL_UNSIGNED_BYTE, (*ppData) );
	return size;
}

bool ImageOperator::CopyPixelDataToImage(int nOffsetX, int nOffsetY, int nWidth, int nHeight, void* pData)
{
    if (!m_pData)
    {
        return false;
    }

    int nPixelOffset = 0;
    int nEndX = nOffsetX + nWidth;
    int nEndY = nOffsetY + nHeight;
    for (int row = nOffsetY, i = 0; row < nEndY; ++row, ++i)
    {
        for (int col = nOffsetX, j = 0; col < nEndX; ++col, ++j)
        {
            nPixelOffset = (row * m_nWidth + col) * m_byBPP;
            int nPixelSrcOffset = (i * nWidth + j) * m_byBPP;
            memcpy((u8*)m_pData + nPixelOffset, (u8*)pData + nPixelSrcOffset, m_byBPP * sizeof(u8));
            /*
            if (m_byBPP == 1)
            {
                *(u8*)&m_pData[nPixelOffset] = *(u8*)&pData[nPixelSrcOffset];
            }
            else if (m_byBPP == 3)
            {
                memcpy((u8*)m_pData + nPixelOffset, (u8*)pData + nPixelSrcOffset, 3);
            }
            else if (m_byBPP == 4)
            {
                *(u32*)&m_pData[nPixelOffset] = *(u32*)&pData[nPixelSrcOffset];
            }
            */
        }
    }

    ilBindImage( m_dwImgName );

    // 设置图像数据格式
    if( !ilConvertImage( m_PixelFormat, IL_UNSIGNED_BYTE ) )
    {
        return false;
    }

    // 调整图像大小
    if( !iluEnlargeCanvas( m_nWidth, m_nHeight, m_nDepth ) )
    {
        return false;
    }

    // 设置数据
    if( !ilSetData(m_pData) )
    {
        return false;
    }

    return true;
}


void		ImageOperator::BlurAvg( u32 iter )
{
	ilBindImage( m_dwImgName );
	if ( !iluBlurAvg( iter ) )
	{
		DT_TO_DBGSTR( LT_ERROR, "BlurAvg() Failed!" );
	}
	ilCopyPixels(0, 0, 0, m_nWidth, m_nHeight, m_nDepth, m_PixelFormat, IL_UNSIGNED_BYTE, m_pData);
}


void		ImageOperator::BlurGaussian( u32 iter )
{
	ilBindImage( m_dwImgName );
	if ( !iluBlurGaussian( iter ) )
	{
		DT_TO_DBGSTR( LT_ERROR, "BlurGaussian() Failed!" );
	}
	ilCopyPixels(0, 0, 0, m_nWidth, m_nHeight, m_nDepth, m_PixelFormat, IL_UNSIGNED_BYTE, m_pData);
}


void		ImageOperator::EdgeDetectE( void )
{
	ilBindImage( m_dwImgName );
	if ( !iluEdgeDetectE( ) )
	{
		DT_TO_DBGSTR( LT_ERROR, "EdgeDetectE() Failed!" );
	}
	ilCopyPixels(0, 0, 0, m_nWidth, m_nHeight, m_nDepth, m_PixelFormat, IL_UNSIGNED_BYTE, m_pData);
}

void		ImageOperator::EdgeDetectP( void )
{
	ilBindImage( m_dwImgName );
	if ( !iluEdgeDetectP( ) )
	{
		DT_TO_DBGSTR( LT_ERROR, "EdgeDetectP() Failed!" );
	}
	ilCopyPixels(0, 0, 0, m_nWidth, m_nHeight, m_nDepth, m_PixelFormat, IL_UNSIGNED_BYTE, m_pData);
}

void		ImageOperator::EdgeDetectS( void )
{
	ilBindImage( m_dwImgName );
	if ( !iluEdgeDetectS( ) )
	{
		DT_TO_DBGSTR( LT_ERROR, "EdgeDetectS() Failed!" );
	}
	ilCopyPixels(0, 0, 0, m_nWidth, m_nHeight, m_nDepth, m_PixelFormat, IL_UNSIGNED_BYTE, m_pData);
}


void		ImageOperator::GammaCorrect( float gama )
{
	ilBindImage( m_dwImgName );
	if ( !iluGammaCorrect( gama ) )
	{
		DT_TO_DBGSTR( LT_ERROR, "GammaCorrect() Failed!" );
	}
	ilCopyPixels(0, 0, 0, m_nWidth, m_nHeight, m_nDepth, m_PixelFormat, IL_UNSIGNED_BYTE, m_pData);
}


void		ImageOperator::FlipImage( void )
{
	ilBindImage( m_dwImgName );
	if ( !iluFlipImage( ) )
	{
		DT_TO_DBGSTR( LT_ERROR, "FlipImage() Failed!" );
	}
	ilCopyPixels(0, 0, 0, m_nWidth, m_nHeight, m_nDepth, m_PixelFormat, IL_UNSIGNED_BYTE, m_pData);
}


void		ImageOperator::Mirror( void )
{
	ilBindImage( m_dwImgName );
	if ( !iluMirror( ) )
	{
		DT_TO_DBGSTR( LT_ERROR, "Mirror() Failed!" );
	}
	ilCopyPixels(0, 0, 0, m_nWidth, m_nHeight, m_nDepth, m_PixelFormat, IL_UNSIGNED_BYTE, m_pData);
}


void		ImageOperator::Negative( void )
{
	ilBindImage( m_dwImgName );
	if ( !iluNegative( ) )
	{
		DT_TO_DBGSTR( LT_ERROR, "Negative() Failed!" );
	}
	ilCopyPixels(0, 0, 0, m_nWidth, m_nHeight, m_nDepth, m_PixelFormat, IL_UNSIGNED_BYTE, m_pData);

}


void		ImageOperator::Noisify( float tolerance )
{
	ilBindImage( m_dwImgName );
	if ( !iluNoisify( tolerance ) )
	{
		DT_TO_DBGSTR( LT_ERROR, "Noisify() Failed!" );
	}
	ilCopyPixels(0, 0, 0, m_nWidth, m_nHeight, m_nDepth, m_PixelFormat, IL_UNSIGNED_BYTE, m_pData);

}


void		ImageOperator::Pixelize( u32 dwPixelSize )
{
	ilBindImage( m_dwImgName );
	if ( !iluPixelize( dwPixelSize ) )
	{
		DT_TO_DBGSTR( LT_ERROR, "Pixelize() Failed!" );
	}
	ilCopyPixels(0, 0, 0, m_nWidth, m_nHeight, m_nDepth, m_PixelFormat, IL_UNSIGNED_BYTE, m_pData);

}


void		ImageOperator::Sharpen( float Factor, u32 Iter)
{
	ilBindImage( m_dwImgName );
	if ( !iluSharpen( Factor, Iter ) )
	{
		DT_TO_DBGSTR( LT_ERROR, "Sharpen() Failed!" );
	}
	ilCopyPixels(0, 0, 0, m_nWidth, m_nHeight, m_nDepth, m_PixelFormat, IL_UNSIGNED_BYTE, m_pData);
}


void		ImageOperator::SwapColours( void )
{
	ilBindImage( m_dwImgName );
	if ( !iluSwapColours() )
	{
		DT_TO_DBGSTR( LT_ERROR, "SwapColours() Failed!" );
	}
	ilCopyPixels(0, 0, 0, m_nWidth, m_nHeight, m_nDepth, m_PixelFormat, IL_UNSIGNED_BYTE, m_pData);

}


void		ImageOperator::ScaleColours( float r, float g, float b )
{
	ilBindImage( m_dwImgName );
	if ( !iluScaleColours( r, g, b ) )
	{
		DT_TO_DBGSTR( LT_ERROR, "ScaleColours() Failed!" );
	}
	ilCopyPixels(0, 0, 0, m_nWidth, m_nHeight, m_nDepth, m_PixelFormat, IL_UNSIGNED_BYTE, m_pData);

}


void		ImageOperator::Rotate(float Angle)
{
	ilBindImage( m_dwImgName );
	if ( !iluRotate( Angle ) )
	{
		DT_TO_DBGSTR( LT_ERROR, "Rotate() Failed!" );
	}
	ilCopyPixels(0, 0, 0, m_nWidth, m_nHeight, m_nDepth, m_PixelFormat, IL_UNSIGNED_BYTE, m_pData);

}


void		ImageOperator::SetScaleParm( ScaleParmName parm, ScaleParmValue value )
{
	if ( parm == eSPN_FILTER )
	{
		m_ScaleValueFilter = value;
	}
	else
	{
		m_ScaleValuePlacement = value;
	}
}


void		ImageOperator::ScaleSize( int nWidth, int nHeight, int nDepth/* = 1*/)
{
	ilBindImage( m_dwImgName );

    m_ScaleValueFilter = eSPV_SCALE_BELL;
	iluImageParameter( eSPN_FILTER, m_ScaleValueFilter );
	iluImageParameter( eSPN_PLACEMENT, m_ScaleValuePlacement );
	if ( !iluScale( nWidth, nHeight, nDepth ) )
	{
		DT_TO_DBGSTR( LT_ERROR, "ScaleSize() Failed!" );
	}

	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nDepth = nDepth;
    
	//ilCopyPixels(0, 0, 0, m_nWidth, m_nHeight, m_nDepth, m_PixelFormat, IL_UNSIGNED_BYTE, m_pData);
}

void ImageOperator::SetData(float* pData, u32 size)
{
    if (pData == NULL)
    {
        return;
    }
    SafeDeleteArray(m_pData);
    m_pData = new float[size];
    memcpy(m_pData, pData, size * sizeof(float));
    ilBindImage(m_dwImgName);

    if (!ilConvertImage(m_PixelFormat, IL_FLOAT))
    {
        return;
    }

    // 调整图像大小
    if( !iluEnlargeCanvas( m_nWidth, m_nHeight, m_nDepth ) )
    {
        return;
    }

    // 设置数据
    if( !ilSetData(m_pData) )
    {
        return;
    }
}

void		ImageOperator::SetData( u8* pData, u32 size )
{
	SafeDeleteArray( m_pData );
	m_pData = new u8[size];
	memcpy( m_pData, pData, size );

	ilBindImage( m_dwImgName );

	// 设置图像数据格式
	if( !ilConvertImage( m_PixelFormat, IL_UNSIGNED_BYTE ) )
	{
		return;
	}

	// 调整图像大小
	if( !iluEnlargeCanvas( m_nWidth, m_nHeight, m_nDepth ) )
	{
		return;
	}

	// 设置数据
	if( !ilSetData(m_pData) )
	{
		return;
	}
}


void		ImageOperator::SetPaletteData( u8* pData, u32 size )
{
	SafeDeleteArray( m_pPelette );
	m_pPelette = new u8[size];
	memcpy( m_pPelette, pData, size );
}

void		ImageOperator::SetPaletteType( PaletteType pt )
{
	m_PaletteType = pt;
}

void		ImageOperator::SetPaletteDataSize( u32 size )
{
	m_PalSize = size;
}

void		ImageOperator::SetNumNext( int numNext )
{
	m_nNumNext = numNext;
}


void		ImageOperator::SetImageWidth( int nWidth )
{
	m_nWidth = nWidth;
}

void		ImageOperator::SetImageHeight( int nHeight )
{
	m_nHeight = nHeight;
}

void		ImageOperator::SetImageDepth( int nDepth )
{
	m_nDepth = nDepth; 
}

void		ImageOperator::SetBytePerPixel( u8 byBPP )
{
	m_byBPP = byBPP;
}

void		ImageOperator::SetSizeOfData( u32 size )
{
	m_dwSizeOfData = size;
}

void		ImageOperator::SetPixelFormat( PixelFormatDevil pf )
{
	m_PixelFormat = pf;
}

void		ImageOperator::SetCodecType( CodecType codectype )
{
	m_CodecType = codectype;
}

void		ImageOperator::SetOriginType( OriginType origintype )
{
	m_OriginType = origintype;
}

void		ImageOperator::SetCubeFlag( CubeFlag cudeflag )
{
	m_CubeFlag = cudeflag;
}

void		ImageOperator::SetNumMips( int nMips )
{
	m_nNumMips = nMips;
}

void		ImageOperator::SetNumLayers( int nLayers )
{
	m_nNumLayers = nLayers;
}



// 获得像素数据指针
void*		ImageOperator::GetData( void )
{
	return m_pData;
}

void*		ImageOperator::GetPaletteData( void )
{
	return m_pPelette;
}

int			ImageOperator::GetImageWidth( void )
{
	return m_nWidth;
}

int			ImageOperator::GetImageHeight( void )
{
	return m_nHeight;
}

int			ImageOperator::GetImageDepth( void )
{
	return m_nDepth; 
}

u8		ImageOperator::GetBytePerPixel( void )
{
	return m_byBPP;
}

u32		ImageOperator::GetSizeOfData( void )
{
	return m_dwSizeOfData;
}

PixelFormatDevil	ImageOperator::GetPixelFormat( void )
{
	return m_PixelFormat;
}

CodecType	ImageOperator::GetCodecType( void )
{
	return m_CodecType;
}

OriginType	ImageOperator::GetOriginType( void )
{
	return m_OriginType;
}

CubeFlag	ImageOperator::GetCubeFlag( void )
{
	return m_CubeFlag;
}

int			ImageOperator::GetNumMips( void )
{
	return m_nNumMips;
}

int			ImageOperator::GetNumLayers( void )
{
	return m_nNumLayers;
}

int			ImageOperator::GetNumNexe( void )
{
	return m_nNumNext;
}

PaletteType ImageOperator::GetPaletteType( void )
{
	return m_PaletteType;
}

u32		ImageOperator::GetPaletteDataSize( void )
{
	return m_PalSize;
}

CColorValue ImageOperator::GetColourAt(int x, int y)
{
    if ( !m_pData )
    {
        DT_TO_DBGSTR( LT_ERROR, "no pixel data!" );
        return CColorValue::White;
    }
    if ( x < 0 || x >= m_nWidth || y < 0 || y >= m_nHeight)
    {
        DT_TO_DBGSTR( LT_ERROR, "pixel position out of bound!" );
        return CColorValue::White;
    }
    u32 id = (m_nWidth * y + x) * m_byBPP;
    u8* pData = (u8*)m_pData;
	float r = 0,g = 0,b = 0,a = 1;
    switch ( m_PixelFormat )
    {
	case ePF_COLOR_INDEX:
		{
			//像素对应的索引号
			int nPalID = pData[id];
			//索引颜色数据表中的id
			int nCurID = 0;
			switch ( m_PaletteType )
			{
			case ePAL_NONE:
				break;

				// RGB 24 位色, 3个字节全部用上
			case ePAL_RGB24:
				{
					nCurID = nPalID * 3;
					r = (float)m_pPelette[nCurID] / 255;
					g = (float)m_pPelette[nCurID + 1] / 255;
					b = (float)m_pPelette[nCurID + 2] / 255;
				}
				break;

				// RGB 32 位色, 前3个字节分别对应RGB, 第4个字节没用
			case ePAL_RGB32:
				{
					nCurID = nPalID * 4;
					r = (float)m_pPelette[nCurID] / 255;
					g = (float)m_pPelette[nCurID + 1] / 255;
					b = (float)m_pPelette[nCurID + 2] / 255;
				}
				break;

				// RGBA 32 位色, 4个字节分别对应RGBA
			case ePAL_RGBA32:
				{
					nCurID = nPalID * 4;
					r = (float)m_pPelette[nCurID] / 255;
					g = (float)m_pPelette[nCurID + 1] / 255;
					b = (float)m_pPelette[nCurID + 2] / 255;
					a = (float)m_pPelette[nCurID + 3] / 255;
				}
				break;

				// BGR 24 位色, 3个字节全部用上
			case ePAL_BGR24:
				{
					nCurID = nPalID * 3;
					b = (float)m_pPelette[nCurID]  / 255;
					g = (float)m_pPelette[nCurID + 1] / 255;
					r = (float)m_pPelette[nCurID + 2] / 255;
				}
				break;

				// BGR 32 位色, 前3个字节分别对应BGR, 第4个字节没用
			case ePAL_BGR32:
				{
					nCurID = nPalID * 4;
					b = (float)m_pPelette[nCurID] / 255;
					g = (float)m_pPelette[nCurID + 1] / 255;
					r = (float)m_pPelette[nCurID + 2] / 255;
				}
				break;

				// BGRA 32 位色, 4个字节分别对应BGRA
			case ePAL_BGRA32:
				{
					nCurID = nPalID * 4;
					b = (float)m_pPelette[nCurID] / 255;
					g = (float)m_pPelette[nCurID + 1] / 255;
					r = (float)m_pPelette[nCurID + 2] / 255;
					a = (float)m_pPelette[nCurID + 3] / 255;
				}
				break;
			}
			return CColorValue(a, r, g, b);
		}

	case ePF_ALPHA:
		{
			float a = float(pData[id]) / 255.0f;
			return CColorValue(a, 0, 0, 0);
		}

    case ePF_LUMINANCE:
        {
            float rgb = (float)pData[id] / 255;
            return CColorValue(1.0f, rgb, rgb, rgb);
        }

    case ePF_LUMINANCE_ALPHA:	
        {
            float a = (float)pData[id] / 255;
            float rgb = (float)pData[id + 1] / 255;
            return CColorValue(a, rgb, rgb, rgb);
        }

        // | R | G | B | ...| R | G | B |
        //   0   1   2   ...  k  k+1 k+2
    case ePF_RGB:				
        {
            float r = (float)pData[id] / 255;  // << 16;
            float g = (float)pData[id + 1] / 255;   // << 8;
            float b = (float)pData[id + 2] / 255;
            return CColorValue(1.0f, r, g, b);
        }

        // | R | G | B | A | ...| R | G | B | A |
        //   0   1   2   3   ...  k  k+1 k+2 k+3
    case ePF_RGBA:				
        {
            float r = (float)pData[id] / 255;  // << 24;
            float g = (float)pData[id + 1] / 255; // << 16;
            float b = (float)pData[id + 2] / 255; //<< 8;
            float a = (float)pData[id + 3] / 255;
            return CColorValue(a, r, g, b);
        }
        // add by liangiaran BGR格式
    case ePF_BGR:
        {
            float b = (float)pData[id] / 255; //<< 16;
            float g = (float)pData[id + 1] / 255; //<< 8;
            float r = (float)pData[id + 2] / 255;
            return CColorValue(1.0f, r, g, b);
        }

	case ePF_BGRA:
		{
			float b = (float)pData[id] / 255; //<< 16;
			float g = (float)pData[id + 1] / 255; //<< 8;
			float r = (float)pData[id + 2] / 255;
			float a = (float)pData[id + 3] / 255;
			return CColorValue(a, r, g, b);
		}

    default:
        return CColorValue(1.0f, 0, 0, 0);
    }

    return CColorValue::White;
}

bool ImageOperator::ConvertImage(PixelFormatDevil pfDest, CodecType ctDest)
{
    if (ilConvertImage(pfDest, ctDest))
    {
        m_PixelFormat = pfDest;
        m_CodecType = ctDest;
        return true;
    }

    return false;
}

HBITMAP ImageOperator::ConvertToHBITMAP(HDC hDC)
{
    return ilutConvertToHBitmap(hDC);
}


void ImageOperator::UsePalette( u8* pData, u32 size, PaletteType pt )
{
	SetPaletteData( pData, size );
	SetPaletteDataSize( size );
	SetPaletteType( pt );

	ilBindImage( m_dwImgName );

	ilRegisterPal( pData, size, pt );
	ilConvertPal( pt );
}

bool ImageOperator::SavePaletteFile( const char* szPalFile )
{
	ilBindImage( m_dwImgName );

	return ilSavePal( szPalFile );
}
//---------------------------------------------------------



ImageOperatorMgr::ImageOperatorMgr()
{
	Init();
}


ImageOperatorMgr::~ImageOperatorMgr()
{
	Release();
}

// 创建图像操作器并加载文件
ImageOperator* ImageOperatorMgr::CreateImageOperatorFromFile( const char* szName, const char* szFile, bool bFullPath /* = false */ )
{
	MAP_IMAGEOPERATOR_ITOR itor = m_mapImageOperator.find( szName );
	if ( itor != m_mapImageOperator.end() )
	{
		return (itor->second);
	}

	AIRString filepath = szFile;
	if ( !bFullPath )
	{
		filepath = FILESYSTEM->GetFileFullPath( szFile );
	}	
	if ( filepath.length() == 0 )
	{
		DT_TO_DBGSTR( LT_WARN, GetString("Get Image File [%s] Failed!",filepath.c_str()) );
		return NULL;
	}
	else
	{
		ImageOperator* pIO = new ImageOperator;
		if ( !pIO->LoadImageFromFile( filepath.c_str(), !bFullPath ) )
		{
			SafeDelete( pIO );
			return NULL;
		}
        pIO->SetName(szName); 
		m_mapImageOperator.insert( std::make_pair( szName, pIO ) );
		return pIO;
	}
}

// 创建一个动态的图像操作器
ImageOperator* ImageOperatorMgr::CreateDynamicImageOperator( const char* szName )
{
	MAP_IMAGEOPERATOR_ITOR itor = m_mapImageOperator.find( szName );
	if ( itor != m_mapImageOperator.end() )
	{
		return (itor->second);
	}
	ImageOperator* pIO = new ImageOperator;
	m_mapImageOperator.insert( std::make_pair( szName, pIO ) );
	return pIO;
}


// 释放图像操作器
bool	ImageOperatorMgr::ReleaseImageOperator( const char* szName )
{
	MAP_IMAGEOPERATOR_ITOR itor = m_mapImageOperator.find( szName );
	if ( itor != m_mapImageOperator.end() )
	{
		ImageOperator* pIO = (itor->second);
		SafeDelete( pIO );
		m_mapImageOperator.erase( itor );
		return true;
	}
	return false;
}

bool ImageOperatorMgr::ReleaseImageOperator(ImageOperator* pImageOperator)
{
    MAP_IMAGEOPERATOR_ITOR it = m_mapImageOperator.find(pImageOperator->GetName());
    if (it != m_mapImageOperator.end())
    {
        ImageOperator* pIO = it->second;
        SafeDelete(pIO);
        m_mapImageOperator.erase(it);
        return true;
    }
    return false;
}

// 获取图像操作器
ImageOperator* ImageOperatorMgr::GetImageOperator( const char* szName )
{
	MAP_IMAGEOPERATOR_ITOR itor = m_mapImageOperator.find( szName );
	if ( itor != m_mapImageOperator.end() )
	{
		return (itor->second);
	}
	return NULL;
}

ImageOperatorMgr* ImageOperatorMgr::GetSingletonPtr( void )
{
	static ImageOperatorMgr inst;
	return &inst;
}


// 初始化
void	ImageOperatorMgr::Init( void )
{
	ilInit();
	iluInit();
	m_mapImageOperator.clear();
	m_mapImgExtType.clear();

	m_mapImgExtType.insert( std::make_pair( "BMP",		eIET_BMP) );
	m_mapImgExtType.insert( std::make_pair( "CUT",		eIET_CUT) );
	m_mapImgExtType.insert( std::make_pair( "DOOM",		eIET_DOOM) );
	m_mapImgExtType.insert( std::make_pair( "DOOMFLAT", eIET_DOOM_FLAT) );
	m_mapImgExtType.insert( std::make_pair( "ICO",		eIET_ICO ) );
	m_mapImgExtType.insert( std::make_pair( "JPG",		eIET_JPG) );
	m_mapImgExtType.insert( std::make_pair( "JFIF",		eIET_JFIF) );
	m_mapImgExtType.insert( std::make_pair( "LBM",		eIET_LBM) );
	m_mapImgExtType.insert( std::make_pair( "PCD",		eIET_PCD) );
	m_mapImgExtType.insert( std::make_pair( "PCX",		eIET_PCX) );
	m_mapImgExtType.insert( std::make_pair( "PIC",		eIET_PIC) );
	m_mapImgExtType.insert( std::make_pair( "PNG",		eIET_PNG) );
	m_mapImgExtType.insert( std::make_pair( "PNM",		eIET_PNM) );
	m_mapImgExtType.insert( std::make_pair( "SGI",		eIET_SGI) );
	m_mapImgExtType.insert( std::make_pair( "TGA",		eIET_TGA) );
	m_mapImgExtType.insert( std::make_pair( "TIF",		eIET_TIF) );
	m_mapImgExtType.insert( std::make_pair( "CHEAD",	eIET_CHEAD ) );
	m_mapImgExtType.insert( std::make_pair( "RAW",		eIET_RAW) );
	m_mapImgExtType.insert( std::make_pair( "MDL",		eIET_MDL) );
	m_mapImgExtType.insert( std::make_pair( "WAL",		eIET_WAL) );
	m_mapImgExtType.insert( std::make_pair( "LIF",		eIET_LIF) );
	m_mapImgExtType.insert( std::make_pair( "MNG",		eIET_MNG) );
	m_mapImgExtType.insert( std::make_pair( "JNG",		eIET_JNG) );
	m_mapImgExtType.insert( std::make_pair( "GIF",		eIET_GIF) );
	m_mapImgExtType.insert( std::make_pair( "DDS",		eIET_DDS) );
	m_mapImgExtType.insert( std::make_pair( "DCX",		eIET_DCX) );
	m_mapImgExtType.insert( std::make_pair( "PSD",		eIET_PSD) );
	m_mapImgExtType.insert( std::make_pair( "EXIF",		eIET_EXIF) );
	m_mapImgExtType.insert( std::make_pair( "PSP",		eIET_PSP) );
	m_mapImgExtType.insert( std::make_pair( "PIX",		eIET_PIX) );
	m_mapImgExtType.insert( std::make_pair( "PXR",		eIET_PXR) );
	m_mapImgExtType.insert( std::make_pair( "XPM",		eIET_XPM) );
	m_mapImgExtType.insert( std::make_pair( "HDR",		eIET_HDR) );
	m_mapImgExtType.insert( std::make_pair( "ICNS",		eIET_ICNS) );
	m_mapImgExtType.insert( std::make_pair( "JP2",		eIET_JP2) );
	m_mapImgExtType.insert( std::make_pair( "EXR",		eIET_EXR) );
	m_mapImgExtType.insert( std::make_pair( "WDP",		eIET_WDP) );
	m_mapImgExtType.insert( std::make_pair( "VTF",		eIET_VTF) );
	m_mapImgExtType.insert( std::make_pair( "WBMP",		eIET_WBMP) );
	m_mapImgExtType.insert( std::make_pair( "SUN",		eIET_SUN) );
	m_mapImgExtType.insert( std::make_pair( "IFF",		eIET_IFF) );
	m_mapImgExtType.insert( std::make_pair( "TPL",		eIET_TPL) );
	m_mapImgExtType.insert( std::make_pair( "JPEG",		eIET_JPG) );
}

// 释放
void	ImageOperatorMgr::Release( void )
{
	ilShutDown();
	ImageOperator* pIO = NULL;
	MAP_IMAGEOPERATOR_ITOR itor = m_mapImageOperator.begin(); 
	for ( ; itor != m_mapImageOperator.end(); ++itor )
	{
		pIO = (itor->second);
		SafeDelete( pIO );
	}
	m_mapImageOperator.clear();
}


ImgExtType ImageOperatorMgr::GetImgExtType(const AIRString& szExt )
{
    std::map<AIRString, ImgExtType>::iterator itor = m_mapImgExtType.find( szExt );
	if ( itor != m_mapImgExtType.end() )
	{
		return itor->second;
	}
	return eIET_UNKNOWN;
}

bool ImageOperatorMgr::JudgeImageExt(const char* szExt)
{
    AIRString strExt = szExt;
    std::transform(strExt.begin(), strExt.end(), strExt.begin(), tolower);

    if (strExt == "bmp" || strExt == "jpeg" || strExt == "jpg" || strExt == "tga" || strExt == "png" || strExt == "dds")
    {
        return true;
    }
    return false;
}

#endif
#endif
#endif

