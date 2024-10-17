#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
public:
  sphere(const point3 &center, double radius)
      : center(center), radius(std::fmax(0, radius)){};
  bool hit(const ray &r, double t_min, double t_max,
           hit_record &rec) const override {
    vec3 oc = r.origin() - center;
    double a = r.direction().length_squared();
    double half_b = dot(oc, r.direction());
    double c = oc.length_squared() - radius * radius;
    double discriminant = half_b * half_b - a * c;
    if (discriminant < 0)
      return false;
    double sqrtd = std::sqrt(discriminant);
    double root = (half_b - sqrtd) / a;
    if (root <= t_min || t_max <= root) {
      root = (half_b + sqrtd) / a;
      if (root <= t_min || t_max <= root)
        return false;
    }
    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    return true;
  }

private:
  point3 center;
  double radius;
};

#endif
