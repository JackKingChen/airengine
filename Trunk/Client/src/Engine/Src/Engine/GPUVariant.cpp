#include "GPUVariant.h"


GPUVariant::GPUVariant() : m_type(VAR_NULL)
{

}

GPUVariant::GPUVariant(const GPUVariant& var) : m_type(var.m_type)
{
    if (m_type == VAR_FLOAT4X4)
    {
        m_pMatrix = AIR_NEW_ARRAY_T(f32, 16);
        memcpy(m_pMatrix, var.m_pMatrix, 16 * sizeof(f32));
    }
    else
    {
        memcpy(m_vec, var.m_vec, sizeof(m_vec));
    }
}

 
GPUVariant::GPUVariant(bool b) : m_type(VAR_BOOL), m_b(b)
{

}

 GPUVariant::GPUVariant(s32 i) : m_type(VAR_INT), m_i(i)
{

}

 GPUVariant::GPUVariant(f32 f) : m_type(VAR_FLOAT), m_f(f)
{

}

 GPUVariant::GPUVariant(const Vector2Df& v) : m_type(VAR_FLOAT2)
{
    m_vec[0] = v.x;
    m_vec[1] = v.y;
}

 GPUVariant::GPUVariant(const Vector3Df& v) : m_type(VAR_FLOAT3)
{
    m_vec[0] = v.x;
    m_vec[1] = v.y;
    m_vec[2] = v.z;
}

 GPUVariant::GPUVariant(const Vector4f& v) : m_type(VAR_FLOAT4)
{
    memcpy(m_vec, &v, sizeof(Vector4f));
}

 GPUVariant::GPUVariant(const CColorValue& col) : m_type(VAR_FLOAT4)
{
    memcpy(m_vec, &col, sizeof(CColorValue));
}

 GPUVariant::GPUVariant(const Matrix4f& mat) : m_type(VAR_FLOAT4X4)
{
    m_pMatrix = AIR_NEW_ARRAY_T(f32, 16);
    memcpy(m_pMatrix, &mat, sizeof(Matrix4f));
}

 GPUVariant::GPUVariant(ITexture* pTexture) : m_type(VAR_TEXTURE), m_pTexture(pTexture)
{
    
}

GPUVariant::~GPUVariant()
{
    Clear();
}

void GPUVariant::SetBool(bool b)
{
    Clear();
    m_type = VAR_BOOL;
    m_b = b;
}

void GPUVariant::SetInt(s32 i)
{
    Clear();
    m_type = VAR_INT;
    m_i = i;
}

void GPUVariant::SetFloat(f32 f)
{
    Clear();
    m_type = VAR_FLOAT;
    m_f = f;
}

void GPUVariant::SetVector2(const Vector2Df& vec)
{
    Clear();
    m_type = VAR_FLOAT2;
    m_vec[0] = vec.x;
    m_vec[1] = vec.y;
}

void GPUVariant::SetVector3(const Vector3Df& vec)
{
    Clear();
    m_type = VAR_FLOAT3;
    m_vec[0] = vec.x;
    m_vec[1] = vec.y;
    m_vec[2] = vec.z;
}

void GPUVariant::SetVector4(const Vector4f& vec)
{
    Clear();
    m_type = VAR_FLOAT4;
    memcpy(m_vec, &vec, sizeof(Vector4f));
}

void GPUVariant::SetColor(const CColorValue& color)
{
    Clear();
    m_type = VAR_FLOAT4;
    memcpy(m_vec, &color, sizeof(Vector4f));
}

void GPUVariant::SetMatrix(const Matrix4f& mat)
{
    Clear();
    m_type = VAR_FLOAT4X4;
    m_pMatrix = AIR_NEW_ARRAY_T(f32, 16);
    memcpy(m_pMatrix, &mat, sizeof(Matrix4f));
}

void GPUVariant::SetPointer(void* pPointer)
{
    Clear();
    m_type = VAR_POINTER;
    m_p = pPointer;
}

void GPUVariant::SetTexture(ITexture* pTexture)
{
    Clear();
    m_type = VAR_TEXTURE;
    m_pTexture = pTexture;
}

void GPUVariant::SetValue(const char* pValue)
{
    Clear();
    switch(m_type)
    {
    case VAR_BOOL:
        break;
    }
}

void GPUVariant::Set(VARIANT_T type, const char* pValue)
{
    m_type = type;
}

GPUVariantArray::GPUVariantArray()
{

}

GPUVariantArray::GPUVariantArray(const GPUVariantArray& rhs)
{

}



