// ***************************************************************
//  �ļ�: DataBuffer.h
//  ����: 2010-7-21
//  ����: huangjunjie
//  ˵��: ���ݶ��й�����
// ***************************************************************

#pragma once

#ifndef __DataBuffer_H__
#define __DataBuffer_H__


#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include "DebugTools.h"


//���ݶ��й�����
template <const int _max_line, const int _max_size>
class DataBufferTPL
{
public:

	// �����������
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
	

	// �Ӷ�����ȡ������
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
	
	
	// ��ȡ�����ɹ����
	bool CtrlStatus()			
	{
		return m_ControlStatus;
	}
	
	
	// ������ڵ����ݴ�С
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

	// �Ƿ������
    bool IsFull()		
	{
		ASSERT_RET( m_NumData >=0 && m_NumData <= _max_line, false );
		if( m_NumData == _max_line ) 
			return true; 
		else 
			return false;
	}

	// �Ƿ���п�
    bool IsNull()		
	{
		ASSERT_RET( m_NumData >=0 && m_NumData <= _max_line, false );
		if( m_NumData == 0 )
			return true;
		else
			return false;
	}

	// ͷλ������
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

	// βλ������
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

    BYTE	m_DataTeam[_max_line][_max_size];	//���ݻ���
	int		m_NumData;							//���ݸ���
    bool	m_ControlStatus;					//�������

    BYTE	*m_start_ptr;						//��ʼλ��
	BYTE	*m_end_ptr;							//����λ��
    BYTE	*m_s_ptr;							//�Ŷ���ʼλ��
	BYTE	*m_e_ptr;							//�Ŷӽ���λ��
};




//////////////////////////////////////////////////////////////////////////
// �ŵ������ˣ�

//ID�Զ���λ�б�ģ�壬�����Զ��б��޼��˳���б�
template <const int _max_count>
class IDListTPL
{
public:
    // �������
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

	//��ҵ�����
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
        
        //ID Ψһ�ԣ����ܴ�����ͬID
        if ( Find( dwID ) != -1 ) 
            return false;

        m_dwList[m_counter] = dwID;
        m_counter++;

		if( bPlayer )
			m_nPlayerCounter++;

        return true;
    }



    // û��Assert�ļ���ID����
    bool Add( u32 dwID, BOOL bPlayer )
    {
        if( m_counter <0 || m_counter >= _max_count ) 
            return false;

        //ID Ψһ�ԣ����ܴ�����ͬID
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

		//�򵥵Ľ����� Ԫ���ƶ���ɾ����
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
	int		m_nPlayerCounter; //��ҵ�����
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

    // ������
    _DataType	m_DataQ[_MaxCount];

    // ��ǰ����
    int			m_Counter;

    // ��ǰָ���λ�ã���������ʱ���뵽��λ��
    int			m_Pointer;
};

#endif //! __DataBuffer_H__