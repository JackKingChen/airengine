// ***************************************************************
//  文件: DataBuffer.h
//  日期: 2010-7-21
//  作者: huangjunjie
//  说明: 数据队列管理类
// ***************************************************************

#pragma once

#ifndef __DataBuffer_H__
#define __DataBuffer_H__


#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include "DebugTools.h"


//数据队列管理类
template <const int _max_line, const int _max_size>
class DataBufferTPL
{
public:

	// 加入队列数据
	bool Add( BYTE *data )		
	{
		ASSERT_RET( data, false );
		m_ControlStatus = false;
		
		if( IsFull() ) 
        {
            return false;
        }
		
		memcpy( m_s_ptr, data, _max_size );
		
		NextSptr();
		m_NumData++;
		
		m_ControlStatus = true;
		return true;
	}    
	

	// 从队列中取出数据
	bool Get( BYTE *data )		
	{
		ASSERT_RET( data, false );
		m_ControlStatus = false;
		
		if( IsNull() ) 
			return false;
		
		memcpy( data, m_e_ptr, _max_size );
		
		NextEptr();
		m_NumData--;
		
		m_ControlStatus = true;
		return true;
	}
	
	
	// 获取操作成功结果
	bool CtrlStatus()			
	{
		return m_ControlStatus;
	}
	
	
	// 获得现在的数据大小
	int GetNumber()				
	{
		return m_NumData;
	}

public:

    DataBufferTPL()
	{
		m_NumData	= 0;
		m_start_ptr	= m_DataTeam[0];
		m_end_ptr	= m_DataTeam[_max_line-1];
		m_s_ptr		= m_start_ptr;
		m_e_ptr		= m_start_ptr;
	}
    ~DataBufferTPL()
	{
		m_NumData	= 0;
		m_s_ptr		= m_start_ptr;
		m_e_ptr		= m_start_ptr;
	}

private:

	// 是否队列满
    bool IsFull()		
	{
		ASSERT_RET( m_NumData >=0 && m_NumData <= _max_line, false );
		if( m_NumData == _max_line ) 
			return true; 
		else 
			return false;
	}

	// 是否队列空
    bool IsNull()		
	{
		ASSERT_RET( m_NumData >=0 && m_NumData <= _max_line, false );
		if( m_NumData == 0 )
			return true;
		else
			return false;
	}

	// 头位置增加
    void NextSptr()		
	{
		assert(m_start_ptr);
		assert(m_end_ptr);
		assert(m_s_ptr);
		assert(m_e_ptr);
		m_s_ptr += _max_size;
		if( m_s_ptr > m_end_ptr )
			m_s_ptr = m_start_ptr;
	}

	// 尾位置增加
    void NextEptr()		
	{
		assert(m_start_ptr);
		assert(m_end_ptr);
		assert(m_s_ptr);
		assert(m_e_ptr);
		m_e_ptr += _max_size;
		if( m_e_ptr > m_end_ptr )
			m_e_ptr = m_start_ptr;
	}

private:

    BYTE	m_DataTeam[_max_line][_max_size];	//数据缓冲
	int		m_NumData;							//数据个数
    bool	m_ControlStatus;					//操作结果

    BYTE	*m_start_ptr;						//起始位置
	BYTE	*m_end_ptr;							//结束位置
    BYTE	*m_s_ptr;							//排队起始位置
	BYTE	*m_e_ptr;							//排队结束位置
};




//////////////////////////////////////////////////////////////////////////
// 放到这里了！

//ID自动补位列表模板，用于自动列表，无间空顺序列表。
template <const int _max_count>
class IDListTPL
{
public:
    // 清除重置
    void Reset() 
    {
		for( int i=0; i<_max_count; i++ )
		{
			m_dwList[i] = -1;
		}
		
		m_counter = 0;
		m_nPlayerCounter = 0;
    }

    int MaxSize() const { return _max_count; }

	//( Hotspot )
    int Count()   const { return m_counter; }

	//玩家的数量
	int PlayerCount()	const { return m_nPlayerCounter; }


    const u32 operator[]( int iIndex ) 
	{ 
        ASSERT_MSG( iIndex >= 0 && iIndex < m_counter, "Out of bound!" );

        return m_dwList[ iIndex ]; 
    }


    bool AddNoCheckOnly( u32 dwID, BOOL bPlayer )
    {
        if( m_counter < 0 || m_counter >= _max_count ) 
            return false;

        m_dwList[m_counter] = dwID;
        m_counter++;

		if( bPlayer )
			m_nPlayerCounter++;

        return true;
    }


    bool New( u32 dwID, BOOL bPlayer )
    {
        ASSERT_RET( m_counter >= 0 && m_counter < _max_count, false );
        if (m_counter < 0 || m_counter >= _max_count)
        {
            return false;
        }
        
        //ID 唯一性，不能存在相同ID
        if ( Find( dwID ) != -1 ) 
            return false;

        m_dwList[m_counter] = dwID;
        m_counter++;

		if( bPlayer )
			m_nPlayerCounter++;

        return true;
    }



    // 没有Assert的加入ID功能
    bool Add( u32 dwID, BOOL bPlayer )
    {
        if( m_counter <0 || m_counter >= _max_count ) 
            return false;

        //ID 唯一性，不能存在相同ID
        if ( Find( dwID ) != -1 ) 
            return false;

        m_dwList[m_counter] = dwID;
		m_counter++;

		if( bPlayer )
			m_nPlayerCounter++;

		return true;
    }



    bool Del( int iIndex, BOOL bPlayer )
    {
        ASSERT_RET( iIndex >= 0 && iIndex < m_counter, false );
        if (iIndex < 0 || iIndex >= m_counter)
        {
            return false;
        }

		//简单的将最后的 元素移动到删除点
		if( iIndex < m_counter - 1 && iIndex >= 0)
		{		
			m_dwList[ iIndex ] = m_dwList[ m_counter-1];
		}

		m_dwList[ m_counter-1 ] = -1;
        m_counter--;

		if( bPlayer )
		{
			if( m_nPlayerCounter > 0 )
				m_nPlayerCounter--;
		}

        return true;
    }


    int Find( u32 dwID )
    {
        for( int i=0; i < m_counter; i++ )
        {
			if( m_dwList[i] == dwID ) 
				return i;
        }
        return -1;
    }


    IDListTPL():m_counter(0),m_nPlayerCounter(0)
    {
    }

    virtual ~IDListTPL(){}

private:
    u32	m_dwList[_max_count];
	int		m_nPlayerCounter; //玩家的数量
    int		m_counter;
};





//////////////////////////////////////////////////////////////////////////
template< typename _DataType, const int _MaxCount >
class TDataQueue
{
public:
    TDataQueue()
    {
        m_Counter = 0;
        m_Pointer = 0;
    }
    
    void Reset()
    {
        m_Counter = 0;
        m_Pointer = 0;
    }
    
    void Copy( _DataType &Dst, _DataType &Src )
    {
        memcpy( &Dst, &Src, sizeof(_DataType) );
    }

    void Join( _DataType &Data )
    {
        Copy( m_DataQ[m_Pointer], Data );
        m_Pointer++;
        m_Counter++;
        if( m_Pointer >= _MaxCount )
            m_Pointer = 0;
        if( m_Counter >= _MaxCount )
            m_Counter = _MaxCount;
    }
    
    _DataType *KickOut()
    {
        if( m_Counter <= 0 )
            return NULL;

        m_Pointer--;
        if( m_Pointer < 0 )
            m_Pointer = _MaxCount - 1;
        m_Counter--;
        return &m_DataQ[m_Pointer];
    }


    int Count() { return m_Counter; }

protected:

private:

    // 数据区
    _DataType	m_DataQ[_MaxCount];

    // 当前总数
    int			m_Counter;

    // 当前指向的位置，就是增加时加入到的位置
    int			m_Pointer;
};

#endif //! __DataBuffer_H__