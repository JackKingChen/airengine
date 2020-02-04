#include "stdafx.h"
#include "Color.h"


const SColor SColor::White		= 0xFFFFFFFF;
const SColor SColor::Black		= 0xFF000000;
const SColor SColor::Zero		= 0x00000000;
const SColor SColor::Gray		= 0xFF808080;
const SColor SColor::Red		= 0xFFFF0000;
const SColor SColor::Green		= 0xFF00FF00;
const SColor SColor::Blue		= 0xFF0000FF;
const SColor SColor::Yellow		= 0xFFFFFF00;
const SColor SColor::Cyan		= 0xFF00FFFF;
const SColor SColor::Magenta	= 0xFFFF00FF;

const CColorValue CColorValue::White(SColor::White);
const CColorValue CColorValue::Black(SColor::Black);
const CColorValue CColorValue::Zero(SColor::Zero);
const CColorValue CColorValue::Gray(SColor::Gray);
const CColorValue CColorValue::Red(SColor::Red);
const CColorValue CColorValue::Green(SColor::Green);
const CColorValue CColorValue::Blue(SColor::Blue);
const CColorValue CColorValue::Yellow(SColor::Yellow);
const CColorValue CColorValue::Cyan(SColor::Cyan);
const CColorValue CColorValue::Magenta(SColor::Magenta);



//#include "ColorValue.h"

//const CColorValue CColorValue::ZERO = CColorValue(0.0, 0.0, 0.0, 0.0);
//const CColorValue CColorValue::Black = CColorValue(1.0, 0.0, 0.0, 0.0);
//const CColorValue CColorValue::White = CColorValue(1.0, 1.0, 1.0, 1.0);
//const CColorValue CColorValue::Red = CColorValue(1.0, 1.0, 0.0, 0.0);
//const CColorValue CColorValue::Green = CColorValue(1.0, 0.0, 1.0, 0.0);
//const CColorValue CColorValue::Blue = CColorValue(1.0, 0.0, 0.0, 1.0);
//const CColorValue CColorValue::Yellow = CColorValue(1.0f, 1.0f, 1.0f, 0.0f);


//void CColorValue::SetAsARGB(const u32 val)
//{
//    u32 val32 = val;
//
//    // Convert from 32bit pattern
//    // (ARGB = 8888)
//
//    // Alpha
//    a = ((val32 >> 24) & 0xFF) / 255.0f;
//
//    // Red
//    r = ((val32 >> 16) & 0xFF) / 255.0f;
//
//    // Green
//    g = ((val32 >> 8) & 0xFF) / 255.0f;
//
//    // Blue
//    b = (val32 & 0xFF) / 255.0f;
//}
//
//void CColorValue::SetAsRGBA(const u32 val)
//{
//    u32 val32 = val;
//
//    // Convert from 32bit pattern
//    // (RGBA = 8888)
//
//    // Red
//    r = ((val32 >> 24) & 0xFF) / 255.0f;
//
//    // Green
//    g = ((val32 >> 16) & 0xFF) / 255.0f;
//
//    // Blue
//    b = ((val32 >> 8) & 0xFF) / 255.0f;
//
//    // Alpha
//    a = (val32 & 0xFF) / 255.0f;
//}
//
//void CColorValue::SetAsBGRA(const u32 val)
//{
//    u32 val32 = val;
//
//    // Convert from 32bit pattern
//    // (ARGB = 8888)
//
//    // Blue
//    b = ((val32 >> 24) & 0xFF) / 255.0f;
//
//    // Green
//    g = ((val32 >> 16) & 0xFF) / 255.0f;
//
//    // Red
//    r = ((val32 >> 8) & 0xFF) / 255.0f;
//
//    // Alpha
//    a = (val32 & 0xFF) / 255.0f;
//}
//
//void CColorValue::SetAsBGR(const u32 val)
//{
//    u32 val32 = val;
//    // Blue
//    a = 1.0f;
//
//    // Green
//    b = ((val32 >> 16) & 0xFF) / 255.0f;
//
//    // Red
//    g = ((val32 >> 8) & 0xFF) / 255.0f;
//
//    // Alpha
//    r = (val32 & 0xFF) / 255.0f;
//}
//
//void CColorValue::SetAsABGR(const u32 val)
//{
//    u32 val32 = val;
//
//    // Convert from 32bit pattern
//    // (ABGR = 8888)
//
//    // Alpha
//    a = ((val32 >> 24) & 0xFF) / 255.0f;
//
//    // Blue
//    b = ((val32 >> 16) & 0xFF) / 255.0f;
//
//    // Green
//    g = ((val32 >> 8) & 0xFF) / 255.0f;
//
//    // Red
//    r = (val32 & 0xFF) / 255.0f;
//}
//
//void CColorValue::SetAsRGB(const u32 val)
//{
//    u32 val32 = val;
//
//    // Convert from 32bit pattern
//    // (ARGB = 8888)
//
//    // Alpha
//    //a = ((val32 >> 24) & 0xFF) / 255.0f;
//    a = 1.0f;
//
//    // Red
//    r = ((val32 >> 16) & 0xFF) / 255.0f;
//
//    // Green
//    g = ((val32 >> 8) & 0xFF) / 255.0f;
//
//    // Blue
//    b = (val32 & 0xFF) / 255.0f;
//}
