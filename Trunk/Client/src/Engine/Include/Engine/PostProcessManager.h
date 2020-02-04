/********************************************************************
	created:	2010/06/17
	created:	17:6:2010   10:41
	filename: 	PostProcessManager.h
	author:		liangairan
	
	purpose:	���������
*********************************************************************/

#pragma once 

#include "PostScreenCompositor.h"
#include "Singleton.h"

//enum PostProcessFilter
//{
//    ePPF_Distortion = 0,
//    ePPF_HBloom,
//    ePPF_VBloom,
//    ePPF_VBlur,
//    ePPF_HBlur,
//    ePPF_RayBlur,
//    ePPF_HDR,
//    ePPF_MAX,
//};

class SamplerStateObject;
class NormalSceneRenderProcess;

enum PostEffectType
{
 //   ePET_PostScene = 0,	//����
//ePET_Monochrome,    //�ڰ�
 //   
 //   ePET_Bloom,			//Bloom
 //   ePET_Blur,			//blur
 //   ePET_RayBlur,       //����ģ��
 //   ePET_HDR,			//high dynamin range
 //   ePET_MAX,
    ePET_Monochrome = 0,    //�ڰ�
    ePET_Bloom,			//Bloom
	ePET_Distortion,	//Ť��
    ePET_Blur,          //ģ��
    ePET_SoftLight,     //���
    ePET_Glow,          //ȫ������
	ePET_MAX,
};


class EngineExport PostEffect
{
public:
    typedef AIRList<PostScreenCompositor*>	CompositortList;
    typedef CompositortList::iterator			CompositortList_Iter;

public:
    PostEffect();
    virtual ~PostEffect();

    bool				IsActive() const
    {
        return m_bActive;
    }

    void				SetActive(bool bActive);

    PostEffectType		GetPostEffectType() const
    {
        return m_postEffectType;
    }

    CompositortList&	GetCompositorList()
    {
        return m_lstCompositors;
    }

    //��������
    virtual void		Create(int nPosX, int nPosY, int nWidth, int nHeight, PostEffectType type);

    //���
    virtual void		Clear();

    void				OnResetDevice();

protected:
    bool				m_bActive;
    PostEffectType		m_postEffectType;
    CompositortList		m_lstCompositors;

private:
    PostScreenCompositor* CreatePostScreenCompositor(int nPosX, int nPosY, int nWidth, int nHeight, const char* szName, ScreenCompositorType pscType);
};

class EngineExport PostProcess
{
	EngineAllocator(PostProcess);
public:
	PostProcess();
	virtual ~PostProcess();

	void Process(ITexture* pInputTex);

	ITexture* GetOutputTexture();

protected:
	TextureRenderTarget* m_pOutputTarget;
	RHWVERTEX   m_renderQuad[4];
	//�����������ö�������
	void BuildScreenSizeData();
	float m_fScale;
	float m_fUVScale;
};


class EngineExport PostProcessManager : public Singleton<PostProcessManager>
{
    BE_SINGLETON(PostProcessManager);
	typedef AIRList<PostScreenCompositor*>	CompositortList;
	typedef CompositortList::iterator			CompositortList_Iter;

public:
    ~PostProcessManager();

    //Ԥ����
    void PostProcess();

    //������Ⱦ����Ļ��
    void		Render();

	//����������Ч
    PostEffect* CreatePostEffect(PostEffectType type, int nWidth, int nHeight);

	//��ú�����Ч
    PostEffect* GetPostEffect(PostEffectType type)
    {
        return m_PostEffects[type];
    }

    //���
    void		Clear();

	//��/�رպ�����Ч
    void		EnablePostEffect(bool bActive, PostEffectType type);
 
	//
	TextureRenderTarget*  GetSceneRenderTarget()
	{
		return m_pSceneRenderTarget;
	}
	//����ֶ���gpu���ζ���
    //PostCompositorManualGPUDataSource* GetPostManualGPUDataSource()
    //{
    //    return m_pPostManualGPUDataSource;
    //}

	//��ѯĳ�����͵ĺϳ�����ǰ�Ƿ񼤻�
    bool		IsPostEffectActive(PostEffectType type);

    void		SaveToFile(const char* szFilename);

	//��ü���ĺϳ�������
    size_t		GetActiveCompositorsNum() const;

    //�豸����
    void		OnResetDevice();


protected:
    
private:
	NormalSceneRenderProcess*           m_pSceneRenderProcess;
    TextureRenderTarget*						m_pSceneRenderTarget;		//������rendertarget
    //PostCompositorManualGPUDataSource*  m_pPostManualGPUDataSource;	//
    ScreenCompositor*					m_pCompositor;				//�����������Ļ��compositor
    PostEffect*							m_PostEffects[ePET_MAX];	//
    CompositortList						m_lstActiveCompositors;		//
    float                               m_fScreenScale;             //�������ű���
	float								m_fUVScale;                 //uv����
    SamplerStateObject* m_pSamplerState;
};

#define POSTPROCESS_MANAGER PostProcessManager::GetInstance()
