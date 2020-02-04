// ***************************************************************
//  文件: Light.h
//  日期: 2009-1-5
//  作者: liangairan
//  说明: 光照类
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

    //这里只画它的boundingbox，可用于延迟渲染时的lightbuffer
    virtual void PostRender(CCamera* pCamera);

protected:
    Vector3Df   m_vPosition;     //位置
    Vector3Df   m_vDirection;    //方向
    Vector3Df   m_DerivedPosition;  //世界位置
    Vector3Df   m_DerivedDirection; //世界空间中的方向
    CColorValue   m_clrAmbient;  //环境光
    CColorValue   m_specular;    //镜面颜色
    CColorValue   m_diffuse;     //漫反射

    LightTypes    m_type;         //光照类型

    f32       m_spotOuter;     //spotline's outer 
    f32       m_spotInner;
    f32       m_spotFalloff;
    f32       m_range;         //衰减范围

    f32       m_fAttentionConst;
    f32       m_fAttentionLinear;
    f32       m_fAttentionQuad;
    
    f32       m_fTempDistance;  //使用的时候与影响物体的距离

    mutable bool      m_bTransform;     //是否已经进行了世界变换

protected:
    virtual void OnUpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue);

    void CaculateBoundingbox();
private:
    //AIRString m_strName;   //名字
};
