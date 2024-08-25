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

    template<class T, size_t S> struct MatrixSq;
    template<class T> using MatrixSq2 = MatrixSq<T, 2>;
    template<class T> using MatrixSq3 = MatrixSq<T, 3>;

    template<class T, size_t row, size_t col>
    using Array2 = std::array<std::array<T, col>, row>;	// helper for creating 2d arrays

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
   
    template<class T, size_t S> 
    struct MatrixSq
    {
        Array2<T, S, S> elements;

    public:
        MatrixSq() = default;
        MatrixSq(const std::initializer_list<std::initializer_list<T>>& ls)
        {
            for (size_t i = 0; i < ls.size(); ++i)
            {
                auto r = *(ls.begin() + i);
                for (size_t j = 0; j < r.size(); j++)
                {
                    elements[i][j] = *(r.begin() + j);
                }
            }
        }
        MatrixSq(T val) { for (auto& r : elements) r.fill(val); }

    public:
        MatrixSq& operator = (const MatrixSq& rhs)
        {
            std::copy(rhs.elements.begin(), rhs.elements.end(), elements.begin());
            return *this;
        }

        MatrixSq& operator *= (T rhs)
        {
            for (auto& r : elements)
            {
                for (auto& e : r) e *= rhs;
            }
            return *this;
        }

        MatrixSq& operator * (T rhs)
        {
            MatrixSq res = *this;
            return res *= rhs;
        }

        MatrixSq& operator*=(const MatrixSq& rhs)
        {
            return *this = *this * rhs;
        }

        MatrixSq operator*(const MatrixSq& rhs) const
        {
            MatrixSq res;
            for (size_t j = 0; j < S; j++)
            {
                for (size_t k = 0; k < S; k++)
                {
                    T sum = (T)0.0;
                    for (size_t i = 0; i < S; i++)
                    {
                        sum += elements[j][i] * rhs.elements[i][k];
                    }
                    res.elements[j][k] = sum;
                }
            }
            return res;
        }

        MatrixSq operator!() const
        {
            MatrixSq xp;
            for (size_t j = 0; j < S; j++)
            {
                for (size_t k = 0; k < S; k++)
                {
                    xp.elements[j][k] = elements[k][j];
                }
            }
            return xp;
        }

        constexpr static MatrixSq Identity()
        {
            if constexpr (S == 3)
            {
                return {
                    (T)1.0,(T)0.0,(T)0.0,
                    (T)0.0,(T)1.0,(T)0.0,
                    (T)0.0,(T)0.0,(T)1.0,
                };
            }
            else if constexpr (S == 4)
            {
                return {
                    (T)1.0,(T)0.0,(T)0.0,(T)0.0,
                    (T)0.0,(T)1.0,(T)0.0,(T)0.0,
                    (T)0.0,(T)0.0,(T)1.0,(T)0.0,
                    (T)0.0,(T)0.0,(T)0.0,(T)1.0,
                };
            }
            else
            {
                static_assert(S == 3 || S == 4, "Bad dimensionality");
            }
        }
        constexpr static MatrixSq Scaling(T factor)
        {
            if constexpr (S == 3)
            {
                return{
                    factor,(T)0.0,(T)0.0,
                    (T)0.0,factor,(T)0.0,
                    (T)0.0,(T)0.0,factor,
                };
            }
            else if constexpr (S == 4)
            {
                return {
                    factor,(T)0.0,(T)0.0,(T)0.0,
                    (T)0.0,factor,(T)0.0,(T)0.0,
                    (T)0.0,(T)0.0,factor,(T)0.0,
                    (T)0.0,(T)0.0,(T)0.0,(T)1.0,
                };
            }
            else
            {
                static_assert(S == 3 || S == 4, "Bad dimensionality");
            }
        }
        static MatrixSq RotationZ(T theta)
        {
            const T sinTheta = sin(theta);
            const T cosTheta = cos(theta);
            if constexpr (S == 3)
            {
                return{
                        cosTheta, sinTheta, (T)0.0,
                    -sinTheta, cosTheta, (T)0.0,
                    (T)0.0,    (T)0.0,   (T)1.0,
                };
            }
            else if constexpr (S == 4)
            {
                return {
                        cosTheta, sinTheta, (T)0.0,(T)0.0,
                    -sinTheta, cosTheta, (T)0.0,(T)0.0,
                    (T)0.0,    (T)0.0,   (T)1.0,(T)0.0,
                    (T)0.0,	   (T)0.0,   (T)0.0,(T)1.0,
                };
            }
            else
            {
                static_assert(S == 3 || S == 4, "Bad dimensionality");
            }
        }
        static MatrixSq RotationY(T theta)
        {
            const T sinTheta = sin(theta);
            const T cosTheta = cos(theta);
            if constexpr (S == 3)
            {
                return{
                        cosTheta, (T)0.0,-sinTheta,
                        (T)0.0,   (T)1.0, (T)0.0,
                        sinTheta, (T)0.0, cosTheta
                };
            }
            else if constexpr (S == 4)
            {
                return {
                    cosTheta, (T)0.0, -sinTheta,(T)0.0,
                    (T)0.0,   (T)1.0, (T)0.0,   (T)0.0,
                    sinTheta, (T)0.0, cosTheta, (T)0.0,
                    (T)0.0,   (T)0.0, (T)0.0,   (T)1.0,
                };
            }
            else
            {
                static_assert(S == 3 || S == 4, "Bad dimensionality");
            }
        }
        static MatrixSq RotationX(T theta)
        {
            const T sinTheta = sin(theta);
            const T cosTheta = cos(theta);
            if constexpr (S == 3)
            {
                return{
                    (T)1.0, (T)0.0,   (T)0.0,
                    (T)0.0, cosTheta, sinTheta,
                    (T)0.0,-sinTheta, cosTheta,
                };
            }
            else if constexpr (S == 4)
            {
                return {
                    (T)1.0, (T)0.0,   (T)0.0,  (T)0.0,
                    (T)0.0, cosTheta, sinTheta,(T)0.0,
                    (T)0.0,-sinTheta, cosTheta,(T)0.0,
                    (T)0.0, (T)0.0,   (T)0.0,  (T)1.0,
                };
            }
            else
            {
                static_assert(S == 3 || S == 4, "Bad dimensionality");
            }
        }

        template<class V>
        constexpr static MatrixSq Translation(const V& tl)
        {
            return Translation(tl.x, tl.y, tl.z);
        }
        constexpr static MatrixSq Translation(T x, T y, T z)
        {
            if constexpr (S == 4)
            {
                return {
                    (T)1.0,(T)0.0,(T)0.0,(T)0.0,
                    (T)0.0,(T)1.0,(T)0.0,(T)0.0,
                    (T)0.0,(T)0.0,(T)1.0,(T)0.0,
                    x,     y,      z,    (T)1.0,
                };
            }
            else
            {
                static_assert(S == 3 || S == 4, "Bad dimensionality");
            }
        }
        constexpr static MatrixSq Projection(T w, T h, T n, T f)
        {
            if constexpr (S == 4)
            {
                return {
                    (T)2.0 * n / w,	(T)0.0,			(T)0.0,				(T)0.0,
                    (T)0.0,			(T)2.0 * n / h,	(T)0.0,				(T)0.0,
                    (T)0.0,			(T)0.0,			f / (f - n),		(T)1.0,
                    (T)0.0,			(T)0.0,			-n * f / (f - n),	(T)0.0,
                };
            }
            else
            {
                static_assert(S == 3 || S == 4, "Bad dimensionality");
            }
        }
        
    };

    template<typename T>
    Vector3<T>& operator*=(Vector3<T>& lhs, const MatrixSq<T, 3>& rhs)
    {
        return lhs = lhs * rhs;
    }

    template<typename T>
    Vector3<T> operator*(const Vector3<T>& lhs, const MatrixSq<T, 3>& rhs)
    {
        return{
            lhs.x * rhs.elements[0][0] + lhs.y * rhs.elements[1][0] + lhs.z * rhs.elements[2][0],
            lhs.x * rhs.elements[0][1] + lhs.y * rhs.elements[1][1] + lhs.z * rhs.elements[2][1],
            lhs.x * rhs.elements[0][2] + lhs.y * rhs.elements[1][2] + lhs.z * rhs.elements[2][2]
        };
    }

    template<typename T>
    Vector4<T>& operator*=(Vector4<T>& lhs, const MatrixSq<T, 4>& rhs)
    {
        return lhs = lhs * rhs;
    }

    template<typename T>
    Vector4<T> operator*(const Vector4<T>& lhs, const MatrixSq<T, 4>& rhs)
    {
        return{
            lhs.x * rhs.elements[0][0] + lhs.y * rhs.elements[1][0] + lhs.z * rhs.elements[2][0] + lhs.w * rhs.elements[3][0],
            lhs.x * rhs.elements[0][1] + lhs.y * rhs.elements[1][1] + lhs.z * rhs.elements[2][1] + lhs.w * rhs.elements[3][1],
            lhs.x * rhs.elements[0][2] + lhs.y * rhs.elements[1][2] + lhs.z * rhs.elements[2][2] + lhs.w * rhs.elements[3][2],
            lhs.x * rhs.elements[0][3] + lhs.y * rhs.elements[1][3] + lhs.z * rhs.elements[2][3] + lhs.w * rhs.elements[3][3]
        };
    }
    
    // ---------------------

    template<class T>
    struct Rectangle
    {
        T top, bottom, left, right;

    public:
        Rectangle() {}
        Rectangle(T top, T bottom, T left, T right)
            :
            top(top),
            bottom(bottom),
            left(left),
            right(right)
        {}
        Rectangle(const Rectangle& rect)
            :
            top(rect.top),
            bottom(rect.bottom),
            left(rect.left),
            right(rect.right)
        {}
        Rectangle(Vector2<T> p0, Vector2<T> p1)
            :
            Rectangle(min(p0.y, p1.y),
                max(p0.y, p1.y),
                min(p0.x, p1.x),
                max(p0.x, p1.x))
        {}
        void Translate(Vector2<T> d)
        {
            Translate(d.x, d.y);
        }
        void Translate(T dx, T dy)
        {
            top += dy;
            bottom += dy;
            left += dx;
            right += dx;
        }
        template <typename T2>
        operator Rectangle<T2>() const
        {
            return { (T2)top,(T2)bottom,(T2)left,(T2)right };
        }
        void ClipTo(const Rectangle& rect)
        {
            top = std::max(top, rect.top);
            bottom = std::min(bottom, rect.bottom);
            left = std::max(left, rect.left);
            right = std::min(right, rect.right);
        }
        T GetWidth() const
        {
            return right - left;
        }
        T GetHeight() const
        {
            return bottom - top;
        }
        bool Overlaps(const Rectangle& rect) const
        {
            return top < rect.bottom && bottom > rect.top &&
                left < rect.right && right > rect.left;
        }
        template <typename T2>
        bool Contains(Vector2<T2> p) const
        {
            return p.y >= top && p.y <= bottom && p.x >= left && p.x <= right;
        }
        template <typename T2>
        bool Contains(Rectangle<T2> p) const
        {
            return p.top >= top && p.bottom <= bottom && p.left >= left && p.right <= right;
        }
    };

    // ---------------------

    template<class T>
    struct Triangle
    {
        T v0, v1, v2;
    };

    // ---------------------

    template<class T> inline T Sqr(T a) { return a * a; }

}