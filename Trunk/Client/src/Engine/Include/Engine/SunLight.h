#pragma once

#include "FlareSpot.h"
#include "Camera.h"

const int SPOT_COUNT = 5;

class EngineExport CSunLight
{
public:
public:
    CSunLight(void);
    virtual ~CSunLight(void);

    void Render(CCamera* pCamera);

    void Initialize(const Vector3Df& pos, const Vector3Df& camPos);

    void SetSunLight(const Vector3Df& vLight);
    void SetSunPos(const Vector3Df& pos);
    void GetSunLightVec(Vector3Df& vLight) const;// { vLight = m_vSunLight; }

    void Translate3Dto2D(const Vector2D& viewport, const Vector3Df& vPos, int& nX, int& nY, bool* bClip);
    void Clear();
    void   Update(CCamera* pCamera);

    virtual u32 GetRenderTriangles() { return 6 * 2; }

    //virtual bool IsInFrustum(CFrustum* pFrustum) { return true; }
private:
    //bool   LoadTextures();

    void   SetupMatrix();

    //bool   CreateSprite();

    Vector3Df  m_posSun;    //太阳所在位置
    Vector3Df  m_vSunLight; //太阳光照方向
    float         m_fSunToCam; //太阳到摄像机的方向

    CFlareSpot      m_sun;
    CFlareSpot      m_spots[SPOT_COUNT];

    Matrix4f      m_matWorld;

    //LPD3DXSPRITE    m_pSprite;
    ITexture*   m_pSunTexture;
    ITexture*   m_pMinSpot;
    ITexture*   m_pMiddleSpot;
    ITexture*   m_pLargeSpot;

};
