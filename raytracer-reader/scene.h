#pragma once

#include <material.h>
#include <vector.h>
#include <object.h>
#include <light.h>
#include <triangle.h>
#include <sphere.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <utility>

#include <vector>
#include <map>
#include <string>
#include <filesystem>

std::array<int, 3> GoodSplit(const std::string& string, size_t v_size, size_t n_size,
                             const std::string& delimiter = "/");

class Scene {
public:
    void AddObject(Object& obj) {
        objects_.push_back(obj);
    }

    void AddSphereObject(SphereObject& sphere_object) {
        spheres_.push_back(sphere_object);
    }

    void AddLight(Light& light) {
        lights_.push_back(light);
    }

    void AddMaterials(std::map<std::string, Material> material) {
        materials_.insert(material.begin(), material.end());
    }

    const std::vector<Object>& GetObjects() const {
        return objects_;
    };
    const std::vector<SphereObject>& GetSphereObjects() const {
        return spheres_;
    };
    const std::vector<Light>& GetLights() const {
        return lights_;
    };
    const std::map<std::string, Material>& GetMaterials() const {
        return materials_;
    };

private:
    std::vector<Object> objects_;
    std::vector<SphereObject> spheres_;
    std::vector<Light> lights_;
    std::map<std::string, Material> materials_;
};

inline std::map<std::string, Material> ReadMaterials(std::string_view filename) {
    std::map<std::string, Material> materials;
    std::ifstream mtl_file(filename.data());
    std::string line;
    Material current_material;

    // parse string lines of mtl file
    while (std::getline(mtl_file, line)) {
        if (!line.empty()) {
            std::istringstream iss(line);
            std::string mtl_token;
            iss >> mtl_token;

            if (mtl_token.front() != '#') {
                if (mtl_token == "newmtl") {
                    std::string material_name;
                    iss >> material_name;
                    if (!current_material.name.empty()) {
                        materials.emplace(current_material.name, current_material);
                        current_material = Material();
                    }
                    current_material.name = material_name;
                }
                if (mtl_token == "Ka") {
                    double p1, p2, p3;
                    iss >> p1 >> p2 >> p3;
                    current_material.ambient_color = Vector({p1, p2, p3});
                }
                if (mtl_token == "Kd") {
                    double p1, p2, p3;
                    iss >> p1 >> p2 >> p3;
                    current_material.diffuse_color = Vector({p1, p2, p3});
                }
                if (mtl_token == "Ks") {
                    double p1, p2, p3;
                    iss >> p1 >> p2 >> p3;
                    current_material.specular_color = Vector({p1, p2, p3});
                }
                if (mtl_token == "Ke") {
                    double p1, p2, p3;
                    iss >> p1 >> p2 >> p3;
                    current_material.intensity = Vector({p1, p2, p3});
                }
                if (mtl_token == "Ns") {
                    double p;
                    iss >> p;
                    current_material.specular_exponent = p;
                }
                if (mtl_token == "Ni") {
                    double p;
                    iss >> p;
                    current_material.refraction_index = p;
                }
                if (mtl_token == "al") {
                    double p1, p2, p3;
                    iss >> p1 >> p2 >> p3;
                    current_material.albedo = {p1, p2, p3};
                }
            }
        }
    }
    materials.emplace(current_material.name, current_material);

    return materials;
}

inline Scene ReadScene(const std::string& filename) {
    std::ifstream obj_file(filename.data());
    std::filesystem::path path(filename);
    std::string dir = path.parent_path().string();

    std::string line;
    std::string current_material;

    std::vector<Vector> vertices;
    std::vector<Vector> normals;
    std::vector<Light> lights;
    std::vector<SphereObject> sphere_objects;
    std::vector<Object> objects;
    std::map<std::string, Material> materials;

    Scene scene;
    // parse string lines of obj file
    while (std::getline(obj_file, line)) {
        if (!line.empty()) {
            std::istringstream iss(line);
            std::string obj_token;
            iss >> obj_token;

            if (obj_token.front() != '#') {
                if (obj_token == "mtllib") {
                    std::string mtl_file;
                    iss >> mtl_file;
                    auto materials_file = ReadMaterials(dir + "/" + mtl_file);
                    scene.AddMaterials(materials_file);
                }

                if (obj_token == "usemtl") {
                    iss >> current_material;
                }

                if (obj_token == "v") {
                    double x, y, z;
                    iss >> x >> y >> z;
                    vertices.push_back(Vector({x, y, z}));
                }

                if (obj_token == "vn") {
                    double x, y, z;
                    iss >> x >> y >> z;
                    normals.push_back(Vector({x, y, z}));
                }

                if (obj_token == "S") {
                    double x, y, z, r;
                    iss >> x >> y >> z >> r;
                    Sphere sphere({x, y, z}, r);
                    SphereObject sphere_object(&scene.GetMaterials().at(current_material), sphere);
                    scene.AddSphereObject(sphere_object);
                }

                if (obj_token == "P") {
                    double x, y, z, r, g, b;
                    iss >> x >> y >> z >> r >> g >> b;
                    Light light(Vector({x, y, z}), Vector({r, g, b}));
                    //                    lights.push_back(light);
                    scene.AddLight(light);
                }

                if (obj_token == "f") {
                    std::vector<std::string> f_params((std::istream_iterator<std::string>(iss)),
                                                      {});
                    size_t v_size = vertices.size();
                    size_t n_size = normals.size();
                    std::vector<Vector> f_vertices;
                    std::vector<Vector> f_normals;
                    std::array<int, 3> params_cast0 = GoodSplit(f_params[0], v_size, n_size);

                    for (size_t i = 1; i < f_params.size() - 1; ++i) {
                        std::array<int, 3> cur_params_cast = GoodSplit(f_params[i], v_size, n_size);
                        std::array<int, 3> next_params_cast =
                            GoodSplit(f_params[i + 1], v_size, n_size);

                        Triangle polygon = {vertices[params_cast0[0]], vertices[cur_params_cast[0]],
                                            vertices[next_params_cast[0]]};

                        std::vector<Vector> nrmls = {Vector{0, 0, 0}, Vector{0, 0, 0},
                                                     Vector{0, 0, 0}};
                        // if all vertices have normals => interpolate data
                        if (params_cast0[2] != -1 && cur_params_cast[2] != -1 &&
                            next_params_cast[2] != -1) {
                            nrmls[0] = std::move(normals[params_cast0[2]]);
                            nrmls[1] = std::move(normals[cur_params_cast[2]]);
                            nrmls[2] = std::move(normals[next_params_cast[2]]);
                        } else {
                        }
                        Object cur_obj(&scene.GetMaterials().at(current_material), polygon, nrmls);
                        scene.AddObject(cur_obj);
                    }
                }
            }
        }
    }
    return scene;
}

std::array<int, 3> GoodSplit(const std::string& string, size_t v_size, size_t n_size,
                             const std::string& delimiter) {
    int start = 0;
    auto end = string.find(delimiter);
    std::vector<std::string> str_vector;

    while (end != std::string::npos) {
        str_vector.push_back(string.substr(start, end - start));
        start = end + delimiter.length();
        end = string.find(delimiter, start);
    }
    str_vector.push_back(string.substr(start, end));

    std::array<int, 3> res;

    if (str_vector.size() == 1 || str_vector.size() == 2) {  // situation -- Input: 1 or 1/
        res = {std::stoi(str_vector[0]), 0, 0};
    }

    if (str_vector.size() == 3) {  // situation -- Input: 1// or 1/2/3 or 1//3
        if (str_vector[1].empty()) {
            res = {std::stoi(str_vector[0]), 0, std::stoi(str_vector[2])};
        } else {
            res = {std::stoi(str_vector[0]), std::stoi(str_vector[1]), std::stoi(str_vector[2])};
        }
    }

    // vector indices from zero
    res[0] = res[0] < 0 ? v_size + res[0] : res[0] - 1;
    res[2] = res[2] < 0 ? n_size + res[2] : res[2] - 1;

    return res;
}
