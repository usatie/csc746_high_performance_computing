#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "material.h"
#include <chrono>
#include <iomanip>
#include <omp.h>
#ifdef SDL2
#include <SDL2/SDL.h>
#endif
#define DEBUG 0

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

  void write_ppm(const std::vector<color> &image) {
    // Write the image to the standard output stream
    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";
    for (int j = 0; j < image_height; j++) {
      for (int i = 0; i < image_width; ++i) {
        color pixel_color = image[j * image_width + i];
        write_color(std::cout, pixel_samples_scale * pixel_color);
      }
    }
  }
  void render(const hittable &world) {
    initialize();

    std::vector<color> image(image_width * image_height);

#ifdef SDL2
    bool quit = false;
    SDL_Event e;
    omp_lock_t sdl_lock;
    omp_init_lock(&sdl_lock);
#endif
#if DEBUG
    int progress = 0;
#endif

    std::chrono::time_point<std::chrono::high_resolution_clock> start_time =
        std::chrono::high_resolution_clock::now();
#pragma omp parallel for collapse(2) schedule(dynamic, 1)
    for (int j = 0; j < image_height; j++) {
#ifdef SDL2
      if (omp_get_thread_num() == 0) {
        while (SDL_PollEvent(&e) != 0) {
          if (e.type == SDL_QUIT) {
            quit = true;
          }
        }
        if (quit)
          continue;
      }
#endif
      for (int i = 0; i < image_width; i++) {
        color pixel_color = color(0, 0, 0);
        int x = i;
        int y = j;
        for (int s = 0; s < samples_per_pixel; s++) {
          ray r = get_ray(x, y);
          pixel_color += ray_color(r, max_depth, world);
        }
        image[y * image_width + x] += pixel_color;
      }
#ifdef SDL2
      if (omp_test_lock(&sdl_lock)) {
        render_on_sdl(image);
        omp_unset_lock(&sdl_lock);
      }
#endif
#if DEBUG
#pragma omp critical
      {
        progress += image_width;
        if (progress % 2 == 0)
          std::clog << "\rPixels remaining: "
                    << (image_height * image_width - progress) << ' '
                    << std::flush;
      }
#endif
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

#ifdef SDL2
    while (!quit) {
      while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
          quit = true;
        }
      }
      render_on_sdl(image);
    }
    omp_destroy_lock(&sdl_lock);
    cleanup();
#endif
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
#ifdef SDL2
  /* SDL2 Rendering Parameters */
  SDL_Window *window;     // SDL window
  SDL_Renderer *renderer; // SDL renderer
  SDL_Texture *texture;   // SDL texture
  void render_on_sdl(const std::vector<color> &image) {
    // Convert to SDL surface format (RGB 8-bit per channel)
    std::vector<uint8_t> pixels(image_width * image_height * 3);
    for (int j = 0; j < image_height; j++) {
      for (int i = 0; i < image_width; ++i) {
        color pixel_color = image[j * image_width + i] * pixel_samples_scale;
        sdl_write_color(&pixels[3 * (j * image_width + i)], pixel_color);
      }
    }
    // Update SDL texture with ray traced image
    SDL_UpdateTexture(texture, nullptr, pixels.data(), image_width * 3);

    // Clear renderer and copy texture to the window
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
  }
  void cleanup() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }
#endif

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

#ifdef SDL2
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError()
                << std::endl;
      return;
    }

    window = SDL_CreateWindow("Ray Tracing", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, image_width,
                              image_height, SDL_WINDOW_SHOWN);
    if (!window) {
      std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError()
                << std::endl;
      return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
      std::cerr << "Renderer could not be created! SDL_Error: "
                << SDL_GetError() << std::endl;
      SDL_DestroyWindow(window);
      SDL_Quit();
      return;
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24,
                                SDL_TEXTUREACCESS_STREAMING, image_width,
                                image_height);
    if (!texture) {
      std::cerr << "Texture could not be created! SDL_Error: " << SDL_GetError()
                << std::endl;
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
      SDL_Quit();
      return;
    }
#endif
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
