#pragma once
#include <cassert>
#include <cmath>
#include <ostream>
#include <utility>
#include "vectors.h"

template<int nrows, int ncols> struct mat {
    vec<ncols> rows[nrows] = {};

    vec<ncols>&       operator[](int i)       { assert(i >= 0 && i < nrows); return rows[i]; }
    const vec<ncols>& operator[](int i) const { assert(i >= 0 && i < nrows); return rows[i]; }

    vec<nrows> col(int j) const {
        vec<nrows> r;
        for (int i = 0; i < nrows; i++) r[i] = rows[i][j];
        return r;
    }

    void set_col(int j, const vec<nrows>& v) {
        for (int i = 0; i < nrows; i++) rows[i][j] = v[i];
    }

    mat<ncols, nrows> transpose() const {
        mat<ncols, nrows> r;
        for (int i = 0; i < nrows; i++)
            for (int j = 0; j < ncols; j++)
                r[j][i] = rows[i][j];
        return r;
    }

    static mat identity() {
        mat r;
        for (int i = 0; i < nrows; i++)
            for (int j = 0; j < ncols; j++)
                r[i][j] = (i == j);
        return r;
    }
};

using mat2 = mat<2, 2>;
using mat3 = mat<3, 3>;
using mat4 = mat<4, 4>;

template<int r, int c> mat<r, c> operator+(const mat<r, c>& a, const mat<r, c>& b) {
    mat<r, c> m;
    for (int i = 0; i < r; i++) m[i] = a[i] + b[i];
    return m;
}

template<int r, int c> mat<r, c> operator-(const mat<r, c>& a, const mat<r, c>& b) {
    mat<r, c> m;
    for (int i = 0; i < r; i++) m[i] = a[i] - b[i];
    return m;
}

template<int r, int c> mat<r, c> operator*(const mat<r, c>& a, double s) {
    mat<r, c> m;
    for (int i = 0; i < r; i++) m[i] = a[i] * s;
    return m;
}

template<int r, int c> mat<r, c> operator*(double s, const mat<r, c>& a) { return a * s; }

template<int r, int c> mat<r, c> operator/(const mat<r, c>& a, double s) {
    mat<r, c> m;
    for (int i = 0; i < r; i++) m[i] = a[i] / s;
    return m;
}

// matrix * column vector
template<int r, int c> vec<r> operator*(const mat<r, c>& a, const vec<c>& v) {
    vec<r> out;
    for (int i = 0; i < r; i++) out[i] = dot(a[i], v);
    return out;
}

// row vector * matrix
template<int r, int c> vec<c> operator*(const vec<r>& v, const mat<r, c>& a) {
    vec<c> out;
    for (int j = 0; j < c; j++) out[j] = dot(v, a.col(j));
    return out;
}

template<int r, int k, int c> mat<r, c> operator*(const mat<r, k>& a, const mat<k, c>& b) {
    mat<r, c> m;
    for (int i = 0; i < r; i++)
        for (int j = 0; j < c; j++)
            m[i][j] = dot(a[i], b.col(j));
    return m;
}

// Gaussian elimination with partial pivoting
template<int n> double det(mat<n, n> a) {
    double d = 1;
    for (int k = 0; k < n; k++) {
        int pivot = k;
        for (int i = k + 1; i < n; i++)
            if (std::abs(a[i][k]) > std::abs(a[pivot][k])) pivot = i;
        if (a[pivot][k] == 0) return 0;
        if (pivot != k) { std::swap(a[pivot], a[k]); d = -d; }

        d *= a[k][k];
        for (int i = k + 1; i < n; i++)
            a[i] -= a[k] * (a[i][k] / a[k][k]);
    }
    return d;
}

// Gauss-Jordan elimination with partial pivoting: reduce [a | I] to [I | a^-1]
template<int n> mat<n, n> inverse(mat<n, n> a) {
    mat<n, n> inv = mat<n, n>::identity();
    for (int k = 0; k < n; k++) {
        int pivot = k;
        for (int i = k + 1; i < n; i++)
            if (std::abs(a[i][k]) > std::abs(a[pivot][k])) pivot = i;
        assert(a[pivot][k] != 0 && "matrix is singular");
        std::swap(a[pivot], a[k]);
        std::swap(inv[pivot], inv[k]);

        double p = a[k][k];
        a[k]   /= p;
        inv[k] /= p;

        for (int i = 0; i < n; i++) {
            if (i == k) continue;
            double f = a[i][k];
            a[i]   -= a[k] * f;
            inv[i] -= inv[k] * f;
        }
    }
    return inv;
}

template<int r, int c> std::ostream& operator<<(std::ostream& out, const mat<r, c>& m) {
    for (int i = 0; i < r; i++) out << m[i] << "\n";
    return out;
}
