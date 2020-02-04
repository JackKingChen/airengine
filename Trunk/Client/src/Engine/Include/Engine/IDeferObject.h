// ***************************************************************
//  �ļ�: IDeferObject.h
//  ����: 2011-4-9
//  ����: huangjunjie
//  ˵��: 
// ***************************************************************

#pragma once

#ifndef __IDeferObject_H__
#define __IDeferObject_H__

class IDeferObject
{
public:
    IDeferObject() : m_bPrepared( false ) {}

    virtual ~IDeferObject() {}

    //����ʵ������������Ⱦ����
    virtual bool Prepare() = 0;

    bool IsPrepared() const
    {
        return m_bPrepared;
    }

protected:
    bool m_bPrepared;

};//! end class IDeferObject

#endif //! __IDeferObject_H__