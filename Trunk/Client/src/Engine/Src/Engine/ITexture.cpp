#include "stdafx.h"
#include "ITexture.h"
#include "Engine.h"
#include "MemoryDataStream.h"
#include "StringUtil.h"
#include "AIRImage.h"
#include "FileSystem.h"
#include "TimeBaseProfile.h"

IMPLEMENT_RTTI(ITexture, Resource)

void FilePathAnalyze( const char* szFullPath, AIRString& folderPath, AIRString& fileNameWithoutExt, AIRString& fileExt )
{
	fileExt = strrchr(szFullPath, '.');
	AIRString fullpath(szFullPath);
	AIRString filename = CStringUtil::GetFilename(szFullPath);
	fileNameWithoutExt = filename.substr(0, filename.size() - fileExt.size());
	folderPath = fullpath.substr(0, fullpath.size() - filename.size() - 1);
}


TextureTrunk::TextureTrunk():mPixelDataSize(0), mPixels(NULL)
{

}

TextureTrunk::~TextureTrunk()
{
	if( mPixels != NULL )
	{
		AIR_DELETE_ARRAY_T(mPixels, u8, mPixelDataSize);
		mPixels = NULL;
		mPixelDataSize = 0;
	}
}

bool TextureTrunk::ReadFromStream( CDataStream* pStream )
{
	if (pStream)
	{
		CMemoryDataStream dataStream(*pStream);

		if( mPixels != NULL )
		{
			AIR_DELETE_ARRAY_T(mPixels, u8, mPixelDataSize);
			mPixels = NULL;
			mPixelDataSize = 0;
		}

		dataStream.Read( &mTexParam, sizeof(mTexParam) );
		dataStream.Read( &mPixelDataSize, sizeof(mPixelDataSize) );
		mPixels = AIR_NEW_ARRAY_T( u8, mPixelDataSize );
		dataStream.Read( mPixels, mPixelDataSize );
		return true;
	}
	return false;
}

bool TextureTrunk::SaveToFile( const char* szPath )
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	std::locale loc1 = std::locale::global(std::locale(".936"));
#endif

	std::ofstream fout;
	fout.open( szPath, std::ios::out | std::ios::binary);
	fout.flush();

	fout.write( (char*)(&mTexParam), sizeof(mTexParam) );
	fout.write( (char*)(&mPixelDataSize), sizeof(mPixelDataSize) );
	fout.write( (char*)mPixels, mPixelDataSize );

	fout.flush();
	fout.close();
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
	std::locale::global(std::locale(loc1));
#endif

	return true;
}

bool TextureTrunk::ConvertImageToTextureTrunk(const char* szSrcImageFullPath, bool coverIfExist)
{
	PROFILE_FUNC(TextureTrunk_ConvertImageToTextureTrunk);

	AIRString folderPath, fileNameNoExt, fileExt, srcFileName;
	FilePathAnalyze(szSrcImageFullPath, folderPath, fileNameNoExt, fileExt);
	srcFileName = fileNameNoExt + fileExt;

	if( fileExt == ".png" || fileExt == ".tga" || fileExt == ".dds" || fileExt == ".jpg" || fileExt == ".bmp" )
	{
		AIRString dstTexFileName = fileNameNoExt + ".texture";
		AIRString dstTexFullPath = folderPath + "/" + dstTexFileName;
		if( FILESYSTEM->IsFileExist( folderPath.c_str(), dstTexFileName.c_str() ) && coverIfExist)
		{
			int ret = remove( dstTexFullPath.c_str() );
			if( ret == -1 )
			{
				PrintDebugString( "Del file %s failed!\n", dstTexFullPath.c_str() );
			}
		}

		Image2D image(szSrcImageFullPath);
		if(image.IsValid())
		{
			mTexParam.nBitsPerPixel = image.GetBitsPerPixel();
			mTexParam.nWidth = image.GetWidth();
			mTexParam.nHeight = image.GetHeight();
			mTexParam.nDepth = 1;
			mTexParam.nMipmaps = 0;
			mTexParam.type = TT_TEXTURE;
			switch (mTexParam.nBitsPerPixel)
			{
			case 8:
				mTexParam.pixelFormat = RPF_L8;
				break;
			case 16:
				mTexParam.pixelFormat = RPF_L16;
				break;
			case 24:
				mTexParam.pixelFormat = RPF_R8G8B8;
				break;
			case 32:
				mTexParam.pixelFormat = RPF_A8R8G8B8;
				break;
			}
			mPixelDataSize = image.GetImageSize();
			mPixels = image.GetBuffer();

			if( !SaveToFile( dstTexFullPath.c_str() ) )
			{
				mPixels = NULL;
				PrintDebugString( "save texture %s failed!\n", dstTexFullPath.c_str() );
				return false;
			}

			mPixels = NULL;
			PrintDebugString( "Convert [%s] to [%s] success.\n", szSrcImageFullPath, dstTexFullPath.c_str() );
			return true;
		}
		else
			return false;

		//CDataStream* pFileStream = FILESYSTEM->GetFileStream( folderPath.c_str(), srcFileName.c_str() );
		//if( pFileStream )
		//{
		//	AIRImage image;
		//	if (!image.LoadFromFileStream(pFileStream))
		//	{
		//		PrintDebugString( "load image %s failed!\n", szSrcImageFullPath );

		//		delete pFileStream;
		//		return false;
		//	}

		//	FREE_IMAGE_TYPE freeImageType = image.GetFreeImageType();

		//	u32 bpp = image.GetBytePerPixel();
		//	mTexParam.nBitsPerPixel = bpp;
		//	switch (bpp)
		//	{
		//	case 8:
		//		mTexParam.pixelFormat = RPF_L8;
		//		break;
		//	case 16:
		//		mTexParam.pixelFormat = RPF_L16;
		//		break;
		//	case 24:
		//		mTexParam.pixelFormat = RPF_R8G8B8;
		//		break;
		//	case 32:
		//		mTexParam.pixelFormat = RPF_A8R8G8B8;
		//		break;
		//	}

		//	mTexParam.nWidth = image.GetWidth();
		//	mTexParam.nHeight = image.GetHeight();
		//	mTexParam.nDepth = 1;
		//	mTexParam.type = TT_TEXTURE;
		//	mPixelDataSize = bpp * image.GetWidth() * image.GetHeight();
		//	mPixels = image.GetPixelBits();

		//	if( !SaveToFile( dstTexFullPath.c_str() ) )
		//	{
		//		PrintDebugString( "save texture %s failed!\n", dstTexFullPath.c_str() );
		//	}

		//	delete pFileStream;
		//	return true;
		//}
	}
	return true;
}


bool ITexture::CreateFromFile(const char* szFilename)
{
#if AIR_TARGET_PLATFORM != AIR_PLATFORM_WIN32
	AIRString folderPath, fileNameNoExt, fileExt, srcFileName;
	FilePathAnalyze(szFilename, folderPath, fileNameNoExt, fileExt);
	AIRString dstFileName = fileNameNoExt + ".texture";

	AIRString dstfullpath = FILESYSTEM->GetFileFullPath(dstFileName.c_str());

	if( FILESYSTEM->IsFileExist(dstfullpath.c_str()))
	{
		SetFilename( dstFileName.c_str() );
		m_strName = dstFileName;
		m_LoadFromImage = false;
	}
	else
	{
		SetFilename(szFilename);
		m_strName = szFilename;
		m_LoadFromImage = true;
	}
#else
	SetFilename(szFilename);
	m_strName = szFilename;
	m_LoadFromImage = true;
#endif
    return LoadFromFile(m_strName.c_str(), ENGINE_INST->GetConfig()->m_bBackgroundThread);
}
