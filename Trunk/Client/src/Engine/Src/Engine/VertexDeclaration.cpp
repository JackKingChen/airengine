#include "stdafx.h"
#include "VertexDeclaration.h"
#include "DebugTools.h"

//CVertexElement::CVertexElement() : m_bInstancing(false)
//{
//
//}

size_t GetDeclTypeSize(VertexElementType type)
{
    switch (type)
    {
    case eVET_FLOAT1:
        return sizeof(float);

    case eVET_FLOAT2:
        return sizeof(float) * 2;

    case eVET_FLOAT3:
        return sizeof(float) * 3;

    case eVET_FLOAT4:
        return sizeof(float) * 4;

    case eVET_COLOUR:
        //return sizeof(u32);
        return sizeof(float) * 4;
        //case eVET_SHORT1:
        //    return 0;

    case eVET_SHORT2:
        return sizeof(short) * 2;

        //case eVET_SHORT3:
        //    return 0;

    case eVET_SHORT4:
        return sizeof(short) * 4;

    case eVET_UBYTE4:
        return sizeof(unsigned char) * 4;

    case eVET_COLOUR_ARGB:
        return sizeof(u32);
    }
    DT_TO_MSGBOX_FMT( LT_ERROR, GetString("Invalid vertex element type [%d]!", type) );

    return 0;
}

CVertexElement::CVertexElement(unsigned short nStream, unsigned short nOffset, 
                               VertexElementType type, 
                               VertexElementUsage usage, 
							   bool bInstancing,
                               unsigned short nIndex) :
m_nStream(nStream),
m_nOffset(nOffset),
m_eType(type),
m_eUsage(usage),
m_bInstancing(bInstancing),
m_nIndex(nIndex)
{

}

CVertexDeclaration::CVertexDeclaration():
m_nStride(0)
, m_bAutoDelete(false)
{

}

CVertexDeclaration::~CVertexDeclaration()
{

}

void CVertexDeclaration::AddVertexElement(unsigned short nStream, 
										  unsigned short nOffset, 
                                          VertexElementType type, 
                                          VertexElementUsage usage,
                                          unsigned short nIndex,
										  bool bInstancing)
{
    m_lstVertexElement.push_back(VERTEX_ELEMENT(nStream, nOffset, type, usage, bInstancing, nIndex));
}


size_t CVertexDeclaration::GetTypeSize( VertexElementType etype )
{
	switch(etype)
	{
	case eVET_COLOUR:
	case eVET_COLOUR_ABGR:
	case eVET_COLOUR_ARGB:
		return sizeof(f32) * 4;
	case eVET_FLOAT1:
		return sizeof(f32);
	case eVET_FLOAT2:
		return sizeof(f32)*2;
	case eVET_FLOAT3:
		return sizeof(f32)*3;
	case eVET_FLOAT4:
		return sizeof(f32)*4;
	case eVET_SHORT1:
		return sizeof(s32);
	case eVET_SHORT2:
		return sizeof(s32)*2;
	case eVET_SHORT3:
		return sizeof(s32)*3;
	case eVET_SHORT4:
		return sizeof(s32)*4;
	case eVET_UBYTE4:
		return sizeof(u8)*4;
	}
	return 0;
}

void CVertexDeclaration::Build(EVertexBufferType type)
{
    size_t offset = 0;
    switch( type )
    {
    case E_VB_STANDARD:
        {
            AddVertexElement( 0, offset, eVET_FLOAT3, eVEU_POSITION, 0, false );
            offset += sizeof(float) * 3;
            AddVertexElement( 0, offset, eVET_COLOUR, eVEU_DIFFUSE,  0, false );
            offset += sizeof(u32);
            AddVertexElement( 0, offset, eVET_FLOAT2, eVEU_TEXTURE_COORDINATES, 0, false );
            offset += sizeof(float) * 2;
            m_nStride = offset;
        }
        break;

    case E_VB_NORMAL:
        {
            AddVertexElement( 0, offset, eVET_FLOAT3, eVEU_POSITION, 0, false );
            offset += sizeof(float) * 3;
            AddVertexElement( 0, offset, eVET_FLOAT3, eVEU_NORMAL, 0, false );
            offset += sizeof(float) * 3;
            AddVertexElement( 0, offset, eVET_COLOUR, eVEU_DIFFUSE,  0, false );
            offset += sizeof(u32);
            AddVertexElement( 0, offset, eVET_FLOAT2, eVEU_TEXTURE_COORDINATES, 0, false );
            offset += sizeof(float) * 2;
            m_nStride = offset;
        }
        break;

    case E_VB_RHW:
        {
            AddVertexElement( 0, offset, eVET_FLOAT4, eVEU_POSITIONT, 0, false );
            offset += sizeof(float) * 4;
            AddVertexElement( 0, offset, eVET_COLOUR, eVEU_DIFFUSE,  0, false );
            offset += sizeof(u32);
            AddVertexElement( 0, offset, eVET_FLOAT2, eVEU_TEXTURE_COORDINATES, 0, false );
            offset += sizeof(float) * 2;
            m_nStride = offset;
        }
        break;

    case E_VB_NOTEX:
        {
            AddVertexElement( 0, offset, eVET_FLOAT3, eVEU_POSITION, 0, false );
            offset += sizeof(float) * 3;
            AddVertexElement( 0, offset, eVET_COLOUR, eVEU_DIFFUSE,  0, false );
            offset += sizeof(u32);
            m_nStride = offset;
        }
        break;

    case E_VB_NOCOLOR:
        {
            AddVertexElement( 0, offset, eVET_FLOAT3, eVEU_POSITION, 0, false );
            offset += sizeof(float) * 3;
            AddVertexElement( 0, offset, eVET_FLOAT2, eVEU_TEXTURE_COORDINATES, 0, false );
            offset += sizeof(float) * 2;
            m_nStride = offset;
        }
        break;

    case E_VB_NORMAL_TANGENT:
        {
            AddVertexElement( 0, offset, eVET_FLOAT3, eVEU_POSITION, 0, false );
            offset += sizeof(float) * 3;
            AddVertexElement( 0, offset, eVET_FLOAT3, eVEU_NORMAL, 0, false );
            offset += sizeof(float) * 3;
            AddVertexElement( 0, offset, eVET_FLOAT3, eVEU_TANGENT, 0, false );
            offset += sizeof(float) * 3;
            AddVertexElement( 0, offset, eVET_COLOUR, eVEU_DIFFUSE,  0, false );
            offset += sizeof(u32);
            AddVertexElement( 0, offset, eVET_FLOAT2, eVEU_TEXTURE_COORDINATES, 0, false );
            offset += sizeof(float) * 2;
            m_nStride = offset;
        }
        break;

    case E_VB_NORMAL_BONE_WEIGHT:
        {
            AddVertexElement( 0, offset, eVET_FLOAT3, eVEU_POSITION, 0, false );
            offset += sizeof(float) * 3;
            AddVertexElement( 0, offset, eVET_FLOAT3, eVEU_NORMAL, 0, false );
            offset += sizeof(float) * 3;
            AddVertexElement( 0, offset, eVET_COLOUR, eVEU_DIFFUSE,  0, false );
            offset += sizeof(u32);
            AddVertexElement( 0, offset, eVET_FLOAT2, eVEU_TEXTURE_COORDINATES, 0, false );
            offset += sizeof(float) * 2;
            AddVertexElement( 0, offset, eVET_FLOAT4, eVEU_TEXTURE_COORDINATES, 1, false );
            offset += sizeof(float) * 4;
            AddVertexElement( 0, offset, eVET_FLOAT4, eVEU_TEXTURE_COORDINATES, 2, false );
            offset += sizeof(float) * 4;
            m_nStride = offset;
        }
        break;
    case E_VB_NORMAL_INSTANCE_DATA:
        {
            AddVertexElement( 0, offset, eVET_FLOAT3, eVEU_POSITION, 0, false );
            offset += sizeof(float) * 3;
            AddVertexElement( 0, offset, eVET_FLOAT3, eVEU_NORMAL, 0, false );
            offset += sizeof(float) * 3;
            AddVertexElement( 0, offset, eVET_COLOUR, eVEU_DIFFUSE,  0, false );
            offset += sizeof(u32);
            AddVertexElement( 0, offset, eVET_FLOAT2, eVEU_TEXTURE_COORDINATES, 0, false );
            offset += sizeof(float) * 2;
            AddVertexElement( 0, offset, eVET_FLOAT1, eVEU_TEXTURE_COORDINATES, 1, false );
            offset += sizeof(float);
            m_nStride = offset;
        }
        break;
    case E_VB_RHW_NOTEX:
        {
            AddVertexElement( 0, offset, eVET_FLOAT4, eVEU_POSITIONT, 0, false );
            offset += sizeof(float) * 4;
            AddVertexElement( 0, offset, eVET_COLOUR, eVEU_DIFFUSE,  0, false );
            offset += sizeof(u32);
            m_nStride = offset;
        }
        break;

    default:
        DT_TO_MSGBOX_FMT( LT_ERROR, "Invalid vertex type!" );
        break;
    }
    Build();
}

void CVertexDeclaration::Create(AIRVector<CVertexElement>& elements, IGPUProgram* pVertexShader)
{
    m_lstVertexElement.clear();

    for (size_t i = 0; i < elements.size(); ++i)
    {
        m_lstVertexElement.push_back(elements[i]);
    }

    BuildByVertexShader(pVertexShader);
}

void CVertexDeclaration::Create(AIRVector<CVertexElement>& elements)
{
    m_lstVertexElement.clear();

    for (size_t i = 0; i < elements.size(); ++i)
    {
        m_lstVertexElement.push_back(elements[i]);
    }

    Build();
}

void CVertexDeclaration::BuildByVertexShader(IGPUProgram* pVertexShader)
{
    Build();
}

void CVertexDeclaration::Build(void)
{
	m_nStride = 0;
	for (VertexElementList::iterator it = m_lstVertexElement.begin(); it != m_lstVertexElement.end(); ++it)
	{
		m_nStride += GetDeclTypeSize(it->m_eType);
	}
}
