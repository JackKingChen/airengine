#pragma once 

#include "CustomRenderProcess.h"
class PostProcess;
class NormalSceneRenderProcess;
class ScreenCompositor;
class TextureRenderTarget;
class ITexture;

enum PostProcessType
{
	ePP_Monochrome = 0,    //�ڰ�
	ePP_Bloom,			//Bloom
	ePP_Distortion,	//Ť��
	ePP_Blur,          //ģ��
	ePP_SoftLight,     //���
	ePP_Glow,          //ȫ������
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
	ITexture* m_pSceneTexture;   //������Ⱦ�������
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

