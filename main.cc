#include "color.h"
#include "ray.h"
#include "vec3.h"
#include <cmath>
#include <iostream>

double hit_sphere(const point3 &center, double radius, const ray &r) {

    // does it intersect
    vec3 oc = center - r.origin();
    auto a = dot(r.direction() , r.direction());
    auto b = -2. * dot(r.direction(),oc);
    auto c = dot(oc,oc) - radius*radius;
    auto discriminant = b*b -4*a*c;
    return(discriminant >= 0 ); // discriminant >= 0 means there is at least 1 hit. So intersection

    

}

color ray_color(const ray &r) {
    
    if (hit_sphere(point3(0,0,-1), 0.5, r)) {
        return color(1, 0, 0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}



int main() {

  // | Value                 | How it's determined                                                |
  // |-----------------------|--------------------------------------------------------------------|
  // | focal_length          | chosen: distance from camera to viewport                           |
  // | viewport_height       | chosen: indirectly sets the FOV                                    |
  // | viewport_width        | derived: viewport_height * (pixel aspect ratio), avoids distortion |
  // | pixel_delta_u/v       | derived: viewport size / pixel count                               |
  // | pixel00_loc           | derived: viewport corner + half a pixel step                       |

  // Image
  auto aspect_ratio = 16. / 9.;
  int image_width = 400;

  int image_height = int(image_width / aspect_ratio); // image_height = 25 x 9 = 225
  image_height = (image_height < 1) ? 1 : image_height;

  // Camera
  auto focal_length = 1.0;
  auto viewport_height = 2.;
  auto viewport_width = viewport_height * (double(image_width) / image_height); // 2 x (400/225) approximately 3.5
  auto camera_center = point3(0, 0, 0);

  // Calculate the vectors across the horizontal and down the vertical viewport edges.
  auto viewport_u = vec3(viewport_width, 0, 0);   // viewport_u = is a vector like 3.5i
  auto viewport_v = vec3(0, -viewport_height, 0); // viewport_v = is a vector like -2k

  // Calculate the horizontal and vertical delta vectors from pixel to pixel.
  auto pixel_delta_u = viewport_u / image_width;  // vec3 / double so 3.5/400 = 0.00875i
  auto pixel_delta_v = viewport_v / image_height; // and here 2 / 225 = -0.00888888888k

  // Calculate the location of the upper left pixel
  auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
  // after first decremant (0 ,0, -focal_length) then at the end viewport_upper_left = (-1.7,1,-1)

  auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
  // (-1.7 , 1 , -1) + (0.00437 , -0.0044 , 0) = (-1.69563 , 0.9956 , -1)

  // Render

  std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

  for (int j = 0; j < image_height; j++) {
    std::cerr << "Scanlines remaining: " << j << '\n';
    for (int i = 0; i < image_width; i++) {
      auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
      auto ray_direction = pixel_center - camera_center;
      ray r(camera_center, ray_direction);

      color pixel_color = ray_color(r);
      write_color(std::cout, pixel_color);
    }
  }

  std::cerr << "\nDone.\n";
}