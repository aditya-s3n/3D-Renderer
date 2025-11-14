#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include <iostream>



constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

void line(int ax, int ay, int bx, int by, TGAImage &framebuffer, TGAColor color) {
    bool steep = std::abs(ax-bx) < std::abs(ay-by);
    if (steep) {
        std::swap(ax, ay);
        std::swap(bx, by);
    }
    
    // check if ax is in opposite direction
    if (ax > bx) {
        std::swap(ax, bx);
        std::swap(ay, by);
    }


    int y = ay;    
    int error = 0;
    int error_increase = 2 * std::abs(by-ay);
    for (int x = ax; x <= bx; x++) {
        if (steep) 
            framebuffer.set(y, x, color); // de-transpose
        else
            framebuffer.set(x, y, color);

        error += error_increase;

        if (error > bx - ax) {
            y += by > ay ? 1 : -1;
            error -= 2 * (bx-ax);
        }
    }   
}

int main(int argc, char** argv) {
    constexpr int width  = 1000;
    constexpr int height = 1000;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    std::vector<Model> model_list;


    
    for (int i = 1; i < argc; i++) {
        Model new_model;
        if (new_model.load_model(argv[i])) {
            std::cout << "CREATED " << new_model.get_vertex_list().size() << " Vertices\n";
            std::cout << "CREATED " << new_model.get_face_list().size() << " Faces\n";

            model_list.push_back(new_model);
        } else {
            std::cerr << "FAILED TO LOAD MODEL\n";
            return 1;
        }
    }
    

    
   
    
    for (size_t i = 0; i < model_list.size(); i++) {
        for (size_t j = 0; j < model_list[i].get_face_list().size(); j++) {

            int v1 = model_list[i].get_face_list()[j].v1 - 1;
            int v2 = model_list[i].get_face_list()[j].v2 - 1;
            int v3 = model_list[i].get_face_list()[j].v3 - 1;

            vec3 first_p = model_list[i].get_scaled_point(width, height, v1);
            vec3 second_p = model_list[i].get_scaled_point(width, height, v2);
            vec3 third_p = model_list[i].get_scaled_point(width, height, v3);

        
            

            // lines of wireframe
            line(first_p.x, first_p.y, second_p.x, second_p.y, framebuffer, red);
            line(second_p.x, second_p.y, third_p.x, third_p.y , framebuffer, red);
            line(first_p.x, first_p.y, third_p.x, third_p.y, framebuffer, red);
            line(first_p.x, first_p.y , second_p.x , second_p.y , framebuffer, red);

            // vertex of wireframe
            framebuffer.set(first_p.x , first_p.y , white);
            framebuffer.set(second_p.x , second_p.y, white);
            framebuffer.set(third_p.x , third_p.y , white);
        }
    }

    


    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}

