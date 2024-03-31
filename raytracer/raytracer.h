#pragma once

#include <image.h>
#include <camera_options.h>
#include <render_options.h>
#include <string>
#include <scene.h>
#include <cmath>
#include <ray.h>
#include <geometry.h>
#include <vector>
#include <map>

std::optional<std::pair<Intersection, std::string>> GetFirstIntersection(const Scene& scene,
                                                                         const Ray& ray);

Image GammaCorrection(std::vector<std::vector<Vector>>& pixels);

void ToneMapping(std::vector<std::vector<Vector>>& pixels);

Vector DiffuseByLight(const Intersection& intersection_point, const Light& light,
                      const Material* material);

Vector SpecularByLight(const Intersection& intersection_point, const Light& light,
                       const Material* material, const Vector& see);

Vector PhongIllumination(const Ray& ray, const Scene& scene, const RenderOptions& render_options,
                         const CameraOptions& camera_options, int depth, bool is_inside);

bool IsVisible(const Light& light, const Vector& pos, const Scene& scene);

class CameraScreen {
public:
    CameraScreen(const CameraOptions& options) : options_(options) {
        BasisScreen(options);
    }

    void BasisScreen(const CameraOptions& options) {
        double width = options.screen_width;
        double height = options.screen_height;
        image_aspect_ratio_ = width / height;
        tan_half_fov_ = tan(options.fov / 2);
        Vector from(options.look_from);
        Vector to(options.look_to);
        forward_ = from - to;
        forward_.Normalize();

        if (forward_[0] == 0 && forward_[2] == 0) {
            right_ = Vector{1, 0, 0};
        } else {
            right_ = CrossProduct(Vector{0, 1, 0}, forward_);
        }
        right_.Normalize();
        up_ = CrossProduct(forward_, right_);
        up_.Normalize();
    }

    Ray GetRay(int x, int y) {
        double width = options_.screen_width;
        double height = options_.screen_height;
        double pixel_screen_x = (2 * (x + 0.5) / width - 1) * tan_half_fov_ * image_aspect_ratio_;
        double pixel_screen_y = (1 - 2 * (y + 0.5) / height) * tan_half_fov_;
        Vector from(options_.look_from);
        Vector to(options_.look_to);
        Vector ray_direction = right_ * pixel_screen_x + up_ * pixel_screen_y - forward_;
        ray_direction.Normalize();
        Ray ray(from, ray_direction);
        return ray;
    }

private:
    CameraOptions options_;
    Vector forward_, right_, up_;
    double tan_half_fov_, image_aspect_ratio_;
};

Image Render(const std::string& filename, const CameraOptions& camera_options,
             const RenderOptions& render_options) {
    Scene scene = ReadScene(filename);
    CameraScreen screen(camera_options);
    double width = camera_options.screen_width;
    double height = camera_options.screen_height;
    std::vector<std::vector<Vector>> pixels(width, std::vector(height, Vector{}));
    for (int x{}; x < width; ++x) {
        for (int y{}; y < height; ++y) {
            auto cur_ray = screen.GetRay(x, y);
            pixels[x][y] =
                PhongIllumination(cur_ray, scene, render_options, camera_options, 0, false);
        }
    }

    ToneMapping(pixels);
    auto final_image = GammaCorrection(pixels);

    return final_image;
}

Vector PhongIllumination(const Ray& ray, const Scene& scene, const RenderOptions& render_options,
                         const CameraOptions& camera_options, int depth, bool is_inside) {
    Vector result{0, 0, 0};
    double eps = 1e-4;
    auto first_intersection = GetFirstIntersection(scene, ray);
    if (first_intersection.has_value()) {
        Intersection cur_intersection = first_intersection.value().first;
        std::string material_name = first_intersection.value().second;
        auto cur_material = &scene.GetMaterials().at(material_name);
        auto albedo = cur_material->albedo;
        result = cur_material->intensity + cur_material->ambient_color;
        for (auto light : scene.GetLights()) {
            if (IsVisible(light, cur_intersection.GetPosition(), scene)) {
                result += DiffuseByLight(cur_intersection, light, cur_material) * albedo[0];
                result +=
                    SpecularByLight(cur_intersection, light, cur_material, ray.GetDirection()) *
                    albedo[0];
            }
        }
        if (depth < render_options.depth) {
            if (albedo[1] && !is_inside) {
                Vector reflect_direction =
                    Reflect(ray.GetDirection(), cur_intersection.GetNormal());
                Vector reflect_position = cur_intersection.GetPosition() + reflect_direction * eps;
                Ray ray_reflect{reflect_position, reflect_direction};
                result += PhongIllumination(ray_reflect, scene, render_options, camera_options,
                                            depth + 1, false) *
                          albedo[1];
            }
            if (albedo[2]) {
                double eta = is_inside ? cur_material->refraction_index
                                       : 1. / cur_material->refraction_index;
                double transparency = is_inside ? 1 : albedo[2];
                auto refract_direction =
                    Refract(ray.GetDirection(), cur_intersection.GetNormal(), eta);
                auto refract_position =
                    cur_intersection.GetPosition() - cur_intersection.GetNormal() * eps;
                if (refract_direction.has_value()) {
                    Ray ray_refract{refract_position, refract_direction.value()};
                    result += PhongIllumination(ray_refract, scene, render_options, camera_options,
                                                depth + 1, !is_inside) *
                              transparency;
                }
            }
        }
    }
    return result;
}

std::optional<std::pair<Intersection, std::string>> GetFirstIntersection(const Scene& scene,
                                                                         const Ray& ray) {
    std::optional<Intersection> first = std::nullopt;
    std::string first_material;
    std::optional<std::pair<Intersection, std::string>> result_pair;
    for (auto& cur_sphere : scene.GetSphereObjects()) {
        auto cur_intersect = GetIntersection(ray, cur_sphere.sphere);
        if (cur_intersect) {
            if (!first.has_value() ||
                cur_intersect.value().GetDistance() < first.value().GetDistance()) {
                first = cur_intersect.value();
                first_material = cur_sphere.material->name;
            }
        }
    }
    std::optional<Object> first_obj = std::nullopt;
    for (auto& cur_obj : scene.GetObjects()) {
        auto cur_intersect = GetIntersection(ray, cur_obj.polygon);
        if (cur_intersect) {
            if (!first.has_value() ||
                cur_intersect.value().GetDistance() < first.value().GetDistance()) {
                first = cur_intersect.value();
                first_obj = cur_obj;
                first_material = first_obj->material->name;
            }
        }
    }
    if (first_obj) {
        if (Length(*first_obj.value().GetNormal(0)) != 0) {
            Vector bars = GetBarycentricCoords(first_obj->polygon, first->GetPosition());
            Vector new_normal = *(first_obj.value().GetNormal(0)) * bars[0] +
                                *(first_obj.value().GetNormal(1)) * bars[1] +
                                *(first_obj.value().GetNormal(2)) * bars[2];
            new_normal *= -DotProduct(new_normal, ray.GetDirection());
            new_normal.Normalize();
            first.value().ChangeNormal(new_normal);
        }
    }
    if (first) {
        return std::pair(first.value(), first_material);
    } else {
        return std::nullopt;
    }
}

bool IsVisible(const Light& light, const Vector& pos, const Scene& scene) {
    const double eps = 1e-4;
    auto ray_direction = pos - light.position;
    auto distance = Length(ray_direction);
    ray_direction.Normalize();
    Ray ray{light.position, ray_direction};
    for (auto& cur_obj : scene.GetObjects()) {
        auto object_barrier = GetIntersection({light.position, ray_direction}, cur_obj.polygon);
        if (object_barrier.has_value() && object_barrier->GetDistance() + eps < distance) {
            return false;
        }
    }
    for (auto& obj : scene.GetSphereObjects()) {
        auto sphere_barrier = GetIntersection({light.position, ray_direction}, obj.sphere);
        if (sphere_barrier.has_value() && sphere_barrier->GetDistance() + eps < distance) {
            return false;
        }
    }
    return true;
}
Vector SpecularByLight(const Intersection& intersection_point, const Light& light,
                       const Material* material, const Vector& see) {
    Vector v_l = light.position - intersection_point.GetPosition();
    v_l.Normalize();
    Vector v_r = Reflect(v_l * (-1), intersection_point.GetNormal());
    Vector v_e = see * (-1);
    v_e.Normalize();
    auto l_s =
        light.intensity * pow(std::max(0., DotProduct(v_e, v_r)), material->specular_exponent);
    return material->specular_color * l_s;
}

Vector DiffuseByLight(const Intersection& intersection_point, const Light& light,
                      const Material* material) {
    Vector v_l = light.position - intersection_point.GetPosition();
    v_l.Normalize();
    Vector l_d = light.intensity * std::max(0., DotProduct(intersection_point.GetNormal(), v_l));
    return material->diffuse_color * l_d;
}

Image GammaCorrection(std::vector<std::vector<Vector>>& pixels) {
    int width = pixels.size();
    int height = pixels[0].size();
    Image corrected(width, height);
    for (int x{}; x < width; ++x) {
        for (int y{}; y < height; ++y) {
            RGB pixel;
            pixel.r = static_cast<int>(255. * pow(pixels[x][y][0], 1. / 2.2));
            pixel.g = static_cast<int>(255. * pow(pixels[x][y][1], 1. / 2.2));
            pixel.b = static_cast<int>(255. * pow(pixels[x][y][2], 1. / 2.2));
            corrected.SetPixel(pixel, y, x);
        }
    }

    return corrected;
}

void ToneMapping(std::vector<std::vector<Vector>>& pixels) {
    double c_max = pixels[0][0][0];
    int width = pixels.size();
    int height = pixels[0].size();
    for (int x{}; x < width; ++x) {
        for (int y{}; y < height; ++y) {
            c_max = std::max(c_max, std::max({pixels[x][y][0], pixels[x][y][1], pixels[x][y][2]}));
        }
    }
    for (int x{}; x < width; ++x) {
        for (int y{}; y < height; ++y) {
            auto transform_rgb_out = [](Vector vector_in, double c) {
                return vector_in * (vector_in * (1. / (c * c)) + 1.) / (vector_in + 1.);
            };
            pixels[x][y] = transform_rgb_out(pixels[x][y], c_max);
        }
    }
}
