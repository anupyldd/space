#pragma once

#include <initializer_list>
#include <algorithm>
#include <cstdint>
#include <math.h>

// API ---------------------------------

namespace math
{
    template<class T> struct Vector2;
    template<class T> struct Vector3;
    template<class T> struct Vector4;

    template<class T, size_t S> struct Matrix;
    template<class T> using Matrix2 = Matrix<T, 2>;
    template<class T> using Matrix3 = Matrix<T, 3>;

    template<class T> struct Triangle;
    template<class T> struct Rectangle;
    template<class T> struct Circle;

    template<class T> inline T Sqr(T a);

    // average
    template<class T> inline float Avg(T a, T b);
    template<class T> inline float Avg(T a, T b, T c);
    template<class T> inline float Avg(std::initializer_list<T> ls);
    template<class T> inline Vector2<float> Avg(const Vector2<T>& v1, const Vector2<T>& v2);

    // distance
    template<class T> inline float Distance(const Vector2<T>& p1, const Vector2<T>& p2);
    template<class T> inline float DistanceSq(const Vector2<T>& p1, const Vector2<T>& p2);

    // dot product
    template<class T> inline float Dot(const Vector2<T>& v1, const Vector2<T>& v2);
    template<class T> inline float Dot(const Vector3<T>& v1, const Vector3<T>& v2);
    template<class T> inline float Dot(const Vector4<T>& v1, const Vector4<T>& v2);

    // angles
    inline float DegToRad(float d);
    inline float RadToDeg(float r);
}

// -------------------------------------

namespace math
{
    namespace itrn
    {

    }

    template<class T>
    struct Vector2
    {
        T x, y;

    public:
        Vector2() = default;
        Vector2(T x, T y) : x(x), y(y) { }
        Vector2(T val) : x(val), y(val) { }
        Vector2(const Vector2& src) : x(src.x), y(src.y) { }

        T LengthSq() { return Sqr(x) + Sqr(y); }
        T Length() { return sqrt(LengthSq()); }

        // normalize in place
        Vector2& Normalize()
        {
            const T len = Length();
            x /= len;
            y /= len;
            return *this;
        }
        // get normalized without changine the vector
        Vector2 GetNormalized()
        {
            Vector2 vec;
            vec.Normalize();
            return vec;
        }

        T Dot(const Vector2& rhs) const { return x * rhs.x + y * rhs.y; }

    public:

        template<class NT>
        explicit operator Vector2<NT>() const { return { static_cast<NT>(x), static_cast<NT>(y) }; }

        Vector2& operator = (const Vector2& rhs) { x = rhs.x; y = rhs.y; return *this; }

        Vector2 operator - () const { return Vector2(-x, -y); }
        
        Vector2& operator += (const Vector2& rhs) { x += rhs.x; y += rhs.y; return *this; }
        Vector2& operator += (T rhs) { x += rhs; y += rhs; return *this; }
        Vector2 operator + (const T& rhs) { return Vector2(*this) += rhs; }

        Vector2& operator -= (const Vector2& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
        Vector2& operator -= (T rhs) { x -= rhs; y -= rhs; return *this; }
        Vector2 operator - (const T& rhs) { return Vector2(*this) -= rhs; }

        Vector2& operator *= (const Vector2& rhs) { x *= rhs.x; y *= rhs.y; return *this; }
        Vector2& operator *= (T rhs) { x *= rhs; y *= rhs; return *this; }
        Vector2 operator * (const T& rhs) { return Vector2(*this) *= rhs; }

        Vector2& operator /= (const Vector2& rhs) { x /= rhs.x; y /= rhs.y; return *this; }
        Vector2& operator /= (T rhs) { x /= rhs; y /= rhs; return *this; }
        Vector2 operator / (const T& rhs) { return Vector2(*this) /= rhs; }

        bool operator == (const Vector2& rhs) { return x == rhs.x && y == rhs.y; }
        bool operator != (const Vector2& rhs) { return !(*this == rhs); }
    };

    template<class T>
    struct Vector3 : public Vector2<T>
    {
        T z;

    public:
        Vector3() = default;
        Vector3(T x, T y, T z) : Vector2<T>(x, y), z(z) { }
        Vector3(T val) : Vector2<T>(val), z(val) { }

        T LengthSq() { return Sqr(this->x) + Sqr(this->y) + Sqr(z); }
        T Length() { return sqrt(LengthSq()); }

        // normalize in place
        Vector3& Normalize()
        {
            const T len = Length();
            this->x /= len;
            this->y /= len;
            z /= len;
            return *this;
        }
        // get normalized without changine the vector
        Vector3 GetNormalized()
        {
            Vector3 vec;
            vec.Normalize();
            return vec;
        }

        T Dot(const Vector3& rhs) const { return this->x * rhs.x + this->y * rhs.y + z * rhs.z; }
        Vector3 Cross(const Vector3& rhs) const
        {
            return Vector3(
                this->y * rhs.z - z * rhs.y,
                z * rhs.x - this->x * rhs.z, 
                this->x * rhs.y - this->y * rhs.x);
        }

    public:
        template<class NT>
        explicit operator Vector3<NT>() const { return { static_cast<NT>(this->x), static_cast<NT>(this->y), static_cast<NT>(z)}; }

        Vector3& operator = (const Vector3& rhs) { this->x = rhs.x; this->y = rhs.y; z = rhs.z; return *this; }

        Vector3 operator - () const { return Vector3(-(this->x), -(this->y), -z); }

        Vector3& operator += (const Vector3& rhs) { this->x += rhs.x; this->y += rhs.y; z += rhs.z; return *this; }
        Vector3& operator += (T rhs) { this->x += rhs; this->y += rhs; z += rhs; return *this; }
        Vector3 operator + (const T& rhs) { return Vector3(*this) += rhs; }

        Vector3& operator -= (const Vector3& rhs) { this->x -= rhs.x; this->y -= rhs.y; z -= rhs.z; return *this; }
        Vector3& operator -= (T rhs) { this->x -= rhs; this->y -= rhs; z -= rhs; return *this; }
        Vector3 operator - (const T& rhs) { return Vector3(*this) -= rhs; }

        Vector3& operator *= (const Vector3& rhs) { this->x *= rhs.x; this->y *= rhs.y; z *= rhs.z; return *this; }
        Vector3& operator *= (T rhs) { this->x *= rhs; this->y *= rhs; z *= rhs; return *this; }
        Vector3 operator * (const T& rhs) { return Vector3(*this) *= rhs; }

        Vector3& operator /= (const Vector3& rhs) { this->x /= rhs.x; this->y /= rhs.y; z /= rhs.z; return *this; }
        Vector3& operator /= (T rhs) { this->x /= rhs; this->y /= rhs; z /= rhs; return *this; }
        Vector3 operator / (const T& rhs) { return Vector3(*this) /= rhs; }

        bool operator == (const Vector3& rhs) { return this->x == rhs.x && this->y == rhs.y && z == rhs.z; }
        bool operator != (const Vector3& rhs) { return !(*this == rhs); }
    };

    template<class T>
    struct Vector4 : public Vector2<T>
    {
        T z, w;

    public:
        Vector4() = default;
        Vector4(T x, T y, T z, T w) : Vector2<T>(x, y), z(z), w(w) { }
        Vector4(T val) : Vector2<T>(val), z(val), w(val) { }

        T LengthSq() { return Sqr(this->x) + Sqr(this->y) + Sqr(z) + Sqr(w); }
        T Length() { return sqrt(LengthSq()); }

    public:
        template<class NT>
        explicit operator Vector4<NT>() const { return { static_cast<NT>(this->x), static_cast<NT>(this->y), static_cast<NT>(z), static_cast<NT>(w) }; }

        Vector4& operator = (const Vector4& rhs) { this->x = rhs.x; this->y = rhs.y; z = rhs.z; w = rhs.w; return *this; }

        Vector4 operator - () const { return Vector4(-(this->x), -(this->y), -z, -w); }

        Vector4& operator += (const Vector4& rhs) { this->x += rhs.x; this->y += rhs.y; z += rhs.z; w += rhs.w; return *this; }
        Vector4& operator += (T rhs) { this->x += rhs; this->y += rhs; z += rhs; w += rhs; return *this; }
        Vector4 operator + (const T& rhs) { return Vector4(*this) += rhs; }

        Vector4& operator -= (const Vector4& rhs) { this->x -= rhs.x; this->y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this; }
        Vector4& operator -= (T rhs) { this->x -= rhs; this->y -= rhs; z -= rhs; w -= rhs; return *this; }
        Vector4 operator - (const T& rhs) { return Vector4(*this) -= rhs; }

        Vector4& operator *= (const Vector4& rhs) { this->x *= rhs.x; this->y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this; }
        Vector4& operator *= (T rhs) { this->x *= rhs; this->y *= rhs; z *= rhs; w *= rhs; return *this; }
        Vector4 operator * (const T& rhs) { return Vector4(*this) *= rhs; }

        Vector4& operator /= (const Vector4& rhs) { this->x /= rhs.x; this->y /= rhs.y; z /= rhs.z; w /= rhs.w; return *this; }
        Vector4& operator /= (T rhs) { this->x /= rhs; this->y /= rhs; z /= rhs; w /= rhs; return *this; }
        Vector4 operator / (const T& rhs) { return Vector4(*this) /= rhs; }

        bool operator == (const Vector4& rhs) { return this->x == rhs.x && this->y == rhs.y && z == rhs.z && w == rhs.w; }
        bool operator != (const Vector4& rhs) { return !(*this == rhs); }
    };

    // ---------------------

    template<class T> inline T Sqr(T a) { return a * a; }

}