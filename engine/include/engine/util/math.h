// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_UTIL_MATH_H
#define UNNAMEDGAME_ENGINE_UTIL_MATH_H

#include <array>

namespace math {
    struct Color4F {
        float r, g, b, a;
    };

    struct Color4B {
        unsigned char r, g, b, a;

        Color4F toColor4F() const {
            return {
                static_cast<float>(r) / 255.0f,
                static_cast<float>(g) / 255.0f,
                static_cast<float>(b) / 255.0f,
                static_cast<float>(a) / 255.0f,
            };
        }
    };

    using Color = Color4B;

    template<class T>
    struct Vec2T {
        using ValueType = T;

        T x, y;

        Vec2T() = default;
        Vec2T(T x, T y) : x(x), y(y) {}

        static constexpr Vec2T Zero() { return {0, 0}; }
        static constexpr Vec2T One() { return {1, 1}; }
        static constexpr Vec2T UnitX() { return {1, 0}; };
        static constexpr Vec2T UnitY() { return {0, 1}; };
        static constexpr Vec2T Up() { return {0, -1}; };
        static constexpr Vec2T Down() { return {0, 1}; };
        static constexpr Vec2T Left() { return {-1, 0}; };
        static constexpr Vec2T Right() { return {1, 0}; };

        Vec2T& operator+=(Vec2T other) {
            x += other.x;
            y += other.y;
            return *this;
        }

        Vec2T& operator-=(Vec2T other) {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        Vec2T& operator*=(T scale) {
            x *= scale;
            y *= scale;
            return *this;
        }

        Vec2T& operator/=(T scale) {
            x /= scale;
            y /= scale;
            return *this;
        }
    };

    using Vec2I = Vec2T<int>;
    using Vec2L = Vec2T<long long>;
    using Vec2F = Vec2T<float>;
    using Vec2D = Vec2T<double>;

    using Vec2 = Vec2F;

    template<class T>
    Vec2T<T> operator+(Vec2T<T> lhs, Vec2T<T> rhs) {
        lhs += rhs;
        return lhs;
    }

    template<class T>
    Vec2T<T> operator-(Vec2T<T> lhs, Vec2T<T> rhs) {
        lhs -= rhs;
        return lhs;
    }

    template<class T>
    Vec2T<T> operator*(Vec2T<T> vec, T scale) {
        vec *= scale;
        return vec;
    }

    template<class T>
    Vec2T<T> operator*(T scale, Vec2T<T> vec) {
        vec *= scale;
        return vec;
    }

    template<class T>
    Vec2T<T> operator/(Vec2T<T> vec, T scale) {
        vec /= scale;
        return vec;
    }

    template<class T>
    Vec2T<T> operator-(Vec2T<T> vec) {
        return {-vec.x, -vec.y};
    }

    template<class T>
    struct RectT {
        T left, right, top, bottom;

        RectT() = default;
        RectT(T left, T right, T top, T bottom) : left(left), right(right), top(top), bottom(bottom) {}
    };

    using RectI = RectT<int>;
    using RectL = RectT<long long>;
    using RectF = RectT<float>;
    using RectD = RectT<double>;

    using Rect = RectF;

    // Column Major
    template<class T, int R, int C>
    class MatrixT {
    public:
        using ValueType = T;
        static constexpr int Rows = R;
        static constexpr int Cols = C;

        MatrixT() = default;

        static constexpr MatrixT Zero() {
            MatrixT result;
            for (int i = 0; i < Rows * Cols; i++) {
                result.mData[i] = 0;
            }
            return result;
        }

        static constexpr MatrixT One() {
            MatrixT result;
            for (int i = 0; i < Rows * Cols; i++) {
                result.mData[i] = 1;
            }
            return result;
        }

        static constexpr MatrixT Identity() requires(Rows == Cols) {
            return Diagonal(T(1));
        }

        static constexpr MatrixT Diagonal(T value) requires(Rows == Cols) {
            MatrixT result;
            for (int i = 0; i < std::min(Rows, Cols); i++) {
                result(i, i) = 1;
            }
            return result;
        }

        static constexpr MatrixT Diagonal(const std::array<T, std::min(Rows, Cols)> values) {
            MatrixT result;
            for (int i = 0; i < std::min(Rows, Cols); i++) {
                result(i, i) = values[i];
            }
            return result;
        }

        static constexpr MatrixT Orthographic(T left, T right, T top, T bottom) requires(Rows == 4 && Cols == 4) {
            MatrixT result;

            result(0, 0) = T(2) / (right - left);
            result(1, 1) = T(2) / (top - bottom);
            result(2, 2) = T(-1);
            result(0, 3) = -(right + left) / (right - left);
            result(1, 3) = -(top + bottom) / (top - bottom);
            result(3, 3) = T(1);

            return result;
        }

        static constexpr MatrixT Orthographic(RectT<T> rect) { return Orthographic(rect.left, rect.right, rect.top, rect.bottom); }

        static constexpr MatrixT Translation(Vec2T<T> vec) requires(Rows == Cols) {
            constexpr int N = Rows;

            static_assert(N >= 3, "2D translation requires 3x3 matrix minimum");

            auto result = MatrixT<T, N, N>::Identity();

            result(0, N - 1) = vec.x;
            result(1, N - 1) = vec.y;

            return result;
        }

        static constexpr MatrixT Scale(Vec2T<T> scale) requires(Rows == Cols) {
            return Diagonal({scale.x, scale.y, T(1), T(1)});
        }

        template<int OtherCols>
        constexpr MatrixT& operator*=(const MatrixT<T, Cols, OtherCols>& other) requires(OtherCols == Cols) {
            MatrixT lhs = *this;

            for (int i = 0; i < Rows; i++) {
                for (int j = 0; j < OtherCols; j++) {
                    T sum{};

                    for (int k = 0; k < Cols; k++) {
                        sum += lhs(i, k) * other(k, j);
                    }

                    (*this)(i, j) = sum;
                }
            }

            return *this;
        }

        T& operator()(int row, int col) {
            return mData[col * Rows + row];
        }

        const T& operator()(int row, int col) const {
            return mData[col * Rows + row];
        }

        T* data() { return mData; }
        const T* data() const { return mData; }

    private:
        T mData[Rows * Cols]{};
    };

    template<class T>
    using Matrix2 = MatrixT<T, 2, 2>;

    template<class T>
    using Matrix3 = MatrixT<T, 3, 3>;

    template<class T>
    using Matrix4 = MatrixT<T, 4, 4>;

    using Mat2F = Matrix2<float>;
    using Mat3F = Matrix3<float>;
    using Mat4F = Matrix4<float>;

    using Mat2D = Matrix2<double>;
    using Mat3D = Matrix3<double>;
    using Mat4D = Matrix4<double>;

    using Mat2 = Mat2F;
    using Mat3 = Mat3F;
    using Mat4 = Mat4F;

    template<class T, int Rows, int Cols>
    MatrixT<T, Rows, Cols> operator*(MatrixT<T, Rows, Cols> lhs, const MatrixT<T, Cols, Cols>& rhs) {
        lhs *= rhs;
        return lhs;
    }
}

#endif //UNNAMEDGAME_ENGINE_UTIL_MATH_H
