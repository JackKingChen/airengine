// ***************************************************************
//  文件: Set.h
//  日期: 2010-8-13
//  作者: huangjunjie
//  说明: 
// ***************************************************************

#pragma once

#ifndef __Set_H__
#define __Set_H__

#include <stdlib.h>
#include <string.h>

template <class ARG_TYPE>
class Set 
{
public:
	Set()
	{
		count = capasity = 0;
		list = NULL;
	}

	Set(const unsigned int iCapasity)
	{
		count = 0;
		capasity = iCapasity;
		list = (ARG_TYPE *) malloc(capasity * sizeof(ARG_TYPE));
	}

	~Set()
	{
		free(list);
	}

	ARG_TYPE *GetArray() const { return list; }
	ARG_TYPE &operator [] (const unsigned int index) const { return list[index]; }
	unsigned int GetCount() const { return count; }

	void SetCount(const unsigned int newCount)
	{
		capasity = count = newCount;
		list = (ARG_TYPE *) realloc(list, capasity * sizeof(ARG_TYPE));		
	}

	unsigned int Add(const ARG_TYPE object)
	{
		if (count >= capasity)
		{
			if (capasity) 
				capasity += capasity; 
			else 
				capasity = 8;

			list = (ARG_TYPE *) realloc(list, capasity * sizeof(ARG_TYPE));
		}
		list[count] = object;
		return count++;
	}

	void Remove(const unsigned int index)
	{
		if (index < count)
		{
			count--;
			list[index] = list[count];
		}
	}

	void OrderedRemove(const unsigned int index)
	{
		if (index < count)
		{
			count--;
			memmove(list + index, list + index + 1, (count - index) * sizeof(ARG_TYPE));
		}
	}

	void RemoveObject(const ARG_TYPE object)
	{
		for (unsigned int i = 0; i < count; i++)
		{
			if (object == list[i])
			{
				remove(i);
				return;
			}
		}
	}

	void Clear()
	{
		count = 0;
	}

	void Reset()
	{
		free(list);
		list = NULL;
		count = capasity = 0;
	}

private:
	int Partition(int (*compare)(const ARG_TYPE &elem0, const ARG_TYPE &elem1), int p, int r)
	{
		ARG_TYPE tmp, pivot = list[p];
		int left = p;

		for (int i = p + 1; i <= r; i++)
		{
			if (compare(list[i], pivot) < 0)
			{
				left++;
				tmp = list[i];
				list[i] = list[left];
				list[left] = tmp;
			}
		}
		tmp = list[p];
		list[p] = list[left];
		list[left] = tmp;
		return left;
	}

	void QuickSort(int (*compare)(const ARG_TYPE &elem0, const ARG_TYPE &elem1), int p, int r)
	{
		if (p < r)
		{
			int q = Partition(compare, p, r);
			QuickSort(compare, p, q - 1);
			QuickSort(compare, q + 1, r);
		}
	}
public:
	void Sort(int (*compare)(const ARG_TYPE &elem0, const ARG_TYPE &elem1))
	{
		QuickSort(compare, 0, count - 1);
	}

protected:
	unsigned int	capasity;
	unsigned int	count;
	ARG_TYPE *		list;
};

#endif //! __Set_H__