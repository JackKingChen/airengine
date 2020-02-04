// ***************************************************************
//  文件: IteratorWrappers.h
//  日期: 2010-4-2010/04/17
//  作者: huangjunjie
//  说明: stl iterator wrappers
// ***************************************************************


#pragma once

#ifndef __IteratorWrappers_H_
#define __IteratorWrappers_H_



template <class T>
class ListIterator
{
private:
	typename T::iterator mCurrent;
	typename T::iterator mEnd;
	ListIterator() {};

public:
	typedef typename T::value_type ValueType;

	ListIterator(typename T::iterator start, typename T::iterator end)
		: mCurrent(start), mEnd(end)
	{
	}

	explicit ListIterator(T& c)
		: mCurrent(c.begin()), mEnd(c.end())
	{
	}

	bool IsEnd( void ) const
	{
		return mCurrent == mEnd;
	}

	typename T::value_type GetNext(void)
	{
		return *mCurrent++;
	}

	typename T::value_type PeekNext(void)
	{
		return *mCurrent;
	}

	typename T::pointer PeekNextPtr(void)
	{
		return &(*mCurrent);
	}

	void MoveNext(void)
	{
		++mCurrent;
	}

};




template <class T>
class ConstListIterator
{
private:
	mutable typename T::const_iterator mCurrent;
	typename T::const_iterator mEnd;
	ConstListIterator() {};

public:
	typedef typename T::value_type ValueType;

	ConstListIterator(typename T::const_iterator start, typename T::const_iterator end)
		: mCurrent(start), mEnd(end)
	{
	}

	explicit ConstListIterator(const T& c)
		: mCurrent(c.begin()), mEnd(c.end())
	{
	}

	bool IsEnd( void ) const
	{
		return mCurrent == mEnd;
	}

	typename T::value_type GetNext(void)
	{
		return *mCurrent++;
	}

	typename T::value_type PeekNext(void) const
	{
		return *mCurrent;
	}

	typename T::const_pointer PeekNextPtr(void) const
	{
		return &(*mCurrent);
	}

	void MoveNext(void) const
	{
		++mCurrent;
	}
};




template <class T>
class VectorIterator
{
private:
	typename T::iterator mCurrent;
	typename T::iterator mEnd;
	VectorIterator() {};

public:
	typedef typename T::value_type ValueType;

	VectorIterator(typename T::iterator start, typename T::iterator end)
		: mCurrent(start), mEnd(end)
	{
	}

	explicit VectorIterator(T& c)
		: mCurrent(c.begin()), mEnd(c.end())
	{
	}

	bool IsEnd( void ) const
	{
		return mCurrent == mEnd;
	}

	typename T::value_type GetNext(void)
	{
		return *mCurrent++;
	}

	typename T::value_type PeekNext(void)
	{
		return *mCurrent;
	}

	typename T::pointer PeekNextPtr(void)
	{
		return &(*mCurrent);
	}

	void MoveNext(void)
	{
		++mCurrent;
	}

};







template <class T>
class MapIterator
{
private:
	typename T::iterator mCurrent;
	typename T::iterator mEnd;
	MapIterator() {};
public:
	typedef typename T::mapped_type MappedType;
	typedef typename T::key_type KeyType;

	MapIterator(typename T::iterator start, typename T::iterator end)
		: mCurrent(start), mEnd(end)
	{
	}

	explicit MapIterator(T& c)
		: mCurrent(c.begin()), mEnd(c.end())
	{
	}

	bool IsEnd( void ) const
	{
		return mCurrent == mEnd;
	}

	typename T::mapped_type GetNext(void)
	{
		return (mCurrent++)->second;
	}

	typename T::mapped_type PeekNextValue(void)
	{
		return mCurrent->second;
	}

	typename T::key_type PeekNextKey(void)
	{
		return mCurrent->first;
	}

	MapIterator<T> & operator=(const MapIterator<T> &rhs )
	{
		mCurrent = rhs.mCurrent;
		mEnd = rhs.mEnd;
		return *this;
	}

	typename T::mapped_type* PeekNextValuePtr(void)
	{
		return &(mCurrent->second);
	}

	void MoveNext(void)
	{
		++mCurrent;
	}
};





template <class T>
class ConstVectorIterator
{
private:
	mutable typename T::const_iterator mCurrent;
	typename T::const_iterator mEnd;

	ConstVectorIterator() {};
public:
	typedef typename T::value_type ValueType;

	ConstVectorIterator(typename T::const_iterator start, typename T::const_iterator end)
		: mCurrent(start), mEnd(end)
	{
	}

	explicit ConstVectorIterator(const T& c)
		: mCurrent(c.begin()), mEnd(c.end())
	{
	}

	bool IsEnd( void ) const
	{
		return mCurrent == mEnd;
	}

	typename T::value_type GetNext(void)
	{
		return *mCurrent++;
	}

	typename T::value_type PeekNext(void) const
	{
		return *mCurrent;
	}

	typename T::const_pointer PeekNextPtr(void) const
	{
		return &(*mCurrent);
	}

	void MoveNext(void) const
	{
		++mCurrent;
	}
};






template <class T>
class ConstMapIterator
{
private:
	mutable typename T::const_iterator mCurrent;
	typename T::const_iterator mEnd;

	ConstMapIterator() {};
public:
	typedef typename T::mapped_type MappedType;
	typedef typename T::key_type KeyType;

	ConstMapIterator(typename T::const_iterator start, typename T::const_iterator end)
		: mCurrent(start), mEnd(end)
	{
	}

	explicit ConstMapIterator(const T& c)
		: mCurrent(c.begin()), mEnd(c.end())
	{
	}

	bool IsEnd( void ) const
	{
		return mCurrent == mEnd;
	}

	typename T::mapped_type GetNext(void)
	{
		return (mCurrent++)->second;
	}

	typename T::mapped_type PeekNextValue(void) const
	{
		return mCurrent->second;
	}

	typename T::key_type PeekNextKey(void) const
	{
		return mCurrent->first;
	}

	ConstMapIterator<T> & operator=( ConstMapIterator<T> &rhs )
	{
		mCurrent = rhs.mCurrent;
		mEnd = rhs.mEnd;
		return *this;
	}

	const typename T::mapped_type* PeekNextValuePtr(void) const
	{
		return &(mCurrent->second);
	}

	void MoveNext(void) const
	{
		++mCurrent;
	}
};

template <class T>
class SetIterator
{
private:
    typename T::iterator mCurrent;
    typename T::iterator mEnd;
    SetIterator() {};
public:
    typedef typename T::value_type ValueType;
    //typedef typename T::key_type KeyType;

    SetIterator(typename T::iterator start, typename T::iterator end)
        : mCurrent(start), mEnd(end)
    {
    }

    explicit SetIterator(T& c)
        : mCurrent(c.begin()), mEnd(c.end())
    {
    }

    bool IsEnd( void ) const
    {
        return mCurrent == mEnd;
    }

    typename T::value_type GetNext(void)
    {
        return (*mCurrent++);
    }

    typename T::value_type PeekNextValue(void)
    {
        return mCurrent->second;
    }

    SetIterator<T> & operator = ( SetIterator<T> &rhs )
    {
        mCurrent = rhs.mCurrent;
        mEnd = rhs.mEnd;
        return *this;
    }

    typename T::value_type* PeekNextValuePtr(void)
    {
        return &(*mCurrent);
    }

    void MoveNext(void)
    {
        ++mCurrent;
    }
};


#endif //! end __IteratorWrappers_H_