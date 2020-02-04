#ifndef __MemorySTLAllocator_H__
#define __MemorySTLAllocator_H__

#include <typeinfo>

//
//A User-Defined Allocator 用于代替stl的默认分配器
//
template
<
	typename T
	//typename AllocPolicy
>
class CMemorySTLAllocator
{
public:
	//type definitions as iso c++
	typedef size_t    size_type;
	typedef ptrdiff_t difference_type;
	typedef T*        pointer;
	typedef const T*  const_pointer;
	typedef T&        reference;
	typedef const T&  const_reference;
	typedef T         value_type;

	/*constructors and destructor
	*-nothing to do because the allocator has no state
	*/

	CMemorySTLAllocator() throw()
	{

	}

	CMemorySTLAllocator( CMemorySTLAllocator const&) throw()
	{

	}

	template<typename U>
	CMemorySTLAllocator( CMemorySTLAllocator<U> const&) throw()
	{

	}

	template<typename U, typename P>
	CMemorySTLAllocator( CMemorySTLAllocator<U> const&) throw()
	{

	}

	~CMemorySTLAllocator() throw()
	{

	}



	//rebind allocator to type U
	//This template structure provides the ability 
	//that any allocator may allocate storage of another type indirectly
	template <class U>
	struct rebind
	{
		typedef CMemorySTLAllocator<U> other;
	};

	//return address of values
	pointer address(reference value) const
	{
		return &value;
	}

	const_pointer address(const_reference value) const
	{
		return &value;
	}

	//return maximum number of elements that can
	//be allocated 
	size_type max_size() const throw()
	{
		return HighLevelSTLAllocator::getMaxAllocationSize() / sizeof(T);
	}

	//allocate but don't initialize num elements of type T
    pointer allocate(size_type nNum/*, typename std::allocator<void>::const_pointer ptr = 0*/)
	{
		pointer p  = static_cast<pointer>(HighLevelSTLAllocator::AllocBytes( nNum * sizeof( T ), typeid(T).name(), __FILE__, __LINE__, __FUNCTION__ ) );
		return p;
	}

	//initialize elements of allocated storage p with value value
	void construct (pointer p, const T& value) 
	{
		//initialize memory with placement new
		new( static_cast<void*>( p ) )T(value);
	}

	//destroy elements of initialized storage p
	void destroy (pointer p)
	{
		// destroy objects by calling their destructor
		p->~T();
	}

	//deallocate storage p of deleted elements
	void deallocate (pointer p, size_type num)
	{
		//deallocate memory with global delete
		HighLevelSTLAllocator::deallocBytes(p);
	}

	inline bool operator != (const CMemorySTLAllocator<T>& other) const 
	{ 
		return false; 
	}

protected:
private:
};

/// determine equality, can memory from another allocator
/// be released by this allocator, (ISO C++)
template<typename T, typename T2>
inline bool operator==(CMemorySTLAllocator<T> const&,
					   CMemorySTLAllocator<T2> const&)
{
	// same alloc policy (P), memory can be freed
	return true;
}

/// determine equality, can memory from another allocator
/// be released by this allocator, (ISO C++)
template<typename T, typename P, typename OtherAllocator>
inline bool operator==(CMemorySTLAllocator<T> const&,
					   OtherAllocator const&)
{
	return false;
}
/// determine equality, can memory from another allocator
/// be released by this allocator, (ISO C++)
template<typename T, typename T2, typename P>
inline bool operator!=(CMemorySTLAllocator<T> const&,
					   CMemorySTLAllocator<T2> const&)
{
	// same alloc policy (P), memory can be freed
	return false;
}

/// determine equality, can memory from another allocator
/// be released by this allocator, (ISO C++)
template<typename T, typename P, typename OtherAllocator>
inline bool operator!=(CMemorySTLAllocator<T> const&,
					   OtherAllocator const&)
{
	return true;
}



#endif// __MemorySTLAllocator_H__