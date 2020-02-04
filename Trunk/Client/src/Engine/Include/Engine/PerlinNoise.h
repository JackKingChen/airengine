
#pragma once

#include "EngineConfig.h"

class EngineExport CPerlinNoise
{
public:
    // һά����
    static float noise1(const float x);
    // ��ά����
    static float noise2(const float x, const float y);
    // ��ά����
    static float noise3(const float x, const float y, const float z);

    // ��ά�Ŷ�
    static float turbulence2(const float x, const float y, float freq);
    // ��ά�Ŷ�
    static float turbulence3(const float x, const float y, const float z, float freq);

    // һά��������
    static float tileableNoise1(const float x, const float w);
    // ��ά��������
    static float tileableNoise2(const float x, const float y, const float w, const float h);
    // ��ά��������
    static float tileableNoise3(const float x, const float y, const float z, const float w, const float h, const float d);

    // ��ά��������
    static float tileableTurbulence2(const float x, const float y, const float w, const float h, float freq);
    // ��ά��������
    static float tileableTurbulence3(const float x, const float y, const float z, const float w, const float h, const float d, float freq);

    // ��ʼ������
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


