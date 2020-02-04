#pragma once

//#include "DetectMemLeak.h"

template<typename T>
class MyVector
{
public:
    MyVector()
    {
        m_pArray = 0;
        m_nSize = 0;
        m_nMemorySize = 0;
    }
    MyVector(unsigned int nSize)
    {
        if (nSize != 0)
        {
            m_pArray = /*new*/AURO_NEW_VERBOSE T[nSize];
        }
        //m_nSize = nSize;
        m_nMemorySize = nSize;
    }
    ~MyVector()
    {
        Clear();
    }

    void Reserve(unsigned int nSize)
    {
        if (m_pArray != 0)
        {
            delete [] m_pArray;
        }
        m_pArray = /*new*/AURO_NEW_VERBOSE T[nSize];
        m_nMemorySize = nSize;
        memset(m_pArray, 0, sizeof(T) * m_nMemorySize);
    }

    void Clear()
    {
        if (m_pArray)
        {
            delete [] m_pArray;
            m_pArray = 0;
			//SAFE_DELETE_ARRAY( m_pArray );
        }
        m_nMemorySize = 0;
        m_nSize = 0;
        m_pArray = 0;
    }

    T& operator [] (int nIndex)
    {
        return m_pArray[nIndex];
    }

    unsigned int Size() const
    {
        return m_nSize;
    }

    void Resize(unsigned int nSize)
    {
        if (m_pArray)
        {
            delete [] m_pArray;
			//SAFE_DELETE_ARRAY( m_pArray );
        }
        m_nSize = nSize;
        m_nMemorySize = nSize;
        m_pArray = new T[m_nMemorySize];
        memset(m_pArray, 0, sizeof(T) * m_nMemorySize);
    }

    void Reset()
    {
        m_nSize = 0;
        memset(m_pArray, 0, sizeof(T) * m_nMemorySize);
    }

    void Pushback(const T& t)
    {
        memcpy(m_pArray + m_nSize, &t, sizeof(T));
        //m_pArray[m_nSize++] = t;
    }

private:
    T* m_pArray;
    unsigned int m_nSize;
    unsigned int m_nMemorySize;
};

template<typename T>
struct MyListNode
{
    MyListNode()
    {
        m_pNext = NULL;
    }
    MyListNode* m_pNext;
    T  m_data;
};

template<typename T>
class MyList
{
public:
    MyList()
    {
        m_pHead = NULL;
        m_pTail = NULL;
        m_nSize = 0;
    }
    ~MyList()
    {
        Clear();
    }

    void Insert(T& pWhere, T& pNode)
    {
        MyListNode<T>* pNewNode = /*new*/AURO_NEW_VERBOSE MyListNode<T>;
        pNode.m_pNext = pWhere.m_pNext;
        pWhere.m_pNext = &pNode;
        m_nSize++;
    }

    void PushBack(T& pNode)
    {
        MyListNode<T>* pNewNode = /*new*/AURO_NEW_VERBOSE MyListNode<T>;
        pNewNode->m_data = pNode;
        if (m_pHead == NULL)
        {
            
            
            m_pHead = pNewNode;
            m_pTail = m_pHead;
        }
        else
        {
            m_pTail->m_pNext = pNewNode;
            m_pTail = pNewNode;
        }

        m_nSize++;

    }

    void Erase(T& pWhere)
    {
        MyListNode<T>* pCur = m_pHead;
        MyListNode<T>* pLast = NULL;
        while (pCur->m_pNext != NULL)
        {
            if (pCur == pWhere)
            {
                pLast->m_pNext = pCur->m_pNext;
                delete pCur;
				//SAFE_DELETE_ATOM( pCur );
                m_nSize--;
                return;
            }
            else
            {
                pLast = pCur;
                pCur = pCur->m_pNext;
            }
        }
    }

    void Clear()
    {
        while (m_pHead != NULL)
        {
            MyListNode<T>* pCur = m_pHead;
            m_pHead = pCur->m_pNext;
            delete pCur;
			//SAFE_DELETE_ATOM( pCur );
            m_nSize--;
        }
        m_pHead = m_pTail = NULL;
    }

    int Size() { return m_nSize; }

    T& GetHead() 
    { 
        if (m_pHead == NULL)
        {
            return 0;
        }
        return m_pHead->m_data;
    }

private:
    MyListNode<T>* m_pHead;
    MyListNode<T>* m_pTail;

    int m_nSize;
};

