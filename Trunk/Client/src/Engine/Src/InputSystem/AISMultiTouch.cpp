//
//  AISMultiTouch.cpp
//  iosInputDemo
//
//  Created by hjj on 13-6-17.
//
//
#include "stdafx.h"
#include "AISMultiTouch.h"

using namespace AIS;


MultiTouch::MultiTouch(const std::string &vendor, bool buffered, int devID, InputManager* creator)
: DeviceObject(vendor, AISMultiTouch, buffered, devID, creator), mListener(0)
{
    resetTouchRecords();
}


TouchRecord* MultiTouch::GetUnusedTouch()
{
    TouchRecord* pTouch = NULL;
    for ( int i = 0; i < MAX_TOUCHES; ++i )
    {
        if ( mValidTouchList[i].m_TouchObjID == -1 && mValidTouchList[i].m_touchType == MT_None )
        {
            pTouch = &mValidTouchList[i];
            break;
        }
    }
    return pTouch;
}

TouchRecord* MultiTouch::IsTouchExist( int touchObjID )
{
    TouchRecord* pTouch = NULL;
    for ( int i = 0; i < MAX_TOUCHES; ++i )
    {
        if ( mValidTouchList[i].m_TouchObjID == touchObjID )
        {
            pTouch = &mValidTouchList[i];
            break;
        }
    }
    return pTouch;
}


bool MultiTouch::handleTouchesBegan( int touchCound, int ids[], float posxs[], float posys[] )
{
    std::cout<<touchCound<<" touch began\n";
    MultiTouchStateVec stateList;
    
    int touchType = MT_Pressed;
    for ( int i = 0; i < touchCound; ++i )
    {
        int touchObjID = ids[i];
        int posx = posxs[i];
        int posy = posys[i];
        
        TouchRecord* pTouch = IsTouchExist( touchObjID );
        if ( !pTouch )
		{
			pTouch = GetUnusedTouch();
		}	
		if ( pTouch )
		{
			pTouch->m_point.Set( posx, posy );
			pTouch->m_lastPt.Set( posx, posy );
			pTouch->m_TouchObjID = touchObjID;
			pTouch->m_touchType = touchType;
			stateList.push_back( pTouch );
		}
    }
    
    if ( mListener && mBuffered && stateList.size() )
    {
        return mListener->touchesPressed( MultiTouchEvent(ET_MultiTouchesPress, this, stateList) );
    }
    
    return true;
}

bool MultiTouch::handleTouchesMoved( int touchCound, int ids[], float posxs[], float posys[] )
{
    std::cout<<touchCound<<" touch moved\n";
    MultiTouchStateVec stateList;
    
    int touchType = MT_Moved;
    for ( int i = 0; i < touchCound; ++i )
    {
        int touchObjID = ids[i];
        int posx = posxs[i];
        int posy = posys[i];
        
        TouchRecord* pTouch = IsTouchExist( touchObjID );
        if ( pTouch )
        {
            pTouch->m_lastPt = pTouch->m_point;
            pTouch->m_point.Set( posx, posy );
            pTouch->m_touchType = touchType;
            stateList.push_back( pTouch );
        }
    }
    
    if ( mListener && mBuffered && stateList.size() )
    {
        return mListener->touchesMoved( MultiTouchEvent(ET_MultiTouchesMove, this, stateList) );
    }
    
    return true;
}

bool MultiTouch::handleTouchesEnded( int touchCound, int ids[], float posxs[], float posys[] )
{
    std::cout<<touchCound<<" touch ended\n";
    MultiTouchStateVec stateList;
    
    int touchType = MT_Released;
    for ( int i = 0; i < touchCound; ++i )
    {
        int touchObjID = ids[i];
        int posx = posxs[i];
        int posy = posys[i];
        
        TouchRecord* pTouch = IsTouchExist( touchObjID );
        if ( pTouch )
        {
            pTouch->m_lastPt = pTouch->m_point;
            pTouch->m_point.Set( posx, posy );
            pTouch->m_touchType = touchType;
            stateList.push_back( pTouch );
        }
    }
    
	bool res = true;
    if ( mListener && mBuffered && stateList.size() )
    {
        res = mListener->touchesReleased( MultiTouchEvent(ET_MultiTouchesRelease, this, stateList) );
    }
    
    for ( int i = 0; i < stateList.size(); ++i )
    {
        stateList[i]->m_TouchObjID = -1;
		stateList[0]->m_touchType = MT_None;
    }
    
    return res;
}

bool MultiTouch::handleTouchesCancelled( int touchCound, int ids[], float posxs[], float posys[] )
{
    MultiTouchStateVec stateList;
    
    int touchType = MT_Cancelled;
    for ( int i = 0; i < touchCound; ++i )
    {
        int touchObjID = ids[i];
        int posx = posxs[i];
        int posy = posys[i];
        
        TouchRecord* pTouch = IsTouchExist( touchObjID );
        if ( pTouch )
        {
            pTouch->m_lastPt = pTouch->m_point;
            pTouch->m_point.Set( posx, posy );
            pTouch->m_touchType = touchType;
            stateList.push_back( pTouch );
        }
    }
    
	bool res = true;
    if ( mListener && mBuffered && stateList.size() )
    {
        res = mListener->touchesCancelled( MultiTouchEvent(ET_MultiTouchesCancle, this, stateList) );
    }
    
    for ( int i = 0; i < stateList.size(); ++i )
    {
        stateList[i]->m_TouchObjID = -1;
		stateList[i]->m_touchType = MT_None;
    }
    
    return res;
}

void MultiTouch::resetTouchRecords()
{
    for ( int i = 0; i < MAX_TOUCHES; ++i )
    {
        mValidTouchList[i].m_TouchObjID = -1;
        mValidTouchList[i].m_touchType = MT_None;
		mValidTouchList[i].m_touchID = i;
    }
}


void MultiTouch::SetViewSize( int w, int h )
{
	mWidth = w;
	mHeight = h;
}

void MultiTouch::GetViewSize( int& w, int &h )
{
	w = mWidth;
	h = mHeight;
}

void MultiTouch::GetValidTouches(MultiTouchStateVec& validTouches)
{
	validTouches.clear();
	for ( int i = 0; i < MAX_TOUCHES; ++i )
	{
		if( mValidTouchList[i].m_touchType != MT_None && mValidTouchList[i].m_TouchObjID != -1 )
		{
			validTouches.push_back( &mValidTouchList[i] );
		}
	}
}