#include "rtweekend.h"
#include <algorithm>
#include <unistd.h> // getopt
// After including rtweekend.h, we can include the other headers.
#include "bvh.h"
#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"

#include <iomanip>
// Custom locale to format numbers with commas
struct comma_numpunct : std::numpunct<char> {
protected:
  virtual char do_thousands_sep() const override {
    return ','; // Comma as the thousands separator
  }

  virtual std::string do_grouping() const override {
    return "\3"; // Groups of 3 digits
  }
};

size_t calculate_hittable_size(const hittable *object);

size_t calculate_sphere_size(const sphere *s) {
  size_t size = sizeof(*s); // Size of the sphere itself
  if (s->mat) {
    // Add the size of the material object
    size += sizeof(*(s->mat)); // Material size (approximation)
  }
  return size;
}

size_t calculate_bvh_node_size(const bvh_node *bvh) {
  size_t size = sizeof(*bvh); // Size of the BVH node itself
  if (bvh->left) {
    size += calculate_hittable_size(bvh->left.get());
  }
  if (bvh->right) {
    size += calculate_hittable_size(bvh->right.get());
  }
  return size;
}

size_t calculate_hittable_list_size(const hittable_list *list) {
  size_t size = sizeof(*list); // Size of the list itself
  for (const auto &obj : list->objects) {
    size += calculate_hittable_size(obj.get());
  }
  return size;
}

size_t calculate_hittable_size(const hittable *object) {
  if (const sphere *s = dynamic_cast<const sphere *>(object)) {
    return calculate_sphere_size(s);
  } else if (const bvh_node *bvh = dynamic_cast<const bvh_node *>(object)) {
    return calculate_bvh_node_size(bvh);
  } else if (const hittable_list *list =
                 dynamic_cast<const hittable_list *>(object)) {
    return calculate_hittable_list_size(list);
  } else {
    return sizeof(*object); // Default for unknown types
  }
}

size_t calculate_scene_size(const hittable_list &world) {
  return calculate_hittable_list_size(&world);
}

void setup_world(hittable_list &world, camera &cam, int sphere_grid_size) {
  auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
  world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

  std::vector<point3> large_sphere_centers(
      {point3(0, 1, 0), point3(-4, 1, 0), point3(4, 1, 0)});
  std::vector<point3> centers;
  for (int a = -sphere_grid_size; a < sphere_grid_size; a++) {
    for (int b = -sphere_grid_size; b < sphere_grid_size; b++) {
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
  world = hittable_list(make_shared<bvh_node>(world));

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

// omp_sched_monotonic = 0x80000000u can be combined with these kinds
#define OMP_SCHED_MASK 0x7

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
  int loop_order = LOOPORDER_YX;
  int sphere_grid_size = 11;
  while ((c = getopt(argc, argv, "S:W:H:D:o:l:g:")) != -1) {
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
    case 'l':
      if (std::string(optarg) == "XY")
        loop_order = LOOPORDER_XY;
      else if (std::string(optarg) == "YX")
        loop_order = LOOPORDER_YX;
      else
        loop_order = -999;
      break;
    case 'g':
      sphere_grid_size = std::atoi(optarg == NULL ? "-999" : optarg);
    }
  }
  if (samples_per_pixel <= 0 || width <= 0 || height <= 0 || max_depth <= 0 ||
      loop_order < 0 || sphere_grid_size <= 0) {
    std::cerr
        << "Usage: " << argv[0]
        << " [-S samples_per_pixel] [-W width] [-H height] [-D "
           "max_depth] [-o filename] [-l loop_order] [-g sphere_grid_size]\n";
    return 1;
  }

  const int collapse = OMP_COLLAPSE;
  // Print configuration in a single line
  std::clog << "Sample: " << samples_per_pixel << ", Width: " << width
            << ", Height: " << height << ", Depth: " << max_depth
            << "Sphere Grid Size: " << sphere_grid_size;
  if (collapse) {
    std::clog << ", Collapse: enabled";
  } else {
    std::clog << ", Collapse: disabled";
  }
  if (loop_order == LOOPORDER_YX) {
    std::clog << ", Loop Order: YX";
  } else if (loop_order = LOOPORDER_XY) {
    std::clog << ", Loop Order: XY";
  }

#pragma omp parallel
  {
    if (omp_get_thread_num() == 0) {
      std::clog << ", Threads: " << omp_get_num_threads();
      cam.nthreads = omp_get_num_threads();
      omp_sched_t kind;
      int chunk_size;
      omp_get_schedule(&kind, &chunk_size);
      if ((kind & OMP_SCHED_MASK) == omp_sched_static) {
        std::clog << ", Schedule: Static, chunk size: " << chunk_size
                  << std::endl;
      } else if ((kind & OMP_SCHED_MASK) == omp_sched_dynamic) {
        std::clog << ", Schedule: Dynamic, chunk size: " << chunk_size
                  << std::endl;
      } else if ((kind & OMP_SCHED_MASK) == omp_sched_guided) {
        std::clog << ", Schedule: Guided, chunk size: " << chunk_size
                  << std::endl;
      } else if ((kind & OMP_SCHED_MASK) == omp_sched_auto) {
        std::clog << ", Schedule: Auto, chunk size: " << chunk_size
                  << std::endl;
      } else {
        std::clog << ", Schedule: Unknown(" << kind
                  << "), chunk size: " << chunk_size << std::endl;
      }
    }
  }

  setup_world(world, cam, sphere_grid_size);
  cam.samples_per_pixel = samples_per_pixel;
  cam.image_width = width;
  cam.max_depth = max_depth;
  cam.aspect_ratio = double(width) / double(height);
  cam.output_filename = output_filename;
  cam.loop_order = loop_order;

  // cam.render(world);
  std::cout << "Sizes of primitive types:\n";
  std::cout << "Size of double:                " << sizeof(double)
            << " bytes\n";
  std::cout << "Size of int:                   " << sizeof(int) << " bytes\n";
  std::cout << "Size of bool:                  " << sizeof(bool) << " bytes\n";

  std::cout << "\nSizes of standard library types:\n";
  std::cout << "Size of std::shared_ptr<void>: "
            << sizeof(std::shared_ptr<void>) << " bytes\n";
  std::cout << "Size of std::vector<void*>:    " << sizeof(std::vector<void *>)
            << " bytes\n";
  std::cout << "Size of std::string:           " << sizeof(std::string)
            << " bytes\n";

  std::cout << "\nSizes of custom classes:\n";
  std::cout << "Size of aabb:                  " << sizeof(aabb) << " bytes\n";
  std::cout << "Size of interval:              " << sizeof(interval)
            << " bytes\n";
  std::cout << "Size of point3 (vec3):         " << sizeof(point3)
            << " bytes\n";
  std::cout << "Size of vec3:                  " << sizeof(vec3) << " bytes\n";
  std::cout << "Size of ray:                   " << sizeof(ray) << " bytes\n";
  std::cout << "Size of color (vec3):          " << sizeof(color) << " bytes\n";
  std::cout << "Size of hit_record:            " << sizeof(hit_record)
            << " bytes\n";
  std::cout << "Size of hittable:              " << sizeof(hittable)
            << " bytes\n";
  std::cout << "Size of hittable_list:         " << sizeof(hittable_list)
            << " bytes\n";
  std::cout << "Size of bvh_node:              " << sizeof(bvh_node)
            << " bytes\n";
  std::cout << "Size of camera:                " << sizeof(camera)
            << " bytes\n";
  std::cout << "Size of material:              " << sizeof(material)
            << " bytes\n";
  std::cout << "Size of lambertian:            " << sizeof(lambertian)
            << " bytes\n";
  std::cout << "Size of metal:                 " << sizeof(metal) << " bytes\n";
  std::cout << "Size of dielectric:            " << sizeof(dielectric)
            << " bytes\n";
  std::cout << "Size of sphere:                " << sizeof(sphere)
            << " bytes\n";

  int world_size = calculate_scene_size(world);
  // Print out the world size comma separated
  // Apply the custom locale
  std::locale comma_locale(std::locale(), new comma_numpunct());
  std::cout.imbue(comma_locale);
  int world_size_kb = world_size / 1024;
  int world_size_mb = world_size_kb / 1024;
  std::cout << "World size: " << world_size << " bytes, " << world_size_kb
            << " KB, " << world_size_mb << " MB" << std::endl;
  return 0;
}
