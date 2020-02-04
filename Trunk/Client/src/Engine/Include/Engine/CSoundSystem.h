//// ***************************************************************
////  文件: CSoundSystem.h
////  日期: 2010-3-2010/03/18
////  作者: huangjunjie
////  说明: 声音系统
//// ***************************************************************
//
//
//#pragma once
//
//#ifndef __CSoundSystem_H_
//#define __CSoundSystem_H_
//
//#include "EngineConfig.h"
//#include "MemoryAllocatorConfig.h"
////#include <Windows.h>
//#include <list>
//#include <map>
//#include "AIRTypes.h"
//
//using namespace AIR;
//
//typedef DWORD HEFFECT;
//typedef DWORD HMUSIC;
//typedef DWORD HSTREAM;
//typedef DWORD HCHANNEL;
//
//typedef std::map<AIRString, HEFFECT>	MAP_HEFFECT;
//typedef std::map<AIRString, HMUSIC>	MAP_HMUSIC;
//typedef std::map<AIRString, HSTREAM>	MAP_HSTREAM;
//typedef std::map<AIRString, HCHANNEL> MAP_HCHANNEL;
//
//typedef MAP_HEFFECT::iterator			MAP_HEFFECT_ITOR;
//typedef MAP_HMUSIC::iterator			MAP_HMUSIC_ITOR;
//typedef MAP_HSTREAM::iterator			MAP_HSTREAM_ITOR;
//typedef MAP_HCHANNEL::iterator			MAP_HCHANNEL_ITOR;
//
//class EngineExport CSoundSystem
//{
//
//	CSoundSystem() : m_hBass(NULL),m_bSilent(false),m_nSampleRate(44100), m_nFXVolume(100), m_nMusVolume(100),m_nStreamVolume(100),m_fVolumn(0.5f)
//	{
//		m_mapHChanel.clear();
//		m_mapHEffect.clear();
//		m_mapHMusic.clear();
//		m_mapHStream.clear();
//		_SoundInit(); 
//	}
//
//public:
//	~CSoundSystem(){ _SoundDone(); }
//
//	static CSoundSystem* GetSingleton( void )
//	{
//		static CSoundSystem soundsys;
//		return &soundsys;
//	}
//
//	virtual bool			LoadSound( const char* szName, const char* szFile );
//	virtual void			FreeSound( const char* szName );
//	virtual void			PlaySound( const char* szName, bool loop = false );
//	virtual HCHANNEL		GetSoundHandle( const char* szName );
//
//	virtual HEFFECT			Effect_Load(const char *filename, u32 size=0);
//	virtual void			Effect_Free(HEFFECT eff);
//	virtual HCHANNEL	 	Effect_Play(HEFFECT eff);
//	virtual HCHANNEL	 	Effect_PlayEx(HEFFECT eff, int volume=100, int pan=0, float pitch=1.0f, bool loop=false);
//
//	virtual HMUSIC		 	Music_Load(const char *filename, u32 size=0);
//	virtual void			Music_Free(HMUSIC mus);
//	virtual HCHANNEL	 	Music_Play(HMUSIC mus, bool loop, int volume = 100, int order = 0, int row = 0);
//	virtual void			Music_SetAmplification(HMUSIC music, int ampl);
//	virtual int				Music_GetAmplification(HMUSIC music);
//	virtual int				Music_GetLength(HMUSIC music);
//	virtual void			Music_SetPos(HMUSIC music, int order, int row);
//	virtual bool			Music_GetPos(HMUSIC music, int *order, int *row);
//	virtual void			Music_SetInstrVolume(HMUSIC music, int instr, int volume);
//	virtual int				Music_GetInstrVolume(HMUSIC music, int instr);
//	virtual void			Music_SetChannelVolume(HMUSIC music, int channel, int volume);
//	virtual int				Music_GetChannelVolume(HMUSIC music, int channel);
//
//	virtual HSTREAM			Stream_Load(const char *filename, u32 size=0);
//	virtual void			Stream_Free(HSTREAM stream);
//	virtual HCHANNEL		Stream_Play(HSTREAM stream, bool loop, int volume = 100);
//
//	virtual void		 	Channel_SetPanning(HCHANNEL chn, int pan);
//	virtual void		 	Channel_SetVolume(HCHANNEL chn, int volume);
//	virtual void		 	Channel_SetPitch(HCHANNEL chn, float pitch);
//	virtual void		 	Channel_Pause(HCHANNEL chn);
//	virtual void		 	Channel_Resume(HCHANNEL chn);
//	virtual void		 	Channel_Stop(HCHANNEL chn);
//	virtual void		 	Channel_PauseAll();
//	virtual void		 	Channel_ResumeAll();
//	virtual void		 	Channel_StopAll();
//	virtual bool			Channel_IsPlaying(HCHANNEL chn);
//	virtual float			Channel_GetLength(HCHANNEL chn);
//	virtual float			Channel_GetPos(HCHANNEL chn);
//	virtual void			Channel_SetPos(HCHANNEL chn, float fSeconds);
//	virtual void			Channel_SlideTo(HCHANNEL channel, float time, int volume, int pan = -101, float pitch = -1);
//	virtual bool			Channel_IsSliding(HCHANNEL channel);
//
//	//************************************
//	// Method:    _SoundInit
//	// Returns:   bool
//	// Qualifier: 初始化音频系统
//	//************************************
//	bool					_SoundInit();
//
//	//************************************
//	// Method:    _SoundDone
//	// Returns:   void
//	// Qualifier: 结束音频系统
//	//************************************
//	void					_SoundDone();
//
//	//************************************
//	// Method:    _SetMusVolume
//	// Returns:   void
//	// Qualifier: 设置music音量 (1-100)
//	// Parameter: int vol
//	//************************************
//	void					_SetMusVolume(int vol);
//
//	int						_GetMusVolume( void )
//	{
//		return m_nMusVolume;
//	}
//
//	//************************************
//	// Method:    _SetStreamVolume
//	// Returns:   void
//	// Qualifier: 设置stream音量 (1-100)
//	// Parameter: int vol
//	//************************************
//	void					_SetStreamVolume(int vol);
//
//	int						_GetStreamVolume( void )
//	{
//		return m_nStreamVolume;
//	}
//
//	//************************************
//	// Method:    _SetFXVolume
//	// Returns:   void
//	// Qualifier: 设置fx音量 (1-100)
//	// Parameter: int vol
//	//************************************
//	void					_SetFXVolume(int vol);
//
//	int						_GetFXVolume( void )
//	{
//		return m_nFXVolume;
//	}
//
//	void					_SetVolumn( float volumn );
//
//	float					_GetVolumn( void );
//	
//private:
//	HINSTANCE				m_hBass;			// bass dll instance
//	bool					m_bSilent;			// 是否静音模式
//	int						m_nSampleRate;		// 取样率
//	int						m_nFXVolume;		// fx 音量
//	int						m_nMusVolume;		// music 音量
//	int						m_nStreamVolume;	// stream 音量
//	float					m_fVolumn;
//
//	MAP_HCHANNEL			m_mapHChanel;
//	MAP_HSTREAM				m_mapHStream;
//	MAP_HMUSIC				m_mapHMusic;
//	MAP_HEFFECT				m_mapHEffect;
//
//}; //! end class CSoundSystem
//
//#ifndef SOUNDSYSTEM
//#define SOUNDSYSTEM	(CSoundSystem::GetSingleton())
//#endif
//
//#endif //! end __CSoundSystem_H_