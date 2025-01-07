#ifndef COLOR_H
#define COLOR_H

#include "interval.h"
#include "vec3.h"

using color = vec3;

// We use gamme 2 as our transformation
inline double linear_to_gamma(double linear_component) {
  if (linear_component > 0)
    return std::sqrt(linear_component);
  return 0;
}

void write_color(std::ostream &out, color pixel_color) {
  auto r = pixel_color.x();
  auto g = pixel_color.y();
  auto b = pixel_color.z();

  // Apply a linear to gamma transformation.
  r = linear_to_gamma(r);
  g = linear_to_gamma(g);
  b = linear_to_gamma(b);

  // Translate the [0,1] component values to the byte range [0,255].
  static const interval intensity(0.0, 0.999);
  int rbyte = static_cast<int>(256 * intensity.clamp(r));
  int gbyte = static_cast<int>(256 * intensity.clamp(g));
  int bbyte = static_cast<int>(256 * intensity.clamp(b));

  // Write out the pixel color components.
  out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

inline void sdl_write_color(uint8_t *pixel, color pixel_color) {
  auto r = pixel_color.x();
  auto g = pixel_color.y();
  auto b = pixel_color.z();

  // Apply a linear to gamma transformation.
  r = linear_to_gamma(r);
  g = linear_to_gamma(g);
  b = linear_to_gamma(b);

  // Translate the [0,1] component values to the byte range [0,255].
  static const interval intensity(0.0, 0.999);
  uint8_t rbyte = static_cast<uint8_t>(256 * intensity.clamp(r));
  uint8_t gbyte = static_cast<uint8_t>(256 * intensity.clamp(g));
  uint8_t bbyte = static_cast<uint8_t>(256 * intensity.clamp(b));

  pixel[0] = rbyte;
  pixel[1] = gbyte;
  pixel[2] = bbyte;
}

#endif
