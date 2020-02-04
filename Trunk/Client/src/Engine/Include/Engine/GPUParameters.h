// ***************************************************************
//  文件: GPUParameters.h
//  日期: 2010-3-11
//  作者: liangairan
//  说明: GPU参数类，专门处理GPU的参数
// ***************************************************************

#pragma once 
#include "EngineConfig.h"
#include "Vector3D.h"
#include "Vector4.h"
#include "Matrix4.h"
#include "ColorValue.h"
#include <vector>
#include <map>
#include <string>
#include "GPUDataParam.h"
#include "MemoryAlloter.h"
#include "FixedString.h"

using namespace AIR;

class CShaderParameter;
//class ManualGPUDataParamSource;

enum GpuConstantType
{
    eGCT_FLOAT1 = 1,
    eGCT_FLOAT2 = 2,
    eGCT_FLOAT3 = 3,
    eGCT_FLOAT4 = 4,
    eGCT_SAMPLER1D = 5,
    eGCT_SAMPLER2D = 6,
    eGCT_SAMPLER3D = 7,
    eGCT_SAMPLERCUBE = 8,
    eGCT_SAMPLER1DSHADOW = 9,
    eGCT_SAMPLER2DSHADOW = 10,
    eGCT_MATRIX_2X2 = 11,
    eGCT_MATRIX_2X3 = 12,
    eGCT_MATRIX_2X4 = 13,
    eGCT_MATRIX_3X2 = 14,
    eGCT_MATRIX_3X3 = 15,
    eGCT_MATRIX_3X4 = 16,
    eGCT_MATRIX_4X2 = 17,
    eGCT_MATRIX_4X3 = 18,
    eGCT_MATRIX_4X4 = 19,
    eGCT_INT1 = 20,
    eGCT_INT2 = 21,
    eGCT_INT3 = 22,
    eGCT_INT4 = 23,
    eGCT_BOOL = 24,
    eGCT_UNKNOWN = 99
};

enum AutoConstantType
{
    eACT_WORLDMATRIX,
    eACT_VIEWMATRIX,
    eACT_PROJECTIONMATRIX,
    eACT_WVPMATRIX,
	eACT_INVWVPMATRIX,
    eACT_WORLDVIEWMATIRX,   //
    eACT_VIEWPROJMATRIX,
    eACT_TEXPROJMATRIX,     //投影纹理矩阵
    eACT_CAMERAPOSITION,
    eACT_AMBIENT_COLOR,
    eACT_FOG_COLOR,
    eACT_FOG_START,
    eACT_FOG_END,
    eACT_FOG_DENSITY,      //雾指数值
    eACT_LIGHT_POSITION,
    eACT_LIGHT_DIRECTION,
    eACT_SUN_LIGHT_DIRECTION,
    eACT_SUN_LIGHT_COLOR,       //太阳光颜色
    eACT_LIGHT_DIFFUSE_COLOR,
    eACT_LIGHT_SPECULAR_COLOR,
    eACT_LIGHT_AMBIENT_COLOR,
    eACT_CLIPPLANE,
    //eACT_UVSCALE_ARRAY,     //纹理坐标缩放数组
    //eACT_UVSCALE,           //单个纹理坐标缩放
    eACT_MATERIAL_AMBIENT,  //材质表面环境光参数
    eACT_MATERIAL_DIFFUSE,  //材质表面漫反射参数
    eACT_MATERIAL_SPECULAR, //材质表面镜面反射参数
    eACT_MATERIAL_SHINESS,  //材质表面镜面反射强度
	eACT_FRAME_TIME,
    eACT_BONE_MATRIX,       //骨骼矩阵
    eACT_WORLDMATRIX_ARRAY, //世界矩阵数量
    eACT_POSITION_ARRAY,
    eACT_LIGHT_RADIUS,
    eACT_VIEWPORT_WIDTH,    //视口宽
    eACT_VIEWPORT_HEIGHT,   //视口高
    eACT_LIGHTPOS_INVIEW,   //在视图空间的点光源坐标
    eACT_INVVIEWPROJ_MATRIX,   //inverse(view * project)
    eACT_CAMERA_FARPLANE,    //摄像机远截面
    eACT_FARPLANE_CORNERS,   //远截面的四个点
    eACT_HALFPIXEL,          //半个像素
    eACT_COLOR_DIFFUSE,
    eACT_FOG_TYPE,
    eACT_CAMERA_RIGHT,       //摄像机右向量
    eACT_CAMERA_LOOK,        //摄像机朝向
    eACT_CAMERA_UP,          //摄像机上向量
	eACT_CLIPENABLE,         //是否要用裁截面
    eACT_JITTERSCALE,
    eACT_FILTTERSIZE,
    eACT_JITTERDELTA,
    eACT_LightSpaceProjMatrix,
    eACT_LightSpaceViewProjMatrix,
    //eACT_AlphaRef,
    eACT_UNKNOWN,
};

struct GPUConstantDefinition 
{
    FixedString      strName;      //名字
    GpuConstantType  ConstantType; //变量类型
    AutoConstantType UsageType;    //用途类型
    u32    nHandle;          //对应的handle
    size_t nPhysicalIndex;   //在GPUParameters里的数组索引
    size_t nRegisterIndex;   //sm3.0以下是寄存器索引，sm5.0是cbuffer中的起始字节索引
    size_t nRegisterCount;    //寄存器数量，即ogre中的elementSize
    size_t nElementSize;     //D3D9中单位为4个float的大小，SM5以上是一个值的大小
    size_t nSizeInBytes;
    size_t nArraySize;   //数组长度
    int    nManualUsageType;   //手动类型用途
    void*  pDefaultValue;
    //ManualGPUDataParamSource* pManaulGPUSource;
    u32 nRealValueOffset;   //一个值的真正大小 byte
    void* pValue;
    u32 nConstantBuffer;    //属于哪个ConstantBuffer

    bool IsFloat() const
    {
        return IsFloat(ConstantType);
    }

    static bool IsFloat(GpuConstantType c)
    {
        switch(c)
        {
        case eGCT_INT1:
        case eGCT_INT2:
        case eGCT_INT3:
        case eGCT_INT4:
        case eGCT_SAMPLER1D:
        case eGCT_SAMPLER2D:
        case eGCT_SAMPLER3D:
        case eGCT_SAMPLERCUBE:
        case eGCT_SAMPLER1DSHADOW:
        case eGCT_SAMPLER2DSHADOW:
            return false;
        default:
            return true;
        };

    }

    static bool IsInt(GpuConstantType c)
    {
        switch(c)
        {
        case eGCT_INT1:
        case eGCT_INT2:
        case eGCT_INT3:
        case eGCT_INT4:
            return true;
        case eGCT_SAMPLER1D:
        case eGCT_SAMPLER2D:
        case eGCT_SAMPLER3D:
        case eGCT_SAMPLERCUBE:
        case eGCT_SAMPLER1DSHADOW:
        case eGCT_SAMPLER2DSHADOW:
            return false;
        default:
            return false;
        };

    }

    bool IsInt() const
    {
        return IsInt(ConstantType);
    }

    bool IsSampler() const
    {
        return IsSampler(ConstantType);
    }

    static bool IsSampler(GpuConstantType c)
    {
        switch(c)
        {
        case eGCT_SAMPLER1D:
        case eGCT_SAMPLER2D:
        case eGCT_SAMPLER3D:
        case eGCT_SAMPLERCUBE:
        case eGCT_SAMPLER1DSHADOW:
        case eGCT_SAMPLER2DSHADOW:
            return true;
        default:
            return false;
        };
    }

    /** Get the element size of a given type, including whether to pad the 
    elements into multiples of 4 (e.g. SM1 and D3D does, GLSL doesn't)
    */
    static size_t GetElementSize(GpuConstantType ctype, bool padToMultiplesOf4 = false)
    {
        if (padToMultiplesOf4)
        {
            switch(ctype)
            {
            case eGCT_FLOAT1:
            case eGCT_INT1:
            case eGCT_SAMPLER1D:
            case eGCT_SAMPLER2D:
            case eGCT_SAMPLER3D:
            case eGCT_SAMPLERCUBE:
            case eGCT_SAMPLER1DSHADOW:
            case eGCT_SAMPLER2DSHADOW:
            case eGCT_FLOAT2:
            case eGCT_INT2:
            case eGCT_FLOAT3:
            case eGCT_INT3:
            case eGCT_FLOAT4:
            case eGCT_INT4:
                return 4;
            case eGCT_MATRIX_2X2:
            case eGCT_MATRIX_2X3:
            case eGCT_MATRIX_2X4:
                return 8; // 2 float4s
            case eGCT_MATRIX_3X2:
            case eGCT_MATRIX_3X3:
            case eGCT_MATRIX_3X4:
                return 12; // 3 float4s
            case eGCT_MATRIX_4X2:
            case eGCT_MATRIX_4X3:
            case eGCT_MATRIX_4X4:
                return 16; // 4 float4s
            default:
                return 4;
            };
        }
        else
        {
            switch(ctype)
            {
            case eGCT_FLOAT1:
            case eGCT_INT1:
            case eGCT_SAMPLER1D:
            case eGCT_SAMPLER2D:
            case eGCT_SAMPLER3D:
            case eGCT_SAMPLERCUBE:
            case eGCT_SAMPLER1DSHADOW:
            case eGCT_SAMPLER2DSHADOW:
                return 1;
            case eGCT_FLOAT2:
            case eGCT_INT2:
                return 2;
            case eGCT_FLOAT3:
            case eGCT_INT3:
                return 3;
            case eGCT_FLOAT4:
            case eGCT_INT4:
                return 4;
            case eGCT_MATRIX_2X2:
                return 4;
            case eGCT_MATRIX_2X3:
            case eGCT_MATRIX_3X2:
                return 6;
            case eGCT_MATRIX_2X4:
            case eGCT_MATRIX_4X2:
                return 8; 
            case eGCT_MATRIX_3X3:
                return 9;
            case eGCT_MATRIX_3X4:
            case eGCT_MATRIX_4X3:
                return 12; 
            case eGCT_MATRIX_4X4:
                return 16; 
            default:
                return 4;
            };

        }
    }

    GPUConstantDefinition()
        : ConstantType(eGCT_UNKNOWN)
        , nHandle(0)
        , nPhysicalIndex((std::numeric_limits<size_t>::max)())
        , nManualUsageType(-1)
        , pDefaultValue(NULL)
        //, pManaulGPUSource(NULL)
        , nRealValueOffset(0)
        , nArraySize(0)
        , pValue(0)
        , nConstantBuffer(0)
        //, elementSize(0)
        //, arraySize(1)
        //, variability(GPV_GLOBAL) 
    {

    }
};

class EngineExport GPUParameters// : public EngineAllocator(GPUParameters)
{
	EngineAllocator(GPUParameters)
public:
    typedef AIRVector<GPUConstantDefinition> GPUConstDef_Vector;
    typedef GPUConstDef_Vector::iterator  GPUConstDef_Vector_Iter;

    //取得手动更新参数
    GPUConstDef_Vector* GetManualGPUConstants()
    {
        return &m_vtManualGPUConstDef;
    }

    GPUConstDef_Vector  m_vtGPUConstDef;         //自动更新
    GPUConstDef_Vector  m_vtManualGPUConstDef;   //手动更新

    s32 GetGPUVariantIndex(const FixedString& strName) const; 
    
public:
    GPUParameters();
    virtual ~GPUParameters();

 

    virtual void AutoUpdateParameters(CGPUDataParamSource* pGPUDataParam) {};

    //size_t GetFloatPhysicalIndex(size_t requestedSize);

    //size_t GetIntPhysicalIndex(size_t requestedSize);

    virtual void AddConstantDef(const FixedString& strName, GPUConstantDefinition& def) {};

    //virtual void BuildManualConstantType(ManualGPUDataParamSource* pSource);

    //更新手动GPU参数
    //virtual void ManualUpdateGPUParameters() = 0;

	//inline void SetManualConstantTypeBuilded( bool builded )
	//{
	//	m_bManualConstantTypeBuilded = builded;
	//}
    
    //AIRVector<int>      m_vtIntConstants;
    //AIRVector<float>    m_vtFloatConstants;

    //bool m_bManualConstantTypeBuilded;    //是否已经更新了手动constant的类型

    static AutoConstantType GetAutoConstantTypeByName(const FixedString& strName);
    //ManualGPUDataParamSource* m_pManualGPUParamSource;
private:
};
