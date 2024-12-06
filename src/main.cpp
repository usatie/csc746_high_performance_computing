#include "rtweekend.h"
#include <algorithm>
#include <unistd.h> // getopt
// After including rtweekend.h, we can include the other headers.
#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "likwid-stuff.h"
#include "sphere.h"

void setup_world(hittable_list &world, camera &cam) {
  auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
  world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

  std::vector<point3> large_sphere_centers(
      {point3(0, 1, 0), point3(-4, 1, 0), point3(4, 1, 0)});
  std::vector<point3> centers;
  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      auto choose_mat = random_double();
      point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
      // Only add a sphere if it is not too close to another one.
      while (std::find_if(centers.begin(), centers.end(),
                          [center](point3 c) {
                            return (c - center).length() < 0.4;
                          }) != centers.end() ||
             std::find_if(large_sphere_centers.begin(),
                          large_sphere_centers.end(), [center](point3 c) {
                            return (c - center).length() < 1.2;
                          }) != large_sphere_centers.end()) {
        center =
            point3(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
      }

      if ((center - point3(4, 0.2, 0)).length() > 0.9) {
        centers.push_back(center);
        shared_ptr<material> sphere_material;

        if (choose_mat > 0.25) {
          // diffuse
          auto albedo = color::random() * color::random();
          sphere_material = make_shared<lambertian>(albedo);
          world.add(make_shared<sphere>(center, 0.2, sphere_material));
        } else if (choose_mat > 0.05) {
          // metal
          auto albedo = color::random(0.5, 1);
          auto fuzz = random_double(0, 0.5);
          sphere_material = make_shared<metal>(albedo, fuzz);
          world.add(make_shared<sphere>(center, 0.2, sphere_material));
        } else {
          // glass
          sphere_material = make_shared<dielectric>(1.5);
          world.add(make_shared<sphere>(center, 0.2, sphere_material));
        }
      }
    }
  }

  auto material1 = make_shared<dielectric>(1.5);
  world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

  auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
  world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

  auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
  world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 1200;
  cam.samples_per_pixel = 5;
  cam.max_depth = 50;

  cam.vfov = 20;
  cam.lookfrom = point3(13, 2, 3);
  cam.lookat = point3(0, 0, 0);
  cam.vup = vec3(0, 1, 0);

  cam.defocus_angle = 0.6;
  cam.focus_dist = 10.0;
}

int main(int argc, char **argv) {
  hittable_list world;
  camera cam;

  // Parse command line arguments
  // -S n: set the number of samples per pixel to n
  // -W n: set the width of the image to n
  // -H n: set the height of the image to n
  // -D n: set the maximum depth of the ray to n
  int samples_per_pixel = 32;
  int width = 512;
  int height = 288;
  int max_depth = 32;
  int c;
  std::string output_filename = "image.ppm";
  while ((c = getopt(argc, argv, "S:W:H:D:o:")) != -1) {
    switch (c) {
    case 'S':
      samples_per_pixel = std::atoi(optarg == NULL ? "-999" : optarg);
      break;
    case 'W':
      width = std::atoi(optarg == NULL ? "-999" : optarg);
      break;
    case 'H':
      height = std::atoi(optarg == NULL ? "-999" : optarg);
      break;
    case 'D':
      max_depth = std::atoi(optarg == NULL ? "-999" : optarg);
      break;
    case 'o':
      if (optarg)
        output_filename = std::string(optarg);
      break;
    }
  }
  if (samples_per_pixel <= 0 || width <= 0 || height <= 0 || max_depth <= 0) {
    std::cerr << "Usage: " << argv[0]
              << " [-S samples_per_pixel] [-W width] [-H height] [-D "
                 "max_depth] [-o filename]\n";
    return 1;
  }

  const int collapse = OMP_COLLAPSE;
  // Print configuration in a single line
  std::clog << "==============================================================="
               "==========="
            << std::endl;
  std::clog << "Sample: " << samples_per_pixel << ", Width: " << width
            << ", Height: " << height << ", Depth: " << max_depth;
  if (collapse) {
    std::clog << ", Collapse: enabled";
  } else {
    std::clog << ", Collapse: disabled";
  }

#pragma omp parallel
  {
    if (omp_get_thread_num() == 0) {
      std::clog << ", Threads: " << omp_get_num_threads();
      omp_sched_t schedule;
      int chunk_size;
      omp_get_schedule(&schedule, &chunk_size);
      if (schedule & omp_sched_static) {
        std::clog << ", Schedule: Static, chunk size: " << chunk_size
                  << std::endl;
      } else if (schedule & omp_sched_dynamic) {
        std::clog << ", Schedule: Dynamic, chunk size: " << chunk_size
                  << std::endl;
      } else if (schedule & omp_sched_guided) {
        std::clog << ", Schedule: Guided, chunk size: " << chunk_size
                  << std::endl;
      } else if (schedule & omp_sched_auto) {
        std::clog << ", Schedule: Auto, chunk size: " << chunk_size
                  << std::endl;
      } else {
        std::clog << ", Schedule: Unknown, chunk size: " << chunk_size
                  << std::endl;
      }
    }
  }

  // initialize the LIKWID marker API in a serial code region once in the
  // beginning
  LIKWID_MARKER_INIT;
#pragma omp parallel
  {
    // Each thread must add itself to the Marker API, therefore must be
    // in parallel region
    LIKWID_MARKER_THREADINIT;
    // Register region name
    LIKWID_MARKER_REGISTER(MY_MARKER_REGION_NAME);
  }
  setup_world(world, cam);
  cam.samples_per_pixel = samples_per_pixel;
  cam.image_width = width;
  cam.max_depth = max_depth;
  cam.aspect_ratio = double(width) / double(height);
  cam.output_filename = output_filename;

  cam.render(world);

  // Close Marker API and write results to file for further evaluation done
  // by likwid-perfctr
  LIKWID_MARKER_CLOSE;
  return 0;
}
