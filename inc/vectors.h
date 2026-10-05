#pragma once
#include <cassert>
#include <cmath>
#include <ostream>
#include <utility>



template<int n> struct vec {
    double data[n] = {0};

    double& operator[](int i)       { assert(i >= 0 && i < n); return data[i]; }
    double  operator[](int i) const { assert(i >= 0 && i < n); return data[i]; }
};

template<> struct vec<3> {
    double x = 0;
    double y = 0;
    double z = 0;

    double& operator[](int i)       { assert(i >= 0 && i < 3); return i == 0 ? x : (i == 1 ? y : z); }
    double  operator[](int i) const { assert(i >= 0 && i < 3); return i == 0 ? x : (i == 1 ? y : z); }
};

template<> struct vec<2> {
    double x = 0, y = 0;

    double& operator[](int i)       { assert(i >= 0 && i < 2); return i == 0 ? x : y; }
    double  operator[](int i) const { assert(i >= 0 && i < 2); return i == 0 ? x : y; }
};

template<> struct vec<4> {
    double x = 0, y = 0, z = 0, w = 0;

    double& operator[](int i)       { assert(i >= 0 && i < 4); return i == 0 ? x : (i == 1 ? y : (i == 2 ? z : w)); }
    double  operator[](int i) const { assert(i >= 0 && i < 4); return i == 0 ? x : (i == 1 ? y : (i == 2 ? z : w)); }
};

using vec2 = vec<2>;
using vec3 = vec<3>;
using vec4 = vec<4>;

// operators (+, -, *, /)
// scalar values
template<int n> vec<n> operator+(const vec<n>& a, const vec<n>& b) {
    vec<n> r;
    for (int i = 0; i < n; i++) {
        r[i] = a[i] + b[i];
    }

    return r;
}

template<int n> vec<n> operator-(const vec<n>& a, const vec<n>& b) {
    vec<n> r;
    for (int i = 0; i < n; i++) r[i] = a[i] - b[i];
    return r;
}

template<int n> vec<n> operator-(const vec<n>& a) {
    vec<n> r;
    for (int i = 0; i < n; i++) r[i] = -a[i];
    return r;
}

template<int n> vec<n> operator*(const vec<n>& a, double s) {
    vec<n> r;
    for (int i = 0; i < n; i++) r[i] = a[i] * s;
    return r;
}

template<int n> vec<n> operator*(double s, const vec<n>& a) { return a * s; }

template<int n> vec<n> operator/(const vec<n>& a, double s) {
    vec<n> r;
    for (int i = 0; i < n; i++) r[i] = a[i] / s;
    return r;
}

template<int n> vec<n>& operator+=(vec<n>& a, const vec<n>& b) { return a = a + b; }
template<int n> vec<n>& operator-=(vec<n>& a, const vec<n>& b) { return a = a - b; }
template<int n> vec<n>& operator*=(vec<n>& a, double s)        { return a = a * s; }
template<int n> vec<n>& operator/=(vec<n>& a, double s)        { return a = a / s; }


// dot product
template<int n> double dot(const vec<n> &a, const vec<n> &b) {
    double r = 0;
    for(int i = 0; i < n; i++) {
        r += a[i] * b[i];
    }

    return r;
}

template<int n> double norm(const vec<n>& a) { return std::sqrt(dot(a, a)); }

template<int n> vec<n> normalized(const vec<n>& a) { return a / norm(a); }

// cross product
inline vec3 cross(const vec3& a, const vec3& b) {
    return { a.y * b.z - a.z * b.y,
             a.z * b.x - a.x * b.z,
             a.x * b.y - a.y * b.x };
}


template<int n> std::ostream& operator<<(std::ostream& out, const vec<n>& a) {
    out << "(";
    for (int i = 0; i < n; i++) out << a[i] << (i + 1 < n ? ", " : ")");
    return out;
}

/* MATRICES */