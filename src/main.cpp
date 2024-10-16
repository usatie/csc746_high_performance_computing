#include <iostream>
#include "vec3.h"
#include "color.h"

int main() {
    // Image
    int image_width = 256;
    int image_height = 256;

    // Render
    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; j--) {
        std::clog << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; i++) {
            auto pixel_color = color(double(i) / (image_width - 1), double(j) / (image_height - 1), 0.25);
            write_color(std::cout, pixel_color);
        }
    }
    std::clog << "\nDone.\n";
    return 0;
}
