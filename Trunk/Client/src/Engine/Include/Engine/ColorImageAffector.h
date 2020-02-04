// ***************************************************************
//  �ļ�: ColorImageAffector.h
//  ����: 2010-3-16
//  ����: liangairan
//  ˵��: ��ɫͼƬӰ����
// ***************************************************************

#pragma once

#include "ParticleAffector.h"
#include "AIRImage.h"

class EngineExport CColorImageAffector : public CParticleAffector
{
public:
    CColorImageAffector();
    virtual ~CColorImageAffector();

    virtual void AffectParticle(CParticleSystem* pParticleSystem, float fTimeElapsed);

    //virtual void InitParticle(CParticle* pParticle);

    virtual bool LoadFromXML(CMarkupSTL& xml);

    virtual bool SaveToXMLNode(CMarkupSTL& xml);

    virtual AIRString GetTypeStr()
    {
        return "ColorImage";
    }

    //�����ļ�
    void SetImageFile(const AIRString& strFile);

    const AIRString& GetColorImgFile() const
    {
        return m_strImageFile;
    }

protected:
    //ImageOperator  m_imgColor;
	AIRImage       m_imgColor;
    bool           m_bLoadImage;

    AIRString    m_strImageFile;

    void LoadImage();
private:
};
