#include "model.h"



bool Model::load_model(std::string file_path) {
    std::ifstream file(file_path);
    if (!file) {
        return false;
    }

    std::string line_s;
    while(std::getline(file, line_s)) {
        std::stringstream ss(line_s);
        std::string type;
        ss >> type;

        if (type == "v") {
            vec3 v;
            ss >> v.x >> v.y >> v.z;
            vertices.push_back(v);
        } else if (type == "f") {
            face f;

            std::string a, b, c;
            ss >> a >> b >> c;  // example: "6/1/6"

            auto parseIndex = [](const std::string &s) {
                return std::stoi(s.substr(0, s.find('/')));
            };

            f.v1 = parseIndex(a);
            f.v2 = parseIndex(b);
            f.v3 = parseIndex(c);

            faces.push_back(f);
        }
    }

    return true;
}

vec3 Model::get_scaled_point(int width, int height, int v_index) {
    vec3 scaled_vals;

    scaled_vals.x = vertices[v_index].x * (width / 2) + (width / 2);
    scaled_vals.y = vertices[v_index].y * (height / 2) + (height / 2);
    scaled_vals.z = vertices[v_index].z;


    return scaled_vals;
}

