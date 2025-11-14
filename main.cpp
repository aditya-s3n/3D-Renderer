#include <cmath>
#include <ctime>
#include <cstdlib>
#include "tgaimage.h"

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
    constexpr int width  = 64;
    constexpr int height = 64;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    std::srand(std::time({}));
    for (int i=0; i<(1<<24); i++) {
        int ax = rand()%width, ay = rand()%height;
        int bx = rand()%width, by = rand()%height;
        line(ax, ay, bx, by, framebuffer, { rand()%255, rand()%255, rand()%255, rand()%255 });
    }
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}

