#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>



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


// shoelace formula
double subtriangle_area(double ax, double ay, double bx, double by, double cx, double cy) {
    return ((by - ay)*(bx + ax) + (cy - by)*(cx + bx) + (ay - cy)*(ax + cx)) / 2;
}

// modern raster approach
void triangle(int ax, int ay, double az, int bx, int by, double bz, int cx, int cy, double cz, TGAImage& framebuffer, std::vector<double> &zbuffer, TGAColor color) {
    const int width = framebuffer.width();
    const int height = framebuffer.height();

    // find bounding box, clipped to the image
    int bb_min_x = std::max(std::min(std::min(ax, bx), cx), 0);
    int bb_min_y = std::max(std::min(std::min(ay, by), cy), 0);
    int bb_max_x = std::min(std::max(std::max(ax, bx), cx), width - 1);
    int bb_max_y = std::min(std::max(std::max(ay, by), cy), height - 1);


    // get barycentric coordinates
    /** Splits triangle into 3 sub-triangles to calculates the area
        * if sub-triangle area proportial = full triangle area proportial then P is inside triangle
    */
    double total_area = subtriangle_area(ax, ay, bx, by, cx, cy);
    if (total_area < 1) return;

    // loop through bounding x
#pragma omp parallel for
    for (int x = bb_min_x; x <= bb_max_x; x++) {
        // loop through bounding y
        for (int y = bb_min_y; y <= bb_max_y; y++) {
            // check if negative (pixel not in triangle)
            double alpha = subtriangle_area(x, y, bx, by, cx, cy) / total_area;
            double beta = subtriangle_area(ax, ay, x, y, cx, cy) / total_area;
            double gamma = subtriangle_area(ax, ay, bx, by, x, y) / total_area;


            if (alpha < 0 || beta < 0 || gamma < 0) continue;

            // wireframe
            // constexpr double t = 0.1; // edge thickness in barycentric units
            // if (alpha > t && beta > t && gamma > t) continue;



            double z = alpha * az + beta * bz + gamma * cz;

            // if point is behind another skip this
            if (z <= zbuffer[x + y * width]) continue;
            zbuffer[x + y * width] = z;


            // TGAColor z_color;
            // for (int i = 0; i < 3; i++) {
            //     z_color[i] = static_cast<unsigned char>(alpha * ca[i] + beta * cb[i] + gamma * cc[i]);
            // }
        
            // place pixel
            framebuffer.set(x, y, color);
        }
    } 
}



int main(int argc, char** argv) {
    constexpr int width  = 1000;
    constexpr int height = 1000;
    TGAImage framebuffer(width, height, TGAImage::RGB);
    std::vector<double> zbuffer(width * height, -std::numeric_limits<double>::infinity());

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
            
            
            

            TGAColor rnd;
            for (int d = 0; d < 3; d++) rnd[d] = std::rand() % 255;


            triangle(first_p.x, first_p.y, first_p.z, second_p.x, second_p.y, second_p.z,third_p.x, third_p.y, third_p.z,framebuffer, zbuffer, rnd);
        }
    }
    


    // constexpr double s = width / 64.0;

    // TGAColor ca = { 0, 255, 0 };
    // TGAColor cb = { 0, 0, 255 };
    // TGAColor cc = { 255, 0, 0 };
    // int ax = 17*s, ay =  4*s;
    // int bx = 55*s, by = 39*s;
    // int cx = 23*s, cy = 59*s;

    // triangle(ax, ay, ca, bx, by, cb, cx, cy, cc, framebuffer);
    


    // normalize depth to 0-255 using the actual min/max for the zbuffer image
    double z_min = std::numeric_limits<double>::infinity();
    double z_max = -std::numeric_limits<double>::infinity();
    for (double z : zbuffer) {
        if (std::isinf(z)) continue;
        z_min = std::min(z_min, z);
        z_max = std::max(z_max, z);
    }

    TGAImage zbuffer_image(width, height, TGAImage::GRAYSCALE);
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            double z = zbuffer[x + y * width];
            
            if (std::isinf(z) || z_max <= z_min) continue;
            
            unsigned char depth = static_cast<unsigned char>((z - z_min) / (z_max - z_min) * 255);
            zbuffer_image.set(x, y, { depth });
        }
    }

    framebuffer.write_tga_file("framebuffer.tga");
    zbuffer_image.write_tga_file("zbuffer.tga");
    return 0;
}

