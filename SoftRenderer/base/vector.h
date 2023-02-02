#pragma once
#include <cmath>
#include <cassert>
#include <iostream>

//---------------------------------------------------------------------
// 数学库：矢量定义
//---------------------------------------------------------------------

// 通用矢量：N 是矢量维度，T 为数据类型
template <size_t N, typename T>
struct Vector
{
    T m[N]; // 元素数组
    inline Vector() { for (size_t i = 0; i < N; i++) m[i] = T(); }
    inline Vector(const T* ptr) { for (size_t i = 0; i < N; i++) m[i] = ptr[i]; }
    inline Vector(const Vector<N, T>& u) { for (size_t i = 0; i < N; i++) m[i] = u.m[i]; }

    inline Vector(const std::initializer_list<T>& u)
    {
        auto it = u.begin();
        for (size_t i = 0; i < N; i++) m[i] = *it++;
    }

    inline const T& operator[](size_t i) const
    {
        assert(i < N);
        return m[i];
    }

    inline T& operator[](size_t i)
    {
        assert(i < N);
        return m[i];
    }

    inline void load(const T* ptr) { for (size_t i = 0; i < N; i++) m[i] = ptr[i]; }
    inline void save(T* ptr) { for (size_t i = 0; i < N; i++) ptr[i] = m[i]; }
};


// 特化二维矢量
template <typename T>
struct Vector<2, T>
{
    union
    {
        struct
        {
            T x, y;
        }; // 元素别名
        struct
        {
            T u, v;
        }; // 元素别名
        T m[2]; // 元素数组
    };

    inline Vector(): x(T()), y(T())
    {
    }

    inline Vector(T X, T Y): x(X), y(Y)
    {
    }

    inline Vector(const Vector<2, T>& u): x(u.x), y(u.y)
    {
    }

    inline Vector(const T* ptr): x(ptr[0]), y(ptr[1])
    {
    }

    inline const T& operator[](size_t i) const
    {
        assert(i < 2);
        return m[i];
    }

    inline T& operator[](size_t i)
    {
        assert(i < 2);
        return m[i];
    }

    inline void load(const T* ptr) { for (size_t i = 0; i < 2; i++) m[i] = ptr[i]; }
    inline void save(T* ptr) { for (size_t i = 0; i < 2; i++) ptr[i] = m[i]; }
    inline Vector<2, T> xy() const { return *this; }
    inline Vector<3, T> xy1() const { return Vector<3, T>(x, y, 1); }
    inline Vector<4, T> xy11() const { return Vector<4, T>(x, y, 1, 1); }
    inline Vector<2, T> operator *(float f) const { return Vector<2, T>(x * f, y * f); }
};


// 特化三维矢量
template <typename T>
struct Vector<3, T>
{
    union
    {
        struct
        {
            T x, y, z;
        }; // 元素别名
        struct
        {
            T r, g, b;
        }; // 元素别名
        T m[3]; // 元素数组
    };

    static Vector zero;
    static Vector one;
    static Vector up;
    static Vector right;
    static Vector forward;
    static Vector down;
    static Vector back;

    inline Vector(): x(T()), y(T()), z(T())
    {
    }

    inline Vector(T X, T Y, T Z): x(X), y(Y), z(Z)
    {
    }

    inline Vector(const Vector<3, T>& u): x(u.x), y(u.y), z(u.z)
    {
    }

    inline Vector(const T* ptr): x(ptr[0]), y(ptr[1]), z(ptr[2])
    {
    }

    inline const T& operator[](size_t i) const
    {
        assert(i < 3);
        return m[i];
    }

    inline T& operator[](size_t i)
    {
        assert(i < 3);
        return m[i];
    }

    inline void load(const T* ptr) { for (size_t i = 0; i < 3; i++) m[i] = ptr[i]; }
    inline void save(T* ptr) { for (size_t i = 0; i < 3; i++) ptr[i] = m[i]; }
    inline Vector<2, T> xy() const { return Vector<2, T>(x, y); }
    inline Vector<3, T> xyz() const { return *this; }
    inline Vector<4, T> xyz1() const { return Vector<4, T>(x, y, z, 1); }
    inline Vector<4, T> xyz0() const { return Vector<4, T>(x, y, z, 0); }
    float normal() { return std::sqrt(x * x + y * y + z * z); }

    inline Vector<3, T>& normalize(T l = 1)
    {
        *this = (*this) * (l / normal());
        return *this;
    }
};


// 特化四维矢量
template <typename T>
struct Vector<4, T>
{
    union
    {
        struct
        {
            T x, y, z, w;
        }; // 元素别名
        struct
        {
            T r, g, b, a;
        }; // 元素别名
        T m[4]; // 元素数组
    };

    inline Vector(): x(T()), y(T()), z(T()), w(T())
    {
    }

    inline Vector(T X, T Y, T Z, T W): x(X), y(Y), z(Z), w(W)
    {
    }

    inline Vector(T X, T Y, T Z): x(X), y(Y), z(Z), w(1)
    {
    }

    inline Vector(const Vector<4, T>& u): x(u.x), y(u.y), z(u.z), w(u.w)
    {
    }

    inline Vector(const T* ptr): x(ptr[0]), y(ptr[1]), z(ptr[2]), w(ptr[3])
    {
    }

    inline const T& operator[](size_t i) const
    {
        assert(i < 4);
        return m[i];
    }

    inline T& operator[](size_t i)
    {
        assert(i < 4);
        return m[i];
    }

    inline void load(const T* ptr) { for (size_t i = 0; i < 4; i++) m[i] = ptr[i]; }
    inline void save(T* ptr) { for (size_t i = 0; i < 4; i++) ptr[i] = m[i]; }
    inline Vector<2, T> xy() const { return Vector<2, T>(x, y); }
    inline Vector<3, T> xyz() const { return Vector<3, T>(x, y, z); }
    inline Vector<4, T> xyzw() const { return *this; }

    static Vector red;
    static Vector green;
    static Vector blue;
    static Vector white;
    static Vector black;
    static Vector yellow;
    static Vector cyan;
    static Vector magenta;
    static Vector gray;
    static Vector grey;
    static Vector clear;
};


//---------------------------------------------------------------------
// 数学库：矢量运算
//---------------------------------------------------------------------

// = (+a)
template <size_t N, typename T>
inline Vector<N, T> operator +(const Vector<N, T>& a)
{
    return a;
}

// = (-a)
template <size_t N, typename T>
inline Vector<N, T> operator -(const Vector<N, T>& a)
{
    Vector<N, T> b;
    for (size_t i = 0; i < N; i++) b[i] = -a[i];
    return b;
}

// = (a == b) ? true : false
template <size_t N, typename T>
inline bool operator ==(const Vector<N, T>& a, const Vector<N, T>& b)
{
    for (size_t i = 0; i < N; i++) if (a[i] != b[i]) return false;
    return true;
}

// = (a != b)? true : false
template <size_t N, typename T>
inline bool operator !=(const Vector<N, T>& a, const Vector<N, T>& b)
{
    return !(a == b);
}

// = a + b
template <size_t N, typename T>
inline Vector<N, T> operator +(const Vector<N, T>& a, const Vector<N, T>& b)
{
    Vector<N, T> c;
    for (size_t i = 0; i < N; i++) c[i] = a[i] + b[i];
    return c;
}

// = a - b
template <size_t N, typename T>
inline Vector<N, T> operator -(const Vector<N, T>& a, const Vector<N, T>& b)
{
    Vector<N, T> c;
    for (size_t i = 0; i < N; i++) c[i] = a[i] - b[i];
    return c;
}

// = a * b，不是点乘也不是叉乘，而是各个元素分别相乘，色彩计算时有用
template <size_t N, typename T>
inline Vector<N, T> operator *(const Vector<N, T>& a, const Vector<N, T>& b)
{
    Vector<N, T> c;
    for (size_t i = 0; i < N; i++) c[i] = a[i] * b[i];
    return c;
}

// = a / b，各个元素相除
template <size_t N, typename T>
inline Vector<N, T> operator /(const Vector<N, T>& a, const Vector<N, T>& b)
{
    Vector<N, T> c;
    for (size_t i = 0; i < N; i++) c[i] = a[i] / b[i];
    return c;
}

// = a * x
template <size_t N, typename T>
inline Vector<N, T> operator *(const Vector<N, T>& a, T x)
{
    Vector<N, T> b;
    for (size_t i = 0; i < N; i++) b[i] = a[i] * x;
    return b;
}

// = x * a
template <size_t N, typename T>
inline Vector<N, T> operator *(T x, const Vector<N, T>& a)
{
    Vector<N, T> b;
    for (size_t i = 0; i < N; i++) b[i] = a[i] * x;
    return b;
}

// = a / x
template <size_t N, typename T>
inline Vector<N, T> operator /(const Vector<N, T>& a, T x)
{
    Vector<N, T> b;
    for (size_t i = 0; i < N; i++) b[i] = a[i] / x;
    return b;
}

// = x / a
template <size_t N, typename T>
inline Vector<N, T> operator /(T x, const Vector<N, T>& a)
{
    Vector<N, T> b;
    for (size_t i = 0; i < N; i++) b[i] = x / a[i];
    return b;
}

// a += b
template <size_t N, typename T>
inline Vector<N, T>& operator +=(Vector<N, T>& a, const Vector<N, T>& b)
{
    for (size_t i = 0; i < N; i++) a[i] += b[i];
    return a;
}

// a -= b
template <size_t N, typename T>
inline Vector<N, T>& operator -=(Vector<N, T>& a, const Vector<N, T>& b)
{
    for (size_t i = 0; i < N; i++) a[i] -= b[i];
    return a;
}

// a *= b
template <size_t N, typename T>
inline Vector<N, T>& operator *=(Vector<N, T>& a, const Vector<N, T>& b)
{
    for (size_t i = 0; i < N; i++) a[i] *= b[i];
    return a;
}

// a /= b
template <size_t N, typename T>
inline Vector<N, T>& operator /=(Vector<N, T>& a, const Vector<N, T>& b)
{
    for (size_t i = 0; i < N; i++) a[i] /= b[i];
    return a;
}

// a *= x
template <size_t N, typename T>
inline Vector<N, T>& operator *=(Vector<N, T>& a, T x)
{
    for (size_t i = 0; i < N; i++) a[i] *= x;
    return a;
}

// a /= x
template <size_t N, typename T>
inline Vector<N, T>& operator /=(Vector<N, T>& a, T x)
{
    for (size_t i = 0; i < N; i++) a[i] /= x;
    return a;
}


//---------------------------------------------------------------------
// 数学库：矢量函数
//---------------------------------------------------------------------

// 不同维度的矢量转换
template <size_t N1, size_t N2, typename T>
inline Vector<N1, T> vector_convert(const Vector<N2, T>& a, T fill = 1)
{
    Vector<N1, T> b;
    for (size_t i = 0; i < N1; i++)
        b[i] = (i < N2) ? a[i] : fill;
    return b;
}

// = |a| ^ 2
template <size_t N, typename T>
inline T vector_length_square(const Vector<N, T>& a)
{
    T sum = 0;
    for (size_t i = 0; i < N; i++) sum += a[i] * a[i];
    return sum;
}

// = |a|
template <size_t N, typename T>
inline T vector_length(const Vector<N, T>& a)
{
    return sqrt(vector_length_square(a));
}

// = |a| , 特化 float 类型，使用 sqrtf
template <size_t N>
inline float vector_length(const Vector<N, float>& a)
{
    return sqrtf(vector_length_square(a));
}

// = a / |a|
template <size_t N, typename T>
inline Vector<N, T> vector_normalize(const Vector<N, T>& a)
{
    return a / vector_length(a);
}

// 矢量点乘
template <size_t N, typename T>
inline T vector_dot(const Vector<N, T>& a, const Vector<N, T>& b)
{
    T sum = 0;
    for (size_t i = 0; i < N; i++) sum += a[i] * b[i];
    return sum;
}

// 二维矢量叉乘，得到标量
template <typename T>
inline T vector_cross(const Vector<2, T>& a, const Vector<2, T>& b)
{
    return a.x * b.y - a.y * b.x;
}

// 三维矢量叉乘，得到新矢量
template <typename T>
inline Vector<3, T> vector_cross(const Vector<3, T>& a, const Vector<3, T>& b)
{
    return Vector<3, T>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

// 四维矢量叉乘：前三维叉乘，后一位保留
template <typename T>
inline Vector<4, T> vector_cross(const Vector<4, T>& a, const Vector<4, T>& b)
{
    return Vector<4, T>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x, a.w);
}

// = a + (b - a) * t
template <size_t N, typename T>
inline Vector<N, T> vector_lerp(const Vector<N, T>& a, const Vector<N, T>& b, float t)
{
    return a + (b - a) * t;
}

// 各个元素取最大值
template <size_t N, typename T>
inline Vector<N, T> vector_max(const Vector<N, T>& a, const Vector<N, T>& b)
{
    Vector<N, T> c;
    for (size_t i = 0; i < N; i++) c[i] = (a[i] > b[i]) ? a[i] : b[i];
    return c;
}

// 各个元素取最小值
template <size_t N, typename T>
inline Vector<N, T> vector_min(const Vector<N, T>& a, const Vector<N, T>& b)
{
    Vector<N, T> c;
    for (size_t i = 0; i < N; i++) c[i] = (a[i] < b[i]) ? a[i] : b[i];
    return c;
}

// 将矢量的值控制在 minx/maxx 范围内
template <size_t N, typename T>
inline Vector<N, T> vector_between(const Vector<N, T>& minx, const Vector<N, T>& maxx, const Vector<N, T>& x)
{
    return vector_min(vector_max(minx, x), maxx);
}

// 判断矢量是否接近
template <size_t N, typename T>
inline bool vector_near(const Vector<N, T>& a, const Vector<N, T>& b, T dist)
{
    return (vector_length_square(a - b) <= dist);
}

// 判断两个单精度矢量是否近似
template <size_t N>
inline bool vector_near_equal(const Vector<N, float>& a, const Vector<N, float>& b, float e = 0.0001)
{
    return vector_near(a, b, e);
}

// 判断两个双精度矢量是否近似
template <size_t N>
inline bool vector_near_equal(const Vector<N, double>& a, const Vector<N, double>& b, double e = 0.0000001)
{
    return vector_near(a, b, e);
}

// 矢量值元素范围裁剪
template <size_t N, typename T>
inline Vector<N, T> vector_clamp(const Vector<N, T>& a, T minx = 0, T maxx = 1)
{
    Vector<N, T> b;
    for (size_t i = 0; i < N; i++)
    {
        T x = (a[i] < minx) ? minx : a[i];
        b[i] = (x > maxx) ? maxx : x;
    }
    return b;
}

// 输出到文本流
template <size_t N, typename T>
inline std::ostream& operator <<(std::ostream& os, const Vector<N, T>& a)
{
    os << "[";
    for (size_t i = 0; i < N; i++)
    {
        os << a[i];
        if (i < N - 1) os << ", ";
    }
    os << "]";
    return os;
}

// 输出成字符串
template <size_t N, typename T>
inline std::string vector_repr(const Vector<N, T>& a)
{
    std::stringstream ss;
    ss << a;
    return ss.str();
}

template <>
Vector<3, float> Vector<3, float>::zero = Vector<3, float>(0, 0, 0);
template <>
Vector<3, float> Vector<3, float>::one = Vector<3, float>(1, 1, 1);
template <>
Vector<3, float> Vector<3, float>::up = Vector<3, float>(0, 1, 0);
template <>
Vector<3, float> Vector<3, float>::right = Vector<3, float>(1, 0, 0);
template <>
Vector<3, float> Vector<3, float>::forward = Vector<3, float>(0, 0, 1);
template <>
Vector<3, float> Vector<3, float>::down = Vector<3, float>(0, -1, 0);
template <>
Vector<3, float> Vector<3, float>::back = Vector<3, float>(0, 0, -1);

//******************************************颜色************************************
template <>
Vector<4, float> Vector<4, float>::red = Vector<4, float>(1.f, 0.0f, 0.0f, 1.f);
template <>
Vector<4, float> Vector<4, float>::green = Vector<4, float>(0.0f, 1.f, 0.0f, 1.f);
template <>
Vector<4, float> Vector<4, float>::blue = Vector<4, float>(0.0f, 0.0f, 1.f, 1.f);
template <>
Vector<4, float> Vector<4, float>::white = Vector<4, float>(1.f, 1.f, 1.f, 1.f);
template <>
Vector<4, float> Vector<4, float>::black = Vector<4, float>(0.0f, 0.0f, 0.0f, 1.f);
template <>
Vector<4, float> Vector<4, float>::yellow = Vector<4, float>(1.f, 0.92156863f, 0.015686275f, 1.f);
template <>
Vector<4, float> Vector<4, float>::cyan = Vector<4, float>(0.0f, 1.f, 1.f, 1.f);
template <>
Vector<4, float> Vector<4, float>::magenta = Vector<4, float>(1.f, 0.0f, 1.f, 1.f);
template <>
Vector<4, float> Vector<4, float>::gray = Vector<4, float>(0.5f, 0.5f, 0.5f, 1.f);
template <>
Vector<4, float> Vector<4, float>::grey = Vector<4, float>(0.5f, 0.5f, 0.5f, 1.f);
template <>
Vector<4, float> Vector<4, float>::clear = Vector<4, float>(0.0f, 0.0f, 0.0f, 0.0f);
//******************************************颜色************************************

typedef Vector<2, float> Vector2f;
typedef Vector<2, int> Vector2i;
typedef Vector<3, float> Vector3f;
typedef Vector<3, int> Vector3i;
typedef Vector<4, float> Vector4f;
typedef Vector<4, float> Color;

template <typename T>
inline T Abs(T x) { return (x < 0) ? (-x) : x; }

template <typename T>
inline T Max(T x, T y) { return (x < y) ? y : x; }

template <typename T>
inline T Min(T x, T y) { return (x > y) ? y : x; }

template <typename T>
inline T Between(T xmin, T xmax, T x)
{
    return Min(Max(xmin, x), xmax);
}
// 截取 [0, 1] 的范围
template <typename T>
inline T Saturate(T x)
{
    return Between<T>(0, 1, x);
}
// 矢量转整数颜色
inline static uint32_t vector_to_color(const Color& color)
{
    uint32_t r = (uint32_t)Between(0, 255, (int)(color.r * 255.0f));
    uint32_t g = (uint32_t)Between(0, 255, (int)(color.g * 255.0f));
    uint32_t b = (uint32_t)Between(0, 255, (int)(color.b * 255.0f));
    uint32_t a = (uint32_t)Between(0, 255, (int)(color.a * 255.0f));
    return (r << 16) | (g << 8) | b | (a << 24);
}

// 整数颜色到矢量
inline static Color vector_from_color(uint32_t rgba)
{
    Color out;
    out.r = ((rgba >> 16) & 0xff) / 255.0f;
    out.g = ((rgba >> 8) & 0xff) / 255.0f;
    out.b = ((rgba >> 0) & 0xff) / 255.0f;
    out.a = ((rgba >> 24) & 0xff) / 255.0f;
    return out;
}
