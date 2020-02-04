//#include "stdafx.h"
//#include "CSoundSystem.h"
//#include "DebugTools.h"
//#include "Engine.h"
//#include "FileSystem.h"
//#include "MemoryDataStream.h"
//
//
//
//#define USE_BASS24
//
//
//
//
//#ifdef USE_BASS24
//#	include "bass24.h"
//#	pragma comment( lib, "bass.lib" )
//#	define MAKEMUSICPOS(order,row) (0x80000000|MAKELONG(order,row))
//#else
//#	define BASSDEF(f) (WINAPI *f)
//#	include "bass23.h"
//#	define LOADBASSFUNCTION(f) *((void**)&f)=(void*)GetProcAddress(m_hBass,#f)
//#endif
//
//
//
//
//bool CSoundSystem::LoadSound( const char* szName, const char* szFile )
//{
//	if ( !szName || !szFile )
//	{
//		return false;
//	}
//
//	HCHANNEL hChanel = GetSoundHandle( szName );
//	if ( hChanel )
//	{
//		return true;
//	}
//	hChanel = Effect_Load( szFile );
//	if ( hChanel )
//	{
//		m_mapHChanel.insert( std::make_pair( szName, hChanel ) );
//		return true;
//	}
//	return false;
//}
//
//
//void CSoundSystem::FreeSound( const char* szName )
//{
//	if ( m_mapHChanel.empty() )
//	{
//		return;
//	}
//	MAP_HCHANNEL_ITOR it = m_mapHChanel.find( szName );
//	if ( it != m_mapHChanel.end() )
//	{
//		Effect_Free( (HEFFECT)(it->second) );
//		m_mapHChanel.erase( it );
//	}
//}
//
//
//void CSoundSystem::PlaySound(const char* szName, bool loop /* = false  */)
//{
//	HCHANNEL hChanel = GetSoundHandle( szName );
//	if ( hChanel )
//	{
//		u32 volumn = BASS_GetVolume();
//		Effect_PlayEx( (HEFFECT)hChanel, volumn, 0, 1.0, loop );
//	}
//}
//
//HCHANNEL CSoundSystem::GetSoundHandle( const char* szName )
//{
//	if ( m_mapHChanel.empty() )
//	{
//		return NULL;
//	}
//	MAP_HCHANNEL_ITOR it = m_mapHChanel.find( szName );
//	if ( it != m_mapHChanel.end() )
//	{
//		return it->second;
//	}
//	return NULL;
//}
//
//
//HEFFECT  CSoundSystem::Effect_Load(const char *filename, u32 size)
//{
//	u32				_size, length, samples;
//	HSAMPLE				hs = NULL;
//	HSTREAM				hstrm = NULL;
//	BASS_CHANNELINFO	info;
//	void				*buffer = NULL, *data = NULL;
//	
//	// 如果bass handle不为空
//	if( m_hBass )
//	{
//		// 如果是静音模式,返回1
//		if( m_bSilent ) 
//		{
//			return 1;
//		}
//
//		// 通过文件系统读取音频文件数据流
//		CDataStream* pStream = FILESYSTEM->GetFileStream( filename );
//		if ( !pStream )
//		{
//			DT_TO_MSGBOX_FMT( LT_ERROR, GetString("加载音频文件 [%s] 失败!",filename) );
//			return NULL;
//		}
//		// 加载文件数据到内存,自动释放数据
//		CMemoryDataStream	MemDataStream(*pStream, true);
//		// 释放文件流对象
//		SafeDelete( pStream );
//
//		// 如果获取文件数据失败
//		if ( !(data = MemDataStream.GetPtr()) )
//		{
//			DT_TO_MSGBOX_FMT( LT_ERROR, GetString("加载音频文件 [%s] 失败!",filename) );
//			return NULL;
//		}
//		_size = MemDataStream.Size();
//
//		// 从内存数据加载音频样本
//		hs=BASS_SampleLoad(TRUE, data, 0, _size, 4, BASS_SAMPLE_OVER_VOL);
//		// 如果加载音频样本失败
//		if(!hs) 
//		{
//			// 尝试从内存数据创建音频流
//			hstrm=BASS_StreamCreateFile(TRUE, data, 0, _size, BASS_STREAM_DECODE);
//			// 如果创建音频流成功
//			if(hstrm) 
//			{
//				// 获得通道的长度
//#ifdef USE_BASS24
//				length=(u32)BASS_ChannelGetLength(hstrm,BASS_POS_BYTE);
//#else
//				length=(u32)BASS_ChannelGetLength(hstrm);
//#endif
//				// 获得通道的信息
//				BASS_ChannelGetInfo(hstrm, &info);
//				samples=length;
//				if(info.chans < 2) 
//				{
//					samples>>=1;
//				}
//				if((info.flags & BASS_SAMPLE_8BITS) == 0) 
//				{
//					samples>>=1;
//				}
//				// 创建样本缓冲
//#ifdef USE_BASS24
//				hs=BASS_SampleCreate( length, info.freq, info.chans, 4, info.flags | BASS_SAMPLE_OVER_VOL );
//				if ( !hs )
//				{
//					DT_TO_MSGBOX_FMT( LT_ERROR, "Can't create sound effect: Not enough memory" );
//				}
//				// 释放音频流对象
//				BASS_StreamFree(hstrm);
//#else
//				buffer=BASS_SampleCreate(samples, info.freq, 2, 4, info.flags | BASS_SAMPLE_OVER_VOL);
//				// 如果创建样本缓冲失败
//				if(!buffer)
//				{
//					// 释放音频流对象
//					BASS_StreamFree(hstrm);
//					DT_TO_MSGBOX_FMT( LT_ERROR, "Can't create sound effect: Not enough memory" );
//				}
//				// 创建样本缓冲成功
//				else
//				{
//					// 获得通道的数据信息
//					BASS_ChannelGetData(hstrm, buffer, length);
//					// 创建音频样本
//					hs=BASS_SampleCreateDone();
//					// 释放音频流
//					BASS_StreamFree(hstrm);
//					if(!hs)	
//					{
//						DT_TO_MSGBOX_FMT( LT_ERROR, "Can't create sound effect" );
//					}
//				}
//#endif
//			}
//		}
//
//		return hs;
//	}
//	else 
//	{
//		return NULL;
//	}
//}
//
//HCHANNEL  CSoundSystem::Effect_Play(HEFFECT eff)
//{
//	if(m_hBass)
//	{
//		HCHANNEL chn;
//		chn=BASS_SampleGetChannel(eff, FALSE);
//		BASS_ChannelPlay(chn, TRUE);
//		return chn;
//	}
//	else 
//	{
//		return 0;
//	}
//}
//
//HCHANNEL  CSoundSystem::Effect_PlayEx(HEFFECT eff, int volume, int pan, float pitch, bool loop)
//{
//	if(m_hBass)
//	{
//		BASS_SAMPLE info;
//		HCHANNEL chn;
//		BASS_SampleGetInfo(eff, &info);
//
//		chn=BASS_SampleGetChannel(eff, FALSE);
//
//#ifdef USE_BASS24
//		BASS_ChannelSetAttribute( chn, BASS_ATTRIB_FREQ, (int)(pitch*info.freq) );
//		BASS_ChannelSetAttribute( chn, BASS_ATTRIB_VOL, volume );
//		BASS_ChannelSetAttribute( chn, BASS_ATTRIB_PAN, pan );
//#else
//		BASS_ChannelSetAttributes(chn, (int)(pitch*info.freq), volume, pan);
//#endif
//
//		info.flags &= ~BASS_SAMPLE_LOOP;
//		if(loop) 
//		{
//			info.flags |= BASS_SAMPLE_LOOP;
//		}
//
//#ifdef USE_BASS24
//		BASS_ChannelFlags( chn, info.flags, info.flags );
//#else
//		BASS_ChannelSetFlags(chn, info.flags);
//#endif
//
//		BASS_ChannelPlay(chn, TRUE);
//		return chn;
//	}
//	else 
//	{
//		return 0;
//	}
//}
//
//
//void  CSoundSystem::Effect_Free(HEFFECT eff)
//{
//	if(m_hBass) 
//	{
//		BASS_SampleFree(eff);
//	}
//}
//
//
//HMUSIC  CSoundSystem::Music_Load(const char *filename, u32 size)
//{
//	void	*data = NULL;
//	u32	_size;
//	HMUSIC	hm = NULL;
//	bool	bDataFromInside = true;
//
//	if(m_hBass)
//	{
//		CDataStream* pStream = FILESYSTEM->GetFileStream( filename );
//		if ( !pStream )
//		{
//			DT_TO_MSGBOX_FMT( LT_ERROR, GetString("加载音频文件 [%s] 失败!",filename) );
//			return NULL;
//		}
//		// 加载文件数据到内存,自动释放数据
//		CMemoryDataStream	MemDataStream(*pStream, true);
//		// 释放文件流对象
//		SafeDelete( pStream );
//
//		// 如果获取文件数据失败
//		if ( !(data = MemDataStream.GetPtr()) )
//		{
//			DT_TO_MSGBOX_FMT( LT_ERROR, GetString("加载音频文件 [%s] 失败!",filename) );
//			return NULL;
//		}
//		_size = MemDataStream.Size();
//
//		// 加载音频对象
//		hm=BASS_MusicLoad(TRUE, data, 0, 0, BASS_MUSIC_PRESCAN | BASS_MUSIC_POSRESETEX | BASS_MUSIC_RAMP, 0);
//		if(!hm)	
//		{
//			DT_TO_MSGBOX_FMT( LT_ERROR, "Can't load music!" );
//		}
//		return hm;
//	}
//	else 
//	{
//		return 0;
//	}
//}
//
//HCHANNEL  CSoundSystem::Music_Play(HMUSIC mus, bool loop, int volume, int order, int row)
//{
//	if(m_hBass)
//	{
//#ifdef USE_BASS24
//		u32 pos = BASS_ChannelGetPosition(mus,BASS_POS_MUSIC_ORDER);
//#else
//		u32 pos = BASS_MusicGetOrderPosition(mus);
//#endif
//		if(order == -1) order = LOWORD(pos);
//		if(row == -1) row = HIWORD(pos);
//
//#ifdef USE_BASS24
//		BASS_ChannelSetPosition(mus,MAKEMUSICPOS(order, row),BASS_POS_MUSIC_ORDER | BASS_MUSIC_POSRESET);
//#else
//		BASS_ChannelSetPosition(mus, MAKEMUSICPOS(order, row));
//#endif
//
//		BASS_CHANNELINFO info;
//		BASS_ChannelGetInfo(mus, &info);
//
//#ifdef USE_BASS24
//		BASS_ChannelSetAttribute(mus, BASS_ATTRIB_FREQ, info.freq);
//		BASS_ChannelSetAttribute(mus, BASS_ATTRIB_VOL, volume);
//		BASS_ChannelSetAttribute(mus, BASS_ATTRIB_PAN, 0);
//#else
//		BASS_ChannelSetAttributes(mus, info.freq, volume, 0);
//#endif
//
//		info.flags &= ~BASS_SAMPLE_LOOP;
//		if(loop) 
//		{
//			info.flags |= BASS_SAMPLE_LOOP;
//		}
//
//#ifdef USE_BASS24
//		BASS_ChannelFlags(mus, info.flags, info.flags);
//#else
//		BASS_ChannelSetFlags(mus, info.flags);
//#endif
//
//		BASS_ChannelPlay(mus, FALSE);
//
//		return mus;
//	}
//	else
//	{
//		return NULL;
//	}
//}
//
//void  CSoundSystem::Music_Free(HMUSIC mus)
//{
//	if(m_hBass) 
//	{
//		BASS_MusicFree(mus);
//	}
//}
//
//void  CSoundSystem::Music_SetAmplification(HMUSIC music, int ampl)
//{
//	if(m_hBass) 
//	{
//#ifdef USE_BASS24
//		BASS_ChannelSetAttribute(music, BASS_ATTRIB_MUSIC_AMPLIFY, ampl);
//#else
//		BASS_MusicSetAttribute(music, BASS_MUSIC_ATTRIB_AMPLIFY, ampl);
//#endif
//	}
//}
//
//int  CSoundSystem::Music_GetAmplification(HMUSIC music)
//{
//	if(m_hBass) 
//	{
//#ifdef USE_BASS24
//		float ampl = -1.0f;
//		BASS_ChannelGetAttribute(music, BASS_ATTRIB_MUSIC_AMPLIFY, &ampl);
//		return int(ampl);
//#else
//		return BASS_MusicGetAttribute(music, BASS_MUSIC_ATTRIB_AMPLIFY);
//#endif
//	}
//	else
//	{
//		return -1;
//	}
//}
//
//int  CSoundSystem::Music_GetLength(HMUSIC music)
//{
//	if(m_hBass)
//	{
//#ifdef USE_BASS24
//		return (int)BASS_ChannelGetLength(music, BASS_POS_MUSIC_ORDER);
//#else
//		return BASS_MusicGetOrders(music);
//#endif
//	}
//	else 
//	{
//		return -1;
//	}
//}
//
//void  CSoundSystem::Music_SetPos(HMUSIC music, int order, int row)
//{
//	if(m_hBass)
//	{
//#ifdef USE_BASS24
//		BASS_ChannelSetPosition(music, MAKEMUSICPOS(order, row), BASS_POS_MUSIC_ORDER | BASS_MUSIC_POSRESET);
//#else
//		BASS_ChannelSetPosition(music, MAKEMUSICPOS(order, row));
//#endif
//	}
//}
//
//bool  CSoundSystem::Music_GetPos(HMUSIC music, int *order, int *row)
//{
//	if(m_hBass)
//	{
//		u32 pos;
//#ifdef USE_BASS24
//		pos = BASS_ChannelGetPosition(music, BASS_POS_MUSIC_ORDER);
//#else
//		pos = BASS_MusicGetOrderPosition(music);
//#endif
//		if(pos == -1) 
//		{
//			return false;
//		}
//		*order = LOWORD(pos);
//		*row = HIWORD(pos);
//		return true;
//	}
//	else 
//	{
//		return false;
//	}
//}
//
//void  CSoundSystem::Music_SetInstrVolume(HMUSIC music, int instr, int volume)
//{
//	if(m_hBass)
//	{
//#ifdef USE_BASS24
//		BASS_ChannelSetAttribute(music, BASS_ATTRIB_MUSIC_VOL_INST + instr, volume);
//#else
//		BASS_MusicSetAttribute(music, BASS_MUSIC_ATTRIB_VOL_INST + instr, volume);	
//#endif
//	}
//}
//
//int  CSoundSystem::Music_GetInstrVolume(HMUSIC music, int instr)
//{
//	if(m_hBass)
//	{
//#ifdef USE_BASS24
//		float value = 0.0f;
//		BASS_ChannelGetAttribute(music, BASS_ATTRIB_MUSIC_VOL_INST + instr, &value);
//		return int(value);
//#else
//		return BASS_MusicGetAttribute(music, BASS_MUSIC_ATTRIB_VOL_INST + instr);		
//#endif
//	}
//	else 
//	{
//		return -1;
//	}
//}
//
//void  CSoundSystem::Music_SetChannelVolume(HMUSIC music, int channel, int volume)
//{
//	if(m_hBass)
//	{
//#ifdef USE_BASS24
//		BASS_ChannelSetAttribute(music, BASS_ATTRIB_MUSIC_VOL_CHAN + channel, volume);
//#else
//		BASS_MusicSetAttribute(music, BASS_MUSIC_ATTRIB_VOL_CHAN + channel, volume);	
//#endif
//	}
//}
//
//int  CSoundSystem::Music_GetChannelVolume(HMUSIC music, int channel)
//{
//	if(m_hBass)
//	{
//#ifdef USE_BASS24
//		float volumn = 0.0f;
//		BASS_ChannelGetAttribute(music, BASS_ATTRIB_MUSIC_VOL_CHAN + channel, &volumn);
//		return int(volumn);
//#else
//		return BASS_MusicGetAttribute(music, BASS_MUSIC_ATTRIB_VOL_CHAN + channel);		
//#endif
//	}
//	else 
//	{
//		return -1;
//	}
//}
//
//HSTREAM  CSoundSystem::Stream_Load(const char *filename, u32 size)
//{
//	void		*data = NULL;
//	u32		_size;
//	HSTREAM		hs = NULL;
//
//	if(m_hBass)
//	{
//		if(m_bSilent) 
//		{
//			return 1;
//		}
//
//		CDataStream* pStream = FILESYSTEM->GetFileStream( filename );
//		if ( !pStream )
//		{
//			DT_TO_MSGBOX_FMT( LT_ERROR, GetString("加载音频文件 [%s] 失败!",filename) );
//			return NULL;
//		}
//		// 加载文件数据到内存,自动释放数据
//		CMemoryDataStream	MemDataStream(*pStream, true);
//		// 释放文件流对象
//		SafeDelete( pStream );
//
//		// 如果获取文件数据失败
//		if ( !(data = MemDataStream.GetPtr()) )
//		{
//			DT_TO_MSGBOX_FMT( LT_ERROR, GetString("加载音频文件 [%s] 失败!",filename) );
//			return NULL;
//		}
//		_size = MemDataStream.Size();
//
//		hs=BASS_StreamCreateFile(TRUE, data, 0, _size, 0);
//		if(!hs)
//		{
//			DT_TO_MSGBOX_FMT( LT_ERROR, "Can't load stream" );
//			return NULL;
//		}
//		return hs;
//	}
//	else 
//	{
//		return NULL;
//	}
//}
//
//void  CSoundSystem::Stream_Free(HSTREAM stream)
//{
//	if ( stream )
//	{
//		BASS_StreamFree(stream);
//	}
//}
//
//HCHANNEL  CSoundSystem::Stream_Play(HSTREAM stream, bool loop, int volume)
//{
//	if(m_hBass)
//	{
//		BASS_CHANNELINFO info;
//		BASS_ChannelGetInfo(stream, &info);
//
//#ifdef USE_BASS24
//		BASS_ChannelSetAttribute(stream, BASS_ATTRIB_FREQ, info.freq);
//		BASS_ChannelSetAttribute(stream, BASS_ATTRIB_VOL, volume);
//		BASS_ChannelSetAttribute(stream, BASS_ATTRIB_PAN, 0);
//#else
//		BASS_ChannelSetAttributes(stream, info.freq, volume, 0);
//#endif
//
//		info.flags &= ~BASS_SAMPLE_LOOP;
//		if(loop) 
//		{
//			info.flags |= BASS_SAMPLE_LOOP;
//		}
//
//#ifdef USE_BASS24
//		BASS_ChannelFlags(stream, info.flags, info.flags);
//#else
//		BASS_ChannelSetFlags(stream, info.flags);
//#endif
//
//		BASS_ChannelPlay(stream, TRUE);
//		return stream;
//	}
//	else 
//	{
//		return NULL;
//	}
//}
//
//void  CSoundSystem::Channel_SetPanning(HCHANNEL chn, int pan)
//{
//	if(m_hBass) 
//	{
//#ifdef USE_BASS24
//		BASS_ChannelSetAttribute(chn, BASS_ATTRIB_FREQ, -1);
//		BASS_ChannelSetAttribute(chn, BASS_ATTRIB_VOL, -1);
//		BASS_ChannelSetAttribute(chn, BASS_ATTRIB_PAN, pan);
//#else
//		BASS_ChannelSetAttributes(chn, -1, -1, pan);
//#endif
//	}
//}
//
//void  CSoundSystem::Channel_SetVolume(HCHANNEL chn, int volume)
//{
//	if(m_hBass) 
//	{
//#ifdef USE_BASS24
//		BASS_ChannelSetAttribute(chn, BASS_ATTRIB_FREQ, -1);
//		BASS_ChannelSetAttribute(chn, BASS_ATTRIB_VOL, volume);
//		BASS_ChannelSetAttribute(chn, BASS_ATTRIB_PAN, -101);
//#else
//		BASS_ChannelSetAttributes(chn, -1, volume, -101);
//#endif
//		
//	}
//}
//
//void  CSoundSystem::Channel_SetPitch(HCHANNEL chn, float pitch)
//{
//	if(m_hBass)
//	{
//		BASS_CHANNELINFO info;
//		BASS_ChannelGetInfo(chn, &info);
//#ifdef USE_BASS24
//		BASS_ChannelSetAttribute(chn, BASS_ATTRIB_FREQ, (int)(pitch*info.freq));
//		BASS_ChannelSetAttribute(chn, BASS_ATTRIB_VOL, -1);
//		BASS_ChannelSetAttribute(chn, BASS_ATTRIB_PAN, -101);
//#else
//		BASS_ChannelSetAttributes(chn, (int)(pitch*info.freq), -1, -101);
//#endif
//		
//	}
//}
//
//void  CSoundSystem::Channel_Pause(HCHANNEL chn)
//{
//	if(m_hBass) 
//	{
//		BASS_ChannelPause(chn);
//	}
//}
//
//void  CSoundSystem::Channel_Resume(HCHANNEL chn)
//{
//	if(m_hBass) 
//	{
//		BASS_ChannelPlay(chn, FALSE);
//	}
//}
//
//void  CSoundSystem::Channel_Stop(HCHANNEL chn)
//{
//	if(m_hBass) 
//	{
//		BASS_ChannelStop(chn);
//	}
//}
//
//void  CSoundSystem::Channel_PauseAll()
//{
//	if(m_hBass) 
//	{
//		BASS_Pause();
//	}
//}
//
//void  CSoundSystem::Channel_ResumeAll()
//{
//	if(m_hBass) 
//	{
//		BASS_Start();
//	}
//}
//
//void  CSoundSystem::Channel_StopAll()
//{
//	if(m_hBass)
//	{
//		BASS_Stop();
//		BASS_Start();
//	}
//}
//
//bool  CSoundSystem::Channel_IsPlaying(HCHANNEL chn)
//{
//	if(m_hBass)
//	{
//		if(BASS_ChannelIsActive(chn)==BASS_ACTIVE_PLAYING) 
//		{
//			return true;
//		}
//		else
//		{
//			return false;
//		}
//	}
//	else 
//	{
//		return false;
//	}
//}
//
//float  CSoundSystem::Channel_GetLength(HCHANNEL chn) 
//{
//	if(m_hBass)
//	{
//#ifdef USE_BASS24
//		u32 length = BASS_ChannelGetLength(chn, BASS_POS_BYTE);
//		return BASS_ChannelBytes2Seconds(chn, length);
//#else
//		return BASS_ChannelBytes2Seconds(chn, BASS_ChannelGetLength(chn));
//#endif
//	}
//	else 
//	{
//		return -1;
//	}
//}
//
//float  CSoundSystem::Channel_GetPos(HCHANNEL chn) 
//{
//	if(m_hBass)
//	{
//#ifdef USE_BASS24
//		u32 pos = BASS_ChannelGetPosition(chn, BASS_POS_BYTE);
//		return BASS_ChannelBytes2Seconds(chn, pos);
//#else
//		return BASS_ChannelBytes2Seconds(chn, BASS_ChannelGetPosition(chn));
//#endif
//		
//	}
//	else 
//	{
//		return -1;
//	}
//}
//
//void  CSoundSystem::Channel_SetPos(HCHANNEL chn, float fSeconds) 
//{
//	if(m_hBass)
//	{
//#ifdef USE_BASS24
//		BASS_ChannelSetPosition(chn, BASS_ChannelSeconds2Bytes(chn, fSeconds), BASS_POS_BYTE);
//#else
//		BASS_ChannelSetPosition(chn, BASS_ChannelSeconds2Bytes(chn, fSeconds));
//#endif
//	}
//}
//
//void  CSoundSystem::Channel_SlideTo(HCHANNEL channel, float time, int volume, int pan, float pitch)
//{
//	if(m_hBass)
//	{
//		BASS_CHANNELINFO info;
//		BASS_ChannelGetInfo(channel, &info);
//
//		int freq;
//		if(pitch == -1) 
//		{
//			freq = -1;
//		}
//		else 
//		{
//			freq = (int)(pitch*info.freq);
//		}
//
//#ifdef USE_BASS24
//		BASS_ChannelSlideAttribute(channel, BASS_ATTRIB_FREQ, freq, u32(time*1000));
//		BASS_ChannelSlideAttribute(channel, BASS_ATTRIB_VOL, volume, u32(time*1000));
//		BASS_ChannelSlideAttribute(channel, BASS_ATTRIB_PAN, pan, u32(time*1000));
//#else
//		BASS_ChannelSlideAttributes(channel, freq, volume, pan, u32(time*1000));
//#endif
//	}
//}
//
//bool  CSoundSystem::Channel_IsSliding(HCHANNEL channel)
//{
//	if(m_hBass)
//	{
//#ifdef USE_BASS24
//		if ( BASS_ChannelIsSliding(channel,BASS_ATTRIB_FREQ) ||
//			 BASS_ChannelIsSliding(channel,BASS_ATTRIB_VOL) ||
//			 BASS_ChannelIsSliding(channel,BASS_ATTRIB_PAN))
//		{
//			return true;
//		}
//		return false;
//#else
//		if(BASS_ChannelIsSliding(channel)) 
//		{
//			return true;
//		}
//		return false;
//#endif
//	}
//	else 
//	{
//		return false;
//	}
//}
//
//
////////// Implementation ////////
//
//
//bool CSoundSystem::_SoundInit()
//{
//	if( m_hBass )
//	{
//		return true;
//	}
//
//	m_hBass = HINSTANCE(0xffffffff);
//
//
//#ifndef USE_BASS24
//	m_hBass = LoadLibrary("bass.dll");
//	if ( !m_hBass )
//	{
//		DT_TO_LOG( LT_ERROR, "加载 BASS.DLL 失败!" );
//		DT_TO_MSGBOX_FMT( LT_ERROR, "加载 BASS.DLL 失败!" );
//		return false;
//	}
//
//	LOADBASSFUNCTION( BASS_GetVersion );
//	LOADBASSFUNCTION(BASS_GetDeviceDescription);
//	LOADBASSFUNCTION(BASS_GetDevice);
//	LOADBASSFUNCTION(BASS_Init);
//	LOADBASSFUNCTION(BASS_Free);
//	LOADBASSFUNCTION(BASS_Start);
//	LOADBASSFUNCTION(BASS_Pause);
//	LOADBASSFUNCTION(BASS_Stop);
//	LOADBASSFUNCTION(BASS_SetConfig);
//	LOADBASSFUNCTION(BASS_ErrorGetCode);
//
//	LOADBASSFUNCTION(BASS_GetInfo);
//	LOADBASSFUNCTION(BASS_Update);
//	LOADBASSFUNCTION(BASS_GetCPU);
//	LOADBASSFUNCTION(BASS_SetVolume);
//	LOADBASSFUNCTION(BASS_GetVolume);
//
//	LOADBASSFUNCTION(BASS_PluginLoad);
//	LOADBASSFUNCTION(BASS_PluginFree);
//	LOADBASSFUNCTION(BASS_PluginGetInfo);
//
//	LOADBASSFUNCTION(BASS_Set3DFactors);
//	LOADBASSFUNCTION(BASS_Get3DFactors);
//	LOADBASSFUNCTION(BASS_Set3DPosition);
//	LOADBASSFUNCTION(BASS_Get3DPosition);
//	LOADBASSFUNCTION(BASS_SetEAXParameters);
//	LOADBASSFUNCTION(BASS_GetEAXParameters);
//
//	LOADBASSFUNCTION(BASS_SampleLoad);
//	LOADBASSFUNCTION(BASS_SampleCreate);
//	LOADBASSFUNCTION(BASS_SampleCreateDone);
//	LOADBASSFUNCTION(BASS_SampleGetInfo);
//	LOADBASSFUNCTION(BASS_SampleSetInfo);
//	LOADBASSFUNCTION(BASS_SampleGetChannel);
//	LOADBASSFUNCTION(BASS_SampleFree);
//	LOADBASSFUNCTION(BASS_SampleGetChannels);
//	LOADBASSFUNCTION(BASS_SampleStop);
//
//	LOADBASSFUNCTION(BASS_MusicLoad);
//	LOADBASSFUNCTION(BASS_MusicFree);
//	LOADBASSFUNCTION(BASS_MusicGetOrders);
//	LOADBASSFUNCTION(BASS_MusicGetOrderPosition);
//	LOADBASSFUNCTION(BASS_MusicSetAttribute);
//	LOADBASSFUNCTION(BASS_MusicGetAttribute);
//
//	LOADBASSFUNCTION(BASS_StreamCreateFile);
//	LOADBASSFUNCTION(BASS_StreamCreate);
//	LOADBASSFUNCTION(BASS_StreamCreateURL);
//	LOADBASSFUNCTION(BASS_StreamCreateFileUser);
//	LOADBASSFUNCTION(BASS_StreamGetFilePosition);
//	LOADBASSFUNCTION(BASS_StreamFree);
//
//	LOADBASSFUNCTION(BASS_ChannelGetInfo);
//	LOADBASSFUNCTION(BASS_ChannelGetAttributes);
//	LOADBASSFUNCTION(BASS_ChannelSetAttributes);
//	LOADBASSFUNCTION(BASS_ChannelSlideAttributes);
//	LOADBASSFUNCTION(BASS_ChannelIsSliding);
//	LOADBASSFUNCTION(BASS_ChannelSetFlags);
//	LOADBASSFUNCTION(BASS_ChannelGetData);
//	LOADBASSFUNCTION(BASS_ChannelPlay);
//	LOADBASSFUNCTION(BASS_ChannelPause);
//	LOADBASSFUNCTION(BASS_ChannelStop);
//	LOADBASSFUNCTION(BASS_ChannelIsActive);
//	LOADBASSFUNCTION(BASS_ChannelGetLength);
//	LOADBASSFUNCTION(BASS_ChannelGetPosition);
//	LOADBASSFUNCTION(BASS_ChannelSetPosition);
//	LOADBASSFUNCTION(BASS_ChannelSeconds2Bytes);
//	LOADBASSFUNCTION(BASS_ChannelBytes2Seconds);
//	LOADBASSFUNCTION(BASS_ChannelGetDevice);
//	LOADBASSFUNCTION(BASS_ChannelSetDevice);
//	LOADBASSFUNCTION(BASS_ChannelGetTags);
//	LOADBASSFUNCTION(BASS_ChannelPreBuf);
//	LOADBASSFUNCTION(BASS_ChannelSet3DAttributes);
//	LOADBASSFUNCTION(BASS_ChannelGet3DAttributes);
//	LOADBASSFUNCTION(BASS_ChannelSet3DPosition);
//	LOADBASSFUNCTION(BASS_ChannelGet3DPosition);
//	LOADBASSFUNCTION(BASS_ChannelGetLevel);
//	LOADBASSFUNCTION(BASS_ChannelSetSync);
//	LOADBASSFUNCTION(BASS_ChannelRemoveSync);
//	LOADBASSFUNCTION(BASS_ChannelSetDSP);
//	LOADBASSFUNCTION(BASS_ChannelRemoveDSP);
//	LOADBASSFUNCTION(BASS_ChannelSetLink);
//	LOADBASSFUNCTION(BASS_ChannelRemoveLink);
//	LOADBASSFUNCTION(BASS_ChannelSetEAXMix);
//	LOADBASSFUNCTION(BASS_ChannelGetEAXMix);
//	LOADBASSFUNCTION(BASS_ChannelSetFX);
//	LOADBASSFUNCTION(BASS_ChannelRemoveFX);
//	LOADBASSFUNCTION(BASS_FXSetParameters);
//	LOADBASSFUNCTION(BASS_FXGetParameters);
//#endif	
//
//	if (HIWORD(BASS_GetVersion()) != BASSVERSION)
//	{
//		DT_TO_LOG( LT_ERROR, "错误的 BASS.DLL 版本!" );
//		DT_TO_MSGBOX_FMT( LT_ERROR, "错误的 BASS.DLL 版本!" );
//		return false;
//	}
//
//	m_bSilent = false;
//	if (!BASS_Init( -1,m_nSampleRate, 0, ENGINE_INST->GetHWnd(), NULL ))
//	{
//		DT_TO_LOG( LT_ERROR, "初始化BASS失败,使用静音模式!" );
//		DT_TO_MSGBOX_FMT( LT_ERROR, "初始化BASS失败,使用静音模式!" );
//		BASS_Init(0,m_nSampleRate,0,ENGINE_INST->GetHWnd(),NULL);
//		m_bSilent=true;
//	}
//
//
//#ifdef USE_BASS24
//	u32 device = BASS_GetDevice();
//	BASS_DEVICEINFO bdi;
//	BASS_GetDeviceInfo( device, &bdi );
//	DT_TO_LOG( LT_INFO, GetString("Sound Device: %d\nDevice Name: %s\nSounder Driver: %s\n",device, bdi.name, bdi.driver) );
//	DT_TO_DBGSTR( LT_INFO, GetString("Sound Device: %d\nDevice Name: %s\nSounder Driver: %s\n",device, bdi.name, bdi.driver) );
//#else
//	DT_TO_LOG( LT_INFO, GetString("Sound Device: %s\n",BASS_GetDeviceDescription(1)) );
//	DT_TO_LOG( LT_INFO, GetString("Sample rate: %ld\n", m_nSampleRate));
//	DT_TO_DBGSTR( LT_INFO, GetString("Sound Device: %s\n",BASS_GetDeviceDescription(1)) );
//	DT_TO_DBGSTR( LT_INFO, GetString("Sample rate: %ld\n", m_nSampleRate));
//#endif
//
//
//	//BASS_SetConfig(BASS_CONFIG_BUFFER, 5000);
//	//BASS_SetConfig(BASS_CONFIG_UPDATEPERIOD, 50);
//
//	m_nFXVolume = 5000;
//	m_nMusVolume = 5000;
//	m_nStreamVolume = 5000;
//	_SetFXVolume(m_nFXVolume);
//	_SetMusVolume(m_nMusVolume);
//	_SetStreamVolume(m_nStreamVolume);
//
//	//_SetVolumn( m_fVolumn );
//
//	return true;
//}
//
//void CSoundSystem::_SoundDone()
//{
//	if(m_hBass)
//	{
//		BASS_Stop();
//		BASS_Free();
//
//		//int err = BASS_ErrorGetCode(); 
//		MAP_HEFFECT_ITOR efIT = m_mapHEffect.begin();
//		for ( ; efIT != m_mapHEffect.end(); ++efIT )
//		{
//			Effect_Free( efIT->second );
//		}
//		m_mapHEffect.clear();
//
//		MAP_HMUSIC_ITOR muIT = m_mapHMusic.begin();
//		for ( ; muIT != m_mapHMusic.end(); ++muIT )
//		{
//			Music_Free( muIT->second );
//		}
//		m_mapHMusic.clear();
//
//		MAP_HSTREAM_ITOR strIT = m_mapHStream.begin();
//		for ( ; strIT != m_mapHStream.end(); ++strIT )
//		{
//			Stream_Free( strIT->second );
//		}
//		m_mapHStream.clear();
//
//#ifndef USE_BASS24
//		FreeLibrary(m_hBass);
//#endif
//		m_hBass=0;
//	}
//}
//
//void CSoundSystem::_SetMusVolume(int vol)
//{
//	if(m_hBass)
//	{
//		BASS_SetConfig(BASS_CONFIG_GVOL_MUSIC, vol);
//		m_nMusVolume = BASS_GetConfig(BASS_CONFIG_GVOL_MUSIC);
//	}
//}
//
//void CSoundSystem::_SetStreamVolume(int vol)
//{
//	if(m_hBass) 
//	{
//		BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, vol);
//		m_nStreamVolume = BASS_GetConfig(BASS_CONFIG_GVOL_STREAM);
//	}
//}
//
//void CSoundSystem::_SetFXVolume(int vol)
//{
//	if(m_hBass) 
//	{
//		BASS_SetConfig(BASS_CONFIG_GVOL_SAMPLE, vol);
//		m_nFXVolume = BASS_GetConfig(BASS_CONFIG_GVOL_SAMPLE);
//	}
//}
//
//
//void  CSoundSystem::_SetVolumn( float volumn )
//{
//	if(m_hBass) 
//	{
//		if ( BASS_SetVolume( volumn ) )
//		{
//			m_fVolumn = BASS_GetVolume();
//		}
//		else
//		{
//			int err = BASS_ErrorGetCode();
//			switch ( err )
//			{
//			case BASS_ERROR_INIT:		DT_TO_MSGBOX( LT_ERROR, "BASS_Init has not been successfully called." ); break;
//			case BASS_ERROR_NOTAVAIL:	DT_TO_MSGBOX( LT_ERROR, "There is no volume control when using the \"no sound\" device." ); break;
//			case BASS_ERROR_ILLPARAM:	DT_TO_MSGBOX( LT_ERROR, "volume is invalid." ); break;
//			case BASS_ERROR_UNKNOWN:	DT_TO_MSGBOX( LT_ERROR, "Some other mystery problem!" ); break;
//			}
//		}
//		
//		
//	}
//}
//
//float  CSoundSystem::_GetVolumn( void )
//{
//	return m_fVolumn;
//}
//
//
//
//
//
