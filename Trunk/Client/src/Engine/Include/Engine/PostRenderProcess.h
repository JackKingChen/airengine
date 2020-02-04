#pragma once 

#include "CustomRenderProcess.h"
class PostProcess;
class NormalSceneRenderProcess;
class ScreenCompositor;
class TextureRenderTarget;
class ITexture;

enum PostProcessType
{
	ePP_Monochrome = 0,    //黑白
	ePP_Bloom,			//Bloom
	ePP_Distortion,	//扭曲
	ePP_Blur,          //模糊
	ePP_SoftLight,     //柔光
	ePP_Glow,          //全屏泛光
	ePP_MAX,
};

class EngineExport PostRenderProcess : public CustomRenderProcess
{
	EngineAllocator(PostRenderProcess);
public:
	PostRenderProcess();
	virtual ~PostRenderProcess();

	void Reset();
	virtual void Render();

	void Clear();

	void SetInputTexture(ITexture* pTexture)
	{
		m_pInputTexture = pTexture;
	}

	ITexture* GetOutputTexture()
	{
		return m_pOutputTexture;
	}

	void SetSceneTexture(ITexture* pSceneTexture);
protected:
	AIRVector<ScreenCompositor*> m_vtPostEffects;
	AIRVector<TextureRenderTarget*> m_vtRenderTargets;
	ITexture* m_pInputTexture;
	ITexture* m_pOutputTexture;
	ITexture* m_pSceneTexture;   //场景渲染后的纹理
	//NormalSceneRenderProcess* m_pSceneProcess;
	PostProcessType m_type;
};


class EngineExport PostGlowRenderProcess : public PostRenderProcess
{
public:
	PostGlowRenderProcess();
	virtual ~PostGlowRenderProcess();

	virtual void Render();

private:
	void Init();
};

class EngineExport PostSoftlightRenderProcess : public PostRenderProcess
{
public:
	PostSoftlightRenderProcess();
	virtual ~PostSoftlightRenderProcess();

	virtual void Render();

private:
	void Init();
};

