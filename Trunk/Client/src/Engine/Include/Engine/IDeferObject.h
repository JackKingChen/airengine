// ***************************************************************
//  文件: IDeferObject.h
//  日期: 2011-4-9
//  作者: huangjunjie
//  说明: 
// ***************************************************************

#pragma once

#ifndef __IDeferObject_H__
#define __IDeferObject_H__

class IDeferObject
{
public:
    IDeferObject() : m_bPrepared( false ) {}

    virtual ~IDeferObject() {}

    //这里实际上是生成渲染数据
    virtual bool Prepare() = 0;

    bool IsPrepared() const
    {
        return m_bPrepared;
    }

protected:
    bool m_bPrepared;

};//! end class IDeferObject

#endif //! __IDeferObject_H__