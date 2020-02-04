#pragma once

#include "ITexture.h"
#include "Vector2D.h"
#include "Color.h"
#include "Rect.h"

class EngineExport CFlareSpot
{
public:
    CFlareSpot(void);
    virtual ~CFlareSpot(void);

    //void SetPos(const D3DXVECTOR3& pos);
    void SetWidth(int nWidth);

    void Render();

    int  GetWidth() { return m_nWidth; }

    void SetScreenPos(int nX, int nY);
    void SetScreenPos(const Vector2Df& pos);

    void GetScreenPos(int& nX, int& nY);


    void Clear();

    bool Create(const char* szTextureFile);

    void SetColor(const SColor& dwColor);
protected:
private:
    ITexture*  m_pTexture;
    int m_nScreenX;     //ÆÁÄ»×ø±ê
    int m_nScreenY;

    int m_nWidth;       //ÆÁÄ»¿í
    SColor  m_clrBlend;

    AIR::Rectf      m_Rect;

    bool LoadTexture(const char* szFilename);
};
