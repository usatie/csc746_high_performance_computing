#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "likwid-stuff.h"
#include "material.h"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <omp.h>

#define LOOPORDER_YX 0
#define LOOPORDER_XY 1

class camera {
public:
  /* Public Camera Parameters Here */
  double aspect_ratio = 1.0;         // Ratio of image width over height
  int image_width = 100;             // Rendered image width
  int samples_per_pixel = 10;        // Count of random samples for each pixel
  int max_depth = 10;                // Maximum number of ray bounces into scene
  uint64_t total_rays_simulated = 0; // Total number of rays simulated

  double vfov = 90.0;                // Vertical view angle (filed of view)
  point3 lookfrom = point3(0, 0, 0); // Point camera is looking from
  point3 lookat = point3(0, 0, -1);  // Point camera is looking at
  vec3 vup = vec3(0, 1, 0);          // Camera-relative "up" vector

  double defocus_angle = 0; // Variation angle of rays through each pixel
  double focus_dist =
      10; // Distance from camera lookfrom point to plane of focus
  std::string output_filename = "image.ppm";
  int loop_order = LOOPORDER_YX;
  int nthreads = 1;

  void write_ppm(const std::vector<color> &image) {
    // Write the image to the standard output stream
    std::ofstream ostrm(output_filename, std::ios::binary);
    ostrm << "P3\n" << image_width << " " << image_height << "\n255\n";
    for (int j = 0; j < image_height; j++) {
      for (int i = 0; i < image_width; ++i) {
        color pixel_color = image[j * image_width + i];
        write_color(ostrm, pixel_samples_scale * pixel_color);
      }
    }
  }
  void render(const hittable &world) {
    initialize();

    std::vector<color> image(image_width * image_height);

    std::chrono::time_point<std::chrono::high_resolution_clock> start_time =
        std::chrono::high_resolution_clock::now();
    int imax = image_width;
    int jmax = image_height;
    if (loop_order == LOOPORDER_YX) {
      std::swap(imax, jmax);
    }
    std::vector<std::chrono::duration<double>> runtimes(nthreads);
#if OMP_COLLAPSE
#pragma omp parallel for collapse(2) schedule(runtime)
#else
#pragma omp parallel for schedule(runtime)
#endif
    for (int i = 0; i < imax; i++) {
      for (int j = 0; j < jmax; j++) {
        std::chrono::time_point<std::chrono::high_resolution_clock> s =
            std::chrono::high_resolution_clock::now();
        color pixel_color = color(0, 0, 0);
        int x = i;
        int y = j;
        if (loop_order == LOOPORDER_YX) {
          std::swap(x, y);
        }
        for (int s = 0; s < samples_per_pixel; s++) {
          ray r = get_ray(x, y);
          pixel_color += ray_color(r, max_depth, world);
        }
        image[y * image_width + x] += pixel_color;
        std::chrono::time_point<std::chrono::high_resolution_clock> e =
            std::chrono::high_resolution_clock::now();
        runtimes[omp_get_thread_num()] += (e - s);
      }
    }
    std::chrono::time_point<std::chrono::high_resolution_clock> end_time =
        std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;
    std::clog << std::endl;
    std::clog << "Elapsed time: " << elapsed_time.count() << " " << std::endl;
    std::clog << "Total rays simulated: " << total_rays_simulated << std::endl;
    std::clog << "Rays Simulated per second: "
              << static_cast<int>(total_rays_simulated / elapsed_time.count())
              << std::endl;
    std::chrono::duration<double> min = runtimes[0], sum(0), max(0);
    for (int i = 0; i < nthreads; i++) {
      std::clog << "(#" << i << "): " << runtimes[i].count() << "s"
                << std::endl;
      min = std::min(min, runtimes[i]);
      max = std::max(max, runtimes[i]);
      sum += runtimes[i];
    }
    std::clog << "Min: " << min.count() << "s"
              << " | Max: " << max.count() << "s"
              << " | Avg: " << sum.count() / nthreads << "s" << std::endl;

    write_ppm(image);
  }

private:
  /* Private Camera Parameters Here */
  int image_height;           // Rendered image height
  double pixel_samples_scale; // Color scale factor for sum of pixel samples
  point3 center;              // Camera center
  point3 pixel00_loc;         // Location of pixel (0, 0)
  vec3 pixel_delta_u;         // Offset to pixel to the right
  vec3 pixel_delta_v;         // Offset to pixel below
  vec3 u, v, w;               // Camera frame basis vectors
  vec3 defocus_disk_u;        // Defocus disk horizontal radius
  vec3 defocus_disk_v;        // Defocus disk vertical radius

  void initialize() {
    image_height = static_cast<int>(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    pixel_samples_scale = 1.0 / samples_per_pixel;

    center = lookfrom;

    // Determin viewport dimensions
    auto theta = degrees_to_radians(vfov);
    auto h = std::tan(theta / 2);
    auto view_port_height = 2.0 * h * focus_dist;
    auto view_port_width =
        view_port_height * (double(image_width) / image_height);

    // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
    w = unit_vector(lookfrom - lookat);
    u = unit_vector(cross(vup, w));
    v = cross(w, u);

    // Calculate the vectors accross the horizontal and down the vertical
    // viewport edges.
    auto viewport_u = view_port_width * u;
    auto viewport_v = view_port_height * -v;

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    pixel_delta_u = viewport_u / image_width;
    pixel_delta_v = viewport_v / image_height;

    // Calculate the location of the upper left pixel.
    auto viewport_upper_left =
        center - viewport_u / 2 - viewport_v / 2 - focus_dist * w;
    pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Calculate the camera defocus disk basis vectors.
    auto defocus_radius =
        focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
    defocus_disk_u = defocus_radius * u;
    defocus_disk_v = defocus_radius * v;
  }

  ray get_ray(int i, int j) const {
    auto offset = sample_square();
    auto pixel_sample = pixel00_loc + (i + offset.x()) * pixel_delta_u +
                        (j + offset.y()) * pixel_delta_v;
    auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
    auto ray_direction = pixel_sample - ray_origin;
    return ray(ray_origin, ray_direction);
  }

  vec3 sample_square() const {
    // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit
    // square.
    return vec3(random_double() - 0.5, random_double() - 0.5, 0);
  }

  point3 defocus_disk_sample() const {
    // Returns a random point in the camera defocus disk.
    auto p = random_in_unit_disk();
    return center + p[0] * defocus_disk_u + p[1] * defocus_disk_v;
  }

  color ray_color(const ray &r, int depth, const hittable &world) {
    if (depth <= 0)
      return color(0, 0, 0);
    hit_record rec;
#pragma omp atomic
    total_rays_simulated++;

    if (world.hit(r, interval(0.001, infinity), rec)) {
      ray scattered;
      color attenuation;
      // If the ray scatters, recursively call ray_color with the scattered ray.
      if (rec.mat->scatter(r, rec, attenuation, scattered))
        return attenuation * ray_color(scattered, depth - 1, world);
      // If the ray does not scatter, return black.
      return color(0, 0, 0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
  }
};

#endif
