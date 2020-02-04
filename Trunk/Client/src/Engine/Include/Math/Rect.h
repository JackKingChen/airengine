#pragma once
#include "AIRTypes.h"
#include "MyMath.h"
#include "Vector2D.h"

namespace AIR
{

	enum RC_RELATION
	{
		RR_DES_INTERSECT_SELF	= 0x1,	// 目标矩形与自身相交
		RR_DES_INSIDE_SELF		= 0x2,	// 目标矩形在自身里面
		RR_DES_OUTSIDE_SELF		= 0x3,	// 目标矩形在自身外面
		RR_SELF_INSID_DES		= 0x4	// 自身在目标矩形里面
	};

    class EngineExport Rect
    {
    public:
        inline Rect()
        {
            left = 0;
            top = 0;
            right = 0;
            bottom = 0;
        }

        inline Rect(f32 topLeftX, f32 topLeftY, f32 bottomRightX, f32 bottomRightY)
        {
            left = topLeftX;
            top = topLeftY;
            right = bottomRightX;
            bottom = bottomRightY;
        }

		inline Rect( const Vector2D& vTopLeft, const Vector2D& vBottomRight )
		{
			left = vTopLeft.x;
			top = vTopLeft.y;
			right = vBottomRight.x;
			bottom = vBottomRight.y;
		}

		inline Rect( const Rect& other )
		{
			left = other.left;
			top = other.top;
			right = other.right;
			bottom = other.bottom;
		}

        ~Rect() 
        {

        }

		inline void Set(f32 topLeftX, f32 topLeftY, f32 bottomRightX, f32 bottomRightY)
		{
			left = topLeftX;
			top = topLeftY;
			right = bottomRightX;
			bottom = bottomRightY;
		}

        bool IsInRect(f32 x, f32 y) const
        {
            if (x >= left && x <= right && y >= top && y <= bottom)
            {
                return true;
            }
            return false;
        }

		void Normalize( void )
		{
			f32 tmp;
			if ( left > right )
			{
				tmp = left;
				left = right;
				right = tmp;
			}
			if ( top > bottom )
			{
				tmp = top;
				top = bottom;
				bottom = tmp;
			}
		}

        f32 GetWidth() const { return right - left; }
        f32 GetHeight() const { return bottom - top; }

		f32 GetLeft() const { return left; }
		f32 GetTop() const { return top; }
		f32 GetRight() const { return right; }
		f32 GetBottom() const { return bottom; }

        void  SetRect(f32 topLeftX, f32 topLeftY, f32 bottomRightX, f32 bottomRightY)
        {
            left = topLeftX;
            top = topLeftY;
            right = bottomRightX;
            bottom = bottomRightY;
        }

        void Scale(float fScaleW, float fScaleH)
        {
            left *= fScaleW;
            top *= fScaleH;
            right *= fScaleW;
            bottom *= fScaleH;
        }

        void Move( f32 offsetX, f32 offsetY )
        {
            left   += offsetX;
            top    += offsetY;
            right  += offsetX;
            bottom += offsetY;
        }

        void MoveTo( f32 posX, f32 posY )
        {
            f32 offsetX = posX - left;
            f32 offsetY = posY - top;
            Move( offsetX, offsetY );
        }

		void MoveCenterTo( f32 centerX, f32 centerY )
		{
			f32 oldCenterX, oldCenterY;
			GetCenter( oldCenterX, oldCenterY );
			f32 offX = centerX - oldCenterX;
			f32 offY = centerY - oldCenterY;
			Move( offX, offY );
		}

        bool operator == (const Rect rect) const
        {
            return left == rect.left && right == rect.right && top == rect.top && bottom == rect.bottom;
        }

        //判断矩阵是否相交
        bool IsRectInter(const Rect& other) const
        {
            return IsInRect(other.left, other.top) || IsInRect(other.right, other.bottom) || IsInRect(other.left, other.bottom) || IsInRect(other.right, other.top);
        }

		bool RangeInterset( f32 desTL, f32 desBR, bool TL )
		{
			if( TL )
			{
				bool b = desTL < left;
				if ( b )
				{
					return (desTL <= left && desBR >= right);
				}
				else
				{
					return (left <= desTL && right >= desBR);
				}
			}
			else
			{
				bool b = desTL < top;
				if ( b )
				{
					return (desTL <= top && desBR >= bottom);
				}
				else
				{
					return (top <= desTL && bottom >= desBR);
				}
			}
		}

		RC_RELATION GetRectRelation( const Rect& des ) const
		{
			int count = 0;
			if ( IsInRect(des.left,  des.top) )	    
			{ 
				count++;
			}
			if ( IsInRect(des.right, des.bottom) )	
			{ 
				count++;
			}
			if ( IsInRect(des.left,  des.bottom) )	
			{ 
				count++;
			}
			if ( IsInRect(des.right, des.top) )	    
			{ 
				count++;
			}
			// 如果4个顶点都在自身里面
			if ( count == 4 )
			{
				// 表示目标矩形被包含在自身里面
				return RR_DES_INSIDE_SELF;
			}
			// 如果有1个以上顶点在自身里面
			else if( count >= 1 )
			{
				// 表示目标矩形与自身相交
				return RR_DES_INTERSECT_SELF;
			}

			// 如果自身与目标矩形相交
			if ( des.IsRectInter( *this ) )
			{
				// 表明自身被包含在目标矩形里面
				return RR_SELF_INSID_DES;
			}

			bool bRangeInterset;
			bool b = des.left < left;
			if ( b )
			{
				bRangeInterset = (des.left <= left && des.right >= right);
			}
			else
			{
				bRangeInterset = (left <= des.left && right >= des.right);
			}
			if ( bRangeInterset )
			{
				b = des.top < top;
				if ( b )
				{
					bRangeInterset = (des.top <= top && des.bottom >= bottom);
				}
				else
				{
					bRangeInterset = (top <= des.top && bottom >= des.bottom);
				}
				if ( bRangeInterset )
				{
					return RR_SELF_INSID_DES;
				}
			}

			// 否则目标矩形与自身想离
			return RR_DES_OUTSIDE_SELF;
		}

        void GetCenter(f32& x, f32& y)
        {
            x = (left + right) / 2;
            y = (top + bottom) / 2;
        }

		void SuitRect( Rect& dst )
		{
			f32 dx = 0, dy = 0;
			if ( dst.left < left )
			{
				dx = left - dst.left;
			}
			else if ( dst.right > right )
			{
				dx = right - dst.right;
			}

			if ( dst.top < top )
			{
				dy = top - dst.top;
			}
			else if ( dst.bottom > bottom )
			{
				dy = bottom - dst.bottom;
			}

			dst.Move( dx, dy );
		}

		void Expand( f32 DelX, f32 DelY )
		{
			left   -= DelX;
			right  += DelY;
			top    -= DelX;
			bottom += DelY;
			Normalize();
		}

		inline bool IsCross( const Rect& rc )
		{
			if (right < rc.left || left > rc.right || top > rc.bottom || bottom < rc.top )
			{
				return false;
			}
			return true;
		}

		inline bool IsIn( const Vector2D& pt )
		{
			if (pt.x < left || pt.x > right || pt.y < top || pt.y > bottom )
			{
				return false;
			}
			return true;
		}

    public:
        f32 left;
        f32 top;
        f32 right;
        f32 bottom;

    public:
        static const Rect ZERO;
    };

    typedef Rect Rectf;

    //const Rectf Rectf::ZERO;
    //const Rect Rect::ZERO;
	
};




