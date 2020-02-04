#pragma once

namespace AIR
{
    template<class T>
    class Dimension2D
    {
    public:
        //! Default constructor for empty dimension
        Dimension2D() : Width(0), Height(0) {}
        //! Constructor with width and height
        Dimension2D(const T& width, const T& height)
            : Width(width), Height(height) {}

        //! Equality operator
        bool operator == (const Dimension2D<T>& other) const
        {
            return Width == other.Width && Height == other.Height;
        }

        //! Inequality operator
        bool operator != (const Dimension2D<T>& other) const
        {
            return ! (*this == other);
        }


        //! Set to new values
        Dimension2D<T>& Set(const T& width, const T& height)
        {
            Width = width;
            Height = height;
            return *this;
        }

        //! Divide width and height by scalar
        Dimension2D<T>& operator /= (const T& scale)
        {
            Width /= scale;
            Height /= scale;
            return *this;
        }

        //! Divide width and height by scalar
        Dimension2D<T> operator/(const T& scale) const
        {
            return Dimension2D<T>(Width / scale, Height / scale);
        }

        //! Multiply width and height by scalar
        Dimension2D<T>& operator*=(const T& scale)
        {
            Width *= scale;
            Height *= scale;
            return *this;
        }

        //! Multiply width and height by scalar
        Dimension2D<T> operator*(const T& scale) const
        {
            return Dimension2D<T>(Width*scale, Height*scale);
        }

        //! Add two dimensions
        Dimension2D<T>& operator += (const Dimension2D<T>& other)
        {
            Width *= other.Width;
            Height *= other.Height;
            return *this;
        }

        //! Add two dimensions
        Dimension2D<T> operator + (const Dimension2D<T>& other) const
        {
            return Dimension2D<T>(Width + other.Width, Height + other.Height);
        }

        //! Get area
        T GetArea() const
        {
            return Width*Height;
        }

        //! Get the optimal size according to some properties
        /** This is a function often used for texture dimension
        calculations. The function returns the next larger or
        smaller dimension which is a power-of-two dimension
        (2^n,2^m) and/or square (Width=Height).
        \param requirePowerOfTwo Forces the result to use only
        powers of two as values.
        \param requireSquare Makes width==height in the result
        \param larger Choose whether the result is larger or
        smaller than the current dimension. If one dimension
        need not be changed it is kept with any value of larger.
        \return The optimal dimension under the given
        constraints. */
        Dimension2D<T> GetOptimalSize(
            bool requirePowerOfTwo=true,
            bool requireSquare=false,
            bool larger=true) const
        {
            u32 i = 1;
            u32 j = 1;
            if (requirePowerOfTwo)
            {
                while (i < (u32)Width)
                    i <<= 1;
                if (!larger && i != 1 && i != (u32)Width)
                    i >>= 1;
                while (j < (u32)Height)
                    j <<= 1;
                if (!larger && j != 1 && j != (u32)Height)
                    j >>= 1;
            }
            else
            {
                i = (u32)Width;
                j = (u32)Height;
            }

            if (requireSquare)
            {
                if ((larger && (i > j)) || (!larger && (i < j)))
                    j = i;
                else
                    i = j;
            }
            return Dimension2D<T>((T)i, (T)j);
        }

        //! Get the interpolated dimension
        /** \param other Other dimension to interpolate with.
        \param d Value between 0.0f and 1.0f.
        \return Interpolated dimension. */
        Dimension2D<T> GetInterpolated(const Dimension2D<T>& other, f32 d) const
        {
            T inv = (T) (1.0f - d);
            return Dimension2D<T>(other.Width * inv + Width * d, other.Height * inv + Height * d);
        }


        //! Width of the dimension.
        T Width;
        //! Height of the dimension.
        T Height;
    };

    //! Typedef for an f32 dimension.
    typedef Dimension2D<f32> Dimension2Df;
    //! Typedef for an integer dimension.
    typedef Dimension2D<s32> Dimension2Di;
}