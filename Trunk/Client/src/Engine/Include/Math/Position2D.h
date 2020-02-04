#pragma once

namespace AIR
{
    template<class T>
    class Position2D
    {
    public:
        Position2D() : X(0), Y(0) {}
        Position2D(T x, T y) : X(x), Y(y) {}
        Position2D(const Position2D<T>& other)
            : X(other.X), Y(other.Y) {}

        bool operator == (const Position2D<T>& other) const
        {
            return X == other.X && Y == other.Y;
        }

        bool operator != (const Position2D<T>& other) const
        {
            return X != other.X || Y != other.Y;
        }

        const Position2D<T>& operator+=(const Position2D<T>& other)
        {
            X += other.X;
            Y += other.Y;
            return *this;
        }

        const Position2D<T>& operator -= (const Position2D<T>& other)
        {
            X -= other.X;
            Y -= other.Y;
            return *this;
        }

        const Position2D<T>& operator += (const Dimension2D<T>& other)
        {
            X += other.Width;
            Y += other.Height;
            return *this;
        }

        const Position2D<T>& operator -= (const Dimension2D<T>& other)
        {
            X -= other.Width;
            Y -= other.Height;
            return *this;
        }

        Position2D<T> operator - (const Position2D<T>& other) const
        {
            return Position2D<T>(X - other.X, Y - other.Y);
        }

        Position2D<T> operator + (const Position2D<T>& other) const
        {
            return Position2D<T>(X+other.X, Y+other.Y);
        }

        Position2D<T> operator * (const Position2D<T>& other) const
        {
            return Position2D<T>(X * other.X, Y * other.Y);
        }

        Position2D<T> operator * (const T& scalar) const
        {
            return Position2D<T>(X * scalar, Y*scalar);
        }

        Position2D<T> operator+(const Dimension2D<T>& other) const
        {
            return Position2D<T>(X + other.Width, Y + other.Height);
        }

        Position2D<T> operator-(const Dimension2D<T>& other) const
        {
            return Position2D<T>(X - other.Width, Y - other.Height);
        }

        const Position2D<T>& operator=(const Position2D<T>& other)
        {
            X = other.X;
            Y = other.Y;
            return *this;
        }

        //! X coordinate of the position.
        T X;
        //! Y coordinate of the position.
        T Y;
    };
}