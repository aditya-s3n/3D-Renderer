#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "vectors.h"
#include "matrices.h"

struct face {
    int v1;
    int v2;
    int v3;
};

class Model {

    private:
        std::vector<vec3> vertices;
        std::vector<face> faces;


    public:
        Model() = default;
        ~Model() = default;


        bool load_model(std::string file_path);

        vec3 get_scaled_point (int width, int height, int v_index);


        std::vector<vec3> get_vertex_list() { return vertices; }
        std::vector<face> get_face_list() { return faces; }

        vec3 rotation(vec3 v);

    

};