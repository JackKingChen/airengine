// ***************************************************************
//  �ļ�: Light.h
//  ����: 2009-1-5
//  ����: liangairan
//  ˵��: ������
// ***************************************************************

#pragma once 

#include "Vector3D.h"
#include "Vector4.h"
#include "ColorValue.h"
#include "Color.h"
#include "Common.h"
#include "Renderable.h"

class EngineExport CLight : public Renderable
{
public:
    enum LightTypes
    {
        LT_POINT = 0,
        LT_DIRECTIONAL = 1,
        LT_SPOTLIGNT = 2,
    };

    /*
    CLight() : m_type(LT_POINT),
        m_spotOuter(0.0f),
        m_spotInner(0.0f),
        m_spotFalloff(0.0f),
        m_range(0.0f),
        m_fAttentionConst(0.0f),
        m_fAttentionLinear(0.0f),
        m_fAttentionQuad(0.0f),
        m_fTempDistance(0.0f)
    {

    }
    */

    CLight() : m_type(LT_POINT),
        m_spotOuter(0.0f),
        m_spotInner(0.0f),
        m_spotFalloff(0.0f),
        m_range(0.0f),
        m_fAttentionConst(0.0f),
        m_fAttentionLinear(0.0f),
        m_fAttentionQuad(0.0f),
        m_fTempDistance(0.0f)
    { 
        m_szType = "light"; 
    }
    virtual ~CLight() {}

    LightTypes GetType() { return m_type; }

    const CColorValue& GetAmbient() const { return m_clrAmbient; }
    const CColorValue& GetDiffuse() const { return m_diffuse; }
    const CColorValue& GetSpecular() const { return m_specular; }
    const Vector3Df& GetPosition() const { return m_vPosition; }
    const Vector3Df& GetDirection() const { return m_vDirection; }
    
    void SetPosition(const Vector3Df& position); 

    virtual void SetPosition(float x, float y, float z);

    void SetDirection(const Vector3Df& direction); 

    void SetDiffuse(const CColorValue& color) { m_diffuse = color; }

    void SetSpecular(const CColorValue& color) { m_specular = color; }

    void SetAmbient(const CColorValue& color);

    void SetType(LightTypes etype) { m_type = etype; } 

    f32  GetRange() { return m_range; }

    f32  GetTmpDistance() { return m_fTempDistance; }

    f32  GetSpotOuter() { return m_spotOuter; }

    f32  GetSpotInner() { return m_spotInner; }

    f32  GetSpotFalloff() { return m_spotFalloff; }

    f32  GetTempDistance() { return m_fTempDistance; }

    f32  GetAttentionConst() 
    {
        return m_fAttentionConst;
    }

    f32  GetAttentionLinear()
    {
        return m_fAttentionLinear;
    }

    f32  GetAttentionQuad()
    {
        return m_fAttentionQuad;
    }

    void SetAttentionConst(f32 fAttention);

    void SetAttentionLinear(f32 fAttention);

    void SetAttentionQuad(f32 fAttention);

    const Vector3Df& GetDerivedPosition();

    const Vector3Df& GetDerivedDirection();

    void Update();

    virtual void Update(float fTimeElapse);

    virtual void SetAttachedNode(CSceneNode* pNode);

    virtual void NotifyMove();

    virtual void SetRange(float fRange);

    //����ֻ������boundingbox���������ӳ���Ⱦʱ��lightbuffer
    virtual void PostRender(CCamera* pCamera);

protected:
    Vector3Df   m_vPosition;     //λ��
    Vector3Df   m_vDirection;    //����
    Vector3Df   m_DerivedPosition;  //����λ��
    Vector3Df   m_DerivedDirection; //����ռ��еķ���
    CColorValue   m_clrAmbient;  //������
    CColorValue   m_specular;    //������ɫ
    CColorValue   m_diffuse;     //������

    LightTypes    m_type;         //��������

    f32       m_spotOuter;     //spotline's outer 
    f32       m_spotInner;
    f32       m_spotFalloff;
    f32       m_range;         //˥����Χ

    f32       m_fAttentionConst;
    f32       m_fAttentionLinear;
    f32       m_fAttentionQuad;
    
    f32       m_fTempDistance;  //ʹ�õ�ʱ����Ӱ������ľ���

    mutable bool      m_bTransform;     //�Ƿ��Ѿ�����������任

protected:
    virtual void OnUpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue);

    void CaculateBoundingbox();
private:
    //AIRString m_strName;   //����
};
