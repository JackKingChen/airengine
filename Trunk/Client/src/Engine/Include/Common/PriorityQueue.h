#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H


#include <ostream>
#include <vector>
#include <cassert>

//----------------------- Swap -------------------------------------------
//  交换2个值
//------------------------------------------------------------------------
template<class T>
void Swap(T &a, T &b)
{
	T temp = a;
	a = b;
	b = temp;
}

//-------------------- ReorderUpwards ------------------------------------
//
//  given a heap and a node in the heap, this function moves upwards
//  through the heap swapping elements until the heap is ordered
//  给定一个堆和堆中的一个节点, 该方法通过向上移动元素来使堆变得有序
//  
//------------------------------------------------------------------------
template<class T>
void ReorderUpwards(AIRVector<T>& heap, int nd)
{
	//move up the heap swapping the elements until the heap is ordered
	while ( (nd>1) && (heap[nd/2] < heap[nd]))
	{
		Swap(heap[nd/2], heap[nd]);

		nd /= 2;
	}
}

//--------------------- ReorderDownwards ---------------------------------
//
//  given a heap, the heapsize and a node in the heap, this function
//  reorders the elements in a top down fashion by moving down the heap
//  and swapping the current node with the greater of its two children
//  (provided a child is larger than the current node)
//------------------------------------------------------------------------
template<class T>
void ReorderDownwards(AIRVector<T>& heap, int nd, int HeapSize)
{
	//move down the heap from node nd swapping the elements until
	//the heap is reordered
	while (2*nd <= HeapSize)
	{
		int child = 2 * nd;

		//set child to largest of nd's two children
		if ( (child < HeapSize) && (heap[child] < heap[child+1]) )
		{
			++child;
		}

		//if this nd is smaller than its child, swap
		if (heap[nd] < heap[child])
		{
			Swap(heap[child], heap[nd]);

			//move the current node down the tree
			nd = child;
		}

		else
		{
			break;
		}
	}
}



//--------------------- PriorityQ ----------------------------------------
//
//  basic heap based priority queue implementation
//------------------------------------------------------------------------
template<class T>
class PriorityQ
{
private:

	AIRVector<T>  m_Heap;

	int             m_iSize;

	int             m_iMaxSize;

	//given a heap and a node in the heap, this function moves upwards
	//through the heap swapping elements until the heap is ordered
	void ReorderUpwards(AIRVector<T>& heap, int nd)
	{
		//move up the heap swapping the elements until the heap is ordered
		while ( (nd>1) && (heap[nd/2] < heap[nd]))
		{
			Swap(heap[nd/2], heap[nd]);
			nd /= 2;
		}
	}

	//given a heap, the heapsize and a node in the heap, this function
	//reorders the elements in a top down fashion by moving down the heap
	//and swapping the current node with the greater of its two children
	//(provided a child is larger than the current node)
	void ReorderDownwards(AIRVector<T>& heap, int nd, int HeapSize)
	{
		//move down the heap from node nd swapping the elements until
		//the heap is reordered
		while (2*nd <= HeapSize)
		{
			int child = 2 * nd;
			//set child to largest of nd's two children
			if ( (child < HeapSize) && (heap[child] < heap[child+1]) )
			{
				++child;
			}
			//if this nd is smaller than its child, swap
			if (heap[nd] < heap[child])
			{
				Swap(heap[child], heap[nd]);
				//move the current node down the tree
				nd = child;
			}
			else
			{
				break;
			}
		}
	}

public:

	PriorityQ(int MaxSize):m_iMaxSize(MaxSize), m_iSize(0)
	{
		m_Heap.assign(MaxSize+1, T());
	}

	bool empty()const{return (m_iSize==0);}

	//to insert an item into the queue it gets added to the end of the heap
	//and then the heap is reordered
	void insert(const T item)
	{

		assert (m_iSize+1 <= m_iMaxSize);

		++m_iSize;

		m_Heap[m_iSize] = item;

		ReorderUpwards(m_Heap, m_iSize);
	}

	//to get the max item the first element is exchanged with the lowest
	//in the heap and then the heap is reordered from the top down. 
	T pop()
	{
		Swap(m_Heap[1], m_Heap[m_iSize]);
		ReorderDownwards(m_Heap, 1, m_iSize-1);
		return m_Heap[m_iSize--];
	}

	//so we can take a peek at the first in line
	const T& Peek()const{return m_Heap[1];}
};

//--------------------- PriorityQLow -------------------------------------
//
//  basic 2-way heap based priority queue implementation. This time the priority
//  is given to the lowest valued key
//------------------------------------------------------------------------


template<class T>
class PriorityQLow
{
public:
	typedef bool (*JudgeBiggerEqual) (const T& a, const T& b);

private:
	JudgeBiggerEqual	m_funcJudgeBiggerEqual;
	AIRVector<T>		m_Heap;
	int					m_iSize;
	int					m_iMaxSize;

	//given a heap and a node in the heap, this function moves upwards
	//through the heap swapping elements until the heap is ordered
	void ReorderUpwards(AIRVector<T>& heap, int nd)
	{
		//move up the heap swapping the elements until the heap is ordered
		while ( (nd>1) && (heap[nd/2] >= heap[nd]))
		{
			Swap(heap[nd/2], heap[nd]);
			nd /= 2;
		}
	}

	void ReorderUpwardsWithJFunc(AIRVector<T>& heap, int nd)
	{
		T a = heap[nd/2];
		T b = heap[nd];
		while ( (nd>1) && m_funcJudgeBiggerEqual( (a), (b) ) )
		{
			Swap(heap[nd/2], heap[nd]);
			nd /= 2;

			a = heap[nd/2];
			b = heap[nd];
		}
	}

	//given a heap, the heapsize and a node in the heap, this function
	//reorders the elements in a top down fashion by moving down the heap
	//and swapping the current node with the smaller of its two children
	//(provided a child is larger than the current node)
	void ReorderDownwards(AIRVector<T>& heap, int nd, int HeapSize)
	{
		//move down the heap from node nd swapping the elements until
		//the heap is reordered
		while (2*nd <= HeapSize)
		{
			int child = 2 * nd;
			//set child to largest of nd's two children
			if ( (child < HeapSize) && (heap[child] >= heap[child+1]) )
			{
				++child;
			}
			//if this nd is smaller than its child, swap
			if (heap[nd] >= heap[child])
			{
				Swap(heap[child], heap[nd]);
				//move the current node down the tree
				nd = child;
			}
			else
			{
				break;
			}
		}
	}

	void ReorderDownwardsWithJFunc(AIRVector<T>& heap, int nd, int HeapSize)
	{
		while (2*nd <= HeapSize)
		{
			int child = 2 * nd;
			//set child to largest of nd's two children
			if ( (child < HeapSize) && m_funcJudgeBiggerEqual( (heap[child]), (heap[child+1]) ) )
			{
				++child;
			}
			//if this nd is smaller than its child, swap
			if ( m_funcJudgeBiggerEqual( (heap[nd]), (heap[child]) ) )
			{
				Swap(heap[child], heap[nd]);
				//move the current node down the tree
				nd = child;
			}
			else
			{
				break;
			}
		}
	}

public:
	PriorityQLow(int MaxSize):m_iMaxSize(MaxSize), m_iSize(0)
	{
		m_funcJudgeBiggerEqual = NULL;
		m_Heap.assign(MaxSize+1, T());
	}

	void setFuncJudgeBigger( void* func )
	{
		m_funcJudgeBiggerEqual = (JudgeBiggerEqual)func;
	}

	void resize( int MaxSize)
	{
		if ( MaxSize > m_iMaxSize )
		{
			m_iMaxSize = MaxSize;
		}
		m_iSize = 0;
		m_Heap.assign(MaxSize+1, T());
	}

	bool empty()const{return (m_iSize==0);}

	//to insert an item into the queue it gets added to the end of the heap
	//and then the heap is reordered
	void insert(const T item)
	{
		assert (m_iSize+1 <= m_iMaxSize);

		++m_iSize;

		m_Heap[m_iSize] = item;

		if ( m_funcJudgeBiggerEqual )
		{
			ReorderUpwardsWithJFunc(m_Heap, m_iSize);
		}
		else
		{
			ReorderUpwards(m_Heap, m_iSize);
		}
	}

	//to get the max item the first element is exchanged with the lowest
	//in the heap and then the heap is reordered from the top down. 
	T pop()
	{
		Swap(m_Heap[1], m_Heap[m_iSize]);
		if ( m_funcJudgeBiggerEqual )
		{
			ReorderDownwardsWithJFunc(m_Heap, 1, m_iSize-1);
		}
		else
		{
			ReorderDownwards(m_Heap, 1, m_iSize-1);
		}
		return m_Heap[m_iSize--];
	}

	//so we can take a peek at the first in line
	const T& peek()const
	{
		return m_Heap[1];
	}

	size_t size() const 
	{ 
		return m_iSize; 
	}

	void clear() 
	{ 
		m_Heap.clear(); 
		resize( m_iMaxSize );
	}
};

//----------------------- IndexedPriorityQLow ---------------------------
//
//  Priority queue based on an index into a set of keys. The queue is
//  maintained as a 2-way heap.
//
//  The priority in this implementation is the lowest valued key
//------------------------------------------------------------------------
template<class KeyType>
class IndexedPriorityQLow
{
private:

	AIRVector<KeyType>&  m_vecKeys;

	AIRVector<int>       m_Heap;

	AIRVector<int>       m_invHeap;

	int                    m_iSize,
		m_iMaxSize;

	void Swap(int a, int b)
	{
		int temp = m_Heap[a]; m_Heap[a] = m_Heap[b]; m_Heap[b] = temp;

		//change the handles too
		m_invHeap[m_Heap[a]] = a; m_invHeap[m_Heap[b]] = b;
	}

	void ReorderUpwards(int nd)
	{
		//move up the heap swapping the elements until the heap is ordered
		while ( (nd>1) && (m_vecKeys[m_Heap[nd/2]] > m_vecKeys[m_Heap[nd]]) )
		{      
			Swap(nd/2, nd);

			nd /= 2;
		}
	}

	void ReorderDownwards(int nd, int HeapSize)
	{
		//move down the heap from node nd swapping the elements until
		//the heap is reordered
		while (2*nd <= HeapSize)
		{
			int child = 2 * nd;

			//set child to smaller of nd's two children
			if ((child < HeapSize) && (m_vecKeys[m_Heap[child]] > m_vecKeys[m_Heap[child+1]]))
			{
				++child;
			}

			//if this nd is larger than its child, swap
			if (m_vecKeys[m_Heap[nd]] > m_vecKeys[m_Heap[child]])
			{
				Swap(child, nd);

				//move the current node down the tree
				nd = child;
			}

			else
			{
				break;
			}
		}
	}


public:

	//you must pass the constructor a reference to the AIRVector the PQ
	//will be indexing into and the maximum size of the queue.
	IndexedPriorityQLow(AIRVector<KeyType>& keys,
		int              MaxSize):m_vecKeys(keys),
		m_iMaxSize(MaxSize),
		m_iSize(0)
	{
		m_Heap.assign(MaxSize+1, 0);
		m_invHeap.assign(MaxSize+1, 0);
	}

	bool empty()const{return (m_iSize==0);}

	//to insert an item into the queue it gets added to the end of the heap
	//and then the heap is reordered from the bottom up.
	void insert(const int idx)
	{
		assert (m_iSize+1 <= m_iMaxSize);

		++m_iSize;

		m_Heap[m_iSize] = idx;

		m_invHeap[idx] = m_iSize;

		ReorderUpwards(m_iSize);
	}

	//to get the min item the first element is exchanged with the lowest
	//in the heap and then the heap is reordered from the top down. 
	int Pop()
	{
		Swap(1, m_iSize);

		ReorderDownwards(1, m_iSize-1);

		return m_Heap[m_iSize--];
	}

	//if the value of one of the client key's changes then call this with 
	//the key's index to adjust the queue accordingly
	void ChangePriority(const int idx)
	{
		ReorderUpwards(m_invHeap[idx]);
	}
};


#endif