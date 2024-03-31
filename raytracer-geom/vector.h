#pragma once

#include <array>
#include <cmath>
#include <iostream>
#include <initializer_list>
#include <algorithm>

class Vector {
public:
    Vector(){};

    Vector(std::initializer_list<double> list) {
        std::copy(list.begin(), list.end(), data_.begin());
    };

    Vector(std::array<double, 3> data) : data_(data){};

    double& operator[](size_t ind) {
        return data_[ind];
    };

    double operator[](size_t ind) const {
        return data_[ind];
    };

    Vector operator+(const Vector& other) const {
        return Vector(
            {data_[0] + other.data_[0], data_[1] + other.data_[1], data_[2] + other.data_[2]});
    };

    Vector operator-(const Vector& other) const {
        return Vector(
            {data_[0] - other.data_[0], data_[1] - other.data_[1], data_[2] - other.data_[2]});
    };

    Vector operator*(double a) const {
        Vector result;
        result[0] = a * data_[0];
        result[1] = a * data_[1];
        result[2] = a * data_[2];
        return result;
    }

    Vector operator*=(double a) {
        data_[0] *= a;
        data_[1] *= a;
        data_[2] *= a;
        return *this;
    }

    void Normalize() {
        auto length = std::sqrt(pow(data_[0], 2) + pow(data_[1], 2) + pow(data_[2], 2));
        data_[0] /= length;
        data_[1] /= length;
        data_[2] /= length;
    }

    Vector operator*(const Vector& other) const {
        return Vector(
            {data_[0] * other.data_[0], data_[1] * other.data_[1], data_[2] * other.data_[2]});
    }

    Vector operator+(double a) const {
        Vector result;
        result[0] = a + data_[0];
        result[1] = a + data_[1];
        result[2] = a + data_[2];
        return result;
    }

    Vector operator/(const Vector& other) const {
        return Vector(
            {data_[0] / other.data_[0], data_[1] / other.data_[1], data_[2] / other.data_[2]});
    }

    Vector& operator+=(const Vector& other) {
        data_[0] += other[0];
        data_[1] += other[1];
        data_[2] += other[2];
        return *this;
    }

private:
    std::array<double, 3> data_;
};

inline double DotProduct(const Vector& lhs, const Vector& rhs) {
    double res = 0;
    for (size_t i = 0; i < 3; ++i) {
        res += lhs[i] * rhs[i];
    }
    return res;
}

inline Vector CrossProduct(const Vector& a, const Vector& b) {
    return Vector(
        {a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]});
}

inline double Length(const Vector& vec) {
    return std::sqrt(DotProduct(vec, vec));
}