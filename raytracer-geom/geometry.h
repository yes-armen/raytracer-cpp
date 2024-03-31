#include <vector.h>
#include <sphere.h>
#include <intersection.h>
#include <triangle.h>
#include <ray.h>
#include <optional>

std::optional<Intersection> GetIntersection(const Ray& ray, const Sphere& sphere) {
    Vector center = sphere.GetCenter();
    Vector origin = ray.GetOrigin();
    Vector direction = ray.GetDirection();
    double r = sphere.GetRadius();
    Vector u = center - origin;
    Vector u_proj = direction * DotProduct(direction, u);
    double u_proj_length = DotProduct(direction, u);
    double h = Length(u - u_proj);
    double nearest_dist = sqrt(r * r - h * h);
    // origin out of sphere
    if (Length(u) > r) {
        if (r >= h) {
            double dist = u_proj_length - nearest_dist;
            if (dist < 0) {
                return std::nullopt;
            }
            Vector pos = direction * dist + origin;
            Vector normal = pos - center;
            normal.Normalize();
            return Intersection(pos, normal, dist);
        } else {
            return std::nullopt;
        }
        // origin in sphere
    } else {
        double dist = nearest_dist + u_proj_length;
        Vector pos = direction * dist + origin;
        Vector normal = center - pos;
        normal.Normalize();
        return Intersection(pos, normal, dist);
    }
}
std::optional<Intersection> GetIntersection(const Ray& ray, const Triangle& triangle) {
    const double eps = 1e-6;
    Vector edge1 = triangle.GetVertex(1) - triangle.GetVertex(0);
    Vector edge2 = triangle.GetVertex(2) - triangle.GetVertex(0);
    Vector direction = ray.GetDirection();
    Vector origin = ray.GetOrigin();
    Vector t_vec = ray.GetOrigin() - triangle.GetVertex(0);
    Vector p_vec = CrossProduct(direction, edge2);
    Vector q_vec = CrossProduct(t_vec, edge1);

    double det = DotProduct(edge1, p_vec);
    if (det < eps && det > -eps) {  // the ray is parallel to the triangle
        return std::nullopt;
    } else {  // the ray is NOT parallel to the triangle
        double inv_det = 1 / det;
        double u = DotProduct(t_vec, p_vec) * inv_det;
        double v = DotProduct(direction, q_vec) * inv_det;
        double dist = DotProduct(q_vec, edge2) * inv_det;
        if (u < 0 || u > 1 || v < 0 || u + v > 1 || dist < 0) {
            return std::nullopt;
        } else {
            Vector pos = origin + direction * dist;
            Vector normal = CrossProduct(edge1, edge2);
            normal *= -DotProduct(direction, normal);
            normal.Normalize();
            return Intersection(pos, normal, dist);
        }
    }
}

std::optional<Vector> Refract(const Vector& ray, const Vector& normal, double eta) {
    double product_c = -DotProduct(ray, normal);
    return ray * eta +
           normal * (eta * product_c - sqrt(1. - eta * eta * (1. - product_c * product_c)));
}

Vector Reflect(const Vector& ray, const Vector& normal) {
    return ray - normal * 2. * DotProduct(normal, ray);
}

Vector GetBarycentricCoords(const Triangle& triangle, const Vector& point) {
    Vector v0 = triangle.GetVertex(1) - triangle.GetVertex(0);
    Vector v1 = triangle.GetVertex(2) - triangle.GetVertex(0);
    Vector v2 = point - triangle.GetVertex(0);

    double d00 = DotProduct(v0, v0);
    double d01 = DotProduct(v0, v1);
    double d11 = DotProduct(v1, v1);
    double d20 = DotProduct(v2, v0);
    double d21 = DotProduct(v2, v1);
    double den = d00 * d11 - d01 * d01;
    double v = (d11 * d20 - d01 * d21) / den;
    double w = (d00 * d21 - d01 * d20) / den;
    return Vector({1.0f - v - w, v, w});
}
