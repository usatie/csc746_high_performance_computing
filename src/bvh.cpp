#include "bvh.h"
#include "aabb.h"
#include <algorithm>

bvh_node::bvh_node(const std::vector<std::shared_ptr<hittable>> &src_objects,
                   size_t start, size_t end, double time0, double time1) {
  auto objects =
      src_objects; // Create a modifiable array of the source scene objects

  int axis = random_int(0, 2);
  auto comparator = (axis == 0)   ? box_x_compare
                    : (axis == 1) ? box_y_compare
                                  : box_z_compare;

  size_t object_span = end - start;

  if (object_span == 1) {
    left = right = objects[start];
  } else if (object_span == 2) {
    if (comparator(objects[start], objects[start + 1])) {
      left = objects[start];
      right = objects[start + 1];
    } else {
      left = objects[start + 1];
      right = objects[start];
    }
  } else {
    std::sort(objects.begin() + start, objects.begin() + end, comparator);

    auto mid = start + object_span / 2;
    left = std::make_shared<bvh_node>(objects, start, mid, time0, time1);
    right = std::make_shared<bvh_node>(objects, mid, end, time0, time1);
  }

  aabb box_left, box_right;

  if (!left->bounding_box(time0, time1, box_left) ||
      !right->bounding_box(time0, time1, box_right))
    std::cerr << "No bounding box in bvh_node constructor.\n";

  box = surrounding_box(box_left, box_right);
}

bool bvh_node::hit(const ray &r, interval ray_t, hit_record &rec) const {
  if (!box.hit(r, ray_t))
    return false;

  bool hit_left = left->hit(r, ray_t, rec);
  bool hit_right =
      right->hit(r, interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);

  return hit_left || hit_right;
}