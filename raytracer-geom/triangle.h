#pragma once

#include <vector.h>

class Triangle {
public:
    Triangle(std::initializer_list<Vector> list) {
        std::copy(list.begin(), list.end(), vertices_.begin());
    };

    double Area() const {
        Vector v0 = vertices_[0];
        Vector v1 = vertices_[1];
        Vector v2 = vertices_[2];

        Vector cross_product = CrossProduct(v2 - v0, v1 - v0);
        double res = Length(cross_product) / 2;

        return res;
    };

    const Vector& GetVertex(size_t ind) const {
        return vertices_[ind];
    };

private:
    std::array<Vector, 3> vertices_;
};
