
#pragma once

#include "EngineConfig.h"

class EngineExport CPerlinNoise
{
public:
    // 一维噪声
    static float noise1(const float x);
    // 二维噪声
    static float noise2(const float x, const float y);
    // 三维噪声
    static float noise3(const float x, const float y, const float z);

    // 二维扰动
    static float turbulence2(const float x, const float y, float freq);
    // 三维扰动
    static float turbulence3(const float x, const float y, const float z, float freq);

    // 一维连续噪声
    static float tileableNoise1(const float x, const float w);
    // 二维连续噪声
    static float tileableNoise2(const float x, const float y, const float w, const float h);
    // 三维连续噪声
    static float tileableNoise3(const float x, const float y, const float z, const float w, const float h, const float d);

    // 二维连续湍流
    static float tileableTurbulence2(const float x, const float y, const float w, const float h, float freq);
    // 三维连续湍流
    static float tileableTurbulence3(const float x, const float y, const float z, const float w, const float h, const float d, float freq);

    // 初始化噪声
    static void  initNoise();

    inline static float noise(int x, int y)
    {
        int n=x+y*57;
        n=(n<<13)^n;
        return ( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0); 
    }
protected:
private:
};


