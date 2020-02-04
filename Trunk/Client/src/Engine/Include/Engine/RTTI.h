#pragma once 

class RTTI
{
public:
	RTTI(const char* szName, const RTTI* pBaseClass) : m_pClassName(szName), m_pBaseRTTI(pBaseClass)
	{

	}

	const char* GetName() const
	{
		return m_pClassName;
	}

	const RTTI* GetBaseRTTI() const
	{
		return m_pBaseRTTI;
	}

private:
	const char *m_pClassName;
	const RTTI *m_pBaseRTTI;
};

#define DECLARE_ROOT_RTTI \
	public: \
	static const RTTI m_RTTI; \
	virtual const RTTI *GetRTTI() const{ return &m_RTTI; } \
	bool IsExactKindOf( const RTTI *prtti ) const \
{ \
	return (GetRTTI() == prtti); \
} \
	bool IsKindOf( const RTTI *prtti ) const;

#define DECLARE_RTTI \
	public: \
	static const RTTI m_RTTI; \
	virtual const RTTI *GetRTTI() const{ return &m_RTTI; } \
	//static RootObject *NewObject(){ return new classname; }

//#define DECLARE_RTTI_VIRTUAL(classname) \
//	public: \
//	static const RTTI m_RTTI; \
//	virtual const RTTI *GetRTTI() const{ return &m_RTTI; } \

#define IMPLEMENT_ROOT_RTTI(classname) \
	const RTTI classname::m_RTTI(#classname, 0); \
	bool classname::IsKindOf( const RTTI *prtti ) const \
{ \
	const RTTI *pmine = GetRTTI(); \
	while( pmine != 0 ) \
{ \
	if( pmine == prtti ) return true; \
			else pmine = pmine->GetBaseRTTI(); \
} \
	return false; \
}

#define IMPLEMENT_RTTI(classname, baseclassname) \
	const RTTI classname::m_RTTI(#classname, &baseclassname::m_RTTI);

#define IMPLEMENT_RTTI_VIRTUAL(classname, baseclassname) \
	const RTTI classname::m_RTTI(#classname, &baseclassname::m_RTTI);


#define IS_EXACT_KIND_OF(classname, pobj) \
	pobj->IsExactKindOf( &classname::m_RTTI )

#define IS_KIND_OF(classname, pobj) \
	pobj->IsKindOf( &classname::m_RTTI )

