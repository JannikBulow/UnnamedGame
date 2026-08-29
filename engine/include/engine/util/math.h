// Copyright 2026 Jannik Laugmand Bülow

#ifndef UNNAMEDGAME_ENGINE_UTIL_MATH_H
#define UNNAMEDGAME_ENGINE_UTIL_MATH_H

#include <array>
#include <cmath>

namespace math {
    struct Color4B {
        static const Color4B Black;
        static const Color4B White;
        static const Color4B Transparent;
        static const Color4B DarkGray;
        static const Color4B Gray;
        static const Color4B LightGray;
        static const Color4B Red;
        static const Color4B Green;
        static const Color4B Blue;
        static const Color4B Yellow;
        static const Color4B Cyan;
        static const Color4B Magenta;
        static const Color4B Orange;
        static const Color4B Purple;
        static const Color4B Teal;
        unsigned char r, g, b, a;

        constexpr Color4B() = default;
        constexpr Color4B(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) : r(r), g(g), b(b), a(a) {}

        constexpr int rgb() const {
            return (0xFF << 24) | (r << 16) | (g << 8) | b;
        }

        constexpr int rgba() const {
            return (a << 24) | (r << 16) | (g << 8) | b;
        }

        explicit constexpr operator struct Color4F() const;
    };

    struct Color4F {static const Color4F Black;
        static const Color4F White;
        static const Color4F Transparent;
        static const Color4F DarkGray;
        static const Color4F Gray;
        static const Color4F LightGray;
        static const Color4F Red;
        static const Color4F Green;
        static const Color4F Blue;
        static const Color4F Yellow;
        static const Color4F Cyan;
        static const Color4F Magenta;
        static const Color4F Orange;
        static const Color4F Purple;
        static const Color4F Teal;

        float r, g, b, a;

        constexpr Color4F() = default;
        constexpr Color4F(float r, float g, float b) : r(r), g(g), b(b), a(1.0f) {}
        constexpr Color4F(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

        explicit constexpr operator struct Color4B() const;
    };

    constexpr Color4F::operator Color4B() const {
        return {
            static_cast<unsigned char>(r * 255),
            static_cast<unsigned char>(g * 255),
            static_cast<unsigned char>(b * 255),
            static_cast<unsigned char>(a * 255)
        };
    }

    constexpr Color4B::operator Color4F() const {
        return {
            static_cast<float>(r) / 255.0f,
            static_cast<float>(g) / 255.0f,
            static_cast<float>(b) / 255.0f,
            static_cast<float>(a) / 255.0f,
        };
    }

    constexpr Color4B Color4B::Black{0, 0, 0};
    constexpr Color4B Color4B::White{255, 255, 255};
    constexpr Color4B Color4B::Transparent{0, 0, 0, 0};
    constexpr Color4B Color4B::DarkGray{32, 32, 32};
    constexpr Color4B Color4B::Gray{128, 128, 128};
    constexpr Color4B Color4B::LightGray{200, 200, 200};
    constexpr Color4B Color4B::Red{255, 0, 0};
    constexpr Color4B Color4B::Green{0, 255, 0};
    constexpr Color4B Color4B::Blue{0, 0, 255};
    constexpr Color4B Color4B::Yellow{255, 255, 0};
    constexpr Color4B Color4B::Cyan{0, 255, 255};
    constexpr Color4B Color4B::Magenta{255, 0, 255};
    constexpr Color4B Color4B::Orange{255, 128, 0};
    constexpr Color4B Color4B::Purple{128, 0, 255};
    constexpr Color4B Color4B::Teal{0, 128, 128};

    constexpr Color4F Color4F::Black{Color4B::Black};
    constexpr Color4F Color4F::White{Color4B::White};
    constexpr Color4F Color4F::Transparent{Color4B::Transparent};
    constexpr Color4F Color4F::DarkGray{Color4B::DarkGray};
    constexpr Color4F Color4F::Gray{Color4B::Gray};
    constexpr Color4F Color4F::LightGray{Color4B::LightGray};
    constexpr Color4F Color4F::Red{Color4B::Red};
    constexpr Color4F Color4F::Green{Color4B::Green};
    constexpr Color4F Color4F::Blue{Color4B::Blue};
    constexpr Color4F Color4F::Yellow{Color4B::Yellow};
    constexpr Color4F Color4F::Cyan{Color4B::Cyan};
    constexpr Color4F Color4F::Magenta{Color4B::Magenta};
    constexpr Color4F Color4F::Orange{Color4B::Orange};
    constexpr Color4F Color4F::Purple{Color4B::Purple};
    constexpr Color4F Color4F::Teal{Color4B::Teal};

    using Color = Color4B;

    template<class T>
    struct Vec2T {
        using ValueType = T;

        T x, y;

        constexpr Vec2T() = default;
        constexpr Vec2T(T x, T y) : x(x), y(y) {}

        template<class U>
        explicit constexpr operator Vec2T<U>() const {
            return {U(x), U(y)};
        }

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

        constexpr MatrixT() = default;

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

        static constexpr MatrixT RotationZ(T rads) requires(Rows == Cols && Rows >= 3) {
            MatrixT result = Identity();

            T c = std::cos(rads);
            T s = std::sin(rads);

            result(0, 0) = c;
            result(0, 1) = -s;
            result(1, 0) = s;
            result(1, 1) = c;

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


    constexpr double PI = 3.14159265358979323846;

    template<class T>
    constexpr T rads(T degs) {
        return degs * (PI / 180.0);
    }
}

#endif //UNNAMEDGAME_ENGINE_UTIL_MATH_H
