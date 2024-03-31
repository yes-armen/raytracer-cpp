#pragma once

#include <triangle.h>
#include <material.h>
#include <sphere.h>

struct Object {

    const Material* material = nullptr;
    Triangle polygon;
    std::vector<Vector> normals;

    Object(const Material* mtlr, Triangle plgn, std::vector<Vector> nrmls)
        : material(mtlr), polygon(plgn), normals(nrmls){};

    const Vector* GetNormal(size_t index) const {
        return &(normals[index]);
    }
};

struct SphereObject {
    SphereObject(const Material* mtrl, Sphere sphr) : material(mtrl), sphere(sphr) {
    }

    const Material* material = nullptr;
    Sphere sphere;
};